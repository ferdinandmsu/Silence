# SOCKET.IO
set(SIO_PATH ${VENDOR_DIR}/socket.io-client-cpp)

add_subdirectory(${SIO_PATH})
include_directories(${SIO_PATH}/src)