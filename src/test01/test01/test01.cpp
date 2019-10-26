#include <stdio.h>
#include <SDL2/SDL.h>
#include <ryulib/sdl_window.hpp>

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
#include <libavutil/imgutils.h>	
}

#pragma comment(lib, "sdl2maind.lib")

int main(int argc, char* argv[])
{
	AVFormatContext* pFormatCtx = NULL;
	if (avformat_open_input(&pFormatCtx, "D:/Work/test.mp4", NULL, NULL) != 0) return -1;
	if (avformat_find_stream_info(pFormatCtx, NULL) < 0) return -1;

	av_dump_format(pFormatCtx, 0, argv[1], 0);

	int videoStream = -1;
	for (int i = 0; i < pFormatCtx->nb_streams; i++)
		if (pFormatCtx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
			videoStream = i;
			break;
		}
	if (videoStream == -1) return -1;

	AVCodecParameters* pCodecCtxOrig = pFormatCtx->streams[videoStream]->codecpar;
	AVCodec* pCodec = avcodec_find_decoder(pCodecCtxOrig->codec_id);
	if (pCodec == NULL) return -2;

	AVCodecContext* pCodecCtx = avcodec_alloc_context3(pCodec);
	if (avcodec_parameters_to_context(pCodecCtx, pCodecCtxOrig) != 0)  return -3;

	if (avcodec_open2(pCodecCtx, pCodec, NULL) < 0) return -4;

	AVFrame* frame = av_frame_alloc();
	if (!frame) return -5;

	WindowSDL window;
	window.open("ffmpeg", pCodecCtx->width, pCodecCtx->height);

	AVPacket packet;
	while (av_read_frame(pFormatCtx, &packet) >= 0) {
		if (packet.stream_index == videoStream) {
			int ret = avcodec_send_packet(pCodecCtx, &packet) < 0;
			if (ret < 0) {
				printf("Error sending a packet for decoding \n");
				return -5;
			}

			while (ret >= 0) {
				ret = avcodec_receive_frame(pCodecCtx, frame);
				if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF) {
					break;
				} else if (ret < 0) {
					printf("Error sending a packet for decoding \n");
					return -5;
				}

				window.showYUV(frame->data[0], frame->linesize[0], frame->data[1], frame->linesize[1], frame->data[2], frame->linesize[2]);
			}
		}

		av_packet_unref(&packet);
	}

	return 0;
}