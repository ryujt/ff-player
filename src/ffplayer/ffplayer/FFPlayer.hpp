#pragma once

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
			switch(task) {
				case TASK_OPEN: {
					if (stream_.open(text)) {
						// TODO: open error
						audio_.open( stream_.getContext() );
						video_.open( stream_.getContext() );
					} else {
						// TODO: open error
						printf("open error \n");
					}
				} break;

				case TASK_CLOSE: {
					stream_.close();
					audio_.close();
					video_.close();
				} break;

				case TASK_PLAY: {
					stream_.play();
				} break;

				case TASK_PAUSE: {
					stream_.pause();
				} break;
			}
		});

		scheduler_.setOnRepeat([&](){
			if (stream_.isPlaying() == true) {
				if (audio_.isEmpty()) {
					AVPacket* packet = stream_.read();
					if (packet != nullptr) {
						if (packet->stream_index = audio_.getStreamIndex()) audio_.write(packet);
						else if (packet->stream_index = video_.getStreamIndex()) video_.write(packet);
						else av_packet_free(&packet);
					} else {
						// TODO: EOF
					}
				}
			}
		});

		scheduler_.start();
	}

	~FFPlayer()
	{
		scheduler_.terminateNow();
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
	VideoStream stream_;
	FFAudio audio_;
	FFVideo video_;
};
