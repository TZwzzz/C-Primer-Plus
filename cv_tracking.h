#ifndef KCF_TRACKING_H_
#define KCF_TRACKING_H

#include "opencv2/opencv.hpp"
#include "opencv2/tracking.hpp"

#define KCF     0
#define MOSSE   1

#define PRINT_ROI 0

int cv_tracking(void *src_mb,int f_width,int f_height,
                  int roi_x1,int roi_y1,int width,int height,int tracking_algo = KCF);

#endif