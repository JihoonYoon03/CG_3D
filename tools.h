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

class Model {
	std::vector<glm::vec3> vertices;
	std::vector<glm::uvec3> faces;

	glm::mat4 modelMatrix = glm::mat4(1.0f);

	// 로컬 모델 변환값 (모델 원점 기준 변환)
	glm::vec3 position = glm::vec3(0.0f);
	glm::vec4 rotation = glm::vec4(0.0f, 0.0f, 1.0f, 0.0f);
	glm::vec3 scale = glm::vec3(1.0f);

	// 누적 변환 행렬 (월드 원점 기준 변환)
	glm::mat4 modelMatrixDelta = glm::mat4(1.0f);

	GLuint VAO, VBO, EBO;
public:
	Model(const std::string& filename);

	void scaleModel(const glm::vec3& scaleFactor);
	void rotateModel(GLfloat angle, const glm::vec3 axis);
	void translateModel(const glm::vec3& delta);

	void instantScale(const glm::vec3& scale);
	void instantRotate(GLfloat angle, const glm::vec3& axis);
	void instantTranslate(const glm::vec3& delta);

	void Render();
	void resetModelMatrix() { modelMatrix = glm::mat4(1.0f); }
	glm::mat4 getModelMatrix();
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