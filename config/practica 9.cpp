// Previo 9: Fuentes de Luz en OpenGL
// Alumno: Cristian Rea Alberto
// Numero de cuenta: 318273130
// Institucion: FI UNAM Grupo 7
// Fecha de entrega: 18 de Octubre del 2026
//
// Escena: un auto sobre una calle con seis farolas (tres por lado).
//   - Luz interior del auto, direccionales traseras y faros delanteros (cada grupo con su tecla)
//   - Seis farolas de la calle, cada una con su propio interruptor (teclas 1 a 6)
//   - Tecla 7: inicia / detiene la rotacion del Sol y la Luna (dia, atardecer, noche y amanecer)

#include <iostream>
#include <cmath>
#include <string>

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
Camera camera(glm::vec3(0.0f, 2.0f, 6.5f));
GLfloat lastX = WIDTH / 2.0f;
GLfloat lastY = HEIGHT / 2.0f;
bool keys[1024];
bool firstMouse = true;

// Estados de encendido/apagado de cada fuente de luz
bool cabinLightActive   = true;   // Luz interior del auto (donde va el conductor) - Tecla Z
bool rearSignalsActive  = true;   // Direccionales traseras (intermitentes)        - Tecla X
bool headlightsActive   = true;   // Faros delanteros (cofre)                      - Tecla C
bool streetLampActive[6] = { true, true, true, true, true, true };  // Farolas 1 a 6 - Teclas 1 a 6
bool flashlightActive   = false;  // Linterna de la camara (Spotlight)             - Tecla F

// Ciclo dia / noche (Tecla 7): el Sol y la Luna giran en un arco detras de la calle.
// Con la tecla 7 se inicia la rotacion; al volver a presionarla se detiene en ese instante.
bool  skyRotating = false;
float sunAngle    = 270.0f;          // 0 = amanecer (izquierda), 90 = mediodia, 180 = atardecer, 270 = media noche
const float SKY_SPEED  = 12.0f;      // grados por segundo (un ciclo completo dura 30 s)
const float SKY_RADIUS = 18.0f;
const glm::vec3 SKY_CENTER = glm::vec3(0.0f, -0.38f, -8.0f);

static float Smooth(float a, float b, float x)
{
	float t = glm::clamp((x - a) / (b - a), 0.0f, 1.0f);
	return t * t * (3.0f - 2.0f * t);
}

// Escena
const float ROAD_Y        = -0.38f;                    // altura de la calle (donde se apoyan las llantas)
const float SIDEWALK_Y    = -0.26f;                    // altura de la banqueta
const glm::vec3 carPos    = glm::vec3(-1.6f, ROAD_Y, 0.0f);   // carril derecho para un auto que avanza hacia +Z
const float carScale      = 0.7f;
// Farolas 1 a 3: lado izquierdo (brazo hacia +X). Farolas 4 a 6: lado derecho (brazo hacia -X).
// En cada lado se reparten de la mas cercana a la camara a la mas lejana.
const float lampZ[3] = { 3.0f, -2.0f, -7.0f };
const float lampX    = 3.8f;

// Deltatime
GLfloat deltaTime = 0.0f;
GLfloat lastFrame = 0.0f;

// ---------------------------------------------------------------------------
// Funciones auxiliares para enviar las luces al shader
// ---------------------------------------------------------------------------
static void SetPointLight(GLuint prog, int i, const glm::vec3& pos, const glm::vec3& col, float ambientK,
	float linear, float quadratic, bool on)
{
	std::string n = "pointLights[" + std::to_string(i) + "].";
	glm::vec3 c = on ? col : glm::vec3(0.0f);
	glUniform3f(glGetUniformLocation(prog, (n + "position").c_str()), pos.x, pos.y, pos.z);
	glUniform3f(glGetUniformLocation(prog, (n + "ambient").c_str()), c.x * ambientK, c.y * ambientK, c.z * ambientK);
	glUniform3f(glGetUniformLocation(prog, (n + "diffuse").c_str()), c.x, c.y, c.z);
	glUniform3f(glGetUniformLocation(prog, (n + "specular").c_str()), c.x, c.y, c.z);
	glUniform1f(glGetUniformLocation(prog, (n + "constant").c_str()), 1.0f);
	glUniform1f(glGetUniformLocation(prog, (n + "linear").c_str()), linear);
	glUniform1f(glGetUniformLocation(prog, (n + "quadratic").c_str()), quadratic);
}

