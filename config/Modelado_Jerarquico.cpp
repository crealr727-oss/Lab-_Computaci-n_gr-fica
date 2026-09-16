//Previo 5 // 
// Rea Alberto Cristian // 
// numero de cuenta : 318273130 //
//Fecha de entrega 15 de septiembre del 2026//

#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// Shaders
#include "Shader.h"

void Inputs(GLFWwindow* window);

const GLint WIDTH = 1200, HEIGHT = 800;

// ============================================================
// VARIABLES PARA TECLADO
// ============================================================

float movX = 0.0f,
movY = 0.0f,
movZ = -8.0f,
rot = 0.0f;

// ============================================================
// VARIABLES DEL MODELO JERÁRQUICO
// ============================================================

float hombro = 0.0f;
float codo = 0.0f;
float muneca = 0.0f;
float dedos = 0.0f;


// ============================================================
// MAIN
// ============================================================

int main()
{
    // --------------------------------------------------------
    // INICIALIZAR GLFW
    // --------------------------------------------------------

    glfwInit();

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

    GLFWwindow* window = glfwCreateWindow(
        WIDTH,
        HEIGHT,
        "previo 5 - Rea Alberto Cristian - 318273130",
        nullptr,
        nullptr
    );

    int screenWidth, screenHeight;

    glfwGetFramebufferSize(
        window,
        &screenWidth,
        &screenHeight
    );

    // --------------------------------------------------------
    // VERIFICAR VENTANA
    // --------------------------------------------------------

    if (nullptr == window)
    {
        std::cout << "Failed to create GLFW window"
            << std::endl;

        glfwTerminate();

        return EXIT_FAILURE;
    }

    glfwMakeContextCurrent(window);

    // --------------------------------------------------------
    // INICIALIZAR GLEW
    // --------------------------------------------------------

    glewExperimental = GL_TRUE;

    if (GLEW_OK != glewInit())
    {
        std::cout << "Failed to initialise GLEW"
            << std::endl;

        return EXIT_FAILURE;
    }

    // --------------------------------------------------------
    // VIEWPORT
    // --------------------------------------------------------

    glViewport(
        0,
        0,
        screenWidth,
        screenHeight
    );

    // --------------------------------------------------------
    // CONFIGURACIÓN OPENGL
    // --------------------------------------------------------

    glEnable(GL_DEPTH_TEST);

    glEnable(GL_BLEND);

    glBlendFunc(
        GL_SRC_ALPHA,
        GL_ONE_MINUS_SRC_ALPHA
    );

    // --------------------------------------------------------
    // SHADER
    // --------------------------------------------------------

    Shader ourShader(
        "Shader/core.vs",
        "Shader/core.frag"
    );


    // ========================================================
    // VÉRTICES DEL CUBO
    // ========================================================

    float vertices[] =
    {
        // ----------------------------------------------------
        // CARA FRONTAL
        // ----------------------------------------------------

        -0.5f, -0.5f,  0.5f,
         0.5f, -0.5f,  0.5f,
         0.5f,  0.5f,  0.5f,

         0.5f,  0.5f,  0.5f,
        -0.5f,  0.5f,  0.5f,
        -0.5f, -0.5f,  0.5f,

        // ----------------------------------------------------
        // CARA TRASERA
        // ----------------------------------------------------

        -0.5f, -0.5f, -0.5f,
         0.5f, -0.5f, -0.5f,
         0.5f,  0.5f, -0.5f,

         0.5f,  0.5f, -0.5f,
        -0.5f,  0.5f, -0.5f,
        -0.5f, -0.5f, -0.5f,

        // ----------------------------------------------------
        // CARA DERECHA
        // ----------------------------------------------------

         0.5f, -0.5f,  0.5f,
         0.5f, -0.5f, -0.5f,
         0.5f,  0.5f, -0.5f,

         0.5f,  0.5f, -0.5f,
         0.5f,  0.5f,  0.5f,
         0.5f, -0.5f,  0.5f,

         // ----------------------------------------------------
         // CARA IZQUIERDA
         // ----------------------------------------------------

         -0.5f,  0.5f,  0.5f,
         -0.5f,  0.5f, -0.5f,
         -0.5f, -0.5f, -0.5f,

         -0.5f, -0.5f, -0.5f,
         -0.5f, -0.5f,  0.5f,
         -0.5f,  0.5f,  0.5f,

         // ----------------------------------------------------
         // CARA INFERIOR
         // ----------------------------------------------------

         -0.5f, -0.5f, -0.5f,
          0.5f, -0.5f, -0.5f,
          0.5f, -0.5f,  0.5f,

          0.5f, -0.5f,  0.5f,
         -0.5f, -0.5f,  0.5f,
         -0.5f, -0.5f, -0.5f,

         // ----------------------------------------------------
         // CARA SUPERIOR
         // ----------------------------------------------------

         -0.5f,  0.5f, -0.5f,
          0.5f,  0.5f, -0.5f,
          0.5f,  0.5f,  0.5f,

          0.5f,  0.5f,  0.5f,
         -0.5f,  0.5f,  0.5f,
         -0.5f,  0.5f, -0.5f
    };


    // ========================================================
    // VAO Y VBO
    // ========================================================

    GLuint VBO, VAO;

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);

    glBindBuffer(
        GL_ARRAY_BUFFER,
        VBO
    );

    glBufferData(
        GL_ARRAY_BUFFER,
        sizeof(vertices),
        vertices,
        GL_STATIC_DRAW
    );

    // --------------------------------------------------------
    // POSICIÓN
    // --------------------------------------------------------

    glVertexAttribPointer(
        0,
        3,
        GL_FLOAT,
        GL_FALSE,
        3 * sizeof(GLfloat),
        (GLvoid*)0
    );

    glEnableVertexAttribArray(0);

    glBindBuffer(
        GL_ARRAY_BUFFER,
        0
    );

    glBindVertexArray(0);


    // ========================================================
    // PROYECCIÓN
    // ========================================================

    glm::mat4 projection = glm::mat4(1.0f);

    projection = glm::perspective(
        glm::radians(45.0f),
        (GLfloat)screenWidth /
        (GLfloat)screenHeight,
        0.1f,
        100.0f
    );


    // ========================================================
    // LOOP PRINCIPAL
    // ========================================================

    while (!glfwWindowShouldClose(window))
    {
        // ----------------------------------------------------
        // ENTRADAS
        // ----------------------------------------------------

        Inputs(window);

        glfwPollEvents();


        // ----------------------------------------------------
        // LIMPIAR PANTALLA
        // ----------------------------------------------------

        glClearColor(
            0.0f,
            0.0f,
            0.0f,
            1.0f
        );

        glClear(
            GL_COLOR_BUFFER_BIT |
            GL_DEPTH_BUFFER_BIT
        );


        // ----------------------------------------------------
        // ACTIVAR SHADER
        // ----------------------------------------------------

        ourShader.Use();


        // ====================================================
        // MATRICES
        // ====================================================

        glm::mat4 model = glm::mat4(1.0f);

        glm::mat4 view = glm::mat4(1.0f);

        // Checkpoints
        glm::mat4 modelTemp = glm::mat4(1.0f);
        glm::mat4 modelTemp2 = glm::mat4(1.0f);


        // ====================================================
        // CÁMARA
        // ====================================================

        view = glm::translate(
            view,
            glm::vec3(
                movX,
                movY,
                movZ
            )
        );

        view = glm::rotate(
            view,
            glm::radians(rot),
            glm::vec3(
                0.0f,
                1.0f,
                0.0f
            )
        );


        // ====================================================
        // UNIFORMS
        // ====================================================

        GLint modelLoc =
            glGetUniformLocation(
                ourShader.Program,
                "model"
            );

        GLint viewLoc =
            glGetUniformLocation(
                ourShader.Program,
                "view"
            );

        GLint projectionLoc =
            glGetUniformLocation(
                ourShader.Program,
                "projection"
            );

        GLint uniformColor =
            ourShader.uniformColor;


        // ----------------------------------------------------
        // ENVIAR MATRICES
        // ----------------------------------------------------

        glUniformMatrix4fv(
            projectionLoc,
            1,
            GL_FALSE,
            glm::value_ptr(projection)
        );

        glUniformMatrix4fv(
            viewLoc,
            1,
            GL_FALSE,
            glm::value_ptr(view)
        );


        // ====================================================
        // ACTIVAR VAO
        // ====================================================

        glBindVertexArray(VAO);


        // ====================================================
        // 1. HOMBRO / BRAZO SUPERIOR
        // ====================================================

        /*
            El hombro es el PADRE.

            Todo lo que esté debajo de esta transformación
            heredará su rotación.
        */

        model = glm::mat4(1.0f);


        // ----------------------------------------------------
        // Posición inicial del brazo
        // ----------------------------------------------------

        model = glm::translate(
            model,
            glm::vec3(
                -3.0f,
                0.0f,
                0.0f
            )
        );


        // ----------------------------------------------------
        // ROTACIÓN DEL HOMBRO
        // ----------------------------------------------------

        model = glm::rotate(
            model,
            glm::radians(hombro),
            glm::vec3(
                0.0f,
                0.0f,
                1.0f
            )
        );


        // ----------------------------------------------------
        // CHECKPOINT DEL HOMBRO
        // ----------------------------------------------------

        modelTemp = model;


        // ----------------------------------------------------
        // MOVER AL CENTRO DEL CUBO
        // ----------------------------------------------------

        model = glm::translate(
            model,
            glm::vec3(
                1.5f,
                0.0f,
                0.0f
            )
        );


        // ----------------------------------------------------
        // ESCALA BRAZO SUPERIOR
        // ----------------------------------------------------

        model = glm::scale(
            model,
            glm::vec3(
                3.0f,
                1.0f,
                1.0f
            )
        );


        // ----------------------------------------------------
        // COLOR VERDE
        // ----------------------------------------------------

        glm::vec3 color =
            glm::vec3(
                0.0f,
                1.0f,
                0.0f
            );

        glUniform3fv(
            uniformColor,
            1,
            glm::value_ptr(color)
        );


        // ----------------------------------------------------
        // ENVIAR MODEL
        // ----------------------------------------------------

        glUniformMatrix4fv(
            modelLoc,
            1,
            GL_FALSE,
            glm::value_ptr(model)
        );


        // ----------------------------------------------------
        // DIBUJAR BRAZO
        // ----------------------------------------------------

        glDrawArrays(
            GL_TRIANGLES,
            0,
            36
        );


        // ====================================================
        // 2. CODO / ANTEBRAZO
        // ====================================================

        /*
            Regresamos al checkpoint del hombro.

            Esto es importante porque NO queremos que la
            escala del brazo superior afecte al antebrazo.
        */

        model = modelTemp;


        // ----------------------------------------------------
        // IR AL CODO
        // ----------------------------------------------------

        model = glm::translate(
            model,
            glm::vec3(
                3.0f,
                0.0f,
                0.0f
            )
        );


        // ----------------------------------------------------
        // CHECKPOINT DEL CODO
        // ----------------------------------------------------

        modelTemp2 = model;


        // ----------------------------------------------------
        // ROTAR CODO
        // ----------------------------------------------------

        model = glm::rotate(
            model,
            glm::radians(codo),
            glm::vec3(
                0.0f,
                0.0f,
                1.0f
            )
        );


        // ----------------------------------------------------
        // CENTRAR ANTEBRAZO
        // ----------------------------------------------------

        model = glm::translate(
            model,
            glm::vec3(
                1.25f,
                0.0f,
                0.0f
            )
        );


        // ----------------------------------------------------
        // ESCALA ANTEBRAZO
        // ----------------------------------------------------

        model = glm::scale(
            model,
            glm::vec3(
                2.5f,
                0.8f,
                0.8f
            )
        );


        // ----------------------------------------------------
        // COLOR ROJO
        // ----------------------------------------------------

        color =
            glm::vec3(
                1.0f,
                0.0f,
                0.0f
            );

        glUniform3fv(
            uniformColor,
            1,
            glm::value_ptr(color)
        );


        // ----------------------------------------------------
        // ENVIAR MODEL
        // ----------------------------------------------------

        glUniformMatrix4fv(
            modelLoc,
            1,
            GL_FALSE,
            glm::value_ptr(model)
        );


        // ----------------------------------------------------
        // DIBUJAR ANTEBRAZO
        // ----------------------------------------------------

        glDrawArrays(
            GL_TRIANGLES,
            0,
            36
        );


        // ====================================================
        // 3. MUÑECA / PALMA
        // ====================================================

        /*
            Recuperamos el checkpoint del codo.

            De esta forma la palma hereda:

                HOMBRO
                   +
                CODO
                   +
                MUÑECA
        */

        model = modelTemp2;


        // ----------------------------------------------------
        // IR A LA MUÑECA
        // ----------------------------------------------------

        model = glm::translate(
            model,
            glm::vec3(
                2.5f,
                0.0f,
                0.0f
            )
        );


        // ----------------------------------------------------
        // ROTACIÓN DE MUÑECA
        // ----------------------------------------------------

        model = glm::rotate(
            model,
            glm::radians(muneca),
            glm::vec3(
                0.0f,
                0.0f,
                1.0f
            )
        );


        // ----------------------------------------------------
        // CHECKPOINT DE LA MUÑECA
        // ----------------------------------------------------

        modelTemp = model;


        // ----------------------------------------------------
        // CENTRO DE LA PALMA
        // ----------------------------------------------------

        model = glm::translate(
            model,
            glm::vec3(
                0.6f,
                0.0f,
                0.0f
            )
        );


        // ----------------------------------------------------
        // ESCALA PALMA
        // ----------------------------------------------------

        model = glm::scale(
            model,
            glm::vec3(
                1.2f,
                1.2f,
                0.7f
            )
        );


        // ----------------------------------------------------
        // COLOR BLANCO
        // ----------------------------------------------------

        color =
            glm::vec3(
                1.0f,
                1.0f,
                1.0f
            );

        glUniform3fv(
            uniformColor,
            1,
            glm::value_ptr(color)
        );


        // ----------------------------------------------------
        // ENVIAR MODEL
        // ----------------------------------------------------

        glUniformMatrix4fv(
            modelLoc,
            1,
            GL_FALSE,
            glm::value_ptr(model)
        );


        // ----------------------------------------------------
        // DIBUJAR PALMA
        // ----------------------------------------------------

        glDrawArrays(
            GL_TRIANGLES,
            0,
            36
        );


        // ====================================================
        // 4. DEDO SUPERIOR
        // ====================================================

        /*
            Los dedos son HIJOS de la muñeca.

            Por eso recuperamos modelTemp.
        */

        model = modelTemp;


        // ----------------------------------------------------
        // POSICIÓN DEL DEDO
        // ----------------------------------------------------

        model = glm::translate(
            model,
            glm::vec3(
                0.9f,
                0.65f,
                0.0f
            )
        );


        // ----------------------------------------------------
        // ROTACIÓN DE LOS DEDOS
        // ----------------------------------------------------

        model = glm::rotate(
            model,
            glm::radians(dedos),
            glm::vec3(
                0.0f,
                0.0f,
                1.0f
            )
        );


        // ----------------------------------------------------
        // CENTRAR CUBO
        // ----------------------------------------------------

        model = glm::translate(
            model,
            glm::vec3(
                0.35f,
                0.0f,
                0.0f
            )
        );


        // ----------------------------------------------------
        // ESCALA
        // ----------------------------------------------------

        model = glm::scale(
            model,
            glm::vec3(
                0.7f,
                0.22f,
                0.22f
            )
        );


        // ----------------------------------------------------
        // COLOR CYAN
        // ----------------------------------------------------

        color =
            glm::vec3(
                0.0f,
                1.0f,
                1.0f
            );

        glUniform3fv(
            uniformColor,
            1,
            glm::value_ptr(color)
        );


        glUniformMatrix4fv(
            modelLoc,
            1,
            GL_FALSE,
            glm::value_ptr(model)
        );


        glDrawArrays(
            GL_TRIANGLES,
            0,
            36
        );


        // ====================================================
        // 5. DEDO CENTRAL
        // ====================================================

        // Regresar a la muñeca
        model = modelTemp;


        model = glm::translate(
            model,
            glm::vec3(
                1.0f,
                0.0f,
                0.0f
            )
        );


        model = glm::rotate(
            model,
            glm::radians(dedos),
            glm::vec3(
                0.0f,
                0.0f,
                1.0f
            )
        );


        model = glm::translate(
            model,
            glm::vec3(
                0.35f,
                0.0f,
                0.0f
            )
        );


        model = glm::scale(
            model,
            glm::vec3(
                0.8f,
                0.22f,
                0.22f
            )
        );


        // ----------------------------------------------------
        // COLOR MAGENTA
        // ----------------------------------------------------

        color =
            glm::vec3(
                1.0f,
                0.0f,
                1.0f
            );

        glUniform3fv(
            uniformColor,
            1,
            glm::value_ptr(color)
        );


        glUniformMatrix4fv(
            modelLoc,
            1,
            GL_FALSE,
            glm::value_ptr(model)
        );


        glDrawArrays(
            GL_TRIANGLES,
            0,
            36
        );


        // ====================================================
        // 6. DEDO INFERIOR
        // ====================================================

        model = modelTemp;


        model = glm::translate(
            model,
            glm::vec3(
                0.9f,
                -0.65f,
                0.0f
            )
        );


        model = glm::rotate(
            model,
            glm::radians(-dedos),
            glm::vec3(
                0.0f,
                0.0f,
                1.0f
            )
        );


        model = glm::translate(
            model,
            glm::vec3(
                0.35f,
                0.0f,
                0.0f
            )
        );


        model = glm::scale(
            model,
            glm::vec3(
                0.7f,
                0.22f,
                0.22f
            )
        );


        // ----------------------------------------------------
        // COLOR MAGENTA
        // ----------------------------------------------------

        color =
            glm::vec3(
                1.0f,
                0.0f,
                1.0f
            );

        glUniform3fv(
            uniformColor,
            1,
            glm::value_ptr(color)
        );


        glUniformMatrix4fv(
            modelLoc,
            1,
            GL_FALSE,
            glm::value_ptr(model)
        );


        glDrawArrays(
            GL_TRIANGLES,
            0,
            36
        );


        // ====================================================
        // FINAL
        // ====================================================

        glBindVertexArray(0);

        glfwSwapBuffers(window);
    }


    // ========================================================
    // LIBERAR MEMORIA
    // ========================================================

    glDeleteVertexArrays(
        1,
        &VAO
    );

    glDeleteBuffers(
        1,
        &VBO
    );

    glfwTerminate();

    return EXIT_SUCCESS;
}


