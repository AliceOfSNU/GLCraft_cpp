cmake_minimum_required(VERSION 3.5)
set(CMAKE_CXX_STANDARD 17)
project(GLcraft)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
find_package(OpenGL REQUIRED)
find_package(GLFW3 REQUIRED)

SET(TARGET_H
rendering.h
GLObjects.h
world.h
blocks.h
gui.h
plants.h
collision.h
debug.h
weather.h
)

SET(TARGET_SRC
glad.c
rendering.cpp
GLObjects.cpp
blocks.cpp
world.cpp
gui.cpp
plants.cpp
collision.cpp
debug.cpp
weather.cpp
model_utils.cpp
)
add_executable(GLcraft ${TARGET_SRC} main.cpp )
add_subdirectory(generation)
add_subdirectory(miniGLTF)
target_include_directories(GLcraft PUBLIC ${CMAKE_SOURCE_DIR}/miniGLTF ${CMAKE_SOURCE_DIR}/generation ${CMAKE_SOURCE_DIR}/Libraries/include ${GLFW3_INCLUDE_DIR})
target_link_directories(GLcraft PRIVATE generation miniGLTF)
target_link_libraries(GLcraft PRIVATE ${GLFW3_LIBRARY} OpenGL::GL Generation MiniGLTF)
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/resources DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/Debug)