#include "Camera.h"
#include <iostream>
#include "Application.h"

const float MOVEMENT_SPEED = 10.0f;
const float PANNING_SPEED = 50.0f;

Camera::Camera(int windowWidth, int windowHeight) :
	viewDirection(0.0f, 0.0f, -1.0f),
	UP(0.0f, 1.0f, 0.0f),
	position(0.0f, 0.0f, 0.0f){
	this->windowHeight = windowHeight;
	this->windowWidth = windowWidth;
	mouseSpeed = 70.0f;
	panning_zooming_speed = 35;
	viewMatrix = glm::lookAt(position, position + viewDirection, UP);
	orthoMatrix = glm::ortho(-windowWidth / 2 * zoom, windowWidth / 2 * zoom, -windowHeight / 2 * zoom, windowHeight / 2 * zoom, nearPlane, farPlane);
}

glm::mat4 Camera::getWorldToViewMatrix() {
	viewMatrix = glm::lookAt(position, position + viewDirection, UP);
	return viewMatrix;
}

glm::mat4 Camera::getOrthoMatrix() {
	windowWidth = (double)Application::GetInstance()->windowWidth;
	windowHeight = (double)Application::GetInstance()->windowHeight;
	orthoMatrix = glm::ortho(-windowWidth * 0.8f /2 * zoom, windowWidth*0.8f / 2 * zoom, -windowHeight / 2 * zoom, windowHeight / 2 * zoom, nearPlane, farPlane);
	//printf("zoom: %f\n",zoom);
	//double size = 200.0f;
	//double aspect = (double)Application::GetInstance()->windowWidth / (double)Application::GetInstance()->windowHeight;
	//orthoMatrix = glm::ortho(-aspect * size / 2.0 * zoom, aspect * size / 2.0 * zoom, -size / 2.0 * zoom, size / 2.0 * zoom, -1.0, 1.0);
	return orthoMatrix;
}


void Camera::moveForward(float time) {
	float speed = MOVEMENT_SPEED * time;
	zoom -= speed;

	if (zoom < 0.015f) zoom = 0.015f;
	//std::cout << zoom << std::endl;
}

void Camera::moveBackward(float time) {
	float speed = MOVEMENT_SPEED * time;
	zoom += speed;
}

void Camera::moveLeft(float time) {
	float speed = MOVEMENT_SPEED * time;
	glm::vec3 moveDirection = glm::vec3(-1,0,0) * (panning_zooming_speed * zoom);
	position += speed * moveDirection;
	viewMatrix = glm::lookAt(position, position + viewDirection, UP);

}

void Camera::moveRight(float time) {
	float speed = MOVEMENT_SPEED * time;
	glm::vec3 moveDirection = glm::vec3(1,0,0) * (panning_zooming_speed * zoom);
	position += speed * moveDirection;
	viewMatrix = glm::lookAt(position, position + viewDirection, UP);
}

void Camera::moveUp(float time) {
	float speed = MOVEMENT_SPEED * time;
	position += speed * glm::vec3(0,1,0) * (panning_zooming_speed * zoom);
	viewMatrix = glm::lookAt(position, position + viewDirection, UP);
	//printf("(%f,%f,%f)\n", position.x,position.y,position.z);
}

void Camera::moveDown(float time) {
	float speed = MOVEMENT_SPEED * time;
	position += speed * glm::vec3(0, -1, 0) * (panning_zooming_speed * zoom);
	viewMatrix = glm::lookAt(position, position + viewDirection, UP);
	//printf("(%f,%f,%f)\n", position.x, position.y, position.z);

}

void Camera::moveDir(float time, glm::vec2 dir) {
	float speed = panning_zooming_speed * time;
	glm::vec3 delta = glm::vec3(-dir.x, dir.y, 0)/2.0f * zoom;
	position += /* speed * */ delta;
	//printf("position: (%f,%f,%f) \n", position.x, position.y, position.z);
	viewMatrix = glm::lookAt(position, position + viewDirection, UP);
}

void Camera::checkCameraRotation() {
	//if (yaw > 130.0f) yaw = 130.0f;
	if (pitch > 90.0f) pitch = 90.0f;
	//if (yaw < -130.0f) yaw = -130.0f;
	if (pitch < -90.0f) pitch = -90.0f;
}

float Camera::getUIZoom() {
	float uiZoom = 1 / zoom * 100;
	return uiZoom;
}

void Camera::resize(int windowWidth, int windowHeight) {
	//perspectiveMatrix = glm::perspective(glm::radians(45.0f), (float)windowWidth / (float)windowHeight, nearPlane, farPlane);
}