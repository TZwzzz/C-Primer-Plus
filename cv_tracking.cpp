#include "cv_tracking.h"

using namespace cv;
struct tracking_stu
{
    cv::Mat cv_frame;
    cv::Rect2d cv_roi;
    cv::Ptr<cv::Tracker> tracker;
};
tracking_stu tracker_stu;

static int cv_tracking_create(cv::Rect2d rect2d,int tracking_algo = KCF)
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
    if(PRINT_ROI)
    {
      printf("Before cv_tracking_create,roi_x:%.2f  roi_y:%.2f  width:%.2f  height:%.2f\r\n",roi.x,roi.y,roi.width,roi.height);
    }
    cv_tracking_create(roi,tracking_algo);
    printf("cv_tracking_create done!\r\n");
    tracker_stu.cv_roi = roi;
    tracker_stu.cv_frame = Mat(f_height, f_width, CV_8UC3, src_mb);
    if (tracker_stu.cv_frame.rows == 0 || tracker_stu.cv_frame.cols == 0)
    {
      fprintf(stderr,"tracker_stu.cv_frame.rows == 0 || tracker_stu.cv_frame.cols == 0\r\n");
      exit(EXIT_FAILURE);
    }
    if(PRINT_ROI)
    {
      printf("Before tracker->init,roi_x:%.2f  roi_y:%.2f  width:%.2f  height:%.2f\r\n",
            tracker_stu.cv_roi.x,tracker_stu.cv_roi.y,tracker_stu.cv_roi.width,tracker_stu.cv_roi.height);
    }
    tracker_stu.tracker->init(tracker_stu.cv_frame,tracker_stu.cv_roi);
    printf("tracker_init done!\r\n");
    return tracker_stu.cv_roi;
  }
  tracker_stu.cv_frame = Mat(f_height, f_width, CV_8UC3, src_mb);
  if (tracker_stu.cv_frame.rows == 0 || tracker_stu.cv_frame.cols == 0)
  {
    fprintf(stderr,"tracker_stu.cv_frame.rows == 0 || tracker_stu.cv_frame.cols == 0\r\n");
    exit(EXIT_FAILURE);
  }
  if(PRINT_ROI)
  {
    printf("Before tracker->update,roi_x:%.2f  roi_y:%.2f  width:%.2f  height:%.2f\r\n",
          tracker_stu.cv_roi.x,tracker_stu.cv_roi.y,tracker_stu.cv_roi.width,tracker_stu.cv_roi.height);
  }
  tracker_stu.tracker->update(tracker_stu.cv_frame,tracker_stu.cv_roi);
  if(PRINT_UPDATE)
  {
    printf("tracker update done\r\n");
  }
  if(PRINT_ROI)
  {
    printf("After tracker->update,roi_x:%.2f  roi_y:%.2f  width:%.2f  height:%.2f\r\n",
          tracker_stu.cv_roi.x,tracker_stu.cv_roi.y,tracker_stu.cv_roi.width,tracker_stu.cv_roi.height);
  }
  if(DRAW_ROI)
  {
    rectangle(tracker_stu.cv_frame,tracker_stu.cv_roi,Scalar(255,0,0),5,8,0);
  }
  
  return tracker_stu.cv_roi;
}
