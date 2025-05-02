#version 330 core
out vec4 FragColor;
  
in vec2 texCoord;

uniform sampler2D tex0;
uniform vec3 tint;

void main()
{
    FragColor = texture(tex0, texCoord) * vec4(tint, 1.0);
} 