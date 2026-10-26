//Previo 10 // 
// Rea Alberto Cristian // 
// numero de cuenta : 318273130 //
//Fecha de entrega 20 de octubre del 2026//

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

//Load Models
#include "SOIL2/SOIL2.h"


// Other includes
#include "Shader.h"
#include "Camera.h"
#include "Model.h"

// Function prototypes
void KeyCallback(GLFWwindow *window, int key, int scancode, int action, int mode);
void MouseCallback(GLFWwindow *window, double xPos, double yPos);
void DoMovement();
void Animation();
glm::mat4 DogTransform();

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
glm::vec3 lightPos(0.0f, 0.0f, 0.0f);
bool active = true;   // la luz cian inicia encendida (SPACE la apaga/enciende)

// Positions of the point lights
glm::vec3 pointLightPositions[] = {
	glm::vec3(0.0f,2.0f, 0.0f),
	glm::vec3(0.0f,0.0f, 0.0f),
	glm::vec3(0.0f,0.0f,  0.0f),
	glm::vec3(0.0f,0.0f, 0.0f)
};

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



glm::vec3 Light1 = glm::vec3(0);
//Anim
float rotBall = 0;
bool AnimBall = false;

// ---- Animacion por fases (como en el video) ----
// Fase 0: el perro se va hacia +X y la pelota hacia -X (se separan).
// Fase 1: regresan uno hacia el otro (el perro mira a -X) y se cruzan en el centro.
// Fase 2: el perro salta y GOLPEA la pelota, que sale volando; despues el ciclo se repite.
// En fases 0 y 1 la pelota oscila en Y como una senoidal.
int   fase = 0;
float faseT = 0.0f;                              // tiempo transcurrido dentro de la fase actual
float durFase[3] = { 5.0f, 5.0f, 2.0f };        // duracion de cada fase en segundos
float dogLimite = 1.2f;                          // el perro y la pelota se alejan hasta +-dogLimite
float dogNariz = 0.47f;                          // distancia del centro del perro a su nariz (donde queda la pelota)
float ballOscilaciones = 2.0f;                   // oscilaciones completas del seno por fase (termina siempre a la altura de la nariz)
float golpeAlcance = 0.9f;                       // cuanto viaja la pelota en X al ser golpeada
float golpeAlturaFrac = 0.4f;                    // altura maxima del golpe (fraccion del recorrido hasta la luz)
float dogSaltoAlt = 0.15f;                       // altura del salto con el que golpea

float dogX = 0.0f, dogXIni = 0.0f;               // posicion actual / al inicio de la fase del perro (X)
float ballX = 0.47f, ballXIni = 0.47f;           // posicion actual / al inicio de la fase de la pelota (X); inicia en la nariz
float dogYaw = 90.0f;                            // orientacion actual (+90 = mira a +X, -90 = mira a -X)
float dogSalto = 0.0f;                           // altura actual del salto
glm::mat4 dogModel(1.0f);                        // transformacion actual del perro (la sombra la reutiliza)

// Animacion de subida y bajada de la pelota (como si hiciera dominadas)
float movBall = 0.0f;       // desplazamiento actual en Y respecto a la nariz del perro
float ballInicioY = 0.312f; // altura Y real del centro de la pelota en ball.obj (punto de partida, junto a la nariz)
glm::vec3 ballCentro(0.0f, 0.312f, 0.47f); // centro de la pelota en ball.obj (para girar sobre si misma)
float ballMaxMov = 0.0f;    // recorrido maximo (se calcula en Animation)


// Deltatime
GLfloat deltaTime = 0.0f;	// Time between current frame and last frame
GLfloat lastFrame = 0.0f;  	// Time of last frame

