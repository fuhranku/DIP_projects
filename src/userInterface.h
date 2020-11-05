#pragma once
#include <imgui.hpp>
#include <string>
#include "Image.h"

class UI{
public:
	UI();
	bool init(GLFWwindow* window);
	void mainDraw();
	void terminate();
	std::string activeModal = "";
	bool flood_fill_bool = 0;
	bool paint_mode = true;
	int floodfill_range_selected = 0;
	int nhbrhd_elements_count = 0;
	glm::ivec4 loDiff, upDiff;
	cv::Scalar floodFill_color;
private:
	void drawModals();
	void drawTopMenu();
	void drawBottomMenu();
	void drawSidebar();
	void drawHistograms();
	void pushDisable(bool condition);
	void popDisable(bool condition);

};

