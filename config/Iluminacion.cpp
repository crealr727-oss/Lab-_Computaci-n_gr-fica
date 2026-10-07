//Previo 8// 
// Rea Alberto Cristian // 
// numero de cuenta : 318273130 //
//Fecha de entrega 6 de octubre del 2026//
#include <iostream>
#include <cmath>

// GLEW
#include <GL/glew.h>

// GLFW
#include <GLFW/glfw3.h>

// GLM Mathematics (radianes forzados: funciona igual en versiones viejas y nuevas de GLM)
#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// Other includes
#include "Shader.h"
#include "Camera.h"
#include "Model.h"

// Prototipos
void KeyCallback(GLFWwindow *window, int key, int scancode, int action, int mode);
void MouseCallback(GLFWwindow *window, double xPos, double yPos);
void DoMovement();

// ---- Configuración del modelo (cambia aquí la ruta si mueves los archivos) ----
const char *MODEL_PATH = "Models/carro.obj";   // junto a carro.mtl y car_texture.png
const float MODEL_FIT_SIZE = 2.0f;                    // el carro se escala para caber en un cubo de este tamaño

// Window dimensions
const GLuint WIDTH = 800, HEIGHT = 600;
int SCREEN_WIDTH, SCREEN_HEIGHT;

// Camera
Camera camera(glm::vec3(0.0f, 0.6f, 4.0f));
GLfloat lastX = WIDTH / 2.0;
GLfloat lastY = HEIGHT / 2.0;
bool keys[1024];
bool firstMouse = true;

// Luz (la lámpara) y estado
glm::vec3 lightPos(1.6f, 1.0f, 1.8f);
bool animateLightColor = false;
bool autoRotate = false;
float carAngle = glm::radians(35.0f);   // vista inicial 3/4 para ver frente y costado

// Materiales de la práctica (teclas 1, 2, 3)
struct MaterialDef { const char *name; glm::vec3 ambient, diffuse, specular; float shininess; };
MaterialDef materials[] =
{
	{ "Pintura brillante", glm::vec3(0.60f), glm::vec3(0.90f), glm::vec3(0.90f),  64.0f },
	{ "Mate (sin brillo)", glm::vec3(0.60f), glm::vec3(0.90f), glm::vec3(0.05f),   4.0f },
	{ "Metalico",          glm::vec3(0.40f), glm::vec3(0.55f), glm::vec3(1.00f), 128.0f },
};
int currentMaterial = 0;

// Deltatime
GLfloat deltaTime = 0.0f;
GLfloat lastFrame = 0.0f;

