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

float Plot_ArrayGetter(void* data, int idx)
{
	int* int_data = (int*)data;
	int value = int_data[idx] / 1.0f;
	return value;
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
		//std::cout << Application::GetInstance()->image->histogram[2].data[125] << std::endl;
		//std::cout << app->image->histogram[2].data[125] << std::endl;
		//for (int i = 0; i < 256 * 256 * 256; i++) {
		//	printf("index: %i, val: %i, max: %i\n", i, Application::GetInstance()->image->histogram[2].data[i], app->image->histogram[3].maxValue);
		//}
		//for (int i = 0; i < 256 * 256 * 256; i++) {
		//	std::cout << Application::GetInstance()->image->histogram[3].data[i] << std::endl;
		//}
		//std::cout << Application::GetInstance()->image->histogram[3].data[7701] << std::endl;
		ImGui::PlotHistogram(
			"##Histogram",
			Plot_ArrayGetter,
			(int*)app->image->histogram[0].data,
			256,
			0,
			NULL,
			0.0f,
			app->image->histogram[0].maxValue, ImVec2(Application::windowWidth * 0.2f, 255.0f));
		//ImGui::PlotHistogram("Histogram", arr, IM_ARRAYSIZE(arr), 0, NULL, 0.0f, 1.0f, ImVec2(0, 80.0f));
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
