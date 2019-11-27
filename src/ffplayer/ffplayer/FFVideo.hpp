#pragma once

#include <ryulib/Worker.hpp>
#include <ryulib/sdl_window.hpp>

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/imgutils.h>	
}

class FFVideo {
public:
	FFVideo()
	{
		frame_ = av_frame_alloc();

		worker_.setOnTask([&](int task, const string text, const void* data, int size, int tag){
			decode_and_play((AVPacket*) data);
		});
	}

	bool open(AVFormatContext* context)
	{
		stream_index_ = -1;
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

		video_.open("ffplayer", context_->width, context_->height);

		return true;
	}

	void close()
	{

	}

	void write(AVPacket* packet)
	{
		worker_.add(0, packet);
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
	Worker worker_;
	WindowSDL video_;
	AVFrame* frame_ = nullptr;

	void decode_and_play(AVPacket* packet)
	{
		int ret = avcodec_send_packet(context_, packet) < 0;
		if (ret < 0) {
			printf("FFVideo - Error sending a packet for decoding \n");
			return;
		}	

		while (ret >= 0) {
			ret = avcodec_receive_frame(context_, frame_);
			if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF) {
				break;
			} else if (ret < 0) {
				printf("Error sending a packet for decoding \n");
				return;
			}

			video_.showYUV(frame_->data[0], frame_->linesize[0], frame_->data[1], frame_->linesize[1], frame_->data[2], frame_->linesize[2]);
		}

		av_packet_free(&packet);
	}

};
