#version 330 core

layout (location = 0) in vec3 aPos;


//uniform mat4 model;

uniform vec3 position;
uniform vec3 scale;
uniform mat4 view;
uniform mat4 projection;

void main()
{
	gl_Position = projection * view * vec4(aPos * scale + position, 1.0f);
}