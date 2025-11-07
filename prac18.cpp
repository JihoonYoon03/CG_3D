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

Model* model_list[2][2] = {nullptr, nullptr, nullptr, nullptr};
DisplayBasis* XYZ;

glm::vec3 bgColor = { 0.1f, 0.1f, 0.1f };

// 변환 수치
GLfloat m_rotationX = 0.0f, m_rotationY = 0.0f;
glm::vec3 model1_startPos = { 0.0f, 0.0f, 0.0f }, model2_startPos = { 0.0f, 0.0f, 0.0f };
glm::vec3 model1_targetPos = { 0.0f, 0.0f, 0.0f }, model2_targetPos = { 0.0f, 0.0f, 0.0f };
glm::vec3 model1_middlePos = { 0.0f, 0.0f, 0.0f }, model2_middlePos = { 0.0f, 0.0f, 0.0f };
glm::vec3 scale_model[2][2] = { {{ 1.0f, 1.0f, 1.0f }, { 1.0f, 1.0f, 1.0f }}, {{ 1.0f, 1.0f, 1.0f }, { 1.0f, 1.0f, 1.0f }} };
glm::vec3 scale_from_origin[2][2] = { {{ 1.0f, 1.0f, 1.0f }, { 1.0f, 1.0f, 1.0f }}, {{ 1.0f, 1.0f, 1.0f }, { 1.0f, 1.0f, 1.0f }} };

// 수치 변화량
GLfloat delta_spinX = 0.0f, delta_spinY = 0.0f, delta_orbitY = 0.0f, delta_translateX = 0.0f, delta_translateY = 0.0f, delta_scale = 0.2f;
glm::vec3 delta_model2to1 = { 0.0f, 0.0f, 0.0f }, delta_model1to2 = { 0.0f, 0.0f, 0.0f };

