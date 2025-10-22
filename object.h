#pragma once
#include <iostream>
#include <gl/glew.h>
#include <gl/freeglut.h>
#include <gl/freeglut_ext.h>
#include <gl/glm/glm.hpp>
#include <gl/glm/ext.hpp>
#include <gl/glm/gtc/matrix_transform.hpp>
#include "tools.h"

extern GLuint shaderProgramID;

class Cube {
	ColoredVertex vertices[8] =
	{
		// 좌하 앞, 우하 앞, 우상 앞, 좌상 앞
		{ { -0.5f, -0.5f,  -0.5f }, { 1.0f, 0.0f, 1.0f } },
		{ {  0.5f, -0.5f,  -0.5f }, { 0.0f, 1.0f, 1.0f } },
		{ {  0.5f,  0.5f,  -0.5f }, { 1.0f, 1.0f, 1.0f } },
		{ { -0.5f,  0.5f,  -0.5f }, { 0.0f, 1.0f, 0.0f } },

		// 좌하 뒤, 우하 뒤, 우상 뒤, 좌상 뒤
		{ { -0.5f, -0.5f, 0.5f }, { 1.0f, 0.0f, 0.0f } },
		{ {  0.5f, -0.5f, 0.5f }, { 0.0f, 1.0f, 0.0f } },
		{ {  0.5f,  0.5f, 0.5f }, { 0.0f, 0.0f, 1.0f } },
		{ { -0.5f,  0.5f, 0.5f }, { 1.0f, 1.0f, 0.0f } }
	};
	GLuint indices[36] =
	{
		// 앞면
		0, 1, 2,
		0, 2, 3,
		// 왼쪽면
		4, 0, 3,
		4, 3, 7,
		// 윗면
		3, 2, 6,
		3, 6, 7,
		// 오른쪽면
		1, 5, 6,
		1, 6, 2,
		// 아랫면
		4, 5, 1,
		4, 1, 0,
		// 뒷면
		5, 4, 7,
		5, 7, 6
	};

	glm::vec3 toOrigin[6] = {
		glm::vec3(0.0f, -0.5f, 0.5f),
		glm::vec3(0.5f, 0.0f, 0.0f),
		glm::vec3(0.0f, -0.5f, 0.0f),
		glm::vec3(-0.5f, 0.0f, 0.0f),
		glm::vec3(0.0f, 0.5f, 0.0f),
		glm::vec3(0.0f, 0.0f, -0.5f)
	};

	bool faceToggle[6] = { false };
	bool faceModify[6] = { false };
	int lastDisplayFace = -1;

	GLuint VAO, VBO, EBO;

public:
	Cube(GLfloat size = 1.0f);
	void Render();
	void DisplayOnly(int index);
	void DisplayRandom();
	void modifyFace(int index, bool toggle = false);
	bool isFaceModified(int index) { return faceModify[index]; }
};

class Pyramid {
	ColoredVertex vertices[16] =
	{
		// 앞면 삼각
		{ {  0.0f,  0.5f,  0.0f }, { 1.0f, 1.0f, 0.0f } },
		{ { -0.5f, -0.5f, -0.5f }, { 1.0f, 1.0f, 0.0f } },
		{ {  0.5f, -0.5f, -0.5f }, { 1.0f, 1.0f, 0.0f } },

		// 왼쪽면 삼각
		{ {  0.0f,  0.5f,  0.0f }, { 0.0f, 1.0f, 0.0f } },
		{ { -0.5f, -0.5f,  0.5f }, { 0.0f, 1.0f, 0.0f } },
		{ { -0.5f, -0.5f, -0.5f }, { 0.0f, 1.0f, 0.0f } },

		// 오른쪽면 삼각
		{ {  0.0f,  0.5f,  0.0f }, { 0.0f, 0.0f, 1.0f } },
		{ {  0.5f, -0.5f, -0.5f }, { 0.0f, 0.0f, 1.0f } },
		{ {  0.5f, -0.5f,  0.5f }, { 0.0f, 0.0f, 1.0f } },

		// 뒷면 삼각
		{ {  0.0f,  0.5f,  0.0f }, { 1.0f, 0.0f, 0.0f } },
		{ {  0.5f, -0.5f,  0.5f }, { 1.0f, 0.0f, 0.0f } },
		{ { -0.5f, -0.5f,  0.5f }, { 1.0f, 0.0f, 0.0f } },

		// 밑면 사각
		{ { -0.5f, -0.5f, -0.5f }, { 0.0f, 1.0f, 1.0f } },
		{ { -0.5f, -0.5f,  0.5f }, { 0.0f, 1.0f, 1.0f } },
		{ {  0.5f, -0.5f,  0.5f }, { 0.0f, 1.0f, 1.0f } },
		{ {  0.5f, -0.5f, -0.5f }, { 0.0f, 1.0f, 1.0f } }

	};

	GLuint indices[18] =
	{
		// 앞면
		0, 1, 2,
		// 왼쪽면
		3, 4, 5,
		// 오른쪽면
		6, 7, 8,
		// 뒷면
		9, 10, 11,
		// 밑면
		12, 13, 14,
		12, 14, 15
	};

	bool faceToggle[5] = { false };
	bool faceModify[5] = { false };
	int lastDisplayFace = -1;

	GLuint VAO, VBO, EBO;

public:
	Pyramid(GLfloat size = 1.0f);
	void Render();
	void DisplayOnly(int index);
	void DisplayRandom();
	void modifyFace(int index, bool toggle = false);
	bool isFaceModified(int index) { return faceModify[index]; }
};