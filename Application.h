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
#include "userInterface.h"
#include <opencv2/highgui/highgui.hpp>
#include "Shader.h"
#include "Image.h"
#include "Camera.h"
#include "History.h"

class Application {

	static unsigned int windowWidth; // Window current width
	static unsigned int windowHeight; // Window current height
	const char* windowTitle = "Tarea PDI"; // Window title
	float deltaTime;
	float lastTime = 0;
	float currentTime = 0;
	float totalTime = 0;
	int totalFrames = 0;
	int fps = 0;
	GLFWwindow* window; // Window pointer
	Shader* shader; // Shader object
	Image *image; // Index (GPU) of the texture
	UI ui; // User Interface 
	static Camera camera;
	History history;

protected:
	static Application* _application;


private:
	static void OnMouseMotion(GLFWwindow* window, double xpos, double ypos);
	static void OnMouseButton(GLFWwindow* window, int button, int action, int mods);
	static void OnKeyPress(GLFWwindow* window, int key, int scancode, int action, int mods);
	void ProcessKeyboardInput(GLFWwindow* window);
	void CalcDeltaTime();
public:
	Application();
	~Application();

	/**
	 * Singletons should not be cloneable.
	 */
	Application(Application& other) = delete;
	/**
	 * Singletons should not be assignable.
	 */
	void operator=(const Application&) = delete;
	/**
	* Control access
	**/
	static Application* GetInstance();
	
	void MainLoop();
	void Render();
	void Update();
	void Resize();
	unsigned int LoadTexture(const char* path);

	bool Init();
	bool InitGlad();
	bool InitWindow();
	void InitGL();


};