int selectedModel = 0, backup_selected = 0;
bool cursorEnabled = false, move_to_other_direct = false, move_to_other_around = false, move_check = false, anim_V = false;
unsigned int page = 0;
int swap_frame = 0;
const int SWAP_DURATION = 60;

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
	XYZ = new DisplayBasis(1.2f);
	model_list[0][0] = new Model("Models/test.obj", {0.2f, 0.2f, 0.2f});
	model_list[0][1] = new Model("Models/Pistol.obj", { 0.0002f, 0.0002f, 0.0002f });
	model_list[1][0] = new Model("Models/K1.obj", { 0.002f, 0.002f, 0.002f });
	model_list[1][1] = new Model("Models/Knife.obj", { 0.2f, 0.2f, 0.2f });

	model_list[0][0]->translate({ 1.0, 0.0, 0.0 });
	model_list[0][1]->translate({ -1.0, 0.0, 0.0 });

	model_list[1][0]->translate({ 1.0, 0.0, 0.0 });
	model_list[1][1]->translate({ -1.0, 0.0, 0.0 });

	model_list[1][0]->setEnabled(false);
	model_list[1][1]->setEnabled(false);

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

	glm::mat4 projection = glm::perspective(glm::radians(45.0f), 1.0f, 0.1f, 100.0f);
	glUniformMatrix4fv(glGetUniformLocation(shaderProgramID, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

	glm::mat4 view = glm::lookAt(glm::vec3(0.0f, 0.0f, 5.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	glUniformMatrix4fv(glGetUniformLocation(shaderProgramID, "view"), 1, GL_FALSE, glm::value_ptr(view));

	glm::mat4 world = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));
	world = glm::rotate(world, glm::radians(30.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	world = glm::rotate(world, glm::radians(-30.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	if (cursorEnabled)
	{
		world = glm::rotate(world, glm::radians(m_rotationX), glm::vec3(0.0f, 1.0f, 0.0f));
		world = glm::rotate(world, glm::radians(m_rotationY), glm::vec3(1.0f, 0.0f, 0.0f));
	}
	glUniformMatrix4fv(glGetUniformLocation(shaderProgramID, "world"), 1, GL_FALSE, glm::value_ptr(world));


	glUniform1i(glGetUniformLocation(shaderProgramID, "use_color_set"), false);
	glUniform1i(glGetUniformLocation(shaderProgramID, "isBasis"), true);
	XYZ->Render();
	glUniform1i(glGetUniformLocation(shaderProgramID, "isBasis"), false);


	glUniform1i(glGetUniformLocation(shaderProgramID, "use_color_set"), true);
	glUniform3f(glGetUniformLocation(shaderProgramID, "color_set"), 0.8f, 0.8f, 0.8f);

	for (int i = 0; i < 2; i++) {
		if (model_list[page][i]) {
			glm::mat4 scale_origin = glm::mat4(1.0f);
			if (selectedModel == i || selectedModel == 2) {
				if (!move_to_other_direct && !move_to_other_around) {

					glm::vec3 current_pos = model_list[page][i]->retDistTo();

					// 모델 기준 변환 구간
					model_list[page][i]->translate(current_pos * -1.0f);
					model_list[page][i]->rotate(delta_spinX, glm::vec3(1.0f, 0.0f, 0.0f));
					model_list[page][i]->rotate(delta_spinY, glm::vec3(0.0f, 1.0f, 0.0f));
					model_list[page][i]->translate({ delta_translateX, delta_translateY, 0.0f });
					model_list[page][i]->translate(current_pos);

					// 원점 기준 변환 구간
					model_list[page][i]->rotate(delta_orbitY, glm::vec3(0.0f, 1.0f, 0.0f));
				}
			}
			scale_origin = glm::scale(scale_origin, scale_from_origin[page][i]);

			glUniformMatrix4fv(glGetUniformLocation(shaderProgramID, "model"), 1, GL_FALSE, glm::value_ptr(scale_origin * model_list[page][i]->getModelMatrix()));
			model_list[page][i]->Render();
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

	m_rotationX += (x - winWidth / 2) * 0.2f;
	m_rotationY += (y - winHeight / 2) * 0.2f;

	if (m_rotationY > 59.0f) m_rotationY = 59.0f;
	if (m_rotationY < -119.0f) m_rotationY = -119.0f;

	glutWarpPointer(winWidth / 2, winHeight / 2);
}

GLvoid Keyboard(unsigned char key, int x, int y)
{
	switch (key) {
	case 's':
		page = 0;
		for (int i = 0; i < 2; i++) {
			for (int j = 0; j < 2; j++) {
				model_list[i][j]->setEnabled(true);
				model_list[i][j]->resetModelMatrix();
				scale_from_origin[i][j] = { 1.0f, 1.0f, 1.0f };
				
				scale_model[i][j] = { 1.0f, 1.0f, 1.0f };
				model_list[i][j]->setDefScale(scale_model[i][j]);
				model_list[i][j]->translate({ j == 0 ? 1.0f : -1.0f, 0.0, 0.0 });

				model_list[i][j]->setEnabled(i == 0);
			}
		}
		m_rotationX = 0.0f, m_rotationY = 0.0f;
		model1_startPos = { 0.0f, 0.0f, 0.0f }, model2_startPos = { 0.0f, 0.0f, 0.0f };
		model1_targetPos = { 0.0f, 0.0f, 0.0f }, model2_targetPos = { 0.0f, 0.0f, 0.0f };
		model1_middlePos = { 0.0f, 0.0f, 0.0f }, model2_middlePos = { 0.0f, 0.0f, 0.0f };

		delta_spinX = 0.0f, delta_spinY = 0.0f, delta_orbitY = 0.0f, delta_translateX = 0.0f, delta_translateY = 0.0f;
		delta_model2to1 = { 0.0f, 0.0f, 0.0f }, delta_model1to2 = { 0.0f, 0.0f, 0.0f };

		selectedModel = 0;
		cursorEnabled = false, move_to_other_direct = false, move_to_other_around = false, move_check = false;
		swap_frame = 0;
		break;
	case 'c':
		model_list[page][0]->setEnabled(false);
		model_list[page][1]->setEnabled(false);
		page = (page + 1) % 2;
		delta_spinX = 0.0f, delta_spinY = 0.0f, delta_orbitY = 0.0f, delta_translateX = 0.0f, delta_translateY = 0.0f;

		model_list[page][0]->setEnabled(true);
		model_list[page][1]->setEnabled(true);
		break;
	case '1': case '2': case '3':
		selectedModel = key - '1';
		break;
	case 'x': case 'X':
		if (delta_spinX == 0.0f)
			delta_spinX = key == 'x' ? 1.0f : -1.0f;
		else
			delta_spinX = 0.0f;
		break;
	case 'y': case 'Y':
		if (delta_spinY == 0.0f)
			delta_spinY = key == 'y' ? 1.0f : -1.0f;
		else
			delta_spinY = 0.0f;
		break;
	case 'r': case 'R':
		if (delta_orbitY == 0.0f)
			delta_orbitY = key == 'r' ? 1.0f : -1.0f;
		else
			delta_orbitY = 0.0f;
		break;
	case 'a':
		if (selectedModel == 2) {
			if (scale_model[page][0].x < 2.0f) scale_model[page][0] += delta_scale;
			if (scale_model[page][1].x < 2.0f) scale_model[page][1] += delta_scale;
		}
		else {
			if (scale_model[page][selectedModel].x < 2.0f) scale_model[page][selectedModel] += delta_scale;
		}
		
		if (model_list[page][0]) model_list[page][0]->setDefScale(scale_model[page][0]);
		if (model_list[page][1]) model_list[page][1]->setDefScale(scale_model[page][1]);
		break;
	case 'A':
		if (selectedModel == 2) {
			if (scale_model[page][0].x > 0.4f) scale_model[page][0] -= delta_scale;
			if (scale_model[page][1].x > 0.4f) scale_model[page][1] -= delta_scale;
		}
		else {
			if (scale_model[page][selectedModel].x > 0.4f) scale_model[page][selectedModel] -= delta_scale;
		}

		if (model_list[page][0]) model_list[page][0]->setDefScale(scale_model[page][0]);
		if (model_list[page][1]) model_list[page][1]->setDefScale(scale_model[page][1]);
		break;
	case 'b':
		if (selectedModel == 2) {
			if (scale_from_origin[page][0].x < 2.0f) scale_from_origin[page][0] += delta_scale;
			if (scale_from_origin[page][1].x < 2.0f) scale_from_origin[page][1] += delta_scale;
		}
		else {
			if (scale_from_origin[page][selectedModel].x < 2.0f) scale_from_origin[page][selectedModel] += delta_scale;
		}
		break;
	case 'B':
		if (selectedModel == 2) {
			if (scale_from_origin[page][0].x > 0.4f) scale_from_origin[page][0] -= delta_scale;
			if (scale_from_origin[page][1].x > 0.4f) scale_from_origin[page][1] -= delta_scale;
		}
		else {
			if (scale_from_origin[page][selectedModel].x > 0.4f) scale_from_origin[page][selectedModel] -= delta_scale;
		}
		break;
	case 'd': case 'D':
		if (delta_translateX == 0)
			delta_translateX = key == 'd' ? 0.01f : -0.01f;
		else
			delta_translateX = 0.0f;
		break;
	case 'e': case 'E':
		if (delta_translateY == 0)
			delta_translateY = key == 'e' ? 0.01f : -0.01f;
		else
			delta_translateY = 0.0f;
		break;
	case 't':
		if (move_to_other_around) break;

		model1_startPos = model_list[page][0]->retDistTo();
		model2_startPos = model_list[page][1]->retDistTo();

		model1_targetPos = model2_startPos;
		model2_targetPos = model1_startPos;

		move_to_other_direct = true;
		swap_frame = 0;
		break;
	case 'u':
		if (move_to_other_direct) break;

		model1_startPos = model_list[page][0]->retDistTo();
		model2_startPos = model_list[page][1]->retDistTo();

		model1_targetPos = model2_startPos;
		model2_targetPos = model1_startPos;

		model1_middlePos = { 0.0f, 1.0f, 0.0f };
		model2_middlePos = { 0.0f, -1.0f, 0.0f };

		move_check = false;
		move_to_other_around = true;
		swap_frame = 0;
		break;
	case 'v':
	{
		if (anim_V) {
			anim_V = false;
			selectedModel = backup_selected;
			break;
		}
		else {
			anim_V = true;
			backup_selected = selectedModel;
			selectedModel = 2;
		}

		// 랜덤 선택
		bool index = rand() % 2;

		model_list[page][index]->setDefScale(glm::vec3(1.6f, 1.6f, 1.6f));
		model_list[page][(index + 1) % 2]->setDefScale(glm::vec3(0.6f, 0.6f, 0.6f));

		delta_spinY = rand() % 2 ? 1.0f : -1.0f;

		delta_orbitY = rand() % 2 ? 1.0f : -1.0f;

		break;
	}
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
	if (move_to_other_direct) {
		swap_frame++;

		// 프레임에 따른 매개변수 t 계산
		float t = static_cast<float>(swap_frame) / static_cast<float>(SWAP_DURATION);

		if (t >= 1.0f) {
			t = 1.0f;
			move_to_other_direct = false;
			swap_frame = 0;
		}

		glm::vec3 model1_currentPos = model_list[page][0]->retDistTo();
		glm::vec3 model2_currentPos = model_list[page][1]->retDistTo();

		// t에 따라 선형 보간
		glm::vec3 model1_newPos = glm::mix(model1_startPos, model1_targetPos, t);
		glm::vec3 model2_newPos = glm::mix(model2_startPos, model2_targetPos, t);

		glm::vec3 delta1 = model1_newPos - model1_currentPos;
		glm::vec3 delta2 = model2_newPos - model2_currentPos;

		model_list[page][0]->translate(delta1);
		model_list[page][1]->translate(delta2);
	}
	else if (move_to_other_around) {
		swap_frame++;

		// 프레임에 따른 매개변수 t 계산
		float t = static_cast<float>(swap_frame) / static_cast<float>(SWAP_DURATION);

		if (t >= 1.0f) {
			t = 1.0f;

			if (move_check) {
				// 마지막 지점 도달
				swap_frame = 0;
				move_to_other_around = false;
			}
			else {
				// 중간 지점 도달
				t = 0.0f;
				move_check = true;
				swap_frame = 0;
			}
		}

		glm::vec3 model1_currentPos = model_list[page][0]->retDistTo();
		glm::vec3 model2_currentPos = model_list[page][1]->retDistTo();

		glm::vec3 model1_newPos, model2_newPos;

		if (move_check) {
			model1_newPos = glm::mix(model1_middlePos, model1_targetPos, t);
			model2_newPos = glm::mix(model2_middlePos, model2_targetPos, t);
		}
		else {
			model1_newPos = glm::mix(model1_startPos, model1_middlePos, t);
			model2_newPos = glm::mix(model2_startPos, model2_middlePos, t);
		}

		glm::vec3 delta1 = model1_newPos - model1_currentPos;
		glm::vec3 delta2 = model2_newPos - model2_currentPos;

		model_list[page][0]->translate(delta1);
		model_list[page][1]->translate(delta2);
	}
	glutPostRedisplay();
	glutTimerFunc(1000 / 60, TimerFunc, 1);
}