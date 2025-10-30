#version 330 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 color;

uniform mat4 rotation, faceModify[6];	// rotation - 월드 회전
uniform vec3 moveToOrigin;
uniform int faceID;
out vec3 vertexColor;

void main()
{
	vec3 originPos = position + moveToOrigin;
	vec4 modifiedPos = faceModify[faceID] * vec4(originPos, 1.0);
	vec3 finalPos = modifiedPos.xyz - moveToOrigin;

	gl_Position = rotation * vec4(finalPos, 1);
	vertexColor = color;
} 