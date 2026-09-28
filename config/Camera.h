#pragma once
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

enum Camera_Movement { FORWARD, BACKWARD, LEFT, RIGHT };

// Valores por defecto (el diorama mide ~2.5 m, por eso la velocidad es menor que con RedDog)
const GLfloat YAW         = -90.0f;
const GLfloat PITCH       =  0.0f;
const GLfloat SPEED       =  3.0f;
const GLfloat SENSITIVITY =  0.25f;
const GLfloat ZOOM        =  45.0f;

class Camera
{
public:
    Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f),
           glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f),
           GLfloat yaw = YAW, GLfloat pitch = PITCH)
        : front(glm::vec3(0.0f, 0.0f, -1.0f)),
          movementSpeed(SPEED), mouseSensitivity(SENSITIVITY), zoom(ZOOM)
    {
        this->position = position;
        this->worldUp  = up;
        this->yaw      = yaw;
        this->pitch    = pitch;
        this->updateCameraVectors();
    }

    glm::mat4 GetViewMatrix() { return glm::lookAt(position, position + front, up); }

    void ProcessKeyboard(Camera_Movement direction, GLfloat deltaTime)
    {
        GLfloat velocity = movementSpeed * deltaTime;
        if (direction == FORWARD)  position += front * velocity;
        if (direction == BACKWARD) position -= front * velocity;
        if (direction == LEFT)     position -= right * velocity;
        if (direction == RIGHT)    position += right * velocity;
    }

    void ProcessMouseMovement(GLfloat xOffset, GLfloat yOffset, GLboolean constrainPitch = true)
    {
        xOffset *= mouseSensitivity;
        yOffset *= mouseSensitivity;
        yaw   += xOffset;
        pitch += yOffset;

        if (constrainPitch)
        {
            if (pitch >  89.0f) pitch =  89.0f;
            if (pitch < -89.0f) pitch = -89.0f;
        }
        updateCameraVectors();
    }

    void ProcessMouseScroll(GLfloat yOffset)
    {
        zoom -= yOffset;
        if (zoom < 20.0f) zoom = 20.0f;
        if (zoom > 60.0f) zoom = 60.0f;
    }

    GLfloat   GetZoom()     { return zoom; }
    glm::vec3 GetPosition() { return position; }

private:
    glm::vec3 position, front, up, right, worldUp;
    GLfloat yaw, pitch;
    GLfloat movementSpeed, mouseSensitivity, zoom;

    void updateCameraVectors()
    {
        glm::vec3 f;
        f.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
        f.y = sin(glm::radians(pitch));
        f.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
        front = glm::normalize(f);
        right = glm::normalize(glm::cross(front, worldUp));
        up    = glm::normalize(glm::cross(right, front));
    }
};
