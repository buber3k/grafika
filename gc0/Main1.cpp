#include <glad/glad.h>  // musi by� do��czony jako pierwszy
#include <GLFW/glfw3.h>

#include <iostream>
#include <vector>
#include <array>

#include "Shader.h"

typedef std::pair<std::vector<float>, std::vector<uint32_t>> FigureData;



FigureData getFigure(int n, float radius = 0.5f) {
    FigureData result;
    if (n < 3) {
        return {};
    }
    std::vector<float> vertices = {
       0.0f, 0.0f, 0.0f
    };
    std::vector<uint32_t> indices = {
    };
    for (int i = 0; i <= n; i++) {
        double angle = i / (double)n  * 3.14159265 * 2.0;
        vertices.push_back(std::cos(angle) * radius);
        vertices.push_back(std::sin(angle) * radius);
        vertices.push_back(0.0f);

        indices.push_back(0);
        indices.push_back(i+1);
        indices.push_back(i+2);
    }

    result.first = vertices;
    result.second = indices;

    return result;
}

int n = 3;
FigureData figure = getFigure(3);

GLuint vao;
GLuint vbo;
GLuint ebo;

int idColor = 0;

std::array<std::array < float, 3>, 3> colors = { {
    {0.3f, 0.6f, 0.1f},
    {0.3f, 0.6f, 0.7f},
    {0.9f, 0.2f, 0.2f},
} };

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    printf("%f %f\n", xoffset, yoffset);
    n += static_cast<int>(yoffset);
    if (n < 3) {
        n = 3;
    }
    figure = getFigure(n);
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, figure.first.size() * sizeof(figure.first[0]), figure.first.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(figure.second[0]) * figure.second.size(), figure.second.data(), GL_STATIC_DRAW);
}

void processInputKeyboard(GLFWwindow* window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);


    if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS) {
        idColor = 0;
    }
    if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS) {
        idColor = 1;
    }
    if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS) {
        idColor = 2;
    }
}



int main()
{
    // inicjalizacja GLFW
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Tworzenie okna
    const unsigned int window_width = 900;
    const unsigned int window_height = 900;
    GLFWwindow* window = glfwCreateWindow(window_width, window_height, "grafika komputerowa", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    glfwSetScrollCallback(window, scroll_callback);


    // inicjalizacja GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }


    glViewport(0, 0, (GLuint)window_width, (GLuint)window_height);


    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glGenBuffers(1, &ebo);

    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, figure.first.size() * sizeof(figure.first[0]), figure.first.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);


    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(figure.second[0]) * figure.second.size(), figure.second.data(), GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);

    Shader shader("basic.vert", "basic.frag");

    // p�tla zdarze�
    while (!glfwWindowShouldClose(window))
    {
        // renderowanie
        glClearColor(0.298f, 0.141f, 0.141f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);


        shader.use();
        auto colorUniform = glGetUniformLocation(shader(), "figureColor");
        glUniform3fv(colorUniform, 1, colors[idColor].data());
        glBindVertexArray(vao);
        glDrawElements(GL_TRIANGLES, figure.second.size(), GL_UNSIGNED_INT, nullptr);

        processInputKeyboard(window);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }


    glfwTerminate();
    return 0;
}
