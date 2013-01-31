cmake_minimum_required(VERSION 2.6)
project(ycbcrexporter)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/CMakeModules)

find_package(Qt4 REQUIRED)
include(${QT_USE_FILE})

find_package(FFmpeg REQUIRED)
include(${FFMPEG_INCLUDE_DIRS})

find_package(OpenImageIO REQUIRED)
include(${OPENIMAGEIO_INCLUDE_DIRS})

set(ycbcrexporter_SRC main.cpp ffsequence.cpp ycbcrlabview.cpp mainwindow.cpp qbasegraphicsview.cpp qgraphicstrimslider.cpp textpill.cpp)
set(ycbcrexporter_HEADERS ffsequence.h ycbcrlabview.h mainwindow.h qbasegraphicsview.h qgraphicstrimslider.h textpill.h)

QT4_WRAP_CPP(ycbcrexporter_HEADERS_MOC ${ycbcrexporter_HEADERS})
ADD_DEFINITIONS(${QT_DEFINITIONS})

add_executable(ycbcrexporter ${ycbcrexporter_HEADERS_MOC} ${ycbcrexporter_SRC})
target_link_libraries(ycbcrexporter ${QT_LIBRARIES} ${FFMPEG_LIBRARIES} ${OPENIMAGEIO_LIBRARIES})
