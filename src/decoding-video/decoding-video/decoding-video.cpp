#include <stdio.h>
#include <ryulib/sdl_window.hpp>

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/imgutils.h>	
}

#pragma comment(lib, "sdl2maind.lib")

int main(int argc, char* argv[])
{
	//string filename = "D:/Work/test.mp4";
	string filename = "https://etc.s3.ap-northeast-2.amazonaws.com/AsomeIT.mp4";

	// 파일(비디오 소스) 오픈
	AVFormatContext* ctx_format = NULL;
	if (avformat_open_input(&ctx_format, filename.c_str(), NULL, NULL) != 0) return -1;
	if (avformat_find_stream_info(ctx_format, NULL) < 0) return -1;

	// 비디오 스트림 찾기
	int video_stream = -1;
	for (int i = 0; i < ctx_format->nb_streams; i++)
		if (ctx_format->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
			video_stream = i;
			break;
		}
	if (video_stream == -1) return -2;

	// 비디오 코덱 오픈
	AVCodecParameters* ctx_video = ctx_format->streams[video_stream]->codecpar;
	AVCodec* codec = avcodec_find_decoder(ctx_video->codec_id);
	if (codec == NULL) return -3;
	AVCodecContext* ctx_codec = avcodec_alloc_context3(codec);
	if (avcodec_parameters_to_context(ctx_codec, ctx_video) != 0)  return -3;
	if (avcodec_open2(ctx_codec, codec, NULL) < 0) return -3;

	// 비디오를 출력할 윈도우 오픈
	WindowSDL window;
	window.open("ffmpeg", ctx_codec->width, ctx_codec->height);

	AVFrame* frame = av_frame_alloc();
	if (!frame) return -4;

	AVPacket packet;

	// 파일(비디오 소스) 반복해서 끝까지 읽기
	while (av_read_frame(ctx_format, &packet) >= 0) {
		// 비디오 스트림만 처리
		if (packet.stream_index == video_stream) {
			int ret = avcodec_send_packet(ctx_codec, &packet) < 0;
			if (ret < 0) {
				printf("Error sending a packet for decoding \n");
				return -5;
			}

			while (ret >= 0) {
				ret = avcodec_receive_frame(ctx_codec, frame);
				if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF) {
					break;
				} else if (ret < 0) {
					printf("Error sending a packet for decoding \n");
					return -5;
				}

				// 디코딩된 영상을 화면에 표시
				window.showYUV(frame->data[0], frame->linesize[0], frame->data[1], frame->linesize[1], frame->data[2], frame->linesize[2]);
			}
		}

		av_packet_unref(&packet);
	}

	return 0;
}