// ============================================================
// INPUTS
// ============================================================

void Inputs(GLFWwindow* window)
{
    // ========================================================
    // ESC
    // ========================================================

    if (glfwGetKey(
        window,
        GLFW_KEY_ESCAPE
    ) == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(
            window,
            true
        );
    }


    // ========================================================
    // MOVIMIENTO DE CÁMARA
    // ========================================================

    if (glfwGetKey(
        window,
        GLFW_KEY_D
    ) == GLFW_PRESS)
    {
        movX += 0.08f;
    }

    if (glfwGetKey(
        window,
        GLFW_KEY_A
    ) == GLFW_PRESS)
    {
        movX -= 0.08f;
    }

    if (glfwGetKey(
        window,
        GLFW_KEY_UP
    ) == GLFW_PRESS)
    {
        movY += 0.08f;
    }

    if (glfwGetKey(
        window,
        GLFW_KEY_DOWN
    ) == GLFW_PRESS)
    {
        movY -= 0.08f;
    }

    if (glfwGetKey(
        window,
        GLFW_KEY_W
    ) == GLFW_PRESS)
    {
        movZ -= 0.08f;
    }

    if (glfwGetKey(
        window,
        GLFW_KEY_S
    ) == GLFW_PRESS)
    {
        movZ += 0.08f;
    }

    if (glfwGetKey(
        window,
        GLFW_KEY_RIGHT
    ) == GLFW_PRESS)
    {
        rot += 0.18f;
    }

    if (glfwGetKey(
        window,
        GLFW_KEY_LEFT
    ) == GLFW_PRESS)
    {
        rot -= 0.18f;
    }


    // ========================================================
    // HOMBRO
    // R = aumentar
    // F = disminuir
    // ========================================================

    if (glfwGetKey(
        window,
        GLFW_KEY_R
    ) == GLFW_PRESS)
    {
        hombro += 0.5f;
    }

    if (glfwGetKey(
        window,
        GLFW_KEY_F
    ) == GLFW_PRESS)
    {
        hombro -= 0.5f;
    }


    // ========================================================
    // CODO
    // T = aumentar
    // G = disminuir
    // ========================================================

    if (glfwGetKey(
        window,
        GLFW_KEY_T
    ) == GLFW_PRESS)
    {
        codo += 0.5f;
    }

    if (glfwGetKey(
        window,
        GLFW_KEY_G
    ) == GLFW_PRESS)
    {
        codo -= 0.5f;
    }


    // ========================================================
    // MUÑECA
    // Y = aumentar
    // H = disminuir
    // ========================================================

    if (glfwGetKey(
        window,
        GLFW_KEY_Y
    ) == GLFW_PRESS)
    {
        muneca += 0.5f;
    }

    if (glfwGetKey(
        window,
        GLFW_KEY_H
    ) == GLFW_PRESS)
    {
        muneca -= 0.5f;
    }


    // ========================================================
    // DEDOS
    // U = abrir
    // J = cerrar
    // ========================================================

    if (glfwGetKey(
        window,
        GLFW_KEY_U
    ) == GLFW_PRESS)
    {
        dedos += 0.5f;
    }

    if (glfwGetKey(
        window,
        GLFW_KEY_J
    ) == GLFW_PRESS)
    {
        dedos -= 0.5f;
    }
}