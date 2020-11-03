#include "Image.h"


Image::Image() {
}

Image::Image(const char *path) {
    //
    // Creates the texture on GPU
    imgData = cv::imread(path, cv::IMREAD_UNCHANGED);

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

        // Compute Half size of image
        computeHalfSize();

        // Compute DPI
        dpi = (int)glm::round((float)width * 25.4 / (float)(width * 0.264583));
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
        // Compute canvas
        canvas.Build(width, height, arrhalfWidth, arrhalfHeight);
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

        // Compute Half size of image
        computeHalfSize();

        // Compute Size
        size = Size(width*height, channels);

        // Compute DPI
        dpi = (int)glm::round((float)width * 25.4 / (float)(height * 0.264583));

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

        // Compute canvas
        canvas.Build(width, height, arrhalfWidth, arrhalfHeight);
    }
    else {
        std::cout << "ERROR::Image empty" << std::endl;
    }

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

void Image::cloneImage(Image* src, Image* dst) {
    dst->width = src->width;
    dst->height = src->height;
    dst->bitDepth = src->bitDepth;
    dst->canvas = src->canvas;
    dst->channels = src->channels;
    dst->currentFilter = src->currentFilter;
    dst->dpi = src->dpi;
    dst->ext = src->ext;
    dst->format = src->format;
    dst->histogram = src->histogram;
    dst->id = src->id;
    dst->imgData = src->imgData.clone();
    dst->internalFormat = src->internalFormat;
    dst->path = src->path;
    dst->size = src->size;
    dst->freqComputed = src->freqComputed;
    dst->freqData = src->freqData;
}

void Image::computeHalfSize() {
    float halfWidth = width / 2;
    float halfHeight = height / 2;
    if (this->width % 2 != 0) {
        arrhalfWidth[0] = halfWidth;
        arrhalfWidth[1] = halfWidth + 1;
    }
    else
        arrhalfWidth[0] = arrhalfWidth[1] = halfWidth;
    if (this->height % 2 != 0) {
        arrhalfHeight[0] = halfHeight;
        arrhalfHeight[1] = halfHeight + 1;
    }
    else
        arrhalfHeight[0] = arrhalfHeight[1] = halfHeight;
}
