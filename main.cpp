#include <iostream>
#include <glew.h>
#include <glfw3.h>
#include <vector>
#include "src/vendor/glm/glm.hpp"
#include "src/VBO.h"
#include "src/vendor/glm/gtc/type_ptr.hpp"

#define LOG(a) std::cout << a << '\n';

// Window dimensions
const GLuint WIDTH = 800, HEIGHT = 600;

// Shaders
const char* vertexShaderSource = R"glsl(
#version 330 core
layout (location = 0) in vec3 position;

void main()
{
    gl_Position = vec4(position, 1.0f);
}
)glsl";
const char* fragmentShaderSource = R"glsl(
#version 330 core
out vec4 color;
void main()
{
    color = vec4(1.0f, 0.5f, 0.2f, 1.0f);
}
)glsl";

static glm::vec2 mousePos;
static bool mouseDown = false;

static void mousePosCallback(GLFWwindow* window, double x, double y) {
    //std::cout << x << ':' << y << '\n';
    mousePos = {x, y};
}

static void mousePressCallback(GLFWwindow* window, int button, int action, int mods) {
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
        //LOG("down");
        mouseDown = true;
    }

    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE) {
        //LOG("up");
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
    // Init GLFW
    glfwInit( );
    // Set all the required options for GLFW
    glfwWindowHint( GLFW_CONTEXT_VERSION_MAJOR, 3 );
    glfwWindowHint( GLFW_CONTEXT_VERSION_MINOR, 3 );
    glfwWindowHint( GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE );
    glfwWindowHint( GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE );

    glfwWindowHint( GLFW_RESIZABLE, GL_FALSE );

    // Create a GLFWwindow object that we can use for GLFW's functions
    GLFWwindow *window = glfwCreateWindow( WIDTH, HEIGHT, "LearnOpenGL", nullptr, nullptr );

    int screenWidth, screenHeight;
    glfwGetFramebufferSize( window, &screenWidth, &screenHeight );



    if (window == nullptr)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate( );

        return EXIT_FAILURE;
    }

    glfwMakeContextCurrent(window);

    // Set this to true so GLEW knows to use a modern approach to retrieving function pointers and extensions
    glewExperimental = GL_TRUE;
    // Initialize GLEW to setup the OpenGL Function pointers
    if ( GLEW_OK != glewInit( ) )
    {
        std::cout << "Failed to initialize GLEW" << std::endl;
        return EXIT_FAILURE;
    }

    // Define the viewport dimensions
    glViewport( 0, 0, screenWidth, screenHeight );


    // Build and compile our shader program
    // Vertex shader
    GLuint vertexShader = glCreateShader( GL_VERTEX_SHADER );
    glShaderSource( vertexShader, 1, &vertexShaderSource, NULL );
    glCompileShader( vertexShader );

    // Check for compile time errors
    GLint success;
    GLchar infoLog[512];

    glGetShaderiv( vertexShader, GL_COMPILE_STATUS, &success );
    if ( !success )
    {
        glGetShaderInfoLog( vertexShader, 512, NULL, infoLog );
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    // Fragment shader
    GLuint fragmentShader = glCreateShader( GL_FRAGMENT_SHADER );
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL );
    glCompileShader(fragmentShader);

    // Check for compile time errors
    glGetShaderiv( fragmentShader, GL_COMPILE_STATUS, &success );

    if ( !success )
    {
        glGetShaderInfoLog( fragmentShader, 512, NULL, infoLog );
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    // Link shaders
    GLuint shaderProgram = glCreateProgram( );
    glAttachShader( shaderProgram, vertexShader );
    glAttachShader( shaderProgram, fragmentShader );
    glLinkProgram( shaderProgram );

    // Check for linking errors
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);

    if (!success)
    {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);


    // Set up vertex data (and buffer(s)) and attribute pointers
    std::vector<GLfloat> vec {
            -0.5f, -0.5f, 0.0f, // Left
            0.5f, -0.5f, 0.0f, // Right
            0.0f,  0.5f, 0.0f  // Top
    };

    std::vector<GLfloat> vec2 {
            -0.5f, -0.7f, 0.0f, // Left
            0.5f, -0.5f, 0.0f, // Right
            0.0f,  0.5f, 0.0f  // Top
    };

    VBO vbo;
    GLuint VBO, VAO;

    glGenVertexArrays( 1, &VAO );
    VBO = vbo.ID;
    // Bind the Vertex Array Object first, then bind and set vertex buffer(s) and attribute pointer(s).
    glBindVertexArray( VAO );

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vec.size() * sizeof(GLfloat), &vec[0], GL_DYNAMIC_DRAW);

    glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof( GLfloat ), ( GLvoid * ) 0 );
    glEnableVertexAttribArray(0);

    glBindBuffer( GL_ARRAY_BUFFER, 0 ); // Note that this is allowed, the call to glVertexAttribPointer registered VBO as the currently bound vertex buffer object so afterwards we can safely unbind

    glBindVertexArray( 0 ); // Unbind VAO (it's always a good thing to unbind any buffer/array to prevent strange bugs)


    glfwSetCursorPosCallback(window, mousePosCallback);
    glfwSetMouseButtonCallback(window, mousePressCallback);


//    glm::mat4 trans = glm::mat4(1.0f);
//    trans = glm::rotate(trans, glm::radians(90.0f), glm::vec3(0.0, 0.0, 1.0));
//    trans = glm::scale(trans, glm::vec3(0.5, 0.5, 0.5));
//
//
//    unsigned int transformLoc = glGetUniformLocation(shaderProgram, "transform");
//    glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(trans));
//
//    glm::vec4 v(1.0f, 1.0f, 0.0f, 1.0f);
//    auto trans = glm::translate(glm::mat4(1.0f), glm::vec3(0.1f, -0.1f, 0.0f));
//    v = trans * v;
//    std::cout << v.x << ':' << v.y << ":" << v.z << std::endl;


    auto transformLoc = glGetUniformLocation(shaderProgram, "transform");
    glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(glm::mat4(1.0f)));

    // Game loop
    while (!glfwWindowShouldClose( window ))
    {

        auto onScreen = MouseToScreen(mousePos);
        std::vector<GLfloat> tempVec {
                -0.5f, -0.7f, 0.0f, // Left
                0.5f, -0.5f, 0.0f, // Right
                onScreen.x, onScreen.y, 0.0f  // Top
        };

        // Check if any events have been activiated (key pressed, mouse moved etc.) and call corresponding response functions
        glfwPollEvents();

        // Render
        // Clear the colorbuffer
        auto normMouse = MouseToScreenNorm01(mousePos);
        glClearColor(normMouse.x, 0.0f, normMouse.y, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // Draw our first triangle
        glUseProgram( shaderProgram );
        glBindVertexArray( VAO );

        glDrawArrays(GL_TRIANGLES, 0, 3);


        vbo.BufferData(tempVec);

        glBindVertexArray( 0 );

        // Swap the screen buffers
        glfwSwapBuffers( window );
    }

    // Properly de-allocate all resources once they've outlived their purpose
    glDeleteVertexArrays( 1, &VAO );

    // Terminate GLFW, clearing any resources allocated by GLFW.
    glfwTerminate( );

    return EXIT_SUCCESS;
}