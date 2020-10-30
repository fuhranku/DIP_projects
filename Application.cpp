#include "Application.h"
Application* Application::_application = NULL;
unsigned int Application::windowWidth = 1366;
unsigned int Application::windowHeight = 768;
Camera Application::camera(windowWidth, windowHeight);


/**
* Creates an instance of the class
*
* @return the instance of this class
*/
Application* Application::GetInstance() {
    if (!_application)   // Only allow one instance of class to be generated.
        _application = new Application();
    return _application;
}

Application::Application(){}

void Application::InitInstance() {

    if (Init())
    {
        std::cout 
            << "=====================================================" << std::endl
            << "        Press Escape to close the program            " << std::endl
            << "=====================================================" << std::endl;

        //Main Loop
        Update();

        if (imageLoaded) {
            // Deletes the texture from the gpu
            glDeleteTextures(1, &image->id);
            // Deletes the vertex array from the GPU
            glDeleteVertexArrays(1, &image->VAO);
            // Deletes the vertex object from the GPU
            glDeleteBuffers(1, &image->VBO);
        }
        // Destroy the shader
        delete shader;

        // Stops the glfw program
        glfwTerminate();
    }
}


void Application::OnMouseMotion(GLFWwindow* window, double xpos, double ypos)
{
    /*glfwSetCursorPos(window, windowWidth / 2.0, windowHeight / 2.0);
    double xoffset = ((windowWidth / 2.0) - xpos) * camera.mouseSpeed * _application->deltaTime;
    double yoffset = ((windowHeight / 2.0) - ypos) * camera.mouseSpeed * _application->deltaTime;

    camera.mouseUpdate(glm::vec2(xoffset, yoffset));*/
}

void Application::OnMouseButton(GLFWwindow* window, int button, int action, int mods) {
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
    {
        double xpos, ypos;
        // Step 0: 2d Viewport Coordinates
        glfwGetCursorPos(window, &xpos, &ypos);
        
        // Step 1: From Screen Space to Clip Space
        float mouse_pos_x_clip = xpos / (windowWidth * 0.8f) * 2 - 1;
        float mouse_pos_y_clip = ypos / windowHeight * 2 - 1;

        glm::vec4 mouse_pos_near_clip = glm::vec4(mouse_pos_x_clip, mouse_pos_y_clip, -1, 1);
        glm::vec4 mouse_pos_far_clip = glm::vec4(mouse_pos_x_clip, mouse_pos_y_clip, 1, 1);

        // Step 2: Compute inverse view and projection matrices
        glm::mat4 M_wv = camera.getWorldToViewMatrix();
        glm::mat4 M_vc = camera.getOrthoMatrix();

        glm::mat4 M_vw = glm::inverse(M_wv);
        glm::mat4 M_cv = glm::inverse(M_vc);

        // Step 3: From Clip to View Space
        glm::vec4 mouse_pos_near_view = M_cv * mouse_pos_near_clip;
        glm::vec4 mouse_pos_far_view = M_cv * mouse_pos_far_clip;

        // Step 4: From View to World Space
        glm::vec4 mouse_pos_near_world = M_vw * mouse_pos_near_view;
        glm::vec4 mouse_pos_far_world = M_vw * mouse_pos_far_view;

        printf("(%f,%f,%f)\n", mouse_pos_near_world.x
                             , mouse_pos_near_world.y
                             , mouse_pos_near_world.z);

        printf("(%f,%f,%f)\n", mouse_pos_far_world.x
                             , mouse_pos_far_world.y
                             , mouse_pos_far_world.z);


    }
}

void Application::OnKeyPress(GLFWwindow* window, int key, int scancode, int action, int mods) {
    // Move Forward
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) {
        camera.moveForward(Application::GetInstance()->deltaTime);
    }
    // Move Backward
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) {
        camera.moveBackward(Application::GetInstance()->deltaTime);
    }

    if (glfwGetKey(window, GLFW_KEY_O) == GLFW_PRESS && mods == GLFW_MOD_CONTROL) {
        Application::GetInstance()->ui.activeModal = "Open Image##open_image";
    }

    if (glfwGetKey(window, GLFW_KEY_S)            == GLFW_PRESS &&
        glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS &&
        Application::GetInstance()->imageLoaded) {
        Application::UpdateImageOnDisk(Application::GetInstance()->image);
        Application::GetInstance()->ui.activeModal = "Success##saved_modal";
    }

    if (glfwGetKey(window, GLFW_KEY_S)            == GLFW_PRESS &&
        glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS &&
        glfwGetKey(window, GLFW_KEY_LEFT_SHIFT)   == GLFW_PRESS &&
        Application::GetInstance()->imageLoaded) {
        std::string path;
        if (Application::WindowsPathGetter(path, 1)) {
            Application::ExportImage(path.c_str(), Application::GetInstance()->image);
            Application::GetInstance()->ui.activeModal = "Success##saved_modal";
        }
    } 
}

