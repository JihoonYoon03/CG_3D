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

class ObjCube {
protected:
	// 원본 정점 데이터 (변경되지 않음)
	const glm::vec3 original_vertices[24] =
	{
		// 앞면 (0-3): Z = +0.5
		{ -0.5f, -0.5f,  0.5f }, {  0.5f, -0.5f,  0.5f },
		{  0.5f,  0.5f,  0.5f }, { -0.5f,  0.5f,  0.5f },

		// 왼쪽면 (4-7): X = -0.5
		{ -0.5f, -0.5f, -0.5f }, { -0.5f, -0.5f,  0.5f },
		{ -0.5f,  0.5f,  0.5f }, { -0.5f,  0.5f, -0.5f },

		// 윗면 (8-11): Y = +0.5
		{ -0.5f,  0.5f,  0.5f }, {  0.5f,  0.5f,  0.5f },
		{  0.5f,  0.5f, -0.5f }, { -0.5f,  0.5f, -0.5f },

		// 오른쪽면 (12-15): X = +0.5
		{  0.5f, -0.5f,  0.5f }, {  0.5f, -0.5f, -0.5f },
		{  0.5f,  0.5f, -0.5f }, {  0.5f,  0.5f,  0.5f },

		// 아랫면 (16-19): Y = -0.5
		{ -0.5f, -0.5f, -0.5f }, {  0.5f, -0.5f, -0.5f },
		{  0.5f, -0.5f,  0.5f }, { -0.5f, -0.5f,  0.5f },

		// 뒷면 (20-23): Z = -0.5
		{  0.5f, -0.5f, -0.5f }, { -0.5f, -0.5f, -0.5f },
		{ -0.5f,  0.5f, -0.5f }, {  0.5f,  0.5f, -0.5f }
	};

	glm::vec3 vertices[24];

	glm::vec3 normals[24] =
	{
		// 앞면
		{  0.0f,  0.0f,  1.0f }, {  0.0f,  0.0f,  1.0f },
		{  0.0f,  0.0f,  1.0f }, {  0.0f,  0.0f,  1.0f },

		// 왼쪽면
		{ -1.0f,  0.0f,  0.0f }, { -1.0f,  0.0f,  0.0f },
		{ -1.0f,  0.0f,  0.0f }, { -1.0f,  0.0f,  0.0f },

		// 윗면
		{  0.0f,  1.0f,  0.0f }, {  0.0f,  1.0f,  0.0f },
		{  0.0f,  1.0f,  0.0f }, {  0.0f,  1.0f,  0.0f },

		// 오른쪽면
		{  1.0f,  0.0f,  0.0f }, {  1.0f,  0.0f,  0.0f },
		{  1.0f,  0.0f,  0.0f }, {  1.0f,  0.0f,  0.0f },

		// 아랫면
		{  0.0f, -1.0f,  0.0f }, {  0.0f, -1.0f,  0.0f },
		{  0.0f, -1.0f,  0.0f }, {  0.0f, -1.0f,  0.0f },

		// 뒷면
		{  0.0f,  0.0f, -1.0f }, {  0.0f,  0.0f, -1.0f },
		{  0.0f,  0.0f, -1.0f }, {  0.0f,  0.0f, -1.0f }
	};

	glm::vec3 colors[24] =
	{
		// 앞면
		{ 1.0f, 1.0f, 1.0f }, { 1.0f, 1.0f, 1.0f },
		{ 1.0f, 1.0f, 1.0f }, { 1.0f, 1.0f, 1.0f },

		// 왼쪽면
		{ 1.0f, 1.0f, 1.0f }, { 1.0f, 1.0f, 1.0f },
		{ 1.0f, 1.0f, 1.0f }, { 1.0f, 1.0f, 1.0f },

		// 윗면
		{ 1.0f, 1.0f, 1.0f }, { 1.0f, 1.0f, 1.0f },
		{ 1.0f, 1.0f, 1.0f }, { 1.0f, 1.0f, 1.0f },

		// 오른쪽면
		{ 1.0f, 1.0f, 1.0f }, { 1.0f, 1.0f, 1.0f },
		{ 1.0f, 1.0f, 1.0f }, { 1.0f, 1.0f, 1.0f },

		// 아랫면
		{ 1.0f, 1.0f, 1.0f }, { 1.0f, 1.0f, 1.0f },
		{ 1.0f, 1.0f, 1.0f }, { 1.0f, 1.0f, 1.0f },

		// 뒷면
		{ 1.0f, 1.0f, 1.0f }, { 1.0f, 1.0f, 1.0f },
		{ 1.0f, 1.0f, 1.0f }, { 1.0f, 1.0f, 1.0f }
	};

	GLuint indices[36] =
	{
		// 앞면
		0, 1, 2,    0, 2, 3,
		// 왼쪽면
		4, 5, 6,    4, 6, 7,
		// 윗면
		8, 9, 10,   8, 10, 11,
		// 오른쪽면
		12, 13, 14, 12, 14, 15,
		// 아랫면
		16, 17, 18, 16, 18, 19,
		// 뒷면
		20, 21, 22, 20, 22, 23
	};

