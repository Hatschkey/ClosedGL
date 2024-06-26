cmake_minimum_required (VERSION 2.8.12)

project (ClosedGL)

LINK_DIRECTORIES( ${CMAKE_SOURCE_DIR}/lib )

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

# Second, for multi-config builds (e.g. msvc)
foreach( OUTPUTCONFIG ${CMAKE_CONFIGURATION_TYPES} )
    string( TOUPPER ${OUTPUTCONFIG} OUTPUTCONFIG )
    set( CMAKE_RUNTIME_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ${CMAKE_SOURCE_DIR}/bin)
    set( CMAKE_LIBRARY_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ${CMAKE_SOURCE_DIR}/lib )
    set( CMAKE_ARCHIVE_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ${CMAKE_SOURCE_DIR}/lib )
endforeach( OUTPUTCONFIG CMAKE_CONFIGURATION_TYPES )

set (CMAKE_DEBUG_POSTFIX "_d")

find_package(OpenGL REQUIRED)

if(WIN32)
set(COMMON_LIBS ${OPENGL_LIBRARIES} optimized glfw debug glfw)
elseif (UNIX)
set(COMMON_LIBS ${OPENGL_LIBRARIES} glfw ${GLFW_LIBRARIES})
else()
set(COMMON_LIBS)
endif()
set(COMMON_LIBS ${COMMON_LIBS} ${EXTRA_LIBS})

set(RUN_DIR ${PROJECT_SOURCE_DIR}/bin)

add_executable(ClosedGL 
  src/main.cpp
  src/ProgramManager.cpp
  src/3dScene/Model3D.cpp
  src/3dScene/Camera.cpp
  src/3dScene/Scene.cpp
  src/3dScene/primitives/Texture.cpp
  src/ui/PropertyManager.cpp
  src/ui/Window.cpp
  src/rendering/RenderingManager.cpp
  src/rendering/shaders/ShaderUtils.cpp
  src/rendering/shaders/vertexShader.vert 
  src/rendering/shaders/fragmentShader.frag
  src/rendering/engines/OpenGL/OpenGLRenderer.cpp
  src/rendering/engines/Close2GL/Close2GLRenderer.cpp
  src/rendering/engines/Close2GL/Close2GLRenderingUtils.cpp
  src/rendering/engines/Close2GL/shading/vertex/VertexLightingStrategy.cpp
  src/rendering/engines/Close2GL/shading/vertex/FlatLightingStrategy.cpp
  src/rendering/engines/Close2GL/shading/vertex/GouraudADLightingStrategy.cpp
  src/rendering/engines/Close2GL/shading/vertex/GouraudADSLightingStrategy.cpp
  src/rendering/engines/Close2GL/shading/vertex/PhongLightingStrategyVertex.cpp
  src/rendering/engines/Close2GL/shading/fragment/FragmentLightingStrategy.cpp
  src/rendering/engines/Close2GL/shading/fragment/PhongLightingStrategyFragment.cpp
  src/rendering/engines/Close2GL/shading/fragment/TextureLightingStrategy.cpp
  src/rendering/engines/Close2GL/rasterization/RasterizationStrategy.cpp
  src/rendering/engines/Close2GL/rasterization/WireframeRasterizationStrategy.cpp
  src/rendering/engines/Close2GL/rasterization/FilledRasterizationStrategy.cpp
  src/rendering/engines/Close2GL/rasterization/PointsRasterizationStrategy.cpp
  src/rendering/engines/Close2GL/rasterization/primitives/Edge.cpp
  lib/gl3w.c
  lib/imgui/imgui_demo.cpp
  lib/imgui/imgui_draw.cpp
  lib/imgui/imgui_tables.cpp
  lib/imgui/imgui_widgets.cpp
  lib/imgui/imgui.cpp
  lib/imgui/imgui_impl_glfw.cpp
  lib/imgui/imgui_impl_opengl3.cpp
  lib/FileBrowser/ImGuiFileBrowser.cpp
  )
set_property(TARGET ClosedGL PROPERTY DEBUG_POSTFIX _d)
target_link_libraries(ClosedGL ${COMMON_LIBS})

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -DIMGUI_IMPL_OPENGL_LOADER_GL3W -DGLFW_EXPOSE_NATIVE_WGL")

IF (${CMAKE_SYSTEM_NAME} MATCHES "Linux")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D_LINUX -")
ENDIF (${CMAKE_SYSTEM_NAME} MATCHES "Linux")

MACRO(HEADER_DIRECTORIES return_list)
    FILE(GLOB_RECURSE new_list *.hpp)
    SET(dir_list "")
    FOREACH(file_path ${new_list})
        GET_FILENAME_COMPONENT(dir_path ${file_path} PATH)
        SET(dir_list ${dir_list} ${dir_path})
    ENDFOREACH()
    LIST(REMOVE_DUPLICATES dir_list)
    SET(${return_list} ${dir_list})
ENDMACRO()

HEADER_DIRECTORIES( header_dir_list )
include_directories( ${header_dir_list} )

include_directories(lib)
include_directories(lib/imgui/)
include_directories(lib/FileBrowser)
include_directories(lib/FileBrower/Dirent)
include_directories(lib/glfw/include)

target_link_libraries(ClosedGL imm32.lib)

option(GLFW_BUILD_EXAMPLES OFF)
option(GLFW_BUILD_TESTS OFF)
option(GLFW_BUILD_DOCS OFF)
set(CMAKE_WARN_DEPRECATED OFF CACHE BOOL "" FORCE)
add_subdirectory(lib/glfw)
