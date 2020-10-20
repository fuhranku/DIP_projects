#include "Camera.h"
#include <iostream>

const float MOVEMENT_SPEED = 400.0f;

Camera::Camera(int windowWidth, int windowHeight) :
	viewDirection(0, 0, -1),
	UP(0.0f, 1.0f, 0.0f),
	position(0, 0, 0.0f){
	this->windowHeight = windowHeight;
	this->windowWidth = windowWidth;
	mouseSpeed = 70.0f;
	//yaw = 0.0f;
	//pitch = -20.0f;
	//glm::mat4 Rotation = glm::yawPitchRoll(glm::radians(yaw), glm::radians(pitch), 0.0f);
	//viewDirection = glm::vec3(Rotation * glm::vec4(0.0f, 0.0f, -1.0f, 0.0f));
	viewMatrix = glm::lookAt(position, position + viewDirection, UP);
	orthoMatrix = glm::ortho(-windowWidth/2 - zoom, windowWidth / 2 + zoom, -windowHeight / 2 - zoom, windowHeight / 2 + zoom, nearPlane, farPlane);
}

void Camera::mouseUpdate(const glm::vec2& newMousePosition) {
	yaw += newMousePosition.x;
	pitch += newMousePosition.y;
	// Avoid camera turning and invert controls
	checkCameraRotation();

	// Update camera rotation angle
	glm::mat4 Rotation = glm::yawPitchRoll(glm::radians(yaw), glm::radians(pitch), 0.0f);
	viewDirection = glm::vec3(Rotation * glm::vec4(0.0f, 0.0f, -1.0f, 0.0f));
	UP = glm::vec3(Rotation * glm::vec4(0.0f, 1.0f, 0.0f, 0.0f));
	//printf("yaw,pitch(%f,%f)\n", yaw, pitch);
	viewMatrix = glm::lookAt(position, position + viewDirection, UP);
}

glm::mat4 Camera::getWorldToViewMatrix() {
	viewMatrix = glm::lookAt(position, position + viewDirection, UP);
	return viewMatrix;
}

glm::mat4 Camera::getOrthoMatrix() {
	orthoMatrix = glm::ortho(-windowWidth / 2 - zoom, windowWidth / 2 + zoom, -windowHeight - zoom, windowHeight / 2 + zoom, nearPlane, farPlane);
	//printf("(%f,%f,%f,%f)\n", -windowWidth / 2 - zoom, windowWidth / 2 + zoom, -windowHeight - zoom, windowHeight / 2 + zoom);
	return orthoMatrix;
}


void Camera::moveForward(float time) {


	float speed = MOVEMENT_SPEED * time;
	zoom -= speed;
}

void Camera::moveBackward(float time) {
	
	float speed = MOVEMENT_SPEED * time;
	zoom += speed;
}

void Camera::moveLeft(float time) {
	float speed = MOVEMENT_SPEED * time;
	glm::vec3 moveDirection = glm::cross(viewDirection, UP);
	position += -speed * moveDirection;
	viewMatrix = glm::lookAt(position, position + viewDirection, UP);

}

void Camera::moveRight(float time) {
	float speed = MOVEMENT_SPEED * time;
	glm::vec3 moveDirection = glm::cross(viewDirection, UP);
	position += speed * moveDirection;
	viewMatrix = glm::lookAt(position, position + viewDirection, UP);
}

void Camera::moveUp(float time) {
	float speed = MOVEMENT_SPEED * 0.7 * time;
	position += speed * UP;
	viewMatrix = glm::lookAt(position, position + viewDirection, UP);
	printf("(%f,%f,%f)\n", position.x,position.y,position.z);
}

void Camera::moveDown(float time) {
	float speed = MOVEMENT_SPEED * 0.7 * time;
	position -= speed * UP;
	viewMatrix = glm::lookAt(position, position + viewDirection, UP);
	printf("(%f,%f,%f)\n", position.x, position.y, position.z);

}

void Camera::checkCameraRotation() {
	//if (yaw > 130.0f) yaw = 130.0f;
	if (pitch > 90.0f) pitch = 90.0f;
	//if (yaw < -130.0f) yaw = -130.0f;
	if (pitch < -90.0f) pitch = -90.0f;
}

void Camera::resize(int windowWidth, int windowHeight) {
	//perspectiveMatrix = glm::perspective(glm::radians(45.0f), (float)windowWidth / (float)windowHeight, nearPlane, farPlane);
}