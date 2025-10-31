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
Cube* cube;
Pyramid* pyramid;
DisplayBasis* d_basis;
bool depthTest = true, displayCube = true, animate[11] = { false };

GLfloat xRot = 30.0f, yRot = 30.0f, dxRot = 0.0f, dyRot = 0.0f;
GLfloat animateOffset[11] = { 0.0f }, deltaOffset[11] = { 0.0f };

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
	make_vertexShaders(vertexShader, "vertex_prac17.glsl"); //--- 버텍스 세이더 만들기
	make_fragmentShaders(fragmentShader, "fragment_prac17.glsl"); //--- 프래그먼트 세이더 만들기
	shaderProgramID = make_shaderProgram(vertexShader, fragmentShader);

	// 데이터 초기화
	cube = new Cube();
	d_basis = new DisplayBasis();
	pyramid = new Pyramid();
	for (int i = 0; i < 11; i++)
		deltaOffset[i] = 1.0f;

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

	// 투영 행렬과 뷰 행렬 설정
	glm::mat4 projection = glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f, 0.1f, 100.0f);
	glm::mat4 view = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -3.0f));

	glUniformMatrix4fv(glGetUniformLocation(shaderProgramID, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
	glUniformMatrix4fv(glGetUniformLocation(shaderProgramID, "view"), 1, GL_FALSE, glm::value_ptr(view));

	for (int i = 0; i < 11; i++) {

		std::string matName = "faceModify[" + std::to_string(i) + "]";
		GLuint matLoc = glGetUniformLocation(shaderProgramID, matName.c_str());
		glm::mat4 modify = glm::mat4(1.0f);

		switch (i) {
		case 0:
			modify = glm::rotate(modify, glm::radians(-animateOffset[i]), glm::vec3(1.0f, 0.0f, 0.0f));
			break;
		case 1: case 3:
			modify = glm::rotate(modify, glm::radians(animateOffset[i]), glm::vec3(1.0f, 0.0f, 0.0f));
			break;
		case 2:
			modify = glm::rotate(modify, glm::radians(animateOffset[i]), glm::vec3(0.0f, 1.0f, 0.0f));
			break;
		case 5:
			modify = glm::scale(modify, glm::vec3(1.0f - animateOffset[i] / 100.0f, 1.0f - animateOffset[i] / 100.0f, 1.0f));
			break;
		case 6:
			modify = glm::rotate(modify, glm::radians(animateOffset[i]), glm::vec3(1.0f, 0.0f, 0.0f));
			break;
		case 7:
			modify = glm::rotate(modify, glm::radians(animateOffset[i]), glm::vec3(0.0f, 0.0f, 1.0f));
			break;
		case 8:
			modify = glm::rotate(modify, glm::radians(-animateOffset[i]), glm::vec3(0.0f, 0.0f, 1.0f));
			break;
		case 9:
			modify = glm::rotate(modify, glm::radians(-animateOffset[i]), glm::vec3(1.0f, 0.0f, 0.0f));
			break;
		default:
			break;
		}

		glUniformMatrix4fv(matLoc, 1, GL_FALSE, glm::value_ptr(modify));
	}

	glm::mat4 basisRotate = glm::mat4(1.0f);
	basisRotate = glm::rotate(basisRotate, glm::radians(30.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	basisRotate = glm::rotate(basisRotate, glm::radians(30.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	glUniformMatrix4fv(glGetUniformLocation(shaderProgramID, "rotation"), 1, GL_FALSE, glm::value_ptr(basisRotate));
	glUniform1i(glGetUniformLocation(shaderProgramID, "isBasis"), true);
	d_basis->Render();

	glUniform1i(glGetUniformLocation(shaderProgramID, "isBasis"), false);
	glm::mat4 worldRotate = glm::mat4(1.0f);
	worldRotate = glm::rotate(worldRotate, glm::radians(xRot), glm::vec3(1.0f, 0.0f, 0.0f));
	worldRotate = glm::rotate(worldRotate, glm::radians(yRot), glm::vec3(0.0f, 1.0f, 0.0f));
	glUniformMatrix4fv(glGetUniformLocation(shaderProgramID, "rotation"), 1, GL_FALSE, glm::value_ptr(worldRotate));

	if (displayCube) {
		cube->Render();
	}
	else {
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
	case 'h':
		if (!depthTest) {
			glEnable(GL_DEPTH_TEST);
			depthTest = true;
			std::cout << "Depth test Enabled" << std::endl;
		}
		else {
			glDisable(GL_DEPTH_TEST);
			depthTest = false;
			std::cout << "Depth test Disabled" << std::endl;
		}
		break;
	case 'y':
		if (dyRot < 1.0f) dyRot = 1.0f;
		else dyRot = 0.0f;
		break;
	case 'Y':
		if (dyRot > -1.0f) dyRot = -1.0f;
		else dyRot = 0.0f;
		break;
	case 'f':
		animate[0] = !animate[0];
		break;
	case 't':
		animate[2] = !animate[2];
		break;
	case 's':
		animate[1] = !animate[1];
		animate[3] = !animate[3];
		break;
	case 'b':
		animate[5] = !animate[5];
		break;
	case 'o':
		for (int i = 6; i < 10; i++)
			animate[i] = !animate[i];
		break;
	case 'r':
		break;
	case 'c':
		xRot = 30;
		yRot = 30;
		dxRot = 0.0f;
		dyRot = 0.0f;
		depthTest = true;
		glEnable(GL_DEPTH_TEST);
		displayCube = true;
		for (int i = 0; i < 11; i++) {
			animate[i] = false;
			animateOffset[i] = 0.0f;
			deltaOffset[i] = 1.0f;
		}
		break;
	case 'q':
		exit(0);
		break;
	}
}

GLvoid Timer(int value)
{
	xRot += dxRot;
	yRot += dyRot;
	for (int i = 0; i < 11; i++) {
		if (!animate[i]) continue;
		animateOffset[i] += deltaOffset[i];

		switch (i) {
		case 0:
			if (animateOffset[i] >= 90.0f || animateOffset[i] <= 0.0f)
				deltaOffset[i] = -deltaOffset[i];
			break;
		case 1: case 2: case 3:
			if (animateOffset[i] >= 360.0f)
				animateOffset[i] = 0.0f;
			break;
		case 5:
			if (animateOffset[i] > 100.0f || animateOffset[i] < 0.0f)
				deltaOffset[i] = -deltaOffset[i];
			break;
		case 6: case 7: case 8: case 9:
			if (animateOffset[i] > 235.00f || animateOffset[i] < 0.0f)
				deltaOffset[i] = -deltaOffset[i];
			break;
		}
	}
	glutPostRedisplay();
	glutTimerFunc(1000 / 60, Timer, 1);
}