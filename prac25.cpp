//--- 필요한 헤더파일 선언
#include <iostream>
#include <stdlib.h>
#include <stdio.h>	
#include <string>
#include <gl/glew.h>
#include <gl/freeglut.h>
#include <gl/freeglut_ext.h>
#include <gl/glm/glm.hpp>
#include <gl/glm/ext.hpp>
#include <gl/glm/gtc/matrix_transform.hpp>
#include "tools.h"
#include "object.h"

//--- 아래 5개 함수는 사용자 정의 함수 임
GLvoid drawScene();
GLvoid Reshape(int w, int h);
GLvoid Keyboard(unsigned char key, int x, int y);
GLvoid Timer(int value);

//--- 필요한 변수 선언
GLint winWidth = 600, winHeight = 600;
GLuint shaderProgramID; //--- 세이더 프로그램 이름
GLuint vertexShader; //--- 버텍스 세이더 객체
GLuint fragmentShader; //--- 프래그먼트 세이더 객체

glm::vec3 bgColor = { 0.05f, 0.05f, 0.05f };
ObjCube* cube;
ObjCube* light;
ObjPyramid* pyramid;
DisplayBasis* d_basis;

glm::vec3 lightColor{ 1.0f, 1.0f, 1.0f };
glm::vec3 lightPos{ 0.0f, 0.0f, 2.0f };
GLfloat shininess = 32.0f;
glm::vec3 viewPos{ 5.0f, 5.0f, 5.0f };


GLfloat dyRotObj = 2.0f, dyRotLight = 2.0f;
bool displayCube = true;

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
	make_vertexShaders(vertexShader, "vertex_prac26.glsl"); //--- 버텍스 세이더 만들기
	make_fragmentShaders(fragmentShader, "fragment_prac26.glsl"); //--- 프래그먼트 세이더 만들기
	shaderProgramID = make_shaderProgram(vertexShader, fragmentShader);

	// 데이터 초기화
	cube = new ObjCube();
	d_basis = new DisplayBasis();
	pyramid = new ObjPyramid();
	light = new ObjCube(glm::vec3(0.2f, 0.2f, 0.2f), glm::vec3(0.0), lightPos);

	// 컬링 관련 설정
	glEnable(GL_DEPTH_TEST);

	//--- 세이더 프로그램 만들기
	glutDisplayFunc(drawScene); //--- 출력 콜백 함수
	glutReshapeFunc(Reshape);
	glutKeyboardFunc(Keyboard);
	glutTimerFunc(1000 / 60, Timer, 1);
	glutMainLoop();
	delete cube;
	delete pyramid;
	delete d_basis;
}

//--- 출력 콜백 함수
GLvoid drawScene() //--- 콜백 함수: 그리기 콜백 함수
{
	glClearColor(bgColor.x, bgColor.y, bgColor.z, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glUseProgram(shaderProgramID);

	glUniform3f(glGetUniformLocation(shaderProgramID, "lightColor"), lightColor.r, lightColor.g, lightColor.b);
	glUniform3f(glGetUniformLocation(shaderProgramID, "lightPos"), lightPos.x, lightPos.y, lightPos.z);
	glUniform1f(glGetUniformLocation(shaderProgramID, "shininess"), shininess);
	glUniform3f(glGetUniformLocation(shaderProgramID, "viewPos"), viewPos.x, viewPos.y, viewPos.z);

	glm::mat4 projection;
	projection = glm::perspective(glm::radians(45.0f), 1.0f, 0.1f, 100.0f);
	glUniformMatrix4fv(glGetUniformLocation(shaderProgramID, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

	glm::vec3 EYE = viewPos;
	glm::vec3 AT = glm::vec3(0.0f, 0.0f, 0.0f);

	glm::mat4 view = glm::lookAt(EYE, AT, glm::vec3(0.0f, 1.0f, 0.0f));
	glUniformMatrix4fv(glGetUniformLocation(shaderProgramID, "view"), 1, GL_FALSE, glm::value_ptr(view));

	glUniformMatrix4fv(glGetUniformLocation(shaderProgramID, "model"), 1, GL_FALSE, glm::value_ptr(glm::mat4(1.0f)));
	d_basis->Render();

	glUniformMatrix4fv(glGetUniformLocation(shaderProgramID, "model"), 1, GL_FALSE, glm::value_ptr(light->getModelMatrix()));
	light->Render();

	if (displayCube) {
		glUniformMatrix4fv(glGetUniformLocation(shaderProgramID, "model"), 1, GL_FALSE, glm::value_ptr(cube->getModelMatrix()));
		cube->Render();
	}
	else {
		glUniformMatrix4fv(glGetUniformLocation(shaderProgramID, "model"), 1, GL_FALSE, glm::value_ptr(pyramid->getModelMatrix()));
		pyramid->Render();
	}

	glutSwapBuffers(); // 화면에 출력하기
}

//--- 다시그리기 콜백 함수
GLvoid Reshape(int w, int h) //--- 콜백 함수: 다시 그리기 콜백 함수
{
	glViewport(0, 0, w, h);
	winWidth = w;
	winHeight = h;
}

GLvoid Keyboard(unsigned char key, int x, int y)
{
	switch (key) {
	case 'p':
		displayCube = !displayCube;
		break;
	case 'y':
		if (displayCube)
			cube->rotating(dyRotObj * glm::vec3(0.0f, 1.0f, 0.0f));
		else
			pyramid->rotating(dyRotObj * glm::vec3(0.0f, 1.0f, 0.0f));
		break;
	case 'r':
		light->rotating(dyRotLight * glm::vec3(0.0f, 1.0f, 0.0f));
		break;
	case 'q':
		exit(0);
		break;
	}
}

GLvoid Timer(int value)
{
	glutPostRedisplay();
	glutTimerFunc(1000 / 60, Timer, 1);
}