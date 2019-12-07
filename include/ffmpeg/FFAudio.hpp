#pragma once

#include <ryulib/Worker.hpp>
#include <ryulib/sdl_audio.hpp>

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswresample/swresample.h>
}

class FFAudio {
public:
	FFAudio()
	{
		frame_ = av_frame_alloc();
		reframe_ = av_frame_alloc();

		worker_.setOnTask([&](int task, const string text, const void* data, int size, int tag){
			AVPacket* packet = (AVPacket*) data;
			if (context_ == nullptr) {
				av_packet_free(&packet);
				return;
			}

			decode_and_play(packet);
		});
	}

	void terminateNow()
	{
		worker_.terminateNow();
		close();
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

		return audio_.open(context_->channels, context_->sample_rate, 1024);
	}

	void close()
	{
		if (context_ != nullptr) avcodec_close(context_);
		context_ = nullptr;

		audio_.close();
	}

	void write(AVPacket* packet)
	{
		if (context_ != nullptr) worker_.add(0, packet);
	}

	int getStreamIndex() { return stream_index_; }

	bool isEmpty() { return audio_.getDelayCount() < 4; }

	int64_t getPTS() 
	{ 
		if (context_ == nullptr) return 0;
		else return context_->pts_correction_last_pts; 
	}

private:
	int stream_index_ = -1;
	AVCodecParameters* parameters_ = nullptr;
	AVCodecContext* context_ = nullptr;
	AVCodec* codec_ = nullptr;
	Worker worker_;
	AudioSDL audio_;
	SwrContext* swr_;
	AVFrame* frame_ = nullptr;
	AVFrame* reframe_ = nullptr;

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