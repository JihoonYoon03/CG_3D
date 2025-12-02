//--- 필요한 헤더파일 선언
#include <iostream>
#include <stdlib.h>
#include <stdio.h>	
#include <gl/glew.h>
#include <gl/freeglut.h>
#include <gl/freeglut_ext.h>
#include <gl/glm/glm.hpp>
#include <gl/glm/ext.hpp>
#include <gl/glm/gtc/matrix_transform.hpp>
#include "tools.h"

float vertexData[] = {
	//--- 위치			//--- 노말		//--- 텍스처 좌표
	-0.5f, -0.5f, 0.5f, 0.0, 0.0, 1.0,	0.0, 0.0,
	0.5f, -0.5f, 0.5f,	0.0, 0.0, 1.0,	1.0, 0.0,
	0.5f, 0.5f, 0.5f,	0.0, 0.0, 1.0,	1.0, 1.0,
	0.5f, 0.5f, 0.5f,	0.0, 0.0, 1.0,	1.0, 1.0,
	-0.5f, 0.5f, 0.5f,	0.0, 0.0, 1.0,	0.0, 1.0,
	-0.5f, -0.5f, 0.5f, 0.0, 0.0, 1.0,	0.0, 0.0 };

float vertexDataTri[] = {
	0.0f, 0.5f, 0.0f,	0.0, 0.0, 1.0, 0.5f, 1.0f,
	-0.5f, -0.5f, 0.5f,	0.0, 0.0, 1.0, 0.0f, 0.0f,
	0.5f, -0.5f, 0.5f,	0.0, 0.0, 1.0, 1.0f, 0.0f,
};

class Plane {
	GLuint VAO, VBO;
	float* data;
public:
	glm::mat4 modelMatrix = glm::mat4(1.0f);
	Plane(float* vertexData) : data(vertexData){
		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);

		glBindVertexArray(VAO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, 6 * 8 * sizeof(float), data, GL_STATIC_DRAW);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (GLvoid*)0);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (GLvoid*)(sizeof(float) * 3));
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (GLvoid*)(sizeof(float) * 6));
		glEnableVertexAttribArray(3);
	}

	void Render() {
		glBindVertexArray(VAO);
		glDrawArrays(GL_TRIANGLES, 0, 6);
	}
};

class Triangle {
	GLuint VAO, VBO;
	float* data;
public:
	glm::mat4 modelMatrix = glm::mat4(1.0f);
	Triangle(float* Data) : data(Data) {
		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);

		glBindVertexArray(VAO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, 6 * 8 * sizeof(float), data, GL_STATIC_DRAW);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (GLvoid*)0);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (GLvoid*)(sizeof(float) * 3));
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (GLvoid*)(sizeof(float) * 6));
		glEnableVertexAttribArray(3);
	}

	void Render() {
		glBindVertexArray(VAO);
		glDrawArrays(GL_TRIANGLES, 0, 3);
	}
};


//--- 아래 5개 함수는 사용자 정의 함수 임
GLvoid drawScene();
GLvoid Reshape(int w, int h);
GLvoid Keyboard(unsigned char key, int x, int y);
GLvoid TimerFunc(int value);

//--- 필요한 변수 선언
GLint winWidth = 600, winHeight = 600;
GLuint shaderProgramID; //--- 세이더 프로그램 이름
GLuint vertexShader; //--- 버텍스 세이더 객체
GLuint fragmentShader; //--- 프래그먼트 세이더 객체
Model* test;
DisplayBasis* xyz;

GLfloat rotX = 0.0f, rotY = 0.0f, dX = 0.0f, dY = 0.0f;

glm::vec3 bgColor = { 0.1f, 0.1f, 0.1f };
glm::vec3 lightColor = { 1.0f, 1.0f, 1.0f };
glm::vec3 lightPos = { 0.0f, 2.0f, 2.0f };
glm::vec3 camPos = { 3.0f, 1.0f, 3.0f };
glm::vec3 camAt = { 0.0f, 0.0f, 0.0f };
glm::vec3 camUp = { 0.0f, -1.0f, 0.0f };

