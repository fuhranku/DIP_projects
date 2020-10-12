#include "Application.h"

/**
 * App starting point
 * @param{int} number of arguments
 * @param{char const *[]} running arguments
 * @returns{int} app exit code
 * */
int main(int argc, char const *argv[])
{
    Application *app = new Application();
   /*
    cv::Mat img = cv::imread("../avatar_cat.jfif");
    cv::Mat output_img;

    cv::Sobel(img, output_img, CV_16S, 1, 0);

    if (img.empty())
    {

        std::cout << "Couldn't load image";
        __debugbreak();
    }


    cv::imwrite("../out.jpg", output_img);*/

    //return 0;
}
