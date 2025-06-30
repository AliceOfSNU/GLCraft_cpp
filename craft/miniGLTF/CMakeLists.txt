add_library(MiniGLTF STATIC Texture.cpp Mesh.cpp Model.cpp)
target_include_directories(MiniGLTF PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include ${CMAKE_SOURCE_DIR}/Libraries/include ${GLFW3_INCLUDE_DIR})