# PLATFORM SPECIFIC CODE
add_library(silence_utility Util.cpp)

if (WIN32)
    add_subdirectory(${SRC_DIR}/core/windows)
    target_link_libraries(silence_utility silence_windows)
else ()
    add_subdirectory(${SRC_DIR}/core/linux)
    target_link_libraries(silence_utility silence_linux)
endif ()