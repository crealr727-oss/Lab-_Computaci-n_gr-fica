// Previo 9: Fuentes de Luz en OpenGL
// Alumno: Cristian Rea Alberto
// Numero de cuenta: 318273130
// Institucion: FI UNAM Grupo 7
// Fecha de entrega: 13 de Octubre del 2026

#include <iostream>
#include <cmath>

// GLEW
#include <GL/glew.h>

// GLFW
#include <GLFW/glfw3.h>

// Other Libs
#include "stb_image.h"

// GLM Mathematics
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// Load Models
#include "SOIL2/SOIL2.h"

// Other includes
#include "Shader.h"
#include "Camera.h"
#include "Model.h"

// Function prototypes
void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mode);
void MouseCallback(GLFWwindow* window, double xPos, double yPos);
void DoMovement();

// Window dimensions
const GLuint WIDTH = 800, HEIGHT = 600;
int SCREEN_WIDTH, SCREEN_HEIGHT;

// Camera
Camera camera(glm::vec3(0.0f, 1.8f, 4.5f));
GLfloat lastX = WIDTH / 2.0f;
GLfloat lastY = HEIGHT / 2.0f;
bool keys[1024];
bool firstMouse = true;

// Estados de encendido/apagado de cada fuente de luz
bool dirLightActive     = true;   // Luz direccional (Sol) - Tecla 0
bool pointLight1Active  = true;   // Foco 1 (Luz interactiva) - Tecla 1 o ESPACIO
bool pointLight2Active  = true;   // Foco 2 (Luz calida ambar) - Tecla 2
bool pointLight3Active  = true;   // Foco 3 (Luz fria cian) - Tecla 3
bool pointLight4Active  = true;   // Foco 4 (Luz magenta) - Tecla 4
bool spotlightActive    = true;   // Linterna (Spotlight) - Tecla F

// Positions of the 4 point lights
glm::vec3 pointLightPositions[] = {
	glm::vec3(0.0f,  1.3f,  1.2f),   // Luz interactiva movil (foco frontal)
	glm::vec3(1.8f,  0.8f, -1.2f),   // Luz puntual 2 (calida ambar)
	glm::vec3(-1.8f, 0.8f, -1.2f),   // Luz puntual 3 (fria cian)
	glm::vec3(0.0f,  1.0f, -2.0f)    // Luz puntual 4 (magenta)
};

// Cube vertices for the lamp objects
float vertices[] = {
	-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
	 0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
	 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
	 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
	-0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
	-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,

	-0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
	 0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
	 0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
	 0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
	-0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
	-0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,

	-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
	-0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
	-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
	-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
	-0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
	-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,

	 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
	 0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
	 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
	 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
	 0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
	 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,

	-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
	 0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
	 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
	 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
	-0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
	-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,

	-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
	 0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
	 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
	 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
	-0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
	-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f
};

// Deltatime
GLfloat deltaTime = 0.0f;
GLfloat lastFrame = 0.0f;

