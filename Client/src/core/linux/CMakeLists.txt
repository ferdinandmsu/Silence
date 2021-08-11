# OPENCV LINUX
# find_package(OpenCV REQUIRED) # ONLY FOR BUILD FROM SOURCE

# LINUX LIB
add_library(silence_linux Screenshot.cpp System.cpp)
target_include_directories(silence_linux
        PUBLIC ${INCLUDE_DIR} ${OpenCV_INCLUDE_DIRS})

# OPENCV LINKING
target_link_libraries(silence_linux
        PUBLIC -lX11
        PUBLIC ${OpenCV_LIBS}
        PUBLIC ${CONAN_LIBS_OPENCV})