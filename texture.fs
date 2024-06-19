#version 330 core
out vec4 FragColor;
  
in vec3 color; // the input variable from the vertex shader (same name and same type)  
in vec2 texCoord;

uniform sampler2D tex0;
void main()
{
    FragColor = texture(tex0, texCoord);
} 