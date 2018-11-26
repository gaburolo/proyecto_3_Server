cmake_minimum_required(VERSION 3.8)
project(SERVER)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES ../proyecto_3_Server/main.cpp Video.cpp Video.h Data.cpp Data.h ../proyecto_3_Server/Listas/Node.h ../proyecto_3_Server/Listas/LinkedList.h ../proyecto_3_Server/Singleton.cpp Singleton.h Control.cpp Control.h Tolerancia.h)
add_executable(SERVER ${SOURCE_FILES})

set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake_modules")

set (OpenCV_DIR /home/cmake/opencv/compiled) #change the path to match your complied directory of opencv

find_package(OpenCV REQUIRED)
if(OpenCV_FOUND)
    target_link_libraries(SERVER ${OpenCV_LIBS})
    target_link_libraries(SERVER ${opencv_core})
    target_link_libraries(SERVER ${opencv_videoio})
    target_link_libraries(SERVER ${opencv_highgui})
endif()

find_package(SFML REQUIRED system window graphics network audio)
if (SFML_FOUND)
    include_directories(${SFML_INCLUDE_DIR})
    target_link_libraries(SERVER ${SFML_LIBRARIES})
endif()

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -pthread")