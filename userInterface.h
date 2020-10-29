#pragma once
#include <imgui.hpp>
#include <string>

class UI{
public:
	UI();
	bool init(GLFWwindow* window);
	void mainDraw();
	void terminate();
private:
	std::string activeModal = "";
	void drawModals();
	void drawTopMenu();
	void drawBottomMenu();
	void drawSidebar();
	void drawHistograms();
};

