//Practica 4// 
// Rea Alberto Cristian // 
// numero de cuenta : 318273130 //
//Fecha de entrega 13 de septiembre del 2026//

#include <GLFW/glfw3.h>

// ============================================================
// VENTANA
// ============================================================

const int ANCHO = 900;
const int ALTO = 700;

// ============================================================
// COLORES
// ============================================================

void cafe()
{
    glColor3f(0.38f, 0.25f, 0.15f);
}

void cafeClaro()
{
    glColor3f(0.55f, 0.38f, 0.23f);
}

void cafeMedio()
{
    glColor3f(0.45f, 0.30f, 0.18f);
}

void cafeOscuro()
{
    glColor3f(0.20f, 0.12f, 0.07f);
}

void negro()
{
    glColor3f(0.025f, 0.020f, 0.015f);
}

void blanco()
{
    glColor3f(0.95f, 0.95f, 0.90f);
}

// ============================================================
// CUBO
// ============================================================

void cubo(
    float x, float y, float z,
    float ancho, float alto, float profundidad
)
{
    glPushMatrix();

    glTranslatef(x, y, z);

    glScalef(
        ancho,
        alto,
        profundidad
    );

    glBegin(GL_QUADS);

    // ---------------- FRENTE ----------------

    glNormal3f(0, 0, 1);

    glVertex3f(-0.5f, -0.5f, 0.5f);
    glVertex3f(0.5f, -0.5f, 0.5f);
    glVertex3f(0.5f, 0.5f, 0.5f);
    glVertex3f(-0.5f, 0.5f, 0.5f);

    // ---------------- ATRÁS ----------------

    glNormal3f(0, 0, -1);

    glVertex3f(0.5f, -0.5f, -0.5f);
    glVertex3f(-0.5f, -0.5f, -0.5f);
    glVertex3f(-0.5f, 0.5f, -0.5f);
    glVertex3f(0.5f, 0.5f, -0.5f);

    // ---------------- IZQUIERDA ----------------

    glNormal3f(-1, 0, 0);

    glVertex3f(-0.5f, -0.5f, -0.5f);
    glVertex3f(-0.5f, -0.5f, 0.5f);
    glVertex3f(-0.5f, 0.5f, 0.5f);
    glVertex3f(-0.5f, 0.5f, -0.5f);

    // ---------------- DERECHA ----------------

    glNormal3f(1, 0, 0);

    glVertex3f(0.5f, -0.5f, 0.5f);
    glVertex3f(0.5f, -0.5f, -0.5f);
    glVertex3f(0.5f, 0.5f, -0.5f);
    glVertex3f(0.5f, 0.5f, 0.5f);

    // ---------------- ARRIBA ----------------

    glNormal3f(0, 1, 0);

    glVertex3f(-0.5f, 0.5f, 0.5f);
    glVertex3f(0.5f, 0.5f, 0.5f);
    glVertex3f(0.5f, 0.5f, -0.5f);
    glVertex3f(-0.5f, 0.5f, -0.5f);

    // ---------------- ABAJO ----------------

    glNormal3f(0, -1, 0);

    glVertex3f(-0.5f, -0.5f, -0.5f);
    glVertex3f(0.5f, -0.5f, -0.5f);
    glVertex3f(0.5f, -0.5f, 0.5f);
    glVertex3f(-0.5f, -0.5f, 0.5f);

    glEnd();

    glPopMatrix();
}

// ============================================================
// CUERPO
// ============================================================

void cuerpo()
{
    // Cuerpo principal
    cafe();

    cubo(
        0.6f,
        2.8f,
        0.0f,
        4.5f,
        2.4f,
        2.2f
    );

    // Parte superior del cuerpo
    cafeMedio();

    cubo(
        0.6f,
        4.02f,
        0.0f,
        4.3f,
        0.25f,
        2.05f
    );

    // Parte inferior
    cafeOscuro();

    cubo(
        0.6f,
        1.62f,
        0.0f,
        4.1f,
        0.20f,
        2.0f
    );
}

// ============================================================
// CUELLO
// ============================================================

void cuello()
{
    glPushMatrix();

    glTranslatef(
        -1.45f,
        3.55f,
        0.0f
    );

    // Inclinar hacia adelante
    glRotatef(
        -20.0f,
        0,
        0,
        1
    );

    cafeMedio();

    cubo(
        0,
        0,
        0,
        1.20f,
        2.70f,
        1.40f
    );

    glPopMatrix();
}

// ============================================================
// CABEZA
// ============================================================

