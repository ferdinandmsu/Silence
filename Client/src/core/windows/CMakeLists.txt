# OPENCV WINDOWS
find_package(OpenCV REQUIRED)

# DEBUG MESSAGES
message("Opencv includes: "${OpenCV_INCLUDE_DIRS})
message("Opencv libs: "${OpenCV_LIBS})

# WINDOWS LIB
add_library(silence_windows Screenshot.cpp System.cpp)
target_include_directories(silence_windows 
                PUBLIC ${INCLUDE_DIR} ${OpenCV_INCLUDE_DIRS})

# OPENCV LINKING
target_link_libraries(silence_windows ${OpenCV_LIBS})