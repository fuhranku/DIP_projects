#include "Application.h"


Application *Application::GetInstance() {
    /**
     * This is a safer way to create an instance. instance = new Singleton is
     * dangeruous in case two instance threads wants to access at the same time
     */
    if (_application == nullptr) {
        _application = new Application();
    }
    return _application;
}

Application::Application() {

    if (Init())
    {
        std::cout << "=====================================================" << std::endl
            << "        Press Escape to close the program            " << std::endl
            << "=====================================================" << std::endl;

        //Main Loop
        Update();

        // Deletes the texture from the gpu
        glDeleteTextures(1, &textureID);
        // Deletes the vertex array from the GPU
        glDeleteVertexArrays(1, &VAO);
        // Deletes the vertex object from the GPU
        glDeleteBuffers(1, &VBO);
        // Destroy the shader
        delete shader;

        // Stops the glfw program
        glfwTerminate();
    }
}

/**
* Render Function
* */
void Application::Render() {

    // Clears the color and depth buffers from the frame buffer
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    /** Draws code goes here **/
    // Use the shader
    shader->use();
    // Binds the vertex array to be drawn
    glBindVertexArray(VAO);
    // Renders the triangle gemotry
    glDrawArrays(GL_TRIANGLES, 0, 3);
    glBindVertexArray(0);

    ui.draw();

    // Swap the buffer
    glfwSwapBuffers(window);
    
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

    // Loads the shader
    shader = new Shader("assets/shaders/basic.vert", "assets/shaders/basic.frag");
    // Loads all the geometry into the GPU
    BuildGeometry();
    // Loads the texture into the GPU
    textureID = LoadTexture("assets/textures/bricks2.jpg");

    return true;
}

/**
* Process the keyboard input
* There are ways of implementing this function through callbacks provide by
* the GLFW API, check the GLFW documentation to find more
* @param{GLFWwindow} window pointer
* */
void Application::ProcessKeyboardInput(GLFWwindow* window) {
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

        // Renders everything
        Render();

        // Check and call events
        glfwPollEvents();
    }
}

/**
* Handles the window resize
* @param{GLFWwindow} window pointer
* @param{int} new width of the window
* @param{int} new height of the window
* */
//void Application::Resize(GLFWwindow* window, int width, int height)
//{
//    windowWidth = width;
//    windowHeight = height;
//    // Sets the OpenGL viewport size and position
//    glViewport(0, 0, windowWidth, windowHeight);
//}



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
    //glfwSetFramebufferSizeCallback(window, Resize);
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
    glViewport(0, 0, windowWidth, windowHeight);
    // Sets the clear color
    glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
}


/**
 * Builds all the geometry buffers and
 * loads them up into the GPU
 * (Builds a simple triangle)
 * */
void Application::BuildGeometry(){
    float triangleVertices[] = {
        // Bottom left vertex
        -0.5f, -0.5f, 0.0f, // Position
        1.0f, 0.0f, 0.0f,   // Color
        // Bottom right vertex
        0.5f, -0.5f, 0.0f, // Position
        0.0f, 1.0f, 0.0f,  // Color
        // Top Center vertex
        0.0f, 0.5f, 0.0f, // Position
        0.0f, 0.0f, 1.0f  // Color

    };
    // Creates on GPU the vertex array
    glGenVertexArrays(1, &VAO);
    // Creates on GPU the vertex buffer object
    glGenBuffers(1, &VBO);
    // Binds the vertex array to set all the its properties
    glBindVertexArray(VAO);
    // Sets the buffer geometry data
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(triangleVertices), &triangleVertices, GL_STATIC_DRAW);

    // Sets the vertex attributes
    // Position
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    // Color
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glBindVertexArray(0);
}



/**
 * Loads a texture into the GPU
 * @param{const char} path of the texture file
 * @returns{unsigned int} GPU texture index
 * */
unsigned int Application::LoadTexture(const char* path)
{
    unsigned int id;
    // Creates the texture on GPU
    glGenTextures(1, &id);
    // Loads the texture
    int textureWidth, textureHeight, numberOfChannels;
    // Flips the texture when loads it because in opengl the texture coordinates are flipped
    stbi_set_flip_vertically_on_load(true);
    // Loads the texture file data
    unsigned char* data = stbi_load(path, &textureWidth, &textureHeight, &numberOfChannels, 0);
    if (data)
    {
        // Gets the texture channel format
        GLenum format;
        switch (numberOfChannels)
        {
        case 1:
            format = GL_RED;
            break;
        case 3:
            format = GL_RGB;
            break;
        case 4:
            format = GL_RGBA;
            break;
        }

        // Binds the texture
        glBindTexture(GL_TEXTURE_2D, id);
        // Creates the texture
        glTexImage2D(GL_TEXTURE_2D, 0, format, textureWidth, textureHeight, 0, format, GL_UNSIGNED_BYTE, data);
        // Creates the texture mipmaps
        glGenerateMipmap(GL_TEXTURE_2D);

        // Set the filtering parameters
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    }
    else
    {
        std::cout << "ERROR:: Unable to load texture " << path << std::endl;
        glDeleteTextures(1, &id);
    }
    // We dont need the data texture anymore because is loaded on the GPU
    stbi_image_free(data);

    return id;
}

