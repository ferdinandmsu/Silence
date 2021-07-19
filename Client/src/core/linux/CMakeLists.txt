# OPENCV LINUX
find_package(OpenCV REQUIRED)

# DEBUG MESSAGES
message("Opencv includes: ${OpenCV_INCLUDE_DIRS}")
message("Opencv libs: ${OpenCV_LIBS}")

# LINUX LIB
add_library(silence_linux Screenshot.cpp System.cpp)
target_include_directories(silence_linux 
                PUBLIC ${INCLUDE_DIR} ${OpenCV_INCLUDE_DIRS})

# OPENCV LINKING
target_link_libraries(silence_linux 
        -lX11
        ${OpenCV_LIBS}
)