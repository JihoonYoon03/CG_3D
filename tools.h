#pragma once
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include <gl/glew.h>
#include <gl/freeglut.h>
#include <gl/freeglut_ext.h>
#include <gl/glm/glm.hpp>
#include <gl/glm/ext.hpp>
#include <gl/glm/gtc/matrix_transform.hpp>

struct rtPos {
	GLfloat x1, y1, x2, y2;
};

struct ColoredVertex {
	glm::vec3 pos;
	glm::vec3 color;
};

struct Model {
	std::vector<glm::vec3> vertices;
	std::vector<glm::uvec3> faces;
};

class DisplayBasis {
	ColoredVertex xyz[3][2] =
	{
	{ { { 0.0f, 0.0f, 0.0f }, { 0.1f, 0.1f, 0.0f } }, { { 1.0f, 0.0f, 0.0f }, { 1.0f, 0.0f, 0.0f } } },
	{ { { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.1f, 0.1f } }, { { 0.0f, 1.0f, 0.0f }, { 0.0f, 1.0f, 0.0f } } },
	{ { { 0.0f, 0.0f, 0.0f }, { 0.1f, 0.0f, 0.1f } }, { { 0.0f, 0.0f, 1.0f }, { 0.0f, 0.0f, 1.0f } } }
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

std::string read_file(const std::string& filename);
void make_vertexShaders(GLuint& vertexShader, const std::string& shaderName);
void make_fragmentShaders(GLuint& fragmentShader, const std::string& shaderName);
GLuint make_shaderProgram(const GLuint& vertexShader, const GLuint& fragmentShader);
void read_obj_file(const std::string& filename, Model& model);