//Practica 3// 
// Rea Alberto Cristian // 
// numero de cuenta : 318273130 //
//Fecha de entrega 6 de septiembre del 2026//


#include<iostream>
#include <vector>

//#define GLEW_STATIC

#include <GL/glew.h>

#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>



// Sombreadores
#include "Shader.h"

const GLint WIDTH = 800, HEIGHT = 600;

// Factor de escala base del cubo
const float CUBE_SCALE = 300.0f;

// Posiciones (x,y,z) de un cubo unitario de -0.5 a 0.5, 36 vertices (6 caras x 2 triangulos x 3 vertices).
// Es el mismo cubo de siempre, pero sin el color "pegado": el color se agrega aparte para
// poder pintar cada cubo de un solo color solido (como en el diseño de referencia).
static const GLfloat cubePositions[] = {
	-0.5f, -0.5f,  0.5f, //Frente
	 0.5f, -0.5f,  0.5f,
	 0.5f,  0.5f,  0.5f,
	 0.5f,  0.5f,  0.5f,
	-0.5f,  0.5f,  0.5f,
	-0.5f, -0.5f,  0.5f,

	-0.5f, -0.5f, -0.5f, //Atras
	 0.5f, -0.5f, -0.5f,
	 0.5f,  0.5f, -0.5f,
	 0.5f,  0.5f, -0.5f,
	-0.5f,  0.5f, -0.5f,
	-0.5f, -0.5f, -0.5f,

	 0.5f, -0.5f,  0.5f, //Derecha
	 0.5f, -0.5f, -0.5f,
	 0.5f,  0.5f, -0.5f,
	 0.5f,  0.5f, -0.5f,
	 0.5f,  0.5f,  0.5f,
	 0.5f, -0.5f,  0.5f,

	-0.5f,  0.5f,  0.5f, //Izquierda
	-0.5f,  0.5f, -0.5f,
	-0.5f, -0.5f, -0.5f,
	-0.5f, -0.5f, -0.5f,
	-0.5f, -0.5f,  0.5f,
	-0.5f,  0.5f,  0.5f,

	-0.5f, -0.5f, -0.5f, //Abajo
	 0.5f, -0.5f, -0.5f,
	 0.5f, -0.5f,  0.5f,
	 0.5f, -0.5f,  0.5f,
	-0.5f, -0.5f,  0.5f,
	-0.5f, -0.5f, -0.5f,

	-0.5f,  0.5f, -0.5f, //Arriba
	 0.5f,  0.5f, -0.5f,
	 0.5f,  0.5f,  0.5f,
	 0.5f,  0.5f,  0.5f,
	-0.5f,  0.5f,  0.5f,
	-0.5f,  0.5f, -0.5f,
};

const int CUBE_VERTEX_COUNT = 36;

// Arma el arreglo de vertices (posicion + color) para un cubo pintado TODO del mismo color.
// La cara superior e inferior se aclaran/oscurecen un poco para simular la sombra/luz
// de la imagen de referencia (igual que se ve mas clara la tapa y mas oscuro el costado).
std::vector<GLfloat> BuildSolidColorCube(const glm::vec3& color)
{
	std::vector<GLfloat> data;
	data.reserve(CUBE_VERTEX_COUNT * 6);

	glm::vec3 topColor = glm::clamp(color * 1.30f, 0.0f, 1.0f);   // tapa bien mas clara (le pega la luz de arriba)
	glm::vec3 sideColor = color;                                   // frente/costado derecho: color base
	glm::vec3 shadeColor = glm::clamp(color * 0.70f, 0.0f, 1.0f); // costado izquierdo/atras bien mas oscuro (en sombra)

	for (int face = 0; face < 6; ++face)
	{
		glm::vec3 faceColor;
		if (face == 5) faceColor = topColor;              // Arriba
		else if (face == 3 || face == 1) faceColor = shadeColor; // Izquierda / Atras
		else faceColor = sideColor;                        // Frente / Derecha / Abajo

		for (int v = 0; v < 6; ++v)
		{
			int idx = (face * 6 + v) * 3;
			data.push_back(cubePositions[idx + 0]);
			data.push_back(cubePositions[idx + 1]);
			data.push_back(cubePositions[idx + 2]);
			data.push_back(faceColor.r);
			data.push_back(faceColor.g);
			data.push_back(faceColor.b);
		}
	}
	return data;
}

