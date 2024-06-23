#version 330 core
layout (location = 0) in vec3 aPos; // the position variable has attribute position 0
out vec2 screenPos;
uniform vec2 center;
uniform vec2 resolution;

void main(){
	gl_Position = vec4(aPos, 1.0);

	//transform center position to screen space coordinates
    screenPos = resolution * (center * 0.5 + 0.5);
}