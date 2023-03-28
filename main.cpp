// Copyright 2020 Fuzhou Rockchip Electronics Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "main.h"


int main(int argc, char *argv[])
{
  RK_CHAR *pDeviceName_01 = "rkispp_scale0";
  RK_CHAR *pDeviceName_02 = "rkispp_scale1";
  RK_CHAR *pcDevNode = "/dev/dri/card0";
  char *iq_file_dir = "/etc/iqfiles";
  RK_S32 s32CamId = 0;
  RK_U32 u32BufCnt = 3;
  RK_U32 fps = 20;
  int ret;

  rga_arg_t rga_arg;
  rga_arg.u32SrcWidth = video_width;
  rga_arg.u32SrcHeight = video_height;
  rga_arg.u32RgaX = result_rect2d.x;
  rga_arg.u32RgaY = result_rect2d.y;
  rga_arg.u32RgaWidth = result_rect2d.width;
  rga_arg.u32RgaHeight = result_rect2d.height;

  printf("\n###############################################\n");
  printf("VI CameraIdx: %d\npDeviceName: %s\nResolution: %dx%d\n\n",
          s32CamId,pDeviceName_01,video_width,video_height);
  printf("###############################################\n\n");

  if (iq_file_dir) 
  {
#ifdef RKAIQ
  printf("#Rkaiq XML DirPath: %s\n", iq_file_dir);
  rk_aiq_working_mode_t hdr_mode = RK_AIQ_WORKING_MODE_NORMAL;
  SAMPLE_COMM_ISP_Init(s32CamId,hdr_mode, RK_FALSE,iq_file_dir);
  SAMPLE_COMM_ISP_Run(s32CamId);
  SAMPLE_COMM_ISP_SetFrameRate(s32CamId,25);
#endif
  }
  // init rtsp
  g_rtsplive = create_rtsp_demo(554);
  g_rtsp_session = rtsp_new_session(g_rtsplive, "/live/main_stream");
  if (enCodecType == RK_CODEC_TYPE_H264)
  {
    rtsp_set_video(g_rtsp_session, RTSP_CODEC_ID_VIDEO_H264, NULL, 0);
  }
  else if (enCodecType == RK_CODEC_TYPE_H265)
  {
    rtsp_set_video(g_rtsp_session, RTSP_CODEC_ID_VIDEO_H265, NULL, 0);
  }
  else
  {
    printf("not support other type\n");
    return -1;
  }
  rtsp_sync_video_ts(g_rtsp_session, rtsp_get_reltime(), rtsp_get_ntptime());


  RK_MPI_SYS_Init();
  VI_CHN_ATTR_S vi_chn_attr_01;
  memset(&vi_chn_attr_01, 0, sizeof(vi_chn_attr_01));
  vi_chn_attr_01.pcVideoNode = pDeviceName_01;
  vi_chn_attr_01.u32BufCnt = u32BufCnt;
  vi_chn_attr_01.u32Width = video_width;
  vi_chn_attr_01.u32Height = video_height;
  vi_chn_attr_01.enPixFmt = IMAGE_TYPE_NV12;
  vi_chn_attr_01.enBufType = VI_CHN_BUF_TYPE_MMAP;
  vi_chn_attr_01.enWorkMode = VI_WORK_MODE_NORMAL;
  ret = RK_MPI_VI_SetChnAttr(s32CamId, 0, &vi_chn_attr_01);
  ret |= RK_MPI_VI_EnableChn(s32CamId, 0);
  if (ret)
  {
    printf("ERROR: create VI[0:0] error! ret=%d\n", ret);
    return 0;
  }

  RGA_ATTR_S stRgaAttr_01;
  memset(&stRgaAttr_01, 0, sizeof(stRgaAttr_01));
  stRgaAttr_01.bEnBufPool = RK_TRUE;
  stRgaAttr_01.u16BufPoolCnt = 3;
  stRgaAttr_01.u16Rotaion = 0;
  stRgaAttr_01.stImgIn.u32X = 0;
  stRgaAttr_01.stImgIn.u32Y = 0;
  stRgaAttr_01.stImgIn.imgType = IMAGE_TYPE_NV12;
  stRgaAttr_01.stImgIn.u32Width = video_width;
  stRgaAttr_01.stImgIn.u32Height = video_height;
  stRgaAttr_01.stImgIn.u32HorStride = video_width;
  stRgaAttr_01.stImgIn.u32VirStride = video_height;
  stRgaAttr_01.stImgOut.u32X = 0;
  stRgaAttr_01.stImgOut.u32Y = 0;
  stRgaAttr_01.stImgOut.imgType = IMAGE_TYPE_RGB888;
  stRgaAttr_01.stImgOut.u32Width = video_width;
  stRgaAttr_01.stImgOut.u32Height = video_height;
  stRgaAttr_01.stImgOut.u32HorStride = video_width;
  stRgaAttr_01.stImgOut.u32VirStride = video_height;
  ret = RK_MPI_RGA_CreateChn(0, &stRgaAttr_01);
  if (ret) 
  {
    printf("ERROR: create RGA[0:0] falied! ret=%d\n", ret);
    return -1;
  }

  VENC_CHN_ATTR_S venc_chn_attr;
  memset(&venc_chn_attr, 0, sizeof(venc_chn_attr));
  switch (enCodecType)
  {
  case RK_CODEC_TYPE_H265:
    venc_chn_attr.stVencAttr.enType = RK_CODEC_TYPE_H265;
    venc_chn_attr.stRcAttr.enRcMode = VENC_RC_MODE_H265CBR;
    venc_chn_attr.stRcAttr.stH265Cbr.u32Gop = 30;
    venc_chn_attr.stRcAttr.stH265Cbr.u32BitRate = video_width * video_height;
    // frame rate: in 30/1, out 30/1.
    venc_chn_attr.stRcAttr.stH265Cbr.fr32DstFrameRateDen = 1;
    venc_chn_attr.stRcAttr.stH265Cbr.fr32DstFrameRateNum = 30;
    venc_chn_attr.stRcAttr.stH265Cbr.u32SrcFrameRateDen = 1;
    venc_chn_attr.stRcAttr.stH265Cbr.u32SrcFrameRateNum = 30;
    break;
  case RK_CODEC_TYPE_H264:
  default:
    venc_chn_attr.stVencAttr.enType = RK_CODEC_TYPE_H264;
    venc_chn_attr.stRcAttr.enRcMode = VENC_RC_MODE_H264CBR;
    venc_chn_attr.stRcAttr.stH264Cbr.u32Gop = 30;
    venc_chn_attr.stRcAttr.stH264Cbr.u32BitRate = video_width * video_height * 3;
    // frame rate: in 30/1, out 30/1.
    venc_chn_attr.stRcAttr.stH264Cbr.fr32DstFrameRateDen = 1;
    venc_chn_attr.stRcAttr.stH264Cbr.fr32DstFrameRateNum = 30;
    venc_chn_attr.stRcAttr.stH264Cbr.u32SrcFrameRateDen = 1;
    venc_chn_attr.stRcAttr.stH264Cbr.u32SrcFrameRateNum = 30;
    break;
  }

  venc_chn_attr.stVencAttr.imageType = IMAGE_TYPE_NV12;
  venc_chn_attr.stVencAttr.u32PicWidth = video_width;
  venc_chn_attr.stVencAttr.u32PicHeight = video_height;
  venc_chn_attr.stVencAttr.u32VirWidth = video_width;
  venc_chn_attr.stVencAttr.u32VirHeight = video_height;
  venc_chn_attr.stVencAttr.u32Profile = 66;
  ret = RK_MPI_VENC_CreateChn(0, &venc_chn_attr);
  if (ret)
  {
    printf("ERROR: create VENC[0:0] error! ret=%d\n", ret);
    return -1;
  }

  MPP_CHN_S stSrcChn;
  MPP_CHN_S stDestChn;
  printf("Bind VI[0:0] to RGA[0:0]....\n");
  stSrcChn.enModId = RK_ID_VI;
  stSrcChn.s32DevId = s32CamId;
  stSrcChn.s32ChnId = 0;
  stDestChn.enModId = RK_ID_RGA;
  stSrcChn.s32DevId = s32CamId;
  stDestChn.s32ChnId = 0;
  ret = RK_MPI_SYS_Bind(&stSrcChn, &stDestChn);
  if (ret) 
  {
    printf("ERROR: bind VI[0:0] to RGA[0:0] failed! ret=%d\n", ret);
    return -1;
  }
  pthread_t rkmedia_tidp;
  pthread_t tracking_tidp;
  pthread_t rtsp_tidp;              // rtsp线程tid
  pthread_t key_tidp;
  pthread_create(&rkmedia_tidp, NULL, rkmedia_thread, &rga_arg);
  pthread_create(&tracking_tidp, NULL, tracking_thread, &rga_arg);
  pthread_create(&rtsp_tidp, NULL, venc_rtsp_tidp, NULL);
  pthread_create(&key_tidp, NULL, key_thread, NULL);
  printf("%s initial finish\n", __func__);

  while (!quit)
  {
    usleep(500000);
  }

  printf("UnBind VI[0:0] to RGA[0:0]....\n");
  stSrcChn.enModId = RK_ID_VI;
  stSrcChn.s32DevId = s32CamId;
  stSrcChn.s32ChnId = 0;
  stDestChn.enModId = RK_ID_RGA;
  stSrcChn.s32DevId = s32CamId;
  stDestChn.s32ChnId = 0;
  ret = RK_MPI_SYS_UnBind(&stSrcChn, &stDestChn);
  if (ret) 
  {
    printf("ERROR: unbind VI[0:0] to RGA[0:0] failed! ret=%d\n", ret);
    return -1;
  }

  printf("Destroy VENC[0:0] channel\n");
  ret = RK_MPI_VENC_DestroyChn(0);
  if (ret)
  {
    printf("ERROR: Destroy VENC[0:0] error! ret=%d\n", ret);
    return 0;
  }

  RK_MPI_RGA_DestroyChn(0);
  RK_MPI_VI_DisableChn(s32CamId, 0);
  
  if (iq_file_dir) 
  {
#if RKAIQ
    SAMPLE_COMM_ISP_Stop(s32CamId);
#endif
  }
  return 0;
}