// Datos de cada uno de los 8 cubos: color, posicion (en fraccion del ancho/alto de pantalla),
// escala (en fraccion de CUBE_SCALE), profundidad Z y rotacion.
// La rotacion se da en dos pasos, como en un dibujo tecnico de un cubo:
//   yaw   = giro sobre el eje Y (vertical) -> revela la cara lateral (derecha)
//   pitch = giro sobre el eje X (horizontal) -> revela la cara de arriba (tapa)
// Con yaw ~30-45 grados y pitch ~18-25 grados siempre se alcanzan a ver las 3 caras
// (frente, lado y tapa), que es lo que hace que se note claramente que es un cubo 3D.
struct CuboDatos {
	glm::vec3 color;
	float xFrac, yFrac;
	float scaleFrac;
	float z;
	float yaw;
	float pitch;
};

int main() {
	glfwInit();
	//Verificación de compatibilidad
	// Establece todas las opciones necesarias para GLFW
	/*glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);*/

	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

	GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "practica 3 -Cristian Rea Alberto - 318273130", nullptr, nullptr);

	int screenWidth, screenHeight;

	glfwGetFramebufferSize(window, &screenWidth, &screenHeight);

	//Verificación de errores de creación de ventana
	if (nullptr == window)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();

		return EXIT_FAILURE;
	}

	glfwMakeContextCurrent(window);
	glewExperimental = GL_TRUE;

	//Verificación de errores de inicialización de glew

	if (GLEW_OK != glewInit()) {
		std::cout << "Failed to initialise GLEW" << std::endl;
		return EXIT_FAILURE;
	}


	// Define las dimensiones del viewport
	glViewport(0, 0, screenWidth, screenHeight);


	// Configura las opciones de OpenGL
	glEnable(GL_DEPTH_TEST);

	// Habilita el soporte de canal alfa (transparencia)
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


	// Compila y construye nuestro programa de sombreado
	Shader ourShader("Shader/core.vs", "Shader/core.frag");


	// ---------------------------------------------------------------------
	// Definición de los 8 cubos: colores pastel igual que la imagen de
	// referencia (6 cubos chicos "flotando" arriba + 2 cubos grandes abajo).
	// ---------------------------------------------------------------------
	std::vector<CuboDatos> cubos = {
		// ---- Grupo flotante (6 cubos chicos, arriba, centrados en el hueco entre los 2 grandes) ----
		// Durazno (el mas chico, hasta arriba)
		{ glm::vec3(0.95f, 0.76f, 0.62f), 0.500f, 0.90f, 0.15f, -650.0f,  30.0f, 20.0f },
		// Naranja/mostaza
		{ glm::vec3(0.91f, 0.65f, 0.16f), 0.363f, 0.78f, 0.22f, -650.0f, -35.0f, 22.0f },
		// Rojo/coral
		{ glm::vec3(0.93f, 0.42f, 0.38f), 0.638f, 0.78f, 0.22f, -650.0f,  38.0f, 18.0f },
		// Verde
		{ glm::vec3(0.56f, 0.78f, 0.45f), 0.500f, 0.67f, 0.28f, -680.0f,  32.0f, 24.0f },
		// Morado/lavanda (se asoma un poco encima del cubo menta grande de abajo-izquierda)
		{ glm::vec3(0.78f, 0.70f, 0.87f), 0.375f, 0.53f, 0.35f, -700.0f, -40.0f, 20.0f },
		// Menta mediano (se asoma un poco encima del cubo azul grande de abajo-derecha)
		{ glm::vec3(0.62f, 0.83f, 0.73f), 0.650f, 0.53f, 0.35f, -700.0f,  42.0f, 22.0f },

		// ---- Los 2 cubos grandes, abajo, completos (sin cortarse con el borde inferior) ----
		// Menta grande (abajo, izquierda)
		{ glm::vec3(0.62f, 0.83f, 0.73f), 0.20f, 0.26f, 0.68f, -950.0f,  35.0f, 20.0f },
		// Azul grande (abajo, derecha)
		{ glm::vec3(0.63f, 0.75f, 0.87f), 0.80f, 0.26f, 0.68f, -950.0f, -30.0f, 20.0f },
	};

	const int NUM_CUBOS = (int)cubos.size();

	// Un VAO/VBO por cubo, cada uno con su propio color solido ya "horneado" en los vertices.
	std::vector<GLuint> VAOs(NUM_CUBOS), VBOs(NUM_CUBOS);

	for (int i = 0; i < NUM_CUBOS; ++i)
	{
		std::vector<GLfloat> vertexData = BuildSolidColorCube(cubos[i].color);

		glGenVertexArrays(1, &VAOs[i]);
		glGenBuffers(1, &VBOs[i]);

		glBindVertexArray(VAOs[i]);

		glBindBuffer(GL_ARRAY_BUFFER, VBOs[i]);
		glBufferData(GL_ARRAY_BUFFER, vertexData.size() * sizeof(GLfloat), vertexData.data(), GL_STATIC_DRAW);

		//Posición
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)0);
		glEnableVertexAttribArray(0);

		//Color
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
		glEnableVertexAttribArray(1);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
	}


	glm::mat4 projection = glm::mat4(1);

	//projection = glm::perspective(45.0f, (GLfloat)screenWidth / (GLfloat)screenHeight, 0.1f, 100.0f);//FOV, Radio de aspecto,znear,zfar
	projection = glm::ortho(0.0f, (GLfloat)screenWidth, 0.0f, (GLfloat)screenHeight, 0.1f, 1500.0f);//Izq,Der,Fondo,Alto,Cercania,Lejania

	while (!glfwWindowShouldClose(window))
	{
		// Revisa si se activo algun evento (tecla presionada, mouse movido, etc.) y llama a las funciones de respuesta correspondientes
		glfwPollEvents();

		// Renderizado
		// Limpia el buffer de color
		glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		ourShader.Use();

		GLint modelLoc = glGetUniformLocation(ourShader.Program, "model");
		GLint viewLoc = glGetUniformLocation(ourShader.Program, "view");
		GLint projecLoc = glGetUniformLocation(ourShader.Program, "projection");

		glUniformMatrix4fv(projecLoc, 1, GL_FALSE, glm::value_ptr(projection));

		glm::mat4 view = glm::mat4(1);
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

		for (int i = 0; i < NUM_CUBOS; ++i)
		{
			const CuboDatos& c = cubos[i];

			float x = screenWidth * c.xFrac;
			float y = screenHeight * c.yFrac;
			float scale = CUBE_SCALE * c.scaleFrac;

			glm::mat4 model = glm::mat4(1);
			model = glm::translate(model, glm::vec3(x, y, c.z));
			model = glm::rotate(model, glm::radians(c.yaw), glm::vec3(0.0f, 1.0f, 0.0f));   // gira para mostrar la cara lateral
			model = glm::rotate(model, glm::radians(c.pitch), glm::vec3(1.0f, 0.0f, 0.0f)); // inclina para mostrar la tapa
			model = glm::scale(model, glm::vec3(scale, scale, scale));

			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

			glBindVertexArray(VAOs[i]);
			glDrawArrays(GL_TRIANGLES, 0, CUBE_VERTEX_COUNT);
			glBindVertexArray(0);
		}

		// Intercambia los buffers de pantalla
		glfwSwapBuffers(window);

	}

	for (int i = 0; i < NUM_CUBOS; ++i)
	{
		glDeleteVertexArrays(1, &VAOs[i]);
		glDeleteBuffers(1, &VBOs[i]);
	}


	glfwTerminate();
	return EXIT_SUCCESS;

}