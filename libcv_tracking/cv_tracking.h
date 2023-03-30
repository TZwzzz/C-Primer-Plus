#ifndef KCF_TRACKING_H_
#define KCF_TRACKING_H_

#include "opencv2/opencv.hpp"
#include "opencv2/tracking.hpp"

#define KCF     0
#define MOSSE   1

cv::Rect2d cv_tracking(void *src_mb,int f_width,int f_height,
                  cv::Rect2d roi,bool *tracking_init,int tracking_algo = KCF);
void cv_tracking_log(bool print_roi = false,bool print_update = false,bool draw_result_rect = false);

#endif
