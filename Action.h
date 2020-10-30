#pragma once
#include "Image.h"

enum ACTION_TYPE {

	QUANTIZATION_BIT_REDUCTION,
	QUANTIZATION_MEDIAN_CUT,
	QUANTIZATION_K_MEDIAN,

	THRESHOLD_OTSU,
	THRESHOLD_GAUSSIAN,

	HISTOGRAM_EQUALIZATION,

	REGION_GROW,

	FOURIER_TRANSFORM,
	FOURIER_TRANSFORM_INVERSE

};


class Action
{

public:
	Action(Image* image, ACTION_TYPE type);
	~Action();

	ACTION_TYPE type;
	cv::Mat imgData;
	int width;
	int height;
	int channels;
	int bitDepth = 256;
	int dpi;
	Size size;
	Extension ext;
	std::string path;
	unsigned int format = 0, internalFormat = 0;
	unsigned int id;
};

