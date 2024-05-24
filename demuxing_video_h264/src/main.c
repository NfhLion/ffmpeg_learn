#include "libavutil/log.h"
#include "libavutil/ffversion.h"
#include "libavformat/avformat.h"

const int sampleFrequencyTable[] = {
    96000,
    88200,
    64000,
    48000,
    44100,
    32000,
    24000,
    22050,
    16000,
    12000,
    11025,
    8000,
    7350
};

int getADTSHeader(char *adtsHeader, int packetSize, int profile, int sampleRate, int channels)
{
    int sampleFrequencyIndex = 3;
    int adtsLength = packetSize + 7;

    for (int i = 0; i < sizeof(sampleFrequencyTable) / sizeof(sampleFrequencyTable[0]); i++) {
        if (sampleRate == sampleFrequencyTable[i]) {
            sampleFrequencyIndex = i;
            break;
        }
    }

    adtsHeader[0] = 0xff;
    
    adtsHeader[1] = 0xf0;
    adtsHeader[1] |= (0 << 3);
    adtsHeader[1] |= (0 << 1);
    adtsHeader[1] |= 1;

    adtsHeader[2] = (profile << 6);
    adtsHeader[2] |= (sampleFrequencyIndex & 0x0f) << 2;
    adtsHeader[2] |= (0 << 1);
    adtsHeader[2] |= (channels & 0x04) >> 2;

    adtsHeader[3] = (channels & 0x03) << 6;
    adtsHeader[3] |= (0 << 5);
    adtsHeader[3] |= (0 << 4);
    adtsHeader[3] |= (0 << 3);
    adtsHeader[3] |= (0 << 2);
    adtsHeader[3] |= ((adtsLength & 0x1800) >> 11);

    adtsHeader[4] = (uint8_t)((adtsLength & 0x7f8) >> 3);
    adtsHeader[5] = (uint8_t)((adtsLength & 0x7) << 5);
    adtsHeader[5] |= 0x1f;
    adtsHeader[6] = 0xfc;

    return 0;
}

int main(int argc, char* argv[])
{
    av_log_set_level(AV_LOG_DEBUG);

    if (argc < 3) {
        av_log(NULL, AV_LOG_ERROR, "Usage: %s infileName.\n", argv[0]);
        return -1;
    }

    const char *infileName = argv[1];
    const char *outfileName = argv[2];

    AVFormatContext *inFormatCtx = NULL;
    int ret = avformat_open_input(&inFormatCtx, infileName, NULL, NULL);
    if (ret != 0) {
        av_log(NULL, AV_LOG_ERROR, "open input file format failed: %s\n", av_err2str(ret));
        return -1;
    }

    ret = avformat_find_stream_info(inFormatCtx, NULL);
    if (ret < 0) {
        av_log(NULL, AV_LOG_ERROR, "find stream info failed: %s\n", av_err2str(ret));
        return -1;
    }

    int videoIndex = av_find_best_stream(inFormatCtx, AVMEDIA_TYPE_VIDEO, -1, -1, NULL, 0);
    if (videoIndex < 0) {
        av_log(NULL, AV_LOG_ERROR, "find best stream failed, index is %d.\n", videoIndex);
        return -1;
    }
    av_log(NULL, AV_LOG_INFO, "the video index is %d\n", videoIndex);

    // if(inFormatCtx->streams[videoIndex]->codecpar->codec_id != AV_CODEC_ID_H264) {
    //     av_log(NULL, AV_LOG_ERROR, "the media file no contain h264 stream, it's codec_id is %d\n",
    //     inFormatCtx->streams[videoIndex]->codecpar->codec_id);
    //     return -1;
    // }

    AVPacket packet;
    av_init_packet(&packet);

    const AVBitStreamFilter *bsf = av_bsf_get_by_name("h264_mp4toannexb");
    if (bsf == NULL) {
        av_log(NULL, AV_LOG_ERROR, "get h264_mp4toannexb failed!\n");
        avformat_close_input(&inFormatCtx);
        return -1;
    }
    AVBSFContext *bsfCtx = NULL;
    av_bsf_alloc(bsf, &bsfCtx);
    avcodec_parameters_copy(bsfCtx->par_in, inFormatCtx->streams[videoIndex]->codecpar);
    av_bsf_init(bsfCtx);

    FILE *dest_fp = fopen(outfileName, "wb");
    if (dest_fp == NULL) {
        av_log(NULL, AV_LOG_ERROR, "open %s file failed\n", outfileName);
        avformat_close_input(&inFormatCtx);
        return -1;
    }
    while (av_read_frame(inFormatCtx, &packet) == 0) {
        if (packet.stream_index == videoIndex) {
            if (av_bsf_send_packet(bsfCtx, &packet) == 0) {
                while(av_bsf_receive_packet(bsfCtx, &packet) == 0) {
                    ret = fwrite(packet.data, 1, packet.size, dest_fp);
                    if (ret != packet.size) {
                        av_log(NULL, AV_LOG_ERROR, "write %s file failed!\n", outfileName);
                        fclose(dest_fp);
                        avformat_close_input(&inFormatCtx);
                        return -1;
                    }
                }
            }
            
        }
        av_packet_unref(&packet);
    }

    if (bsfCtx)
        av_bsf_free(&bsfCtx);

    if (dest_fp)
        fclose(dest_fp);
    if (inFormatCtx)
        avformat_close_input(&inFormatCtx);

    return 0;
}
