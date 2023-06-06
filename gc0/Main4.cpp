#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include <iostream>
#include <vector>
#include <array>

#include "Shader.h"


struct Camera {
    glm::vec3 pos = {};
	glm::vec3 cameraFront = glm::vec3{0.0f, 0.0f, -1.0f};
    float pitch = 0.0f;
    float yaw = -90.0f;

    void moveForward(float delta) {
        pos += cameraFront * delta;
    }
    void strafe(float delta) {
        auto cameraRight = glm::normalize(glm::cross(cameraFront, glm::vec3{0.0f, 1.0f, 0.0f}));
        pos += cameraRight * delta;
    }

    void rotatePitch(float deltaPitch) {
        pitch += deltaPitch;
        if (pitch > 89.0f)
            pitch = 89.0f;
        if (pitch < -89.0f)
            pitch = -89.0f;
    }

    void rotateYaw(float deltaYaw) {
        yaw += deltaYaw;
    }

	glm::mat4 getView() {
        cameraFront.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
        cameraFront.y = sin(glm::radians(pitch));
        cameraFront.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
        cameraFront = glm::normalize(cameraFront);

        return glm::lookAt(
            pos,
            pos + cameraFront,
            { 0.0f, 1.0f, 0.0f }
        );
    }
} camera;

int windowWidth = 1600;
int windowHeight = 900;

float previousX = static_cast<float>(windowWidth) / 2.0f;
float previousY = static_cast<float>(windowHeight) / 2.0f;


struct Mesh {

    Mesh() {
        glGenVertexArrays(1, &vao);
        glGenBuffers(1, &vbo);
        glGenBuffers(1, &ebo);
    }
    ~Mesh() {
        glDeleteVertexArrays(1, &vao);
        glDeleteBuffers(1, &vbo);
        glDeleteBuffers(1, &ebo);
    }

    void passVertices(const std::vector<float>& vertices, const std::vector<uint32_t>& indices) {
        indicesCount = indices.size();
        glBindVertexArray(vao);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(vertices[0]), vertices.data(), GL_STATIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), nullptr);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices[0]) * indices.size(), indices.data(), GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glEnableVertexAttribArray(1);

    }

    void draw(Shader& shader) {
        glBindVertexArray(vao);
        shader.use();
        glDrawElements(GL_TRIANGLES, indicesCount, GL_UNSIGNED_INT, nullptr);
    }
private:
    GLuint vao;
    GLuint vbo;
    GLuint ebo;
    GLuint indicesCount = 0;
};

struct Texture {
    Texture(const char* path) {
        stbi_set_flip_vertically_on_load(true);
        GLubyte* data = stbi_load(path, &width, &height, &nrChannels, 0);
        if (!data) {
            std::printf("Could not load!\n");
            return;
        }

        glGenTextures(1, &tid);
        glBindTexture(GL_TEXTURE_2D, tid);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

        stbi_image_free(data);
    }
    ~Texture() {
		glDeleteTextures(1, &tid);
    }

    void use() {
        glBindTexture(GL_TEXTURE_2D, tid);
    }
private:
    GLuint tid;
    int width, height, nrChannels;
};

static bool firstTime = true;
static bool mouseCenterize = false;

void mouseCallback(GLFWwindow* window, double xpos, double ypos) {
    if (firstTime) {
        previousX = xpos;
        previousY = ypos;
        firstTime = false;
    }
    float xdifference = xpos - previousX;
    float ydifference = previousY - ypos;

    previousX = xpos;
    previousY = ypos;

    const float sensitivity = 0.175f;
    xdifference *= sensitivity;
    ydifference *= sensitivity;

    camera.rotateYaw(xdifference);
    camera.rotatePitch(ydifference);

}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
}

void processInputKeyboard(GLFWwindow* window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    const float cameraSpeed = 0.03f;
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        camera.moveForward(1.f * cameraSpeed);
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        camera.moveForward(-1.f * cameraSpeed);
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        camera.strafe(-1.f * cameraSpeed);
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        camera.strafe(1.f * cameraSpeed);
    }
}


int main()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Tworzenie okna
    GLFWwindow* window = glfwCreateWindow(windowWidth, windowHeight, "Kamera", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    glfwSetScrollCallback(window, scroll_callback);
    glfwSetCursorPosCallback(window, mouseCallback);

    glfwGetWindowSize(window, &windowWidth, &windowHeight);

	previousX = static_cast<float>(windowWidth) / 2.0f;
	previousY = static_cast<float>(windowHeight) / 2.0f;


    // inicjalizacja GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }


    glViewport(0, 0, (GLuint)windowWidth, (GLuint)windowHeight);
    glEnable(GL_DEPTH_TEST);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    Mesh triangle;
    triangle.passVertices({ 
			// front
			-1.0f, -1.0f,  1.0f, 1.0f, 0.0f,
			 1.0f, -1.0f,  1.0f, 0.0f, 1.0f,
			 1.0f,  1.0f,  1.0f, 1.0f, 1.0f,
			-1.0f,  1.0f,  1.0f, 1.0f, 0.0f,
			// back
			-1.0f, -1.0f, -1.0f, 0.0f, 1.0f,
			 1.0f, -1.0f, -1.0f, 1.0f, 0.0f,
			 1.0f,  1.0f, -1.0f, 0.0f, 1.0f,
			-1.0f,  1.0f, -1.0f, 1.0f, 0.0f,
		}, {
			// front
			0, 1, 2,
			2, 3, 0,
			// right
			1, 5, 6,
			6, 2, 1,
			// back
			7, 6, 5,
			5, 4, 7,
			// left
			4, 0, 3,
			3, 7, 4,
			// bottom
			4, 5, 1,
			1, 0, 4,
			// top
			3, 2, 6,
			6, 7, 3
		});

    Shader shader("basic4.vert", "basic4.frag");

    while (!glfwWindowShouldClose(window))
    {
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        double time = glfwGetTime();

        auto model = glm::mat4(1.0f);
        auto view = camera.getView();
        auto projection = glm::perspective(glm::radians(45.0f), static_cast<float>(windowWidth) / static_cast<float>(windowHeight), 0.1f, 100.0f);

        shader.use();
        auto modelUniform = glGetUniformLocation(shader(), "Model");
        auto viewUniform = glGetUniformLocation(shader(), "View");
        auto projUniform = glGetUniformLocation(shader(), "Projection");

        model = glm::translate(model, glm::vec3(0.0f, 0.0f, -5.0f));
        glUniformMatrix4fv(modelUniform, 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(viewUniform, 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(projUniform, 1, GL_FALSE, glm::value_ptr(projection));
		triangle.draw(shader);

        GLenum err;
        while ((err = glGetError()) != GL_NO_ERROR)
        {
            printf("err: %d\n", err);
        }

        processInputKeyboard(window);
        glfwSwapBuffers(window);
        glfwPollEvents();
    }


    glfwTerminate();
    return 0;
}
