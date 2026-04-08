cmake_minimum_required(VERSION 3.10)
project(FINALE_SEMESTRALKA)

set(CMAKE_CXX_STANDARD 20)

add_executable(letne_riesenie letne_riesenie.c)
target_link_libraries(letne_riesenie m)

add_executable(zimne_riesenie zimne_riesenie.c)

