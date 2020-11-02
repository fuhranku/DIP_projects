#include "DIPlib.h"

void DIPlib::Histograms(Image* image) {
    unsigned int totalBytes = image->width * image->height * image->channels;
    int red, green, blue, rgb8;
    double ratio;
    image->histogram.clear();
    switch (image->channels) {
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

void DIPlib::OTSU(double thresh, double maxValue, Image* image) {
    // Transform image to Grayscale (if applies)
    Any2Gray(image);
    // Compute OTSU auto threshold
    cv::threshold(image->imgData, image->imgData, thresh, maxValue, cv::THRESH_OTSU);
    // Compute Histograms
    DIPlib::Histograms(image);
    // Update GPU Texture
    DIPlib::UpdateTextureData(image);
    // Update current filter of image
    image->currentFilter = IMG_THRESHOLD_OTSU;
}

void DIPlib::GaussianAdaptiveThreshold(double thresh, double maxValue, Image* image) {
    // Transform image to Grayscale (if applies)
    Any2Gray(image);
    // Compute Adaptive Threshold
    cv::adaptiveThreshold(image->imgData, image->imgData, maxValue, cv::ADAPTIVE_THRESH_GAUSSIAN_C, cv::THRESH_BINARY, 13, 0);
    // Compute Histograms
    DIPlib::Histograms(image);
    // Update GPU Texture
    DIPlib::UpdateTextureData(image);
    // History code here
}

void DIPlib::KMeans(int k, Image* image) {
    // Convert any image to RGB color depth
    DIPlib::Any2BGR(image);

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
    DIPlib::Histograms(image);
    // Update GPU Texture
    DIPlib::UpdateTextureData(image);
}

unsigned int DIPlib::findBiggestRange(std::vector<bgrColor> color) {
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

void DIPlib::QuantizeMC(std::vector<bgrColor>& out, std::vector<bgrColor> color, int currentDepth, int maxDepth) {

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

    QuantizeMC(out, split_lo, currentDepth + 1, maxDepth);
    QuantizeMC(out, split_hi, currentDepth + 1, maxDepth);
}

void DIPlib::MedianCut(int blocks, Image* image) {
    // Look Up table
    std::vector<bgrColor> LUT;
    // Transform any image to RGB color space
    DIPlib::Any2BGR(image);

    // Save image raw data into a std::vector with special bgrColor container
    std::vector<bgrColor> colorList;
    for (int i = 0; i < image->width * image->height * 3; i += 3) {
        colorList.push_back(
            bgrColor(
                image->imgData.data[i],
                image->imgData.data[i + 1],
                image->imgData.data[i + 2])
        );
    }
    // Recursively generate Look Up Table (LUT) with 2^"blocks" colors
    QuantizeMC(LUT, colorList, 0, blocks);

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
        image->imgData.data[i + 1] = LUT[index].g;
        image->imgData.data[i + 2] = LUT[index].r;
    }

    // Compute Histograms
    DIPlib::Histograms(image);
    // Update GPU Texture
    DIPlib::UpdateTextureData(image);

}

void DIPlib::Any2BGR(Image* image) {
    // Transform any kind of image to BGR
    switch (image->channels) {
    case 1:
        cv::cvtColor(image->imgData, image->imgData, cv::COLOR_GRAY2BGR);
        break;
    case 4:
        cv::cvtColor(image->imgData, image->imgData, cv::COLOR_BGRA2BGR);
        break;
    }

    image->internalFormat = GL_RGB;
    image->format = GL_BGR;
    image->channels = 3;
}

void DIPlib::Any2Gray(Image* image) {
    // Transform any kind of image to RGB
    switch (image->channels) {
    case 3:
        cv::cvtColor(image->imgData, image->imgData, cv::COLOR_BGR2GRAY);
        break;
    case 4:
        cv::cvtColor(image->imgData, image->imgData, cv::COLOR_BGRA2GRAY);
        break;
    }

    image->internalFormat = GL_R8;
    image->format = GL_RED;
    image->channels = 1;
}

void DIPlib::Any2YCrCb(Image* image) {
    // Transform any kind of image to RGB
    switch (image->channels) {
    case 3:
        cv::cvtColor(image->imgData, image->imgData, cv::COLOR_BGR2YCrCb);
        break;
    case 4:
        cv::cvtColor(image->imgData, image->imgData, cv::COLOR_BGRA2BGR);
        cv::cvtColor(image->imgData, image->imgData, cv::COLOR_BGR2YCrCb);
        break;
    }
}

void DIPlib::ColorReduce(int numBits, Image* image) {
    // Convert any image to RGB color depth
    DIPlib::Any2BGR(image);
    // Compute Color Reduction
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
    DIPlib::Histograms(image);
    // Update GPU Texture
    DIPlib::UpdateTextureData(image);
    // Update history queue/stack

}

void DIPlib::UpdateTextureData(Image* image) {
    glBindTexture(GL_TEXTURE_2D, image->id);
    glTexImage2D(GL_TEXTURE_2D, 0, image->internalFormat, image->width, image->height, 0, image->format, GL_UNSIGNED_BYTE, image->imgData.data);
}

void DIPlib::Rotate(Image* image, float deg) {

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

    // Update canvas
    image->canvas.Update(image->width, image->height);

    // Update GPU Texture
    DIPlib::UpdateTextureData(image);

    // Update history queue/stack
}

void DIPlib::Flip(Image* image, int mode) {
    cv::flip(image->imgData, image->imgData, mode);

    // Update GPU Texture
    DIPlib::UpdateTextureData(image);
}

void DIPlib::EqualizeHist(Image* image) {
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
    DIPlib::Histograms(image);

    // Update GPU Texture
    DIPlib::UpdateTextureData(image);
}

void DIPlib::DFT(Image* image) {
    // Clear if new transformation is happening
    image->freqData.clear();
    std::vector<cv::Mat> img_channels;
    cv::split(image->imgData, img_channels);

    // Step 1: expand input image to optimal size
    cv::Mat padded;                           
    int m = cv::getOptimalDFTSize(image->imgData.rows);
    int n = cv::getOptimalDFTSize(image->imgData.cols); 
    FreqData data;
    // Step 2: Compute DFT for each image channel
    for (auto channel : img_channels) {

        // Save original dimensions of image for further use
        data.originalWidth = channel.cols;
        data.originalHeight = channel.rows;

        // Compute Complex array of image with DFT
        data.complexI = computeDFT(channel);

        // Rearrange cuadrants
        data.mag = fftShift(data.complexI);

        // Compute magnitude
        data.mag = computeMagnitude(data.complexI);

        // Save frequency data
        image->freqData.push_back(data);
    }

    // Render Image on Frequency Domain (data holds last iteration value)
    normalize(data.mag, data.mag, 0, 255, cv::NORM_MINMAX);
    data.mag.convertTo(data.mag, CV_8U);

    image->imgData = data.mag.clone();
    image->width = data.mag.cols;
    image->height = data.mag.rows;
    image->format = GL_RED;
    image->internalFormat = GL_R8;
    image->channels = 1;
    image->freqComputed = 1;
    // Rebuild Plane
    image->canvas.Update(image->width, image->height);
    // Update GPU Texture
    DIPlib::UpdateTextureData(image);
                                            
}

cv::Mat DIPlib::computeDFT(cv::Mat channel){
    // Step 1: expand input image to optimal size
    cv::Mat padded;
    int m = cv::getOptimalDFTSize(channel.rows);
    int n = cv::getOptimalDFTSize(channel.cols);
    // Step 2: Compute DFT for each image channel

    // On the border add zero values
    // Save diff of resolution

    copyMakeBorder(channel,
        padded,
        0,
        m - channel.rows,
        0,
        n - channel.cols,
        cv::BORDER_CONSTANT,
        cv::Scalar::all(0)
    );
    cv::Mat planes[] = { cv::Mat_<float>(padded), cv::Mat::zeros(padded.size(), CV_32F) };;
    cv::Mat complexI;
    // Add to the expanded another plane with zeros
    cv::merge(planes, 2, complexI);
    // this way the result may fit in the source matrix
    cv::dft(complexI, complexI,cv::DFT_COMPLEX_OUTPUT);

    return complexI;
}

cv::Mat DIPlib::fftShift(cv::Mat complexI) {
    cv::Mat magI = complexI.clone();
    // crop if it has an odd number of rows or columns
    magI = magI(cv::Rect(0, 0, magI.cols & -2, magI.rows & -2));

    int cx = magI.cols / 2;
    int cy = magI.rows / 2;

    cv::Mat q0(magI, cv::Rect(0, 0, cx, cy));   // Top-Left - Create a ROI per quadrant
    cv::Mat q1(magI, cv::Rect(cx, 0, cx, cy));  // Top-Right
    cv::Mat q2(magI, cv::Rect(0, cy, cx, cy));  // Bottom-Left
    cv::Mat q3(magI, cv::Rect(cx, cy, cx, cy)); // Bottom-Right

    cv::Mat tmp;                            // swap quadrants (Top-Left with Bottom-Right)
    q0.copyTo(tmp);
    q3.copyTo(q0);
    tmp.copyTo(q3);

    q1.copyTo(tmp);                     // swap quadrant (Top-Right with Bottom-Left)
    q2.copyTo(q1);
    tmp.copyTo(q2);

    return magI;
}

cv::Mat DIPlib::computeMagnitude(cv::Mat complexI) {
    cv::Mat planes[2];
    cv::split(complexI, planes);
    // planes[0] = magnitude
    cv::Mat magI;
    cv::magnitude(planes[0], planes[1], magI);
    // switch to logarithmic scale
    magI += cv::Scalar::all(1);
    cv::log(magI, magI);
    // crop the spectrum, if it has an odd number of rows or columns
    return DIPlib::fftShift(magI);
}

void DIPlib::FourierFilter(Image *image, int type, int distance){
    
    cv::Mat filter = image->freqData[0].complexI.clone();

    switch (type) {
        case IMG_FOURIER_LOW_PASS:
            LowPassFilter(filter, distance);

            break;
        case IMG_FOURIER_HIGH_PASS:
            HighPassFilter(filter, distance);
            break;
    }

    std::vector<cv::Mat> channelArray;
    cv::Mat planes[2], imgOutput, filterOutput;

    int i = 0;
    for (auto channel : image->freqData) {
        // rearrage quadrants
        channel.complexI = fftShift(channel.complexI);

        // multiply 2 spectrums
        cv::mulSpectrums(channel.complexI, filter, channel.complexI, 0);

        // rearrage quadrants
        channel.complexI = fftShift(channel.complexI);

        // compute inverse
        idft(channel.complexI, channel.complexI);

        split(channel.complexI, planes);
        normalize(planes[0], imgOutput, 0, 255, cv::NORM_MINMAX);
        imgOutput.convertTo(imgOutput, CV_8U);

        channelArray.push_back(imgOutput);
        //cv::imshow("High pass filter" + i++, imgOutput);
    }

    split(filter, planes);
    normalize(planes[1], filterOutput, 0, 1, cv::NORM_MINMAX);

    //cv::imshow("Filter", filterOutput);

    FourierToImage(image, channelArray);
}

void DIPlib::LowPassFilter(const cv::Mat &fourierFilter, int distance) {
    cv::Mat tmp = cv::Mat(fourierFilter.rows, fourierFilter.cols, CV_32F);

    cv::Point centre = cv::Point(fourierFilter.rows / 2, fourierFilter.cols / 2);
    double radius;

    for (int i = 0; i < fourierFilter.rows; i++)
    {
        for (int j = 0; j < fourierFilter.cols; j++)
        {
            radius = (double)sqrt(pow((i - centre.x), 2.0) + pow((double)(j - centre.y), 2.0));
            if (radius > distance) {
                tmp.at<float>(i, j) = (float)0;
            }
            else {
                tmp.at<float>(i, j) = (float)1;
            }

        }
    }

    cv::Mat toMerge[] = { tmp, tmp };
    cv::merge(toMerge, 2, fourierFilter);
}

void DIPlib::HighPassFilter(const cv::Mat &fourierFilter, int distance) {
    cv::Mat tmp = cv::Mat(fourierFilter.rows, fourierFilter.cols, CV_32F);

    cv::Point centre = cv::Point(fourierFilter.rows / 2, fourierFilter.cols / 2);
    double radius;

    for (int i = 0; i < fourierFilter.rows; i++)
    {
        for (int j = 0; j < fourierFilter.cols; j++)
        {
            radius = (double)sqrt(pow((i - centre.x), 2.0) + pow((double)(j - centre.y), 2.0));
            if (radius > distance) {
                tmp.at<float>(i, j) = (float)1;
            }
            else {
                tmp.at<float>(i, j) = (float)0;
            }

        }
    }

    cv::Mat toMerge[] = { tmp, tmp };
    cv::merge(toMerge, 2, fourierFilter);
}

void DIPlib::IDFT(Image* image) {
    std::vector<cv::Mat> channelArray;
    cv::Mat inverseTransform;
    for (auto channel: image->freqData) {
        cv::dft(channel.complexI, inverseTransform, cv::DFT_INVERSE | cv::DFT_REAL_OUTPUT);
        normalize(inverseTransform, inverseTransform, 0, 255, cv::NORM_MINMAX);
        inverseTransform.convertTo(inverseTransform, CV_8U);
        channelArray.push_back(inverseTransform);
    }
    
    FourierToImage(image, channelArray);

}

void DIPlib::FourierToImage(Image *image, std::vector<cv::Mat> channelArray) {
    cv::Mat mat;
    cv::merge(
        channelArray,
        mat
    );

    // Crop to original size
    cv::Rect crop_region(
        0,
        0,
        image->freqData[0].originalWidth,
        image->freqData[0].originalHeight
    );
    mat = mat(crop_region);
    // Render image
    image->imgData = mat.clone();
    image->width = image->freqData[0].originalWidth;
    image->height = image->freqData[0].originalHeight;
    image->format = GL_BGR;
    image->internalFormat = GL_RGB;
    image->channels = 3;
    image->freqComputed = 0;
    // Rebuild Plane
    image->canvas.Update(image->width, image->height);
    // Update GPU Texture
    DIPlib::UpdateTextureData(image);
}

void DIPlib::FloodFill(Image* image, int range_type, int nhbrhd_type, cv::Point seed, cv::Scalar newColor) {      
    cv::Point point;
    DIPlib::FromWorldSpaceToImageSpace(
        seed,
        point,
        image
    ); 

    printf("point: (%i,%i) \n", point.x, point.y);
    cv::Point newPoint = cv::Point(point.x - 1, image->imgData.rows - point.y);
    printf("point: (%i,%i) \n", newPoint.x, newPoint.y);

    // 2d to 1d access equation
    int color_index = DIPlib::Reduce2DTo1DArray(point, image->imgData.cols, image->channels);
    cv::Scalar picked_color = cv::Scalar(3);
    picked_color[0] = image->imgData.data[color_index + 0];
    picked_color[1] = image->imgData.data[color_index + 1];
    picked_color[2] = image->imgData.data[color_index + 2];

    printf("picked color: (%i,%i,%i) \n", image->imgData.data[color_index + 0],
        image->imgData.data[color_index + 1], image->imgData.data[color_index + 2]);


    cv::Mat floodFillTest = image->imgData.clone();
    cv::flip(floodFillTest, floodFillTest, 0);
    
    // Compute flood fill
    cv::floodFill(
        floodFillTest,
        newPoint,
        newColor,
        0,
        cv::Scalar(5,5,5,5),
        cv::Scalar(5,5,5,5)
    );

    cv::imshow("Flooded", floodFillTest);
}

void DIPlib::FromWorldSpaceToImageSpace(cv::Point src, cv::Point &dst, Image* image) {
    // moving from -(w/2) to (0,0) and -h/2 to -(0,0)
    if (src.x < 0)
        dst.x = src.x + image->arrhalfWidth[0];
    else
        dst.x = src.x + image->arrhalfWidth[0];
    if (src.y < 0)
        dst.y = src.y + image->arrhalfHeight[0];
    else
        dst.y = src.y + image->arrhalfHeight[1];
}

bool DIPlib::IsInsideImage(Image* image, cv::Point pos) {
    if (pos.x > -image->arrhalfWidth[0]   &&
        pos.x <  image->arrhalfWidth[1]   &&
        pos.y > -image->arrhalfHeight[0]  &&
        pos.y <  image->arrhalfHeight[1])
            return true;
    return false;
}

cv::Scalar DIPlib::DenormalizeBGR(cv::Scalar normBGR){

    cv::Scalar  denormBGR(3);
    denormBGR[0] = round(normBGR[0] * 255.0f);
    denormBGR[1] = round(normBGR[1] * 255.0f);
    denormBGR[2] = round(normBGR[2] * 255.0f);
    
    return denormBGR;
}

