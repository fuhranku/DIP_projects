#include "Image.h"

//std::vector<rgbColor> Image::medianCutOut;

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


void Image::MedianCut(cv::Mat origin, cv::Mat dst, int blocks, Image* image) {
    std::vector<bgrColor> LUT;

    // Transform any image to RGB color space
    cv::Mat bgrImg = Image::Any2BGR(origin, image->channels);

    std::vector<bgrColor> colorList;
    for (int i = 0; i < image->width * image->height * 3; i += 3 ) {
        colorList.push_back(bgrColor(bgrImg.data[i], bgrImg.data[i + 1], bgrImg.data[i + 2]));
    }

    QuantizeMC(LUT,colorList, 0, 10);

    for (int i = 0; i < image->width * image->height * 3; i += 3) {
        int index = -1;
        float finalDist = INT_MAX;
        for (int j = 0; j < LUT.size(); j++) {
            //printf("%i, %i, %i \n", c.g, c.b, c.r);
            //compute euclidean distance
            glm::vec3 lutColor = glm::vec3(LUT[j].b, LUT[j].g, LUT[j].r);
            glm::vec3 imageColor = glm::vec3(bgrImg.data[i], bgrImg.data[i + 1], bgrImg.data[i + 2]);
            
            float tempDist = glm::distance(lutColor, imageColor);
            if (tempDist < finalDist) {
                finalDist = tempDist;
                index = j;
            };
        }

        bgrImg.data[i] = LUT[index].b;
        bgrImg.data[i+1] = LUT[index].g;
        bgrImg.data[i+2] = LUT[index].r;
    }

    image->imgBGR = bgrImg;

    // Compute Histograms
    Image::Histograms(image);
    // Update GPU Texture
    Image::UpdateTextureData(image);


}

cv::Mat Image::Any2BGR(cv::Mat origin, int channels) {
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