void Application::OnMouseScroll(GLFWwindow* window, double posX, double posY) {
    //std::cout << posX << ", " << posY << std::endl;

    if (posY == 1.0f) {
        camera.moveForward(Application::GetInstance()->deltaTime);
    }
    else if(posY == -1.0f){
        camera.moveBackward(Application::GetInstance()->deltaTime);
    }
}


void Application::ProcessKeyboardInput(GLFWwindow* window)
{
    float MOVEMENT_SPEED = 10.0f;
    float speed = MOVEMENT_SPEED * deltaTime;
    // Checks if the escape key is pressed
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        // Tells glfw to close the window as soon as possible
        glfwSetWindowShouldClose(window, true);

    // Checks if the r key is pressed
    if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS)
    {
        // Reloads the shader
        delete shader;
        shader = new Shader("assets/shaders/basic.vert", "assets/shaders/basic.frag");
    }


    // Move right
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        camera.moveRight(deltaTime);
    }
    // Move left
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        camera.moveLeft(deltaTime);
    }

    // Move Up
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        camera.moveUp(deltaTime);
    }

    // Move Down
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS && 
        glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) != GLFW_PRESS) {
        camera.moveDown(deltaTime);
    }

    if (glfwGetKey(window, GLFW_KEY_SPACE)                  == GLFW_PRESS &&
        glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT)  == GLFW_PRESS &&
        imageLoaded) {

        
        printf("CURSOR: %f , %f \n", currentMousePos.x, currentMousePos.y);

        glm::vec2 dir = glm::vec2(currentMousePos.x - lastMousePos.x,
                                  currentMousePos.y - lastMousePos.y);
        camera.moveDir(deltaTime, dir);

        printf("DIR: %f , %f \n", dir.x, dir.y);
    }
    else {
    }

}

void Application::UpdateCursorPos() {
    double xpos, ypos;
    lastMousePos.x = currentMousePos.x;
    lastMousePos.y = currentMousePos.y;
    glfwGetCursorPos(window, &xpos, &ypos);
    currentMousePos.x = xpos - windowWidth * 0.8f / 2;
    currentMousePos.y = ypos - windowHeight / 2;
}

void Application::CalcDeltaTime() {
    lastTime = currentTime;
    currentTime = glfwGetTime();
    deltaTime = currentTime - lastTime;

    totalFrames++;

    if (totalTime > 1) {
        fps = totalFrames;
        totalFrames = 0;
        totalTime = 0;

        return;
    }

    totalTime += deltaTime;
}


void Application::OpenImage(char* path, int colorSpace) {
    Application* app = Application::GetInstance();
    app->image = new Image(path, colorSpace);

    if (app->image->imgData.empty()) return;
    // Let software know we have a loaded image now
    app->imageLoaded = true;

    // Create Plane with Image Resolution
    Image::BuildPlane(app->image);

    // Initialize grid data
    app->InitGrid();
}

