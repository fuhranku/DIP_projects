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
        Image::Histograms(this);
        cv::flip(imgBGR, imgBGR, 0);

        // Gets the texture channel format
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

void Image::Histograms(Image* image) {
    unsigned int totalBytes = image->width * image->height * image->channels;
    int red, green, blue, rgb8;
    double ratio;
    image->histogram.clear();
    switch (image->channels){
    case 1:
        // Compute 1 channel histogram
        image->histogram.push_back(Histogram(GL_BLUE));
        for (int i = 0; i < totalBytes; i += image->channels) {
            blue = (int)image->imgBGR.data[i];
            image->histogram[0].data[blue]++;
            image->histogram[0].setMaxValue(image->histogram[0].data[blue]);
        }
        break;
    case 3: case 4:
        // Compute 4 histograms
        image->histogram.push_back(Histogram(GL_BLUE));
        image->histogram.push_back(Histogram(GL_GREEN));
        image->histogram.push_back(Histogram(GL_RED));
        image->histogram.push_back(Histogram(GL_BGR));
        for (int i = 0; i < totalBytes; i += image->channels) {
            // Compute Blue  Histogram
            //std::cout << "index " << i << std::endl;
            blue = (int)image->imgBGR.data[i];
            //std::cout << blue << std::endl;
            image->histogram[0].data[blue]++;
            image->histogram[0].setMaxValue(image->histogram[0].data[blue]);
            // Compute Green Histogram
            green = (int)image->imgBGR.data[i + 1];
            image->histogram[1].data[green]++;
            image->histogram[1].setMaxValue(image->histogram[1].data[green]);
            // Compute Red   Histogram
            red = (int)image->imgBGR.data[i + 2];
            image->histogram[2].data[red]++;
            image->histogram[2].setMaxValue(image->histogram[2].data[red]);
            // Compute BGR   Histogram
            rgb8 = blue << 16 | green << 8 | red;
            ratio = (double)rgb8 / (double)16777216;
            rgb8 = ceil(ratio * 256);
            image->histogram[3].data[rgb8]++;
            image->histogram[3].setMaxValue(image->histogram[3].data[rgb8]);
            //if (histogram[3].data[rgb8] != 0) {
            //    std::cout << i << std::endl;
            //}
            //printf("index: %i, val: %i\n, max: %i", i, histogram[3].data[rgb8], histogram[3].maxValue);
        } 
        break;
    }
    // Normalized frequency values
    //for (auto histogram : image->histogram)
    //{
    //    for (int i = 0; i < 255; i++) {
    //        ratio = (double)histogram.data[i] / (double)histogram.maxValue;
    //        histogram.data[i] = ceil(ratio * 100);
    //    }
    //    histogram.maxValue = 100;
    //}
}

void Image::OTSU(cv::Mat origin, cv::Mat dst, double thresh, double maxValue, Image* image){
    if (image->channels > 1) {
        cv::cvtColor(origin, dst, cv::COLOR_BGR2GRAY);
        image->imgBGR = dst;
        image->internalFormat = GL_R8;
        image->format = GL_RED;
        image->channels = 1;
    }
    cv::threshold(dst, dst, thresh, maxValue, cv::THRESH_OTSU);
    // Compute Histograms
    Image::Histograms(image);
    // Update GPU Texture
    Image::UpdateTextureData(image);
    // History code here
}

void Image::GaussianAdaptiveThreshold(cv::Mat origin, cv::Mat dst, double thresh, double maxValue, Image* image){
    if (image->channels > 1) {
        cv::cvtColor(origin, dst, cv::COLOR_BGR2GRAY);
        image->imgBGR = dst;
        image->internalFormat = GL_R8;
        image->format = GL_RED;
        image->channels = 1;
    }
    cv::adaptiveThreshold(dst, dst, maxValue, cv::ADAPTIVE_THRESH_GAUSSIAN_C, cv::THRESH_BINARY, 5, 0);

    // Compute Histograms
    Image::Histograms(image);
    // Update GPU Texture
    Image::UpdateTextureData(image);
    // History code here
}


