#version 330 core
out vec4 FragColor;
  
in vec3 color; // the input variable from the vertex shader (same name and same type)  

void main()
{
    FragColor = vec4(color, 1.0f);
} 