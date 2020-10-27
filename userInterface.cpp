#include "userInterface.h"
#include <ImGuizmo.h>
#include "Application.h"

Application* app;

float thresh = 0.0f, maxValue = 255.0f;

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

float Plot_ArrayGetter(void* data, int idx){
	int* int_data = (int*)data;
	return idx > 255 ? 0 : int_data[idx];
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
		ImGui::PlotHistogram(
			"##Histogram",
			Plot_ArrayGetter,
			(int*)app->image->histogram[0].data,
			265,
			0,
			NULL,
			0.0f,
			app->image->histogram[0].maxValue, ImVec2(Application::windowWidth * 0.2f, 255.0f));
		//ImGui::PlotHistogram("Histogram", arr, IM_ARRAYSIZE(arr), 0, NULL, 0.0f, 1.0f, ImVec2(0, 80.0f));
	}ImGui::End();

	drawModals();

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
		if (ImGui::BeginMenu("Filter")) {
			if (ImGui::BeginMenu("Thresholding")) {
				if(ImGui::MenuItem("OTSU")) {
					activeModal = "OTSU Threshold##otsu_modal";
				}
				if (ImGui::MenuItem("Adaptive Threshold")) {
					activeModal = "Gaussian Adaptive Threshold##adaptive_modal";
				}
				ImGui::EndMenu();
			}
			if (ImGui::BeginMenu("Quantization")) {
				if (ImGui::MenuItem("Color reduce")) {
					activeModal = "Bits per pixel color reduction##color_reduce";
				}
				if (ImGui::MenuItem("Median cut")) {
					//activeModal = "Gaussian Adaptive Threshold##adaptive_modal";
				}
				if (ImGui::MenuItem("K-means")) {
					//activeModal = "Gaussian Adaptive Threshold##adaptive_modal";
				}
				ImGui::EndMenu();
			}
			ImGui::EndMenu();
		}
		ImGui::EndMainMenuBar();
	}
}

void UI::drawModals() {
	if (activeModal != "")
		ImGui::OpenPopup(activeModal.c_str());

	// OTSU Thresholding modal
	ImGui::SetNextWindowSize(ImVec2(300, 150));
	if (ImGui::BeginPopupModal("OTSU Threshold##otsu_modal"))
	{
		ImGui::Text("Please, set filter values");

		ImGui::Text("Thresh");
		ImGui::DragFloat("Thresh", &thresh, 0.05f, 0.0f, 255.0f, "%.2f");

		ImGui::Text("Max Value");
		ImGui::DragFloat("Max Value", &maxValue, 0.05f, 0.0f, 255.0f, "%.2f");

		if (ImGui::Button("Apply", ImVec2(80, 30))) {
			cv::Mat img = Application::GetInstance()->image->imgBGR;
			Image::OTSU(img, img, thresh, maxValue, Application::GetInstance()->image);
			ImGui::CloseCurrentPopup();
			activeModal = "";
		}

		ImGui::SameLine();
		if (ImGui::Button("Cancel", ImVec2(80, 30)))
		{
			ImGui::CloseCurrentPopup();
			activeModal = "";
		}
		ImGui::EndPopup();
	}

	// Gaussian Adaptive Thresholding modal
	ImGui::SetNextWindowSize(ImVec2(300, 150));
	if (ImGui::BeginPopupModal("Gaussian Adaptive Threshold##adaptive_modal"))
	{
		ImGui::Text("Please, set filter values");

		ImGui::Text("Thresh");
		ImGui::DragFloat("Thresh", &thresh, 0.05f, 0.0f, 255.0f, "%.2f");

		ImGui::Text("Max Value");
		ImGui::DragFloat("Max Value", &maxValue, 0.05f, 0.0f, 255.0f, "%.2f");

		if (ImGui::Button("Apply", ImVec2(80, 30))) {
			cv::Mat img = Application::GetInstance()->image->imgBGR;
			Image::GaussianAdaptiveThreshold(img, img, thresh, maxValue, Application::GetInstance()->image);
			ImGui::CloseCurrentPopup();
			activeModal = "";
		}

		ImGui::SameLine();
		if (ImGui::Button("Cancel", ImVec2(80, 30)))
		{
			ImGui::CloseCurrentPopup();
			activeModal = "";
		}
		ImGui::EndPopup();
	}

	// Color reduce modal
	ImGui::SetNextWindowSize(ImVec2(300, 150));
	if (ImGui::BeginPopupModal("Bits per pixel color reduction##color_reduce"))
	{
		ImGui::Text("Please, set operation values");

		ImGui::Text("Thresh");
		ImGui::DragFloat("Thresh", &thresh, 0.05f, 0.0f, 255.0f, "%.2f");

		ImGui::Text("Max Value");
		ImGui::DragFloat("Max Value", &maxValue, 0.05f, 0.0f, 255.0f, "%.2f");

		if (ImGui::Button("Apply", ImVec2(80, 30))) {
			cv::Mat img = Application::GetInstance()->image->imgBGR;
			Image::GaussianAdaptiveThreshold(img, img, thresh, maxValue, Application::GetInstance()->image);
			ImGui::CloseCurrentPopup();
			activeModal = "";
		}

		ImGui::SameLine();
		if (ImGui::Button("Cancel", ImVec2(80, 30)))
		{
			ImGui::CloseCurrentPopup();
			activeModal = "";
		}
		ImGui::EndPopup();
	}

}

void UI::terminate() {
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
}