int main()
{
	// Init GLFW
	glfwInit();

	// Create a GLFWwindow object
	GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Previo 9 - Cristian Rea Alberto (318273130) FI UNAM Grupo 7", nullptr, nullptr);

	if (nullptr == window)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return EXIT_FAILURE;
	}

	glfwMakeContextCurrent(window);
	glfwGetFramebufferSize(window, &SCREEN_WIDTH, &SCREEN_HEIGHT);

	// Set callback functions
	glfwSetKeyCallback(window, KeyCallback);
	glfwSetCursorPosCallback(window, MouseCallback);

	// Initialize GLEW
	glewExperimental = GL_TRUE;
	if (GLEW_OK != glewInit())
	{
		std::cout << "Failed to initialize GLEW" << std::endl;
		return EXIT_FAILURE;
	}

	// Define the viewport dimensions
	glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);

	// Load Shaders
	Shader lightingShader("Shader/lighting.vs", "Shader/lighting.frag");
	Shader lampShader("Shader/lamp.vs", "Shader/lamp.frag");

	// Load Models (modelos de Cristian Rea Alberto)
	Model Piso((char*)"Models/piso.obj");
	Model Carro((char*)"Models/carro.obj");
	Model CristalGema((char*)"Models/cristal_gema.obj");

	std::cout << "==========================================================" << std::endl;
	std::cout << "  Previo 9: Fuentes de Luz en OpenGL" << std::endl;
	std::cout << "  Alumno: Cristian Rea Alberto (Cta: 318273130) - Gpo 7" << std::endl;
	std::cout << "  Modelo con Transparencia: Gema de Cristal 3D (48 facetas)" << std::endl;
	std::cout << "==========================================================" << std::endl;
	std::cout << "  CONTROLES DE LA ESCENA:" << std::endl;
	std::cout << "  [W, A, S, D] -> Movimiento de la camara" << std::endl;
	std::cout << "  [Mouse]      -> Orientacion de la vista" << std::endl;
	std::cout << "  [T / G]      -> Mover Foco 1 en Eje X (+/-)" << std::endl;
	std::cout << "  [Y / H]      -> Mover Foco 1 en Eje Y (+/-)" << std::endl;
	std::cout << "  [U / J]      -> Mover Foco 1 en Eje Z (+/-)" << std::endl;
	std::cout << "----------------------------------------------------------" << std::endl;
	std::cout << "  INTERRUPTORES DE ENCENDIDO / APAGADO DE LUCES:" << std::endl;
	std::cout << "  [1] o [ESPACIO] -> Prender / Apagar FOCO 1 (Luz Interactiva)" << std::endl;
	std::cout << "  [2]             -> Prender / Apagar FOCO 2 (Luz Ambar)" << std::endl;
	std::cout << "  [3]             -> Prender / Apagar FOCO 3 (Luz Cian)" << std::endl;
	std::cout << "  [4]             -> Prender / Apagar FOCO 4 (Luz Magenta)" << std::endl;
	std::cout << "  [0]             -> Prender / Apagar LUZ DIRECCIONAL (Sol)" << std::endl;
	std::cout << "  [F]             -> Prender / Apagar LINTERNA (Spotlight)" << std::endl;
	std::cout << "  [ESC]           -> Salir de la aplicacion" << std::endl;
	std::cout << "==========================================================" << std::endl;

	// Set VAO/VBO for lamp cubes
	GLuint VBO, VAO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);
	glBindVertexArray(0);

	// Set texture units in lightingShader
	lightingShader.Use();
	glUniform1i(glGetUniformLocation(lightingShader.Program, "material.diffuse"), 0);
	glUniform1i(glGetUniformLocation(lightingShader.Program, "material.specular"), 0);

	glm::mat4 projection = glm::perspective(camera.GetZoom(), (GLfloat)SCREEN_WIDTH / (GLfloat)SCREEN_HEIGHT, 0.1f, 100.0f);

	// Game loop
	while (!glfwWindowShouldClose(window))
	{
		// Calculate deltatime of current frame
		GLfloat currentFrame = (GLfloat)glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		// Check events
		glfwPollEvents();
		DoMovement();

		// Clear color and depth buffers
		glClearColor(0.06f, 0.06f, 0.09f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Enable Depth Test
		glEnable(GL_DEPTH_TEST);

		// Activate lighting shader
		lightingShader.Use();

		GLint viewPosLoc = glGetUniformLocation(lightingShader.Program, "viewPos");
		glUniform3f(viewPosLoc, camera.GetPosition().x, camera.GetPosition().y, camera.GetPosition().z);

		// =====================================================================
		// 1. LUZ DIRECCIONAL (Sol) - Controlada por dirLightActive
		// =====================================================================
		glUniform3f(glGetUniformLocation(lightingShader.Program, "dirLight.direction"), -0.2f, -1.0f, -0.3f);
		if (dirLightActive)
		{
			glUniform3f(glGetUniformLocation(lightingShader.Program, "dirLight.ambient"), 0.2f, 0.2f, 0.2f);
			glUniform3f(glGetUniformLocation(lightingShader.Program, "dirLight.diffuse"), 0.4f, 0.4f, 0.4f);
			glUniform3f(glGetUniformLocation(lightingShader.Program, "dirLight.specular"), 0.3f, 0.3f, 0.3f);
		}
		else
		{
			glUniform3f(glGetUniformLocation(lightingShader.Program, "dirLight.ambient"), 0.0f, 0.0f, 0.0f);
			glUniform3f(glGetUniformLocation(lightingShader.Program, "dirLight.diffuse"), 0.0f, 0.0f, 0.0f);
			glUniform3f(glGetUniformLocation(lightingShader.Program, "dirLight.specular"), 0.0f, 0.0f, 0.0f);
		}

		// =====================================================================
		// 2. LUCES PUNTUALES (4 fuentes con interruptores individuales)
		// =====================================================================
		// Foco 1 (Luz Interactiva animada)
		glm::vec3 col1 = glm::vec3(0.0f);
		if (pointLight1Active)
		{
			col1.x = (float)abs(sin(glfwGetTime() * 1.5f));
			col1.y = (float)abs(sin(glfwGetTime() * 1.5f + 2.0f));
			col1.z = (float)abs(sin(glfwGetTime() * 1.5f + 4.0f));
			// Asegurar brillo minimo visible si el seno baja
			col1 = glm::clamp(col1, 0.2f, 1.0f);
			glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[0].ambient"), col1.x * 0.15f, col1.y * 0.15f, col1.z * 0.15f);
			glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[0].diffuse"), col1.x, col1.y, col1.z);
			glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[0].specular"), 1.0f, 1.0f, 1.0f);
		}
		else
		{
			glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[0].ambient"), 0.0f, 0.0f, 0.0f);
			glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[0].diffuse"), 0.0f, 0.0f, 0.0f);
			glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[0].specular"), 0.0f, 0.0f, 0.0f);
		}
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[0].position"), pointLightPositions[0].x, pointLightPositions[0].y, pointLightPositions[0].z);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[0].constant"), 1.0f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[0].linear"), 0.09f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[0].quadratic"), 0.032f);

		// Foco 2 (Luz Ambar / Calida)
		glm::vec3 col2 = glm::vec3(0.0f);
		if (pointLight2Active)
		{
			col2 = glm::vec3(0.9f, 0.55f, 0.15f);
			glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[1].ambient"), col2.x * 0.1f, col2.y * 0.1f, col2.z * 0.1f);
			glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[1].diffuse"), col2.x, col2.y, col2.z);
			glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[1].specular"), 1.0f, 0.6f, 0.2f);
		}
		else
		{
			glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[1].ambient"), 0.0f, 0.0f, 0.0f);
			glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[1].diffuse"), 0.0f, 0.0f, 0.0f);
			glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[1].specular"), 0.0f, 0.0f, 0.0f);
		}
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[1].position"), pointLightPositions[1].x, pointLightPositions[1].y, pointLightPositions[1].z);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[1].constant"), 1.0f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[1].linear"), 0.09f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[1].quadratic"), 0.032f);

		// Foco 3 (Luz Cian / Fria)
		glm::vec3 col3 = glm::vec3(0.0f);
		if (pointLight3Active)
		{
			col3 = glm::vec3(0.15f, 0.7f, 0.95f);
			glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[2].ambient"), col3.x * 0.1f, col3.y * 0.1f, col3.z * 0.1f);
			glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[2].diffuse"), col3.x, col3.y, col3.z);
			glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[2].specular"), 0.3f, 0.8f, 1.0f);
		}
		else
		{
			glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[2].ambient"), 0.0f, 0.0f, 0.0f);
			glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[2].diffuse"), 0.0f, 0.0f, 0.0f);
			glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[2].specular"), 0.0f, 0.0f, 0.0f);
		}
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[2].position"), pointLightPositions[2].x, pointLightPositions[2].y, pointLightPositions[2].z);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[2].constant"), 1.0f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[2].linear"), 0.09f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[2].quadratic"), 0.032f);

		// Foco 4 (Luz Magenta / Violeta)
		glm::vec3 col4 = glm::vec3(0.0f);
		if (pointLight4Active)
		{
			col4 = glm::vec3(0.85f, 0.2f, 0.85f);
			glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[3].ambient"), col4.x * 0.1f, col4.y * 0.1f, col4.z * 0.1f);
			glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[3].diffuse"), col4.x, col4.y, col4.z);
			glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[3].specular"), 0.9f, 0.3f, 0.9f);
		}
		else
		{
			glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[3].ambient"), 0.0f, 0.0f, 0.0f);
			glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[3].diffuse"), 0.0f, 0.0f, 0.0f);
			glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[3].specular"), 0.0f, 0.0f, 0.0f);
		}
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[3].position"), pointLightPositions[3].x, pointLightPositions[3].y, pointLightPositions[3].z);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[3].constant"), 1.0f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[3].linear"), 0.09f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[3].quadratic"), 0.032f);

		// =====================================================================
		// 3. LUZ REFLECTORA / SPOTLIGHT (Linterna que sigue a la camara)
		// =====================================================================
		glUniform3f(glGetUniformLocation(lightingShader.Program, "spotLight.position"), camera.GetPosition().x, camera.GetPosition().y, camera.GetPosition().z);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "spotLight.direction"), camera.GetFront().x, camera.GetFront().y, camera.GetFront().z);
		if (spotlightActive)
		{
			glUniform3f(glGetUniformLocation(lightingShader.Program, "spotLight.ambient"), 0.05f, 0.05f, 0.05f);
			glUniform3f(glGetUniformLocation(lightingShader.Program, "spotLight.diffuse"), 0.95f, 0.95f, 0.95f);
			glUniform3f(glGetUniformLocation(lightingShader.Program, "spotLight.specular"), 1.0f, 1.0f, 1.0f);
		}
		else
		{
			glUniform3f(glGetUniformLocation(lightingShader.Program, "spotLight.ambient"), 0.0f, 0.0f, 0.0f);
			glUniform3f(glGetUniformLocation(lightingShader.Program, "spotLight.diffuse"), 0.0f, 0.0f, 0.0f);
			glUniform3f(glGetUniformLocation(lightingShader.Program, "spotLight.specular"), 0.0f, 0.0f, 0.0f);
		}
		glUniform1f(glGetUniformLocation(lightingShader.Program, "spotLight.constant"), 1.0f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "spotLight.linear"), 0.09f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "spotLight.quadratic"), 0.032f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "spotLight.cutOff"), glm::cos(glm::radians(12.5f)));
		glUniform1f(glGetUniformLocation(lightingShader.Program, "spotLight.outerCutOff"), glm::cos(glm::radians(17.5f)));

		// Material shininess
		glUniform1f(glGetUniformLocation(lightingShader.Program, "material.shininess"), 32.0f);

		// Camera transformations
		glm::mat4 view = camera.GetViewMatrix();
		GLint modelLoc = glGetUniformLocation(lightingShader.Program, "model");
		GLint viewLoc = glGetUniformLocation(lightingShader.Program, "view");
		GLint projLoc = glGetUniformLocation(lightingShader.Program, "projection");

		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

		// =====================================================================
		// RENDERIZADO DE MODELOS OPACOS
		// =====================================================================
		// 1) Piso
		glm::mat4 model = glm::mat4(1.0f);
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		glUniform1i(glGetUniformLocation(lightingShader.Program, "transparency"), 0);
		Piso.Draw(lightingShader);

		// 2) Carro (modelo principal opaco)
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0.0f, -0.38f, 0.0f));
		model = glm::scale(model, glm::vec3(0.7f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		glUniform1i(glGetUniformLocation(lightingShader.Program, "transparency"), 0);
		Carro.Draw(lightingShader);

		// =====================================================================
		// RENDERIZADO DE MODELO CON TRANSPARENCIA (Gema de Cristal 3D)
		// Se dibuja despues de los opacos con Blending activado
		// =====================================================================
		model = glm::mat4(1.0f);
		float floatingOffset = (float)sin(glfwGetTime() * 1.5f) * 0.08f;
		float rotationAngle = (float)glfwGetTime() * 35.0f; // Rotacion continua sobre su eje Y
		model = glm::translate(model, glm::vec3(0.0f, 0.75f + floatingOffset, 0.0f));
		model = glm::rotate(model, glm::radians(rotationAngle), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, glm::radians(15.0f), glm::vec3(1.0f, 0.0f, 0.2f)); // Inclinacion para reflejos
		model = glm::scale(model, glm::vec3(0.6f));

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		glUniform1i(glGetUniformLocation(lightingShader.Program, "transparency"), 1);
		CristalGema.Draw(lightingShader);
		glDisable(GL_BLEND);

		glBindVertexArray(0);

		// =====================================================================
		// DIBUJAR CUBOS DE LAS LAMPARAS (con lampShader y colores dinamicos)
		// =====================================================================
		lampShader.Use();
		modelLoc = glGetUniformLocation(lampShader.Program, "model");
		viewLoc = glGetUniformLocation(lampShader.Program, "view");
		projLoc = glGetUniformLocation(lampShader.Program, "projection");
		GLint lampColorLoc = glGetUniformLocation(lampShader.Program, "lampColor");

		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

		glm::vec3 lampVisualColors[4] = {
			pointLight1Active ? col1 : glm::vec3(0.12f, 0.12f, 0.15f),
			pointLight2Active ? col2 : glm::vec3(0.12f, 0.12f, 0.15f),
			pointLight3Active ? col3 : glm::vec3(0.12f, 0.12f, 0.15f),
			pointLight4Active ? col4 : glm::vec3(0.12f, 0.12f, 0.15f)
		};

		glBindVertexArray(VAO);
		for (GLuint i = 0; i < 4; i++)
		{
			// Si la luz esta apagada, el cubo se ve gris oscuro; si esta encendida, brilla en su color
			glUniform3f(lampColorLoc, lampVisualColors[i].x, lampVisualColors[i].y, lampVisualColors[i].z);

			model = glm::mat4(1.0f);
			model = glm::translate(model, pointLightPositions[i]);
			model = glm::scale(model, glm::vec3(0.18f));
			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}
		glBindVertexArray(0);

		// Swap buffers
		glfwSwapBuffers(window);
	}

	// De-allocate resources
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);

	// Terminate GLFW
	glfwTerminate();

	return 0;
}