static void SetSpotLight(GLuint prog, int i, const glm::vec3& pos, const glm::vec3& dir, const glm::vec3& col,
	float ambientK, float innerDeg, float outerDeg, float linear, float quadratic, bool on)
{
	std::string n = "spotLights[" + std::to_string(i) + "].";
	glm::vec3 c = on ? col : glm::vec3(0.0f);
	glUniform3f(glGetUniformLocation(prog, (n + "position").c_str()), pos.x, pos.y, pos.z);
	glUniform3f(glGetUniformLocation(prog, (n + "direction").c_str()), dir.x, dir.y, dir.z);
	glUniform3f(glGetUniformLocation(prog, (n + "ambient").c_str()), c.x * ambientK, c.y * ambientK, c.z * ambientK);
	glUniform3f(glGetUniformLocation(prog, (n + "diffuse").c_str()), c.x, c.y, c.z);
	glUniform3f(glGetUniformLocation(prog, (n + "specular").c_str()), c.x, c.y, c.z);
	glUniform1f(glGetUniformLocation(prog, (n + "constant").c_str()), 1.0f);
	glUniform1f(glGetUniformLocation(prog, (n + "linear").c_str()), linear);
	glUniform1f(glGetUniformLocation(prog, (n + "quadratic").c_str()), quadratic);
	glUniform1f(glGetUniformLocation(prog, (n + "cutOff").c_str()), glm::cos(glm::radians(innerDeg)));
	glUniform1f(glGetUniformLocation(prog, (n + "outerCutOff").c_str()), glm::cos(glm::radians(outerDeg)));
}

static void SetEmission(GLuint prog, int type, float cabin, float head, float rear, float lamp, float body = 0.0f)
{
	glUniform1f(glGetUniformLocation(prog, "emitBody"), body);
	glUniform1i(glGetUniformLocation(prog, "objectType"), type);
	glUniform1f(glGetUniformLocation(prog, "emitCabin"), cabin);
	glUniform1f(glGetUniformLocation(prog, "emitHead"), head);
	glUniform1f(glGetUniformLocation(prog, "emitRear"), rear);
	glUniform1f(glGetUniformLocation(prog, "emitLamp"), lamp);
}

