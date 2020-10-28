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
#include <functional>
#include <algorithm>
#include <numeric>


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

class bgrColor{
public:
	int b, g, r;
	bgrColor(int b, int g, int r) {
		this->b = b;
		this->g = g;
		this->r = r;
	}
};

class Image
{
private:
	static unsigned int findBiggestRange(std::vector<bgrColor> color);
	static void QuantizeMC(std::vector<bgrColor>& out,std::vector<bgrColor> color, int currentDepth, int maxDepth);
	static Image* Any2BGR (Image* image);
	static Image* Any2Gray(Image* image);
public:
	Image(const char* path);
	Image();
	~Image();
	void BuildPlane();
	unsigned int VAO;
	unsigned int VBO;
	std::vector<Histogram> histogram;
	cv::Mat imgData;
	static void Histograms(Image* image);
	static void UpdateTextureData(Image* image);
	static void OTSU(double thresh, double maxValue, Image* image);
	static void GaussianAdaptiveThreshold(double thresh, double maxValue, Image* image);
	static void MedianCut(int blocks, Image* image);
	static void KMeans(int k, Image* image);
	static void ColorReduce(int numBits, Image* image);
	int width;
	int height;
	int channels;
	int bitDepth = 256;
	unsigned int format = 0, internalFormat = 0;
	unsigned int id;
};

