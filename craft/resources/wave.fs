#version 330 core
out vec4 FragColor;
  
in vec3 texCoord;

uniform sampler2DArray tex0;
uniform float _Time;
uniform float daylight_value;
void main()
{
    FragColor = daylight_value*texture(tex0, vec3(texCoord.x+_Time, texCoord.y, texCoord.z));
} 