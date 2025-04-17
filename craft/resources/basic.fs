#version 330 core
out vec4 FragColor;
  
in vec3 texCoord;
in float light;
in float torchlight;
uniform sampler2DArray tex0;
uniform float daylight_value;
void main()
{
    vec4 texColor = texture(tex0, texCoord);
    vec4 tintedColor = (1.0 + torchlight/15) * texColor;
    FragColor = daylight_value * (light/15) * tintedColor;
} 