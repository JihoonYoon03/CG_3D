#version 330 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 color;
uniform mat4 rotation;
out vec3 vertexColor;

void main()
{
	gl_Position = rotation * vec4(position, 1.0);
	vertexColor = color;
} 