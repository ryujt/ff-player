#pragma once

extern "C" {
#include <libavformat/avformat.h>
}

using namespace std;

class VideoStream {
public:
	bool open(string filename)
	{
		if (avformat_open_input(&context_, filename.c_str(), NULL, NULL) != 0) return false;
		if (avformat_find_stream_info(context_, NULL) < 0) return false;
		return true;
	}

	void close()
	{

	}

	void play()
	{

	}

	void pause()
	{

	}

	bool is_playing()
	{
		return true;
	}

	AVPacket* read()
	{
		return nullptr;
	}

	AVFormatContext* getContext()
	{
		return context_;
	}

private:
	AVFormatContext* context_ = nullptr;
};