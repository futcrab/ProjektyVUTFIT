#include <stdint.h>

extern "C" {
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
}

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

using namespace cv;

static AVFrame *alloc_picture(enum AVPixelFormat pix_fmt, int width, int height) {
    AVFrame *picture;
    uint8_t *picture_buf;
    int size;

    picture = av_frame_alloc();
    if (!picture)
        return NULL;

    size = avpicture_get_size(pix_fmt, width, height);

    picture_buf = (uint8_t *) av_malloc(size);
    if (!picture_buf) {
        av_free(picture);
        return NULL;
    }

    avpicture_fill((AVPicture *) picture, picture_buf, pix_fmt, width, height);

    return picture;
}

int main(int argc, const char *argv[]) {
    av_register_all();

    const char *filename = argc>1 ? argv[1] : "clock.avi";

    av_log(NULL, AV_LOG_INFO, "Opening %s...\n", filename);

    AVFormatContext *pFormatCtx = NULL;

    if (avformat_open_input(&pFormatCtx, filename, NULL, NULL))
        abort();

    if (avformat_find_stream_info(pFormatCtx, NULL) < 0)
        abort();

    av_dump_format(pFormatCtx, 0, filename, 0);

    int videoStream = av_find_best_stream(pFormatCtx, AVMEDIA_TYPE_VIDEO, -1, -1, NULL, 0);
    if (videoStream < 0)
        abort();

    av_log(NULL, AV_LOG_INFO, "Video stream has #%i\n", videoStream);

    AVCodecContext *pCodecCtx = pFormatCtx->streams[videoStream]->codec;

    AVCodec *pCodec = avcodec_find_decoder(pCodecCtx->codec_id);
    if (NULL == pCodec)
        abort();

    if (avcodec_open2(pCodecCtx, pCodec, NULL))
        abort();

    AVFrame *pFrame = av_frame_alloc();
    if (!pFrame)
        abort();

    AVFrame *pFrameRGB = alloc_picture(AV_PIX_FMT_RGB24, pCodecCtx->width, pCodecCtx->height);
    if (!pFrameRGB)
        abort();

    Mat img(pCodecCtx->height, pCodecCtx->width, CV_8UC3, (void *)pFrameRGB->data[0], pFrameRGB->linesize[0]);

    AVPacket pkt;
    struct SwsContext *c = NULL;

    while (av_read_frame(pFormatCtx, &pkt) == 0) {
        if (pkt.stream_index == videoStream) {
            int frameFinished = 0;

            if (avcodec_decode_video2(pCodecCtx, pFrame, &frameFinished, &pkt) < 0)
                abort();

            if (frameFinished) {
                av_log(NULL, AV_LOG_INFO, "Frame decoded\n");

                c = sws_getCachedContext(c, pCodecCtx->width, pCodecCtx->height, (AVPixelFormat)pFrame->format, pCodecCtx->width, pCodecCtx->height, AV_PIX_FMT_BGR24, SWS_BICUBIC, NULL, NULL, NULL);
                sws_scale(c, (const uint8_t * const*)pFrame->data, pFrame->linesize, 0, pFrame->height, pFrameRGB->data, pFrameRGB->linesize);

                imshow("image", img);
                waitKey();
            }
        }
        av_free_packet(&pkt);
    }

    sws_freeContext(c);

    av_log(NULL, AV_LOG_INFO, "Done\n");

    av_free(pFrame);
    av_free(pFrameRGB->data[0]);
    av_free(pFrameRGB);

    avcodec_close(pCodecCtx);
    avformat_close_input(&pFormatCtx);

    destroyWindow("image");

    return 0;
}
