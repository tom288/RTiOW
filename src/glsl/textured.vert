#version 330 core

layout (location = 0) in vec2 worldPos;
layout (location = 1) in vec2 texPos;

out vec2 uv;

void main()
{
    gl_Position = vec4(worldPos, 0.0, 1.0);
    uv = texPos;
}
