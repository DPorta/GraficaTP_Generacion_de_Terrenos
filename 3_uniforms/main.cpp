#include <glutil.h>
#include <vector>
#include <math.h>
#include <stdlib.h>
#include <time.h>
#include "Cube.h"
#include "Files.h"
#include "Model.h"
#include "Camera.h"

const u32 FSIZE = sizeof(f32);
const u32 ISIZE = sizeof(i32);
const u32 SCR_WIDTH = 1080;
const u32 SCR_HEIGHT = 720;
const f32  ASPECT = (f32)SCR_WIDTH / (f32)SCR_HEIGHT;


//PERLIN NOISE TERRENO
u32 y_level = 7;
const u32 x_max{ 50 };
const u32 z_max{ 50 };
double gradient[x_max][z_max][3];
#include "PerlinNoiseTerrain.h"
double terreno[x_max][z_max];
std::vector<glm::vec4> transition;

Cam* cam = new Cam();
f32  lastx;
f32  lasty;
bool firstMouse = true;
f32  deltaTime = 0.0f;
f32  lastFrame = 0.0f;
bool wireframe = false;

//keyboard input processing

void processInput(GLFWwindow* window) {
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, true);
	}
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
		cam->processKeyboard(FORWARD, deltaTime);
	}
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
		cam->processKeyboard(LEFT, deltaTime);
	}
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
		cam->processKeyboard(BACKWARD, deltaTime);
	}
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
		cam->processKeyboard(RIGHT, deltaTime);
	}
}

void mouse_callback(GLFWwindow* window, f64 xpos, f64 ypos) {
	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
		if (firstMouse) {
			lastx = xpos;
			lasty = ypos;
			firstMouse = false;
			return;
		}
		cam->processMouse((f32)(xpos - lastx), (f32)(lasty - ypos));
		lastx = xpos;
		lasty = ypos;
	}
	else {
		firstMouse = true;
	}
}

void key_callback(GLFWwindow*, int key, int, int act, int) {
	wireframe ^= key == GLFW_KEY_E && act == GLFW_PRESS;
}

void scroll_callback(GLFWwindow* window, f64 xoffset, f64 yoffset) {
	cam->processScroll((f32)yoffset);
}

