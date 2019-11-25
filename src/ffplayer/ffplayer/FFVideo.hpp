#pragma once

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/imgutils.h>	
}

class FFVideo {
public:
	bool open(AVFormatContext* context)
	{
		for (int i = 0; i < context->nb_streams; i++)
			if (context->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
				stream_index_ = i;
				break;
			}
		if (stream_index_ == -1) {
			printf("FFVideo.open - stream_ == -1 \n");
			return false;
		}

		parameters_ = context->streams[stream_index_]->codecpar;
		codec_ = avcodec_find_decoder(parameters_->codec_id);
		if (codec_ == NULL) {
			printf("FFVideo.open - codec == NULL \n");
			return false;
		}
		
		context_ = avcodec_alloc_context3(codec_);
		if (avcodec_parameters_to_context(context_, parameters_) != 0) 
		{
			printf("FFVideo.open - avcodec_parameters_to_context \n");
			return false;
		}

		if (avcodec_open2(context_, codec_, NULL) < 0) {
			printf("FFVideo.open - avcodec_open2 \n");
			return false;
		}

		return true;
	}

	void close()
	{

	}

	void write(AVPacket* packet)
	{

	}

	int getStreamIndex() { return stream_index_; }

	bool isEmpty()
	{
		return true;
	}

private:
	int stream_index_ = -1;
	AVCodecParameters* parameters_ = nullptr;
	AVCodecContext* context_ = nullptr;
	AVCodec* codec_ = nullptr;
};
