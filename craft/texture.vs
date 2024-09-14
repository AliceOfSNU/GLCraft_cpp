#version 330 core
layout (location = 0) in vec3 aPos; // the position variable has attribute position 0
layout (location = 1) in vec3 aColor;
layout (location = 2) in vec2 aTex;

out vec3 color; // specify a color output to the fragment shader
out vec2 texCoord; //specify which texture coordinate to assign to vertex

void main()
{
    gl_Position = vec4(aPos, 1.0); // see how we directly give a vec3 to vec4's constructor
    color = aColor;
    texCoord = aTex;
}