int main()
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

	GLFWwindow *window = glfwCreateWindow(WIDTH, HEIGHT, "previo 8 -Cristian Rea Alberto", nullptr, nullptr);
	if (nullptr == window)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return EXIT_FAILURE;
	}

	glfwMakeContextCurrent(window);
	glfwGetFramebufferSize(window, &SCREEN_WIDTH, &SCREEN_HEIGHT);

	glfwSetKeyCallback(window, KeyCallback);
	glfwSetCursorPosCallback(window, MouseCallback);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	glewExperimental = GL_TRUE;
	if (GLEW_OK != glewInit())
	{
		std::cout << "Failed to initialize GLEW" << std::endl;
		return EXIT_FAILURE;
	}

	glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
	glEnable(GL_DEPTH_TEST);

	Shader shader("Shader/modelLoading.vs", "Shader/modelLoading.frag");
	Shader lampShader("Shader/lamp.vs", "Shader/lamp.frag");
	Model carro((GLchar *)MODEL_PATH);

	if (carro.GetSize().x < 0.0f)   // sin vértices: el tamaño sale -inf
	{
		std::cout << "No se cargo el modelo. Revisa MODEL_PATH y el directorio de trabajo." << std::endl;
		glfwTerminate();
		return EXIT_FAILURE;
	}

	// ---- Cubo de la lámpara (solo posiciones) ----
	GLfloat cubeVertices[] =
	{
		-0.5f, -0.5f, -0.5f,   0.5f, -0.5f, -0.5f,   0.5f,  0.5f, -0.5f,  -0.5f,  0.5f, -0.5f,
		-0.5f, -0.5f,  0.5f,   0.5f, -0.5f,  0.5f,   0.5f,  0.5f,  0.5f,  -0.5f,  0.5f,  0.5f
	};
	GLuint cubeIndices[] =
	{
		0,1,2, 2,3,0,   // atrás
		4,5,6, 6,7,4,   // frente
		0,4,7, 7,3,0,   // izquierda
		1,5,6, 6,2,1,   // derecha
		0,1,5, 5,4,0,   // abajo
		3,2,6, 6,7,3    // arriba
	};
	GLuint lampVAO, lampVBO, lampEBO;
	glGenVertexArrays(1, &lampVAO);
	glGenBuffers(1, &lampVBO);
	glGenBuffers(1, &lampEBO);
	glBindVertexArray(lampVAO);
	glBindBuffer(GL_ARRAY_BUFFER, lampVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), cubeVertices, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, lampEBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cubeIndices), cubeIndices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid *)0);
	glEnableVertexAttribArray(0);
	glBindVertexArray(0);

	// ---- Ajuste automático al modelo: centrar en el origen y escalar a MODEL_FIT_SIZE ----
	glm::vec3 center = carro.GetCenter();
	glm::vec3 size = carro.GetSize();
	float maxDim = glm::max(size.x, glm::max(size.y, size.z));
	float fit = (maxDim > 0.0f) ? (MODEL_FIT_SIZE / maxDim) : 1.0f;

	std::cout << "Controles: WASD/flechas mover camara, mouse mirar, ESC salir" << std::endl;
	std::cout << "  I/K/J/L/U/O mover la lampara (z-/z+/x-/x+/y+/y-)" << std::endl;
	std::cout << "  1/2/3 cambiar material, ESPACIO color animado de la luz, R girar el carro" << std::endl;
	std::cout << "Material: " << materials[currentMaterial].name << std::endl;

	glm::mat4 projection = glm::perspective(glm::radians(camera.GetZoom()), (GLfloat)SCREEN_WIDTH / (GLfloat)SCREEN_HEIGHT, 0.1f, 100.0f);

	while (!glfwWindowShouldClose(window))
	{
		GLfloat currentFrame = (GLfloat)glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		glfwPollEvents();
		DoMovement();
		if (autoRotate) carAngle += glm::radians(30.0f) * deltaTime;

		// Fondo negro, como en el ejemplo
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Color de la luz (blanco, o animado con ESPACIO)
		glm::vec3 lightColor(1.0f);
		if (animateLightColor)
		{
			float t = (float)glfwGetTime();
			lightColor = glm::vec3(0.6f + 0.4f * sin(t * 2.0f), 0.6f + 0.4f * sin(t * 0.7f), 0.6f + 0.4f * sin(t * 1.3f));
		}

		glm::mat4 view = camera.GetViewMatrix();
		glm::vec3 viewPos = glm::vec3(glm::inverse(view)[3]);   // posición de la cámara

		// ================= CARRO (iluminado) =================
		shader.Use();
		glUniformMatrix4fv(glGetUniformLocation(shader.Program, "view"), 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(glGetUniformLocation(shader.Program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
		glUniform3f(glGetUniformLocation(shader.Program, "viewPos"), viewPos.x, viewPos.y, viewPos.z);

		// Luz
		glUniform3f(glGetUniformLocation(shader.Program, "light.position"), lightPos.x, lightPos.y, lightPos.z);
		glUniform3f(glGetUniformLocation(shader.Program, "light.ambient"), lightColor.x * 0.25f, lightColor.y * 0.25f, lightColor.z * 0.25f);
		glUniform3f(glGetUniformLocation(shader.Program, "light.diffuse"), lightColor.x * 0.90f, lightColor.y * 0.90f, lightColor.z * 0.90f);
		glUniform3f(glGetUniformLocation(shader.Program, "light.specular"), lightColor.x, lightColor.y, lightColor.z);

		// Material
		const MaterialDef &m = materials[currentMaterial];
		glUniform3f(glGetUniformLocation(shader.Program, "material.ambient"), m.ambient.x, m.ambient.y, m.ambient.z);
		glUniform3f(glGetUniformLocation(shader.Program, "material.diffuse"), m.diffuse.x, m.diffuse.y, m.diffuse.z);
		glUniform3f(glGetUniformLocation(shader.Program, "material.specular"), m.specular.x, m.specular.y, m.specular.z);
		glUniform1f(glGetUniformLocation(shader.Program, "material.shininess"), m.shininess);

		// Matriz del modelo: escalar -> girar -> mover el centro al origen
		glm::mat4 model(1.0f);
		model = glm::rotate(model, carAngle, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(fit));
		model = glm::translate(model, -center);
		glUniformMatrix4fv(glGetUniformLocation(shader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));

		carro.Draw(shader);

		// ================= LÁMPARA (cubo) =================
		lampShader.Use();
		glUniformMatrix4fv(glGetUniformLocation(lampShader.Program, "view"), 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(glGetUniformLocation(lampShader.Program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
		glUniform3f(glGetUniformLocation(lampShader.Program, "lampColor"), lightColor.x, lightColor.y, lightColor.z);

		glm::mat4 lampModel(1.0f);
		lampModel = glm::translate(lampModel, lightPos);
		lampModel = glm::scale(lampModel, glm::vec3(0.2f));
		glUniformMatrix4fv(glGetUniformLocation(lampShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(lampModel));

		glBindVertexArray(lampVAO);
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);

		glfwSwapBuffers(window);
	}

	glDeleteVertexArrays(1, &lampVAO);
	glDeleteBuffers(1, &lampVBO);
	glDeleteBuffers(1, &lampEBO);
	glfwTerminate();
	return 0;
}

void DoMovement()
{
	if (keys[GLFW_KEY_W] || keys[GLFW_KEY_UP])    camera.ProcessKeyboard(FORWARD, deltaTime);
	if (keys[GLFW_KEY_S] || keys[GLFW_KEY_DOWN])  camera.ProcessKeyboard(BACKWARD, deltaTime);
	if (keys[GLFW_KEY_A] || keys[GLFW_KEY_LEFT])  camera.ProcessKeyboard(LEFT, deltaTime);
	if (keys[GLFW_KEY_D] || keys[GLFW_KEY_RIGHT]) camera.ProcessKeyboard(RIGHT, deltaTime);

	// Mover la lámpara
	float v = 2.0f * deltaTime;
	if (keys[GLFW_KEY_I]) lightPos.z -= v;
	if (keys[GLFW_KEY_K]) lightPos.z += v;
	if (keys[GLFW_KEY_J]) lightPos.x -= v;
	if (keys[GLFW_KEY_L]) lightPos.x += v;
	if (keys[GLFW_KEY_U]) lightPos.y += v;
	if (keys[GLFW_KEY_O]) lightPos.y -= v;
}

void KeyCallback(GLFWwindow *window, int key, int scancode, int action, int mode)
{
	if (GLFW_KEY_ESCAPE == key && GLFW_PRESS == action)
		glfwSetWindowShouldClose(window, GL_TRUE);

	if (action == GLFW_PRESS)
	{
		if (key == GLFW_KEY_R) autoRotate = !autoRotate;
		if (key == GLFW_KEY_SPACE) animateLightColor = !animateLightColor;
		if (key >= GLFW_KEY_1 && key <= GLFW_KEY_3)
		{
			currentMaterial = key - GLFW_KEY_1;
			std::cout << "Material: " << materials[currentMaterial].name << std::endl;
		}
	}

	if (key >= 0 && key < 1024)
	{
		if (action == GLFW_PRESS)        keys[key] = true;
		else if (action == GLFW_RELEASE) keys[key] = false;
	}
}

void MouseCallback(GLFWwindow *window, double xPos, double yPos)
{
	if (firstMouse)
	{
		lastX = (GLfloat)xPos;
		lastY = (GLfloat)yPos;
		firstMouse = false;
	}

	GLfloat xOffset = (GLfloat)xPos - lastX;
	GLfloat yOffset = lastY - (GLfloat)yPos;

	lastX = (GLfloat)xPos;
	lastY = (GLfloat)yPos;

	camera.ProcessMouseMovement(xOffset, yOffset);
}
