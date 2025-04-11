#version 330 core
out vec4 FragColor;
  
in vec3 texCoord;

uniform sampler2DArray tex0;
uniform float _Time;
void main()
{
    FragColor = texture(tex0, vec3(texCoord.x+_Time, texCoord.y, texCoord.z));
} 