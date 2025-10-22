#include "object.h"

Cube::Cube(GLfloat offset) {
	for (int i = 0; i < sizeof(vertices) / sizeof(ColoredVertex); i++) {
		vertices[i].pos *= offset;
	}

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(ColoredVertex), (GLvoid*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(ColoredVertex), (GLvoid*)sizeof(glm::vec3));
	glEnableVertexAttribArray(1);
}

void Cube::Render() {
	glBindVertexArray(VAO);
	for (int i = 0; i < 6; i++) {
		if (faceToggle[i]) continue;

		// 해당 면 특수 변환여부 확인
		if (shaderProgramID) {
			GLint location = glGetUniformLocation(shaderProgramID, "useTranslation");
			GLint offset = glGetUniformLocation(shaderProgramID, "moveToOrigin");
			if (faceModify[i]) {
				if (location != -1) {
					glUniform1i(location, true);
					glUniform3f(offset, toOrigin[i].x, toOrigin[i].y, toOrigin[i].z);
				}
			}
			else
				glUniform1i(location, false);
		}
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)(i * 6 * sizeof(GLuint)));
	}
}

void Cube::DisplayOnly(int index) {
	if (index < 0 || index >= 6) return;
	if (lastDisplayFace == index) {
		for (int i = 0; i < 6; i++) {
			faceToggle[i] = false;
		}
		lastDisplayFace = -1;
		return;
	}

	for (int i = 0; i < 6; i++) {
		faceToggle[i] = true;
	}
	faceToggle[index] = false;
	lastDisplayFace = index;
}

void Cube::DisplayRandom() {
	lastDisplayFace = -1;
	for (int i = 0; i < 6; i++) {
		faceToggle[i] = true;
	}
	int cnt = 0;
	do {
		int index = rand() % 6;
		if (faceToggle[index] == false) continue;

		cnt++;
		faceToggle[index] = false;
	} while (cnt < 2);
}

void Cube::modifyFace(int index, bool toggle) {
	if (index < 0 || index >= 6) return;
	if (toggle)
		faceModify[index] = !faceModify[index];
	else
		faceModify[index] = false;
}

Pyramid::Pyramid(GLfloat offset) {
	for (int i = 0; i < sizeof(vertices) / sizeof(ColoredVertex); i++) {
		vertices[i].pos *= offset;
	}
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(ColoredVertex), (GLvoid*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(ColoredVertex), (GLvoid*)sizeof(glm::vec3));
	glEnableVertexAttribArray(1);
}

void Pyramid::Render() {
	glBindVertexArray(VAO);
	for (int i = 0; i < 5; i++) {
		if (faceToggle[i]) continue;

		// 해당 면 특수 변환여부 확인
		if (shaderProgramID) {
			GLint location = glGetUniformLocation(shaderProgramID, "useTranslation");
			if (faceModify[i]) {
				if (location != -1) {
					glUniform1i(location, true);
				}
			}
			else
				glUniform1i(location, false);
		}
		if (i < 4) {
			glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, (void*)(i * 3 * sizeof(GLuint)));
		}
		else {
			glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)(i * 3 * sizeof(GLuint)));
		}
	}
}

void Pyramid::DisplayOnly(int index) {
	if (index < 0 || index >= 5) return;
	if (lastDisplayFace == index) {
		for (int i = 0; i < 5; i++) {
			faceToggle[i] = false;
		}
		lastDisplayFace = -1;
		return;
	}
	for (int i = 0; i < 5; i++) {
		faceToggle[i] = true;
	}
	faceToggle[index] = false;
	lastDisplayFace = index;
}

void Pyramid::DisplayRandom() {
	lastDisplayFace = -1;
	for (int i = 0; i < 4; i++) {
		faceToggle[i] = true;
	}
	faceToggle[rand() % 4] = false;
}

void Pyramid::modifyFace(int index, bool toggle) {
	if (index < 0 || index >= 5) return;
	if (toggle)
		faceModify[index] = !faceModify[index];
	else
		faceModify[index] = false;
}