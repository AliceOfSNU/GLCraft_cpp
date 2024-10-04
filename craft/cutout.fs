#version 330 core
out vec4 FragColor;
  
in vec3 texCoord;

uniform sampler2DArray tex0;
void main()
{
    vec4 texColor = texture(tex0, texCoord);
    if(texColor.a < 0.1) discard;
    FragColor = texColor;
} 