/*************************************************************************/
// not set buffer 0x00000000 before draw
static int RGA_Rect_draw2(rga_buffer_t buf, RK_U32 x, RK_U32 y,
                                RK_U32 width, RK_U32 height,
                                RK_U32 line_pixel) {
  im_rect rect_up = {(int)x, (int)y, (int)width, (int)line_pixel};
  im_rect rect_buttom = {(int)x, (int)(y + height - line_pixel), (int)width, (int)line_pixel};
  im_rect rect_left = {(int)x, (int)y, (int)line_pixel, (int)height};
  im_rect rect_right = {(int)(x + width - line_pixel), (int)y, (int)line_pixel, (int)height};
  int STATUS = imfill(buf, rect_up, 0x0000ff00);
  STATUS |= imfill(buf, rect_buttom, 0x0000ff00);
  STATUS |= imfill(buf, rect_left, 0x0000ff00);
  STATUS |= imfill(buf, rect_right, 0x0000ff00);
  return STATUS;
}

static IM_STATUS RGA_Clear_Rect(rga_buffer_t buf, RK_U32 width, RK_U32 height) {
  im_rect rect_all = {0, 0, (int)width, (int)height};
  IM_STATUS STATUS = imfill(buf, rect_all, 0x00000000);
  return STATUS;
}
/****************************************************************************/

