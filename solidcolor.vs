#version 330 core
layout (location = 0) in vec3 aPos;

out vec3 color;

uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;
uniform vec3 col;

const float PI = 3.1415926535897932384626433832795;

void main()
{
    gl_Position = proj * view * model *  vec4(aPos, 1.0); 
    color = col; 
}