#pragma once

#include <ryulib/Worker.hpp>
#include <ryulib/sdl_audio.hpp>

extern "C" {
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libswresample/swresample.h>
}

class FFAudio {
public:
	FFAudio()
	{
		frame_ = av_frame_alloc();
		reframe_ = av_frame_alloc();

		worker_.setOnTask([&](int task, const string text, const void* data, int size, int tag){
			decode_and_play((AVPacket*) data);
		});
	}

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

		audio_.open(context_->channels, context_->sample_rate, 1024);

		return true;
	}

	void close()
	{

	}

	void write(AVPacket* packet)
	{
		worker_.add(0, packet);
	}

	bool isEmpty()
	{
		return audio_.getDelayCount() < 2;
	}

	int getStreamIndex()
	{
		return stream_index_;
	}

private:
	int stream_index_ = -1;
	AVCodecParameters* parameters_ = nullptr;
	AVCodec* codec_ = nullptr;
	AVCodecContext* context_ = nullptr;
	SwrContext* swr_  = nullptr;
	Worker worker_;
	AVFrame* frame_ = nullptr;
	AVFrame* reframe_ = nullptr;
	AudioSDL audio_;

	void decode_and_play(AVPacket* packet)
	{
		int ret = avcodec_send_packet(context_, packet) < 0;
		if (ret < 0) {
			printf("FFAudio - Error sending a packet for decoding \n");
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

			reframe_->channel_layout = frame_->channel_layout;
			reframe_->sample_rate = frame_->sample_rate;
			reframe_->format = AV_SAMPLE_FMT_FLT;
			int ret = swr_convert_frame(swr_, reframe_, frame_);

			int data_size = av_samples_get_buffer_size(NULL, context_->channels, frame_->nb_samples, (AVSampleFormat) reframe_->format, 0);
			audio_.play(reframe_->data[0], data_size);
		}

		av_packet_free(&packet);
	}
};