void *rkmedia_thread(void *args)
{
  pthread_detach(pthread_self());

  int ret;
  rga_arg_t *rga_arg = (rga_arg_t *)args;

  while (!quit)
  {
    MEDIA_BUFFER src_mb = NULL;
    
    src_mb = RK_MPI_SYS_GetMediaBuffer(RK_ID_VI, 0, -1);
    if (!src_mb)
    {
      printf("ERROR: RK_MPI_SYS_GetMediaBuffer get null buffer!\n");
      break;
    }
    /********************************/
    rga_buffer_t pat;
    rga_buffer_t src;
    MEDIA_BUFFER pat_mb = NULL;
    int STATUS;
    MB_IMAGE_INFO_S stImageInfo = {
        rga_arg->u32SrcWidth, rga_arg->u32SrcHeight, rga_arg->u32SrcWidth,
        rga_arg->u32SrcHeight, IMAGE_TYPE_ARGB8888};
    pat_mb = RK_MPI_MB_CreateImageBuffer(&stImageInfo, RK_TRUE, 0);
    if (!pat_mb) 
    {
      printf("ERROR: RK_MPI_MB_CreateImageBuffer get null buffer!\n");
      break;
    }
    src = wrapbuffer_fd(RK_MPI_MB_GetFD(src_mb), rga_arg->u32SrcWidth,
                        rga_arg->u32SrcHeight, RK_FORMAT_YCbCr_420_SP);
    pat = wrapbuffer_fd(RK_MPI_MB_GetFD(pat_mb), rga_arg->u32SrcWidth,
                              rga_arg->u32SrcHeight, RK_FORMAT_RGBA_8888);
    RGA_Clear_Rect(pat, rga_arg->u32SrcWidth, rga_arg->u32SrcHeight);
    rga_arg->u32RgaX = (RK_U32)result_rect2d.x;
    rga_arg->u32RgaY = (RK_U32)result_rect2d.y;
    rga_arg->u32RgaWidth = (RK_U32)result_rect2d.width;
    rga_arg->u32RgaHeight = (RK_U32)result_rect2d.height;
    if(rga_arg->u32RgaX + rga_arg->u32RgaWidth > rga_arg->u32SrcWidth)
    {
      rga_arg->u32RgaX = rga_arg->u32SrcWidth - rga_arg->u32RgaWidth;
    }
    if(rga_arg->u32RgaY + rga_arg->u32RgaHeight > rga_arg->u32SrcHeight)
    {
      rga_arg->u32RgaY = rga_arg->u32SrcHeight - rga_arg->u32RgaHeight;
    }
    STATUS = RGA_Rect_draw2(pat, rga_arg->u32RgaX, rga_arg->u32RgaY,
                        rga_arg->u32RgaWidth, rga_arg->u32RgaHeight, 5);
    if (STATUS != IM_STATUS_SUCCESS)
      printf(">>>>>>>>>>>>>>>>RGA_Rect_draw failed: %s\n",imStrError(STATUS));
    STATUS = imcomposite(src, pat, src, IM_ALPHA_BLEND_DST_OVER);
    if (STATUS != IM_STATUS_SUCCESS) 
    {
      printf(">>>>>>>>>>>>>>>>imblend failed: %s\n", imStrError(STATUS));
      RK_MPI_MB_ReleaseBuffer(pat_mb);
      RK_MPI_MB_ReleaseBuffer(src_mb);
      quit = true;
      break;
    }
    RK_MPI_SYS_SendMediaBuffer(RK_ID_VENC, 0, src_mb);
    RK_MPI_MB_ReleaseBuffer(pat_mb);
    RK_MPI_MB_ReleaseBuffer(src_mb);

    /********************************/
    src_mb = NULL;
  }
  return NULL;
}

