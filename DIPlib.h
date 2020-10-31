#pragma once
#include <vector>
#include "Image.h"
#include "BGRColor.h"

class DIPlib
{
private:
	static unsigned int findBiggestRange(std::vector<bgrColor> color);
	static void QuantizeMC(std::vector<bgrColor>& out, std::vector<bgrColor> color, int currentDepth, int maxDepth);
	static void Any2BGR(Image* image);
	static void Any2Gray(Image* image);
	static void Any2YCrCb(Image* image);
public:
	static void Histograms(Image* image);
	static void UpdateTextureData(Image* image);
	static void OTSU(double thresh, double maxValue, Image* image);
	static void GaussianAdaptiveThreshold(double thresh, double maxValue, Image* image);
	static void MedianCut(int blocks, Image* image);
	static void KMeans(int k, Image* image);
	static void ColorReduce(int numBits, Image* image);
	static void EqualizeHist(Image* image);
	static void Rotate(Image* image, float deg);
	static void Flip(Image* image, int mode);
};

