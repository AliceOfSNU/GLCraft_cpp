#version 330 core
out vec4 FragColor;
  
uniform vec3 _Color;
void main()
{
    gl_FragColor = vec4(_Color, 1.0f);
} 