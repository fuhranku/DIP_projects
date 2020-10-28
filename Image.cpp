#include "Image.h"

//std::vector<rgbColor> Image::medianCutOut;

Image::Image() {

}

Image::Image(const char *path) {
    // Creates the texture on GPU
    imgData = cv::imread(path, cv::IMREAD_UNCHANGED);

    //imgData = calcOTSU(imgData);

    //std::cout << imgData.channels() << std::endl;

    if (!imgData.empty()) {
        glGenTextures(1, &id);
        width = imgData.cols;
        height = imgData.rows;
        channels = imgData.channels();
        // Compute Histogram
        Image::Histograms(this);
        cv::flip(imgData, imgData, 0);

        // Gets the texture channel format
        switch (imgData.channels())
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
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        // The line that solves everything
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
        // Creates the texture
        glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, imgData.cols, imgData.rows, 0, format, GL_UNSIGNED_BYTE, imgData.data);
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
            blue = (int)image->imgData.data[i];
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
            blue = (int)image->imgData.data[i];
            //std::cout << blue << std::endl;
            image->histogram[0].data[blue]++;
            image->histogram[0].setMaxValue(image->histogram[0].data[blue]);
            // Compute Green Histogram
            green = (int)image->imgData.data[i + 1];
            image->histogram[1].data[green]++;
            image->histogram[1].setMaxValue(image->histogram[1].data[green]);
            // Compute Red   Histogram
            red = (int)image->imgData.data[i + 2];
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

void Image::OTSU(double thresh, double maxValue, Image* image){
    // Transform image to Grayscale (if applies)
    Any2Gray(image);
    // Compute OTSU auto threshold
    cv::threshold(image->imgData, image->imgData, thresh, maxValue, cv::THRESH_OTSU);
    // Compute Histograms
    Image::Histograms(image);
    // Update GPU Texture
    Image::UpdateTextureData(image);
    // History code here
}

void Image::GaussianAdaptiveThreshold(double thresh, double maxValue, Image* image){
    // Transform image to Grayscale (if applies)
    Any2Gray(image);
    // Compute Adaptive Threshold
    cv::adaptiveThreshold(image->imgData, image->imgData, maxValue, cv::ADAPTIVE_THRESH_GAUSSIAN_C, cv::THRESH_BINARY, 13, 0);
    // Compute Histograms
    Image::Histograms(image);
    // Update GPU Texture
    Image::UpdateTextureData(image);
    // History code here
}

void Image::KMeans(int k, Image* image) {
    // Transform image to Grayscale (if applies)
    //Any2Gray(image);
    // Compute Adaptive Threshold
    cv::Mat centers(8,1,CV_32FC1);
    cv::Mat labels, data;
    image->imgData.convertTo(data, CV_32F);
    cv::kmeans(
        data,
        k,
        labels,
        cv::TermCriteria(cv::TermCriteria::EPS + cv::TermCriteria::COUNT,10,1.0),
        3,
        cv::KMEANS_PP_CENTERS, centers);
    cv::imshow("asd", data);
    //data.convertTo(data, CV_32FC3);
    //image->imgData = data;
    // Compute Histograms
    Image::Histograms(image);
    // Update GPU Texture
    Image::UpdateTextureData(image);
    // History code here
}


unsigned int Image::findBiggestRange(std::vector<bgrColor> color) {
    // Iterate over R,G and B histograms to find min and max values
    // Get R channel min and max
    glm::ivec2 Rminmax = glm::ivec2(-1, INT_MAX), Gminmax = glm::ivec2(-1, INT_MAX), Bminmax = glm::ivec2(-1, INT_MAX);
    // Reminder: Working at BGR color space
    for (int i = 0; i < color.size(); i++) {
        if (color[i].b > Bminmax.x)
            Bminmax.x = color[i].b;

        if (color[i].b < Bminmax.y)
            Bminmax.y = color[i].b;

        if (color[i].g > Gminmax.x)
            Gminmax.x = color[i].g;

        if (color[i].g < Gminmax.y)
            Gminmax.y = color[i].g;

        if (color[i].r > Rminmax.x)
            Rminmax.x = color[i].r;

        if (color[i].r < Rminmax.y)
            Rminmax.y = color[i].r;
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

void Image::QuantizeMC(std::vector<bgrColor> &out, std::vector<bgrColor> color, int currentDepth, int maxDepth){

    if (currentDepth == maxDepth) {
        //calculate the centroid
        bgrColor centroid = bgrColor(0, 0, 0);

        bgrColor accColor = std::accumulate(color.begin(), color.end(), centroid,
                            [](bgrColor acc, bgrColor actual) {
                                acc.b += actual.b;
                                acc.g += actual.g;
                                acc.r += actual.r;
                                return acc;
                            });

        centroid.b = accColor.b / color.size();
        centroid.g = accColor.g / color.size();
        centroid.r = accColor.r / color.size();

        //printf("%i, %i, %i \n", centroid.g, centroid.b, centroid.r);

        out.push_back(centroid);
        return;
    }

    // Step 1: Find biggest range (to determine how to order)
    unsigned int longestChannel = findBiggestRange(color);

    // Step 2:  Quantize
    if (longestChannel == GL_BLUE) {
        std::sort(color.begin(), color.end(),
            [](bgrColor a, bgrColor b) {return a.b > b.b; });
    }
    else if (longestChannel == GL_GREEN) {
        std::sort(color.begin(), color.end(),
            [](bgrColor a, bgrColor b) {return a.g > b.g; });
    }
    else {
        std::sort(color.begin(), color.end(),
            [](bgrColor a, bgrColor b) {return a.r > b.r; });
    }


    std::size_t const median = color.size() / 2;
    std::vector<bgrColor> split_lo(color.begin(), color.begin() + median);
    std::vector<bgrColor> split_hi(color.begin() + median, color.end());

    QuantizeMC(out,split_lo, currentDepth + 1, maxDepth);
    QuantizeMC(out,split_hi, currentDepth + 1, maxDepth);
}

void Image::MedianCut(int blocks, Image* image) {
    // Look Up table
    std::vector<bgrColor> LUT;
    // Transform any image to RGB color space
    Image::Any2BGR(image);

    // Save image raw data into a std::vector with special bgrColor container
    std::vector<bgrColor> colorList;
    for (int i = 0; i < image->width * image->height * 3; i += 3 ) {
        colorList.push_back(
            bgrColor(
                image->imgData.data[i],
                image->imgData.data[i + 1],
                image->imgData.data[i + 2])
            );
    }
    // Recursively generate Look Up Table (LUT) with 2^"blocks" colors
    QuantizeMC(LUT,colorList, 0, blocks);

    // Compute euclidean distance between each LUT color and image pixel to find the best substitute
    for (int i = 0; i < image->width * image->height * 3; i += 3) {
        int index = -1;
        float finalDist = INT_MAX;
        for (int j = 0; j < LUT.size(); j++) {
            //printf("%i, %i, %i \n", c.g, c.b, c.r);
            // Euclidean distance
            glm::vec3 lutColor = glm::vec3(LUT[j].b, LUT[j].g, LUT[j].r);
            glm::vec3 imageColor = glm::vec3(
                                        image->imgData.data[i],
                                        image->imgData.data[i + 1],
                                        image->imgData.data[i + 2]
                                    );   
            float tempDist = glm::distance(lutColor, imageColor);
            if (tempDist < finalDist) {
                finalDist = tempDist;
                index = j;
            };
        }

        image->imgData.data[i] = LUT[index].b;
        image->imgData.data[i+1] = LUT[index].g;
        image->imgData.data[i+2] = LUT[index].r;
    }

    // Compute Histograms
    Image::Histograms(image);
    // Update GPU Texture
    Image::UpdateTextureData(image);

}

Image* Image::Any2BGR(Image* image) {
    Image* bgrImg = image;
    bgrImg->imgData = image->imgData.clone();
    // Transform any kind of image to BGR
    switch (image->channels) {
    case 1:
        cv::cvtColor(image->imgData, bgrImg->imgData, cv::COLOR_GRAY2BGR);
        break;
    case 4:
        cv::cvtColor(image->imgData, bgrImg->imgData, cv::COLOR_BGRA2BGR);
        break;
    }

    bgrImg->imgData = image->imgData;
    bgrImg->internalFormat = GL_RGB;
    bgrImg->format = GL_BGR;
    bgrImg->channels = 3;

    return bgrImg;
}

Image* Image::Any2Gray(Image* image) {
    Image* grayImg = image;
    grayImg->imgData = image->imgData.clone();
    // Transform any kind of image to RGB
    switch (image->channels) {
        case 3:
            cv::cvtColor(image->imgData, grayImg->imgData, cv::COLOR_BGR2GRAY);
            break;
        case 4:
            cv::cvtColor(image->imgData, grayImg->imgData, cv::COLOR_BGRA2GRAY);
            break;
    }

    grayImg->imgData = image->imgData;
    grayImg->internalFormat = GL_R8;
    grayImg->format = GL_RED;
    grayImg->channels = 1;

    return grayImg;
}

void Image::ColorReduce(int numBits, Image* image) {

    uchar maskBit = 0xFF;
    // keep numBits as 1 and (8 - numBits) would be all 0 towards the right
    maskBit = maskBit << numBits;

    for (int j = 0; j < image->imgData.rows; j++)
        for (int i = 0; i < image->imgData.cols; i++)
        {
            cv::Vec3b valVec = image->imgData.at<cv::Vec3b>(j, i);
            valVec[0] = valVec[0] & maskBit;
            valVec[1] = valVec[1] & maskBit;
            valVec[2] = valVec[2] & maskBit;
            image->imgData.at<cv::Vec3b>(j, i) = valVec;
        }

    // Compute Histograms
    Image::Histograms(image);
    // Update GPU Texture
    Image::UpdateTextureData(image);
    // Update history queue/stack

}

void Image::UpdateTextureData(Image* image) {
    glBindTexture(GL_TEXTURE_2D, image->id);
    glTexImage2D(GL_TEXTURE_2D, 0, image->internalFormat, image->width, image->height, 0, image->format, GL_UNSIGNED_BYTE, image->imgData.data);
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
