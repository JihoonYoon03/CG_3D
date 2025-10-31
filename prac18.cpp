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
GLvoid MouseMotion(int x, int y);

//--- 필요한 변수 선언
GLint winWidth = 600, winHeight = 600;
GLuint shaderProgramID; //--- 세이더 프로그램 이름
GLuint vertexShader; //--- 버텍스 세이더 객체
GLuint fragmentShader; //--- 프래그먼트 세이더 객체
Model* test;

glm::vec3 bgColor = { 0.1f, 0.1f, 0.1f };
GLfloat viewX = 0.0f, viewY = 0.0f;
bool cursorDisabled = false;

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

	make_vertexShaders(vertexShader, "vertex_prac18.glsl");
	make_fragmentShaders(fragmentShader, "fragment_prac18.glsl");
	shaderProgramID = make_shaderProgram(vertexShader, fragmentShader);

	glEnable(GL_DEPTH_TEST);

	// 데이터 초기화
	test = new Model("Models/test.obj");

	glutDisplayFunc(drawScene);
	glutReshapeFunc(Reshape);
	glutKeyboardFunc(Keyboard);
	glutPassiveMotionFunc(MouseMotion);
	glutMainLoop();
}

GLvoid drawScene()
{
	glClearColor(bgColor.x, bgColor.y, bgColor.z, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glUseProgram(shaderProgramID);

	glm::mat4 projection = glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f, 0.1f, 100.0f);
	glm::mat4 view = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -3.0f));
	view = glm::rotate(view, glm::radians(20.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	view = glm::rotate(view, glm::radians(20.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	if (cursorDisabled)
	{
		view = glm::rotate(view, glm::radians(viewX), glm::vec3(0.0f, 1.0f, 0.0f));
		view = glm::rotate(view, glm::radians(viewY), glm::vec3(1.0f, 0.0f, 0.0f));
	}
	glm::mat4 model = glm::scale(glm::mat4(1.0f), glm::vec3(0.2f, 0.2f, 0.2f));

	glUniformMatrix4fv(glGetUniformLocation(shaderProgramID, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
	glUniformMatrix4fv(glGetUniformLocation(shaderProgramID, "view"), 1, GL_FALSE, glm::value_ptr(view));
	glUniformMatrix4fv(glGetUniformLocation(shaderProgramID, "model"), 1, GL_FALSE, glm::value_ptr(model));

	glPointSize(10.0f);
	glDrawArrays(GL_POINTS, 0, 1);

	test->Render();

	glutSwapBuffers();
}

GLvoid Reshape(int w, int h)
{
	glViewport(0, 0, w, h);
	winWidth = w;
	winHeight = h;
}

GLvoid MouseMotion(int x, int y)
{
	if (cursorDisabled == false)
		return;

	viewX += (x - winWidth / 2) * 0.2f;
	viewY += (y - winHeight / 2) * 0.2f;

	if (viewY > 89.0f) viewY = 89.0f;
	if (viewY < -89.0f) viewY = -89.0f;

	glutWarpPointer(winWidth / 2, winHeight / 2);
	glutPostRedisplay();
}

GLvoid Keyboard(unsigned char key, int x, int y)
{
	switch (key) {
	case 'm':
		if (cursorDisabled) {
			cursorDisabled = false;
			glutSetCursor(GLUT_CURSOR_INHERIT);
		}
		else {
			cursorDisabled = true;
			glutSetCursor(GLUT_CURSOR_NONE);
		}
		break;
	case 'q':
		exit(0);
		break;
	}
}