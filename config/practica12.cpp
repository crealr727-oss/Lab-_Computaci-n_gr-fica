// ============================================================================
// Previo 12: Animacion por KeyFrames
// Animaciones: sentado y saludo (tecla M / R, reproducir con L)
//              MUERTE del perro (tecla 5, reproducir de nuevo para revivirlo)
// Grabacion: tecla K guarda cuadros en memoria, tecla P los escribe en
//            "keyframes.txt" y tecla F los carga de ese archivo y los reproduce.
//            La secuencia de muerte se guarda sola en "animacion_muerte.txt".
// Alumno: Rea Alberto Cristian
// No. de Cuenta: 318273130
// FI UNAM - Grupo 7
// Fecha de entrega: 8  de noviembre del 2026
// ============================================================================

#include <iostream>
#include <fstream>
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
void Animation();
void cargarSecuenciaSentadoSaludo();
void cargarSecuenciaMuerte();
void guardarKeyFramesTXT(const char* nombreArchivo);
bool cargarKeyFramesTXT(const char* nombreArchivo);

// Window dimensions
const GLuint WIDTH = 800, HEIGHT = 600;
int SCREEN_WIDTH, SCREEN_HEIGHT;

// Camera
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
GLfloat lastX = WIDTH / 2.0;
GLfloat lastY = HEIGHT / 2.0;
bool keys[1024];
bool firstMouse = true;

// Light attributes
glm::vec3 lightPos(0.0f, 0.0f, 0.0f);
bool active;

