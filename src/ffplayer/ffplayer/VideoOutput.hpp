#pragma once

extern "C" {
#include <libavformat/avformat.h>
}

class VideoOutput {
public:
	void open(AVFormatContext* context)
	{

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
};
