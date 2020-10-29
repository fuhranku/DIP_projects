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

void UI::mainDraw() {
	// Start new frame
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	//Drawing functions
	drawTopMenu();
	drawSidebar();
	drawBottomMenu();
	drawModals();

	// Render dear imgui into screen
	ImGui::EndFrame();
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void UI::drawSidebar() {
	ImGui::SetNextWindowPos(ImVec2(Application::windowWidth * 0.8f, 0));
	ImGui::SetNextWindowSize(ImVec2(Application::windowWidth * 0.2f, Application::windowHeight - 23));
	ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
	ImGui::Begin("Options menu", 0, ImGuiWindowFlags_NoResize |
		ImGuiWindowFlags_NoMove |
		ImGuiWindowFlags_NoCollapse |
		ImGuiWindowFlags_NoSavedSettings |
		ImGuiWindowFlags_NoBringToFrontOnFocus);
	{
		if (Application::GetInstance()->imageLoaded) {
			ImGui::SetNextItemOpen(true);
			if (ImGui::TreeNode("Image metadata")) {
				const char* channels[] = { "Grayscale","Red-Green","RGB","RGBA" };
				ImGui::Text("Channels: %s", channels[Application::GetInstance()->image->channels - 1]);
				ImGui::Separator();
				ImGui::Text("Histogram information: ");
				drawHistograms();
				ImGui::TreePop();
			}
		}
	}
	ImGui::End();
	ImGui::PopStyleVar();
	ImGui::PopStyleVar();
}

void UI::drawTopMenu() {

	if (ImGui::BeginMainMenuBar())
	{
		// File Option
		if (ImGui::BeginMenu("File"))
		{
			if (ImGui::MenuItem("New Tab")) {
			}
			if (ImGui::MenuItem("Open Image")) {
				activeModal = "Open Image##open_image";
			}
			if (ImGui::MenuItem("Save Image", NULL, false)) {
				//Application::SaveImage(Application::GetInstance()->image, imageFormat);
			}
			ImGui::EndMenu();
		}
		if (!Application::GetInstance()->imageLoaded) {
			ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
			ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
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
					activeModal = "Median cut##median_cut";
				}
				if (ImGui::MenuItem("K-means")) {
					activeModal = "K-means Technique##kmean_technique";
				}
				ImGui::EndMenu();
			}
			ImGui::EndMenu();
		}
		if (!Application::GetInstance()->imageLoaded)
		{
			ImGui::PopItemFlag();
			ImGui::PopStyleVar();
		}

		ImGui::EndMainMenuBar();
	}
}