i32 main() {

	GLFWwindow* window = glutilInit(3, 3, SCR_WIDTH, SCR_HEIGHT, "Terreno: Campo");

	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);

	//PATH de texturas y objetos
	Files* files = new Files("bin", "resources/textures", "resources/objects");

	//Ruta de shaders
	Shader* shader = new Shader(files, "shader.vert", "shader.frag");
	Shader* lightingShader = new Shader(files, "shader2.vert", "shader2.frag");
	Shader* objShader = new Shader(files, "shader3.vert", "shader3.frag");

	//Ruta de objetos
	Model* creeper = new Model(files, "creeper/creeper.obj");
	Model* airplane2 = new Model(files, "airplane2/piper_pa18.obj");
	Model* estatua = new Model(files, "estatua/estatua.obj");

	//TERRENO
	MyTerrain* miTerreno = new MyTerrain();

	glm::vec3 lightPos(25.0f, 20.0f, 50.0f);
	glm::vec3 lightColor(1.0f, 1.0f, 1.0f);

	miTerreno->genTerrain<x_max, z_max>(terreno);
	miTerreno->genTerrainTransitions<x_max, z_max>(terreno, transition, y_level);

	cam = new Cam();
	Cube* cubex = new Cube();

	//VBO, VAO, EBO
	u32 vbo, vao, lightCubeVao, ebo;
	glGenVertexArrays(1, &vao);
	glGenVertexArrays(1, &lightCubeVao);
	glGenBuffers(1, &vbo);
	glGenBuffers(1, &ebo);

	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);

	glBufferData(GL_ARRAY_BUFFER, cubex->getVSize() * FSIZE, cubex->getVertices(), GL_STATIC_DRAW);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, cubex->getVSize() * ISIZE, cubex->getIndices(), GL_STATIC_DRAW);

	// posiciones
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, cubex->len(), cubex->skip(0));
	glEnableVertexAttribArray(0);
	// colores
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, cubex->len(), cubex->skip(3));
	glEnableVertexAttribArray(1);
	// normales: ojo que es el 3er comp, por eso offset es 6
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, cubex->len(), cubex->skip(6));
	glEnableVertexAttribArray(2);
	// texturas
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, cubex->len(), cubex->skip(9));
	glEnableVertexAttribArray(3);

	//CARGANDO TEXTURAS
	u32 texture0 = TextureFromFile("resources/textures/arena.jpg");
	u32 texture1 = TextureFromFile("resources/textures/agua.jpg");
	u32 texture2 = TextureFromFile("resources/textures/pasto.jpg");

	shader->use();
	shader->setI32("texture0", 0);
	// luz
	glBindVertexArray(lightCubeVao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, cubex->len(), cubex->skip(0));
	glEnableVertexAttribArray(0);


	glEnable(GL_DEPTH_TEST);

	while (!glfwWindowShouldClose(window)) {
		f32 currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;


		processInput(window);
		glClearColor(0.98f, 0.45f, 0.0f, 0.5f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glPolygonMode(GL_FRONT_AND_BACK, wireframe ? GL_LINE : GL_FILL);

		//ACTIVANDO TEXTURA
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture0);

		shader->use();
		//MATRICES PARA LA CAMARA
		glm::mat4 projection = glm::perspective(cam->getZoom(), ASPECT, 0.1f, 100.0f);
		shader->setMat4("proj", projection);
		glm::mat4 model = glm::mat4(1.0f);
		glm::mat4 view = glm::mat4(1.0f);
		shader->setMat4("view", cam->getViewM4());

		glBindVertexArray(vao);

		shader->use();
		shader->setVec3("xyz", lightPos);
		shader->setVec3("xyzColor", lightColor);
		shader->setVec3("xyzView", cam->getPos());

		//imprimir terreno dependiendo del nivel
		for (auto vec : transition)
		{
			if (vec.y <= 0.3) {
				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_2D, texture1);
			}
			else if (vec.y > 0.5) {
				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_2D, texture2);
			}
			else {
				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_2D, texture0);
			}
			model = glm::mat4(1.0f);
			model = glm::translate(model, glm::vec3(vec.x, vec.y, vec.z) * glm::vec3(2.0f, 2.0f, 2.0f));
			shader->setMat4("model", model);
			glDrawElements(GL_TRIANGLES, 6 * 6, GL_UNSIGNED_INT, 0);
		}
		//CUBO DE ILUMINACION
		glBindVertexArray(lightCubeVao);
		lightingShader->use();
		lightingShader->setMat4("proj", projection);
		lightingShader->setMat4("view", cam->getViewM4());

		model = glm::mat4(1.0f);
		model = glm::translate(model, lightPos);
		model = glm::scale(model, glm::vec3(0.3f));
		lightingShader->setMat4("model", model);
		glDrawElements(GL_TRIANGLES, cubex->getISize(), GL_UNSIGNED_INT, 0);

		//CARGANDO LOS OBJETOS
		objShader->use();
		objShader->setVec3("xyz", lightPos);
		objShader->setVec3("xyzColor", lightColor);
		objShader->setVec3("xyzView", cam->getPos());
		objShader->setMat4("proj", projection);
		objShader->setMat4("view", cam->getViewM4());

		model = glm::mat4(1.0f);
		model = translate(model, glm::vec3(30.0f+sin(currentFrame)*2,5.0f,30.0f));
		model = glm::rotate(model, currentFrame, glm::vec3(0.0f, 0.6f, 0.0f));
		model = glm::scale(model, glm::vec3(0.5f));
		objShader->setMat4("model", model);
		creeper->Draw(objShader);

		/*model = translate(model, glm::vec3(su posicion));
		objShader->setMat4("model", model);
		creeper->Draw(objShader);*/
		//model = glm::rotate(model, 0.5f, glm::vec3(0.0f, 40.0f, 0.0f));
		//objShader->setMat4("model", model);
		model = translate(model, glm::vec3(20.0f + sin(currentFrame) * 2, 20.0f, 30.0f));
		model = glm::rotate(model, currentFrame/6, glm::vec3(0.0f, (20.0f + sin(currentFrame) * 2), 0.0f));
		model = glm::scale(model, glm::vec3(2.0f));
		objShader->setMat4("model", model);
		airplane2->Draw(objShader);

		//Implementando estatua
		model = glm::mat4(1.0f);
		model = translate(model, glm::vec3(25.0f, 2.0f, 30.0f));
		model = glm::scale(model, glm::vec3(0.1f));
		objShader->setMat4("model", model);
		estatua->Draw(objShader);
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glDeleteVertexArrays(1, &vao);
	glDeleteBuffers(1, &vbo);
	glDeleteBuffers(1, &ebo);

	delete lightingShader;
	delete shader;
	delete cam;
	delete cubex;
	delete creeper;

	return 0;
}
