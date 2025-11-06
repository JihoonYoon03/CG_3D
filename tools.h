#pragma once
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include <queue>
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

class DisplayBasis;

class Model {
	std::vector<glm::vec3> vertices;
	std::vector<glm::uvec3> faces;
	glm::vec3 center;
	DisplayBasis* basis;

	// 변환행렬 적용 이전에 SRT순으로 모델 변환
	glm::mat4 default_scale = glm::mat4(1.0f);
	glm::mat4 default_rotate = glm::mat4(1.0f);
	glm::mat4 default_translate = glm::mat4(1.0f);

	// 로컬 모델 변환
	glm::mat4 modelMatrix = glm::mat4(1.0f);
	// modelMatrix에 적용할 변환 행렬 큐
	std::queue<glm::mat4> transformQueue;

	GLuint VAO, VBO, EBO;

	// 비활성 상태에선 동작 X
	bool enabled = true;
public:
	Model(const std::string& filename, const glm::vec3& size = { 1.0f, 1.0f, 1.0f });

	void setDefScale(const glm::vec3& ds);
	void setDefRotate(const glm::mat4& dr);
	void setDefTranslate(const glm::vec3& dt);
	
	void scale(const glm::vec3& scaleFactor);
	void rotate(GLfloat angle, const glm::vec3& axis);
	void translate(const glm::vec3& delta);

	void Render();
	void resetModelMatrix() { modelMatrix = glm::mat4(1.0f); }
	glm::vec3 retDistTo(const glm::vec3& origin = { 0.0f, 0.0f, 0.0f });
	glm::mat4 getModelMatrix();

	void setEnabled(bool state) { enabled = state; }
	
	~Model() { delete basis; }
};

class DisplayBasis {
	glm::vec3 origin;
	ColoredVertex xyz[3][2];

	GLuint VAO, VBO;
public:
	DisplayBasis(GLfloat offset = 1.0f, const glm::vec3& origin = { 0.0f, 0.0f, 0.0f });
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