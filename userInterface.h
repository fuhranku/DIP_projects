#pragma once
#include <imgui.hpp>
#include <string>

class UI{
public:
	UI();
	bool init(GLFWwindow* window);
	void draw();
	void terminate();
private:
	std::string activeModal = "";
	void drawModals();
	void showMainMenuBar();
};

