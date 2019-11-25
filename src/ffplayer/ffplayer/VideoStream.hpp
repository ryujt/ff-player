#pragma once

extern "C" {
#include <libavformat/avformat.h>
}

using namespace std;

class VideoStream {
public:
	bool open(string filename)
	{
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
		return nullptr;
	}

private:
};