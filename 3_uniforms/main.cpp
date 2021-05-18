#include <glutil.h>
#include <vector>
#include <math.h>
#include <stdlib.h>
#include <time.h>
#include "Cube.h"

const ui32 FSIZE = sizeof(f32);
const ui32 ISIZE = sizeof(i32);
const ui32 SCR_WIDTH = 1280;
const ui32 SCR_HEIGHT = 720;
const f32  ASPECT = (f32)SCR_WIDTH / (f32)SCR_HEIGHT;

//valores para la camara
glm::vec3 position = glm::vec3(25.0f, 10.0f, 50.0f);
glm::vec3 front = glm::vec3(25.0f, -10.0f, 0.0f);
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

//PERLIN NOISE TERRENO
ui32 y_level = 7;
const ui32 x_max{ 50 };
const ui32 z_max{ 50 };
double gradient[x_max][z_max][3];
#include "PerlinNoiseTerrain.h"
double terreno[x_max][z_max];
std::vector<glm::vec4> transition;

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

	if (pitch > 89.0f) {
		pitch = 89.0f;
	}
	else if (pitch <-89.0f) {
		pitch = -89.0f;
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

	GLFWwindow* window = glutilInit(3, 3, SCR_WIDTH, SCR_HEIGHT, "Terreno: Campo");
	Shader* shader = new Shader();
	Cube* cubex = new Cube();
	MyTerrain* miTerreno = new MyTerrain();

	miTerreno->genTerrain<x_max, z_max>(terreno);
	miTerreno->genTerrainTransitions<x_max, z_max>(terreno, transition, y_level);

	glfwSetCursorPosCallback(window, mouseCallBack);
	glfwSetScrollCallback(window, scrollCallBack);
		
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

		//imprimir terreno dependiendo del nivel
		for (auto vec : transition)
		{
			if (vec.y <= 0.3) {
				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_2D, texture1);
			}
			else if (vec.y > 0.5){
				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_2D, texture2);
			}
			else {
				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_2D, texture0);
			}
			glm::mat4 model = glm::mat4(1.0f);
			model = glm::translate(model, glm::vec3(vec.x, vec.y, vec.z) * glm::vec3(2.0f, 2.0f, 2.0f));
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
