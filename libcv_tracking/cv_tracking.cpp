#include "cv_tracking.h"

using namespace cv;
cv::Ptr<cv::Tracker> tracker;

static bool Print_roi = false;
static bool Draw_result_rect = false;
static bool Print_update = false;

static int cv_tracking_create(cv::Rect2d rect2d, int tracking_algo = KCF)
{
    switch (tracking_algo)
    {
    case KCF:
        tracker = TrackerKCF::create();
        break;
    case MOSSE:
        tracker = TrackerMOSSE::create();
        break;
    default:
        fprintf(stderr, "Invalid tracking algorithm");
        exit(EXIT_FAILURE);
    }
    return 0;
}

int cv_tracking(void *src_mb, int f_width, int f_height,
                       cv::Rect2d &roi, bool *tracking_init, int tracking_algo)
{
    if (*tracking_init)
    {
        *tracking_init = false;
        cv_tracking_create(roi, tracking_algo);
        printf("cv_tracking_create done!\r\n");
        Mat cv_frame = Mat(f_height, f_width, CV_8UC3, src_mb);
        if (cv_frame.rows == 0 || cv_frame.cols == 0)
        {
            fprintf(stderr, "cv_frame.rows == 0 || cv_frame.cols == 0\r\n");
            exit(EXIT_FAILURE);
        }
        tracker->init(cv_frame, roi);
        printf("tracker_init done!\r\n");
        return 0;
    }
    Mat cv_frame = Mat(f_height, f_width, CV_8UC3, src_mb);
    if (cv_frame.rows == 0 || cv_frame.cols == 0)
    {
        fprintf(stderr, "cv_frame.rows == 0 || cv_frame.cols == 0\r\n");
        exit(EXIT_FAILURE);
    }
    bool ret = tracker->update(cv_frame, roi);
    printf("update %s\r\n",ret?"successfully":"failed");
    return 0;
}

