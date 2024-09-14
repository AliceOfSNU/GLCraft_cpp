#version 330 core
out vec4 FragColor;
in vec2 screenPos;
uniform vec3 _FillColor;
uniform vec3 _BackColor;
uniform vec2 _Radius;
uniform float _Progress;

const float PI = 3.1415926535897932384626433832795;

void main(){
	float dist = distance(gl_FragCoord.xy, screenPos);
	if(dist < _Radius.x || dist > _Radius.y)
		discard;

	float angle = atan(gl_FragCoord.x - screenPos.x, gl_FragCoord.y - screenPos.y);

    // rescale to degrees
    float amount = angle / (PI * 2.0) + 0.5;
	vec3 color = (amount > _Progress)  ? _FillColor : _BackColor;
	gl_FragColor = vec4(color, 1.0 );
	//gl_FragColor = vec4(amount, amount, 1.0, 1.0 );
}