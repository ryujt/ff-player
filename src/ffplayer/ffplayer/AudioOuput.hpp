#pragma once

extern "C" {
#include <libavformat/avformat.h>
}

class AudioOutput {
public:
	bool open(AVFormatContext* context)
	{
		for (int i = 0; i < context->nb_streams; i++)
			if (context->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_AUDIO) {
				stream_ = i;
				break;
			}
		if (stream_ == -1) {
			printf("AudioOutput.open - stream_ == -1 \n");
			return false;
		}

		parameters_ = context->streams[stream_]->codecpar;
		codec_ = avcodec_find_decoder(parameters_->codec_id);
		if (codec_ == NULL) {
			printf("AudioOutput.open - codec == NULL \n");
			return false;
		}

		context_ = avcodec_alloc_context3(codec_);
		if (avcodec_parameters_to_context(context_, parameters_) != 0) 
		{
			printf("AudioOutput.open - avcodec_parameters_to_context \n");
			return false;
		}

		if (avcodec_open2(context_, codec_, NULL) < 0) {
			printf("AudioOutput.open - avcodec_open2 \n");
			return false;
		}
	}

	void close()
	{

	}

	void write(AVPacket* packet)
	{

	}

	bool is_empty()
	{
		return true;
	}
private:
	int stream_ = -1;
	AVCodecParameters* parameters_ = nullptr;
	AVCodecContext* context_ = nullptr;
	AVCodec* codec_ = nullptr;
};