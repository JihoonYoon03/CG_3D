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
GLvoid SpecialKeyboard(int key, int x, int y);
GLvoid SpecialKeyboardUp(int key, int x, int y);
GLvoid TimerFunc(int value);

//--- 필요한 변수 선언
GLint winWidth = 600, winHeight = 600;
GLuint shaderProgramID; //--- 세이더 프로그램 이름
GLuint vertexShader; //--- 버텍스 세이더 객체
GLuint fragmentShader; //--- 프래그먼트 세이더 객체

class Plane {
	glm::vec3 vertices[4][2] = {
		{ { 1.0f, 0.0f, 1.0f }, {0.1f, 0.5f, 0.0f} },
		{ { -1.0f, 0.0f, 1.0f }, {0.1f, 0.5f, 0.0f} },
		{ { -1.0f, 0.0f, -1.0f }, {0.1f, 0.5f, 0.0f} },
		{ { 1.0f, 0.0f, -1.0f }, {0.1f, 0.5f, 0.0f} }
	};

	GLuint faces[6] = {
		0, 2, 1,
		0, 3, 2
	};

	GLuint VAO, VBO, EBO;

public:
	Plane(glm::vec3 scaleFactor) {
		for (int i = 0; i < 4; i++) {
			vertices[i][0] *= scaleFactor;
		}

		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);
		glGenBuffers(1, &EBO);

		glBindVertexArray(VAO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(faces), faces, GL_STATIC_DRAW);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3) * 2, (GLvoid*)0);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3) * 2, (GLvoid*)sizeof(glm::vec3));
		glEnableVertexAttribArray(1);


		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
	}
	void Render() {
		glBindVertexArray(VAO);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);
	}
};

Plane* ground = nullptr;
Model* body_bottom = nullptr, * body_middle = nullptr, * turret1 = nullptr, * turret2 = nullptr;
Model* cannon1 = nullptr, * cannon2 = nullptr, * flag1 = nullptr, * flag2 = nullptr;
DisplayBasis* XYZ;

glm::vec3 bgColor = { 0.1f, 0.1f, 0.1f };

GLfloat camera_rotY_self = 0.0f, camera_rotY_orbit = 0.0f, middle_rot = 0.0f, cannon_rot = 0.0f, flag_rot = 0.0f;
glm::vec3 camera_pos{ 0.0f, 2.0f, 0.0f };
glm::vec3 tank_trans{ 0.0f, 0.0f, 0.0f };
glm::vec3 turret1_start_pos{ 0.0f, 0.0f, 0.0f }, turret2_start_pos{ 0.0f, 0.0f, 0.0f };
glm::vec3 turret1_end_pos{ 0.0f, 0.0f, 0.0f }, turret2_end_pos{ 0.0f, 0.0f, 0.0f };

GLfloat camera_offsetZ = 5.0f, camera_rotY_self_delta = 0.0f, camera_rotY_orbit_delta = 0.0f;
glm::vec3 camera_delta{ 0.0f, 0.0f, 0.0f };
int turret_change_frame = 0;
bool turret_change = false;
const unsigned int TURRET_CHANGE_DURATION = 60;

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
	XYZ = new DisplayBasis(2.0f);
	ground = new Plane({ 10.0f, 1.0f, 10.0f });
	body_bottom = new Model("Models/Cube.obj", { 2.0f, 0.5f, 1.5f });
	body_middle = new Model("Models/Cube.obj", { 1.2f, 0.25f, 0.5f }, { 1.0f, 0.0f, 0.0f });
	turret1 = new Model("Models/Cube.obj", { 0.6f, 0.2f, 0.6f }, { 0.0f, 1.0f, 1.0f });
	turret2 = new Model("Models/Cube.obj", { 0.6f, 0.2f, 0.6f }, { 0.0f, 1.0f, 1.0f });
	cannon1 = new Model("Models/Cube.obj", { 0.1f, 0.1f, 0.4f }, { 0.0f, 1.0f, 1.0f });
	cannon2 = new Model("Models/Cube.obj", { 0.1f, 0.1f, 0.4f }, { 0.0f, 1.0f, 1.0f });
	flag1 = new Model("Models/Cube.obj", { 0.1f, 0.4f, 0.1f }, { 1.0f, 0.0f, 1.0f });
	flag2 = new Model("Models/Cube.obj", { 0.1f, 0.4f, 0.1f }, { 1.0f, 0.0f, 1.0f });
	body_bottom->setDefTranslate({ 0.0f, 0.5f, 0.0f });
	body_middle->setDefTranslate({ 0.0f, 0.75f, 0.0f });
	body_middle->setParent(body_bottom);
	turret1->setDefTranslate({ -1.2f, 0.45f, 0.0f });
	turret1->setParent(body_middle);
	turret2->setDefTranslate({ 1.2f, 0.45f, 0.0f });
	turret2->setParent(body_middle);
	cannon1->setDefTranslate({ 0.0f, 0.0f, 1.0f });
	cannon1->setParent(turret1);
	cannon2->setDefTranslate({ 0.0f, 0.0f, 1.0f });
	cannon2->setParent(turret2);
	flag1->setDefTranslate({ 0.0f, 0.6f, 0.0f });
	flag1->setParent(turret1);
	flag2->setDefTranslate({ 0.0f, 0.6f, 0.0f });
	flag2->setParent(turret2);

	glutDisplayFunc(drawScene);
	glutReshapeFunc(Reshape);
	glutKeyboardFunc(Keyboard);
	glutSpecialFunc(SpecialKeyboard);
	glutSpecialUpFunc(SpecialKeyboardUp);
	glutTimerFunc(1000 / 60, TimerFunc, 1);
	glutMainLoop();
}

