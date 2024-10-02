#pragma once
#ifndef CAMERA_H
#define CAMERA_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <vector>
#include <iostream>


#include "ray.h"

// Defines several possible options for camera movement. Used as abstraction to stay away from window-system specific input methods
enum Camera_Movement {
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT
};

// Default camera values
const float YAW = 0.0f;
const float PITCH = 0.0f;
const float SPEED = 5.0f;
const float SENSITIVITY = 0.1f;
const float ZOOM = 45.0f; //(FOV)

// An abstract camera class that processes input and calculates the corresponding Euler Angles, Vectors and Matrices for use in OpenGL
class Camera
{
public:
    static Camera MainCamera;

    // Camera Attributes
    glm::vec3 position;
    glm::vec3 front;
    glm::vec3 up;
    glm::vec3 right;
    glm::vec3 worldUp;
    // Euler Angles
    float yaw;
    float pitch;
    // Camera options
    float movementSpeed;
    float mouseSensitivity;
    float zoom;
    // Screen spces and projection
    int screenWidth;
    int screenHeight;
    glm::mat4 perspective;
    glm::mat4 inversePerspective;

    // Constructor with vectors
    Camera(glm::vec3 pos = glm::vec3(0.0f, 1.0f, 0.0f), 
        glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), float yaw = YAW, float pitch = PITCH,
        int screenWidth = 800, int screenHeight = 800)
        : front(glm::vec3(0.0f, 0.0f, -1.0f)), movementSpeed(SPEED), mouseSensitivity(SENSITIVITY), zoom(ZOOM), screenHeight(screenHeight), screenWidth(screenWidth)
    {
        // set position
        this->position = pos;
        // set Euler Angles
        // yaw defaults to -z direction.
        this->yaw = yaw;
        this->pitch = pitch;

        worldUp = glm::vec3(0.0f, 1.0f, 0.0f);
        perspective = glm::perspective(glm::radians(45.0f), (float)(screenWidth / screenHeight), 0.1f, 100.0f);
        inversePerspective = glm::inverse(perspective);
        updateCameraVectors();
    }


    // Returns the view matrix calculated using Euler Angles and the LookAt Matrix
    glm::mat4 GetViewMatrix(){
        glm::mat4 view;
        view = glm::lookAt(position, position + front, up);
        return view;
    }

    glm::mat4 GetPerspectiveMatrix() {
        return perspective;
    }

    // Raycasting
    glm::vec3 ScreenPointToRay(float mouseX, float mouseY) {
        mouseX = 2 * mouseX / screenWidth - 1;
        mouseY = 1 - 2 * mouseY / screenHeight;
        glm::vec4 ndc(mouseX, mouseY, -1, 1);
        ndc = inversePerspective * ndc;
        glm::vec3 dir(ndc.x, ndc.y, ndc.z);

        //inverse of view matrix is a transformation from camera to world space
        //this is easy since we already have camera's local axis vectors in world space coordinates.
        glm::mat3 inverseView(0.0f);
        inverseView[0] = right, inverseView[1] = up, inverseView[2] = -front;

        //note the direction returned is not normalized
        return inverseView * dir;
    }

    // Processes input received from any keyboard-like input system. Accepts input parameter in the form of camera defined ENUM (to abstract it from windowing systems)
    void ProcessKeyboard(Camera_Movement direction, float deltaTime)
    {
        switch (direction) {
        case FORWARD:
            position += movementSpeed * deltaTime * front;
            break;
        case BACKWARD:
            position -= movementSpeed * deltaTime * front;
            break;
        case LEFT:
            position -= movementSpeed * deltaTime * right;
            break;
        case RIGHT:
            position += movementSpeed * deltaTime * right;
            break;
        }

    }

    // Processes input received from a mouse input system. Expects the offset value in both the x and y direction.
    void ProcessMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch = true)
    {
        // TODO : fill in
        // pitch between -89 and 89 degree
        xoffset *= mouseSensitivity;
        yoffset *= mouseSensitivity;

        yaw += xoffset;
        pitch += yoffset;


        // clipping 
        if (pitch > 89.f) pitch = 89.f;
        if (pitch < -89.f) pitch = -89.f;

        // calculate front and other vectors
        updateCameraVectors();
    }

    // Processes input received from a mouse scroll-wheel event. Only requires input on the vertical wheel-axis
    void ProcessMouseScroll(float yoffset)
    {
        // TODO : fill in
        zoom -= (float)yoffset;
        if (zoom < 1.0f)
            zoom = 1.0f;
        if (zoom > 45.0f)
            zoom = 45.0f;
    }

private:
    // Calculates the front vector from the Camera's (updated) Euler Angles
    void updateCameraVectors()
    {
        glm::vec3 dir = glm::vec3(0.f, 0.f, 0.f);
        dir.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
        dir.y = sin(glm::radians(pitch));
        dir.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
        front = -glm::normalize(dir);

        // also calculate other axes directions.
        right = glm::normalize(glm::cross(front, worldUp));
        up = glm::cross(right, front);
        
    }
};
#endif