Plane* cube[6] = { nullptr, nullptr, nullptr, nullptr, nullptr, nullptr };
Triangle* pyramidSide[4] = { nullptr, nullptr, nullptr, nullptr };
Plane* pyramidBottom = nullptr;
Model* skydome = nullptr;
GLuint TEX_ID[6]{ 0, 0, 0, 0, 0, 0 };
bool isCube = false, isPyramid = false;

//--- 메인 함수
void main(int argc, char** argv) //--- 윈도우 출력하고 콜백함수 설정
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(winWidth, winHeight);
	glutCreateWindow("Example1");

	glewExperimental = GL_TRUE;
	glewInit();

	make_vertexShaders(vertexShader, "vertex_prac29.glsl");
	make_fragmentShaders(fragmentShader, "fragment_prac29.glsl");
	shaderProgramID = make_shaderProgram(vertexShader, fragmentShader);

	glEnable(GL_DEPTH_TEST);
	glFrontFace(GL_CCW);
	glEnable(GL_CULL_FACE);

	// 데이터 초기화
	skydome = new Model("Models/sphere.obj", { 100.0f, 100.0f, 100.0f }, { 1.0f, 1.0f, 1.0f }, NONE, "Models/SkydomeTex.png");

	for (int i = 0; i < 6; i++) {
		std::string texFile = "Models/" + std::to_string(i + 1) + ".png";
		TEX_ID[i] = loadTexture(texFile);

		cube[i] = new Plane(vertexData);
	}

	for (int i = 0; i < 4; i++) {
		pyramidSide[i] = new Triangle(vertexDataTri);
		pyramidSide[i]->modelMatrix = glm::rotate(pyramidSide[i]->modelMatrix, glm::radians(90.0f * i), glm::vec3(0.0f, 1.0f, 0.0f));
	}

	cube[1]->modelMatrix = glm::rotate(cube[1]->modelMatrix, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	cube[2]->modelMatrix = glm::rotate(cube[2]->modelMatrix, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	cube[3]->modelMatrix = glm::rotate(cube[3]->modelMatrix, glm::radians(270.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	cube[4]->modelMatrix = glm::rotate(cube[4]->modelMatrix, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	cube[5]->modelMatrix = glm::rotate(cube[5]->modelMatrix, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	pyramidBottom = new Plane(vertexData);
	pyramidBottom->modelMatrix = glm::rotate(pyramidBottom->modelMatrix, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));


	glutDisplayFunc(drawScene);
	glutReshapeFunc(Reshape);
	glutKeyboardFunc(Keyboard);
	glutTimerFunc(1000 / 60, TimerFunc, 1);
	glutMainLoop();
}

GLvoid drawScene()
{
	glClearColor(bgColor.x, bgColor.y, bgColor.z, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glUseProgram(shaderProgramID);

	glUniform3fv(glGetUniformLocation(shaderProgramID, "lightColor"), 1, glm::value_ptr(lightColor));
	glUniform3fv(glGetUniformLocation(shaderProgramID, "lightPos"), 1, glm::value_ptr(lightPos));
	glUniform3fv(glGetUniformLocation(shaderProgramID, "viewPos"), 1, glm::value_ptr(camPos));

	glm::mat4 projection = glm::perspective(55.0f, 1.0f, 0.1f, 150.0f);
	glm::mat4 view = glm::lookAt(camPos, camAt, camUp);
	glUniformMatrix4fv(glGetUniformLocation(shaderProgramID, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
	glUniformMatrix4fv(glGetUniformLocation(shaderProgramID, "view"), 1, GL_FALSE, glm::value_ptr(view));

	// 스카이돔 렌더링
	glDisable(GL_CULL_FACE); // 스카이돔은 안쪽 면을 봐야 하므로 면 제거 비활성화

	glUniformMatrix4fv(glGetUniformLocation(shaderProgramID, "rotation"), 1, GL_FALSE, glm::value_ptr(glm::mat4(1.0f)));

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, skydome->getTexture());

	glUniform1i(glGetUniformLocation(shaderProgramID, "tex"), 0);
	glUniform1i(glGetUniformLocation(shaderProgramID, "useTexture"), 1);
	glUniform1i(glGetUniformLocation(shaderProgramID, "isBackground"), 1);
	glUniformMatrix4fv(glGetUniformLocation(shaderProgramID, "model"), 1, GL_FALSE, glm::value_ptr(skydome->getModelMatrix()));
	skydome->Render();

	glBindTexture(GL_TEXTURE_2D, 0);
	glUniform1i(glGetUniformLocation(shaderProgramID, "useTexture"), 0);
	glUniform1i(glGetUniformLocation(shaderProgramID, "isBackground"), 0);

	glEnable(GL_CULL_FACE); // 면 제거 다시 활성화

	glm::mat4 rotate = glm::rotate(glm::rotate(glm::mat4(1.0f), glm::radians(rotX), glm::vec3(1.0f, 0.0f, 0.0f)), glm::radians(rotY), glm::vec3(0.0f, 1.0f, 0.0f));
	glUniformMatrix4fv(glGetUniformLocation(shaderProgramID, "rotation"), 1, GL_FALSE, glm::value_ptr(rotate));

	glUniform1i(glGetUniformLocation(shaderProgramID, "useTexture"), 1);
	glUniform1i(glGetUniformLocation(shaderProgramID, "isBackground"), 0);

	if (isPyramid) {
		for (int i = 0; i < 4; i++) {
			glUniformMatrix4fv(glGetUniformLocation(shaderProgramID, "model"), 1, GL_FALSE, glm::value_ptr(pyramidSide[i]->modelMatrix));
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, TEX_ID[i]);
			pyramidSide[i]->Render();
		}
		glUniformMatrix4fv(glGetUniformLocation(shaderProgramID, "model"), 1, GL_FALSE, glm::value_ptr(pyramidBottom->modelMatrix));
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, TEX_ID[5]);
		pyramidBottom->Render();
	}
	else {
		for (int i = 0; i < 6; i++) {
			glUniformMatrix4fv(glGetUniformLocation(shaderProgramID, "model"), 1, GL_FALSE, glm::value_ptr(cube[i]->modelMatrix));
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, TEX_ID[i]);
			cube[i]->Render();
		}
	}


	glutSwapBuffers();
}

GLvoid Reshape(int w, int h)
{
	glViewport(0, 0, w, h);
	winWidth = w;
	winHeight = h;
}

GLvoid Keyboard(unsigned char key, int x, int y)
{
	switch (key) {
	case 'c':
		isPyramid = false;
		break;
	case 'p':
		isPyramid = true;
		break;
	case 'x':
		if (dX == 0.0f)
			dX = 0.5f;
		else
			dX = 0.0f;
		break;
	case 'X':
		if (dX == 0.0f)
			dX = -0.5f;
		else
			dX = 0.0f;
		break;
	case 'y':
		if (dY == 0.0f)
			dY = 0.5f;
		else
			dY = 0.0f;
		break;
	case 'Y':
		if (dY == 0.0f)
			dY = -0.5f;
		else
			dY = 0.0f;
		break;
	case 's':
		dX = 0.0f;
		dY = 0.0f;
		rotX = 0.0f;
		rotY = 0.0f;
		break;
	case 'q':
		exit(0);
		break;
	}
}

GLvoid TimerFunc(int value) {
	rotX += dX;
	rotY += dY;
	if (rotX > 360.0f) rotX -= 360.0f;
	if (rotX < -360.0f) rotX += 360.0f;
	if (rotY > 360.0f) rotY -= 360.0f;
	if (rotY < -360.0f) rotY += 360.0f;
	glutPostRedisplay();
	glutTimerFunc(1000 / 60, TimerFunc, 1);
}