// Moves/alters the camera positions based on user input
void DoMovement()
{
	// Camera controls
	if (keys[GLFW_KEY_W] || keys[GLFW_KEY_UP])
	{
		camera.ProcessKeyboard(FORWARD, deltaTime);
	}
	if (keys[GLFW_KEY_S] || keys[GLFW_KEY_DOWN])
	{
		camera.ProcessKeyboard(BACKWARD, deltaTime);
	}
	if (keys[GLFW_KEY_A] || keys[GLFW_KEY_LEFT])
	{
		camera.ProcessKeyboard(LEFT, deltaTime);
	}
	if (keys[GLFW_KEY_D] || keys[GLFW_KEY_RIGHT])
	{
		camera.ProcessKeyboard(RIGHT, deltaTime);
	}

	// Movement of Point Light 0 (interactive lamp)
	if (keys[GLFW_KEY_T])
	{
		pointLightPositions[0].x += 1.5f * deltaTime;
	}
	if (keys[GLFW_KEY_G])
	{
		pointLightPositions[0].x -= 1.5f * deltaTime;
	}
	if (keys[GLFW_KEY_Y])
	{
		pointLightPositions[0].y += 1.5f * deltaTime;
	}
	if (keys[GLFW_KEY_H])
	{
		pointLightPositions[0].y -= 1.5f * deltaTime;
	}
	if (keys[GLFW_KEY_U])
	{
		pointLightPositions[0].z -= 1.5f * deltaTime;
	}
	if (keys[GLFW_KEY_J])
	{
		pointLightPositions[0].z += 1.5f * deltaTime;
	}
}

