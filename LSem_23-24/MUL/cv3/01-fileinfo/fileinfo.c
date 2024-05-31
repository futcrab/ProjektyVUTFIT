#include <libavformat/avformat.h>

int main(int argc, const char *argv[])
{
    av_register_all();

    const char *filename = argc > 1 ? argv[1] : "clock.avi";

    av_log(NULL, AV_LOG_INFO, "Opening %s...\n", filename);

    AVFormatContext *pFormatCtx = NULL;

    if (avformat_open_input(&pFormatCtx, filename, NULL, NULL))
        abort();

    if (avformat_find_stream_info(pFormatCtx, NULL) < 0)
        abort();

    av_dump_format(pFormatCtx, 0, filename, 0);

    avformat_close_input(&pFormatCtx);

    return 0;
}
