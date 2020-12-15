//
// Created by kaloka on 16/11/2020.
//
#include <typeinfo>

#ifndef EVENTS_DEFINED_T
#include "eventSystem.h"
#else
// OH GOD NON OF THIS IS COMMENTED // this could also use some improvements
namespace events {
    template <typename ...T>
    class functionWrapperBase {
    public:
        virtual void call(T ...args) = 0;
        virtual bool operator == (functionWrapperBase<T...>* other) = 0;
        virtual ~functionWrapperBase() = default;
    };

    template<>
    class functionWrapperBase<void> {
    public:
        virtual void call() = 0;
        virtual bool operator == (functionWrapperBase<void>* other) = 0;
        virtual ~functionWrapperBase() = default;
    };

    template <typename ...T>
    class functionWrapperFunc : public functionWrapperBase<T...> {
    public:
        explicit functionWrapperFunc(void (*func)(T...)) :
                funcCallback(func)
        {}

        void call(T ...args) override {
            funcCallback(args...);
        }

        bool operator == (functionWrapperBase<T...>* other) override {
            if (typeid(*this) != typeid(*other)) {
                return false;
            } else if (funcCallback != (dynamic_cast<functionWrapperFunc<T...> *>(other)->funcCallback)) {
                return false;
            } else { return true; }
        }

        ~functionWrapperFunc() override = default;

    private:
        void (*funcCallback)(T...);
    };

    template<>
    class functionWrapperFunc<void> : public functionWrapperBase<void> {
    public:
        explicit functionWrapperFunc(void (*func)()) :
                funcCallback(func)
        {}

        void call() override {
            funcCallback();
        }

        bool operator == (functionWrapperBase<void>* other) override {
            if (typeid(*this) != typeid(*other)) {
                return false;
            } else if (funcCallback != (dynamic_cast<functionWrapperFunc<void> *>(other)->funcCallback)) {
                return false;
            } else { return true; }
        }

        ~functionWrapperFunc() override = default;

    private:
        void (*funcCallback)();
    };

    template <typename U, typename ...T>
    class functionWrapperMember : public functionWrapperBase<T...> {
    public:
        functionWrapperMember(void (U::*func)(T...), U *inst) :
                inst(inst),
                memberCallback(func)
        {}

        void call(T... args) override {
            (inst->*memberCallback)(args...);
        }

        bool operator == (functionWrapperBase<T...>* other) override {
            if (typeid(*this) != typeid(*other)) {
                return false;
            } else {
                auto castedOther = dynamic_cast<functionWrapperMember<U, T...> *>(other);
                if (inst != castedOther->inst || memberCallback != castedOther->memberCallback) {
                    return false;
                }
                return true;
            }
        }

        ~functionWrapperMember() override = default;

    private:
        U *inst;
        void (U::*memberCallback)(T...);
    };

    template<typename U>
    class functionWrapperMember<U, void> : functionWrapperBase<void> {
    public:
        functionWrapperMember(void (U::*func)(), U *inst) :
                inst(inst),
                memberCallback(func)
        {}

        void call() override {
            (inst->*memberCallback)();
        }

        bool operator == (functionWrapperBase<void>* other) override {
            if (typeid(*this) != typeid(*other)) {
                return false;
            } else {
                auto castedOther = dynamic_cast<functionWrapperMember<U, void> *>(other);
                if (inst != castedOther->inst || memberCallback != castedOther->memberCallback) {
                    return false;
                }
                return true;
            }
        }

    private:
        U *inst;
        void (U::*memberCallback)();

        ~functionWrapperMember() override = default;
    };

    template <typename ...T>
    functionWrapperBase<T...> *handler(void (*func)(T...)) {
        return new functionWrapperFunc<T...>(func);
    }
    template <typename U, typename ...T>
    functionWrapperBase<T...> *handler(void (U::*func)(T...), U *inst) {
        return new functionWrapperMember<U, T...>(func, inst);
    }
    template<typename ...T>
    functionWrapperBase<void> *handler(void (*func)()) {
        return new functionWrapperFunc<void>(func);
    }
    template <typename U>
    functionWrapperBase<void> *handler(void (U::*func)(), U *inst) {
        return new functionWrapperMember<U, void>(func, inst);
    }

    template <typename ...T>
    class event {
    public:
        event() : handlers() {};

        void add(void (*funcPtr)(T...)) {
            *this += handler(funcPtr);
        }
        template<typename U>
        void add(void (U::*funcPtr)(T...), U *inst) {
            *this += handler(funcPtr, inst);
        }
        void sub(void (*funcPtr)(T...)) {
            *this -= handler(funcPtr);
        }
        template<typename U>
        void sub(void (U::*funcPtr)(T...), U *inst) {
            *this -= handler(funcPtr, inst);
        }

        event & operator += (functionWrapperBase<T...> *handler) {
            handlers.push_back(handler);
            return *this;
        }

        event & operator += (void (*funcPtr)(T...)) {
            *this += handler(funcPtr);
            return *this;
        }

        event & operator -= (functionWrapperBase<T...> *handler) {
            auto i = handlers.begin();
            auto end = handlers.end(); // cuz we modify
            for (; i != end; i++) {
                if (**i == handler) {
                    delete *i;
                    handlers.erase(i);
                    delete handler;
                    break;
                }
            }
            return *this;
        }

        event & operator -= (void (*funcPtr)(T...)) {
            *this -= handler(funcPtr);
            return *this;
        }

        void operator () (T ...args) {
            for (auto handler : handlers) {
                handler->call(args...);
            }
        }
        ~event(){
            for (auto handler : handlers) {
                delete handler;
            }
        }

    private:
        std::vector<functionWrapperBase<T...>*> handlers;

    };
    template <>
    class event<void> {
    public:
        event() : handlers() {};

        void add(void (*funcPtr)()) {
            *this += handler(funcPtr);
        }
        template<typename U>
        void add(void (U::*funcPtr)(), U *inst) {
            *this += handler(funcPtr, inst);
        }
        void sub(void (*funcPtr)()) {
            *this -= handler(funcPtr);
        }
        template<typename U>
        void sub(void (U::*funcPtr)(), U *inst) {
            *this -= handler(funcPtr, inst);
        }

        event & operator += (functionWrapperBase<void> *handler) {
            handlers.push_back(handler);
            return *this;
        }

        event & operator += (void (*funcPtr)()) {
            *this += handler(funcPtr);
            return *this;
        }

        event & operator -= (functionWrapperBase<void> *handler) {
            auto i = handlers.begin();
            auto end = handlers.end(); // cuz we modify
            for (; i != end; i++) {
                if (**i == handler) {
                    delete *i;
                    handlers.erase(i);
                    delete handler;
                    break;
                }
            }
            return *this;
        }

        event & operator -= (void (*funcPtr)()) {
            *this -= handler(funcPtr);
            return *this;
        }

        void operator () () {
            for (auto handler : handlers) {
                handler->call();
            }
        }
        ~event(){
            for (auto handler : handlers) {
                delete handler;
            }
        }

    private:
        std::vector<functionWrapperBase<void>*> handlers;
    };
}
#endif