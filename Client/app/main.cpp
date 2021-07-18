#include <iostream>

#include <Client.h>

int main()
{
    silence::impl::Screenshot screen;
    cv::Mat img;

    while (true)
    {
        cv::Mat img = screen.take();

        cv::imshow("img", img);
        char k = cv::waitKey(1);
        if (k == 'q')
            break;
    }

    return 0;
}