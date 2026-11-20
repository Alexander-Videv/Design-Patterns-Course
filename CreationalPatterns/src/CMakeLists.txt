# 1. Create the library WITHOUT main.cpp
add_library(figure_lib 
    Figure.cpp
    Factory.cpp
)

target_include_directories(figure_lib PUBLIC 
    ${CMAKE_SOURCE_DIR}/include
)

# 2. Create your application executable WITH main.cpp
add_executable(my_app
    main.cpp
)

# 3. Link the library to your executable
target_link_libraries(my_app PRIVATE 
    figure_lib
)