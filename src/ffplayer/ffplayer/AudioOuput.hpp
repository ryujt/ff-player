#pragma once

extern "C" {
#include <libavformat/avformat.h>
}

class AudioOutput {
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