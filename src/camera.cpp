#include "camera.h"

Camera::Camera(glm::vec3 target, float yaw, float pitch)
    : Target(target), Yaw(yaw), Pitch(pitch), Radius(INITIAL_RADIUS)
{
}


glm::mat4 Camera::GetViewMatrix() const
{
    glm::vec3 position;
    position.x = Radius * cos(glm::radians(Pitch)) * sin(glm::radians(Yaw));
    position.y = Radius * sin(glm::radians(Pitch));
    position.z = Radius * cos(glm::radians(Pitch)) * cos(glm::radians(Yaw));
    position += Target;

    return glm::lookAt(position, Target, WorldUp);
}


void Camera::ProcessMouseMovement(float xOffset, float yOffset)
{
    Yaw -= xOffset * MouseSensitivity;
    Pitch -= yOffset * MouseSensitivity;

    // Clamp to prevent camera flip
    if (Pitch > 89.0f)  Pitch = 89.0f;
    if (Pitch < -89.0f) Pitch = -89.0f;
}

void Camera::ProcessMouseScroll(float yOffset)
{
    Radius -= yOffset * 0.25f; 
    if (Radius < 0.1f) Radius = 0.1f;
    if (Radius > 100.0f) Radius = 100.0f;
}

glm::vec3 Camera::GetPosition() const
{
    glm::vec3 position;
    position.x = Radius * cos(glm::radians(Pitch)) * sin(glm::radians(Yaw));
    position.y = Radius * sin(glm::radians(Pitch));
    position.z = Radius * cos(glm::radians(Pitch)) * cos(glm::radians(Yaw));
    return position + Target;
}