GLvoid drawScene()
{
	glClearColor(bgColor.x, bgColor.y, bgColor.z, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glUseProgram(shaderProgramID);

	glm::mat4 projection = glm::perspective(glm::radians(55.0f), 1.0f, 0.1f, 100.0f);
	glUniformMatrix4fv(glGetUniformLocation(shaderProgramID, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

	glm::mat4 view = glm::lookAt(camera_pos, camera_pos + glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	view = glm::rotate(view, glm::radians(-camera_rotY_self), glm::vec3(0.0f, 1.0f, 0.0f));
	glUniformMatrix4fv(glGetUniformLocation(shaderProgramID, "view"), 1, GL_FALSE, glm::value_ptr(view));

	glm::mat4 world = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));
	glUniformMatrix4fv(glGetUniformLocation(shaderProgramID, "world"), 1, GL_FALSE, glm::value_ptr(world));

	glUniformMatrix4fv(glGetUniformLocation(shaderProgramID, "model"), 1, GL_FALSE, glm::value_ptr(glm::mat4(1.0f)));
	XYZ->Render();
	ground->Render();

	glUniformMatrix4fv(glGetUniformLocation(shaderProgramID, "model"), 1, GL_FALSE, glm::value_ptr(body_bottom->getModelMatrix()));
	body_bottom->Render();
	glUniformMatrix4fv(glGetUniformLocation(shaderProgramID, "model"), 1, GL_FALSE, glm::value_ptr(body_middle->getModelMatrix()));
	body_middle->Render();
	glUniformMatrix4fv(glGetUniformLocation(shaderProgramID, "model"), 1, GL_FALSE, glm::value_ptr(turret1->getModelMatrix()));
	turret1->Render();
	glUniformMatrix4fv(glGetUniformLocation(shaderProgramID, "model"), 1, GL_FALSE, glm::value_ptr(turret2->getModelMatrix()));
	turret2->Render();
	glUniformMatrix4fv(glGetUniformLocation(shaderProgramID, "model"), 1, GL_FALSE, glm::value_ptr(cannon1->getModelMatrix()));
	cannon1->Render();
	glUniformMatrix4fv(glGetUniformLocation(shaderProgramID, "model"), 1, GL_FALSE, glm::value_ptr(cannon2->getModelMatrix()));
	cannon2->Render();
	glUniformMatrix4fv(glGetUniformLocation(shaderProgramID, "model"), 1, GL_FALSE, glm::value_ptr(flag1->getModelMatrix()));
	flag1->Render();
	glUniformMatrix4fv(glGetUniformLocation(shaderProgramID, "model"), 1, GL_FALSE, glm::value_ptr(flag2->getModelMatrix()));
	flag2->Render();


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
	case 'z':
		if (camera_delta.z == 0)
			camera_delta.z = 1;
		else
			camera_delta.z = 0;
		break;
	case 'Z':
		if (camera_delta.z == 0)
			camera_delta.z = -1;
		else
			camera_delta.z = 0;
		break;
	case 'x':
		if (camera_delta.x == 0)
			camera_delta.x = 1;
		else
			camera_delta.x = 0;
		break;
	case 'X':
		if (camera_delta.x == 0)
			camera_delta.x = -1;
		else
			camera_delta.x = 0;
		break;
	case 'y':
		if (camera_rotY_self_delta == 0)
			camera_rotY_self_delta = 1;
		else
			camera_rotY_self_delta = 0;
		break;
	case 'Y':
		if (camera_rotY_self_delta == 0)
			camera_rotY_self_delta = -1;
		else
			camera_rotY_self_delta = 0;
		break;
	case 't':
		if (middle_rot == 0)
			middle_rot = 1;
		else
			middle_rot = 0;
		break;
	case 'l':
		if (!turret_change) {
			turret1_start_pos = turret1->retDistTo();
			turret2_start_pos = turret2->retDistTo();
			turret1_end_pos = turret2_start_pos;
			turret2_end_pos = turret1_start_pos;
			turret_change = true;
		}
		break;
	case 'g':
		if (cannon_rot == 0)
			cannon_rot = 1;
		else
			cannon_rot = 0;
		break;
	case 'p':
		if (flag_rot == 0)
			flag_rot = 1;
		else
			flag_rot = 0;
		break;
	case 'q':
		exit(0);
		break;
	}
}

GLvoid SpecialKeyboard(int key, int x, int y)
{
	switch (key) {
	case GLUT_KEY_UP:
		if (tank_trans.z >= 0) tank_trans.z -= 1;
		break;
	case GLUT_KEY_DOWN:
		if (tank_trans.z <= 0) tank_trans.z += 1;
		break;
	case GLUT_KEY_LEFT:
		if (tank_trans.x >= 0) tank_trans.x -= 1;
		break;
	case GLUT_KEY_RIGHT:
		if (tank_trans.x <= 0) tank_trans.x += 1;
		break;
	}
}

GLvoid SpecialKeyboardUp(int key, int x, int y)
{
	switch (key) {
	case GLUT_KEY_UP:
		if (tank_trans.z <= 0) tank_trans.z += 1;
		break;
	case GLUT_KEY_DOWN:
		if (tank_trans.z >= 0) tank_trans.z -= 1;
		break;
	case GLUT_KEY_LEFT:
		if (tank_trans.x <= 0) tank_trans.x += 1;
		break;
	case GLUT_KEY_RIGHT:
		if (tank_trans.x >= 0) tank_trans.x -= 1;
		break;
	}
}

GLvoid TimerFunc(int value)
{
	camera_pos += 0.1f * camera_delta;
	camera_rotY_self += camera_rotY_self_delta;
	camera_rotY_orbit += camera_rotY_orbit_delta;
	if (turret_change) {
		turret_change_frame++;
		
		float t = static_cast<float>(turret_change_frame) / TURRET_CHANGE_DURATION;
		
		if (t > 1.0f) {
			t = 1.0f;
			turret_change = false;
			turret_change_frame = 0;
		}

		glm::vec3 new_turret1_pos = glm::mix(turret1_start_pos, turret1_end_pos, t);
		glm::vec3 new_turret2_pos = glm::mix(turret2_start_pos, turret2_end_pos, t);

		glm::vec3 delta_turret1 = new_turret1_pos - turret1->retDistTo();
		glm::vec3 delta_turret2 = new_turret2_pos - turret2->retDistTo();

		turret1->translate(delta_turret1);
		turret2->translate(delta_turret2);
	}
	else {
		body_bottom->translate(tank_trans * 0.1f);
		if (middle_rot) {
			body_middle->translate(-body_middle->retDistTo());
			body_middle->rotate(middle_rot, glm::vec3(0.0f, 1.0f, 0.0f));
			body_middle->translate(body_middle->retDistTo());
		}
		if (cannon_rot) {
			cannon1->translate(-cannon1->retDistTo());
			cannon1->rotate(cannon_rot, glm::vec3(0.0f, 1.0f, 0.0f));
			cannon1->translate(cannon1->retDistTo());
			cannon2->translate(-cannon2->retDistTo());
			cannon2->rotate(-cannon_rot, glm::vec3(0.0f, 1.0f, 0.0f));
			cannon2->translate(cannon2->retDistTo());
		}
		if (flag_rot) {
			flag1->translate(-flag1->retDistTo());
			flag1->rotate(flag_rot, glm::vec3(1.0f, 0.0f, 0.0f));
			flag1->translate(flag1->retDistTo());
			flag2->translate(-flag2->retDistTo());
			flag2->rotate(-flag_rot, glm::vec3(1.0f, 0.0f, 0.0f));
			flag2->translate(flag2->retDistTo());
		}
	}

	glutPostRedisplay();
	glutTimerFunc(1000 / 60, TimerFunc, 1);
}