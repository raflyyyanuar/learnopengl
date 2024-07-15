#include "shader.h"
#include <GLFW/glfw3.h>

#include <iostream>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 800;

std::string getExecutablePath(std::string exePath) {
    std::string::size_type pos = exePath.find("opengles-windows");
    return exePath.substr(0, pos) + "opengles-windows/src/shaders/";
}

int main(int argc, char** argv) {
    const std::string SHADER_PATH = getExecutablePath(argv[0]);

    const std::string VERTEX_SHADER_PATH = SHADER_PATH + "vShader.glsl";
    const std::string BLACK_FRAGMENT_SHADER_PATH = SHADER_PATH + "fShaderBlack.glsl";
    const std::string WHITE_FRAGMENT_SHADER_PATH = SHADER_PATH + "fShaderWhite.glsl";

    // glfw: initialize and configure
    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // glfw window creation
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "OpenGLES on Windows", NULL, NULL);
    if (window == NULL) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // glad: load all OpenGL function pointers
    if (!gladLoadGLES2Loader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    Shader leftTriangleShader(VERTEX_SHADER_PATH, BLACK_FRAGMENT_SHADER_PATH);
    Shader rightTriangleShader(VERTEX_SHADER_PATH, BLACK_FRAGMENT_SHADER_PATH);
    Shader centerSquareShader(VERTEX_SHADER_PATH, WHITE_FRAGMENT_SHADER_PATH);

    // set up vertex data (and buffer(s)) and configure vertex attributes
    // ------------------------------------------------------------------
    float leftTriangle[] = {
        -0.5f,  -0.5f,  0.0f,
        -0.5f,  0.5f,  0.0f,
         0.5f,  -.5f,  0.0f,
    };

    float rightTriangle[] = {
        0.5f,  0.5f,  0.0f,
        -0.5f,  0.5f,  0.0f,
         0.5f,  -.5f,  0.0f,
    };

    float centerSquare[] = {
        -0.25f,  0.25f,  0.0f,
        -0.25f,  -0.25f,  0.0f,
         0.25f,  -.25f,  0.0f,
         0.25f,  .25f,  0.0f,
    };

    int indices[] = {
        0, 1, 2,
        2, 3, 0,
    };

    unsigned int VAOs[3], VBOs[3], EBO;
    glGenVertexArrays(3, VAOs);
    glGenBuffers(3, VBOs);
    glGenBuffers(1, &EBO);

    // LEFT TRIANGLE BUFFER //
    glBindVertexArray(VAOs[0]);
    glBindBuffer(GL_ARRAY_BUFFER, VBOs[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(leftTriangle), leftTriangle, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
    glEnableVertexAttribArray(0);

    // RIGHT TRIANGLE BUFFER //
    glBindVertexArray(VAOs[1]);
    glBindBuffer(GL_ARRAY_BUFFER, VBOs[1]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(rightTriangle), rightTriangle, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
    glEnableVertexAttribArray(0);

    // CENTER SQUARE BUFFER //
    glBindVertexArray(VAOs[2]);
    glBindBuffer(GL_ARRAY_BUFFER, VBOs[2]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(centerSquare), centerSquare, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
    glEnableVertexAttribArray(0);

    // uncomment this call to draw in wireframe polygons.
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    // render loop
    // -----------
    while (!glfwWindowShouldClose(window))
    {
        // input
        // -----
        processInput(window);

        // render
        // ------
        glClearColor(1.f, 1.f, 1.f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        float swayOffset = 0.04f;
        float sinValue = (sin(glfwGetTime() * 3) + 2) / 2;

        leftTriangleShader.use();
        leftTriangleShader.setFloat("direction", -1.f);
        leftTriangleShader.setFloat("swayOffset", sinValue * swayOffset);
        glBindVertexArray(VAOs[0]);
        glDrawArrays(GL_TRIANGLES, 0, 3);

        rightTriangleShader.use();
        rightTriangleShader.setFloat("direction", 1.f);
        rightTriangleShader.setFloat("swayOffset", sinValue * swayOffset);
        glBindVertexArray(VAOs[1]);
        glDrawArrays(GL_TRIANGLES, 0, 3);

        centerSquareShader.use();
        glBindVertexArray(VAOs[2]);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------
    glDeleteVertexArrays(3, VAOs);
    glDeleteBuffers(3, VBOs);
    glDeleteBuffers(1, &EBO);

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}