#include <stdio.h>
#include <ryulib/AudioIO.hpp>

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
}

int main(int argc, char* argv[])
{
	string filename = "D:/Work/test.mp4";

	// 파일(비디오 소스) 오픈
	AVFormatContext* ctx_format = NULL;
	if (avformat_open_input(&ctx_format, filename.c_str(), NULL, NULL) != 0) return -1;
	if (avformat_find_stream_info(ctx_format, NULL) < 0) return -1;
	
	// 오디오 스트림 찾기
	int audio_stream = -1;
	for (int i = 0; i < ctx_format->nb_streams; i++) {
		if (ctx_format->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_AUDIO) {
			audio_stream = i;
			break;
		}
	}
	if (audio_stream == -1) return -2;

	// 오디오 코덱 오픈
	AVCodecParameters* ctx_audio = ctx_format->streams[audio_stream]->codecpar;
	AVCodec* codec = avcodec_find_decoder(ctx_audio->codec_id);
	if (codec == NULL) return -2;
	AVCodecContext* ctx_codec = avcodec_alloc_context3(codec);
	if (avcodec_parameters_to_context(ctx_codec, ctx_audio) != 0)  return -3;
	if (avcodec_open2(ctx_codec, codec, NULL) < 0) return -3;

	// 오디오를 출력할 장치 오픈
	Audio::init();
	AudioOutput audio_out(ctx_audio->channels, ctx_audio->sample_rate, 1024);
	audio_out.open();

	printf("channels: %d, sample_rate: %d, %d \n", ctx_audio->channels, ctx_audio->sample_rate, ctx_audio->bit_rate);

	AVFrame* frame = av_frame_alloc();
	if (!frame) return -4;

	AVPacket packet;

	// 파일(오디오 소스) 반복해서 끝까지 읽기
	while (av_read_frame(ctx_format, &packet) >= 0) {
		// 오디오 스트림만 처리
		if (packet.stream_index == audio_stream) {
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

				// 디코딩된 영상을 출력
				int data_size = av_samples_get_buffer_size(NULL, ctx_codec->channels, frame->nb_samples, ctx_codec->sample_fmt, 1);
				audio_out.play(frame->data[0], data_size);

				// 음성이 처리 될 때까지 기다리기
				while (audio_out.getDelayCount() > 3) Sleep(1);
			}
		}

		av_packet_unref(&packet);
	}

	return 0;
}