// Positions of the point lights
glm::vec3 pointLightPositions[] = {
	glm::vec3(0.0f, 2.0f, 0.0f),
	glm::vec3(0.0f, 0.0f, 0.0f),
	glm::vec3(0.0f, 0.0f, 0.0f),
	glm::vec3(0.0f, 0.0f, 0.0f)
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

// Variables de transformacion y articulaciones independientes del perro
float rotBall = 0.0f;
float rotDog = 0.0f;
float inclinacionCuerpo = 0.0f;
int dogAnim = 0;
float head = 0.0f;
float tail = 0.0f;
float pataDelIzq = 0.0f;
float pataDelDer = 0.0f;
float pataTrasIzq = 0.0f;
float pataTrasDer = 0.0f;

// Posicion del perro por ejes separados
float dogPosX = 0.0f, dogPosY = 0.0f, dogPosZ = 0.0f;

// Configuracion de KeyFrames
#define MAX_FRAMES 15
int i_max_steps = 160;
int i_curr_steps = 0;

typedef struct _frame {
	float rotDog;
	float rotDogInc;
	float inclinacionCuerpo;
	float inclinacionCuerpoInc;
	float dogPosX;
	float dogPosY;
	float dogPosZ;
	float incX;
	float incY;
	float incZ;
	float head;
	float headInc;
	float tail;
	float tailInc;
	float pataDelIzq;
	float pataDelIzqInc;
	float pataDelDer;
	float pataDelDerInc;
	float pataTrasIzq;
	float pataTrasIzqInc;
	float pataTrasDer;
	float pataTrasDerInc;
} FRAME;

FRAME KeyFrame[MAX_FRAMES];
int FrameIndex = 0;
bool play = false;
int playIndex = 0;
bool grabacionManualActiva = false;
bool perroMuerto = false;      // true cuando termina la animacion de muerte
bool animacionMuerte = false;  // true mientras la secuencia cargada es la de muerte

void saveFrame(void)
{
	printf("frameindex %d\n", FrameIndex);

	KeyFrame[FrameIndex].dogPosX = dogPosX;
	KeyFrame[FrameIndex].dogPosY = dogPosY;
	KeyFrame[FrameIndex].dogPosZ = dogPosZ;

	KeyFrame[FrameIndex].rotDog = rotDog;
	KeyFrame[FrameIndex].inclinacionCuerpo = inclinacionCuerpo;
	KeyFrame[FrameIndex].head = head;
	KeyFrame[FrameIndex].tail = tail;
	KeyFrame[FrameIndex].pataDelIzq = pataDelIzq;
	KeyFrame[FrameIndex].pataDelDer = pataDelDer;
	KeyFrame[FrameIndex].pataTrasIzq = pataTrasIzq;
	KeyFrame[FrameIndex].pataTrasDer = pataTrasDer;

	FrameIndex++;
}

void resetElements(void)
{
	dogPosX = KeyFrame[0].dogPosX;
	dogPosY = KeyFrame[0].dogPosY;
	dogPosZ = KeyFrame[0].dogPosZ;

	rotDog = KeyFrame[0].rotDog;
	inclinacionCuerpo = KeyFrame[0].inclinacionCuerpo;
	head = KeyFrame[0].head;
	tail = KeyFrame[0].tail;
	pataDelIzq = KeyFrame[0].pataDelIzq;
	pataDelDer = KeyFrame[0].pataDelDer;
	pataTrasIzq = KeyFrame[0].pataTrasIzq;
	pataTrasDer = KeyFrame[0].pataTrasDer;
}

void interpolation(void)
{
	KeyFrame[playIndex].incX = (KeyFrame[playIndex + 1].dogPosX - KeyFrame[playIndex].dogPosX) / i_max_steps;
	KeyFrame[playIndex].incY = (KeyFrame[playIndex + 1].dogPosY - KeyFrame[playIndex].dogPosY) / i_max_steps;
	KeyFrame[playIndex].incZ = (KeyFrame[playIndex + 1].dogPosZ - KeyFrame[playIndex].dogPosZ) / i_max_steps;

	KeyFrame[playIndex].rotDogInc = (KeyFrame[playIndex + 1].rotDog - KeyFrame[playIndex].rotDog) / i_max_steps;
	KeyFrame[playIndex].inclinacionCuerpoInc = (KeyFrame[playIndex + 1].inclinacionCuerpo - KeyFrame[playIndex].inclinacionCuerpo) / i_max_steps;
	KeyFrame[playIndex].headInc = (KeyFrame[playIndex + 1].head - KeyFrame[playIndex].head) / i_max_steps;
	KeyFrame[playIndex].tailInc = (KeyFrame[playIndex + 1].tail - KeyFrame[playIndex].tail) / i_max_steps;
	KeyFrame[playIndex].pataDelIzqInc = (KeyFrame[playIndex + 1].pataDelIzq - KeyFrame[playIndex].pataDelIzq) / i_max_steps;
	KeyFrame[playIndex].pataDelDerInc = (KeyFrame[playIndex + 1].pataDelDer - KeyFrame[playIndex].pataDelDer) / i_max_steps;
	KeyFrame[playIndex].pataTrasIzqInc = (KeyFrame[playIndex + 1].pataTrasIzq - KeyFrame[playIndex].pataTrasIzq) / i_max_steps;
	KeyFrame[playIndex].pataTrasDerInc = (KeyFrame[playIndex + 1].pataTrasDer - KeyFrame[playIndex].pataTrasDer) / i_max_steps;
}

// Funcion para registrar los cuadros clave de la animacion solicitada:
// el perro se sienta y posteriormente estira la pata delantera derecha saludando
void cargarSecuenciaSentadoSaludo(void)
{
	i_max_steps = 160;
	FrameIndex = 0;
	play = false;
	grabacionManualActiva = false;
	perroMuerto = false;
	animacionMuerte = false;

	// Cuadro 0: Postura base de pie
	dogPosX = 0.0f; dogPosY = 0.0f; dogPosZ = 0.0f;
	rotDog = 0.0f; inclinacionCuerpo = 0.0f;
	head = 0.0f; tail = 0.0f;
	pataDelIzq = 0.0f; pataDelDer = 0.0f;
	pataTrasIzq = 0.0f; pataTrasDer = 0.0f;
	saveFrame();

	// Cuadro 1: Flexion inicial de cadera y patas traseras
	dogPosY = -0.02f;
	inclinacionCuerpo = -10.0f;
	pataDelIzq = 10.0f; pataDelDer = 10.0f;
	pataTrasIzq = -35.0f; pataTrasDer = -35.0f;
	head = 4.0f; tail = 8.0f;
	saveFrame();

	// Cuadro 2: Perro completamente sentado sobre el piso
	dogPosY = -0.04f;
	inclinacionCuerpo = -18.0f;
	pataDelIzq = 18.0f; pataDelDer = 18.0f;
	pataTrasIzq = -65.0f; pataTrasDer = -65.0f;
	head = 8.0f; tail = 15.0f;
	saveFrame();

	// Cuadro 3: Levanta y estira la pata delantera derecha para saludar
	dogPosY = -0.04f;
	inclinacionCuerpo = -18.0f;
	pataDelIzq = 18.0f; pataDelDer = -75.0f;
	pataTrasIzq = -65.0f; pataTrasDer = -65.0f;
	head = 14.0f; tail = -18.0f;
	saveFrame();

	// Cuadro 4: Oscilacion de saludo con la pata derecha y movimiento de cola
	dogPosY = -0.04f;
	inclinacionCuerpo = -18.0f;
	pataDelIzq = 18.0f; pataDelDer = -45.0f;
	pataTrasIzq = -65.0f; pataTrasDer = -65.0f;
	head = -8.0f; tail = 18.0f;
	saveFrame();

	// Cuadro 5: Segundo gesto de saludo elevando la pata derecha
	dogPosY = -0.04f;
	inclinacionCuerpo = -18.0f;
	pataDelIzq = 18.0f; pataDelDer = -80.0f;
	pataTrasIzq = -65.0f; pataTrasDer = -65.0f;
	head = 12.0f; tail = -15.0f;
	saveFrame();

	// Cuadro 6: Regresa la pata derecha al suelo manteniendo la posicion sentada
	dogPosY = -0.04f;
	inclinacionCuerpo = -18.0f;
	pataDelIzq = 18.0f; pataDelDer = 18.0f;
	pataTrasIzq = -65.0f; pataTrasDer = -65.0f;
	head = 0.0f; tail = 8.0f;
	saveFrame();

	// Dejar al perro en la postura inicial (Cuadro 0) listo para reproducir con L
	resetElements();
}


// ---------------------------------------------------------------------------
// Secuencia de MUERTE del perro con giros y final BOCARRIBA (tecla 5).
// 1. Fase de giros: el perro se marea y da vueltas sobre si mismo
//    (rotDog 0 a 360 deg) mientras pierde balance.
// 2. Vuelco hacia atras: el cuerpo rota hasta quedar completamente
//    bocarriba / panza arriba (inclinacionCuerpo = 180 deg).
// 3. Contacto exacto con el pasto: el lomo descansa en dogPosY = -0.18f
//    sobre la hierba sin hundirse; las patas quedan hacia arriba flacidas
//    en el aire (sin tocar ni atravesar el suelo); la cabeza descansa
//    ladeada contra el pasto (-50 deg); ultimo espasmo y muerte.
// Al final la secuencia se guarda sola en "animacion_muerte.txt".
// ---------------------------------------------------------------------------
void cargarSecuenciaMuerte(void)
{
	i_max_steps = 90;
	FrameIndex = 0;
	play = false;
	grabacionManualActiva = false;
	perroMuerto = false;
	animacionMuerte = true;

	// Cuadro 0: Postura base de pie (perro con vida sobre sus 4 patas)
	dogPosX = 0.0f; dogPosY = 0.0f; dogPosZ = 0.0f;
	rotDog = 0.0f; inclinacionCuerpo = 0.0f;
	head = 0.0f; tail = 0.0f;
	pataDelIzq = 0.0f; pataDelDer = 0.0f;
	pataTrasIzq = 0.0f; pataTrasDer = 0.0f;
	saveFrame();

	// Cuadro 1: Inicia el mareo y comienza a dar vueltas (giro 90 deg)
	dogPosX = 0.02f; dogPosY = -0.02f; dogPosZ = 0.01f;
	rotDog = 90.0f; inclinacionCuerpo = 15.0f;
	head = 15.0f; tail = 20.0f;
	pataDelIzq = 15.0f; pataDelDer = -15.0f;
	pataTrasIzq = -15.0f; pataTrasDer = 15.0f;
	saveFrame();

	// Cuadro 2: Giro desorientado perdiendo el equilibrio (giro 180 deg)
	dogPosX = 0.03f; dogPosY = -0.05f; dogPosZ = -0.01f;
	rotDog = 180.0f; inclinacionCuerpo = 45.0f;
	head = -20.0f; tail = -15.0f;
	pataDelIzq = 30.0f; pataDelDer = 10.0f;
	pataTrasIzq = -20.0f; pataTrasDer = -10.0f;
	saveFrame();

	// Cuadro 3: Trompicon fuerte y vuelco hacia atras (giro 270 deg)
	dogPosX = 0.01f; dogPosY = -0.10f; dogPosZ = -0.02f;
	rotDog = 270.0f; inclinacionCuerpo = 90.0f;
	head = -35.0f; tail = -20.0f;
	pataDelIzq = 40.0f; pataDelDer = 30.0f;
	pataTrasIzq = 20.0f; pataTrasDer = 15.0f;
	saveFrame();

	// Cuadro 4: Completa el giro y cae de espaldas hacia el pasto (giro 360 deg)
	dogPosX = 0.0f; dogPosY = -0.15f; dogPosZ = 0.0f;
	rotDog = 360.0f; inclinacionCuerpo = 140.0f;
	head = -40.0f; tail = -15.0f;
	pataDelIzq = 25.0f; pataDelDer = 20.0f;
	pataTrasIzq = 10.0f; pataTrasDer = 5.0f;
	saveFrame();

	// Cuadro 5: Impacto contra el pasto - Cae de lomo y queda BOCARRIBA (180 deg)
	dogPosX = 0.0f; dogPosY = -0.19f; dogPosZ = 0.02f;
	rotDog = 400.0f; inclinacionCuerpo = 180.0f;
	head = -45.0f; tail = -20.0f;
	pataDelIzq = -15.0f; pataDelDer = 20.0f;
	pataTrasIzq = 15.0f; pataTrasDer = -15.0f;
	saveFrame();

	// Cuadro 6: Rebote natural del golpe contra el pasto
	dogPosX = 0.0f; dogPosY = -0.17f; dogPosZ = 0.022f;
	rotDog = 405.0f; inclinacionCuerpo = 178.0f;
	head = -40.0f; tail = -18.0f;
	pataDelIzq = -10.0f; pataDelDer = 15.0f;
	pataTrasIzq = 10.0f; pataTrasDer = -10.0f;
	saveFrame();

	// Cuadro 7: Asentamiento final bocarriba sobre la hierba
	dogPosX = 0.0f; dogPosY = -0.18f; dogPosZ = 0.025f;
	rotDog = 408.0f; inclinacionCuerpo = 180.0f;
	head = -48.0f; tail = -22.0f;
	pataDelIzq = -12.0f; pataDelDer = 18.0f;
	pataTrasIzq = 12.0f; pataTrasDer = -12.0f;
	saveFrame();

	// Cuadro 8: Ultimo espasmo agonico de patitas en el aire
	dogPosX = 0.0f; dogPosY = -0.175f; dogPosZ = 0.025f;
	rotDog = 408.0f; inclinacionCuerpo = 180.0f;
	head = -45.0f; tail = -20.0f;
	pataDelIzq = -5.0f; pataDelDer = 25.0f;
	pataTrasIzq = 20.0f; pataTrasDer = -5.0f;
	saveFrame();

	// Cuadro 9: Relajacion muscular total - Panza arriba y patas inertes al cielo
	dogPosX = 0.0f; dogPosY = -0.18f; dogPosZ = 0.025f;
	rotDog = 408.0f; inclinacionCuerpo = 180.0f;
	head = -50.0f; tail = -22.0f;
	pataDelIzq = -10.0f; pataDelDer = 18.0f;
	pataTrasIzq = 12.0f; pataTrasDer = -12.0f;
	saveFrame();

	// Cuadro 10: Inmovilidad cadaverica definitiva bocarriba en el pasto
	dogPosX = 0.0f; dogPosY = -0.18f; dogPosZ = 0.025f;
	rotDog = 408.0f; inclinacionCuerpo = 180.0f;
	head = -50.0f; tail = -22.0f;
	pataDelIzq = -10.0f; pataDelDer = 18.0f;
	pataTrasIzq = 12.0f; pataTrasDer = -12.0f;
	saveFrame();

	// Dejar al perro en la postura inicial (Cuadro 0) listo para reproducir
	resetElements();

	// Grabar la animacion de muerte en un archivo de texto
	guardarKeyFramesTXT("animacion_muerte.txt");
}

// ---------------------------------------------------------------------------
// GRABACION EN ARCHIVO TXT
// Formato de "keyframes.txt" / "animacion_muerte.txt":
//   primera linea: numero de cuadros guardados
//   cada cuadro, una linea con 11 valores separados por espacio:
//   dogPosX dogPosY dogPosZ rotDog inclinacionCuerpo head tail
//   pataDelIzq pataDelDer pataTrasIzq pataTrasDer
// Asi la animacion que grabes con K (y guardes con P) queda en disco y se
// puede volver a cargar con F sin tener que grabarla otra vez.
// ---------------------------------------------------------------------------
void guardarKeyFramesTXT(const char* nombreArchivo)
{
	if (FrameIndex <= 0)
	{
		printf("No hay cuadros para guardar en %s.\n", nombreArchivo);
		return;
	}

	std::ofstream archivo(nombreArchivo);
	if (!archivo.is_open())
	{
		printf("No se pudo crear el archivo %s.\n", nombreArchivo);
		return;
	}

	archivo << FrameIndex << "\n";
	for (int i = 0; i < FrameIndex; i++)
	{
		archivo << KeyFrame[i].dogPosX << " "
			<< KeyFrame[i].dogPosY << " "
			<< KeyFrame[i].dogPosZ << " "
			<< KeyFrame[i].rotDog << " "
			<< KeyFrame[i].inclinacionCuerpo << " "
			<< KeyFrame[i].head << " "
			<< KeyFrame[i].tail << " "
			<< KeyFrame[i].pataDelIzq << " "
			<< KeyFrame[i].pataDelDer << " "
			<< KeyFrame[i].pataTrasIzq << " "
			<< KeyFrame[i].pataTrasDer << "\n";
	}
	archivo.close();
	printf("Animacion grabada en %s (%d cuadros).\n", nombreArchivo, FrameIndex);
}

bool cargarKeyFramesTXT(const char* nombreArchivo)
{
	std::ifstream archivo(nombreArchivo);
	if (!archivo.is_open())
	{
		printf("No se pudo abrir el archivo %s.\n", nombreArchivo);
		return false;
	}

	int n = 0;
	archivo >> n;
	if (n <= 0 || n > MAX_FRAMES)
	{
		printf("Archivo %s con numero de cuadros invalido (%d).\n", nombreArchivo, n);
		return false;
	}

	FrameIndex = 0;
	play = false;
	grabacionManualActiva = false;
	perroMuerto = false;
	animacionMuerte = false;

	for (int i = 0; i < n; i++)
	{
		archivo >> KeyFrame[i].dogPosX
			>> KeyFrame[i].dogPosY
			>> KeyFrame[i].dogPosZ
			>> KeyFrame[i].rotDog
			>> KeyFrame[i].inclinacionCuerpo
			>> KeyFrame[i].head
			>> KeyFrame[i].tail
			>> KeyFrame[i].pataDelIzq
			>> KeyFrame[i].pataDelDer
			>> KeyFrame[i].pataTrasIzq
			>> KeyFrame[i].pataTrasDer;
		FrameIndex++;
	}
	archivo.close();
	printf("Animacion cargada de %s (%d cuadros).\n", nombreArchivo, FrameIndex);
	return true;
}

// Deltatime
GLfloat deltaTime = 0.0f;
GLfloat lastFrame = 0.0f;

int main()
{
	// Init GLFW
	glfwInit();

	// Create a GLFWwindow object that we can use for GLFW's functions
	GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Practica 12 - Rea Alberto Cristian", nullptr, nullptr);

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

	// Carga de modelos
	Model DogBody((char*)"Models/DogBody.obj");
	Model HeadDog((char*)"Models/HeadDog.obj");
	Model DogTail((char*)"Models/TailDog.obj");
	Model F_RightLeg((char*)"Models/F_RightLegDog.obj");
	Model F_LeftLeg((char*)"Models/F_LeftLegDog.obj");
	Model B_RightLeg((char*)"Models/B_RightLegDog.obj");
	Model B_LeftLeg((char*)"Models/B_LeftLegDog.obj");
	Model Piso((char*)"Models/piso.obj");
	Model Ball((char*)"Models/ball.obj");

	// Inicializacion de la estructura KeyFrame
	for (int i = 0; i < MAX_FRAMES; i++)
	{
		KeyFrame[i].dogPosX = 0;
		KeyFrame[i].dogPosY = 0;
		KeyFrame[i].dogPosZ = 0;
		KeyFrame[i].incX = 0;
		KeyFrame[i].incY = 0;
		KeyFrame[i].incZ = 0;
		KeyFrame[i].rotDog = 0;
		KeyFrame[i].rotDogInc = 0;
		KeyFrame[i].inclinacionCuerpo = 0;
		KeyFrame[i].inclinacionCuerpoInc = 0;
		KeyFrame[i].head = 0;
		KeyFrame[i].headInc = 0;
		KeyFrame[i].tail = 0;
		KeyFrame[i].tailInc = 0;
		KeyFrame[i].pataDelIzq = 0;
		KeyFrame[i].pataDelIzqInc = 0;
		KeyFrame[i].pataDelDer = 0;
		KeyFrame[i].pataDelDerInc = 0;
		KeyFrame[i].pataTrasIzq = 0;
		KeyFrame[i].pataTrasIzqInc = 0;
		KeyFrame[i].pataTrasDer = 0;
		KeyFrame[i].pataTrasDerInc = 0;
	}

	// Precargar desde el inicio la animacion de sentarse y saludar (lista para reproducir con L)
	cargarSecuenciaSentadoSaludo();

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
	glUniform1i(glGetUniformLocation(lightingShader.Program, "Material.difuse"), 0);
	glUniform1i(glGetUniformLocation(lightingShader.Program, "Material.specular"), 1);

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

		glm::mat4 modelTemp = glm::mat4(1.0f);

		// Use cooresponding shader when setting uniforms/drawing objects
		lightingShader.Use();

		glUniform1i(glGetUniformLocation(lightingShader.Program, "diffuse"), 0);

		GLint viewPosLoc = glGetUniformLocation(lightingShader.Program, "viewPos");
		glUniform3f(viewPosLoc, camera.GetPosition().x, camera.GetPosition().y, camera.GetPosition().z);

		// Directional light
		glUniform3f(glGetUniformLocation(lightingShader.Program, "dirLight.direction"), -0.2f, -1.0f, -0.3f);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "dirLight.ambient"), 0.6f, 0.6f, 0.6f);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "dirLight.diffuse"), 0.6f, 0.6f, 0.6f);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "dirLight.specular"), 0.3f, 0.3f, 0.3f);

		// Point light 1
		glm::vec3 lightColor;
		lightColor.x = abs(sin(glfwGetTime() * Light1.x));
		lightColor.y = abs(sin(glfwGetTime() * Light1.y));
		lightColor.z = sin(glfwGetTime() * Light1.z);

		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[0].position"), pointLightPositions[0].x, pointLightPositions[0].y, pointLightPositions[0].z);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[0].ambient"), lightColor.x, lightColor.y, lightColor.z);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[0].diffuse"), lightColor.x, lightColor.y, lightColor.z);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[0].specular"), 1.0f, 0.2f, 0.2f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[0].constant"), 1.0f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[0].linear"), 0.045f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[0].quadratic"), 0.075f);

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

		// Carga de modelo de piso
		view = camera.GetViewMatrix();
		model = glm::mat4(1);
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		Piso.Draw(lightingShader);

		model = glm::mat4(1);
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		glUniform1i(glGetUniformLocation(lightingShader.Program, "transparency"), 0);

		// Body
		modelTemp = model = glm::translate(model, glm::vec3(dogPosX, dogPosY, dogPosZ));
		modelTemp = model = glm::rotate(model, glm::radians(rotDog), glm::vec3(0.0f, 1.0f, 0.0f));
		modelTemp = model = glm::rotate(model, glm::radians(inclinacionCuerpo), glm::vec3(1.0f, 0.0f, 0.0f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		DogBody.Draw(lightingShader);

		// Head
		model = modelTemp;
		model = glm::translate(model, glm::vec3(0.0f, 0.093f, 0.208f));
		model = glm::rotate(model, glm::radians(head), glm::vec3(0.0f, 0.0f, 1.0f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		HeadDog.Draw(lightingShader);

		// Tail
		model = modelTemp;
		model = glm::translate(model, glm::vec3(0.0f, 0.026f, -0.288f));
		model = glm::rotate(model, glm::radians(tail), glm::vec3(0.0f, 0.0f, -1.0f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		DogTail.Draw(lightingShader);

		// Front Left Leg
		model = modelTemp;
		model = glm::translate(model, glm::vec3(0.112f, -0.044f, 0.074f));
		model = glm::rotate(model, glm::radians(pataDelIzq), glm::vec3(1.0f, 0.0f, 0.0f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		F_LeftLeg.Draw(lightingShader);

		// Front Right Leg
		model = modelTemp;
		model = glm::translate(model, glm::vec3(-0.111f, -0.055f, 0.074f));
		model = glm::rotate(model, glm::radians(pataDelDer), glm::vec3(1.0f, 0.0f, 0.0f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		F_RightLeg.Draw(lightingShader);

		// Back Left Leg
		model = modelTemp;
		model = glm::translate(model, glm::vec3(0.082f, -0.046f, -0.218f));
		model = glm::rotate(model, glm::radians(pataTrasIzq), glm::vec3(1.0f, 0.0f, 0.0f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		B_LeftLeg.Draw(lightingShader);

		// Back Right Leg
		model = modelTemp;
		model = glm::translate(model, glm::vec3(-0.083f, -0.057f, -0.231f));
		model = glm::rotate(model, glm::radians(pataTrasDer), glm::vec3(1.0f, 0.0f, 0.0f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		B_RightLeg.Draw(lightingShader);

		// Ball
		model = glm::mat4(1);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		glUniform1i(glGetUniformLocation(lightingShader.Program, "transparency"), 1);
		model = glm::rotate(model, glm::radians(rotBall), glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		Ball.Draw(lightingShader);
		glDisable(GL_BLEND);
		glBindVertexArray(0);

		// Lamp object
		lampShader.Use();
		modelLoc = glGetUniformLocation(lampShader.Program, "model");
		viewLoc = glGetUniformLocation(lampShader.Program, "view");
		projLoc = glGetUniformLocation(lampShader.Program, "projection");

		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));
		model = glm::mat4(1);
		model = glm::translate(model, lightPos);
		model = glm::scale(model, glm::vec3(0.2f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

		model = glm::mat4(1);
		model = glm::translate(model, pointLightPositions[0]);
		model = glm::scale(model, glm::vec3(0.2f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		glBindVertexArray(VAO);
		glDrawArrays(GL_TRIANGLES, 0, 36);

		glBindVertexArray(0);

		// Swap the screen buffers
		glfwSwapBuffers(window);
	}

	glfwTerminate();
	return 0;
}

// Moves/alters the camera positions based on user input
void DoMovement()
{
	// Giro horizontal e inclinacion del cuerpo
	if (keys[GLFW_KEY_2])
	{
		rotDog += 1.0f;
	}
	if (keys[GLFW_KEY_3])
	{
		rotDog -= 1.0f;
	}
	if (keys[GLFW_KEY_Q])
	{
		inclinacionCuerpo += 1.0f;
	}
	if (keys[GLFW_KEY_E])
	{
		inclinacionCuerpo -= 1.0f;
	}

	// Movimiento de cabeza (como en el video) y cola
	// NOTA: la tecla 5 ya NO baja la cabeza: ahora es la tecla de la MUERTE
	// (ver KeyCallback). La cabeza solo se sube con la tecla 4.
	if (keys[GLFW_KEY_4])
	{
		head += 1.0f;
	}
	if (keys[GLFW_KEY_6])
	{
		tail += 1.0f;
	}
	if (keys[GLFW_KEY_7])
	{
		tail -= 1.0f;
	}

	// Articulacion individual de las 4 patas
	if (keys[GLFW_KEY_8])
	{
		pataDelDer -= 1.0f;
	}
	if (keys[GLFW_KEY_9])
	{
		pataDelDer += 1.0f;
	}
	if (keys[GLFW_KEY_Z])
	{
		pataDelIzq -= 1.0f;
	}
	if (keys[GLFW_KEY_X])
	{
		pataDelIzq += 1.0f;
	}
	if (keys[GLFW_KEY_C])
	{
		pataTrasDer -= 1.0f;
	}
	if (keys[GLFW_KEY_V])
	{
		pataTrasDer += 1.0f;
	}
	if (keys[GLFW_KEY_B])
	{
		pataTrasIzq -= 1.0f;
	}
	if (keys[GLFW_KEY_N])
	{
		pataTrasIzq += 1.0f;
	}

	// Desplazamiento en X, Y, Z del perro
	if (keys[GLFW_KEY_H])
	{
		dogPosZ += 0.01f;
	}
	if (keys[GLFW_KEY_Y])
	{
		dogPosZ -= 0.01f;
	}
	if (keys[GLFW_KEY_G])
	{
		dogPosX -= 0.01f;
	}
	if (keys[GLFW_KEY_J])
	{
		dogPosX += 0.01f;
	}
	if (keys[GLFW_KEY_I])
	{
		dogPosY += 0.005f;
	}
	if (keys[GLFW_KEY_O])
	{
		dogPosY -= 0.005f;
	}

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

	// Control de posicion de luz puntual
	if (keys[GLFW_KEY_T])
	{
		pointLightPositions[0].x += 0.01f;
	}
	if (keys[GLFW_KEY_U])
	{
		pointLightPositions[0].z -= 0.01f;
	}
}

// Is called whenever a key is pressed/released via GLFW
void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	if (key == GLFW_KEY_L && action == GLFW_PRESS)
	{
		if (play == false && (FrameIndex > 1))
		{
			resetElements();
			// First Interpolation
			interpolation();

			play = true;
			playIndex = 0;
			i_curr_steps = 0;
		}
		else
		{
			play = false;
		}
	}

	if (key == GLFW_KEY_K && action == GLFW_PRESS)
	{
		if (!grabacionManualActiva)
		{
			FrameIndex = 0;
			grabacionManualActiva = true;
		}
		if (FrameIndex < MAX_FRAMES)
		{
			saveFrame();
		}
	}

// Tecla 5: MUERTE del perro. Carga la secuencia de muerte y la reproduce.
// Si el perro ya esta muerto, la misma tecla 5 lo revive (vuelve a la
// secuencia de sentado/saludo en postura base).
	if (key == GLFW_KEY_5 && action == GLFW_PRESS)
	{
		if (!perroMuerto && !play)
		{
			cargarSecuenciaMuerte();
			resetElements();
			interpolation();
			play = true;
			playIndex = 0;
			i_curr_steps = 0;
			printf("El perro se esta muriendo...\n");
		}
		else if (perroMuerto)
		{
			cargarSecuenciaSentadoSaludo();
			printf("El perro revivio.\n");
		}
	}

	// Tecla P: guarda en "keyframes.txt" la animacion grabada con K
	if (key == GLFW_KEY_P && action == GLFW_PRESS)
	{
		guardarKeyFramesTXT("keyframes.txt");
	}

	// Tecla F: carga "keyframes.txt" y reproduce esa animacion
	if (key == GLFW_KEY_F && action == GLFW_PRESS)
	{
		if (cargarKeyFramesTXT("keyframes.txt"))
		{
			resetElements();
			interpolation();
			play = true;
			playIndex = 0;
			i_curr_steps = 0;
		}
	}

	// Tecla M (o R): Vuelve a cargar la secuencia donde el perro se sienta y saluda con la pata derecha
	if ((key == GLFW_KEY_M || key == GLFW_KEY_R) && action == GLFW_PRESS)
	{
		cargarSecuenciaSentadoSaludo();
	}

	// Tecla 0: Reinicia todos los cuadros a la pose original
	if (key == GLFW_KEY_0 && action == GLFW_PRESS)
	{
		play = false;
		perroMuerto = false;
		animacionMuerte = false;
		FrameIndex = 0;
		grabacionManualActiva = true;
		dogPosX = 0.0f; dogPosY = 0.0f; dogPosZ = 0.0f;
		rotDog = 0.0f; inclinacionCuerpo = 0.0f;
		head = 0.0f; tail = 0.0f;
		pataDelIzq = 0.0f; pataDelDer = 0.0f;
		pataTrasIzq = 0.0f; pataTrasDer = 0.0f;
		printf("KeyFrames reiniciados a cero.\n");
	}

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
		active = !active;
		if (active)
		{
			Light1 = glm::vec3(0.2f, 0.8f, 1.0f);
		}
		else
		{
			Light1 = glm::vec3(0);
		}
	}
}

void Animation()
{
	if (play)
	{
		if (i_curr_steps >= i_max_steps) // end of animation between frames?
		{
			playIndex++;
			if (playIndex > FrameIndex - 2) // end of total animation?
			{
				printf("termina anim\n");
				if (animacionMuerte)
				{
					perroMuerto = true;
					printf("El perro ha muerto. Pulsa 5 para revivirlo.\n");
				}
				playIndex = 0;
				play = false;
			}
			else // Next frame interpolations
			{
				i_curr_steps = 0; // Reset counter
				// Interpolation
				interpolation();
			}
		}
		else
		{
			// Draw animation
			dogPosX += KeyFrame[playIndex].incX;
			dogPosY += KeyFrame[playIndex].incY;
			dogPosZ += KeyFrame[playIndex].incZ;

			rotDog += KeyFrame[playIndex].rotDogInc;
			inclinacionCuerpo += KeyFrame[playIndex].inclinacionCuerpoInc;
			head += KeyFrame[playIndex].headInc;
			tail += KeyFrame[playIndex].tailInc;
			pataDelIzq += KeyFrame[playIndex].pataDelIzqInc;
			pataDelDer += KeyFrame[playIndex].pataDelDerInc;
			pataTrasIzq += KeyFrame[playIndex].pataTrasIzqInc;
			pataTrasDer += KeyFrame[playIndex].pataTrasDerInc;

			i_curr_steps++;
		}
	}
}

void MouseCallback(GLFWwindow* window, double xPos, double yPos)
{
	if (firstMouse)
	{
		lastX = xPos;
		lastY = yPos;
		firstMouse = false;
	}

	GLfloat xOffset = xPos - lastX;
	GLfloat yOffset = lastY - yPos;

	lastX = xPos;
	lastY = yPos;

	camera.ProcessMouseMovement(xOffset, yOffset);
}