#pragma once
#include <glad/glad.h> 
#include <GLFW/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <vector>
#include "Shader.h"

class Grid {
public:
	unsigned int gridVAO, gridVBO, gridIBO, gridLength;
	Grid(int width, int height);
	Grid();
	void Init();
	void Draw(glm::mat4 viewMatrix, glm::mat4 orthoMatrix);
	Shader* gridShader;
};

class Canvas {
public:
	unsigned int VAO;
	unsigned int VBO;
	int width, height;
	Grid grid;
	std::vector<float> vertices;
	Canvas();
	void Build(int width, int height);
	void Update(int width, int height);
	void Delete();
private:

};

