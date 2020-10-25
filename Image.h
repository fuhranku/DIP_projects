#pragma once
#include <glad/glad.h> 
#include <GLFW/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <stb_image.h>
#include <iostream>
#include <vector>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>

struct Histogram {
	int* data;
	int type;
	int maxValue = 0;
	void setMaxValue(int value) {
		maxValue = maxValue < value ? value : maxValue;
	}
	Histogram(int type) {
		if (type == GL_BGR) {
			data = new int[256];
			memset(data, 0, 256 * sizeof(int));
		}
		else {
			data = new int[256];
			memset(data, 0, 256 * sizeof(int));
			this->type = type;
		}
	}
};

struct Vertex {
	glm::vec3 vertices;
	glm::vec2 texture;
	glm::vec3 normal;
	Vertex(glm::vec3 pos, glm::vec2 texCoord = glm::vec2(0, 0)) :
		vertices(pos),
		texture(texCoord) {}
};

class Image
{
private:
public:
	Image(const char* path);
	~Image();
	void BuildPlane();
	unsigned int VAO;
	unsigned int VBO;
	std::vector<Histogram> histogram;
	cv::Mat imgBGR;
	void buildHistograms();
	int width;
	int height;
	int channels;
	unsigned int id;
};

