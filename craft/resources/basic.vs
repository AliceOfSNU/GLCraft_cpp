#version 330 core
layout (location = 0) in vec3 aPos; // the position variable has attribute position 0
layout (location = 1) in vec3 aTex;


out vec3 texCoord; //specify which texture coordinate to assign to vertex

uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;

void main()
{
    gl_Position = proj * view * model * vec4(aPos, 1.0); // see how we directly give a vec3 to vec4's constructor
    texCoord = aTex;
}