int main()
{
	// Init GLFW
	glfwInit();

	// Create a GLFWwindow object
	GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Previo 9 - Cristian Rea Alberto (318273130)", nullptr, nullptr);

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

	// Load Models (modelos de Cristian Rea Alberto)
	Model Calle((char*)"Models/calle.obj");
	Model Farola((char*)"Models/farola.obj");
	Model Carro((char*)"Models/carro.obj");
	Model CristalGema((char*)"Models/cristal_gema.obj");
	Model Sol((char*)"Models/sol.obj");
	Model Luna((char*)"Models/luna.obj");

	std::cout << "==========================================================" << std::endl;
	std::cout << "  Previo 9: Fuentes de Luz en OpenGL" << std::endl;
	std::cout << "  Alumno: Cristian Rea Alberto (Cta: 318273130) - Gpo 7" << std::endl;
	std::cout << "==========================================================" << std::endl;
	std::cout << "  CONTROLES DE LA ESCENA:" << std::endl;
	std::cout << "  [W, A, S, D] -> Movimiento de la camara" << std::endl;
	std::cout << "  [Mouse]      -> Orientacion de la vista" << std::endl;
	std::cout << "----------------------------------------------------------" << std::endl;
	std::cout << "  INTERRUPTORES DE ENCENDIDO / APAGADO DE LUCES:" << std::endl;
	std::cout << "  [1] [2] [3] -> Farolas 1, 2 y 3 (lado izquierdo)" << std::endl;
	std::cout << "  [4] [5] [6] -> Farolas 4, 5 y 6 (lado derecho)" << std::endl;
	std::cout << "  [7] -> Iniciar / detener la rotacion del Sol y la Luna" << std::endl;
	std::cout << "  [Z] -> Luz interior del auto (conductor)" << std::endl;
	std::cout << "  [X] -> Direccionales traseras (intermitentes)" << std::endl;
	std::cout << "  [C] -> Faros delanteros (cofre)" << std::endl;
	std::cout << "  [F] -> Linterna de la camara" << std::endl;
	std::cout << "  [ESC] -> Salir de la aplicacion" << std::endl;
	std::cout << "==========================================================" << std::endl;

	// Set texture units in lightingShader
	lightingShader.Use();
	glUniform1i(glGetUniformLocation(lightingShader.Program, "material.diffuse"), 0);
	glUniform1i(glGetUniformLocation(lightingShader.Program, "material.specular"), 0);

	glm::mat4 projection = glm::perspective(camera.GetZoom(), (GLfloat)SCREEN_WIDTH / (GLfloat)SCREEN_HEIGHT, 0.1f, 100.0f);

	// Matrices de los objetos (no cambian durante la ejecucion)
	glm::mat4 carModel = glm::translate(glm::mat4(1.0f), carPos);
	carModel = glm::scale(carModel, glm::vec3(carScale));

	glm::mat4 lampModel[6];
	glm::vec3 lampPos[6];
	for (int i = 0; i < 6; i++)
	{
		bool left = (i < 3);
		lampModel[i] = glm::translate(glm::mat4(1.0f), glm::vec3(left ? -lampX : lampX, SIDEWALK_Y, lampZ[i % 3]));
		if (!left)
			lampModel[i] = glm::rotate(lampModel[i], glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		// posicion del foco en el mundo
		lampPos[i] = glm::vec3(lampModel[i] * glm::vec4(1.30f, 2.97f, 0.0f, 1.0f));
	}

	// Posiciones de las luces en el mundo, calculadas a partir de las coordenadas locales de cada modelo
	// (el auto avanza hacia +Z; sus medidas locales salen de carro.obj)
	glm::vec3 cabinPos     = glm::vec3(carModel * glm::vec4(0.0f, 1.30f, -0.10f, 1.0f));
	glm::vec3 rearLeftPos  = glm::vec3(carModel * glm::vec4(-0.52f, 0.72f, -1.46f, 1.0f));
	glm::vec3 rearRightPos = glm::vec3(carModel * glm::vec4( 0.52f, 0.72f, -1.46f, 1.0f));
	glm::vec3 headLeftPos  = glm::vec3(carModel * glm::vec4(-0.41f, 0.66f, 1.44f, 1.0f));
	glm::vec3 headRightPos = glm::vec3(carModel * glm::vec4( 0.41f, 0.66f, 1.44f, 1.0f));
	glm::vec3 headDir      = glm::normalize(glm::mat3(carModel) * glm::vec3(0.0f, -0.15f, 1.0f));
	glm::vec3 lampDir      = glm::vec3(0.0f, -1.0f, 0.0f);

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

		// Posicion del Sol y la Luna en su arco (la Luna siempre va del lado opuesto)
		if (skyRotating)
		{
			sunAngle += SKY_SPEED * deltaTime;
			if (sunAngle >= 360.0f) sunAngle -= 360.0f;
		}
		float a = glm::radians(sunAngle);
		glm::vec3 sunOffset = glm::vec3(-cos(a), sin(a), 0.0f) * SKY_RADIUS;
		glm::vec3 sunPos  = SKY_CENTER + sunOffset;
		glm::vec3 moonPos = SKY_CENTER - sunOffset;
		float elev = sin(a);                                   // altura del Sol: 1 arriba, -1 abajo

		float dayAmt   = Smooth(-0.20f, 0.35f, elev);          // 0 = noche, 1 = dia
		float twilight = exp(-(elev * elev) / (0.22f * 0.22f)); // maximo en el amanecer y el atardecer
		float sunW     = Smooth(0.0f, 0.15f, elev);            // intensidad de la luz del Sol
		float moonW    = Smooth(0.0f, 0.15f, -elev);           // intensidad de la luz de la Luna
		float warm     = 1.0f - Smooth(0.05f, 0.50f, elev);    // color naranja cuando el Sol esta bajo

		// Color del cielo: noche -> dia, con tono naranja en el amanecer y el atardecer
		glm::vec3 sky = glm::mix(glm::vec3(0.03f, 0.04f, 0.07f), glm::vec3(0.53f, 0.76f, 0.95f), dayAmt);
		sky = glm::mix(sky, glm::vec3(0.90f, 0.45f, 0.28f), 0.65f * twilight);

		// Clear color and depth buffers (cielo)
		glClearColor(sky.x, sky.y, sky.z, 1.0f);
				glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Enable Depth Test
		glEnable(GL_DEPTH_TEST);

		// Activate lighting shader
		lightingShader.Use();
		GLuint prog = lightingShader.Program;

		glUniform3f(glGetUniformLocation(prog, "viewPos"), camera.GetPosition().x, camera.GetPosition().y, camera.GetPosition().z);

		// Las direccionales parpadean mientras estan encendidas
		bool blinkPhase = fmod(glfwGetTime() * 1.25, 1.0) < 0.5;
		bool rearOn = rearSignalsActive && blinkPhase;

		// =====================================================================
		// 1. LUZ DIRECCIONAL: sigue al Sol de dia y a la Luna de noche
		//    (la direccion apunta desde el astro hacia el centro de la calle)
		// =====================================================================
		glm::vec3 target = glm::vec3(0.0f, ROAD_Y, 0.0f);
		glm::vec3 astro  = (elev >= 0.0f) ? sunPos : moonPos;
		glm::vec3 dirDir = glm::normalize(target - astro);

		glm::vec3 sunCol  = glm::mix(glm::vec3(1.00f, 0.95f, 0.82f), glm::vec3(1.00f, 0.45f, 0.15f), warm) * sunW;
		glm::vec3 moonCol = glm::vec3(0.14f, 0.16f, 0.25f) * moonW;
		glm::vec3 dirDif  = sunCol + moonCol;
		glm::vec3 dirSpe  = (sunCol * 0.45f) + (moonCol * 0.35f);
		glm::vec3 dirAmb  = glm::mix(glm::vec3(0.10f, 0.11f, 0.16f), glm::vec3(0.45f, 0.45f, 0.48f), dayAmt)
		                  + glm::vec3(0.20f, 0.09f, 0.03f) * twilight;

		glUniform3f(glGetUniformLocation(prog, "dirLight.direction"), dirDir.x, dirDir.y, dirDir.z);
		glUniform3f(glGetUniformLocation(prog, "dirLight.ambient"), dirAmb.x, dirAmb.y, dirAmb.z);
		glUniform3f(glGetUniformLocation(prog, "dirLight.diffuse"), dirDif.x, dirDif.y, dirDif.z);
		glUniform3f(glGetUniformLocation(prog, "dirLight.specular"), dirSpe.x, dirSpe.y, dirSpe.z);

		// =====================================================================
		// 2. LUCES PUNTUALES
		//    [0] interior del auto | [1] direccional trasera izq. | [2] direccional trasera der.
		// =====================================================================
		SetPointLight(prog, 0, cabinPos, glm::vec3(1.0f, 0.85f, 0.55f) * 2.0f, 0.0f, 0.70f, 1.80f, cabinLightActive);
		SetPointLight(prog, 1, rearLeftPos,  glm::vec3(1.0f, 0.42f, 0.05f) * 2.5f, 0.0f, 0.70f, 1.80f, rearOn);
		SetPointLight(prog, 2, rearRightPos, glm::vec3(1.0f, 0.42f, 0.05f) * 2.5f, 0.0f, 0.70f, 1.80f, rearOn);

		// =====================================================================
		// 3. LUCES REFLECTORAS (SPOTLIGHTS)
		//    [0],[1] faros delanteros | [2]..[7] farolas 1 a 6 | [8] linterna de la camara
		// =====================================================================
		glm::vec3 headColor = glm::vec3(1.0f, 0.96f, 0.85f) * 3.8f;
		SetSpotLight(prog, 0, headLeftPos,  headDir, headColor, 0.0f, 18.0f, 32.0f, 0.045f, 0.0075f, headlightsActive);
		SetSpotLight(prog, 1, headRightPos, headDir, headColor, 0.0f, 18.0f, 32.0f, 0.045f, 0.0075f, headlightsActive);

		glm::vec3 lampColor = glm::vec3(1.0f, 0.78f, 0.45f) * 3.6f;
		for (int i = 0; i < 6; i++)
			SetSpotLight(prog, 2 + i, lampPos[i], lampDir, lampColor, 0.03f, 25.0f, 50.0f, 0.07f, 0.02f, streetLampActive[i]);

		SetSpotLight(prog, 8, camera.GetPosition(), camera.GetFront(), glm::vec3(0.95f), 0.05f, 12.5f, 17.5f,
			0.09f, 0.032f, flashlightActive);

		// Camera transformations
		glm::mat4 view = camera.GetViewMatrix();
		GLint modelLoc = glGetUniformLocation(prog, "model");
		GLint viewLoc = glGetUniformLocation(prog, "view");
		GLint projLoc = glGetUniformLocation(prog, "projection");

		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

		// =====================================================================
		// RENDERIZADO DE MODELOS OPACOS
		// =====================================================================
		// 1) Calle (calzada, banquetas y guarniciones)
		glm::mat4 model = glm::mat4(1.0f);
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		glUniform1i(glGetUniformLocation(prog, "transparency"), 0);
		glUniform1f(glGetUniformLocation(prog, "material.shininess"), 16.0f);
		SetEmission(prog, 0, 0.0f, 0.0f, 0.0f, 0.0f);
		Calle.Draw(lightingShader);

		// 2) Carro (las partes luminosas brillan segun el interruptor correspondiente)
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(carModel));
		glUniform1f(glGetUniformLocation(prog, "material.shininess"), 32.0f);
		SetEmission(prog, 1, cabinLightActive ? 1.0f : 0.0f, headlightsActive ? 1.0f : 0.0f, rearOn ? 1.0f : 0.0f, 0.0f);
		Carro.Draw(lightingShader);

		// 3) Farolas (el foco brilla solo si su interruptor esta encendido)
		glUniform1f(glGetUniformLocation(prog, "material.shininess"), 48.0f);
		for (int i = 0; i < 6; i++)
		{
			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(lampModel[i]));
			SetEmission(prog, 2, 0.0f, 0.0f, 0.0f, streetLampActive[i] ? 1.0f : 0.0f);
			Farola.Draw(lightingShader);
		}

		// =====================================================================
		// RENDERIZADO DE MODELO CON TRANSPARENCIA (Gema de Cristal 3D)
		// Se dibuja despues de los opacos con Blending activado
		// =====================================================================
		// Sol y Luna (se dibujan con su textura sin iluminar, como cuerpos que emiten luz)
		glUniform1i(glGetUniformLocation(prog, "transparency"), 0);
		if (elev > -0.03f)
		{
			model = glm::translate(glm::mat4(1.0f), sunPos);
			model = glm::rotate(model, glm::radians((float)glfwGetTime() * 6.0f), glm::vec3(0.0f, 0.0f, 1.0f));
			model = glm::scale(model, glm::vec3(1.8f));
			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
			SetEmission(prog, 3, 0.0f, 0.0f, 0.0f, 0.0f, 1.1f);
			Sol.Draw(lightingShader);
		}
		if (elev < 0.03f)
		{
			model = glm::translate(glm::mat4(1.0f), moonPos);
			model = glm::rotate(model, glm::radians((float)glfwGetTime() * 3.0f), glm::vec3(0.0f, 1.0f, 0.0f));
			model = glm::scale(model, glm::vec3(1.3f));
			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
			SetEmission(prog, 3, 0.0f, 0.0f, 0.0f, 0.0f, 0.95f);
			Luna.Draw(lightingShader);
		}

		SetEmission(prog, 0, 0.0f, 0.0f, 0.0f, 0.0f);
		model = glm::mat4(1.0f);
		float floatingOffset = (float)sin(glfwGetTime() * 1.5f) * 0.08f;
		float rotationAngle = (float)glfwGetTime() * 35.0f; // Rotacion continua sobre su eje Y
		model = glm::translate(model, glm::vec3(carPos.x, 0.75f + floatingOffset, carPos.z));
		model = glm::rotate(model, glm::radians(rotationAngle), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, glm::radians(15.0f), glm::vec3(1.0f, 0.0f, 0.2f)); // Inclinacion para reflejos
		model = glm::scale(model, glm::vec3(0.6f));

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		glUniform1i(glGetUniformLocation(prog, "transparency"), 1);
		CristalGema.Draw(lightingShader);
		glDisable(GL_BLEND);

		glBindVertexArray(0);

		// Swap buffers
		glfwSwapBuffers(window);
	}

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
		// Farolas 1 a 6: Teclas 1 a 6
		if (key >= GLFW_KEY_1 && key <= GLFW_KEY_6)
		{
			int i = key - GLFW_KEY_1;
			streetLampActive[i] = !streetLampActive[i];
			std::cout << "[LUZ] Farola " << (i + 1) << ": " << (streetLampActive[i] ? "ENCENDIDA" : "APAGADA") << std::endl;
		}

		// Sol y Luna: Tecla 7 (inicia o detiene la rotacion)
		if (key == GLFW_KEY_7)
		{
			skyRotating = !skyRotating;
			std::cout << "[AMBIENTE] Rotacion del Sol y la Luna: " << (skyRotating ? "EN MOVIMIENTO" : "DETENIDA") << std::endl;
		}

		// Luz interior del auto: Tecla Z
		if (key == GLFW_KEY_Z)
		{
			cabinLightActive = !cabinLightActive;
			std::cout << "[LUZ] Interior del auto: " << (cabinLightActive ? "ENCENDIDA" : "APAGADA") << std::endl;
		}

		// Direccionales traseras: Tecla X
		if (key == GLFW_KEY_X)
		{
			rearSignalsActive = !rearSignalsActive;
			std::cout << "[LUZ] Direccionales traseras: " << (rearSignalsActive ? "ENCENDIDAS" : "APAGADAS") << std::endl;
		}

		// Faros delanteros: Tecla C
		if (key == GLFW_KEY_C)
		{
			headlightsActive = !headlightsActive;
			std::cout << "[LUZ] Faros delanteros: " << (headlightsActive ? "ENCENDIDOS" : "APAGADOS") << std::endl;
		}

		// Linterna (Spotlight): Tecla F o Tecla L
		if (key == GLFW_KEY_F || key == GLFW_KEY_L)
		{
			flashlightActive = !flashlightActive;
			std::cout << "[LUZ] Linterna: " << (flashlightActive ? "ENCENDIDA" : "APAGADA") << std::endl;
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
