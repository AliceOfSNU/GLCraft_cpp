#version 330 core
out vec4 FragColor;
in vec2 screenPos;
uniform vec3 color;
uniform vec2 radius;

void main(){
	float dist = distance(gl_FragCoord.xy, screenPos);
	if(dist < radius.x || dist > radius.y)
		discard;

	gl_FragColor = vec4(color, 1.0 );
}