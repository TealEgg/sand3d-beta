#version 330 core
layout (location = 0) in vec3 aPos;

uniform vec2 position;
uniform vec2 size;
uniform float depth;

uniform mat4 projection;

void main()
{
    gl_Position = projection * vec4(aPos.xy * size + position, depth, 1.0);
}