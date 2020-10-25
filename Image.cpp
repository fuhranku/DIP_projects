#include "Image.h"

Image::Image(const char *path) {
    //// Creates the texture on GPU
    //glGenTextures(1, &id);
    //// Flips the texture when loads it because in opengl the texture coordinates are flipped
    //stbi_set_flip_vertically_on_load(true);
    //// Loads the texture file data
    //unsigned char* data = stbi_load(path, &width, &height, &channels, 0);
    //
    //if (data)
    //{
    //    // Gets the texture channel format
    //    GLenum format;
    //    switch (channels)
    //    {
    //    case 1:
    //        format = GL_RED;
    //        break;
    //    case 3:
    //        format = GL_RGB;
    //        break;
    //    case 4:
    //        format = GL_RGBA;
    //        break;
    //    }

    //    // Binds the texture
    //    glBindTexture(GL_TEXTURE_2D, id);
    //    // Creates the texture
    //    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
    //    // Creates the texture mipmaps
    //    glGenerateMipmap(GL_TEXTURE_2D);

    //    // Set the filtering parameters
    //    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    //    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    //    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    //    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    //}
    //else
    //{
    //    std::cout << "ERROR:: Unable to load texture " << path << std::endl;
    //    glDeleteTextures(1, &id);
    //}
    //// We dont need the data texture anymore because is loaded on the GPU
    //stbi_image_free(data);

    // Creates the texture on GPU
    glGenTextures(1, &id);

    cv::Mat imgBGR = cv::imread(path);
    cv::flip(imgBGR, imgBGR, 0);
    //cv::Mat imgRGB;
    //cv::cvtColor(imgBGR, imgRGB, cv::COLOR_BGR2RGB);

    //cv::imshow("test", imgBGR);

    // Gets the texture channel format
    GLenum format;
    switch (imgBGR.channels())
    {
    case 1:
        format = GL_BLUE;
        break;
    case 3:
        format = GL_BGR;
        break;
    case 4:
        format = GL_BGRA;
        break;
    }

    width = imgBGR.cols;
    height = imgBGR.rows;

    // Binds the texture
    glBindTexture(GL_TEXTURE_2D, id);

    // Set the filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Creates the texture
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, format, GL_UNSIGNED_BYTE, imgBGR.ptr());
    // Creates the texture mipmaps
    glGenerateMipmap(GL_TEXTURE_2D);

}

void Image::BuildPlane() {

    float width = this->width, height = this->height;
    float quadVertices[] = {
        // positions        // Color              // texture Coords
        -width / 2,  height / 2, 0.0f, 1.0f, 0.0f, 0.0f,  0.0f, 1.0f,
        -width / 2, -height / 2, 0.0f, 0.0f, 1.0f, 0.0f,  0.0f, 0.0f,
         width / 2,  height / 2, 0.0f, 0.0f, 0.0f, 1.0f,  1.0f, 1.0f,
         width / 2, -height / 2, 0.0f, 0.5f, 0.5f, 0.75f, 1.0f, 0.0f,
    };
    // Setup plane VAO
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
    // Position
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    // Color
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    // Texture Coords
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
}
