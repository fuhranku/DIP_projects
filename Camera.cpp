#include "Camera.h"
#include <iostream>
#include "Application.h"

const float MOVEMENT_SPEED = 1.0f;
Application* app2;

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
	app2 = Application::GetInstance();
	panning_zooming_speed = 500;
	viewMatrix = glm::lookAt(position, position + viewDirection, UP);
	orthoMatrix = glm::ortho(-windowWidth / 2 * zoom, windowWidth / 2 * zoom, -windowHeight / 2 * zoom, windowHeight / 2 * zoom, nearPlane, farPlane);
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
	std::cout << zoom << std::endl;
	zoom = zoom < 0 ? 0.000000005 : zoom;
}

void Camera::moveBackward(float time) {
	
	float speed = MOVEMENT_SPEED * time;
	zoom += speed;
}

void Camera::moveLeft(float time) {
	float speed = MOVEMENT_SPEED * time;
	glm::vec3 moveDirection = glm::cross(viewDirection, UP) * (panning_zooming_speed * zoom);
	position += -speed * moveDirection;
	viewMatrix = glm::lookAt(position, position + viewDirection, UP);

}

void Camera::moveRight(float time) {
	float speed = MOVEMENT_SPEED * time;
	glm::vec3 moveDirection = glm::cross(viewDirection, UP) * (panning_zooming_speed * zoom);
	position += speed * moveDirection;
	viewMatrix = glm::lookAt(position, position + viewDirection, UP);
}

void Camera::moveUp(float time) {
	float speed = MOVEMENT_SPEED * time;
	position += speed * UP * (panning_zooming_speed * zoom);
	viewMatrix = glm::lookAt(position, position + viewDirection, UP);
	printf("(%f,%f,%f)\n", position.x,position.y,position.z);
}

void Camera::moveDown(float time) {
	float speed = MOVEMENT_SPEED * time;
	position -= speed * UP * (panning_zooming_speed * zoom);
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