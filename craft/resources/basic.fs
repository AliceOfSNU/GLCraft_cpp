#version 330 core
out vec4 FragColor;
  
in vec3 texCoord;
in float light;

uniform sampler2DArray tex0;
uniform float daylight_value;
void main()
{
    vec4 texColor = texture(tex0, texCoord);
    FragColor = daylight_value * (light/15) * texColor;
} 