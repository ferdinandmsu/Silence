# OPENCV WINDOWS
# GENERATED BY CONAN

# WINDOWS LIB
add_library(silence_windows Screenshot.cpp System.cpp)
target_include_directories(silence_windows
        PUBLIC ${INCLUDE_DIR})

# OPENCV LINKING
target_link_libraries(silence_windows ${CONAN_LIBS})