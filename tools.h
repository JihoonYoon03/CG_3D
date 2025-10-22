#pragma once
#include <iostream>
#include <gl/glew.h>
#include <gl/freeglut.h>
#include <gl/freeglut_ext.h>
#include <gl/glm/glm.hpp>
#include <gl/glm/ext.hpp>
#include <gl/glm/gtc/matrix_transform.hpp>
#include <string>

struct rtPos {
	GLfloat x1, y1, x2, y2;
};

struct ColoredVertex {
	glm::vec3 pos;
	glm::vec3 color;
};

class DisplayBasis {
	ColoredVertex xyz[3][2] =
	{
	{ { { -1.0f, 0.0f, 0.0f }, { 0.1f, 0.1f, 0.0f } }, { { 1.0f, 0.0f, 0.0f }, { 1.0f, 0.0f, 0.0f } } },
	{ { { 0.0f, -1.0f, 0.0f }, { 0.0f, 0.1f, 0.1f } }, { { 0.0f, 1.0f, 0.0f }, { 0.0f, 1.0f, 0.0f } } },
	{ { { 0.0f, 0.0f, -1.0f }, { 0.1f, 0.0f, 0.1f } }, { { 0.0f, 0.0f, 1.0f }, { 0.0f, 0.0f, 1.0f } } }
	};

	GLuint VAO, VBO;
public:
	DisplayBasis(GLfloat offset = 1.0f);
	void Render();
};

glm::vec3 randColor();

void mPosToGL(GLuint winWidth, GLuint winHeight, int mx, int my, GLfloat& xGL, GLfloat& yGL);
bool isMouseIn(rtPos& pos, GLuint winWidth, GLuint winHeight, int mx, int my);
bool CircleCollider(const glm::vec3& center, GLfloat distCap, GLfloat xGL, GLfloat yGL);
bool LineCollider(const glm::vec3& p1, const glm::vec3& p2, GLfloat distCap, GLfloat xGL, GLfloat yGL);
bool RectCollider(const glm::vec3& p1, const glm::vec3& p2, GLfloat xGL, GLfloat yGL);

char* filetobuf(const char* file);
void make_vertexShaders(GLuint& vertexShader, const std::string& shaderName);
void make_fragmentShaders(GLuint& fragmentShader, const std::string& shaderName);
GLuint make_shaderProgram(const GLuint& vertexShader, const GLuint& fragmentShader);