	GLuint VAO, VERTEX, FACE, COLOR, NORMAL;

	glm::vec3 center;

	// 초기 변환 (생성자에서 설정, 이후 변경되지 않음)
	glm::mat4 initial_scale = glm::mat4(1.0f);
	glm::mat4 initial_rotate = glm::mat4(1.0f);
	glm::mat4 initial_translate = glm::mat4(1.0f);

	// 추가 변환
	std::queue<glm::mat4> transform_queue;
	glm::mat4 additional_transform = glm::mat4(1.0f);
public:

	ObjCube(glm::vec3 scaling = { 1.0f, 1.0f, 1.0f }, glm::vec3 rotation = { 0.0f, 0.0f, 0.0f }, glm::vec3 location = { 0.0f, 0.0f, 0.0f }, glm::vec3 color = { 1.0f, 1.0f, 1.0f });

	void scaling(glm::vec3 amount);
	void rotating(glm::vec3 amount);
	void translating(glm::vec3 amount);
	glm::mat4 getModelMatrix();
	glm::vec3 getCenter();

	void Render();

	virtual void reset();
};

class Cube {
	ColoredVertex vertices[8] =
	{
		// 좌하 앞(0), 우하 앞(1), 우상 앞(2), 좌상 앞(3)
		{ { -0.5f, -0.5f,  0.5f }, { 1.0f, 0.0f, 1.0f } },
		{ {  0.5f, -0.5f,  0.5f }, { 0.0f, 1.0f, 1.0f } },
		{ {  0.5f,  0.5f,  0.5f }, { 1.0f, 1.0f, 1.0f } },
		{ { -0.5f,  0.5f,  0.5f }, { 0.0f, 1.0f, 0.0f } },

		// 좌하 뒤(4), 우하 뒤(5), 우상 뒤(6), 좌상 뒤(7)
		{ { -0.5f, -0.5f, -0.5f }, { 1.0f, 0.0f, 0.0f } },
		{ {  0.5f, -0.5f, -0.5f }, { 0.0f, 1.0f, 0.0f } },
		{ {  0.5f,  0.5f, -0.5f }, { 0.0f, 0.0f, 1.0f } },
		{ { -0.5f,  0.5f, -0.5f }, { 1.0f, 1.0f, 0.0f } }
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
		glm::vec3(0.0f, -0.5f, -0.5f),
		glm::vec3(0.5f, 0.0f, 0.0f),
		glm::vec3(0.0f, -0.5f, 0.0f),
		glm::vec3(-0.5f, 0.0f, 0.0f),
		glm::vec3(0.0f, 0.5f, 0.0f),
		glm::vec3(0.0f, 0.0f, 0.5f)
	};

	bool faceToggle[6] = { false };
	int lastDisplayFace = -1;

	GLuint VAO, VBO, EBO;

public:
	Cube(GLfloat size = 1.0f);
	void Render();
	void DisplayOnly(int index);
	void DisplayRandom();
};

class ObjPyramid {
protected:
	glm::vec3 original_vertices[16] =
	{
		// 앞면 삼각
		{  0.0f,  0.5f,  0.0f },
		{ -0.5f, -0.5f, 0.5f },
		{  0.5f, -0.5f, 0.5f },

		// 왼쪽면 삼각
		{  0.0f,  0.5f,  0.0f },
		{ -0.5f, -0.5f, -0.5f },
		{ -0.5f, -0.5f, 0.5f },

		// 오른쪽면 삼각
		{  0.0f,  0.5f,  0.0f },
		{  0.5f, -0.5f, 0.5f },
		{  0.5f, -0.5f, -0.5f },

		// 뒷면 삼각
		{  0.0f,  0.5f,  0.0f },
		{  0.5f, -0.5f, -0.5f },
		{ -0.5f, -0.5f, -0.5f },

		// 밑면 사각
		{ -0.5f, -0.5f, 0.5f },
		{ -0.5f, -0.5f, -0.5f },
		{  0.5f, -0.5f, -0.5f },
		{  0.5f, -0.5f, 0.5f },

	};

	glm::vec3 vertices[16];

	glm::vec3 normals[16] =
	{
		// 앞면
		{  0.0f,  1.0f,  1.0f },
		{  0.0f,  0.0f,  1.0f },
		{  0.0f,  1.0f,  1.0f },

		// 왼쪽면
		{ -1.0f,  1.0f,  0.0f },
		{ -1.0f,  1.0f,  0.0f },
		{ -1.0f,  1.0f,  0.0f },

		// 오른쪽면
		{  1.0f,  1.0f,  0.0f },
		{  1.0f,  1.0f,  0.0f },
		{  1.0f,  1.0f,  0.0f },

		// 뒷면
		{  0.0f,  1.0f,  -1.0f },
		{  0.0f,  1.0f,  -1.0f },
		{  0.0f,  1.0f,  -1.0f },

		// 아랫면
		{  0.0f, -1.0f,  0.0f }, {  0.0f, -1.0f,  0.0f },
		{  0.0f, -1.0f,  0.0f }, {  0.0f, -1.0f,  0.0f },
	};

