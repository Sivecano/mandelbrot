#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb/stb_image_write.h"
#include <iostream>
#include <sstream>
#include <iomanip>
#include <string>
#include <cstdint>
#include <limits>
#include "glad/glad.h"
#include "GLFW/glfw3.h"
#include "helpers.h"
#include "glm/glm.hpp"
#include <glm/gtc/type_ptr.hpp>


const int width= 1920;
const int height = 1080;

double scale = 1;
const int max_i = 500;

double x_off = 0;
double y_off = 0;

float ds = .01;
float v = 0.001;
float a = 0.01;

int anim_steps;
double dx;
double dy;
double dscale;
bool animating;
std::string from_to;

void set_anim_goal()
{
    std::stringstream buff;
    double xg, yg, sg;
    buff.precision(std::numeric_limits< double >::digits10);
    buff <<"from:\nx: ";
    buff << x_off << "\ny: " << y_off << "\nscale: " << scale;
    system("mkdir anim");

    std::cout << "-----------------------------\ncurrent:\n\tx:\t" << x_off << "\n\ty:\t" << y_off << "\n\tscale:\t" << scale << "\n\n";

    std::cout << "x_goal: ";
    std::cin >> xg;
    std::cout << "y_goal: ";
    std::cin >> yg;
    std::cout << "size_goal: ";
    std::cin >> sg;
    std::cout << "number of steps: ";
    std::cin >> anim_steps;

    buff << "\n\nto:\nx: " << xg << "\ny: " << yg << "\nsize: " << sg;
    from_to = buff.str();

    dscale = pow(sg / scale, 1./anim_steps);
    if (dscale == 1)
    {
        dx = (xg -x_off) / anim_steps;
        dy = (yg -y_off) / anim_steps;
    }
    else
    {
        dx = scale *(xg -x_off) * (1-1/dscale) / (1-pow(1 / dscale, anim_steps));
    dy = scale *(yg -y_off) * (1-1/dscale) / (1-pow(1 / dscale, anim_steps));
    }


    animating = true;

}

void anim_cleanup()
{
    system(("ffmpeg -r 24 -f image2 -s 1920x1080 -i anim/anim%d.bmp -vcodec libx265 -metadata author=\"Fadri Lardon\" -metadata comment=\"" +  from_to + "\"  mandelbrot.mov").c_str());
    system("rm anim/anim*");
}

void screenshot(const char* filename)
{
    uint8_t *pixels = new uint8_t[3*width*height];
    glReadPixels(0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, pixels);
    stbi_write_bmp(filename, width, height, 3, pixels);
    if (pixels) delete[] pixels;
}


void animate()
{
    static int anim_counter;
    if (anim_counter < anim_steps)
    {
        x_off += dx / scale;
        y_off += dy / scale;
        scale *= dscale;
        anim_counter++;
        std::string file = "anim/anim";
        file += std::to_string(anim_counter);
        file += ".bmp";
        std::cout << "animation " << anim_counter << "/" << anim_steps << "\t" << file << "\n";

        screenshot(file.c_str());
    }
    else
    {
        if (animating)
            anim_cleanup();

        anim_counter = 0;
        anim_steps = 0;
        animating=false;
    }

}

void zoom(GLFWwindow* window, double xoffset, double yoffset)
{
    scale += scale *yoffset * 0.1;
    if (scale <= 0.5) scale = 0.5;
}

void scroll(GLFWwindow* window, double xpos, double ypos)
{
    static double lx, ly;
    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
    {
        x_off += 3.5 * (lx - xpos) /(width * scale);
        y_off += 2 *(ypos - ly) /(height * scale);
    }

    //std::cout << "pos: " << xpos << ", " << ypos << std::endl;
    lx = xpos;
    ly = ypos;
}

void close(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if ((key == GLFW_KEY_Q || key == GLFW_KEY_ESCAPE) && action ==  GLFW_PRESS)
        glfwSetWindowShouldClose(window, GLFW_TRUE);
    if ((key == GLFW_KEY_S) && action == GLFW_PRESS)
        screenshot("screen.bmp");
    if ((key == GLFW_KEY_A) && action == GLFW_PRESS)
        set_anim_goal();
}