void *tracking_thread(void *args)
{
  void *frame;
  rga_arg_t *rga_arg = (rga_arg_t *)args;
  cv::Rect2d roi;
  roi.x = (double)rga_arg->u32RgaX;
  roi.y = (double)rga_arg->u32RgaY;
  roi.width = (double)rga_arg->u32RgaWidth;
  roi.height = (double)rga_arg->u32RgaHeight;

  while(1)
  {
    if(pressKey == true)
    {
      printf("Press Key\r\n");
      break;
    }
    usleep(500000);
  }
  while(!quit)
  {
    MEDIA_BUFFER src_mb = NULL;
    
    src_mb = RK_MPI_SYS_GetMediaBuffer(RK_ID_RGA, 0, -1);
    if (!src_mb)
    {
      printf("ERROR: RK_MPI_SYS_GetMediaBuffer get null buffer!\n");
      break;
    }
    printf("get src_mb\r\n");
    frame = RK_MPI_MB_GetPtr(src_mb);

    clock_t  startTime = clock();
    result_rect2d = cv_tracking(frame,video_width,video_height,roi,&tracking_init,MOSSE);
    clock_t endTime = clock();
    double totalTime = (double)(endTime - startTime) / CLOCKS_PER_SEC;
    cout << totalTime << "s" << endl;

    RK_MPI_MB_ReleaseBuffer(src_mb);
    src_mb = NULL;
  }
  
}

void *venc_rtsp_tidp(void *args)
{
  pthread_detach(pthread_self()); // 将线程状态改为unjoinable状态，确保资源的释放

  MEDIA_BUFFER mb = NULL;

  while (!quit)
  {
    mb = RK_MPI_SYS_GetMediaBuffer(RK_ID_VENC, 0, -1);
    if (!mb)
    {
      printf("ERROR: RK_MPI_SYS_GetMediaBuffer get null buffer!\n");
      break;
    }
    rtsp_tx_video(g_rtsp_session, (unsigned char *)RK_MPI_MB_GetPtr(mb), RK_MPI_MB_GetSize(mb), RK_MPI_MB_GetTimestamp(mb));
    RK_MPI_MB_ReleaseBuffer(mb);
    rtsp_do_event(g_rtsplive);
  }
  return NULL;
}

void *key_thread(void *args)
{
  printf("please enter 'y' to continue\r\n");
  while(1)
  {
    if('y' == getchar())
    {
      pressKey = true;
      break;
    }
    usleep(100);
  }
  pthread_exit(NULL);
}
