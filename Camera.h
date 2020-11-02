#pragma once
#include <glm\glm.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/transform.hpp>
#include <glm/gtx/euler_angles.hpp>


class Camera
{
public:
    glm::vec3 position;
    glm::vec3 viewDirection;
    glm::vec3 UP;
    glm::vec2 oldMousePosition;
    int windowWidth;
    int windowHeight;
    float mouseSpeed;
    float nearPlane = -1.0f;
    float farPlane = 1.0f;
    float zoom = 1.0f;
    float panning_zooming_speed;
    float yaw, pitch;
    glm::mat4 viewMatrix = glm::mat4(1.0f);
    glm::mat4 orthoMatrix = glm::mat4(1.0f);
    Camera(int windowWidth, int windowHeight);
    glm::mat4 getWorldToViewMatrix();
    glm::mat4 getOrthoMatrix();
    void moveForward(float time);
    void moveBackward(float time);
    void moveLeft(float time);
    void moveRight(float time);
    void moveUp(float time);
    void moveDown(float time);
    void moveDir(float time, glm::vec2 dir);
    void resize(int windowWidth, int windowHeight);
    float getUIZoom();
private:
    void checkCameraRotation();
};

