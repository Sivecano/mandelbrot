//
// Created by kaloka on 14/11/2020.
//

#ifndef OGLHANGMAN_HELPERS_H
#define OGLHANGMAN_HELPERS_H
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <string>
#include <vector>
#include <typeinfo>
#include "calibri.c"
#include <eventSystem.h>
#include <ctime>
#include "functional"
#include <fstream>

namespace helpers {
    void framebufferSizeCallback(GLFWwindow *glfwWindow, int width, int height);
}

using events::event;
/*
namespace input {
    struct PerformableInputActionsBase {
        PerformableInputActionsBase();
        void bind();
        void unbind();
        virtual void keypressCallback(GLFWwindow *window, int key, int scancode, int action, int mods);
        virtual void characterCallback(GLFWwindow *window, unsigned int codepoint);
        virtual void mouseButtonCallback(GLFWwindow *window, int key, int action, int mods);
        virtual void mouseMoveCallback(GLFWwindow *window, double posX, double posY);
    };

    struct inputHandler {
    private:
        PerformableInputActionsBase * inputActionHolder;
    public:
        [[nodiscard]] PerformableInputActionsBase *getInputActionHolder() const;
        void setInputActionHolder(PerformableInputActionsBase *inputActionHolder);
    public:
        static bool glfwBound;
        inputHandler();
        ~inputHandler();

    public:
        static void keypressEventCaller(GLFWwindow *window, int key, int scancode, int action, int mods);
        static void characterEventCaller(GLFWwindow *window, unsigned int codepoint);
        static void mouseButtonEventCaller(GLFWwindow *window, int key, int action, int mods);
        static void mouseMoveEventCaller(GLFWwindow *window, double posX, double posY);

    private:
        static event<GLFWwindow *, int, int, int, int> keypressEvent;
        static event<GLFWwindow *, unsigned int> characterEvent;
        static event<GLFWwindow *, double, double> mouseMovementEvent;
        static event<GLFWwindow *, int, int, int> mouseButtonEvent;
    };
    void setupGlfwInputCallbacks(GLFWwindow *window, input::inputHandler *handler);
}
*/
namespace shaders
{
    unsigned int load_shader(const char* filename, int shadertype);
    unsigned int shader_program(const char *vertShaderLoc, const char *fragShaderLoc);
}

namespace sprites
{
    class Sprite
    {
    public:
        float x;
        float y;
        float w;
        float h;

        virtual void draw(unsigned int shader) = 0;
    };

    class Text: public Sprite
    {
    public:

        static unsigned int textids[CALIBRI_FRAME_COUNT];
        static bool initflag;

        static unsigned int VBO, VAO, EBO;

        static void TextInit();
        
        Text();

        Text(std::string text, float posx = 0, float posy = 0, float height = 0.05);
        void setText(std::string newtext);
        std::string getText();

        void draw(unsigned int shader) override;

    private:
        std::string text = "";

    };

};
#endif //OGLHANGMAN_HELPERS_H
