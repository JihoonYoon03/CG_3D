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

class Orbit {
	glm::vec3 center;
	GLfloat angleStep;

	glm::mat4 scale_mat = glm::mat4(1.0f);
	glm::mat4 rotation_mat = glm::mat4(1.0f);
	glm::mat4 translation_mat = glm::mat4(1.0f);
	std::vector<ColoredVertex> orbitPoints;

	GLuint VAO, VBO;
public:
	Orbit(const glm::vec3& color){
		center = glm::vec3(0.0f, 0.0f, 0.0f);
		angleStep = 5.0f; // 5도 간격

		for (GLfloat angle = 0.0f; angle < 360.0f; angle += angleStep) {
			GLfloat rad = glm::radians(angle);
			GLfloat x = center.x + 1.0f * cos(rad);
			GLfloat z = center.z + 1.0f * sin(rad);

			ColoredVertex point;
			point.pos = glm::vec3(x, 0.0f, z);
			point.color = color;

			orbitPoints.push_back(point);
		}

		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);
		glBindVertexArray(VAO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, orbitPoints.size() * sizeof(ColoredVertex), orbitPoints.data(), GL_STATIC_DRAW);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(ColoredVertex), (GLvoid*)0);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(ColoredVertex), (GLvoid*)sizeof(glm::vec3));
		glEnableVertexAttribArray(1);
	}

	void scale(glm::vec3 scaleFactor) {
		scale_mat = glm::scale(glm::mat4(1.0f), scaleFactor);
	}

	void rotate(glm::mat4 rotationFactor) {
		rotation_mat = rotationFactor;
	}

	void translate(glm::vec3 translateFactor) {
		translation_mat = glm::translate(glm::mat4(1.0f), translateFactor);
	}

	glm::mat4 getModelMatrix() {
		return translation_mat * rotation_mat * scale_mat;
	}

	void Render() {
		glBindVertexArray(VAO);
		glDrawArrays(GL_LINE_LOOP, 0, orbitPoints.size());
	}
};

//--- 필요한 변수 선언
GLint winWidth = 600, winHeight = 600;
GLuint shaderProgramID; //--- 세이더 프로그램 이름
GLuint vertexShader; //--- 버텍스 세이더 객체
GLuint fragmentShader; //--- 프래그먼트 세이더 객체

Model* sun, *planet[3], *moon[3];
Orbit* orbit_sun[3];
Orbit* orbit_planet[3];
DisplayBasis* XYZ;

glm::vec3 bgColor = { 0.1f, 0.1f, 0.1f };

// 변환 수치
GLfloat orbit_radius_sun = 2.0f, orbit_radius_planet = 0.8f;
GLfloat m_rotationX = 0.0f, m_rotationY = 0.0f;

// 수치 변화량
bool isOrtho = false, isWire = false, zRotate = false;

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
	sun = new Model("Models/Sphere.obj", glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	for (int i = 0; i < 3; i++) {
		GLfloat offset = i == 0 ? 0 : (i == 1 ? 45.0f : -45.0f);
		planet[i] = new Model("Models/Sphere.obj", glm::vec3(0.3f, 0.3f, 0.3f), glm::vec3(0.0f, 1.0f, 0.0f));
		planet[i]->setParent(sun);
		planet[i]->translate(glm::vec3(orbit_radius_sun + i, 0.0f, 0.0f));
		planet[i]->rotate(120.0f * i, glm::vec3(0.0f, 1.0f, 0.0f));
		planet[i]->rotate(offset, glm::vec3(0.0f, 0.0f, 1.0f));

		orbit_sun[i] = new Orbit(glm::vec3(0.2f, 0.5f, 0.2f));
		orbit_sun[i]->scale(glm::vec3(orbit_radius_sun + i, 1.0f, orbit_radius_sun + i));
		orbit_sun[i]->rotate(glm::rotate(glm::mat4(1.0f), glm::radians(offset), glm::vec3(0.0f, 0.0f, 1.0f)));

		moon[i] = new Model("Models/Sphere.obj", glm::vec3(0.1f, 0.1f, 0.1f), glm::vec3(1.0f, 0.0f, 0.0f));
		moon[i]->setParent(planet[i]);
		moon[i]->setDefTranslate(glm::vec3(orbit_radius_planet, 0.0f, 0.0f));

		orbit_planet[i] = new Orbit(glm::vec3(0.0f, 0.5f, 0.5f));
		orbit_planet[i]->scale(glm::vec3(orbit_radius_planet, 1.0f, orbit_radius_planet));
		orbit_planet[i]->rotate(glm::rotate(glm::mat4(1.0f), glm::radians(offset), glm::vec3(0.0f, 0.0f, 1.0f)));
	}

	glutDisplayFunc(drawScene);
	glutReshapeFunc(Reshape);
	glutKeyboardFunc(Keyboard);
	glutMotionFunc(MouseMotion);
	glutTimerFunc(1000 / 60, TimerFunc, 1);
	glutMainLoop();
}

GLvoid drawScene()
{
	glClearColor(bgColor.x, bgColor.y, bgColor.z, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glUseProgram(shaderProgramID);

	glm::mat4 projection;
	if (isOrtho) projection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, 0.1f, 100.0f);
	else projection = glm::perspective(glm::radians(45.0f), 1.0f, 0.1f, 100.0f);
	glUniformMatrix4fv(glGetUniformLocation(shaderProgramID, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

	glm::mat4 view = glm::lookAt(glm::vec3(0.0f, 0.0f, 15.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	glUniformMatrix4fv(glGetUniformLocation(shaderProgramID, "view"), 1, GL_FALSE, glm::value_ptr(view));

	glm::mat4 world = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));
	world = glm::rotate(world, glm::radians(30.0f + m_rotationY), glm::vec3(1.0f, 0.0f, 0.0f));
	world = glm::rotate(world, glm::radians(-30.0f + m_rotationX), glm::vec3(0.0f, 1.0f, 0.0f));
	glUniformMatrix4fv(glGetUniformLocation(shaderProgramID, "world"), 1, GL_FALSE, glm::value_ptr(world));

	glUniformMatrix4fv(glGetUniformLocation(shaderProgramID, "model"), 1, GL_FALSE, glm::value_ptr(glm::mat4(1.0f)));

	XYZ->Render();

	sun->Render();
	for (int i = 0; i < 3; i++) {
		glUniformMatrix4fv(glGetUniformLocation(shaderProgramID, "model"), 1, GL_FALSE, glm::value_ptr(orbit_sun[i]->getModelMatrix()));
		orbit_sun[i]->Render();

		glUniformMatrix4fv(glGetUniformLocation(shaderProgramID, "model"), 1, GL_FALSE, glm::value_ptr(planet[i]->getModelMatrix()));
		planet[i]->Render();
		orbit_planet[i]->Render();

		glUniformMatrix4fv(glGetUniformLocation(shaderProgramID, "model"), 1, GL_FALSE, glm::value_ptr(moon[i]->getModelMatrix()));
		moon[i]->Render();
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
	case 'p': case 'P':
		// 직각/원근 투영
		isOrtho = key == 'p' ? true : false;
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
	case 'l':
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

GLvoid MouseMotion(int x, int y)
{
	m_rotationX += (x - winWidth / 2) * 0.2f;
	m_rotationY += (y - winHeight / 2) * 0.2f;

	if (m_rotationY > 59.0f) m_rotationY = 59.0f;
	if (m_rotationY < -119.0f) m_rotationY = -119.0f;

	glutWarpPointer(winWidth / 2, winHeight / 2);
}