float QqhToRgb(float q1, float q2, float hue)
{
	if (hue > 360) hue -= 360;
	else if (hue < 0) hue += 360;

	if (hue < 60) return q1 + (q2 - q1) * hue / 60;
	if (hue < 180) return q2;
	if (hue < 240) return q1 + (q2 - q1) * (240 - hue) / 60;
	return q1;
}

glm::vec3 HlsToRgb(float h, float l, float s)
{
	float p2;
	if (l <= 0.5) p2 = l * (1 + s);
	else p2 = l + s - l * s;

	float p1 = 2 * l - p2;
	float float_r, float_g, float_b;
	if (s == 0)
	{
		float_r = l;
		float_g = l;
		float_b = l;
	}
	else
	{
		float_r = QqhToRgb(p1, p2, h + 120);
		float_g = QqhToRgb(p1, p2, h);
		float_b = QqhToRgb(p1, p2, h - 120);
	}

	// Convert RGB to the 0 to 255 range.

	return glm::vec3(float_r, float_g, float_b);
}

void colourinit(unsigned int shader)
{
    glm::vec3 map[max_i + 1];
    int maploc = glGetUniformLocation(shader, "colourmap");

    for (int i = 0; i < max_i; i++)
    {
        map[i] = HlsToRgb(270 * i / max_i, .5, .5);
    }

    map[max_i] = glm::vec3(0., 0.,0.);


    glUniform3fv(maploc, max_i, glm::value_ptr(*map));
}

int main()
{
    std::cout.precision(std::numeric_limits< double >::digits10);
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    stbi_flip_vertically_on_write(1);


    GLFWwindow* win = glfwCreateWindow(width, height, "the mandelbrot set", glfwGetPrimaryMonitor(), NULL);
    if (!win)
    {
        printf("window creation failed!\n");
        return 0;
    }
    glfwMakeContextCurrent(win);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout <<    "glad died something has gone wrong check dependencies!";
        throw "glad died something has gone wrong check dependencies";
    }
    //glfwSetInputMode(win, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glViewport(0, 0, width, height);

    glfwSetCursorPosCallback(win, scroll);
    glfwSetScrollCallback(win, zoom);
    glfwSetKeyCallback(win, close);

    unsigned int shader = shaders::shader_program("shaders/mandelbrot.vert", "shaders/mandelbrot.frag");

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    float points[]{
        1, 1, 0.,
        1, -1, 0.,
        -1, -1, 0.,
        -1, 1, 0.
    };

    unsigned int VAO, VBO, EBO;

      unsigned int indices[] = {  // note that we start from 0!
        0, 1, 3,   // first triangle
        1, 2, 3    // second triangle
        };  

        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);

        glBindVertexArray(VAO);

        glBindBuffer(GL_ARRAY_BUFFER, VBO);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_DYNAMIC_DRAW);

        // position attribute
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
        glEnableVertexAttribArray(0);
        glBufferData(GL_ARRAY_BUFFER, sizeof(points), points, GL_DYNAMIC_DRAW);

        double frametime;
        double dt;
    
    int scaleloc = glGetUniformLocation(shader, "scale");
    int offsetloc = glGetUniformLocation(shader, "offset");
    glUseProgram(shader); 
    colourinit(shader);
    

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);


    bool dir = true;
    

    while (!glfwWindowShouldClose(win))
    {
        //frametime = glfwGetTime();

        glClearColor(.2f, .2f, .8f, 1.0f); // rgba background
        glClear(GL_COLOR_BUFFER_BIT);

        //v *= 1 + a;
        //ds += v * (dir * 2 - 1);

        //scale += ds * (dir * 2 - 1);

        if (scale >= 1000 || scale <= 1) dir = !dir;

        glUniform1d(scaleloc, scale);
        glUniform2d(offsetloc, x_off, y_off);

        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0); 
        
        glfwSwapBuffers(win);
        glfwPollEvents();
        //dt = glfwGetTime() - frametime;
        //std::cout << 1 / dt << "\n";
        animate();
        std::cout << "x: " << x_off << "\ty: " << y_off << "\tscale: " << scale << std::endl;
        //std::cout << "render\n";
    }


    return 0;
    //std::cout << "Hello World";
}
