#version 330 core

in vec3 c;

out vec4 color;

void main()
{
    color = vec4(c, 1.0f);
}
