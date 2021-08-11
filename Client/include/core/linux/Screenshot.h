#pragma once
#ifdef __linux__

#include <opencv2/opencv.hpp>

#include <X11/Xlib.h>
#include <X11/Xutil.h>

#include <X11/extensions/XShm.h>
#include <sys/ipc.h>
#include <sys/shm.h>

namespace silence::impl {
    class Screenshot {
        Display *display;
        Window root;
        int x{0}, y{0}, width{1920}, height{1080};
        XImage *img{nullptr};

    public:
        Screenshot();

        ~Screenshot();

    public:
        cv::Mat take();
    };
}

#endif