#include <glew.h>
#include <glfw3.h>
#include <vector>
#include <cmath>

#include "src/vendor/glm/glm.hpp"
#include "src/VBO.h"
#include "src/vendor/glm/gtc/type_ptr.hpp"
#include "src/Shader.h"
#include "src/GLWindow.h"

// Window dimensions
const GLuint WIDTH = 800, HEIGHT = 600;

glm::vec2 mousePos;
bool mouseDown = false;

float triOffset = 0.0f;
float triIncrement = 0.05f;

static void mousePosCallback(GLFWwindow* window, double x, double y) {
    mousePos = {x, y};
}

static void mousePressCallback(GLFWwindow* window, int button, int action, int mods) {
    if (button == GLFW_MOUSE_BUTTON_LEFT) return;

    if (action == GLFW_PRESS) {
        mouseDown = true;
    } else if (action == GLFW_RELEASE) {
        mouseDown = false;
    }
}

static glm::vec2 MouseToScreen(glm::vec2 mouseVec) {
    return {mousePos.x / WIDTH * 2 - 1, (mousePos.y / HEIGHT - 0.5f) * -2};
}

static glm::vec2 MouseToScreenNorm01(glm::vec2 mouseVec) {
    return {mousePos.x / WIDTH, mousePos.y / HEIGHT};
}

// The MAIN function, from here we start the application and run the game loop
int main() {

    GLWindow window(WIDTH, HEIGHT);

    Shader shader;
    Uniform uniformModel = shader.GenUniform("model");
    Uniform uniformProjection = shader.GenUniform("projection");

    unsigned int indices[] = {
            0, 3, 1,
            1, 3, 2,
            2, 3, 0,
            0, 1, 2
    };

    // Set up vertex data (and buffer(s)) and attribute pointers
    std::vector<GLfloat> vec {
            -1.0f, -1.0f, 0.0f,
            0.0f, -1.0f, 1.0f,
            1.0f, -1.0f, 0.0f,
            0.0f, 1.0f, 0.0f,
    };

    VBO vbo;
    GLuint VBO, VAO, IBO;

    glGenVertexArrays(1, &VAO);
    VBO = vbo.ID;
    // Bind the Vertex Array Object first, then bind and set vertex buffer(s) and attribute pointer(s).
    glBindVertexArray(VAO);

    glGenBuffers(1, &IBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);

    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_DYNAMIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vec.size() * sizeof(GLfloat), &vec[0], GL_DYNAMIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof( GLfloat ), ( GLvoid * ) 0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0); // Note that this is allowed, the call to glVertexAttribPointer registered VBO as the currently bound vertex buffer object so afterwards we can safely unbind

    glBindVertexArray(0); // Unbind VAO (it's always a good thing to unbind any buffer/array to prevent strange bugs)
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0); // must unbind after VAO


    //glfwSetCursorPosCallback(window, mousePosCallback);
    //glfwSetMouseButtonCallback(window, mousePressCallback);

    glm::mat4 projection =glm::perspective(45.0f, (GLfloat)window.GetBufferWidth()/(GLfloat)window.GetBufferHeight(), 0.1f, 100.0f);

    // Game loop
    while (!window.ShouldClose())
    {
        triOffset += triIncrement;

        auto onScreen = MouseToScreen(mousePos);

        // Check if any events have been activiated (key pressed, mouse moved etc.) and call corresponding response functions
        glfwPollEvents();

        // Render
        // Clear the colorbuffer
        auto normMouse = MouseToScreenNorm01(mousePos);
        glClearColor(normMouse.x, 0.0f, normMouse.y, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Draw our first triangle
        shader.Enable();
        glm::mat4 model(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, 0.0f, -2.5f));
        model = glm::rotate(model, triOffset, glm::vec3(0.0f, 1.0f, 0.0f));
        model = glm::rotate(model, triOffset, glm::vec3(1.0f, 1.0f, 1.0f));
        model = glm::scale(model, glm::vec3(0.4f, 0.4f, 1.0f));
        uniformModel.SetMat4(model);
        uniformProjection.SetMat4(projection);
        glBindVertexArray(VAO);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
        glDrawElements(GL_TRIANGLES, 12, GL_UNSIGNED_INT, 0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);


        vbo.BufferData(vec);

        glBindVertexArray(0);

        // Swap the screen buffers
        window.SwapBuffers();
    }

    // Properly de-allocate all resources once they've outlived their purpose
    glDeleteVertexArrays(1, &VAO);

    // Terminate GLFW, clearing any resources allocated by GLFW.
    glfwTerminate();

    return EXIT_SUCCESS;
}