#pragma once

#include <string>
#include <ryulib/Scheduler.hpp>
#include "FFStream.hpp"
#include "FFAudio.hpp"
#include "FFVideo.hpp"

const int TASK_OPEN = 1;
const int TASK_CLOSE = 2;
const int TASK_PLAY = 3;
const int TASK_PAUSE = 4;

using namespace std;

class FFPlayer {
public:
	FFPlayer()
	{
		scheduler_.setOnTask([&](int task, const string text, const void* data, int size, int tag){
			switch (task) {
				case TASK_OPEN: {
					if (stream_.open(text)) {
						if (audio_.open( stream_.getContext() )) printf("OK - audio_.open \n");
						if (video_.open( stream_.getContext() )) printf("OK - video_.open \n");
					} else {
						printf("Error - stream_.open \n");
					}
				} break;

				case TASK_CLOSE: {
					stream_.close();
					audio_.close();
					video_.close();
				} break;

				case TASK_PLAY: {
					scheduler_.start();
				} break;

				case TASK_PAUSE: {
					scheduler_.stop(); 
				} break;
			}
		});

		scheduler_.setOnRepeat([&](){
			if (audio_.isEmpty()) {
				AVPacket* packet = stream_.read();
				if (packet != nullptr) {
					if (packet->stream_index == audio_.getStreamIndex()) audio_.write(packet);
					//else if (packet == 1) video_.write(packet);
					else av_packet_free(&packet);
				}
			}
		});
	}

	void open(string filename)
	{
		scheduler_.add(TASK_OPEN, filename);
	}

	void close()
	{
		scheduler_.add(TASK_CLOSE);
	}

	void play()
	{
		scheduler_.add(TASK_PLAY);
	}

	void pause()
	{
		scheduler_.add(TASK_PAUSE);
	}

private:
	Scheduler scheduler_;
	FFStream stream_;
	FFAudio audio_;
	FFVideo video_;
};
