#pragma once
#include <glad/glad.h> 
#include <GLFW/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <vector>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/imgcodecs.hpp>
#include <functional>
#include <algorithm>
#include <numeric>
#include <string>
#include "Canvas.h"
#include "BGRColor.h"

#define IMG_NONE 0x000000
#define IMG_BMP 0x000001
#define IMG_JPG 0x000002
#define IMG_PNG 0x000003
#define IMG_WEBP 0x000004
#define IMG_TIFF 0x000005
#define	IMG_QUANTIZATION_BIT_REDUCTION 0x000006
#define	IMG_QUANTIZATION_MEDIAN_CUT	   0x000007
#define	IMG_QUANTIZATION_K_MEANS	   0x000008
#define	IMG_THRESHOLD_OTSU			   0x000009
#define	IMG_THRESHOLD_GAUSSIAN		   0x00000A
#define	IMG_HISTOGRAM_EQUALIZATION	   0x00000B
#define	IMG_FLOODFILL				   0x00000C
#define	IMG_FOURIER_TRANSFORM		   0x00000D
#define	IMG_FOURIER_TRANSFORM_INVERSE  0x00000E
#define IMG_TRANSFORM_ROTATE           0x00000F
#define IMG_TRANSFORM_FLIP             0x000010
#define	IMG_FOURIER_LOW_PASS		   0x000011
#define	IMG_FOURIER_HIGH_PASS		   0x000012
#define	IMG_MORPHOLOGY_DILATION		   0x000013
#define	IMG_MORPHOLOGY_EROSION		   0x000014
#define	IMG_MORPHOLOGY_OPEN			   0x000015
#define	IMG_MORPHOLOGY_CLOSE		   0x000016
#define IMG_PAINT					   0x000017




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

struct Extension {
	int ext = 0;
	std::string str_name = "None";
	Extension(int ext, std::string str) {
		this->ext = ext;
		this->str_name = str;
	}
	Extension() {}
};

struct Size {
	int val = 0;
	std::string unit;
	Size(int totalPixels, int channels) {
		val = totalPixels * channels;
		// Size in Bytes
		if (val < 1024)
			unit = "Bytes";
		else if ( val < 1024*1024){
			val /= 1024;
			unit = "KB";
		}
		else {
			val /= 1024*1024;
			unit = "MB";
		}
	}
	Size() {}
};

struct Vertex {
	glm::vec3 vertices;
	glm::vec2 texture;
	glm::vec3 normal;
	Vertex(glm::vec3 pos, glm::vec2 texCoord = glm::vec2(0, 0)) :
		vertices(pos),
		texture(texCoord) {}
};

struct FreqData {
	int originalWidth, originalHeight;
	cv::Mat mag;
	cv::Mat complexI;
};

class Image
{
private:
	void getFileExtension(std::string path);
public:
	void computeHalfSize();
	Image(const char* path);
	Image(const char* path, int colorSpace);
	Image();
	~Image();
	Canvas canvas;
	std::vector<Histogram> histogram;
	std::vector<FreqData> freqData;
	cv::Mat imgData;
	glm::vec2 arrhalfWidth, arrhalfHeight;
	int width;
	int height;
	int channels;
	int bitDepth = 256;
	int dpi;
	int currentFilter = 0;
	bool freqComputed = 0;
	Size size;
	Extension ext;
	std::string path;
	unsigned int format = 0, internalFormat = 0;
	unsigned int id;
	static void cloneImage(Image* src, Image* dst);
};

