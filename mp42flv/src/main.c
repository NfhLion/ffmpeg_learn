#include "libavutil/log.h"
#include "libavutil/ffversion.h"
#include "libavformat/avformat.h"

int main(int argc, char* argv[])
{
    av_log_set_level(AV_LOG_DEBUG);
    if (argc < 3) {
        av_log(NULL, AV_LOG_ERROR, "Usage: %s infileName.\n", argv[0]);
        return -1;
    }
    const char *infileName = argv[1];
    const char *outfileName = argv[2];

    AVFormatContext *inFmtCtx = NULL;
    int ret = avformat_open_input(&inFmtCtx, infileName, NULL, NULL);
    if (ret != 0) {
        av_log(NULL, AV_LOG_ERROR, "open input fpormat failed: %s\n", av_err2str(ret));
        return -1;
    }

    ret = avformat_find_stream_info(inFmtCtx, NULL);
    if (ret != 0) {

        goto fail;
    }

fail:
    if (inFmtCtx) {
        
    }

    return 0;
}