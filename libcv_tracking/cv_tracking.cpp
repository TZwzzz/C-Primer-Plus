#include "cv_tracking.h"

cv::Ptr<cv::Tracker> tracker;

static int cv_tracking_create(cv::Rect2d rect2d, int tracking_algo = KCF)
{
    using namespace cv;

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
    using namespace cv;

    Mat cv_frame = Mat(f_height, f_width, CV_8UC3, src_mb);
    if (cv_frame.rows == 0 || cv_frame.cols == 0)
    {
        fprintf(stderr, "cv_frame.rows == 0 || cv_frame.cols == 0\r\n");
        exit(EXIT_FAILURE);
    }

    if(MOSSE == tracking_algo || KCF == tracking_algo)
    {
        if (*tracking_init)
        {
            *tracking_init = false;
            cv_tracking_create(roi, tracking_algo);
            printf("cv_tracking_create done!\r\n");
            tracker->init(cv_frame, roi);
            printf("tracker_init done!\r\n");
            return 0;
        }
        bool ret = tracker->update(cv_frame, roi);
        printf("update %s\r\n",ret?"successfully":"failed");
        return 0;
    }
    //以下代码还有问题，无法使用MEANSHIFT跟踪
    else if(MEANSHIFT == tracking_algo)
    {
        int hsize = 16;
        float hranges[] = {0,180};
        static const float *phranges = hranges;
        static Mat hsv,hue,hist,backproj;
        Rect selection_Cam = roi;
        printf("input roi: %.2f %.2f %.2f %.2f\n",roi.x,roi.y,roi.width,roi.height);
        printf("input selection: %d %d %d %d\n",selection_Cam.x,selection_Cam.y,selection_Cam.width,selection_Cam.height);

        //将图像转化成HSV颜色空间
        cvtColor(cv_frame,hsv,COLOR_BGR2HSV);
        //定义计算直方图和反向直方图的相关数据和图像
        int ch[] = {0,0};
        hue.create(hsv.size(),hsv.depth());
        mixChannels(&hsv,1,&hue,1,ch,1);
        if (*tracking_init)
        {
            *tracking_init = false;
            //目标区域的HSV颜色空间
            Mat roi_temp(hue,selection_Cam);
            //计算直方图和直方图归一化
            calcHist(&roi_temp,1,0,roi_temp,hist,1,&hsize,&phranges);
            normalize(hist,hist,0,255,NORM_MINMAX);
            printf("meanshift init done!\r\n");
        }
        //计算目标区域的反向直方图
        calcBackProject(&hue,1,0,hist,backproj,&phranges);
        //均值迁移法跟踪目标
        meanShift(backproj,selection_Cam,TermCriteria(TermCriteria::EPS|TermCriteria::COUNT,10,1));
        roi = selection_Cam;
        printf("output roi: %.2f %.2f %.2f %.2f\n",roi.x,roi.y,roi.width,roi.height);
        printf("output selection: %d %d %d %d\n",selection_Cam.x,selection_Cam.y,selection_Cam.width,selection_Cam.height);
        return 0;
    }
    
}


