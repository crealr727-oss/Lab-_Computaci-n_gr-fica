//Practica 7                                   Rea Alberto Cristian
//Fecha de entrega:4 de octubre del 2026              318273130
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

// Other includes
#include "Shader.h"
#include "Camera.h"


// Function prototypes
void KeyCallback(GLFWwindow *window, int key, int scancode, int action, int mode);
void MouseCallback(GLFWwindow *window, double xPos, double yPos);
void DoMovement();

// Window dimensions
const GLuint WIDTH = 800, HEIGHT = 600;
int SCREEN_WIDTH, SCREEN_HEIGHT;

// Camera
Camera  camera(glm::vec3(0.0f, 0.0f, 3.0f));
GLfloat lastX = WIDTH / 2.0;
GLfloat lastY = HEIGHT / 2.0;
bool keys[1024];
bool firstMouse = true;

// Light attributes
glm::vec3 lightPos(1.2f, 1.0f, 2.0f);

// Deltatime
GLfloat deltaTime = 0.0f;	// Time between current frame and last frame
GLfloat lastFrame = 0.0f;  	// Time of last frame

							// The MAIN function, from here we start the application and run the game loop
int main()
{
	// Init GLFW
	glfwInit();
	// Set all the required options for GLFW
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

	// Create a GLFWwindow object that we can use for GLFW's functions
	GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Practica 7 - Rea Alberto Cristian", nullptr, nullptr);

	if (nullptr == window)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();

		return EXIT_FAILURE;
	}

	glfwMakeContextCurrent(window);

	glfwGetFramebufferSize(window, &SCREEN_WIDTH, &SCREEN_HEIGHT);

	// Set the required callback functions
	glfwSetKeyCallback(window, KeyCallback);
	glfwSetCursorPosCallback(window, MouseCallback);

	// GLFW Options
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// Set this to true so GLEW knows to use a modern approach to retrieving function pointers and extensions
	glewExperimental = GL_TRUE;
	// Initialize GLEW to setup the OpenGL Function pointers
	if (GLEW_OK != glewInit())
	{
		std::cout << "Failed to initialize GLEW" << std::endl;
		return EXIT_FAILURE;
	}

	// Define the viewport dimensions
	glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);

	// OpenGL options
	glEnable(GL_DEPTH_TEST);


	// Build and compile our shader program
	Shader lampShader("Shader/lamp.vs", "Shader/lamp.frag");

	// Set up vertex data (and buffer(s)) and attribute pointers
	// Celdas del cruce de dado.png (642x496 px, UV ya considerando el flip vertical):
	//   Fila media: col0 = atras, col1 = izquierda, col2 = frente, col3 = derecha
	//   Arriba = fila alta de col2, Abajo = fila baja de col2
	// Cada cara usa un rectangulo un poco mas chico que su celda para no tomar las lineas negras del borde.
	GLfloat vertices[] =
	{
		// posicion              color              uv
		// Frente (col2, fila media)
		-0.5f, -0.5f,  0.5f,   1.0f, 1.0f, 1.0f,   0.477f, 0.373f, // Punto I
		 0.5f, -0.5f,  0.5f,   1.0f, 1.0f, 1.0f,   0.692f, 0.373f, // Punto J
		 0.5f,  0.5f,  0.5f,   1.0f, 1.0f, 1.0f,   0.692f, 0.653f, // Punto G
		-0.5f,  0.5f,  0.5f,   1.0f, 1.0f, 1.0f,   0.477f, 0.653f, // Punto H

		// Atras (col0, fila media)
		 0.5f, -0.5f, -0.5f,   1.0f, 1.0f, 1.0f,   0.033f, 0.373f, // Punto A1
		-0.5f, -0.5f, -0.5f,   1.0f, 1.0f, 1.0f,   0.248f, 0.373f, // Punto B1
		-0.5f,  0.5f, -0.5f,   1.0f, 1.0f, 1.0f,   0.248f, 0.653f, // Punto Z
		 0.5f,  0.5f, -0.5f,   1.0f, 1.0f, 1.0f,   0.033f, 0.653f, // Punto W

		// Derecha (col3, fila media)
		 0.5f, -0.5f,  0.5f,   1.0f, 1.0f, 1.0f,   0.698f, 0.373f, // Punto V
		 0.5f, -0.5f, -0.5f,   1.0f, 1.0f, 1.0f,   0.914f, 0.373f, // Punto U
		 0.5f,  0.5f, -0.5f,   1.0f, 1.0f, 1.0f,   0.914f, 0.653f, // Punto T
		 0.5f,  0.5f,  0.5f,   1.0f, 1.0f, 1.0f,   0.698f, 0.653f, // Punto S

		// Izquierda (col1, fila media)
		-0.5f, -0.5f, -0.5f,   1.0f, 1.0f, 1.0f,   0.254f, 0.373f, // Punto M
		-0.5f, -0.5f,  0.5f,   1.0f, 1.0f, 1.0f,   0.470f, 0.373f, // Punto N
		-0.5f,  0.5f,  0.5f,   1.0f, 1.0f, 1.0f,   0.470f, 0.653f, // Punto L
		-0.5f,  0.5f, -0.5f,   1.0f, 1.0f, 1.0f,   0.254f, 0.653f, // Punto K

		// Abajo (col2, fila baja)
		-0.5f, -0.5f, -0.5f,   1.0f, 1.0f, 1.0f,   0.477f, 0.085f, // Punto Q
		 0.5f, -0.5f, -0.5f,   1.0f, 1.0f, 1.0f,   0.692f, 0.085f, // Punto R
		 0.5f, -0.5f,  0.5f,   1.0f, 1.0f, 1.0f,   0.692f, 0.365f, // Punto P
		-0.5f, -0.5f,  0.5f,   1.0f, 1.0f, 1.0f,   0.477f, 0.365f, // Punto O

		// Arriba (col2, fila alta)
		-0.5f,  0.5f,  0.5f,   1.0f, 1.0f, 1.0f,   0.477f, 0.660f, // Punto C
		 0.5f,  0.5f,  0.5f,   1.0f, 1.0f, 1.0f,   0.692f, 0.660f, // Punto F
		 0.5f,  0.5f, -0.5f,   1.0f, 1.0f, 1.0f,   0.692f, 0.941f, // Punto E
		-0.5f,  0.5f, -0.5f,   1.0f, 1.0f, 1.0f,   0.477f, 0.941f  // Punto D
	};

	GLuint indices[] =
	{
		0, 1, 2, 2, 3, 0,        // Frente
		4, 5, 6, 6, 7, 4,        // Atras
		8, 9, 10, 10, 11, 8,     // Derecha
		12, 13, 14, 14, 15, 12,  // Izquierda
		16, 17, 18, 18, 19, 16,  // Abajo
		20, 21, 22, 22, 23, 20   // Arriba
	};

	// First, set the container's VAO (and VBO)
	GLuint VBO, VAO, EBO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	// Position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid *)0);
	glEnableVertexAttribArray(0);
	// Color attribute
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid *)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);
	// Texture Coordinate attribute
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid *)(6 * sizeof(GLfloat)));
	glEnableVertexAttribArray(2);
	glBindVertexArray(0);

	// Load textures
	GLuint texture1;
	glGenTextures(1, &texture1);
	glBindTexture(GL_TEXTURE_2D, texture1);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	int textureWidth, textureHeight, nrChannels;
	stbi_set_flip_vertically_on_load(true);
	// Forzamos 4 canales (RGBA) para que siempre coincida con GL_RGBA
	// (dado.png es un PNG de paleta con transparencia, stb lo entrega con 4 canales)
	unsigned char *image = stbi_load("images/dado.png", &textureWidth, &textureHeight, &nrChannels, 4);

	if (image)
	{
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, textureWidth, textureHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		std::cout << "Failed to load texture" << std::endl;
	}
	stbi_image_free(image);
	glBindTexture(GL_TEXTURE_2D, 0);


	// Game loop
	while (!glfwWindowShouldClose(window))
	{
		// Calculate deltatime of current frame
		GLfloat currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		// Check if any events have been activiated (key pressed, mouse moved etc.) and call corresponding response functions
		glfwPollEvents();
		DoMovement();

		// Clear the colorbuffer
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		lampShader.Use();
		//// Create camera transformations
		glm::mat4 view;
		view = camera.GetViewMatrix();
		glm::mat4 projection = glm::perspective(camera.GetZoom(), (GLfloat)SCREEN_WIDTH / (GLfloat)SCREEN_HEIGHT, 0.1f, 100.0f);
		glm::mat4 model(1);
		// Get location objects for the matrices on the lamp shader (these could be different on a different shader)
		// Get the uniform locations
		GLint modelLoc = glGetUniformLocation(lampShader.Program, "model");
		GLint viewLoc = glGetUniformLocation(lampShader.Program, "view");
		GLint projLoc = glGetUniformLocation(lampShader.Program, "projection");

		// Bind diffuse map
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture1);

		// Set matrices
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		// Draw the light object (using light's vertex attributes)
		glBindVertexArray(VAO);
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);

		// Swap the screen buffers
		glfwSwapBuffers(window);
	}

	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);
	glDeleteTextures(1, &texture1);
	// Terminate GLFW, clearing any resources allocated by GLFW.
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
}

// Is called whenever a key is pressed/released via GLFW
void KeyCallback(GLFWwindow *window, int key, int scancode, int action, int mode)
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
}

void MouseCallback(GLFWwindow *window, double xPos, double yPos)
{
	if (firstMouse)
	{
		lastX = xPos;
		lastY = yPos;
		firstMouse = false;
	}

	GLfloat xOffset = xPos - lastX;
	GLfloat yOffset = lastY - yPos;  // Reversed since y-coordinates go from bottom to left

	lastX = xPos;
	lastY = yPos;

	camera.ProcessMouseMovement(xOffset, yOffset);
}
