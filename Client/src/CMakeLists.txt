# ADD UTIL AND CORE
add_subdirectory(${SRC_DIR}/core)

# CLIENT LIBRARY
add_library(silence_client Client.cpp)

# PATH INCLUDE
target_include_directories(silence_client PUBLIC 
                    ${VENDOR_DIR}/socket.io-client-cpp/src)

# CLIENT LINKING
target_link_libraries(silence_client 
                      silence_utility
                      sioclient
                      ${CONAN_LIBS})