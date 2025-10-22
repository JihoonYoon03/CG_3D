#version 330 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 color;
uniform mat4 rotation, modify;	// rotation - 기본 회전, modify - 해당 면만 변환
uniform bool useTranslation;
uniform vec3 moveToOrigin;
out vec3 vertexColor;

void main()
{
	if (useTranslation) {
		gl_Position = rotation * modify * vec4(position - moveToOrigin, 1.0) + vec4(moveToOrigin, 0);
	}
	else {
		gl_Position = rotation * vec4(position, 1.0);
	}
	vertexColor = color;
} 