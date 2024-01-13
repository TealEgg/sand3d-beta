#version 330 core
layout (location = 0) in vec4 aPos;

out vec2 TexCoord;

uniform mat4 projection;
uniform float depth;


void main()
{
    gl_Position = projection * vec4(aPos.x, aPos.y, depth, 1.0);
    TexCoord = vec2(aPos.z, aPos.w);
}