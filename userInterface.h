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
	int floodfill_range_selected = IMG_FLOODFILL_RG_FIXED;
	int nhbrhd_elements_count = 0;
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