bool Application::WindowsPathGetter(std::string &path, int operation) {
    OPENFILENAME ofn;
    char fileName[MAX_PATH] = "";
    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(OPENFILENAME);
    ofn.hwndOwner = NULL;
    ofn.lpstrFile = fileName;
    ofn.nMaxFile = MAX_PATH;
    ofn.lpstrDefExt = "";
    if (operation == 0) {
        ofn.lpstrFilter =
            "All files *.* \0"
            "*.*\0"
            "JPEG files (*.jpg;*.jpeg;*.jpe)\0"
            "*.jpg;*.jpeg;*.jpe\0"
            "Portable Network Graphics (*.png)\0"
            "*.png\0"
            "Windows bitmaps (*.bmp;*.dib)\0"
            "*.bmp;*.dib\0"
            "TIFF (*.tiff;*.tif)\0"
            "*.tiff;*.tif\0"
            "WebP (*.webp)\0"
            "*.webp\0";
        ofn.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY | OFN_NOCHANGEDIR;
        if (GetOpenFileName(&ofn))
            path = fileName;
        else
            return false;
    }
    else {
    ofn.lpstrFilter = 
        "JPEG files (*.jpg;*.jpeg;*.jpe)\0"
        "*.jpg;*.jpeg;*.jpe\0"
        "Portable Network Graphics (*.png)\0"
        "*.png\0"
        "Windows bitmaps (*.bmp;*.dib)\0"
        "*.bmp;*.dib\0"
        "TIFF (*.tiff;*.tif)\0"
        "*.tiff;*.tif\0"
        "WebP (*.webp)\0"
        "*.webp\0";
        ofn.Flags = OFN_EXPLORER | OFN_OVERWRITEPROMPT | OFN_NOCHANGEDIR;
        if (GetSaveFileName(&ofn))
            path = fileName;
        else
            return false;
    }
    return true; 
}

bool Application::ExportImage(const char* path, Image* image) {
    if (path == "") return false;
    cv::Mat flippedImg = image->imgData.clone();
    cv::flip(flippedImg, flippedImg, 0);
    switch (Application::getExtension(path)) {
    case IMG_JPG:
        cv::imwrite(path, flippedImg, { cv::IMWRITE_JPEG_QUALITY, 95 });
        break;
    case IMG_BMP:
        cv::imwrite(path, flippedImg);
        break;
    case IMG_PNG:
        cv::imwrite(path, flippedImg, { cv::IMWRITE_PNG_COMPRESSION, 4 });
        break;
    case IMG_TIFF:
        cv::imwrite(path, flippedImg, { cv::IMWRITE_TIFF_COMPRESSION });
        break;
    case IMG_WEBP:
        cv::imwrite(path, flippedImg, { cv::IMWRITE_WEBP_QUALITY, 100 });
        break;
    }
    //if (!cv::imwrite(path, flippedImg)) {
    //    std::cout << "Error when saving" << std::endl;
    //    return false;
    //}
    return true;

}

void Application::UpdateImageOnDisk(Image* image) {
    cv::Mat flippedImg = image->imgData.clone();
    cv::flip(flippedImg, flippedImg, 0);
    cv::imwrite(image->path, flippedImg);
}

int Application::getExtension(const char* path) {
    // Get extension from path string
    std::string ext = std::string(path).substr(std::string(path).find_last_of('.') + 1);
    if (ext == "jpeg" || ext == "jpg" || ext == "jpe") return IMG_JPG;
    if (ext == "png") return IMG_PNG;
    if (ext == "bmp"  || ext == "dib") return IMG_BMP;
    if (ext == "tiff" || ext == "tif") return IMG_TIFF;
    if (ext == "webp" ) return IMG_WEBP;
}

/**
* Render Function
* */
void Application::Render() {

    // Clears the color and depth buffers from the frame buffer
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    /** Draws code goes here **/
    // Use the shader
    drawImage();

    drawGrid();

    ui.mainDraw();

    // Swap the buffer
    glfwSwapBuffers(window);

    //Process Keyboard Input
    ProcessKeyboardInput(window);
    
}

void Application::drawImage() {
    if (!imageLoaded) return;

    shader->use();
    shader->setInt("image", 0);
    shader->setInt("channels", image->channels);

    shader->setMat4("view", camera.viewMatrix);
    shader->setMat4("projection", camera.getOrthoMatrix());
    // Bind textures into GPU
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, image->id);
    // Binds the vertex array to be drawn
    glBindVertexArray(image->VAO);
    // Renders the triangle gemotry
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindVertexArray(0);
}

void Application::drawGrid() {

    if (!imageLoaded) return;

    if (camera.getUIZoom() < 1000.0f) return;

    gridShader->use();
    gridShader->setMat4("model", glm::mat4(1.0f));
    gridShader->setMat4("view", camera.viewMatrix);
    gridShader->setMat4("projection", camera.getOrthoMatrix());

    glBindVertexArray(gridVAO);

    glDrawElements(GL_LINES, gridLength, GL_UNSIGNED_INT, NULL);

    glBindVertexArray(0);

}


