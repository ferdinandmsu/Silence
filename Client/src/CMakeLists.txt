# ADD UTIL AND CORE
add_subdirectory(${SRC_DIR}/core)

# CLIENT LIBRARY
add_library(silence_client Client.cpp)

# CLIENT LINKING
target_link_libraries(silence_client 
                      silence_ioclient)
