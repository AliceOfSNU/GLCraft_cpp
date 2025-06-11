#version 330 core
layout (location = 0) in vec3 aPos; // the position variable has attribute position 0
layout (location = 1) in vec2 aTex;
layout (location = 2) in vec3 aOffset;

out vec2 texCoord; //specify which texture coordinate to assign to vertex

uniform mat4 modelview;
uniform mat4 proj;
uniform float _Time;

void main()
{
    vec3 offset;
    offset.xz = aOffset.xz;
    offset.y = mod(aOffset.y - _Time*20, 32.0); 
    gl_Position = proj * modelview * vec4(offset + aPos, 1.0); // see how we directly give a vec3 to vec4's constructor
    texCoord = aTex;
}