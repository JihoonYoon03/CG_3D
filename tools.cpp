#include "tools.h"

Model::Model(const std::string& filename, const glm::vec3& size) {
	std::ifstream file(filename);
	if (!file.is_open()) {
		std::cerr << "Error opening file: " << filename << std::endl;
		exit(EXIT_FAILURE);
	}

	std::string line;
	while (std::getline(file, line)) {
		if (line.empty()) continue;
		std::istringstream iss(line);
		std::string prefix;
		iss >> prefix;

		if (prefix == "v") {
			glm::vec3 vertex;
			iss >> vertex.x >> vertex.y >> vertex.z;
			vertices.push_back(vertex);
		}
		else if (prefix == "f") {
			std::string v1, v2, v3;
			iss >> v1 >> v2 >> v3;

			// "정점/텍스처/노멀"에서 정점 인덱스만 추출
			glm::uvec3 face;
			face.x = std::stoi(v1.substr(0, v1.find('/'))) - 1;
			face.y = std::stoi(v2.substr(0, v2.find('/'))) - 1;
			face.z = std::stoi(v3.substr(0, v3.find('/'))) - 1;

			faces.push_back(face);
		}
	}
	file.close();

	std::cout << "Loaded " << vertices.size() << " vertices, "
		<< faces.size() << " faces" << std::endl;  // 디버그 출력

	// 모델 크기 조정
	for (auto& vertex : vertices) {
		vertex *= size;
	}

	glm::vec3 min_pos(FLT_MAX), max_pos(-FLT_MAX);
	for (const auto& vertex : vertices) {
		min_pos = glm::min(min_pos, vertex);
		max_pos = glm::max(max_pos, vertex);
	}

	center = (min_pos + max_pos) * 0.5f;

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3),
		vertices.data(), GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, faces.size() * sizeof(glm::uvec3),
		faces.data(), GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (GLvoid*)0);
	glEnableVertexAttribArray(0);

	basis = new DisplayBasis(0.2f, center);
}

void Model::setDefScale(const glm::vec3& ds) {
	if (enabled) default_scale = glm::scale(glm::mat4(1.0f), ds);
}
void Model::setDefRotate(const glm::mat4& dr) {
	if (enabled) default_rotate = dr;
}
void Model::setDefTranslate(const glm::vec3& dt) {
	if (enabled) default_translate = glm::translate(glm::mat4(1.0f), dt);
}

void Model::scale(const glm::vec3& scaleFactor) {
	if (enabled) transformQueue.push(glm::scale(glm::mat4(1.0f), scaleFactor));
}

void Model::rotate(GLfloat angle, const glm::vec3& axis) {
	if (enabled) transformQueue.push(glm::rotate(glm::mat4(1.0f), glm::radians(angle), axis));
}

void Model::translate(const glm::vec3& delta) {
	if (enabled) transformQueue.push(glm::translate(glm::mat4(1.0f), delta));
}

void Model::Render() {
	if (!enabled) return;
	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, faces.size() * 3, GL_UNSIGNED_INT, 0);

	//basis->Render();
}

glm::vec3 Model::retDistTo(const glm::vec3& origin) {
	glm::vec4 worldLocation = modelMatrix * default_translate * default_rotate * default_scale * glm::vec4(center, 1.0f);
	return glm::vec3(worldLocation) - origin;
}

glm::mat4 Model::getModelMatrix() {
	while (!transformQueue.empty()) {
		modelMatrix = transformQueue.front() * modelMatrix;
		transformQueue.pop();
	}
	return modelMatrix * default_translate * default_rotate * default_scale;
}

void Model::resetModelMatrix() {
	modelMatrix = glm::mat4(1.0f);
	for (int i = 0; i < transformQueue.size(); i++) {
		transformQueue.pop();
	}
}

