#ifndef _ATK_SSD_OBJECT_RECOGNIZE_H
#define _ATK_SSD_OBJECT_RECOGNIZE_H

#include <getopt.h>
#include <math.h>
#include <pthread.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <malloc.h>
#include <time.h>

#include "im2d.h"
#include "rga.h"
#include "rknn_api.h"
#include "rkmedia_api.h"
#include "sample_common.h"
#include "opencv2/opencv.hpp"
#include "librtsp/rtsp_demo.h"
#include "tslib.h"
#include "opencv2/tracking.hpp"
#include "cv_tracking.h"
#include "postprocess.h"
#include "drm_func.h"
#include "rga_func.h"

#define _BASETSD_H
#define MODEL_INPUT_SIZE 300

using namespace std;

typedef struct rga_arg_s {
  RK_U32 u32SrcWidth;
  RK_U32 u32SrcHeight;
  RK_U32 u32RgaX;
  RK_U32 u32RgaY;
  RK_U32 u32RgaWidth;
  RK_U32 u32RgaHeight;
  RK_U32 u32Mode;
} rga_arg_t;

char *track_name[] = 
    {
        // "cup"
        "truck",
        "bus",
        // "bench",
        "car",
        // "suitcase",
        "train"
    };

bool tracking_init = true;
cv::Rect2d result_rect2d;

RK_U32 video_width = 640;
RK_U32 video_height = 360;
rtsp_demo_handle g_rtsplive = NULL;
static rtsp_session_handle g_rtsp_session;
CODEC_TYPE_E enCodecType = RK_CODEC_TYPE_H264;



//用于触摸屏配置
struct tsdev *ts = NULL;
struct ts_sample samp;

bool pressKey = false;
static bool quit = false;

void *object_recognize_thread(void *args);
void *rkmedia_thread(void *args);
void *tracking_thread(void *args);
void *venc_rtsp_tidp(void *args);
void *key_thread(void *args);

#endif
