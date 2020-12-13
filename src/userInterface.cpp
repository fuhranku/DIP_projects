#include "userInterface.h"
#include <ImGuizmo.h>
#include "Application.h"

Application* app;

float thresh = 0.0f, maxValue = 255.0f;



UI::UI() {
	floodFill_color = cv::Scalar(0.89019f, 0.06274f, 0.83529f);
	loDiff = glm::ivec4(5, 5, 5, 5);
	upDiff = glm::ivec4(5, 5, 5, 5);
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
				ImGui::Text("Resolution: %ix%i", Application::GetInstance()->image->width, Application::GetInstance()->image->height);
				ImGui::Text("Size: %i %s", Application::GetInstance()->image->size.val, Application::GetInstance()->image->size.unit.c_str());
				ImGui::Text("DPI: %i", Application::GetInstance()->image->dpi);
				ImGui::Text("File extension: %s", Application::GetInstance()->image->ext.str_name.c_str());
				ImGui::Separator();
				ImGui::Text("Histogram information: ");
				drawHistograms();
				ImGui::TreePop();
			}
			ImGui::Dummy(ImVec2(0, 10.0f));
			ImGui::Separator();
			ImGui::Dummy(ImVec2(0, 10.0f));
			ImGui::Checkbox("Flood fill", &flood_fill_bool);
			if(flood_fill_bool){
				ImGui::SetNextItemOpen(true);
				ImGui::Text("Configuration");
				ImGui::Indent();
				ImGui::Text("Range type: ");
				ImGui::RadioButton("Fixed##fixed_range", &floodfill_range_selected, 1);
				ImGui::SameLine();
				ImGui::RadioButton("Floating##floating_range", &floodfill_range_selected, 0);
				ImGui::Text("Neighborhood: ");
				const char* nhbrhd_options[] = {
					"4 elements", // 0
					"8 elements", // 1
				};
				ImGui::Combo("##nhbrhd_elements_count", &nhbrhd_elements_count, nhbrhd_options, IM_ARRAYSIZE(nhbrhd_options));
				ImGui::Text("Low diff: ");
				ImGui::DragInt4("##lo_diff", &loDiff[0], 0.05f, 0, 255);
				ImGui::Text("Up Diff: ");
				ImGui::DragInt4("##up_diff", &upDiff[0], 0.05f, 0, 255);
				ImGui::Dummy(ImVec2(0, 10.0f));
				ImGui::Text("Flooding color: ");
				static float color[] = { floodFill_color[0],floodFill_color[1],floodFill_color[2]};
				ImGui::ColorPicker3("##color_picker", &color[0]);
				floodFill_color = cv::Scalar(
					color[0],
					color[1],
					color[2]
				);

			}
			ImGui::Dummy(ImVec2(0, 10.0f));
			ImGui::Separator();
			ImGui::Dummy(ImVec2(0, 10.0f));
			ImGui::Checkbox("Brush (experimental)", &paint_mode);
			if (paint_mode) {
				ImGui::SetNextItemOpen(true);
				ImGui::Text("Brush color: ");
				static float brushColor[] = { brush_color[0],brush_color[1],brush_color[2] };
				ImGui::ColorPicker3("##color_picker", &brushColor[0]);
				DIPlib::SetBrushColor(
					cv::Scalar(
						brushColor[2],
						brushColor[1],
						brushColor[0]
					)
				);

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
			//if (ImGui::MenuItem("New Tab")) {
			//}
			if (ImGui::MenuItem("Open Image","ctrl + O")) {
				activeModal = "Open Image##open_image";
				History::_redoStack = std::stack<Action*>();
				History::_undoStack = std::stack<Action*>();
			}
			pushDisable(!Application::GetInstance()->imageLoaded);
			if (ImGui::MenuItem("Save","ctrl + S")) {
				activeModal = "Confirm overwrite##confirm_overwrite_save";
			}
			popDisable(!Application::GetInstance()->imageLoaded);
			pushDisable(!Application::GetInstance()->imageLoaded);
			if (ImGui::MenuItem("Export As", "ctrl + shift + S")) {
				std::string path;
				if (Application::WindowsPathGetter(path, 1)) {
					Application::ExportImage(path.c_str(),Application::GetInstance()->image);
					activeModal = "Success##saved_modal";
				}
			}
			popDisable(!Application::GetInstance()->imageLoaded);
			if (ImGui::MenuItem("Help...", "ctrl + H")) {
				activeModal = "Help##help_modal";
			}

			if (ImGui::MenuItem("About...")) {
				activeModal = "About##about_modal";
			}
			ImGui::EndMenu();
		}
		pushDisable(!Application::GetInstance()->imageLoaded);
		if (ImGui::BeginMenu("Filter")) {
			pushDisable(Application::GetInstance()->image->freqComputed);
			if (ImGui::BeginMenu("Thresholding")) {
				if (ImGui::MenuItem("OTSU")) {
					activeModal = "OTSU Threshold##otsu_modal";
				}
				if (ImGui::MenuItem("Adaptive Threshold")) {
					activeModal = "Gaussian Adaptive Threshold##adaptive_modal";
				}
				ImGui::EndMenu();
			}
			popDisable(Application::GetInstance()->image->freqComputed);
			pushDisable(Application::GetInstance()->image->freqComputed);
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
			popDisable(Application::GetInstance()->image->freqComputed);
			ImGui::Separator();

			bool freqComputed = Application::GetInstance()->image->freqComputed;
			pushDisable(!freqComputed);
			if (ImGui::BeginMenu("Signals")) {
				if (ImGui::MenuItem("Low pass")) {
					DIPlib::FourierFilter(
						Application::GetInstance()->image,
						IMG_FOURIER_LOW_PASS,
						30
					);
				}
				if (ImGui::MenuItem("High pass")) {
					DIPlib::FourierFilter(
						Application::GetInstance()->image,
						IMG_FOURIER_HIGH_PASS,
						30
					);
				}
				ImGui::EndMenu();
			}
			popDisable(!freqComputed);
			ImGui::EndMenu();

		}
		popDisable(!Application::GetInstance()->imageLoaded);

		pushDisable(!Application::GetInstance()->imageLoaded);
		if (ImGui::BeginMenu("Image"))
		{
			if (ImGui::MenuItem("Image Translation")) {
				activeModal = "Translate image##trans_image_modal";
			}
			if (ImGui::BeginMenu("Image Rotation")) {
				if (ImGui::MenuItem("180 degree")) {
					// Save current Action on history
					Application::GetInstance()->image->currentFilter = IMG_TRANSFORM_ROTATE;
					History::PushAction(Application::GetInstance()->image);
					// Perform Action
					DIPlib::Rotate(Application::GetInstance()->image, 180);
				}
				if (ImGui::MenuItem("90 degree Clockwise")) {
					// Save current Action on history
					Application::GetInstance()->image->currentFilter = IMG_TRANSFORM_ROTATE;
					History::PushAction(Application::GetInstance()->image);
					// Perform Action
					DIPlib::Rotate(Application::GetInstance()->image, 90);
				}
				if (ImGui::MenuItem("90 degree Counter Clockwise")) {
					// Save current Action on history
					Application::GetInstance()->image->currentFilter = IMG_TRANSFORM_ROTATE;
					History::PushAction(Application::GetInstance()->image);
					// Perform Action
					DIPlib::Rotate(Application::GetInstance()->image, -90);
				}
				if (ImGui::MenuItem("Arbitrary...")) {
					activeModal = "Rotate Canvas##arb_rot_modal";
				}

				ImGui::Separator();

				if (ImGui::MenuItem("Flip Canvas Horizontal")) {
					// Save current Action on history
					Application::GetInstance()->image->currentFilter = IMG_TRANSFORM_FLIP;
					History::PushAction(Application::GetInstance()->image);
					// Perform Action
					DIPlib::Flip(Application::GetInstance()->image, 1);
				}
				if (ImGui::MenuItem("Flip Canvas Vertical")) {
					// Save current Action on history
					Application::GetInstance()->image->currentFilter = IMG_TRANSFORM_FLIP;
					History::PushAction(Application::GetInstance()->image);
					// Perform Action
					DIPlib::Flip(Application::GetInstance()->image, 0);
				}

				ImGui::EndMenu();
			}

			if (ImGui::MenuItem("Equalize Histogram")) {
				// Save current Action on history
				Application::GetInstance()->image->currentFilter = IMG_HISTOGRAM_EQUALIZATION;
				History::PushAction(Application::GetInstance()->image);
				// Perform Action
				DIPlib::EqualizeHist(Application::GetInstance()->image);
			}
			ImGui::Separator();
			bool freqComputed = Application::GetInstance()->image->freqComputed;
			pushDisable(freqComputed);
			if (ImGui::MenuItem("Switch to Frequency Domain")) {
				// Save current Action on history
				Application::GetInstance()->image->currentFilter = IMG_FOURIER_TRANSFORM;
				History::PushAction(Application::GetInstance()->image);
				// Perform Action
				DIPlib::DFT(Application::GetInstance()->image);
			}
			popDisable(freqComputed);
			pushDisable(!freqComputed);
			if (ImGui::MenuItem("Switch to Spatial Domain")) {
				// Save current Action on history
				Application::GetInstance()->image->currentFilter = IMG_FOURIER_TRANSFORM_INVERSE;
				History::PushAction(Application::GetInstance()->image);
				// Perform Action
				DIPlib::IDFT(Application::GetInstance()->image);
			}
			popDisable(!freqComputed);

			if (ImGui::MenuItem("Morphology")) {
				activeModal = "Morphology##morphology_modal";
			}


			ImGui::EndMenu();
		}
		popDisable(!Application::GetInstance()->imageLoaded);

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
		ImGui::SameLine();
		ImGui::Dummy(ImVec2(100.0f, 0.0f));
		ImGui::SameLine();
		bool stackBool = History::GetInstance()->_undoStack.size() == 0;
		pushDisable(stackBool);
		if (ImGui::ArrowButton("##Undo", ImGuiDir_Left)) {
			History::_undo(Application::GetInstance()->image);
		}
		popDisable(stackBool);
		ImGui::SameLine();
		stackBool = History::GetInstance()->_redoStack.size() == 0;
		pushDisable(stackBool);
		if (ImGui::ArrowButton("##Redo", ImGuiDir_Right)) {
			History::_redo(Application::GetInstance()->image);
		}
		popDisable(stackBool);

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
		//ImGui::Text("Please, set filter values");

		//ImGui::Text("Thresh");
		//ImGui::DragFloat("Thresh", &thresh, 0.05f, 0.0f, 255.0f, "%.2f");

		//ImGui::Text("Max Value");
		//ImGui::DragFloat("Max Value", &maxValue, 0.05f, 0.0f, 255.0f, "%.2f");


		//if (ImGui::Button("Apply", ImVec2(80, 30))) {
		//	// Save current Action on history
		//	Application::GetInstance()->image->currentFilter = IMG_THRESHOLD_OTSU;
		//	History::PushAction(Application::GetInstance()->image);
		//	// Perform Action
		//	DIPlib::OTSU(thresh, maxValue, Application::GetInstance()->image);
		//	ImGui::CloseCurrentPopup();
		//	activeModal = "";
		//}

		//ImGui::SameLine();
		//if (ImGui::Button("Cancel", ImVec2(80, 30)))
		//{
		//	ImGui::CloseCurrentPopup();
		//	activeModal = "";
		//}
		//ImGui::EndPopup();

		// Save current Action on history
		Application::GetInstance()->image->currentFilter = IMG_THRESHOLD_OTSU;
		History::PushAction(Application::GetInstance()->image);
		// Perform Action
		DIPlib::OTSU(125, 255, Application::GetInstance()->image);
		ImGui::CloseCurrentPopup();
		activeModal = "";

		ImGui::EndPopup();
	}

	// Gaussian Adaptive Thresholding modal
	ImGui::SetNextWindowSize(ImVec2(300, 150));
	if (ImGui::BeginPopupModal("Gaussian Adaptive Threshold##adaptive_modal"))
	{
		//ImGui::Text("Please, set filter values");

		//ImGui::Text("Thresh");
		//ImGui::DragFloat("Thresh", &thresh, 0.05f, 0.0f, 255.0f, "%.2f");

		//ImGui::Text("Max Value");
		//ImGui::DragFloat("Max Value", &maxValue, 0.05f, 0.0f, 255.0f, "%.2f");

		//if (ImGui::Button("Apply", ImVec2(80, 30))) {
		//	// Save current Action on history
		//	Application::GetInstance()->image->currentFilter = IMG_THRESHOLD_GAUSSIAN;
		//	History::PushAction(Application::GetInstance()->image);
		//	// Perform Action
		//	DIPlib::GaussianAdaptiveThreshold(thresh, maxValue, Application::GetInstance()->image);
		//	ImGui::CloseCurrentPopup();
		//	activeModal = "";
		//}

		//ImGui::SameLine();
		//if (ImGui::Button("Cancel", ImVec2(80, 30)))
		//{
		//	ImGui::CloseCurrentPopup();
		//	activeModal = "";
		//}
		//ImGui::EndPopup();


		// Save current Action on history
		Application::GetInstance()->image->currentFilter = IMG_THRESHOLD_GAUSSIAN;
		History::PushAction(Application::GetInstance()->image);
		// Perform Action
		DIPlib::GaussianAdaptiveThreshold(127, 255, Application::GetInstance()->image);
		ImGui::CloseCurrentPopup();
		activeModal = "";

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
			// Save current Action on history
			Application::GetInstance()->image->currentFilter = IMG_QUANTIZATION_BIT_REDUCTION;
			History::PushAction(Application::GetInstance()->image);
			// Apply filter
			DIPlib::ColorReduce(current_bit_reduction+1, Application::GetInstance()->image);
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
			// Save current Action on history
			Application::GetInstance()->image->currentFilter = IMG_QUANTIZATION_MEDIAN_CUT;
			History::PushAction(Application::GetInstance()->image);
			// Perform Action
			DIPlib::MedianCut(blocks[current_bit_reduction], Application::GetInstance()->image);
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

		static int k = 4;

		ImGui::DragInt("##k_val", &k, 1, 1, 256);

		if (ImGui::Button("Apply", ImVec2(80, 30))) {
			// Save current Action on history
			Application::GetInstance()->image->currentFilter = IMG_QUANTIZATION_K_MEANS;
			History::PushAction(Application::GetInstance()->image);
			// Perform Action
			DIPlib::KMeans(k, Application::GetInstance()->image);
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
			std::string path;
			if (Application::WindowsPathGetter(path, 0))
				strcpy_s(buffer, path.c_str());
		}

		if (ImGui::Button("Load", ImVec2(80, 30))) {
			Application::OpenImage(buffer, color_space_option);
			ImGui::CloseCurrentPopup();
			if (!Application::GetInstance()->imageLoaded) {
				strcpy_s(buffer, "./assets/textures/landscape1.jpg");
				activeModal = "Error##error_modal";
			}
			else
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

	// Confirm Overwrite modal
	ImGui::SetNextWindowSize(ImVec2(300, 150));
	if (ImGui::BeginPopupModal("Confirm overwrite##confirm_overwrite_save")) {
		ImGui::Text("Do you want to overwrite your image?");
		if (ImGui::Button("Confirm", ImVec2(80, 30))) {
			ImGui::CloseCurrentPopup();
			Application::UpdateImageOnDisk(Application::GetInstance()->image);
			activeModal = "";
		}
		if (ImGui::Button("Cancel", ImVec2(80, 30)))
		{
			ImGui::CloseCurrentPopup();
			activeModal = "";
		}
		ImGui::EndPopup();
	}
	// Success operation modal
	ImGui::SetNextWindowSize(ImVec2(150, 100));
	if (ImGui::BeginPopupModal("Success##saved_modal")) {
		ImGui::Text("Success on\nlast operation!");
		if (ImGui::Button("OK", ImVec2(80, 30))) {
			ImGui::CloseCurrentPopup();
			activeModal = "";
		}
		ImGui::EndPopup();
	}


	// Error operation modal
	ImGui::SetNextWindowSize(ImVec2(150, 100));
	if (ImGui::BeginPopupModal("Error##error_modal")) {
		ImGui::Text("Error on\nlast operation");
		if (ImGui::Button("OK", ImVec2(80, 30))) {
			ImGui::CloseCurrentPopup();
			activeModal = "";
		}
		ImGui::EndPopup();
	}

	// About modal
	ImGui::SetNextWindowSize(ImVec2(350, 150));
	if (ImGui::BeginPopupModal("About##about_modal")) {
		ImGui::Text(
			"Developed by: \n"
			"		Jose Tirado: @jet29\n"
			"		Frank Ponte: @fuhranku\n"
			"Follow us on Github!\n"
			"Universidad Central de Venezuela - 2020."

		);
		if (ImGui::Button("OK", ImVec2(80, 30))) {
			ImGui::CloseCurrentPopup();
			activeModal = "";
		}
		ImGui::EndPopup();
	}

	// About modal
	ImGui::SetNextWindowSize(ImVec2(400, 220));
	if (ImGui::BeginPopupModal("Help##help_modal")) {
		ImGui::Text(
			"Availables Hotkeys: \n"
			"Please, remember to check our tooltip on the\ntop menu\n"
			"ctrl + O -> Open new file\n"
			"ctrl + S -> Overwrite current image\n"
			"ctrl + shift + s -> Export image\n"
			"ctrl + Z -> Undo action\n"
			"ctrl + shift + Z -> Redo action\n"
			"ctrl + H -> Show Help window\n"
			"WASD or Space and left mouse click pressed -> Panning\n"
			"QE or mouse scroll -> Zoom\n"

		);
		if (ImGui::Button("OK", ImVec2(80, 30))) {
			ImGui::CloseCurrentPopup();
			activeModal = "";
		}
		ImGui::EndPopup();
	}

	

	// Arbitrary rotation modal
	ImGui::SetNextWindowSize(ImVec2(300, 150));
	if (ImGui::BeginPopupModal("Rotate Canvas##arb_rot_modal"))
	{
		static float deg = 90;
		ImGui::Text("Angle");
		ImGui::SameLine();
		ImGui::DragFloat("##angle", &deg, 0.005f, -359.99f, 359.99f, "%.2f");


		static int label = 1;
		ImGui::RadioButton("Clockwise##cw", &label, 1);
		ImGui::RadioButton("Counter\nClockwise##ccw", &label, -1);

		if (ImGui::Button("OK", ImVec2(80, 30))) {
			// Save current Action on history
			Application::GetInstance()->image->currentFilter = IMG_TRANSFORM_ROTATE;
			History::PushAction(Application::GetInstance()->image);
			// Perform Action
			DIPlib::Rotate(Application::GetInstance()->image, deg*label);
			ImGui::CloseCurrentPopup();
			activeModal = "";
		}
		ImGui::SameLine();
		if (ImGui::Button("Cancel", ImVec2(80, 30))) {
			ImGui::CloseCurrentPopup();
			activeModal = "";
		}

		ImGui::EndPopup();
	}

	// image translation modal
	ImGui::SetNextWindowSize(ImVec2(300, 150));
	if (ImGui::BeginPopupModal("Translate image##trans_image_modal"))
	{
		static int dx = 10;
		ImGui::Text("Dx");
		ImGui::SameLine();
		ImGui::DragInt("##trans_dx", &dx, 0.05f, -5000, 5000);

		static int dy = 10;
		ImGui::Text("Dy");
		ImGui::SameLine();
		ImGui::DragInt("##trans_dy", &dy, 0.05f, -5000, 5000);


		if (ImGui::Button("OK", ImVec2(80, 30))) {
			// Save current Action on history
			Application::GetInstance()->image->currentFilter = IMG_TRANSFORM_TRANSLATE;
			History::PushAction(Application::GetInstance()->image);
			// Perform Action
			DIPlib::Translate(Application::GetInstance()->image, dx,dy);
			ImGui::CloseCurrentPopup();
			activeModal = "";
		}
		ImGui::SameLine();
		if (ImGui::Button("Cancel", ImVec2(80, 30))) {
			ImGui::CloseCurrentPopup();
			activeModal = "";
		}

		ImGui::EndPopup();
	}

	// Morphology
	ImGui::SetNextWindowSize(ImVec2(300, 150));
	if (ImGui::BeginPopupModal("Morphology##morphology_modal"))
	{
		const char* morphologyOptions[] = {
		"Erosion",
		"Dilation",
		"Open",
		"Close"
		};

		const cv::MorphTypes morphologyInt[] = {
			cv::MORPH_ERODE,
			cv::MORPH_DILATE,
			cv::MORPH_OPEN,
			cv::MORPH_CLOSE
		};

		static int currentMorphologyOption = 0;
		ImGui::Combo("##morphology_type", &currentMorphologyOption, morphologyOptions, IM_ARRAYSIZE(morphologyOptions));

		//structuring element type
		const char* elementOptions[] = {
		"Rect",
		"Cross",
		"Ellipse",
		"Arbitrary",
		};

		const int elementInt[] = {
			cv::MORPH_RECT,
			cv::MORPH_CROSS,
			cv::MORPH_ELLIPSE,
			3
		};

		static int currentElementOption = 0;
		ImGui::Combo("##element_type", &currentElementOption, elementOptions, IM_ARRAYSIZE(elementOptions));

		//Kernel Size
		static int kernelSize = 3;
		ImGui::Text("Kernel size: ");
		ImGui::DragInt("##kernel_morph", &kernelSize, 2, 3, 101);
		kernelSize % 2 == 0 ? kernelSize++ : kernelSize;

		cv::Mat element;
		if (currentElementOption == 3) {
			//User write it's own structuring element
			element = cv::Mat();
		}

		if (ImGui::Button("OK", ImVec2(80, 30))) {

			switch (currentMorphologyOption) {
				case 0:
					Application::GetInstance()->image->currentFilter = IMG_MORPHOLOGY_OPEN;
					break;
				case 1:
					Application::GetInstance()->image->currentFilter = IMG_MORPHOLOGY_CLOSE;
					break;
				case 2:
					Application::GetInstance()->image->currentFilter = IMG_MORPHOLOGY_EROSION;
					break;
				case 3:
					Application::GetInstance()->image->currentFilter = IMG_MORPHOLOGY_DILATION;
					break;
			}
			
			History::PushAction(Application::GetInstance()->image);

			// Perform Action
			DIPlib::Morphology(
				Application::GetInstance()->image,
				morphologyInt[currentMorphologyOption],
				elementInt[currentElementOption],
				kernelSize,
				element);


			ImGui::CloseCurrentPopup();
			activeModal = "";
		}
		ImGui::SameLine();
		if (ImGui::Button("Cancel", ImVec2(80, 30))) {
			ImGui::CloseCurrentPopup();
			activeModal = "";
		}

		ImGui::EndPopup();
	}
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

void UI::pushDisable(bool condition) {
	if (condition) {
		ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
		ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
	}
}

void UI::popDisable(bool condition) {
	if (condition)
	{
		ImGui::PopItemFlag();
		ImGui::PopStyleVar();
	}
}

void UI::terminate() {
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
}
