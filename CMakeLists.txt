cmake_minimum_required(VERSION 3.10)
set(PROJECT_NAME an-opengl-solar-system)
project(${PROJECT_NAME})

set(CMAKE_CXX_STANDARD 17)

message(STATUS "System: ${CMAKE_SYSTEM_NAME}")
message(STATUS "Processor: ${CMAKE_SYSTEM_PROCESSOR}")

find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)

set(SS_ROOT ${CMAKE_CURRENT_SOURCE_DIR})
set(SS_DEP_DIR ${SS_ROOT}/Dependencies)

set(SS_SRC_DIR ${SS_ROOT}/AnOpenGLSolarSystem/src)

set(SS_SRC_FILES
${SS_SRC_DIR}/main.cpp
${SS_SRC_DIR}/Helper.hpp
${SS_SRC_DIR}/Planet.hpp
${SS_SRC_DIR}/World.hpp
${SS_SRC_DIR}/sdk/BufferLayout.hpp
${SS_SRC_DIR}/sdk/Camera.hpp
${SS_SRC_DIR}/sdk/Controller.hpp
${SS_SRC_DIR}/sdk/Headers.hpp
${SS_SRC_DIR}/sdk/IndexBuffer.hpp
${SS_SRC_DIR}/sdk/Renderer.hpp
${SS_SRC_DIR}/sdk/Shader.hpp
${SS_SRC_DIR}/sdk/VertexArray.hpp
${SS_SRC_DIR}/sdk/VertexBuffer.hpp
)

file(GLOB_RECURSE SS_VENDOR_FILES
    "${SS_SRC_DIR}/vendor/*.*"
)

add_executable(${PROJECT_NAME} ${SS_SRC_FILES} ${SS_VENDOR_FILES})

target_include_directories(
    ${PROJECT_NAME} PRIVATE
    ${SS_SRC_DIR}
    ${SS_SRC_DIR}/sdk
    ${SS_SRC_DIR}/vendor
    ${SS_DEP_DIR}/GLEW/include
    ${SS_DEP_DIR}/GLFW/include
)

set(GLFW_LIB_PATH "")
set(GLFW_LIB_NAME "glfw3")

if(${CMAKE_SYSTEM_NAME} STREQUAL "Darwin")
    if(${CMAKE_SYSTEM_PROCESSOR} STREQUAL "arm64")
        set(GLFW_LIB_PATH ${SS_DEP_DIR}/GLFW/lib-arm64)
    elseif(${CMAKE_SYSTEM_PROCESSOR} STREQUAL "x86_64")
        set(GLFW_LIB_PATH ${SS_DEP_DIR}/GLFW/lib-x86_64)
    endif()
elseif(${CMAKE_SYSTEM_NAME} STREQUAL "Windows")
    set(GLFW_LIB_PATH ${SS_DEP_DIR}/GLFW/lib-vc2022)
elseif(${CMAKE_SYSTEM_NAME} STREQUAL "Linux")
    set(GLFW_LIB_PATH ${SS_DEP_DIR}/GLFW/lib-universal)
endif()

message(${GLFW_LIB_PATH})

set(PLATFORM_LINK_LIBS)
if(${CMAKE_SYSTEM_NAME} STREQUAL "Darwin")
    # Add macOS specific frameworks
    list(APPEND PLATFORM_LINK_LIBS
        "-framework Cocoa"
        "-framework IOKit"
        "-framework CoreFoundation"
        "-framework CoreVideo"
    )
elseif(${CMAKE_SYSTEM_NAME} STREQUAL "Windows")
    list(APPEND PLATFORM_LINK_LIBS
        "opengl32"
        "gdi32"
        "user32"
        "shell32"
    )
elseif(${CMAKE_SYSTEM_NAME} STREQUAL "Linux")
    list(APPEND PLATFORM_LINK_LIBS
        "GL"
        "X11"
        "pthread"
        "Xrandr"
        "Xi"
        "dl"
    )
endif()

target_link_directories(${PROJECT_NAME} PUBLIC ${GLFW_LIB_PATH})
target_link_libraries(${PROJECT_NAME} PRIVATE ${GLFW_LIB_NAME} ${PLATFORM_LINK_LIBS} OpenGL::GL GLEW::GLEW)