#ifndef KCF_TRACKING_H_
#define KCF_TRACKING_H

#include "opencv2/opencv.hpp"
#include "opencv2/tracking.hpp"

#define KCF     0
#define MOSSE   1

#define PRINT_ROI       0
#define DRAW_ROI        0
#define PRINT_UPDATE    0

cv::Rect2d cv_tracking(void *src_mb,int f_width,int f_height,
                  cv::Rect2d roi,bool *tracking_init,int tracking_algo = KCF);

#endif