int main()
{
	// Init GLFW
	glfwInit();
	// Set all the required options for GLFW
	/*glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);*/

	// Create a GLFWwindow object that we can use for GLFW's functions
	GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Rea Alberto Cristian - 318273130", nullptr, nullptr);

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
	//glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

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



	Shader lightingShader("Shader/lighting.vs", "Shader/lighting.frag");
	Shader lampShader("Shader/lamp.vs", "Shader/lamp.frag");
	
	//models
	Model Dog((char*)"Models/RedDog.obj");
	Model Piso((char*)"Models/piso.obj");
	Model Ball((char*)"Models/ball.obj");



	// First, set the container's VAO (and VBO)
	GLuint VBO, VAO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	// Position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);
	// normal attribute
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	// Set texture units
	lightingShader.Use();
	glUniform1i(glGetUniformLocation(lightingShader.Program, "material.diffuse"), 0);
	glUniform1i(glGetUniformLocation(lightingShader.Program, "material.specular"), 1);

	glm::mat4 projection = glm::perspective(camera.GetZoom(), (GLfloat)SCREEN_WIDTH / (GLfloat)SCREEN_HEIGHT, 0.1f, 100.0f);

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
		Animation();

		// Clear the colorbuffer
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	   
		// OpenGL options
		glEnable(GL_DEPTH_TEST);

		
		
		
	

		// Use cooresponding shader when setting uniforms/drawing objects
		lightingShader.Use();

        glUniform1i(glGetUniformLocation(lightingShader.Program, "diffuse"), 0);
		//glUniform1i(glGetUniformLocation(lightingShader.Program, "specular"),1);

		GLint viewPosLoc = glGetUniformLocation(lightingShader.Program, "viewPos");
		glUniform3f(viewPosLoc, camera.GetPosition().x, camera.GetPosition().y, camera.GetPosition().z);


		// Directional light
		glUniform3f(glGetUniformLocation(lightingShader.Program, "dirLight.direction"), -0.2f, -1.0f, -0.3f);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "dirLight.ambient"),0.6f,0.6f,0.6f);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "dirLight.diffuse"), 0.6f, 0.6f, 0.6f);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "dirLight.specular"),0.3f, 0.3f, 0.3f);


		// Point light 1
	    glm::vec3 lightColor = active ? glm::vec3(0.0f, 1.0f, 1.0f) : glm::vec3(0.0f);   // cian como en el video

		
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[0].position"), pointLightPositions[0].x, pointLightPositions[0].y, pointLightPositions[0].z);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[0].ambient"), lightColor.x,lightColor.y, lightColor.z);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[0].diffuse"), lightColor.x,lightColor.y,lightColor.z);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[0].specular"), lightColor.x, lightColor.y, lightColor.z);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[0].constant"), 1.0f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[0].linear"), 0.045f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[0].quadratic"),0.075f);


		// SpotLight
		glUniform3f(glGetUniformLocation(lightingShader.Program, "spotLight.position"), camera.GetPosition().x, camera.GetPosition().y, camera.GetPosition().z);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "spotLight.direction"), camera.GetFront().x, camera.GetFront().y, camera.GetFront().z);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "spotLight.ambient"), 0.2f, 0.2f, 0.8f);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "spotLight.diffuse"), 0.2f, 0.2f, 0.8f);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "spotLight.specular"), 0.0f, 0.0f, 0.0f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "spotLight.constant"), 1.0f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "spotLight.linear"), 0.3f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "spotLight.quadratic"), 0.7f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "spotLight.cutOff"), glm::cos(glm::radians(12.0f)));
		glUniform1f(glGetUniformLocation(lightingShader.Program, "spotLight.outerCutOff"), glm::cos(glm::radians(18.0f)));
		

		// Set material properties
		glUniform1f(glGetUniformLocation(lightingShader.Program, "material.shininess"), 5.0f);

		// Create camera transformations
		glm::mat4 view;
		view = camera.GetViewMatrix();

		// Get the uniform locations
		GLint modelLoc = glGetUniformLocation(lightingShader.Program, "model");
		GLint viewLoc = glGetUniformLocation(lightingShader.Program, "view");
		GLint projLoc = glGetUniformLocation(lightingShader.Program, "projection");

		// Pass the matrices to the shader
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));


		glm::mat4 model(1);

	
		
		//Carga de modelo 
        view = camera.GetViewMatrix();	
		model = glm::mat4(1);
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		Piso.Draw(lightingShader);

		// Perro: camina en circulo, gira hacia su direccion y salta
		model = dogModel;
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		glUniform1i(glGetUniformLocation(lightingShader.Program, "transparency"), 0);
		glUniform1i(glGetUniformLocation(lightingShader.Program, "shadow"), 0);
		Dog.Draw(lightingShader);

		// Sombra del perro: proyeccion plana desde la luz puntual sobre el piso (y = h)
		{
			float h = 0.02f;                          // un poco arriba del piso para evitar z-fighting
			glm::vec3 L = pointLightPositions[0];
			float d = L.y - h;
			glm::mat4 sombra(1.0f);
			sombra[0] = glm::vec4(d, 0.0f, 0.0f, 0.0f);
			sombra[1] = glm::vec4(-L.x, -h, -L.z, -1.0f);
			sombra[2] = glm::vec4(0.0f, 0.0f, d, 0.0f);
			sombra[3] = glm::vec4(L.x * h, L.y * h, L.z * h, L.y);
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			glUniform1i(glGetUniformLocation(lightingShader.Program, "shadow"), 1);
			model = sombra * dogModel;
			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
			Dog.Draw(lightingShader);
			glUniform1i(glGetUniformLocation(lightingShader.Program, "shadow"), 0);
			glDisable(GL_BLEND);
		}

		// Pelota: sube y baja (traslacion en Y) y gira sobre su eje
		model = glm::mat4(1);
		glEnable(GL_BLEND);//Avtiva la funcionalidad para trabajar el canal alfa
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glUniform1i(glGetUniformLocation(lightingShader.Program, "transparency"), 1);
		model = glm::translate(model, glm::vec3(ballX, movBall, -ballCentro.z));   // X propia, Y senoidal, y alineada con la linea del perro
		model = glm::translate(model, ballCentro);                       // 3) regresa al lugar original
		model = glm::rotate(model, glm::radians(rotBall), glm::vec3(0.0f, 1.0f, 0.0f)); // 2) gira sobre su propio centro
		model = glm::translate(model, -ballCentro);                      // 1) lleva el centro al origen
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
	    Ball.Draw(lightingShader); 
		glDisable(GL_BLEND);  //Desactiva el canal alfa 
		glBindVertexArray(0);
	

		// Also draw the lamp object, again binding the appropriate shader
		lampShader.Use();
		// Get location objects for the matrices on the lamp shader (these could be different on a different shader)
		modelLoc = glGetUniformLocation(lampShader.Program, "model");
		viewLoc = glGetUniformLocation(lampShader.Program, "view");
		projLoc = glGetUniformLocation(lampShader.Program, "projection");

		// Set matrices
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));
		glm::vec3 cuboColor = active ? lightColor : glm::vec3(0.2f);   // cubo cian cuando la luz esta encendida
		glUniform3f(glGetUniformLocation(lampShader.Program, "lightColor"), cuboColor.x, cuboColor.y, cuboColor.z);
		model = glm::mat4(1);
		model = glm::translate(model, lightPos);
		model = glm::scale(model, glm::vec3(0.2f)); // Make it a smaller cube
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		// Draw the light object (using light's vertex attributes)
		
			model = glm::mat4(1);
			model = glm::translate(model, pointLightPositions[0]);
			model = glm::scale(model, glm::vec3(0.2f)); // Make it a smaller cube
			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
			glBindVertexArray(VAO);
			glDrawArrays(GL_TRIANGLES, 0, 36);
		
		glBindVertexArray(0);



		// Swap the screen buffers
		glfwSwapBuffers(window);
	}


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

	if (keys[GLFW_KEY_T])
	{
		pointLightPositions[0].x += 0.01f;
	}
	if (keys[GLFW_KEY_G])
	{
		pointLightPositions[0].x -= 0.01f;
	}

	if (keys[GLFW_KEY_Y])
	{
		pointLightPositions[0].y += 0.01f;
	}

	if (keys[GLFW_KEY_H])
	{
		pointLightPositions[0].y -= 0.01f;
	}
	if (keys[GLFW_KEY_U])
	{
		pointLightPositions[0].z -= 0.1f;
	}
	if (keys[GLFW_KEY_J])
	{
		pointLightPositions[0].z += 0.01f;
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

	if (key == GLFW_KEY_SPACE && action == GLFW_PRESS)
	{
		active = !active;   // enciende/apaga la luz cian
	}
	if (key == GLFW_KEY_N && action == GLFW_PRESS)
	{
		AnimBall = !AnimBall;   // inicia/detiene la animacion del perro y la pelota
		if (!AnimBall)
		{
			// Al detener, todo regresa a su posicion inicial
			movBall = 0.0f;
			fase = 0;
			faseT = 0.0f;
			dogX = dogXIni = 0.0f;
			ballX = ballXIni = dogNariz;
			dogYaw = 90.0f;
			dogSalto = 0.0f;
			dogModel = DogTransform();
		}
	}
}
// Transformacion del perro: se desplaza en X, salta y mira hacia donde avanza (el modelo mira a +Z de origen)
glm::mat4 DogTransform()
{
	glm::mat4 m = glm::translate(glm::mat4(1.0f), glm::vec3(dogX, dogSalto, 0.0f));
	m = glm::rotate(m, glm::radians(dogYaw), glm::vec3(0.0f, 1.0f, 0.0f));
	return m;
}