/**
* Initialize everything
* @returns{bool} true if everything goes ok
* */
bool Application::Init() {
    // Initialize the window, and the glad components
    if (!InitWindow() || !InitGlad())
        return false;

    // Initialize the opengl context
    InitGL();

    // Init user interface
    ui.init(window);

    // Load Image Shader
    shader = new Shader("assets/shaders/basic.vert", "assets/shaders/basic.frag");
    // Load Grid Shader
    gridShader = new Shader("assets/shaders/gridShader.vert", "assets/shaders/gridShader.frag");

    return true;
}

/**
 * App main loop
 * */
void Application::Update() {
    // Loop until something tells the window, that it has to be closed
    while (!glfwWindowShouldClose(window))
    {
        // Update Cursor Position
        UpdateCursorPos();

        // Checks for keyboard inputs
        ProcessKeyboardInput(window);

        // Calculate Delta Time
        CalcDeltaTime();

        // Renders everything
        Render();

        // Check and call events
        glfwPollEvents();
    }
}

void Application::InitGrid() {
    std::vector<glm::vec3> vertices;
    std::vector<glm::uvec4> indices;

    for (int j = -(image->height /2); j <= image->height /2; ++j) {
        for (int i = -(image->width /2); i <= image->width /2; ++i) {
            float x = (float)i;
            float y = (float)j;
            vertices.push_back(glm::vec3(x, y, 0.015));
        }
    }

    for (int j = 0; j < image->height; ++j) {
        for (int i = 0; i < image->width; ++i) {

            int row1 = j * (image->width + 1);
            int row2 = (j + 1) * (image->width + 1);

            indices.push_back(glm::uvec4(row1 + i, row1 + i + 1, row1 + i + 1, row2 + i + 1));
            indices.push_back(glm::uvec4(row2 + i + 1, row2 + i, row2 + i, row1 + i));

        }
    }

    glGenVertexArrays(1, &gridVAO);
    glBindVertexArray(gridVAO);
    glGenBuffers(1, &gridVBO);
    glBindBuffer(GL_ARRAY_BUFFER, gridVBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), &vertices[0], GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

    glGenBuffers(1, &gridIBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gridIBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(glm::uvec4), &indices[0], GL_STATIC_DRAW);

    glBindVertexArray(0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    gridLength = (GLuint)indices.size() * 4;
}

/**
* Handles the window resize
* @param{GLFWwindow} window pointer
* @param{int} new width of the window
* @param{int} new height of the window
* */
void Application::Resize(GLFWwindow* window, int width, int height)
{
    windowWidth = width;
    windowHeight = height;

    // Sets the OpenGL viewport size and position
    glViewport(0, 0, windowWidth*0.8f, windowHeight);
}



/**
* Initialize the glfw library
* @returns{bool} true if everything goes ok
* */
bool Application::InitWindow() {
    // Initialize glfw
    glfwInit();
    // Sets the Opegl context to Opengl 3.0
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Creates the window
    window = glfwCreateWindow(windowWidth, windowHeight, windowTitle, NULL, NULL);

    // The window couldn't be created
    if (!window)
    {
        std::cout << "Failed to create the glfw window" << std::endl;
        glfwTerminate(); // Stops the glfw program
        return false;
    }

    // Creates the glfwContext, this has to be made before calling initGlad()
    glfwMakeContextCurrent(window);

    // Window resize callback
    glfwSetFramebufferSizeCallback(window, Resize);

    // Mouse position callback
    glfwSetCursorPosCallback(window, OnMouseMotion);
    // Mouse buttons callback
    glfwSetMouseButtonCallback(window, OnMouseButton);
    // Keyboard buttons callback
    glfwSetKeyCallback(window, OnKeyPress);

    glfwSetScrollCallback(window, OnMouseScroll);

    return true;
}

/**
* Initialize the glad library
* @returns{bool} true if everything goes ok
* */
bool Application::InitGlad() {
    // Initialize glad
    int status = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
    // If something went wrong during the glad initialization
    if (!status)
    {
        std::cout << status << std::endl;
        std::cout << "Failed to initialize GLAD" << std::endl;
        return false;
    }
    return true;
}


/**
 * Initialize the opengl context
 * */
void Application::InitGL()
{
    // Enables the z-buffer test
    glEnable(GL_DEPTH_TEST);
    // Sets the ViewPort
    glViewport(0, 0, windowWidth*0.8f, windowHeight);
    // Sets the clear color
    glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
}

