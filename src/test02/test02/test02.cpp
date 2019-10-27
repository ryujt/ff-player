#include <stdio.h>
#include <ryulib/sdl_window.hpp>
#include <ryulib/AudioIO.hpp>

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
	int audioStream = -1;
	for (int i = 0; i < pFormatCtx->nb_streams; i++) {
		if (pFormatCtx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) videoStream = i;
		if (pFormatCtx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_AUDIO) audioStream = i;
	}
	if (videoStream == -1) return -1;
	if (audioStream == -1) return -1;

	AVCodecParameters* pCtxVideo = pFormatCtx->streams[videoStream]->codecpar;
	AVCodec* pCodecVideo = avcodec_find_decoder(pCtxVideo->codec_id);
	if (pCodecVideo == NULL) return -2;

	AVCodecParameters* pCtxAudio = pFormatCtx->streams[audioStream]->codecpar;
	AVCodec* pCodecAudio = avcodec_find_decoder(pCtxAudio->codec_id);
	if (pCodecAudio == NULL) return -2;

	AVCodecContext* pCtxVideoCode = avcodec_alloc_context3(pCodecVideo);
	if (avcodec_parameters_to_context(pCtxVideoCode, pCtxVideo) != 0)  return -3;
	if (avcodec_open2(pCtxVideoCode, pCodecVideo, NULL) < 0) return -4;

	AVCodecContext* pCtxAudioCode = avcodec_alloc_context3(pCodecAudio);
	if (avcodec_parameters_to_context(pCtxAudioCode, pCtxAudio) != 0)  return -3;
	if (avcodec_open2(pCtxAudioCode, pCodecAudio, NULL) < 0) return -4;

	AVFrame* frame = av_frame_alloc();
	if (!frame) return -5;

	WindowSDL window;
	window.open("ffmpeg", pCtxVideoCode->width, pCtxVideoCode->height);

	printf("channels: %d, sample_rate: %d, %d \n", pCtxAudioCode->channels, pCtxAudioCode->sample_rate, pCtxAudioCode->bit_rate);

	Audio::init();
	AudioOutput audio_out(pCtxAudioCode->channels, pCtxAudioCode->sample_rate, 1024);
	audio_out.open();

	AVPacket packet;
	while (av_read_frame(pFormatCtx, &packet) >= 0) {
		if (packet.stream_index == videoStream) {
			int ret = avcodec_send_packet(pCtxVideoCode, &packet) < 0;
			if (ret < 0) {
				printf("Error sending a packet for decoding \n");
				return -6;
			}

			while (ret >= 0) {
				ret = avcodec_receive_frame(pCtxVideoCode, frame);
				if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF) {
					break;
				} else if (ret < 0) {
					printf("Error sending a packet for decoding \n");
					return -6;
				}

				window.showYUV(frame->data[0], frame->linesize[0], frame->data[1], frame->linesize[1], frame->data[2], frame->linesize[2]);
			}

			av_packet_unref(&packet);
		}

		if (packet.stream_index == audioStream) {
			int ret = avcodec_send_packet(pCtxAudioCode, &packet) < 0;
			if (ret < 0) {
				printf("Error sending a packet for decoding \n");
				return -7;
			}

			while (ret >= 0) {
				ret = avcodec_receive_frame(pCtxAudioCode, frame);
				if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF) {
					break;
				} else if (ret < 0) {
					printf("Error sending a packet for decoding \n");
					return -7;
				}

				int data_size = av_samples_get_buffer_size(NULL, pCtxAudioCode->channels, frame->nb_samples, pCtxAudioCode->sample_fmt, 1);
				audio_out.play(frame->data[0], data_size);
				Sleep(10);
			}

			av_packet_unref(&packet);
		}
	}

	return 0;
}