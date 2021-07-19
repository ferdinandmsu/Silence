# PLATFORM SPECIFIC CODE
add_library(silence_utility Util.cpp)

if (WIN32)
    add_subdirectory(${SRC_DIR}/core/windows)
    target_link_libraries(silence_utility silence_windows)
else()
    add_subdirectory(${SRC_DIR}/core/linux)
    target_link_libraries(silence_utility silence_linux)
endif()


# IOCLIENT LIBRARY
add_library(silence_ioclient IOClient.cpp)

# IOCLIENT LINKING
target_include_directories(silence_ioclient PUBLIC 
                    ${VENDOR_DIR}/socket.io-client-cpp/src)

target_link_libraries(silence_ioclient 
                      silence_utility
                      sioclient_tls
                      ${CONAN_LIBS})