void Image::MedianCut(cv::Mat origin, cv::Mat dst, int blocks, Image* image) {
    std::vector<cv::Vec3b> LUT;
    // Step 1: Find biggest range (to determine how to order)
    unsigned int longestChannel = findBiggestRange(image);
    // Transform any image to RGB color space
    cv::Mat rgbImg = Image::Any2RGB(origin, image->channels); 

    // Step 2:  Quantize
    std::list<rgbColor> colorList;
    for (int i = 0; i < image->width * image->height * 3; i += 3 ) {
        colorList.push_back(rgbColor(rgbImg.data[i], rgbImg.data[i + 1], rgbImg.data[i + 2]));
    }

    if (longestChannel == GL_BLUE) {
        std::sort(colorList.begin(), colorList.end(),
            [](rgbColor a, rgbColor b) {return a.b > b.b; });
    }
    else if (longestChannel == GL_GREEN) {
        std::sort(colorList.begin(), colorList.end(),
            [](rgbColor a, rgbColor b) {return a.g > b.g; });
    }
    else {
        std::sort(colorList.begin(),colorList.end(),
            [](rgbColor a, rgbColor b) {return a.r > b.r; });
    }

    for (auto color : colorList)
        printf("(%i,%i,%i)\n", color.b, color.g, color.r);

}

cv::Mat Image::Any2RGB(cv::Mat origin, int channels) {
    cv::Mat rgbimg = origin.clone();
    // Transform any kind of image to RGB
    if (channels == 4) {
        cv::cvtColor(origin, rgbimg, cv::COLOR_BGRA2BGR);
    }
    else if (channels == 1) {
        cv::cvtColor(origin, rgbimg, cv::COLOR_GRAY2BGR);
    }

    return rgbimg;
}

unsigned int Image::findBiggestRange(Image* image) {
    // Iterate over R,G and B histograms to find min and max values
    // Get R channel min and max
    glm::ivec2 Rminmax = glm::ivec2(-1), Gminmax = glm::ivec2(-1), Bminmax = glm::ivec2(-1);
    // Reminder: Working at BGR color space
    for (int i = 0; i < 256; i++) {
        if (image->histogram[0].data[i] != 0 && Bminmax.x == -1)
            Bminmax.x = i;
        if (image->histogram[1].data[i] != 0 && Gminmax.x == -1)
            Gminmax.x = i;
        if (image->histogram[2].data[i] != 0 && Rminmax.x == -1)
            Rminmax.x = i;
        if (image->histogram[0].data[i] != 0)
            Bminmax.y = i;
        if (image->histogram[1].data[i] != 0)
            Gminmax.y = i;
        if (image->histogram[2].data[i] != 0)
            Rminmax.y = i;
    }
    // Get magnitude of each vector
    const int bRange = Bminmax.y - Bminmax.x;
    const int gRange = Gminmax.y - Gminmax.x;
    const int rRange = Rminmax.y - Rminmax.x;
    // Get max between channels
    const int bgrMax = glm::max(glm::max(bRange, gRange), rRange);
    if (bgrMax == bRange)
        return GL_BLUE;
    else if (bgrMax == gRange)
        return GL_GREEN;
    return GL_RED;
}

void Image::ColorReduce(cv::Mat origin, cv::Mat dst, int numBits, Image* image) {

    uchar maskBit = 0xFF;

    // keep numBits as 1 and (8 - numBits) would be all 0 towards the right
    maskBit = maskBit << numBits;

    for (int j = 0; j < origin.rows; j++)
        for (int i = 0; i < origin.cols; i++)
        {
            cv::Vec3b valVec = origin.at<cv::Vec3b>(j, i);
            valVec[0] = valVec[0] & maskBit;
            valVec[1] = valVec[1] & maskBit;
            valVec[2] = valVec[2] & maskBit;
            dst.at<cv::Vec3b>(j, i) = valVec;
        }

    // Compute Histograms
    Image::Histograms(image);
    // Update GPU Texture
    Image::UpdateTextureData(image);

}

void Image::UpdateTextureData(Image* image) {
    glBindTexture(GL_TEXTURE_2D, image->id);
    glTexImage2D(GL_TEXTURE_2D, 0, image->internalFormat, image->width, image->height, 0, image->format, GL_UNSIGNED_BYTE, image->imgBGR.data);
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