	glm::vec3 colors[16] =
	{
		// 앞면
		{  1.0f,  1.0f,  1.0f },
		{  1.0f,  1.0f,  1.0f },
		{  1.0f,  1.0f,  1.0f },

		// 왼쪽면
		{  1.0f,  1.0f,  1.0f },
		{  1.0f,  1.0f,  1.0f },
		{  1.0f,  1.0f,  1.0f },

		// 오른쪽면
		{  1.0f,  1.0f,  1.0f },
		{  1.0f,  1.0f,  1.0f },
		{  1.0f,  1.0f,  1.0f },

		// 뒷면
		{  0.0f,  1.0f,  1.0f },
		{  0.0f,  1.0f,  1.0f },
		{  0.0f,  1.0f,  1.0f },

		// 아랫면
		{  1.0f,  1.0f,  1.0f }, {  1.0f,  1.0f,  1.0f },
		{  1.0f,  1.0f,  1.0f }, {  1.0f,  1.0f,  1.0f },
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

	GLuint VAO, VERTEX, FACE, COLOR, NORMAL;

	glm::vec3 center;

	// 초기 변환 (생성자에서 설정, 이후 변경되지 않음)
	glm::mat4 initial_scale = glm::mat4(1.0f);
	glm::mat4 initial_rotate = glm::mat4(1.0f);
	glm::mat4 initial_translate = glm::mat4(1.0f);

	// 추가 변환
	std::queue<glm::mat4> transform_queue;
	glm::mat4 additional_transform = glm::mat4(1.0f);

public:

	ObjPyramid(glm::vec3 scaling = { 1.0f, 1.0f, 1.0f }, glm::vec3 rotation = { 0.0f, 0.0f, 0.0f }, glm::vec3 location = { 0.0f, 0.0f, 0.0f }, glm::vec3 color = { 1.0f, 1.0f, 1.0f });

	void scaling(glm::vec3 amount);
	void rotating(glm::vec3 amount);
	void translating(glm::vec3 amount);
	glm::mat4 getModelMatrix();
	glm::vec3 getCenter();

	void Render();

	virtual void reset();
};

class Pyramid {
	ColoredVertex vertices[16] =
	{
		// 앞면 삼각
		{ {  0.0f,  0.5f,  0.0f }, { 1.0f, 1.0f, 0.0f } },
		{ { -0.5f, -0.5f, 0.5f }, { 1.0f, 1.0f, 0.0f } },
		{ {  0.5f, -0.5f, 0.5f }, { 1.0f, 1.0f, 0.0f } },

		// 왼쪽면 삼각
		{ {  0.0f,  0.5f,  0.0f }, { 0.0f, 1.0f, 0.0f } },
		{ { -0.5f, -0.5f, -0.5f }, { 0.0f, 1.0f, 0.0f } },
		{ { -0.5f, -0.5f, 0.5f }, { 0.0f, 1.0f, 0.0f } },

		// 오른쪽면 삼각
		{ {  0.0f,  0.5f,  0.0f }, { 0.0f, 0.0f, 1.0f } },
		{ {  0.5f, -0.5f, 0.5f }, { 0.0f, 0.0f, 1.0f } },
		{ {  0.5f, -0.5f, -0.5f }, { 0.0f, 0.0f, 1.0f } },

		// 뒷면 삼각
		{ {  0.0f,  0.5f,  0.0f }, { 1.0f, 0.0f, 0.0f } },
		{ {  0.5f, -0.5f, -0.5f }, { 1.0f, 0.0f, 0.0f } },
		{ { -0.5f, -0.5f, -0.5f }, { 1.0f, 0.0f, 0.0f } },

		// 밑면 사각
		{ { -0.5f, -0.5f, 0.5f }, { 0.0f, 1.0f, 1.0f } },
		{ { -0.5f, -0.5f, -0.5f }, { 0.0f, 1.0f, 1.0f } },
		{ {  0.5f, -0.5f, -0.5f }, { 0.0f, 1.0f, 1.0f } },
		{ {  0.5f, -0.5f, 0.5f }, { 0.0f, 1.0f, 1.0f } }

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

	glm::vec3 toOrigin[5] = {
		glm::vec3(0.0f, 0.5f, -0.5f),
		glm::vec3(0.5f, 0.5f, 0.0f),
		glm::vec3(-0.5f, 0.5f, 0.0f),
		glm::vec3(0.0f, 0.5f, 0.5f),
		glm::vec3(0.0f, 0.5f, 0.0f)
	};

	bool faceToggle[5] = { false };
	int lastDisplayFace = -1;

	GLuint VAO, VBO, EBO;

public:
	Pyramid(GLfloat size = 1.0f);
	void Render();
	void DisplayOnly(int index);
	void DisplayRandom();
};