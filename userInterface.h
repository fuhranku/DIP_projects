#pragma once
#include <imgui.hpp>
#include <string.h>

class UI{
public:
	UI();
	bool init(GLFWwindow* window);
	void draw();
	void terminate();
private:
	void showMainMenuBar();
};