void cabeza()
{
    glPushMatrix();

    glTranslatef(
        -2.20f,
        4.35f,
        0
    );

    glRotatef(
        -10.0f,
        0,
        0,
        1
    );

    // Cabeza
    cafe();

    cubo(
        0,
        0,
        0,
        1.55f,
        1.50f,
        1.35f
    );

    // Parte inferior de la cabeza
    cafeClaro();

    cubo(
        -0.25f,
        -0.25f,
        0,
        1.15f,
        0.90f,
        1.20f
    );

    glPopMatrix();
}

// ============================================================
// HOCICO
// ============================================================

void hocico()
{
    glPushMatrix();

    glTranslatef(
        -3.20f,
        4.00f,
        0
    );

    glRotatef(
        -5.0f,
        0,
        0,
        1
    );

    // Hocico
    cafeClaro();

    cubo(
        0,
        0,
        0,
        1.60f,
        0.90f,
        1.10f
    );

    // Punta
    cafeMedio();

    cubo(
        -0.72f,
        0,
        0,
        0.45f,
        0.78f,
        0.98f
    );

    // Nariz
    cafeOscuro();

    cubo(
        -0.97f,
        0,
        0,
        0.15f,
        0.55f,
        0.78f
    );

    glPopMatrix();
}

// ============================================================
// OJOS
// ============================================================

void ojo(float z)
{
    // Área del ojo
    cafeOscuro();

    cubo(
        -2.78f,
        4.65f,
        z,
        0.28f,
        0.30f,
        0.13f
    );

    // Pupila
    negro();

    cubo(
        -2.88f,
        4.65f,
        z,
        0.10f,
        0.15f,
        0.06f
    );

    // Brillo
    blanco();

    cubo(
        -2.93f,
        4.72f,
        z + 0.04f,
        0.035f,
        0.05f,
        0.025f
    );
}

void ojos()
{
    ojo(0.68f);
    ojo(-0.68f);
}

// ============================================================
// OREJAS
// ============================================================

void oreja(
    float z,
    float inclinacion
)
{
    glPushMatrix();

    glTranslatef(
        -1.95f,
        5.45f,
        z
    );

    glRotatef(
        inclinacion,
        0,
        0,
        1
    );

    // Oreja café
    cafe();

    cubo(
        0,
        0,
        0,
        0.48f,
        1.70f,
        0.52f
    );

    // Punta oscura
    cafeOscuro();

    cubo(
        0,
        0.65f,
        0,
        0.35f,
        0.42f,
        0.43f
    );

    // Interior
    negro();

    cubo(
        0,
        -0.05f,
        0.29f,
        0.22f,
        1.05f,
        0.05f
    );

    glPopMatrix();
}

void orejas()
{
    oreja(
        0.45f,
        -10.0f
    );

    oreja(
        -0.45f,
        10.0f
    );
}

// ============================================================
// PATAS
// ============================================================

void pata(
    float x,
    float z
)
{
    // Parte superior
    cafe();

    cubo(
        x,
        1.00f,
        z,
        0.65f,
        1.90f,
        0.65f
    );

    // Parte inferior
    cafeMedio();

    cubo(
        x,
        0.28f,
        z,
        0.70f,
        0.65f,
        0.70f
    );

    // Pezuña
    cafeOscuro();

    cubo(
        x,
        -0.12f,
        z,
        0.72f,
        0.25f,
        0.75f
    );
}

void patas()
{
    // Delanteras
    pata(
        -0.70f,
        0.70f
    );

    pata(
        -0.70f,
        -0.70f
    );

    // Traseras
    pata(
        2.00f,
        0.70f
    );

    pata(
        2.00f,
        -0.70f
    );
}

// ============================================================
// CRIN
// ============================================================

void crin()
{
    negro();

    // Crin detrás de la cabeza
    cubo(
        -1.55f,
        4.70f,
        0,
        0.20f,
        1.30f,
        1.45f
    );

    cubo(
        -1.38f,
        4.05f,
        0,
        0.18f,
        0.80f,
        1.40f
    );

    cubo(
        -1.20f,
        3.55f,
        0,
        0.18f,
        0.65f,
        1.25f
    );
}

// ============================================================
// COLA
// ============================================================

void cola()
{
    glPushMatrix();

    glTranslatef(
        2.95f,
        3.15f,
        0
    );

    glRotatef(
        -25.0f,
        0,
        0,
        1
    );

    cafe();

    cubo(
        0,
        0,
        0,
        0.40f,
        1.30f,
        0.40f
    );

    glPopMatrix();

    // Pelo de la cola
    negro();

    cubo(
        3.35f,
        2.45f,
        0,
        0.60f,
        0.80f,
        0.55f
    );

    cubo(
        3.55f,
        2.15f,
        0,
        0.45f,
        0.60f,
        0.50f
    );
}

