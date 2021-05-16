#include <glutil.h>
#include <vector>
#include <math.h>
#include <stdlib.h>
#include <time.h>

const ui32 FSIZE = sizeof(f32);
const ui32 ISIZE = sizeof(i32);
const ui32 SCR_WIDTH = 1280;
const ui32 SCR_HEIGHT = 720;
const f32  ASPECT = (f32)SCR_WIDTH / (f32)SCR_HEIGHT;

//valores para la camara
glm::vec3 position = glm::vec3(0.0f, 1.5f, 10.0f);
glm::vec3 front = glm::vec3(0.0f, 1.0f, -1.0f);
glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);

//para el mouse
bool firstMouse = true;
f32 yaw = -90.0f;
f32 pitch = 0.0f;
f32 lastX = SCR_WIDTH / 2.0f;
f32 lastY = SCR_HEIGHT / 2.0f;

//para el scroll (zoom)
f32 fov = 45.0f;

//para el tiempo y que el programa se vea mas suave
f32 deltaTime = 0.0f;
f32 lastFrame = 0.0f;

class Cube {
	f32 width;
	f32 height;
	f32 depth;

	f32* vertices;
	ui32* indices;
public:
	Cube(f32 width = 1.0f, f32 height = 1.0f, f32 depth = 1.0f)
		:width(width), height(height), depth(depth),
		vertices(new f32[16*8]), indices(new ui32[6*6]) {

		f32 wm = width / 2.0f;
		f32 hm = width / 2.0f;
		f32 dm = width / 2.0f;

		//temporal de vertices
		f32 temp[]= {
			//vertices(posiciones)  //colores         //coord. textura
			//cara de los lados
			-wm,  hm,  dm,    0.5f,0.5f,0.5f,     0.0f,1.0f,
			 wm,  hm,  dm,    0.5f,0.5f,0.5f,     1.0f,1.0f,
			-wm, -hm,  dm,    0.5f,0.5f,0.5f,     0.0f,0.0f,
			 wm, -hm,  dm,    0.5f,0.5f,0.5f,     1.0f,0.0f,
			-wm,  hm, -dm,    0.5f,0.5f,0.5f,     1.0f,1.0f,
			 wm,  hm, -dm,    0.5f,0.5f,0.5f,     0.0f,1.0f,
			-wm, -hm, -dm,    0.5f,0.5f,0.5f,     1.0f,0.0f,
			 wm, -hm, -dm,    0.5f,0.5f,0.5f,     0.0f,0.0f,
			//cara de arriba
		   -wm,  hm,  dm,     1.0f, 1.0f, 1.0f,    0.0f, 0.0f,
			wm,  hm,  dm,     1.0f, 1.0f, 1.0f,    1.0f, 0.0f,
		   -wm, -hm,  dm,     1.0f, 1.0f, 1.0f,    0.0f, 0.0f,
			wm, -hm,  dm,     1.0f, 1.0f, 1.0f,    1.0f, 0.0f,
		   -wm,  hm, -dm,     1.0f, 1.0f, 1.0f,    0.0f, 1.0f,
			wm,  hm, -dm,     1.0f, 1.0f, 1.0f,    1.0f, 1.0f,
		   -wm, -hm, -dm,     1.0f, 1.0f, 1.0f,    0.0f, 1.0f,
			wm, -hm, -dm,     1.0f, 1.0f, 1.0f,    1.0f, 1.0f };

		for (ui32 i = 0; i < 16*8; ++i)
		{
			vertices[i] = temp[i];
		}
		//temporal de indices
		ui32 temp2[] = { 
			0,  1,  2,		1,2,3,
			8,  9,  12,		9,12,13,
			1,  5,  3,		3,5,7,
			11, 14, 15,	    10,11,14,
			0,  4,  6,		0,2,6,
			4,  5,  6,		5,6,7 };

		for (ui32 i = 0; i < 6 * 6; ++i)
		{
			indices[i] = temp2[i];
		}
	}
	~Cube() {
		delete[] vertices;
		delete[] indices;
	}
	f32* getVertices() {
		return vertices;
	}
	ui32* getIndices() {
		return indices;
	}
	f32 getVsize() {
		return 16 * 8;
	}
	ui32 getIsize() {
		return 6 * 6;
	}
};

//procesar teclas
void processInput(GLFWwindow* window) {


	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, true);
	}
	f32 speed = 2.5 * deltaTime;
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
		position += speed * front;
	}
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
		position -= glm::normalize(glm::cross(front, up)) * speed;
	}
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
		position -= speed * front;
	}
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
		position += glm::normalize(glm::cross(front, up)) * speed;
	}

}

//procesar mouse
void mouseCallBack(GLFWwindow*window, f64 xpos, f64 ypos) {
	if (firstMouse) {
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}
	f32 xoffset = xpos - lastX;
	f32 yoffset = lastY - ypos;

	lastX = xpos;
	lastY = ypos;

	f32 sensitivity = 0.05f;
	xoffset *= sensitivity;
	yoffset *= sensitivity;

	yaw += xoffset;
	pitch += yoffset;

	if (pitch > 1.0f) {
		pitch = 1.0f;
	}
	else if (pitch <-1.0f) {
		pitch = -1.0f;
	}
	glm::vec3 f;
	f.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	f.y = sin(glm::radians(pitch));
	f.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
	front = glm::normalize(f);
}

