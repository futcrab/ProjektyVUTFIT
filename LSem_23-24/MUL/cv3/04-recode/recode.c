/*
 * Cviceni MUL c. 2 (2016) - Prekodovani videa pomoci FFmpeg
 *
 * Kostra programu pro prekodovani vstupniho videa do formatu HuffYUV
 *
 * Ukol:
 * -----
 * 1. Pridat funkci pro vypsani informaci o vstupnim kontejneru.
 * 2. Doplnit nastaveni koderu, pro zakodovani videa do vysledneho formatu.
 * 3. Pridat inicializaci vystupniho kodek kontextu - pouzije se vystupni kodek.
 * 4. Doplnte kompresi snimku.
 *
 * Piste vlastni kod jen do vyznacenych oblasti.
 *
 * Pozn.: Pouzit FFmpeg N-51503-g0bf8580.
 */

#include <libavformat/avformat.h>
#include <libswscale/swscale.h>

// Inicializuje strukturu AVFrame podle velikosti a pixeloveho formatu formatu.
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

    const char *filename = argc > 1 ? argv[1] : "clock.avi";
    const char *filename_out = argc > 2 ? argv[2] : "output.avi";

    av_log(NULL, AV_LOG_INFO, "Opening %s...\n", filename);

    // Zaobaluje multimedialni kontejner pro vstupni format.
    AVFormatContext *pFormatCtx = NULL;

    // Otevre vstupni kontejner (napr. soubor) a precte z nej hlavicku.
    if (avformat_open_input(&pFormatCtx, filename, NULL, NULL))
        abort();

    // Zaobaluje multimedialni kontejner pro vystupni format.
    AVFormatContext *pFormatCtx_out;

    // Pro vystupni format alokuje strukturu AVFormatContext.
    if (avformat_alloc_output_context2(&pFormatCtx_out, NULL, NULL, filename_out) < 0)
        abort();

    // Nacte ze vstupniho kontejneru ruzne informace. Uzitecne napr. pro funkci av_dump_format.
    if (avformat_find_stream_info(pFormatCtx, NULL) < 0)
        abort();

    // 1. bod ukolu
    // vypsani informaci o vstupnim kontejneru
    // vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv
        av_dump_format(pFormatCtx, 0, filename, 0);
    // ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

    // Vrati z kontejneru pozadovanou datovou stopu, napr. video stopu.
    int videoStream = av_find_best_stream(pFormatCtx, AVMEDIA_TYPE_VIDEO, -1, -1, NULL, 0);
    if (-1 == videoStream)
        abort();

    av_log(NULL, AV_LOG_INFO, "Video stream has #%i\n", videoStream);

    AVCodec *pCodec_out = NULL;

    // 2. bod ukolu
    // nastaveni koderu do pCodec_out, identifikator CODEC_ID_HUFFYUV
    // vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv
    pCodec_out = avcodec_find_encoder(AV_CODEC_ID_HUFFYUV);
    if(pCodec_out == NULL)
        abort();
    // ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

    AVStream *st;
    // Prida do kontejneru novou datovou stopu.
    if (!(st = avformat_new_stream(pFormatCtx_out, pCodec_out)))
        abort();

    // Nese informace pro kodek (rozmery videa, ID kodeku), ne vsak vlastni kodek.
    AVCodecContext *pCodecCtx_out = st->codec;
    AVCodecContext *pCodecCtx = pFormatCtx->streams[videoStream]->codec;

    // Podle ID kodeku najde odpovidajici dekoder.
    AVCodec *pCodec = avcodec_find_decoder(pCodecCtx->codec_id);
    if (NULL == pCodec)
        abort();

    // Nastavi parametry v predanem AVCodecContext na vychozi hodnoty daneho kodeku.
    avcodec_get_context_defaults3(pCodecCtx_out, pCodec_out);
    pCodecCtx_out->pix_fmt = AV_PIX_FMT_RGB32;
    pCodecCtx_out->width = pCodecCtx->width;
    pCodecCtx_out->height = pCodecCtx->height;
    pCodecCtx_out->time_base.den = 1;
    st->time_base.den = 1;
    pCodecCtx_out->time_base.num = 1;
    st->time_base.num = 1;

    // Inicializuje kontext kodeku pro pouziti s vstupnim kodekem.
    if (avcodec_open2(pCodecCtx, pCodec, NULL))
        abort();

    // 3. bod ukolu
    // Inicializace vystupniho kodek kontextu
    // vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv
    if (avcodec_open2(pCodecCtx_out, pCodec_out, NULL))
        abort();
    // ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

    // Zobrazi informace o kontejneru a dostupnych stop uvnitr nej (audio, video).
    av_dump_format(pFormatCtx_out, 0, filename_out, 1);

    // Vytvori a inicializuje strukturu potrebnou pro pristup k danemu kontejneru.
    if (avio_open(&pFormatCtx_out->pb, filename_out, AVIO_FLAG_WRITE) < 0)
        abort();

    // Zapise do kontejneru hlavicku datove stopy.
    avformat_write_header(pFormatCtx_out, NULL);

    // Alokuje strukturu AVFrame.
    AVFrame *pFrame = av_frame_alloc();
    if (!pFrame)
        abort();

    AVFrame *pFrameRGB = alloc_picture(pCodecCtx_out->pix_fmt, pCodecCtx->width, pCodecCtx->height);
    if (!pFrameRGB)
        abort();

    // Vstupni packet.
    AVPacket pkt;

    // Nese informace a data nutna ke zmene velikosti a formatu pixelu snimku.
    struct SwsContext *c = NULL;

    // Vystupni packet.
    AVPacket pkt_out;

    // Pocitadlo snimku.
    int64_t pts = 0;

    // Precte z kontejneru jeden paket. Pro video odpovida paket jednomu snimku.
    while ( av_read_frame(pFormatCtx, &pkt) == 0 ) {
        if (pkt.stream_index == videoStream) {
            int frameFinished = 0;

            // Z paketu dekoduje jeden snimek videa.
            if (avcodec_decode_video2(pCodecCtx, pFrame, &frameFinished, &pkt) < 0)
                abort();

            if (frameFinished) {
                av_log(NULL, AV_LOG_INFO, "Frame decoded\n");

                // Vytvori strukturu pro zmenu rozmeru a formatu pixelu.
                // Zkusi vyuzit jiz existujici. Potrebne pro volani funkce sws_scale.
                c = sws_getCachedContext(
                        c,
                        pCodecCtx->width,
                        pCodecCtx->height,
                        (enum AVPixelFormat) pFrame->format,
                        pCodecCtx->width,
                        pCodecCtx->height,
                        pCodecCtx_out->pix_fmt,
                        SWS_BICUBIC, NULL, NULL, NULL
                    );

                // Fix pro SIGABRT
                pFrameRGB->width = pFrame->width;
                pFrameRGB->height = pFrame->height;
                pFrameRGB->format = AV_PIX_FMT_RGB24;
                av_frame_get_buffer(pFrameRGB,0);

                // Zmeni rozmery a format pixelu snimku.
                sws_scale(
                    c,
                    (const uint8_t * const*) pFrame->data,
                    pFrame->linesize,
                    0,
                    pFrame->height,
                    pFrameRGB->data,
                    pFrameRGB->linesize
                );

                // Paket si alokuje dalsi funkce.
                av_init_packet(&pkt_out);
                pkt_out.data = NULL;
                pkt_out.size = 0;

                // 4. bod ukolu
                // Doplnte kompresi snimku
                // vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv
                if (avcodec_encode_video2(pCodecCtx_out, &pkt_out, pFrameRGB, &frameFinished) < 0)
                    abort();
                // ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

                // Zvysit pocitadlo snimku.
                pkt_out.pts = pts;
                pkt_out.dts = pts++;

                // Zapsat paket do vystupniho kontejneru.
                if (frameFinished) {
                    if (av_write_frame(pFormatCtx_out, &pkt_out) < 0) {
                        av_log(NULL, AV_LOG_INFO, "Unable to write the frame\n");
                        abort();
                    }
                    av_free_packet(&pkt_out);
                }
            }
        }
        av_free_packet(&pkt);
    }

    // Uzavrit kontejner.
    av_write_trailer(pFormatCtx_out);

    // Uvolnovani pameti.
    avcodec_close(st->codec);
    avio_close(pFormatCtx_out->pb);
    avformat_free_context(pFormatCtx_out);
    sws_freeContext(c);
    av_free(pFrame);
    av_free(pFrameRGB->data[0]);
    av_free(pFrameRGB);
    avcodec_close(pCodecCtx);
    avformat_close_input(&pFormatCtx);

    av_log(NULL, AV_LOG_INFO, "Done\n");

    return 0;
}
