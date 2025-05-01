#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// Camera movement directions (kept for future zoom/pan control if needed)
enum Camera_Movement
{
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT,
    UP,
    DOWN,
};

// Default constants
const float YAW = 0.0f;
const float PITCH = 0.0f;
const float SPEED = 0.5f;
const float SENSITIVITY = 0.25f;
const float INITIAL_RADIUS = 3.0f;

const glm::vec3 WorldUp{ 0.0f, 1.0f, 0.0f };

// Orbiting Camera struct
struct Camera
{
    // Target (center of orbit)
    glm::vec3 Target = glm::vec3(0.0f);

    // Orbit properties
    float Radius = INITIAL_RADIUS;
    float Yaw = YAW;
    float Pitch = PITCH;

    // Controls
    float MovementSpeed = SPEED;
    float MouseSensitivity = SENSITIVITY;

    // Constructor
    Camera(glm::vec3 target = glm::vec3(0.0f), float yaw = YAW, float pitch = PITCH);

    // Get view matrix from computed orbit position
    glm::mat4 GetViewMatrix() const;

    // Handle input from mouse drag (to orbit)
    void ProcessMouseMovement(float xOffset, float yOffset);

    void ProcessMouseScroll(float yOffset);

    glm::vec3 GetPosition() const;
};