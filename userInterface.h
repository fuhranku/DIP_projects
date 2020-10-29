#pragma once
#include <imgui.hpp>
#include <string>

class UI{
public:
	UI();
	bool init(GLFWwindow* window);
	void mainDraw();
	void terminate();
	std::string activeModal = "";
private:
	void drawModals();
	void drawTopMenu();
	void drawBottomMenu();
	void drawSidebar();
	void drawHistograms();
	void pushDisable(bool condition);
	void popDisable(bool condition);

};

