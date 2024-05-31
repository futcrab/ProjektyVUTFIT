extern "C" {
#include <libavformat/avformat.h>
}

int main(int argc, const char *argv[]) {
    av_register_all();

    const char *filename = argc > 1 ? argv[1] : "clock.avi";

    av_log(NULL, AV_LOG_INFO, "Opening %s...\n", filename);

    AVFormatContext *pFormatCtx = NULL;

    if (avformat_open_input(&pFormatCtx, filename, NULL, NULL))
        abort();

    if (avformat_find_stream_info(pFormatCtx, NULL) < 0)
        abort();

    av_dump_format(pFormatCtx, 0, filename, 0);

    int videoStream = av_find_best_stream(pFormatCtx, AVMEDIA_TYPE_VIDEO, -1, -1, NULL, 0);
    if (-1 == videoStream)
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

    AVPacket pkt;

    while (av_read_frame(pFormatCtx, &pkt) == 0) {
        if (pkt.stream_index == videoStream) {
            int frameFinished = 0;

            if (avcodec_decode_video2(pCodecCtx, pFrame, &frameFinished, &pkt) < 0)
                abort();

            if (frameFinished) {
                av_log(NULL, AV_LOG_INFO, "Frame decoded\n");

                // ...
            }
        }
        av_free_packet(&pkt);
    }

    av_log(NULL, AV_LOG_INFO, "Done\n");

    av_free(pFrame);
    avcodec_close(pCodecCtx);
    avformat_close_input(&pFormatCtx);

    return 0;
}