// ============================================================
// BURRO
// ============================================================

void burro()
{
    cuerpo();

    patas();

    cuello();

    cabeza();

    hocico();

    orejas();

    ojos();

    crin();

    cola();
}

// ============================================================
// PISO
// ============================================================

void piso()
{
    glColor3f(
        0.72f,
        0.72f,
        0.72f
    );

    cubo(
        0,
        -0.45f,
        0,
        14.0f,
        0.15f,
        10.0f
    );
}

// ============================================================
// CÁMARA
// ============================================================

float rotacionY = -30.0f;
float rotacionX = 15.0f;

void camara()
{
    glMatrixMode(
        GL_PROJECTION
    );

    glLoadIdentity();

    float aspecto =
        (float)ANCHO /
        (float)ALTO;

    glFrustum(
        -aspecto,
        aspecto,
        -1.0f,
        1.0f,
        2.0f,
        100.0f
    );

    glMatrixMode(
        GL_MODELVIEW
    );

    glLoadIdentity();

    glTranslatef(
        0.0f,
        -2.3f,
        -14.0f
    );

    glRotatef(
        rotacionX,
        1,
        0,
        0
    );

    glRotatef(
        rotacionY,
        0,
        1,
        0
    );
}

// ============================================================
// ILUMINACIÓN
// ============================================================

void luces()
{
    glEnable(GL_LIGHTING);

    glEnable(GL_LIGHT0);

    // MUY IMPORTANTE:
    // Permite que glColor3f() funcione con iluminación.

    glEnable(GL_COLOR_MATERIAL);

    glColorMaterial(
        GL_FRONT_AND_BACK,
        GL_AMBIENT_AND_DIFFUSE
    );

    GLfloat posicion[] =
    {
        -5.0f,
        10.0f,
        8.0f,
        1.0f
    };

    GLfloat luz[] =
    {
        1.0f,
        1.0f,
        1.0f,
        1.0f
    };

    glLightfv(
        GL_LIGHT0,
        GL_POSITION,
        posicion
    );

    glLightfv(
        GL_LIGHT0,
        GL_DIFFUSE,
        luz
    );

    GLfloat ambiente[] =
    {
        0.35f,
        0.35f,
        0.35f,
        1.0f
    };

    glLightModelfv(
        GL_LIGHT_MODEL_AMBIENT,
        ambiente
    );
}

// ============================================================
// TECLADO
// ============================================================

void teclado(
    GLFWwindow* ventana,
    int tecla,
    int scancode,
    int accion,
    int mods
)
{
    if (
        accion == GLFW_PRESS ||
        accion == GLFW_REPEAT
        )
    {
        if (tecla == GLFW_KEY_LEFT)
            rotacionY -= 5.0f;

        if (tecla == GLFW_KEY_RIGHT)
            rotacionY += 5.0f;

        if (tecla == GLFW_KEY_UP)
            rotacionX -= 5.0f;

        if (tecla == GLFW_KEY_DOWN)
            rotacionX += 5.0f;

        if (tecla == GLFW_KEY_ESCAPE)
        {
            glfwSetWindowShouldClose(
                ventana,
                true
            );
        }
    }
}

// ============================================================
// MAIN
// ============================================================

int main()
{
    // Inicializar GLFW
    if (!glfwInit())
        return -1;

    GLFWwindow* ventana =
        glfwCreateWindow(
            ANCHO,
            ALTO,
            "Practica 4 - Rea Alberto Cristian - 318273130",
            nullptr,
            nullptr
        );

    if (!ventana)
    {
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(
        ventana
    );

    glfwSetKeyCallback(
        ventana,
        teclado
    );

    // Profundidad
    glEnable(
        GL_DEPTH_TEST
    );

    glEnable(
        GL_NORMALIZE
    );

    // Fondo gris claro
    glClearColor(
        0.88f,
        0.88f,
        0.88f,
        1.0f
    );

    // Luces y colores
    luces();

    // ========================================================
    // LOOP PRINCIPAL
    // ========================================================

    while (
        !glfwWindowShouldClose(
            ventana
        )
        )
    {
        glClear(
            GL_COLOR_BUFFER_BIT |
            GL_DEPTH_BUFFER_BIT
        );

        camara();

        piso();

        burro();

        glfwSwapBuffers(
            ventana
        );

        glfwPollEvents();
    }

    glfwDestroyWindow(
        ventana
    );

    glfwTerminate();

    return 0;
}