#pragma once
#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <glad/glad.h> // Glad has to be include before glfw
#include <GLFW/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <stb_image.h>
#include <IMGUI/imgui.hpp>
#include "userInterface.h"
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "Shader.h"
#include "Image.h"
#include "Camera.h"
#include "History.h"

class Application {

	const char* windowTitle = "Tarea PDI"; // Window title
	float lastTime = 0;
	float currentTime = 0;
	float totalTime = 0;
	int totalFrames = 0;
	int fps = 0;
	GLFWwindow* window; // Window pointer
	Shader *shader, *gridShader; // Shader object
	UI ui; // User Interface 
	History history;
	// GRID
	unsigned int gridVAO, gridVBO, gridIBO, gridLength;

private:
	static Application* _application;
	static void OnMouseMotion(GLFWwindow* window, double xpos, double ypos);
	static void OnMouseButton(GLFWwindow* window, int button, int action, int mods);
	static void OnKeyPress(GLFWwindow* window, int key, int scancode, int action, int mods);
	static void Resize(GLFWwindow* window, int width, int height);
	static void OnMouseScroll(GLFWwindow* window, double posX, double posY);
	void ProcessKeyboardInput(GLFWwindow* window);
	void CalcDeltaTime();
	Application();
	~Application();
public:
	float deltaTime;
	Image *image; // Index (GPU) of the texture
	static Camera camera;
	static unsigned int windowWidth; // Window current width
	static unsigned int windowHeight; // Window current height

	static Application* GetInstance();

	void drawGrid();
	
	void Render();
	void Update();
	unsigned int LoadTexture(const char* path);
	void InitInstance();
	bool Init();
	bool InitGlad();
	bool InitWindow();
	void InitGrid();
	void InitGL();


};