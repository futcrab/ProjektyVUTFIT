cmake_minimum_required(VERSION 3.15)


set(CMAKE_EXPORT_COMPILE_COMMANDS 1 ) 

project(MulGLPlayer)

add_executable(${PROJECT_NAME} main.cpp)

option(SDL_SHARED "" OFF)
option(SDL_STATIC "" ON)
add_subdirectory(libs/SDL2-2.0.20)
add_subdirectory(libs/geGL-master)

target_link_libraries(${PROJECT_NAME} 
  SDL2-static
  SDL2::SDL2main
  geGL::geGL
  )

add_custom_target(run ./${PROJECT_NAME})

