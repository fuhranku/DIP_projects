#include "userInterface.h"
#include <ImGuizmo.h>
#include "Application.h"

Application* app;

UI::UI() {
}

bool UI::init(GLFWwindow* window) {
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	if (!ImGui_ImplGlfw_InitForOpenGL(window, true) || !ImGui_ImplOpenGL3_Init("#version 330 core"))
		return false;
	ImGui::StyleColorsDark();

	//Get singleton instance;
	app = Application::GetInstance();

	return true;
}

void UI::draw() {
	// Start new frame
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	showMainMenuBar();
	ImGui::SetNextWindowPos(ImVec2(Application::windowWidth * 0.8f,0));
	ImGui::SetNextWindowSize(ImVec2(Application::windowWidth * 0.2f, Application::windowHeight));
	ImGui::Begin("Options menu",0,ImGuiWindowFlags_NoResize |
							      ImGuiWindowFlags_NoMove   |
								  ImGuiWindowFlags_NoCollapse |
								  ImGuiWindowFlags_NoSavedSettings); {
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
