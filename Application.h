#pragma once
#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <glad/glad.h> // Glad has to be include before glfw
#include <GLFW/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <stb_image.h>
#include <opencv2/opencv.hpp>
#include <IMGUI/imgui.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "Shader.h"

class Application {

	unsigned int windowWidth = 800; // Window current width
	unsigned int windowHeight = 600; // Window current height
	const char* windowTitle = "Basic Demo"; // Window title
	GLFWwindow* window; // Window pointer
	Shader* shader; // Shader object
	unsigned int VBO; // Index (GPU) of the geometry buffer
	unsigned int VAO; // Index (GPU) vertex array object
	unsigned int textureID; // Index (GPU) of the texture

public:
	Application();
	~Application();

	void MainLoop();
	void Render();
	void Update();
	void ImGui();
	void Resize();
	void BuildGeometry();
	unsigned int LoadTexture(const char* path);
	void ProcessKeyboardInput(GLFWwindow* window);

	bool Init();
	bool InitGlad();
	bool InitWindow();
	void InitGL();


};