// Is called whenever a key is pressed/released via GLFW
void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	if (GLFW_KEY_ESCAPE == key && GLFW_PRESS == action)
	{
		glfwSetWindowShouldClose(window, GL_TRUE);
	}

	if (key >= 0 && key < 1024)
	{
		if (action == GLFW_PRESS)
		{
			keys[key] = true;
		}
		else if (action == GLFW_RELEASE)
		{
			keys[key] = false;
		}
	}

	// INTERRUPTORES DE ENCENDIDO / APAGADO (1 solo toque por tecla presionada)
	if (action == GLFW_PRESS)
	{
		// Foco 1 (Interactiva): Tecla 1 o ESPACIO
		if (key == GLFW_KEY_1 || key == GLFW_KEY_SPACE)
		{
			pointLight1Active = !pointLight1Active;
			std::cout << "[LUZ] Foco 1 (Luz Interactiva): " << (pointLight1Active ? "ENCENDIDO" : "APAGADO") << std::endl;
		}

		// Foco 2 (Ambar): Tecla 2
		if (key == GLFW_KEY_2)
		{
			pointLight2Active = !pointLight2Active;
			std::cout << "[LUZ] Foco 2 (Luz Ambar): " << (pointLight2Active ? "ENCENDIDO" : "APAGADO") << std::endl;
		}

		// Foco 3 (Cian): Tecla 3
		if (key == GLFW_KEY_3)
		{
			pointLight3Active = !pointLight3Active;
			std::cout << "[LUZ] Foco 3 (Luz Cian): " << (pointLight3Active ? "ENCENDIDO" : "APAGADO") << std::endl;
		}

		// Foco 4 (Magenta): Tecla 4
		if (key == GLFW_KEY_4)
		{
			pointLight4Active = !pointLight4Active;
			std::cout << "[LUZ] Foco 4 (Luz Magenta): " << (pointLight4Active ? "ENCENDIDO" : "APAGADO") << std::endl;
		}

		// Luz Direccional (Sol): Tecla 0 o Tecla B
		if (key == GLFW_KEY_0 || key == GLFW_KEY_B)
		{
			dirLightActive = !dirLightActive;
			std::cout << "[LUZ] Luz Direccional (Sol): " << (dirLightActive ? "ENCENDIDA" : "APAGADA") << std::endl;
		}

		// Linterna (Spotlight): Tecla F o Tecla L
		if (key == GLFW_KEY_F || key == GLFW_KEY_L)
		{
			spotlightActive = !spotlightActive;
			std::cout << "[LUZ] Linterna (Spotlight): " << (spotlightActive ? "ENCENDIDA" : "APAGADA") << std::endl;
		}
	}
}

void MouseCallback(GLFWwindow* window, double xPos, double yPos)
{
	if (firstMouse)
	{
		lastX = (GLfloat)xPos;
		lastY = (GLfloat)yPos;
		firstMouse = false;
	}

	GLfloat xOffset = (GLfloat)(xPos - lastX);
	GLfloat yOffset = (GLfloat)(lastY - yPos);

	lastX = (GLfloat)xPos;
	lastY = (GLfloat)yPos;

	camera.ProcessMouseMovement(xOffset, yOffset);
}