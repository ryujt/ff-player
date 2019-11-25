#pragma once

#include <ryulib/Scheduler.hpp>
#include "VideoStream.hpp"
#include "AudioOuput.hpp"
#include "VideoOutput.hpp"

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
			if (stream_.is_playing()) {
				if ((audio_.is_empty()) || (video_.is_empty())) {
					AVPacket* packet = stream_.read();
					if (packet != nullptr) {
						audio_.write(packet);
						video_.write(packet);
					} else {
						if ((audio_.is_empty()) && (video_.is_empty())) {
							// TODO: EOF
						}
					}
				}
			}
		});
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
	AudioOutput audio_;
	VideoOutput video_;
};
