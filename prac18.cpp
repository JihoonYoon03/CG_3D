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
GLvoid TimerFunc(int value);

//--- 필요한 변수 선언
GLint winWidth = 600, winHeight = 600;
GLuint shaderProgramID; //--- 세이더 프로그램 이름
GLuint vertexShader; //--- 버텍스 세이더 객체
GLuint fragmentShader; //--- 프래그먼트 세이더 객체

enum MODELS { DOG = 0, PISTOL, K1, KNIFE };
Model* model_list[4] = { nullptr, nullptr, nullptr, nullptr };
DisplayBasis* XYZ;

glm::vec3 bgColor = { 0.1f, 0.1f, 0.1f };
GLfloat mouseRotX = 0.0f, mouseRotY = 0.0f, deltaSpinX = 0.0f, deltaSpinY = 0.0f, deltaOrbitY = 0.0f, deltaScaleFromSelf = 1.0f, deltaScaleFromOrigin = 1.0f;
GLfloat deltaTransX = 0.0f, deltaTransY = 0.0f;
int selectedModel = 0;
bool cursorEnabled = false;

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
	XYZ = new DisplayBasis(1.2f);
	model_list[0] = new Model("Models/test.obj");
	model_list[1] = new Model("Models/Pistol.obj");
	model_list[2] = new Model("Models/K1.obj");
	// model_list[3] = new Model("Models/Knife.obj");

	model_list[DOG]->scale({ 0.2, 0.2, 0.2 });
	model_list[DOG]->translate({ 1.0, 0.0, 0.0 });

	model_list[K1]->scale({0.002, 0.002, 0.002});
	model_list[K1]->translate({ -1.0, 0.0, 0.0 });

	model_list[PISTOL]->setEnabled(false);
	// model_list[KNIFE]->setEnabled(false);

	glutDisplayFunc(drawScene);
	glutReshapeFunc(Reshape);
	glutKeyboardFunc(Keyboard);
	glutPassiveMotionFunc(MouseMotion);
	glutTimerFunc(1000 / 60, TimerFunc, 1);
	glutMainLoop();
}

