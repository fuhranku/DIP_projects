#include "Image.h"

Image::Image() {
}

Image::Image(const char *path) {
    // Creates the texture on GPU
    imgData = cv::imread(path, cv::IMREAD_UNCHANGED);

    //std::cout << imgData.channels() << std::endl;

    if (!imgData.empty()) {
        // Save path string
        this->path = path;

        // Compute file extension
        getFileExtension(path);


        // Create textures
        glGenTextures(1, &id);
        width = imgData.cols;
        height = imgData.rows;
        channels = imgData.channels();

        // Compute DPI
        dpi = (int)glm::round((float)width * 25.4 / (float)(width * 0.264583));

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

Image::Image(const char* path, int colorSpace) {
    // Creates the texture on GPU
    switch (colorSpace) {
        // Inherit from image
        case 0:
            imgData = cv::imread(path, cv::IMREAD_UNCHANGED);
            break;
        // Force to grayscale
        case 1:
            imgData = cv::imread(path, cv::IMREAD_GRAYSCALE);
            break;
        // Force to BGR
        case 2:
            imgData = cv::imread(path, cv::IMREAD_COLOR);
            break;
         // Force to BGRA
        case 3:
            imgData = cv::imread(path, cv::IMREAD_COLOR);
            cv::cvtColor(imgData, imgData, cv::COLOR_BGR2BGRA);
            break;
    }

    if (!imgData.empty()) {
        // Save path string
        this->path = path;

        // Compute file extension
        getFileExtension(path);

        // Create textures
        glGenTextures(1, &id);
        width = imgData.cols;
        height = imgData.rows;
        channels = imgData.channels();

        // Compute Size
        size = Size(width*height, channels);

        // Compute DPI
        dpi = (int)glm::round((float)width * 25.4 / (float)(height * 0.264583));

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
    image->history._do(image, THRESHOLD_OTSU);
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
    
    // convert to float & reshape to a [3 x W*H] Mat 
    //  (so every pixel is on a row of it's own)
    cv::Mat data;
    image->imgData.convertTo(data, CV_32F);
    data = data.reshape(1, data.total());

    // do kmeans
    cv::Mat labels, centers;
    kmeans(data, k, labels, cv::TermCriteria(cv::TermCriteria::MAX_ITER, 10, 1.0), 3,
        cv::KMEANS_PP_CENTERS, centers);

    // reshape both to a single row of Vec3f pixels:
    centers = centers.reshape(3, centers.rows);
    data = data.reshape(3, data.rows);

    // replace pixel values with their center value:
    cv::Vec3f* p = data.ptr<cv::Vec3f>();
    for (size_t i = 0; i < data.rows; i++) {
        int center_id = labels.at<int>(i);
        p[i] = centers.at<cv::Vec3f>(center_id);
    }

    // back to 2d, and uchar:
    image->imgData = data.reshape(3, image->imgData.rows);
    image->imgData.convertTo(image->imgData, CV_8U);

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

Image* Image::Any2YCrCb(Image* image) {
    Image* ycrcbImg = image;
    ycrcbImg->imgData = image->imgData.clone();
    // Transform any kind of image to RGB
    switch (image->channels) {
    case 3:
        cv::cvtColor(image->imgData, ycrcbImg->imgData, cv::COLOR_BGR2YCrCb);
        break;
    case 4:
        cv::cvtColor(image->imgData, ycrcbImg->imgData, cv::COLOR_BGRA2BGR);
        cv::cvtColor(ycrcbImg->imgData, ycrcbImg->imgData, cv::COLOR_BGR2YCrCb);
        break;
    }

    ycrcbImg->imgData = image->imgData;

    return ycrcbImg;
}

void Image::getFileExtension(std::string path) {
    size_t index = path.find_last_of(".");
    std::string extension = path.substr(index + 1);
    if (extension == "jpeg" | extension == "jpg" | extension == "jpe")
        ext = Extension(IMG_JPG, "JPEG file");
    else if (extension == "bmp" | extension == "dib")
        ext = Extension(IMG_BMP, "Windows bitmaps");
    else if (extension == "png")
        ext = Extension(IMG_PNG, "Portable Network Graphics");
    else if (extension == "webp")
        ext = Extension(IMG_WEBP, "WebP file");
    else if (extension == "tiff" | extension == "tif")
        ext = Extension(IMG_TIFF, "TIFF file");
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

void Image::Rotate(Image* image, float deg) {

    cv::Point2f center((image->imgData.cols - 1) / 2.0, (image->imgData.rows - 1) / 2.0);
    cv::Mat rot = cv::getRotationMatrix2D(center, deg, 1.0);
    // determine bounding rectangle, center not relevant
    cv::Rect2f bbox = cv::RotatedRect(cv::Point2f(), image->imgData.size(), deg).boundingRect2f();
    // adjust transformation matrix
    rot.at<double>(0, 2) += bbox.width / 2.0 - image->imgData.cols / 2.0;
    rot.at<double>(1, 2) += bbox.height / 2.0 - image->imgData.rows / 2.0;

    cv::warpAffine(image->imgData, image->imgData, rot, bbox.size());
    
    image->width = image->imgData.cols;
    image->height = image->imgData.rows;

    //cv::flip(image->imgData, image->imgData,0);
    //cv::imshow("asdasd", image->imgData);

    RemovePlane(image);
    BuildPlane(image);

    // Update GPU Texture
    Image::UpdateTextureData(image);
    // Update history queue/stack
}

void Image::Flip(Image* image, int mode) {
    cv::flip(image->imgData, image->imgData, mode);

    // Compute Histograms
    Image::Histograms(image);
    // Update GPU Texture
    Image::UpdateTextureData(image);
}

void Image::EqualizeHist(Image* image) {
    //Convert the image from current space to YCrCb color space
    Any2YCrCb(image);

    //Split the image into 3 channels; Y, Cr and Cb channels respectively and store it in a std::vector
    std::vector<cv::Mat> vec_channels;
    split(image->imgData, vec_channels);

    //Equalize the histogram of only the Y channel 
    cv::equalizeHist(vec_channels[0], vec_channels[0]);

    //Merge 3 channels in the vector to form the color image in YCrCB color space.
    merge(vec_channels, image->imgData);

    //Convert the histogram equalized image from YCrCb to BGR color space again
    cvtColor(image->imgData, image->imgData, cv::COLOR_YCrCb2BGR);

    // Compute Histograms
    Image::Histograms(image);
    // Update GPU Texture
    Image::UpdateTextureData(image);
}

void Image::RemovePlane(Image *image) {
    // Deletes the vertex array from the GPU
    glDeleteVertexArrays(1, &image->VAO);
    // Deletes the vertex object from the GPU
    glDeleteBuffers(1, &image->VBO);
}

void Image::BuildPlane(Image *image) {

    float width = image->width, height = image->height;
    float quadVertices[] = {
        // positions        // Color              // texture Coords
        -width / 2,  height / 2, 0.0f, 1.0f, 0.0f, 0.0f,  0.0f, 1.0f,
        -width / 2, -height / 2, 0.0f, 0.0f, 1.0f, 0.0f,  0.0f, 0.0f,
         width / 2,  height / 2, 0.0f, 0.0f, 0.0f, 1.0f,  1.0f, 1.0f,
         width / 2, -height / 2, 0.0f, 0.5f, 0.5f, 0.75f, 1.0f, 0.0f,
    };
    // Setup plane VAO
    glGenVertexArrays(1, &image->VAO);
    glGenBuffers(1, &image->VBO);
    glBindVertexArray(image->VAO);
    glBindBuffer(GL_ARRAY_BUFFER, image->VBO);
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

void Image::Undo(Image* image) {
    //Pop action from do stack
    Action* action = image->history._undo();

    if (action) {
        //Update image data
        image->imgData = action->imgData;
        image->width = action->width;
        image->height = action->height;
        image->channels = action->channels;
        image->bitDepth = action->bitDepth;
        image->dpi = action->dpi;
        image->size = action->size;
        image->ext = action->ext;
        image->path = action->path;
        image->format = action->format;
        image->internalFormat = action->internalFormat;

        //Recreate Plane
        RemovePlane(image);
        BuildPlane(image);

        // Compute Histograms
        Histograms(image);
        // Update GPU Texture
        UpdateTextureData(image);
    }
}
