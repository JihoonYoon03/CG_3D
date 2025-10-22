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
		vec3 originPos = position + moveToOrigin;
		vec4 modifiedPos = modify * vec4(originPos, 1.0);
		vec3 finalPos = modifiedPos.xyz - moveToOrigin;

		gl_Position = rotation * vec4(finalPos, 1);
	}
	else {
		gl_Position = rotation * vec4(position, 1.0);
	}
	vertexColor = color;
} 