GLvoid drawScene()
{
	glClearColor(bgColor.x, bgColor.y, bgColor.z, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glUseProgram(shaderProgramID);

	glm::mat4 projection = glm::perspective(glm::radians(50.0f), 1.0f, 0.1f, 100.0f);
	glUniformMatrix4fv(glGetUniformLocation(shaderProgramID, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
	
	glm::mat4 view = glm::lookAt(glm::vec3(0.0f, 0.0f, 3.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	glUniformMatrix4fv(glGetUniformLocation(shaderProgramID, "view"), 1, GL_FALSE, glm::value_ptr(view));

	glm::mat4 world = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));
	world = glm::rotate(world, glm::radians(30.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	world = glm::rotate(world, glm::radians(-30.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	if (cursorEnabled)
	{
		world = glm::rotate(world, glm::radians(30.0f + mouseRotX), glm::vec3(0.0f, 1.0f, 0.0f));
		world = glm::rotate(world, glm::radians(-30.0f + mouseRotY), glm::vec3(1.0f, 0.0f, 0.0f));
	}
	glUniformMatrix4fv(glGetUniformLocation(shaderProgramID, "world"), 1, GL_FALSE, glm::value_ptr(world));



	glUniform1i(glGetUniformLocation(shaderProgramID, "use_color_set"), false);
	glUniform1i(glGetUniformLocation(shaderProgramID, "isBasis"), true);
	XYZ->Render();
	glUniform1i(glGetUniformLocation(shaderProgramID, "isBasis"), false);

	glUniform1i(glGetUniformLocation(shaderProgramID, "use_color_set"), true);
	glUniform3f(glGetUniformLocation(shaderProgramID, "color_set"), 0.8f, 0.8f, 0.8f);

	for (int i = 0; i < 4; i++) {
		if (model_list[i] != nullptr) {
			if (selectedModel == i || selectedModel == 2) {
				glUniformMatrix4fv(glGetUniformLocation(shaderProgramID, "model"), 1, GL_FALSE, glm::value_ptr(model_list[i]->getModelMatrix()));
				model_list[i]->Render();
			}
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

GLvoid MouseMotion(int x, int y)
{
	if (cursorEnabled == false)
		return;

	mouseRotX += (x - winWidth / 2) * 0.2f;
	mouseRotY += (y - winHeight / 2) * 0.2f;

	if (mouseRotY > 89.0f) mouseRotY = 89.0f;
	if (mouseRotY < -89.0f) mouseRotY = -89.0f;

	glutWarpPointer(winWidth / 2, winHeight / 2);
	glutPostRedisplay();
}

GLvoid Keyboard(unsigned char key, int x, int y)
{
	switch (key) {
	case '1': case '2': case '3':
		selectedModel = key - '1';
		break;
	case 'x': case 'X':
		if (deltaSpinX == 0.0f)
			deltaSpinX = key == 'x' ? 1.0f : -1.0f;
		else
			deltaSpinX = 0.0f;
		break;
	case 'y': case 'Y':
		if (deltaSpinY == 0.0f)
			deltaSpinY = key == 'y' ? 1.0f : -1.0f;
		else
			deltaSpinY = 0.0f;
		break;
	case 'r': case 'R':
		if (deltaOrbitY == 0.0f)
			deltaOrbitY = key == 'r' ? 1.0f : -1.0f;
		else
			deltaOrbitY = 0.0f;
		break;
	case 'a':
		if (deltaScaleFromOrigin < 2.0f)
			deltaScaleFromOrigin += 0.2f;
		if (model_list[selectedModel] != nullptr)
			model_list[selectedModel]->setDefScale({ deltaScaleFromOrigin, deltaScaleFromOrigin, deltaScaleFromOrigin });
		break;
	case 'A':
		if (deltaScaleFromOrigin > 0.2f)
			deltaScaleFromOrigin -= 0.2f;
		if (model_list[selectedModel] != nullptr)
			model_list[selectedModel]->setDefScale({ deltaScaleFromOrigin, deltaScaleFromOrigin, deltaScaleFromOrigin });
		break;
	case 'b':
		break;
	case 'B':
		break;
	case 'd': case 'D':
		if (deltaTransX == 0)
			deltaTransX = key == 'd' ? 0.01f : -0.01f;
		else
			deltaTransX = 0.0f;
		break;
	case 'e': case 'E':
		if (deltaTransY == 0)
			deltaTransY = key == 'e' ? 0.01f : -0.01f;
		else
			deltaTransY = 0.0f;
		break;
	case 'm':
		if (cursorEnabled) {
			cursorEnabled = false;
			glutSetCursor(GLUT_CURSOR_INHERIT);
		}
		else {
			cursorEnabled = true;
			glutSetCursor(GLUT_CURSOR_NONE);
		}
		break;
	case 'q':
		exit(0);
		break;
	}
}

GLvoid TimerFunc(int value)
{
	for (int i = 0; i < 4; i++) {
		if (i == selectedModel || selectedModel == 2) {
			if (model_list[i] != nullptr) {
				
				// 모델 기준 변환 구간
				model_list[i]->translate(model_list[i]->retDistTo() * -1.0f);
				model_list[i]->rotate(deltaSpinX, glm::vec3(1.0f, 0.0f, 0.0f));
				model_list[i]->rotate(deltaSpinY, glm::vec3(0.0f, 1.0f, 0.0f));
				model_list[i]->translate({ deltaTransX, deltaTransY, 0.0f });
				model_list[i]->translate(model_list[i]->retDistTo());

				// 원점 기준 변환 구간
				model_list[i]->rotate(deltaOrbitY, glm::vec3(0.0f, 1.0f, 0.0f));
			}
		}
	}
	glutPostRedisplay();
	glutTimerFunc(1000 / 60, TimerFunc, 1);
}