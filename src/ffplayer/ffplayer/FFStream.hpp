#pragma once

extern "C" {
#include <libavformat/avformat.h>
}

using namespace std;

class FFStream {
public:
	bool open(string filename)
	{
		if (avformat_open_input(&context_, filename.c_str(), NULL, NULL) != 0) return false;
		if (avformat_find_stream_info(context_, NULL) < 0) return false;
		return true;
	}

	void close()
	{
		avformat_close_input(&context_);
		context_ = nullptr;
	}

	AVPacket* read()
	{
		if (context_ == nullptr) return nullptr; 

		AVPacket* packet = av_packet_alloc();

		if (av_read_frame(context_, packet) < 0) {
			av_packet_free(&packet);
			return nullptr;
		}

		return packet;
	}

	AVFormatContext* getContext()
	{
		return context_;
	}

private:
	AVFormatContext* context_ = nullptr;
};