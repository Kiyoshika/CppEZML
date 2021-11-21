add_subdirectory(lib)
add_subdirectory(models/classification)

add_executable(build build.cpp)
target_link_libraries(build cppezml)
target_compile_features(build PRIVATE cxx_std_17)