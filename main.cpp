#include "Application.h"

/**
 * App starting point
 * @param{int} number of arguments
 * @param{char const *[]} running arguments
 * @returns{int} app exit code
 * */
int main(int argc, char const *argv[])
{
    Application *app = Application::GetInstance();

    app->InitInstance();
   /*

    cv::Mat img = cv::imread("../avatar_cat.jfif");
    cv::Mat output_img;

    cv::sobel(img, output_img, cv_16s, 1, 0);

    if (img.empty())
    {

        std::cout << "couldn't load image";
        __debugbreak();
    }

    cv::imwrite("../out.jpg", output_img);
    return 0;
    
   */
}
