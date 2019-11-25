#include <stdio.h>
#include <Windows.h>
#include <ryulib/sdl_audio.hpp>

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswresample/swresample.h>
}

int main(int argc, char* argv[])
{
	//string filename = "D:/Work/test.mp4";
	string filename = "https://etc.s3.ap-northeast-2.amazonaws.com/AsomeIT.mp4";

	// ����(����� �ҽ�) ����
	AVFormatContext* ctx_format = NULL;
	if (avformat_open_input(&ctx_format, filename.c_str(), NULL, NULL) != 0) return -1;
	if (avformat_find_stream_info(ctx_format, NULL) < 0) return -1;
	
	// ����� ��Ʈ�� ã��
	int audio_stream = -1;
	for (int i = 0; i < ctx_format->nb_streams; i++) {
		if (ctx_format->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_AUDIO) {
			audio_stream = i;
			break;
		}
	}
	if (audio_stream == -1) return -2;

	// ����� �ڵ� ����
	AVCodecParameters* parameters = ctx_format->streams[audio_stream]->codecpar;
	AVCodec* codec = avcodec_find_decoder(parameters->codec_id);
	if (codec == NULL) return -2;
	AVCodecContext* ctx_codec = avcodec_alloc_context3(codec);
	if (avcodec_parameters_to_context(ctx_codec, parameters) != 0)  return -3;
	if (avcodec_open2(ctx_codec, codec, NULL) < 0) return -3;

	// ������� ����� ��ġ ����
	AudioSDL audio;
	audio.open(parameters->channels, parameters->sample_rate, 1024);

	// ����� ���� ��ȯ (resampling) �غ�
	SwrContext* swr = swr_alloc_set_opts(
		NULL,
		parameters->channel_layout,
		AV_SAMPLE_FMT_FLT,
		parameters->sample_rate,
		parameters->channel_layout,
		(AVSampleFormat) parameters->format,
		parameters->sample_rate,
		0,
		NULL);
	swr_init(swr);

	printf("channels: %d, sample_rate: %d, %d \n", parameters->channels, parameters->sample_rate, parameters->bit_rate);

	AVFrame* frame = av_frame_alloc();
	if (!frame) return -4;

	AVFrame* reframe = av_frame_alloc();
	if (!reframe) return -4;

	AVPacket packet;

	// ����(����� �ҽ�) �ݺ��ؼ� ������ �б�
	while (av_read_frame(ctx_format, &packet) >= 0) {
		// ����� ��Ʈ���� ó��
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

				// ���� ��ȯ
				reframe->channel_layout = frame->channel_layout;
				reframe->sample_rate = frame->sample_rate;
				reframe->format = AV_SAMPLE_FMT_FLT;
				int ret = swr_convert_frame(swr, reframe, frame);

				int data_size = av_samples_get_buffer_size(NULL, ctx_codec->channels, frame->nb_samples, (AVSampleFormat) reframe->format, 0);
				audio.play(reframe->data[0], data_size);

				// ������ ó�� �� ������ ��ٸ���
				while (audio.getDelayCount() > 2) Sleep(1);
			}
		}

		av_packet_unref(&packet);
	}

	return 0;
}