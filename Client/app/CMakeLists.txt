# MAIN EXECUTABLE
add_executable(main main.cpp)

# ADD TARGETS
target_link_libraries(main PUBLIC silence_client ${CONAN_LIBS})