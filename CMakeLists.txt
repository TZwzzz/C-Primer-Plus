# @author        Liang Huanyan
# @email         lianghuanyan@alientek.com
# http://www.openedv.com/forum.php
project(build_libcv_tracking)
cmake_minimum_required(VERSION 3.8)
message(STATUS "cmake version ${CMAKE_VERSION}")

set(COMPILE_STATIC NO)#编译静态库
set(COMPILE_SHARED YES)#编译动态库
set(CMAKE_CXX_STANDARD 11)
add_definitions(-g -O0 -ggdb -gdwarf -funwind-tables -rdynamic)
add_definitions(-Wno-write-strings -Wno-return-type)


set(OPENCV_LIBS opencv_dnn opencv_stitching opencv_superres opencv_videostab opencv_aruco opencv_bgsegm opencv_bioinspired opencv_ccalib opencv_dpm opencv_highgui opencv_videoio opencv_face opencv_fuzzy opencv_hfs opencv_img_hash opencv_line_descriptor opencv_optflow opencv_reg opencv_rgbd opencv_saliency opencv_stereo opencv_structured_light opencv_phase_unwrapping opencv_surface_matching opencv_tracking opencv_datasets opencv_plot opencv_xfeatures2d opencv_shape opencv_video opencv_ml opencv_ximgproc opencv_xobjdetect opencv_objdetect opencv_calib3d opencv_imgcodecs opencv_features2d opencv_flann opencv_freetype opencv_xphoto opencv_photo opencv_imgproc opencv_core)
	
include_directories(/usr/include/opencv4/)
if(COMPILE_STATIC)
    add_library(cv_tracking STATIC cv_tracking.cpp)
endif()
if(COMPILE_SHARED)
    add_library(cv_tracking SHARED cv_tracking.cpp)
endif()
