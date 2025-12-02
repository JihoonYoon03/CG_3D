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

//--- 아래 5개 함수는 사용자 정의 함수 임
GLvoid drawScene();
GLvoid Reshape(int w, int h);
GLvoid Keyboard(unsigned char key, int x, int y);

//--- 필요한 변수 선언
GLint winWidth = 600, winHeight = 600;
GLuint shaderProgramID; //--- 세이더 프로그램 이름
GLuint vertexShader; //--- 버텍스 세이더 객체
GLuint fragmentShader; //--- 프래그먼트 세이더 객체
Model* test;
DisplayBasis* xyz;

glm::vec3 bgColor = { 0.1f, 0.1f, 0.1f };
glm::vec3 lightColor = { 1.0f, 1.0f, 1.0f };
glm::vec3 lightPos = { 0.0f, 2.0f, 2.0f };
glm::vec3 camPos = { 3.0f, 3.0f, 3.0f };
glm::vec3 camAt = { 0.0f, 0.0f, 0.0f };
glm::vec3 camUp = { 0.0f, -1.0f, 0.0f };

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
	test = new Model("Models/Cube.obj");
	xyz = new DisplayBasis(2.0f);

	glutDisplayFunc(drawScene);
	glutReshapeFunc(Reshape);
	glutKeyboardFunc(Keyboard);
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

	glm::mat4 projection = glm::perspective(55.0f, 1.0f, 0.1f, 100.0f);
	glm::mat4 view = glm::lookAt(camPos, camAt, camUp);
	glUniformMatrix4fv(glGetUniformLocation(shaderProgramID, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
	glUniformMatrix4fv(glGetUniformLocation(shaderProgramID, "view"), 1, GL_FALSE, glm::value_ptr(view));

	glUniform1i(glGetUniformLocation(shaderProgramID, "useTexture"), 0);
	glUniform1i(glGetUniformLocation(shaderProgramID, "isBackground"), 0);
	glUniformMatrix4fv(glGetUniformLocation(shaderProgramID, "model"), 1, GL_FALSE, glm::value_ptr(test->getModelMatrix()));
	test->Render();

	glUniformMatrix4fv(glGetUniformLocation(shaderProgramID, "model"), 1, GL_FALSE, glm::value_ptr(glm::mat4(1.0f)));
	xyz->Render();

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
	case 'q':
		exit(0);
		break;
	}
}