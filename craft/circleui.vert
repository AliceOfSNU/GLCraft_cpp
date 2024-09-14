#version 330 core
layout (location = 0) in vec3 aPos; // the position variable has attribute position 0
out vec2 screenPos;
uniform vec2 _Center;
uniform mat4 _Model;

void main(){
	gl_Position = _Model * vec4(aPos, 1.0);

	//transform center position to screen space coordinates
    screenPos = _Center;
}