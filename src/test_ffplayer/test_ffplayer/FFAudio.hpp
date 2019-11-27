#pragma once

extern "C" {
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libswresample/swresample.h>
}

class FFAudio {
public:
	bool open(AVFormatContext* context)
	{
		stream_index_ = -1;
		for (int i = 0; i < context->nb_streams; i++)
			if (context->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_AUDIO) {
				stream_index_ = i;
				break;
			}
		if (stream_index_ == -1) {
			printf("FFAudio.open - stream_ == -1 \n");
			return false;
		}

		parameters_ = context->streams[stream_index_]->codecpar;
		codec_ = avcodec_find_decoder(parameters_->codec_id);
		if (codec_ == NULL) {
			printf("FFAudio.open - codec == NULL \n");
			return false;
		}

		context_ = avcodec_alloc_context3(codec_);
		if (avcodec_parameters_to_context(context_, parameters_) != 0) 
		{
			printf("FFAudio.open - avcodec_parameters_to_context \n");
			return false;
		}

		if (avcodec_open2(context_, codec_, NULL) < 0) {
			printf("FFAudio.open - avcodec_open2 \n");
			return false;
		}

		swr_ = swr_alloc_set_opts(
			NULL,
			context_->channel_layout,
			AV_SAMPLE_FMT_FLT,
			context_->sample_rate,
			context_->channel_layout,
			(AVSampleFormat) parameters_->format,
			context_->sample_rate,
			0,
			NULL
		);
		swr_init(swr_);

		return true;
	}

	void close()
	{

	}

	void write(int packet)
	{

	}

	bool isEmpty()
	{
		return true;
	}

private:
	int stream_index_ = -1;
	AVCodecParameters* parameters_ = nullptr;
	AVCodec* codec_ = nullptr;
	AVCodecContext* context_ = nullptr;
	SwrContext* swr_  = nullptr;
};