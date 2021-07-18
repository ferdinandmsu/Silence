# OPENCV
find_package(OpenCV REQUIRED)

# UTIL LIBRARY
add_library(silence_util Util.cpp)
target_include_directories(silence_util PUBLIC ${INCLUDE_DIR} ${OpenCV_INCLUDE_DIRS})
message("Opencv includes: "${OpenCV_INCLUDE_DIRS})
message("Opencv libs: "${OpenCV_LIBS})

# FIND X11 stuff
if (NOT WIN32 MATCHES True)
    target_link_libraries (silence_util 
        ${X11_LIBRARIES}
        -lX11
        ${OpenCV_LIBS}
    )
else()
    target_link_libraries(silence_util ${OpenCV_LIBS})
endif()


# IOCLIENT LIBRARY
add_library(silence_ioclient IOClient.cpp)

# IOCLIENT LINKING
target_include_directories(silence_ioclient PUBLIC 
                    ${VENDOR_DIR}/socket.io-client-cpp/src)

target_link_libraries(silence_ioclient 
                      silence_util
                      sioclient_tls
                      ${CONAN_LIBS})
