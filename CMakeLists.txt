cmake_minimum_required(VERSION 3.15)
project(SeniorResearch)

set(CMAKE_CXX_STANDARD 17)

# Add header file
set(GLEW_H /opt/homebrew/Cellar/glew/2.2.0_1/include/GL)
set(GLFW_H /opt/homebrew/Cellar/glfw/3.3.4/include/GLFW)
include_directories(${GLEW_H} ${GLFW_H})

# Add target link
set(GLEW_LINK /opt/homebrew/Cellar/glew/2.2.0_1/lib/libGLEW.2.2.dylib)
set(GLFW_LINK /opt/homebrew/Cellar/glfw/3.3.4/lib/libglfw.3.dylib)
link_libraries(${OPENGL} ${GLEW_LINK} ${GLFW_LINK})

# Execute compilation command
set(SOURCE_FILES main.cpp)
add_executable(SeniorResearch main.cpp
        src/vendor/imgui/imgui_impl_glfw.cpp
        src/vendor/imgui/imgui_impl_opengl3.cpp
        src/vendor/imgui/imgui.cpp
        src/vendor/imgui/imgui_demo.cpp
        src/vendor/imgui/imgui_draw.cpp
        src/vendor/imgui/imgui_tables.cpp
        src/vendor/imgui/imgui_widgets.cpp
        src/IBind.h src/Shader.cpp src/Shader.h src/Uniform.cpp src/Uniform.h src/GLWindow.cpp src/GLWindow.h src/Mesh.cpp src/Mesh.h src/Revolver.cpp src/Revolver.h)

if (APPLE)
    target_link_libraries(SeniorResearch "-framework OpenGL")
    target_link_libraries(SeniorResearch "-framework GLUT")
endif()
