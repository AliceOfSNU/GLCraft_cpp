#version 330 core
layout (location = 0) in vec3 aPos; // the position variable has attribute position 0
layout (location = 1) in vec3 aTex; 
uniform mat4 _Model;

out vec3 texCoord;
void main(){
	gl_Position = _Model * vec4(aPos.xy,0.0, 1.0);
	texCoord = aTex;
}