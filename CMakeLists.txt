cmake_minimum_required(VERSION 3.24)
project(QRCodeGen)

set(CMAKE_CXX_STANDARD 17)



add_executable(QRCodeGen main.cpp)
set(OpenCV_DIR "D:\\OpenCV\\mingw-build")
find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})
include_directories("C:\\Program Files (x86)\\QRencode\\include")
target_link_libraries(QRCodeGen ${OpenCV_LIBS})
target_link_libraries(QRCodeGen "C:/Program Files (x86)/QRencode/lib/libqrencode.a")