void Animation() {
	if (AnimBall)
	{
		const float PI = 3.14159265f;
		ballMaxMov = pointLightPositions[0].y - ballInicioY;   // recorrido maximo hasta la luz

		float u = faseT / durFase[fase];
		if (u > 1.0f) u = 1.0f;
		float yawObjetivo = dogYaw;
		float velGiroPelota = 60.0f;   // grados por segundo

		if (fase == 0)
		{
			// Se separan: perro hacia +X, pelota hacia -X
			float s = u * u * (3.0f - 2.0f * u);   // arranque y llegada suaves
			dogX  = dogXIni  + ( dogLimite - dogXIni)  * s;
			ballX = ballXIni + (-dogLimite - ballXIni) * s;
			dogSalto = 0.0f;
			yawObjetivo = 90.0f;
			// Movimiento senoidal: 0 -> altura de la luz -> 0, varias veces
			movBall = (ballMaxMov / 2.0f) * (1.0f - cos(2.0f * PI * ballOscilaciones * u));
		}
		else if (fase == 1)
		{
			// Regresan uno hacia el otro y se cruzan en el centro; el perro termina con la nariz frente a la pelota
			float dogFin  = dogNariz / 2.0f;
			float ballFin = dogFin - dogNariz;
			dogX  = dogXIni  + (dogFin  - dogXIni)  * u;
			ballX = ballXIni + (ballFin - ballXIni) * u;
			dogSalto = 0.0f;
			yawObjetivo = -90.0f;
			movBall = (ballMaxMov / 2.0f) * (1.0f - cos(2.0f * PI * ballOscilaciones * u));
		}
		else
		{
			// Golpe: el perro salta hacia la pelota y la manda volando en su direccion (-X)
			float ease = 1.0f - (1.0f - u) * (1.0f - u);
			dogX  = dogXIni  - 0.3f * ease;
			dogSalto = dogSaltoAlt * sin(PI * (u * 2.0f > 1.0f ? 1.0f : u * 2.0f));
			yawObjetivo = -90.0f;
			ballX = ballXIni - golpeAlcance * ease;
			movBall = 4.0f * (ballMaxMov * golpeAlturaFrac) * u * (1.0f - u);   // arco del golpe
			velGiroPelota = 360.0f;
		}

		// La pelota gira sobre si misma (mas rapido tras el golpe)
		rotBall += velGiroPelota * deltaTime;

		// El perro gira suavemente hacia su orientacion objetivo (360 grados por segundo)
		float dif = yawObjetivo - dogYaw;
		float paso = 360.0f * deltaTime;
		if (dif >  paso) dif =  paso;
		if (dif < -paso) dif = -paso;
		dogYaw += dif;

		// Avanza el tiempo; al terminar la fase se pasa a la siguiente conservando las posiciones finales
		faseT += deltaTime;
		if (faseT >= durFase[fase])
		{
			dogXIni  = dogX;
			ballXIni = ballX;
			faseT = 0.0f;
			fase = (fase + 1) % 3;
		}
	}
	dogModel = DogTransform();
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
