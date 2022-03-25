cmake_minimum_required(VERSION 3.20)
set(PROJECT_NAME N_gramTextPrediction)
project(${PROJECT_NAME})

set(CMAKE_CXX_STANDARD 14)

include_directories(.)

add_executable(${PROJECT_NAME}
        preprocess_text.cpp
        preprocess_text.h main.cpp)
#
#FIND_PACKAGE( Boost 1.77.0 COMPONENTS algorithm string REQUIRED )
#target_include_directories(${PROJECT_NAME} PRIVATE ${Boost_INCLUDE_DIR})
#TARGET_LINK_LIBRARIES( ${PROJECT_NAME} LINK_PUBLIC ${Boost_LIBRARIES} )