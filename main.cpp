#include <iostream>
#include "glad/glad.h"
#include "GLFW/glfw3.h"
#include "helpers.h"
#include "glm/glm.hpp"
#include <glm/gtc/type_ptr.hpp>

const int width = 1920;
const int height = 1080;

double scale = 1;
const int max_i = 500;

double x_off = 0;
double y_off = 0;

float ds = .01;
float v = 0.001;
float a = 0.01;

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
    
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);


    GLFWwindow* win = glfwCreateWindow(1920, 1080, "the mandelbrot set", glfwGetPrimaryMonitor(), NULL);
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
        std::cout << "x: " << x_off << "scale: " << scale << std::endl;
        //std::cout << "render\n";
    }


    return 0;
    //std::cout << "Hello World";
}
