#pragma once
#ifdef __linux__

#include <opencv2/opencv.hpp>

#include <X11/Xlib.h>
#include <X11/Xutil.h>

#include <X11/extensions/XShm.h>
#include <sys/ipc.h>
#include <sys/shm.h>

namespace silence
{
    namespace impl
    {
        class Screenshot
        {
            Display *display;
            Window root;
            int x{0}, y{0}, width{1920}, height{1080};
            XImage *img{nullptr};

        public:
            Screenshot()
            {
                display = XOpenDisplay(nullptr);
                root = DefaultRootWindow(display);
            }

            ~Screenshot()
            {
                if (img != nullptr)
                    XDestroyImage(img);
                XCloseDisplay(display);
            }

        public:
            cv::Mat take()
            {
                if (img != nullptr)
                    XDestroyImage(img);
                img = XGetImage(display, root, x, y, width, height, AllPlanes, ZPixmap);
                return cv::Mat(height, width, CV_8UC4, img->data);
            }
        };

    }
}

#endif