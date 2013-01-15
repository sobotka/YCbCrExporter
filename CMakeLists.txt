cmake_minimum_required(VERSION 2.6)
project(DSLRVideoLab)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/CMakeModules)

find_package(Qt4 REQUIRED)
include(${QT_USE_FILE})

find_package(FFmpeg REQUIRED)
include(${FFMPEG_INCLUDE_DIRS})

find_package(OpenImageIO REQUIRED)
include(${OPENIMAGEIO_INCLUDE_DIRS})

set(DSLRVideoLab_SRC main.cpp ffsequence.cpp dslrlabview.cpp mainwindow.cpp qbasegraphicsview.cpp qgraphicstrimslider.cpp textpill.cpp)
set(DSLRVideoLab_HEADERS ffsequence.h dslrlabview.h mainwindow.h qbasegraphicsview.h qgraphicstrimslider.h textpill.h)

QT4_WRAP_CPP(DSLRVideoLab_HEADERS_MOC ${DSLRVideoLab_HEADERS})
ADD_DEFINITIONS(${QT_DEFINITIONS})

add_executable(DSLRVideoLab  ${DSLRVideoLab_HEADERS_MOC} ${DSLRVideoLab_SRC})
target_link_libraries(DSLRVideoLab ${QT_LIBRARIES} ${FFMPEG_LIBRARIES} ${OPENIMAGEIO_LIBRARIES})
