target_sources(cppezml PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/LogisticRegression.cpp
)
target_compile_features(cppezml PRIVATE cxx_std_17)
target_include_directories(cppezml PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})