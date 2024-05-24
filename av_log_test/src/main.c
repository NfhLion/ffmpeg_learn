#include "libavutil/log.h"
#include "libavutil/ffversion.h"

int main(int argc, char* argv[])
{
    av_log_set_level(AV_LOG_DEBUG);

    av_log(NULL, AV_LOG_ERROR, "Test ffmpeg log ERROR!\n");
    av_log(NULL, AV_LOG_WARNING, "Test ffmpeg log WARNING!, %d\n", argc);
    av_log(NULL, AV_LOG_INFO, "Test ffmpeg log INFO!\n");
    av_log(NULL, AV_LOG_DEBUG, "Test ffmpeg log DEBUG!\n");
    av_log(NULL, AV_LOG_INFO, "ffmpeg version = %s\n", FFMPEG_VERSION);

    return 0;
}