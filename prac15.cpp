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
GLvoid SpecialKey(int key, int x, int y);
GLvoid SpecialKeyUp(int key, int x, int y);
GLvoid Timer(int value);

//--- 필요한 변수 선언
GLint winWidth = 600, winHeight = 600;
GLuint shaderProgramID; //--- 세이더 프로그램 이름
GLuint vertexShader; //--- 버텍스 세이더 객체
GLuint fragmentShader; //--- 프래그먼트 세이더 객체

glm::vec3 bgColor = { 0.95f, 0.95f, 0.95f };
Cube* cube;
DisplayBasis* d_basis;

GLfloat xRot = -30.0f, yRot = -30.0f, dxRot = 0.0f, dyRot = 0.0f; // 월드 회전각

//--- 메인 함수
void main(int argc, char** argv) //--- 윈도우 출력하고 콜백함수 설정
{
	//--- 윈도우 생성하기
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(winWidth, winHeight);
	glutCreateWindow("Example1");

	//--- GLEW 초기화하기
	glewExperimental = GL_TRUE;
	glewInit();

	//--- 세이더 읽어와서 세이더 프로그램 만들기: 사용자 정의함수 호출
	make_vertexShaders(vertexShader, "vertex.glsl"); //--- 버텍스 세이더 만들기
	make_fragmentShaders(fragmentShader, "fragment.glsl"); //--- 프래그먼트 세이더 만들기
	shaderProgramID = make_shaderProgram(vertexShader, fragmentShader);

	// 좌표축 디스플레이 초기화
	cube = new Cube();
	d_basis = new DisplayBasis();

	// 컬링 관련 설정
	glEnable(GL_DEPTH_TEST);
	glFrontFace(GL_CW);

	//--- 세이더 프로그램 만들기
	glutDisplayFunc(drawScene); //--- 출력 콜백 함수
	glutReshapeFunc(Reshape);
	glutKeyboardFunc(Keyboard);
	glutSpecialFunc(SpecialKey);
	glutSpecialUpFunc(SpecialKeyUp);
	glutTimerFunc(1000 / 60, Timer, 1);
	glutMainLoop();
	delete cube;
	delete d_basis;
}

//--- 출력 콜백 함수
GLvoid drawScene() //--- 콜백 함수: 그리기 콜백 함수
{
	glClearColor(bgColor.x, bgColor.y, bgColor.z, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glUseProgram(shaderProgramID);

	d_basis->Render();
	cube->Render();

	glutSwapBuffers(); // 화면에 출력하기
}

//--- 다시그리기 콜백 함수
GLvoid Reshape(int w, int h) //--- 콜백 함수: 다시 그리기 콜백 함수
{
	glViewport(0, 0, w, h);
}

GLvoid Keyboard(unsigned char key, int x, int y)
{
	if ('1' <= key && key <= '6') {
		cube->DisplayOnly(key - '1');
		return;
	}
	switch (key) {
	case 'r':
		xRot = -30;
		yRot = -30;
		glm::mat4 rotation = glm::mat4(1.0f);
		rotation = glm::rotate(rotation, glm::radians(xRot), glm::vec3(1.0f, 0.0f, 0.0f));
		rotation = glm::rotate(rotation, glm::radians(yRot), glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(glGetUniformLocation(shaderProgramID, "rotation"), 1, GL_FALSE, glm::value_ptr(rotation));
		break;
	case 'q':
		exit(0);
		break;
	}
}

GLvoid SpecialKey(int key, int x, int y)
{
	switch (key) {
	case GLUT_KEY_LEFT:
		if (dyRot < 1.0f) dyRot += 1.0f;
		break;
	case GLUT_KEY_RIGHT:
		if (dyRot > -1.0f) dyRot -= 1.0f;
		break;
	case GLUT_KEY_UP:
		if (dxRot < 1.0f) dxRot += 1.0f;
		break;
	case GLUT_KEY_DOWN:
		if (dxRot > -1.0f) dxRot -= 1.0f;
		break;
	}
}

GLvoid SpecialKeyUp(int key, int x, int y)
{
	switch (key) {
	case GLUT_KEY_LEFT:
		if (dyRot > 0.0f) dyRot -= 1.0f;
		break;
	case GLUT_KEY_RIGHT:
		if (dyRot < 0.0f) dyRot += 1.0f;
		break;
	case GLUT_KEY_UP:
		if (dxRot > 0.0f) dxRot -= 1.0f;
		break;
	case GLUT_KEY_DOWN:
		if (dxRot < 0.0f) dxRot += 1.0f;
		break;
	}
}

GLvoid Timer(int value)
{
	xRot += dxRot;
	yRot += dyRot;
	glm::mat4 rotation = glm::mat4(1.0f);
	rotation = glm::rotate(rotation, glm::radians(xRot), glm::vec3(1.0f, 0.0f, 0.0f));
	rotation = glm::rotate(rotation, glm::radians(yRot), glm::vec3(0.0f, 1.0f, 0.0f));
	glUniformMatrix4fv(glGetUniformLocation(shaderProgramID, "rotation"), 1, GL_FALSE, glm::value_ptr(rotation));
	glutPostRedisplay();
	glutTimerFunc(1000 / 60, Timer, 1);
}