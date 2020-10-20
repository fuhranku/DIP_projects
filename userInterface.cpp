#include "userInterface.h"
#include <ImGuizmo.h>

UI::UI() {
}

bool UI::init(GLFWwindow* window) {
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	if (!ImGui_ImplGlfw_InitForOpenGL(window, true) || !ImGui_ImplOpenGL3_Init("#version 330 core"))
		return false;
	ImGui::StyleColorsDark();

	//Get singleton instance;
	//instance = icarus3D::Instance();
	return true;
}

void UI::draw() {
	// Start new frame
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	showMainMenuBar();

	ImGui::Begin("Options menu",0,ImGuiWindowFlags_NoResize |
							      ImGuiWindowFlags_NoMove   |
								  ImGuiWindowFlags_NoCollapse); {
		ImGui::Text("Negative filter");
		ImGui::SameLine();
		ImGui::Button("Apply");
	}ImGui::End();

	// Render dear imgui into screen
	ImGui::EndFrame();
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void UI::showMainMenuBar() {

	if (ImGui::BeginMainMenuBar())
	{
		// File Option
		if (ImGui::BeginMenu("File"))
		{
			if (ImGui::MenuItem("New Scene")) {
			}
			if (ImGui::MenuItem("Open Scene")) {
			}
			if (ImGui::MenuItem("Save Scene", NULL, false)) {
			}
			ImGui::EndMenu();
		}
		ImGui::EndMainMenuBar();
	}
}


void UI::terminate() {
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
}
