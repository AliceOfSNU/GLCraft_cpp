#version 330 core
layout (location = 0) in vec3 aPos; // the position variable has attribute position 0
uniform mat4 _Model;

void main(){
	gl_Position = _Model * vec4(aPos.xy,0.0, 1.0);
}