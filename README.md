# Mandelbrot set
A simple implementation of the mandelbrot set using opengl.
`float32` contains an imlementation using `float32` allowing for less zoom while `double` contains an implementation using `float64` allowing for more zoom at the cost of performance.`main` is a clone of `double` where you can record videos of zoomouts and zoomins. Press `A` to open a dialog in your terminal window and simply input the coordinates and zoom factor that you'd like to go to. the video can then be found in the run directory. MAKE SURE THAT YOU HAVE PLENTY OF STORAGE LEFT. THIS PROGRAM WILL CREATE AN IMAGE FILE FOR EACH FRAME OF VIDEO AT 24fps. 
  * for mac: you may face difficulties running the `float64` version so it's best to stick to the `float32` version.
  * on linux: you may face weird bugs resulting from the fact that the application enters fullscreen mode. to fix this just replace `glfwGetPrimaryMonitor()` on line `50` of `main.cpp` with `NULL`.
  * on windows: make sure to run it with your most powerful graphics card, otherwise you will only get still images or no images at all.
  * for advanced users: you can change the fidelity and colouring of the image in `shaders/mandelbrot.frag`: 
      * on line 56 you can change `max_i` to increase the nuber of iteration and thus the accuracy of the approximation which is the final image (make note that the value `max_i` is inversely proportional to the framerate).
      * on line 68 you can change the inputs of the HLSToRGB function to change the colouring.
      * make sure to always reconfigure the cmake project to apply your changes to the shader files
# Installing
  1. make sure you have cmake as well as a c++ compiler installed
  2. make sure you have ffmpeg installed
  3. clone your prefered branch with `git clone`
  4. get the dependencies with `git submodule init` and `git submodule update`
  5. build using cmake
  6. enjoy
  
  tl;dr:
 ```
 git clone https://github.com/Sivecano/mandelbrot.git
 cd mandelbrot
 git submodule init
 git submodule update
 mkdir build 
 cmake -B build 
 cd build 
 make # or ninja or smth. I assume you can figure it out
 ./sivegl
 ```
