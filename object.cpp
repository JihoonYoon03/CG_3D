#include "object.h"

ObjCube::ObjCube(glm::vec3 scaling, glm::vec3 rotation, glm::vec3 location, glm::vec3 color) : center(location) {
	// 초기 변환 행렬 설정
	initial_translate = glm::translate(glm::mat4(1.0f), location);

	initial_rotate = glm::mat4(1.0f);
	initial_rotate = glm::rotate(initial_rotate, glm::radians(rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
	initial_rotate = glm::rotate(initial_rotate, glm::radians(rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
	initial_rotate = glm::rotate(initial_rotate, glm::radians(rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));

	initial_scale = glm::scale(glm::mat4(1.0f), scaling);

	// 원본 정점 데이터를 vertices 배열에 복사
	for (int i = 0; i < 24; i++) {
		vertices[i] = original_vertices[i];
		colors[i] = color;
	}

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VERTEX);
	glGenBuffers(1, &FACE);
	glGenBuffers(1, &COLOR);
	glGenBuffers(1, &NORMAL);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VERTEX);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (GLvoid*)0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, FACE);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, COLOR);
	glBufferData(GL_ARRAY_BUFFER, sizeof(colors), colors, GL_STATIC_DRAW);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (GLvoid*)0);
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, NORMAL);
	glBufferData(GL_ARRAY_BUFFER, sizeof(normals), normals, GL_STATIC_DRAW);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (GLvoid*)0);
	glEnableVertexAttribArray(2);
}

void ObjCube::scaling(glm::vec3 amount) {
	transform_queue.push(glm::scale(glm::mat4(1.0f), amount));
}

void ObjCube::rotating(glm::vec3 amount) {
	glm::mat4 rotate = glm::mat4(1.0f);
	rotate = glm::rotate(rotate, glm::radians(amount.x), glm::vec3(1.0f, 0.0f, 0.0f));
	rotate = glm::rotate(rotate, glm::radians(amount.y), glm::vec3(0.0f, 1.0f, 0.0f));
	rotate = glm::rotate(rotate, glm::radians(amount.z), glm::vec3(0.0f, 0.0f, 1.0f));
	transform_queue.push(rotate);
}

void ObjCube::translating(glm::vec3 amount) {
	transform_queue.push(glm::translate(glm::mat4(1.0f), amount));
}

glm::mat4 ObjCube::getModelMatrix() {
	while (!transform_queue.empty()) {
		additional_transform *= transform_queue.front();
		transform_queue.pop();
	}

	// 추가 변환 * 초기 변환
	return additional_transform * initial_translate * initial_rotate * initial_scale;
}

glm::vec3 ObjCube::getCenter() {
	glm::mat4 model = getModelMatrix();  // 큐 처리
	return glm::vec3(model * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
}

void ObjCube::Render() {
	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
}

void ObjCube::reset() {
	while (!transform_queue.empty())
		transform_queue.pop();
	additional_transform = glm::mat4(1.0f);
}

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

		if (shaderProgramID) {
			GLuint ptToOrigin = glGetUniformLocation(shaderProgramID, "moveToOrigin");
			GLuint ptID = glGetUniformLocation(shaderProgramID, "faceID");

			glUniform3f(ptToOrigin, toOrigin[i].x, toOrigin[i].y, toOrigin[i].z);	
			glUniform1i(ptID, i);
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

ObjPyramid::ObjPyramid(glm::vec3 scaling, glm::vec3 rotation, glm::vec3 location, glm::vec3 color) : center(location) {
	// 초기 변환 행렬 설정
	initial_translate = glm::translate(glm::mat4(1.0f), location);

	initial_rotate = glm::mat4(1.0f);
	initial_rotate = glm::rotate(initial_rotate, glm::radians(rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
	initial_rotate = glm::rotate(initial_rotate, glm::radians(rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
	initial_rotate = glm::rotate(initial_rotate, glm::radians(rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));

	initial_scale = glm::scale(glm::mat4(1.0f), scaling);

	// 원본 정점 데이터를 vertices 배열에 복사
	for (int i = 0; i < 24; i++) {
		vertices[i] = original_vertices[i];
		colors[i] = color;
	}

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VERTEX);
	glGenBuffers(1, &FACE);
	glGenBuffers(1, &COLOR);
	glGenBuffers(1, &NORMAL);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VERTEX);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (GLvoid*)0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, FACE);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, COLOR);
	glBufferData(GL_ARRAY_BUFFER, sizeof(colors), colors, GL_STATIC_DRAW);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (GLvoid*)0);
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, NORMAL);
	glBufferData(GL_ARRAY_BUFFER, sizeof(normals), normals, GL_STATIC_DRAW);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (GLvoid*)0);
	glEnableVertexAttribArray(2);
}

void ObjPyramid::scaling(glm::vec3 amount) {
	transform_queue.push(glm::scale(glm::mat4(1.0f), amount));
}

void ObjPyramid::rotating(glm::vec3 amount) {
	glm::mat4 rotate = glm::mat4(1.0f);
	rotate = glm::rotate(rotate, glm::radians(amount.x), glm::vec3(1.0f, 0.0f, 0.0f));
	rotate = glm::rotate(rotate, glm::radians(amount.y), glm::vec3(0.0f, 1.0f, 0.0f));
	rotate = glm::rotate(rotate, glm::radians(amount.z), glm::vec3(0.0f, 0.0f, 1.0f));
	transform_queue.push(rotate);
}

void ObjPyramid::translating(glm::vec3 amount) {
	transform_queue.push(glm::translate(glm::mat4(1.0f), amount));
}

glm::mat4 ObjPyramid::getModelMatrix() {
	while (!transform_queue.empty()) {
		additional_transform *= transform_queue.front();
		transform_queue.pop();
	}

	// 추가 변환 * 초기 변환
	return additional_transform * initial_translate * initial_rotate * initial_scale;
}

glm::vec3 ObjPyramid::getCenter() {
	glm::mat4 model = getModelMatrix();  // 큐 처리
	return glm::vec3(model * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
}

void ObjPyramid::Render() {
	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
}

void ObjPyramid::reset() {
	while (!transform_queue.empty())
		transform_queue.pop();
	additional_transform = glm::mat4(1.0f);
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

		if (shaderProgramID) {
			GLuint ptToOrigin = glGetUniformLocation(shaderProgramID, "moveToOrigin");
			GLuint ptID = glGetUniformLocation(shaderProgramID, "faceID");

			glUniform3f(ptToOrigin, toOrigin[i].x, toOrigin[i].y, toOrigin[i].z);
			glUniform1i(ptID, i + 6);
		}
		if (i < 4)
			glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, (void*)(i * 3 * sizeof(GLuint)));
		else
			glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)(12 * sizeof(GLuint)));
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