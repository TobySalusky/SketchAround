cmake_minimum_required(VERSION 3.15)
project(SeniorResearch)

set(CMAKE_CXX_STANDARD 20)

# Add header file
set(GLEW_H /opt/homebrew/Cellar/glew/2.2.0_1/include/GL)
set(GLFW_H /opt/homebrew/Cellar/glfw/3.3.4/include/GLFW)
include_directories(${GLEW_H} ${GLFW_H})

# Add target link
set(GLEW_LINK /opt/homebrew/Cellar/glew/2.2.0_1/lib/libGLEW.2.2.dylib)
set(GLFW_LINK /opt/homebrew/Cellar/glfw/3.3.4/lib/libglfw.3.dylib)
link_libraries(${OPENGL} ${GLEW_LINK} ${GLFW_LINK})


# Execute compilation command
set(SOURCE_FILES src/main.cpp)

set(ICON_NAME "icon.icns")
set(ICON_PATH ${PROJECT_SOURCE_DIR}/assets/${ICON_NAME})
set_source_files_properties(${ICON_NAME} PROPERTIES MACOSX_PACKAGE_LOCATION "Resources")
file(COPY ${ICON_PATH} DESTINATION "SeniorResearch.app/Contents/Resources")


set(SHADERS_PATH "assets/shaders")
file(COPY ${SHADERS_PATH} DESTINATION "SeniorResearch.app/Contents/Resources")

set(FONTS_PATH "assets/fonts")
file(COPY ${FONTS_PATH} DESTINATION "SeniorResearch.app/Contents/Resources")


add_executable(SeniorResearch MACOSX_BUNDLE ${ICON_PATH} src/main.cpp
        src/vendor/imgui/imgui_impl_glfw.cpp
        src/vendor/imgui/imgui_impl_opengl3.cpp
        src/vendor/imgui/imgui.cpp
        src/vendor/imgui/imgui_demo.cpp
        src/vendor/imgui/imgui_draw.cpp
        src/vendor/imgui/imgui_tables.cpp
        src/vendor/imgui/imgui_widgets.cpp
        src/gui/LearnScreen.cpp src/gui/LearnScreen.h src/gui/Markdown.cpp src/gui/Markdown.h src/gui/CreditScreen.cpp src/gui/CreditScreen.h src/editing/ProjectContext.cpp src/editing/ProjectContext.h src/editing/UnnamedProjectContext.cpp src/editing/UnnamedProjectContext.h src/editing/ExistingProjectContext.cpp src/editing/ExistingProjectContext.h
        src/gl/shaders/Shader.cpp src/gl/shaders/Shader.h src/gl/Uniform.cpp src/gl/Uniform.h src/gl/GLWindow.cpp src/gl/GLWindow.h src/gl/Mesh.cpp src/gl/Mesh.h src/generation/Revolver.cpp src/generation/Revolver.h src/util/ImGuiHelper.cpp src/util/ImGuiHelper.h src/gl/Input.cpp src/gl/Input.h src/gl/Camera.cpp src/gl/Camera.h src/gl/Light.cpp src/gl/Light.h src/util/Util.cpp src/util/Util.h src/gl/Normals.cpp src/gl/Normals.h src/generation/Sampler.cpp src/generation/Sampler.h src/vendor/stb/stb_image.h src/vendor/stb/std_image.cpp src/gl/Texture.cpp src/gl/Texture.h src/gl/Mesh2D.cpp src/gl/Mesh2D.h src/gl/shaders/Shader2D.cpp src/gl/shaders/Shader2D.h src/gl/RenderTarget.cpp src/gl/RenderTarget.h src/gl/shaders/Shader3D.cpp src/gl/shaders/Shader3D.h src/gl/Material.cpp src/gl/Material.h src/graphing/Function.cpp src/graphing/Function.h src/graphing/GraphView.cpp src/graphing/GraphView.h src/generation/ModelObject.cpp src/generation/ModelObject.h src/generation/Intersector.cpp src/generation/Intersector.h src/generation/CrossSectionTracer.cpp src/generation/CrossSectionTracer.h src/animation/LineLerper.cpp src/animation/LineLerper.h src/generation/LineAnalyzer.cpp src/generation/LineAnalyzer.h src/generation/Lathe.cpp src/generation/Lathe.h src/generation/CrossSectional.cpp src/generation/CrossSectional.h src/Enums.cpp src/Enums.h src/misc/Undo.cpp src/misc/Undo.h src/gl/MeshUtil.cpp src/gl/MeshUtil.h src/editing/EditingContext.cpp src/editing/EditingContext.h src/animation/Timeline.cpp src/animation/Timeline.h src/util/Rectangle.cpp src/util/Rectangle.h src/animation/KeyFrameLayer.cpp src/animation/KeyFrameLayer.h src/animation/KeyFrame.cpp src/animation/KeyFrame.h src/util/Includes.h src/gl/Display3DContext.cpp src/gl/Display3DContext.h src/util/Exporter.cpp src/util/Exporter.h src/misc/Undos.cpp src/misc/Undos.h src/animation/blending/BlendMode.cpp src/animation/blending/BlendMode.h src/animation/blending/PiecewiseBlendMode.cpp src/animation/blending/PiecewiseBlendMode.h src/animation/blending/LinearBlendMode.cpp src/animation/blending/LinearBlendMode.h src/animation/blending/SineBlendMode.cpp src/animation/blending/SineBlendMode.h src/util/Linq.cpp src/util/Linq.h src/animation/blending/BlendModeManager.cpp src/animation/blending/BlendModeManager.h src/animation/blending/BlendModes.cpp src/animation/blending/BlendModes.h src/animation/Animator.cpp src/animation/Animator.h src/misc/Serialization.cpp src/misc/Serialization.h src/util/Controls.cpp src/util/Controls.h src/gl/TiledTextureAtlas.cpp src/gl/TiledTextureAtlas.h src/exporting/ObjExporter.cpp src/exporting/ObjExporter.h src/generation/TopologyCorrector.cpp src/generation/TopologyCorrector.h src/util/EasingFunctions.cpp src/util/EasingFunctions.h src/animation/blending/EaseBlendModes.cpp src/animation/blending/EaseBlendModes.h src/generation/Collider.cpp src/generation/Collider.h src/misc/UndoTypes/LineStateUndo.cpp src/misc/UndoTypes/LineStateUndo.h src/misc/UndoTypes/MultiUndo.cpp src/misc/UndoTypes/MultiUndo.h src/misc/UndoersInfo.cpp src/misc/UndoersInfo.h src/misc/GuiStyle.cpp src/misc/GuiStyle.h src/generation/CrossSectionSnapPoint.cpp src/generation/CrossSectionSnapPoint.h src/vendor/imgui_markdown.h)


set_target_properties(SeniorResearch PROPERTIES
        BUNDLE True
        MACOSX_BUNDLE_GUI_IDENTIFIER my.domain.style.identifier.SeniorResearch
        MACOSX_BUNDLE_BUNDLE_NAME SeniorResearch
        MACOSX_BUNDLE_BUNDLE_VERSION "0.1"
        MACOSX_BUNDLE_SHORT_VERSION_STRING "0.1"
        MACOSX_BUNDLE_ICON_FILE "icon"
        )

find_package(Boost 1.76 REQUIRED COMPONENTS
        serialization)
target_link_libraries(SeniorResearch Boost::serialization)


if (APPLE)
    target_link_libraries(SeniorResearch "-framework OpenGL")
    target_link_libraries(SeniorResearch "-framework GLUT")
endif()
