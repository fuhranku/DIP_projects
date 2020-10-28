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

        // Deletes the texture from the gpu
        glDeleteTextures(1, &image->id);
        // Deletes the vertex array from the GPU
        glDeleteVertexArrays(1, &image->VAO);
        // Deletes the vertex object from the GPU
        glDeleteBuffers(1, &image->VBO);
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
    //placeholder
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
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        camera.moveDown(deltaTime);
    }
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


    //drawGrid();

    ui.draw();

    // Swap the buffer
    glfwSwapBuffers(window);

    //Process Keyboard Input
    ProcessKeyboardInput(window);
    
}

void Application::drawGrid() {

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


    // Loads the texture into the GPU
    image = new Image("assets/textures/landscape1.jpg");
    // Create Plane with Image Resolution
    image->BuildPlane();

    // Initialize grid data
    InitGrid();

    return true;
}

/**
 * App main loop
 * */
void Application::Update() {
    // Loop until something tells the window, that it has to be closed
    while (!glfwWindowShouldClose(window))
    {
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