DisplayBasis::DisplayBasis(GLfloat offset, const glm::vec3& origin) : origin(origin) {

	xyz[0][0] = { origin, { 0.1f, 0.1f, 0.0f } };
	xyz[0][1] = { origin + glm::vec3(1.0f, 0.0f, 0.0f), { 1.0f, 0.0f, 0.0f } };
	xyz[1][0] = { origin, { 0.0f, 0.1f, 0.1f } };
	xyz[1][1] = { origin + glm::vec3(0.0f, 1.0f, 0.0f), { 0.0f, 1.0f, 0.0f } };
	xyz[2][0] = { origin, { 0.1f, 0.0f, 0.1f } };
	xyz[2][1] = { origin + glm::vec3(0.0f, 0.0f, 1.0f), { 0.0f, 0.0f, 1.0f } };

	for (int i = 0; i < 3; i++) {
		xyz[i][0].pos *= offset;
		xyz[i][1].pos *= offset;
	}
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	glBufferData(GL_ARRAY_BUFFER, sizeof(xyz), xyz, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(ColoredVertex), (GLvoid*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(ColoredVertex), (GLvoid*)sizeof(glm::vec3));
	glEnableVertexAttribArray(1);
}

void DisplayBasis::Render() {
	glBindVertexArray(VAO);
	glDrawArrays(GL_LINES, 0, 6);
}

glm::vec3 randColor() {
	glm::vec3 color;
	color.x = rand() / static_cast<GLfloat>(RAND_MAX);
	color.y = rand() / static_cast<GLfloat>(RAND_MAX);
	color.z = rand() / static_cast<GLfloat>(RAND_MAX);
	return color;
}

rtPos randSquarePos(GLfloat offset) {
	rtPos pos;
	pos.x1 = rand() / static_cast<GLfloat>(RAND_MAX) * 2.0f - 1.0f;
	pos.y1 = rand() / static_cast<GLfloat>(RAND_MAX) * 2.0f - 1.0f;

	pos.x2 = pos.x1 + offset;
	pos.y2 = pos.y1 - offset;
	return pos;
}

rtPos randRectPos(GLfloat offset) {
	rtPos pos;
	pos.x1 = rand() / static_cast<GLfloat>(RAND_MAX) * 2.0f - 1.0f;
	pos.y1 = rand() / static_cast<GLfloat>(RAND_MAX) * 2.0f - 1.0f;

	pos.x2 = pos.x1 + (rand() / static_cast<GLfloat>(RAND_MAX) * 0.1f + offset);
	pos.y2 = pos.y1 - (rand() / static_cast<GLfloat>(RAND_MAX) * 0.1f + offset);
	return pos;
}

void mPosToGL(GLuint winWidth, GLuint winHeight, int mx, int my, GLfloat& xGL, GLfloat& yGL)
{
	xGL = (mx / (winWidth / 2.0f)) - 1.0f;
	yGL = 1.0f - (my / (winHeight / 2.0f));
}

bool isMouseIn(rtPos& pos, GLuint winWidth, GLuint winHeight, int mx, int my)
{
	GLfloat xGL, yGL;
	mPosToGL(winWidth, winHeight, mx, my, xGL, yGL);

	if (xGL > pos.x1 && xGL < pos.x2 && yGL < pos.y1 && yGL > pos.y2) return true;
	else return false;
}

bool CircleCollider(const glm::vec3& center, GLfloat distCap, GLfloat xGL, GLfloat yGL) {
	GLfloat dist = sqrt((center.x - xGL) * (center.x - xGL) + (center.y - yGL) * (center.y - yGL));
	if (dist < distCap) {
		return true;
	}
	return false;
}

bool LineCollider(const glm::vec3& p1, const glm::vec3& p2, GLfloat distCap, GLfloat xGL, GLfloat yGL) {
	GLfloat m;
	m = (p2.y - p1.y) / (p2.x - p1.x);	// 기울기

	// y - y1 = m(x - x1)
	GLfloat xOnLine = (yGL - p1.y) / m + p1.x; // y좌표에 대응하는 x좌표
	GLfloat yOnLine = m * (xGL - p1.x) + p1.y; // x좌표에 대응하는 y좌표

	GLfloat minX = std::min(p1.x, p2.x) - distCap * 0.5f;
	GLfloat maxX = std::max(p1.x, p2.x) + distCap * 0.5f;
	GLfloat minY = std::min(p1.y, p2.y) - distCap * 0.5f;
	GLfloat maxY = std::max(p1.y, p2.y) + distCap * 0.5f;

	// 선분 범위 내에 있는지 확인
	if (xGL < minX || xGL > maxX || yGL < minY || yGL > maxY) {
		return false;
	}

	if (abs(xOnLine - xGL) < distCap && abs(yOnLine - yGL) < distCap) {
		return true;
	}

	return false;
}

bool RectCollider(const glm::vec3& p1, const glm::vec3& p2, GLfloat xGL, GLfloat yGL) {
	GLfloat minX = std::min(p1.x, p2.x);
	GLfloat maxX = std::max(p1.x, p2.x);
	GLfloat minY = std::min(p1.y, p2.y);
	GLfloat maxY = std::max(p1.y, p2.y);

	if (xGL > minX && xGL < maxX && yGL > minY && yGL < maxY) return true;
	else return false;
}

std::string read_file(const std::string& filename) {
	std::ifstream file(filename);
	if (!file.is_open()) {
		std::cerr << "Error: Could not open file " << filename << std::endl;
		return "";
	}

	std::stringstream buffer;
	buffer << file.rdbuf();
	return buffer.str();
}

void make_vertexShaders(GLuint& vertexShader, const std::string& shaderName) {
	std::string vertexSource = read_file(shaderName);
	if (vertexSource.empty()) {
		std::cerr << "ERROR: Failed to read vertex shader file" << std::endl;
		return;
	}

	const char* sourcePtr = vertexSource.c_str();
	vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &sourcePtr, nullptr);
	glCompileShader(vertexShader);

	GLint result;
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &result);
	if (!result) {
		GLchar errorLog[512];
		glGetShaderInfoLog(vertexShader, 512, nullptr, errorLog);
		std::cerr << "ERROR: vertex shader 컴파일 실패\n" << errorLog << std::endl;
	}
}

void make_fragmentShaders(GLuint& fragmentShader, const std::string& shaderName) {
	std::string fragmentSource = read_file(shaderName);
	if (fragmentSource.empty()) {
		std::cerr << "ERROR: Failed to read fragment shader file" << std::endl;
		return;
	}

	const char* sourcePtr = fragmentSource.c_str();
	fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &sourcePtr, nullptr);
	glCompileShader(fragmentShader);

	GLint result;
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &result);
	if (!result) {
		GLchar errorLog[512];
		glGetShaderInfoLog(fragmentShader, 512, nullptr, errorLog);
		std::cerr << "ERROR: fragment shader 컴파일 실패\n" << errorLog << std::endl;
	}
}

GLuint make_shaderProgram(const GLuint& vertexShader, const GLuint& fragmentShader) {
	GLuint shaderID = glCreateProgram();
	glAttachShader(shaderID, vertexShader);
	glAttachShader(shaderID, fragmentShader);
	glLinkProgram(shaderID);

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	GLint result;
	glGetProgramiv(shaderID, GL_LINK_STATUS, &result);
	if (!result) {
		GLchar errorLog[512];
		glGetProgramInfoLog(shaderID, 512, nullptr, errorLog);
		std::cerr << "ERROR: shader program 연결 실패\n" << errorLog << std::endl;
		return 0;
	}

	glUseProgram(shaderID);
	return shaderID;
}