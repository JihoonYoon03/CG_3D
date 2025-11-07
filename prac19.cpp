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
GLvoid TimerFunc(int value);

//--- 필요한 변수 선언
GLint winWidth = 600, winHeight = 600;
GLuint shaderProgramID; //--- 세이더 프로그램 이름
GLuint vertexShader; //--- 버텍스 세이더 객체
GLuint fragmentShader; //--- 프래그먼트 세이더 객체

DisplayBasis* XYZ;

glm::vec3 bgColor = { 0.1f, 0.1f, 0.1f };

// 변환 수치

// 수치 변화량


//--- 메인 함수
void main(int argc, char** argv) //--- 윈도우 출력하고 콜백함수 설정
{
	srand(static_cast<unsigned int>(time(0)));

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
	XYZ = new DisplayBasis(1.0f);

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

	glm::mat4 projection = glm::perspective(glm::radians(45.0f), 1.0f, 0.1f, 100.0f);
	glUniformMatrix4fv(glGetUniformLocation(shaderProgramID, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

	glm::mat4 view = glm::lookAt(glm::vec3(0.0f, 0.0f, 5.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	glUniformMatrix4fv(glGetUniformLocation(shaderProgramID, "view"), 1, GL_FALSE, glm::value_ptr(view));

	glm::mat4 world = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));
	world = glm::rotate(world, glm::radians(30.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	world = glm::rotate(world, glm::radians(-30.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	glUniformMatrix4fv(glGetUniformLocation(shaderProgramID, "world"), 1, GL_FALSE, glm::value_ptr(world));


	glUniform1i(glGetUniformLocation(shaderProgramID, "use_color_set"), false);
	glUniform1i(glGetUniformLocation(shaderProgramID, "isBasis"), true);
	XYZ->Render();
	glUniform1i(glGetUniformLocation(shaderProgramID, "isBasis"), false);

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
	case 'p': case 'P':
		// 직각/원근 투영
		break;
	case 'm': case 'M':
		// 솔리드/와이어
		break;
	case 'w': case 'a': case 's': case 'd':
		// 도형 좌우상하 이동(x,y)
		break;
	case '+': case '-':
		// 도형 앞뒤로 이동(z)
		break;
	case 'y': case 'Y':
		// 궤도 반지름 증감
		break;
	case 'z': case 'Z':
		// 중심 제외 z축 회전
		break;
	case 'q':
		exit(0);
		break;
	}
}

GLvoid TimerFunc(int value)
{
	glutPostRedisplay();
	glutTimerFunc(1000 / 60, TimerFunc, 1);
}