//procesar scroll
void scrollCallBack(GLFWwindow* , f64 xoffset, f64 yoffset) {
	fov -= (f32)yoffset; //para el zoom
	if (fov < 1.0f) {
		fov = 1.0f;
	}
	else if (fov > 45.0f) {
		fov = 45.0f;
	}

}


i32 main() {
	GLFWwindow* window = glutilInit(3, 3, SCR_WIDTH, SCR_HEIGHT, "Terreno");
	Shader* shader = new Shader();
	Shader* shader2 = new Shader();
	glfwSetCursorPosCallback(window, mouseCallBack);
	glfwSetScrollCallback(window, scrollCallBack);

	Cube* cubex = new Cube();
	
	//FALTA: CREAR FUNCION PARA GENERAR DESNIVELES
	srand(time(0));
	auto rndb = [](f32 a, f32 b) {
		return (rand() % 100) / 100.0f ;
	};
	
	//CREANDO POSICIONES: REMPLAZAR "Y" POR EL LAMBDA DE ARRIBA PARA EL DESNIVEL
	ui32 n = 50;
	std::vector<glm::vec3> positions(n*n);
	for (ui32 i = 0; i < n; ++i) {
		for (ui32 j = 0; j < n; ++j) {
			f32 x = i - n / 2.0f;
			f32 z = j - n / 2.0f;
			f32 y = rndb(x*0.01,z*0.01)/2.0f;
			positions[i * n + j] = glm::vec3(x, y, z);
		}
	}
	//random para crear textura que no sea el mar
	std::vector<int>randomNumbers(positions.size());
	auto getrandom = []() {
		return rand() % 3 + 1;
	};
	for (int i = 0; i < positions.size(); ++i) {
		randomNumbers[i] = getrandom();
	}
	
	//VBO, VAO, EBO
	ui32 vbo, vao, ebo;
	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vbo);
	glGenBuffers(1, &ebo);

	glBindVertexArray(vao);

	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	
	glBufferData(GL_ARRAY_BUFFER, cubex->getVsize()*FSIZE, cubex->getVertices(), GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);

	glBufferData(GL_ELEMENT_ARRAY_BUFFER, cubex->getIsize()*ISIZE, cubex->getIndices(), GL_STATIC_DRAW);

	// posiciones
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * FSIZE, (void*)0);
	glEnableVertexAttribArray(0);
	// colores
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * FSIZE, (void*)(3 * FSIZE));
	glEnableVertexAttribArray(1);
	// texturas
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * FSIZE, (void*)(6 * FSIZE));
	glEnableVertexAttribArray(2);

	//CARGANDO TEXTURAS
	ui32 texture0 = shader->loadTexture("arena.jpg");
	ui32 texture1 = shader->loadTexture("agua.jpg");
	ui32 texture2 = shader->loadTexture("pasto.jpg");

	shader->useProgram();
	shader->setI32("texture0", 0);

	glEnable(GL_DEPTH_TEST);
	
	while (!glfwWindowShouldClose(window)) {
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		processInput(window);
		glClearColor(0.98f, 0.45f, 0.0f, 0.5f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//ACTIVANDO TEXTURA
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture0);
		
		shader->useProgram();
		
		//MATRICES PARA LA CAMARA
		glm::mat4 projection = glm::perspective(glm::radians(fov), ASPECT, 0.1f, 100.0f);
		shader->setMat4("proj", projection);

		glm::mat4 view = glm::mat4(1.0f);
		view = glm::lookAt(position,  position + front, up);
		shader->setMat4("view", view);

		glBindVertexArray(vao);

		//USO DE TEXTURAS PARA LA PLAYA 
		for (ui32 i = 0; i < positions.size()/2; ++i) {
			if (randomNumbers[i] == 1) {
				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_2D, texture0);
			}
			else {
				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_2D, texture2);
			}
				glm::mat4 model = glm::mat4(1.0f);
				model = glm::translate(model, positions[i]);
				shader->setMat4("model", model);
				glDrawElements(GL_TRIANGLES, 6 * 6, GL_UNSIGNED_INT, 0);
		}

		//USO DE SEGUNDA TEXTURA PARA EL MAR
		
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture1);

		for (ui32 i = (positions.size() / 2) + 1; i < positions.size(); ++i) {
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, positions[i]);
		shader->setMat4("model", model);
		glDrawElements(GL_TRIANGLES, 6 * 6, GL_UNSIGNED_INT, 0);
		}

		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	glDeleteVertexArrays(1, &vao);
	glDeleteBuffers(1, &vbo);
	glDeleteBuffers(1, &ebo);

	delete shader;
	delete cubex;

	return 0;
}
