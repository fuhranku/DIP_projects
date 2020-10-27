#include "Image.h"

Image::Image(const char *path) {
    // Creates the texture on GPU
    imgBGR = cv::imread(path, cv::IMREAD_UNCHANGED);

    //imgBGR = calcOTSU(imgBGR);

    std::cout << imgBGR.channels() << std::endl;

    if (!imgBGR.empty()) {
        glGenTextures(1, &id);
        width = imgBGR.cols;
        height = imgBGR.rows;
        channels = imgBGR.channels();
        // Compute Histogram
        buildHistograms();
        cv::flip(imgBGR, imgBGR, 0);

        // Gets the texture channel format
        GLenum format, internalFormat;
        switch (imgBGR.channels())
        {
        case 1:
            format = GL_RED;
            internalFormat = GL_R8;
            break;
        case 3:
            format = GL_BGR;
            internalFormat = GL_RGB;
            break;
        case 4:
            format = GL_BGRA;
            internalFormat = GL_RGBA;
            break;
        }

        // Binds the texture
        glBindTexture(GL_TEXTURE_2D, id);

        // Set the filtering parameters
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        // Creates the texture
        glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, imgBGR.cols, imgBGR.rows, 0, format, GL_UNSIGNED_BYTE, imgBGR.data);
        // Creates the texture mipmaps
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else {
        std::cout << "ERROR::Image empty" << std::endl;
    }

}

void Image::buildHistograms() {
    unsigned int totalBytes = width * height * channels;
    int red, green, blue, rgb8;
    switch (channels){
    case 1:
        // Compute 1 channel histogram
        histogram.push_back(Histogram(GL_BLUE));
        for (int i = 0; i < totalBytes; i++) {
            blue = (int)imgBGR.data[i];
            histogram[0].data[blue]++;
            histogram[0].setMaxValue(histogram[0].data[blue]);
        }
        break;
    case 3: case 4:
        // Compute 4 histograms
        histogram.push_back(Histogram(GL_BLUE));
        histogram.push_back(Histogram(GL_GREEN));
        histogram.push_back(Histogram(GL_RED));
        histogram.push_back(Histogram(GL_BGR));
        for (int i = 0; i < totalBytes; i+=channels) {
            // Compute Blue  Histogram
            //std::cout << "index " << i << std::endl;
            blue = (int) imgBGR.data[i];
            //std::cout << blue << std::endl;
            histogram[0].data[blue]++;
            histogram[0].setMaxValue(histogram[0].data[blue]);
            // Compute Green Histogram
            green = (int) imgBGR.data[i + 1];
            histogram[1].data[green]++;
            histogram[1].setMaxValue(histogram[1].data[green]);
            // Compute Red   Histogram
            red = (int)imgBGR.data[i + 2];
            histogram[2].data[red]++;
            histogram[2].setMaxValue(histogram[2].data[red]);
            // Compute BGR   Histogram
            rgb8 = blue << 16 | green << 8 | red;
            double ratio = (double)rgb8 / (double)16777216;
            rgb8 = ceil(ratio * 256);
            histogram[3].data[rgb8]++;
            histogram[3].setMaxValue(histogram[3].data[rgb8]);
            //if (histogram[3].data[rgb8] != 0) {
            //    std::cout << i << std::endl;
            //}
            //printf("index: %i, val: %i\n, max: %i", i, histogram[3].data[rgb8], histogram[3].maxValue);
        } 
        break;
    }
}

cv::Mat Image::calcOTSU(cv::Mat origin){
    cv::Mat dst;
    double thresh = 0;
    double maxValue = 255;
    long double thres = cv::threshold(origin, dst, thresh, maxValue, cv::THRESH_OTSU);
    return dst;
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
