#include "cv_tracking.h"

using namespace cv;
struct tracking_stu
{
    cv::Mat cv_frame;
    cv::Rect2d cv_roi;
    cv::Ptr<cv::Tracker> tracker;
};
tracking_stu tracker_stu;

static int cv_tracking_init(cv::Rect2d rect2d,int tracking_algo = KCF)
{
  tracker_stu.cv_roi = rect2d;
  switch(tracking_algo)
  {
    case KCF:
      tracker_stu.tracker = TrackerKCF::create();
      break;
    case MOSSE:
      tracker_stu.tracker = TrackerMOSSE::create();
      break;
    default:
      fprintf(stderr, "Invalid tracking algorithm");
      exit(EXIT_FAILURE);
  }
  return 0;
}

cv::Rect2d cv_tracking(void *src_mb,int f_width,int f_height,
                  cv::Rect2d roi,bool *tracking_init,int tracking_algo)
{
  if(*tracking_init)
  {
    *tracking_init = false;
#if PRINT_ROI
    printf("Before cv_tracking_init,roi_x:%d  roi_y:%d  width:%d  height:%d\r\n",roi.x,roi.y,roi.width,roi.height);
#endif
    cv_tracking_init(roi,tracking_algo);
    tracker_stu.cv_frame = Mat(f_height, f_width, CV_8UC3, src_mb);
    if (tracker_stu.cv_frame.rows == 0 || tracker_stu.cv_frame.cols == 0)
    {
      fprintf(stderr,"tracker_stu.cv_frame.rows == 0 || tracker_stu.cv_frame.cols == 0\r\n");
      exit(EXIT_FAILURE);
    }
#if PRINT_ROI
    printf("Before tracker->init,roi_x:%.2f  roi_y:%.2f  width:%.2f  height:%.2f\r\n",
            tracker_stu.cv_roi.x,tracker_stu.cv_roi.y,tracker_stu.cv_roi.width,tracker_stu.cv_roi.height);
#endif
    tracker_stu.tracker->init(tracker_stu.cv_frame,tracker_stu.cv_roi);
  }
  tracker_stu.cv_frame = Mat(f_height, f_width, CV_8UC3, src_mb);
  if (tracker_stu.cv_frame.rows == 0 || tracker_stu.cv_frame.cols == 0)
  {
    fprintf(stderr,"tracker_stu.cv_frame.rows == 0 || tracker_stu.cv_frame.cols == 0\r\n");
    exit(EXIT_FAILURE);
  }
#if PRINT_ROI
  printf("Before tracker->update,roi_x:%.2f  roi_y:%.2f  width:%.2f  height:%.2f\r\n",
            tracker_stu.cv_roi.x,tracker_stu.cv_roi.y,tracker_stu.cv_roi.width,tracker_stu.cv_roi.height);
#endif
  tracker_stu.tracker->update(tracker_stu.cv_frame,tracker_stu.cv_roi);
  printf("tracker update done\r\n");
#if PRINT_ROI
  printf("After tracker->update,roi_x:%.2f  roi_y:%.2f  width:%.2f  height:%.2f\r\n",
            tracker_stu.cv_roi.x,tracker_stu.cv_roi.y,tracker_stu.cv_roi.width,tracker_stu.cv_roi.height);
#endif
#if DRAW_ROI
  rectangle(tracker_stu.cv_frame,tracker_stu.cv_roi,Scalar(255,0,0),5,8,0);
#endif
  return tracker_stu.cv_roi;
}