void UI::drawBottomMenu() {
	
	ImGui::SetNextWindowPos(ImVec2(0, Application::windowHeight-27));
	ImGui::SetNextWindowSize(ImVec2(Application::windowWidth, 27));
	ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(5.0f,5.0f));
	ImGui::Begin("##bottom_menu_bar", 0, ImGuiWindowFlags_NoResize |
		ImGuiWindowFlags_NoMove |
		ImGuiWindowFlags_NoCollapse |
		ImGuiWindowFlags_NoSavedSettings |
		ImGuiWindowFlags_NoTitleBar );
	{
		if (ImGui::Button("-##less_zoom")) {
			Application::GetInstance()->camera.moveBackward(Application::GetInstance()->deltaTime);
		}
		ImGui::SameLine();
		ImGui::Text("%.2f%%",Application::GetInstance()->camera.getUIZoom());
		ImGui::SameLine();
		if(ImGui::Button("+##more_zoom")) {
			Application::GetInstance()->camera.moveForward(Application::GetInstance()->deltaTime);
		}
	}
	ImGui::End();
	ImGui::PopStyleVar();
	ImGui::PopStyleVar();
	ImGui::PopStyleVar();
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
			Image::OTSU(thresh, maxValue, Application::GetInstance()->image);
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
			cv::Mat img = Application::GetInstance()->image->imgData;
			Image::GaussianAdaptiveThreshold(thresh, maxValue, Application::GetInstance()->image);
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

		const char* bit_reduction_options[] = {
			"1 bit",
			"2 bits",
			"3 bits",
			"4 bits",
			"5 bits",
			"6 bits",
			"7 bits",
		};

		static int current_bit_reduction = 4;

		ImGui::Combo("##color_bits_reduction", &current_bit_reduction, bit_reduction_options, IM_ARRAYSIZE(bit_reduction_options));

		ImGui::Separator();

		if (ImGui::Button("Apply", ImVec2(80, 30))) {
			cv::Mat img = Application::GetInstance()->image->imgData;
			Image::ColorReduce(current_bit_reduction+1, Application::GetInstance()->image);
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

	// Median cut algorithm modal
	ImGui::SetNextWindowSize(ImVec2(300, 150));
	if (ImGui::BeginPopupModal("Median cut##median_cut"))
	{
		ImGui::Text("Please, set amount of palette colors");

		const char* bit_reduction_options[] = {
			"2 colors",
			"4 colors",
			"8 colors",
			"16 colors",
			"32 colors",
			"64 colors",
			"128 colors",
			"256 colors",
		};

		const int blocks[] = { 1,2,3,4,5,6,7,8 };

		static int current_bit_reduction = 0;

		ImGui::Combo("##median_cut_combo", &current_bit_reduction, bit_reduction_options, IM_ARRAYSIZE(bit_reduction_options));

		if (ImGui::Button("Apply", ImVec2(80, 30))) {
			cv::Mat img = Application::GetInstance()->image->imgData;
			Image::MedianCut(blocks[current_bit_reduction], Application::GetInstance()->image);
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

	// K-means technique modal
	ImGui::SetNextWindowSize(ImVec2(300, 150));
	if (ImGui::BeginPopupModal("K-means Technique##kmean_technique"))
	{
		ImGui::Text("Please, set k");

		static int k = 16;

		ImGui::DragInt("##k_val", &k, 1, 1, 256);

		if (ImGui::Button("Apply", ImVec2(80, 30))) {
			Image::KMeans(k, Application::GetInstance()->image);
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

	// Open Image modal
	ImGui::SetNextWindowSize(ImVec2(300, 150));
	if (ImGui::BeginPopupModal("Open Image##open_image")) {
		ImGui::Text("Color space");
		ImGui::SameLine();
		const char* color_space_options[] = {
			"Inherit", // 0
			"Grayscale", // 1
			"RGB", // 2
			"RGBA", // 3
		};
		static int color_space_option = 0;

		ImGui::Combo("##color_space_options", &color_space_option, color_space_options, IM_ARRAYSIZE(color_space_options));

		ImGui::Text("Image path");
		static char buffer[1024] = "./assets/textures/landscape1.jpg";
		ImGui::InputText("##load_image", buffer, IM_ARRAYSIZE(buffer));
		ImGui::SameLine();
		if (ImGui::Button("...##load_image_btn", ImVec2(60, 20))) {
			strcpy_s(buffer, Application::WindowsPathGetter().c_str());
		}

		if (ImGui::Button("Load", ImVec2(80, 30))) {
			Application::OpenImage(buffer, color_space_option);
			ImGui::CloseCurrentPopup();
			activeModal = "";
		}

		ImGui::SameLine();
		if (ImGui::Button("Cancel", ImVec2(80, 30)))
		{
			strcpy_s(buffer, "./assets/textures/landscape1.jpg");
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

void UI::drawHistograms() {
	switch (Application::GetInstance()->image->channels) {
	case 1:
	{
		const char* histogram_options[] = { "Grayscale" };
		static int histogram_current_option = 0;
		ImGui::Combo("##color_bits_reduction", &histogram_current_option, histogram_options, IM_ARRAYSIZE(histogram_options));
		ImGui::PlotHistogram(
			"##Histogram",
			Plot_ArrayGetter,
			(int*)app->image->histogram[histogram_current_option].data,
			256,
			0,
			"Grayscale",
			0.0f,
			app->image->histogram[histogram_current_option].maxValue, ImVec2(Application::windowWidth * 0.17f, 120.0f));
		break;
	}
	case 3: case 4: {
		const char* histogram_options[] = { "Blue Channel","Green Channel","Red Channel","RGB Channels" };
		const char* histogram_text[] = { "Blue","Green","Red","RGB" };
		static int histogram_current_option = 0;
		ImGui::Combo("##color_bits_reduction", &histogram_current_option, histogram_options, IM_ARRAYSIZE(histogram_options));
		ImGui::PlotHistogram(
			"##Histogram",
			Plot_ArrayGetter,
			(int*)app->image->histogram[histogram_current_option].data,
			256,
			0,
			histogram_text[histogram_current_option],
			0.0f,
			app->image->histogram[histogram_current_option].maxValue, ImVec2(Application::windowWidth * 0.17f, 120.0f));
		break;
	}
	}

}
