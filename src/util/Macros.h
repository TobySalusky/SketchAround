//
// Created by Tobiathan on 9/22/21.
//

#ifndef SENIORRESEARCH_MACROS_H
#define SENIORRESEARCH_MACROS_H

#include <glew.h>
#include <iostream>


#define GLCall(x) GLClearError(); \
try {\
x;\
} catch (std::exception& e) {\
printf(e.what());\
}\
GLCheckError();\

static void GLClearError() {
    while (glGetError() != GL_NO_ERROR);
}

static void GLCheckError() {
    while (GLenum error = glGetError()) {
        std::cout << "[OpenGL Error]: (" << error << ")\n";
    }
}

#endif //SENIORRESEARCH_MACROS_H
