#version 330 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 color;
uniform mat4 projection = mat4(1.0);
uniform mat4 view = mat4(1.0);
uniform mat4 world = mat4(1.0);
uniform mat4 model = mat4(1.0);
uniform vec3 color_set = vec3(0.0, 0.0, 0.0);
uniform bool use_color_set = false, isBasis = false;
out vec3 vertexColor;

void main()
{
	if (isBasis)
	{
		gl_Position = projection * view * world * vec4(position, 1.0);
	}
	else
	{
		gl_Position = projection * view * world * model * vec4(position, 1.0);
	}

	if (use_color_set)
	{
		vertexColor = color_set;
	}
	else
		vertexColor = color;
} 