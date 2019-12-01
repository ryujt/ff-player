#pragma once

#include <ryulib/base.hpp>
#include <ryulib/Scheduler.hpp>
#include "FFStream.hpp"
#include "FFAudio.hpp"
#include "FFVideo.hpp"

const int TASK_OPEN = 1;
const int TASK_CLOSE = 2;
const int TASK_PLAY = 3;
const int TASK_PAUSE = 4;
const int TASK_MOVE = 5;

using namespace std;

const int ERROR_OPEN = -1;

class FFPlayer {
public:
	FFPlayer()
	{
		scheduler_.setOnTask([&](int task, const string text, const void* data, int size, int tag){
			switch(task) {
				case TASK_OPEN: {
					if (stream_.open(text)) {
						if (audio_.open(stream_.getContext()) == false) {
							stream_.close();
							if (on_error_ != nullptr) on_error_(this, ERROR_OPEN, "오디오 코덱을 오픈 할 수가 없습니다.");
							return;
						}

						if (video_.open(stream_.getContext()) == false) {
							stream_.close();
							audio_.close();
							if (on_error_ != nullptr) on_error_(this, ERROR_OPEN, "비디오 코덱을 오픈 할 수가 없습니다.");
							return;
						}
					} else {
						if (on_error_ != nullptr) on_error_(this, ERROR_OPEN, "동영상 파일을 오픈 할 수가 없습니다.");
					}
				} break;

				case TASK_CLOSE: {
					do_close();
				} break;

				case TASK_PLAY: {
					scheduler_.start();
				} break;

				case TASK_PAUSE: {
					scheduler_.stop();
				} break;

				case TASK_MOVE: {
					stream_.move(tag);
				} break;
			}
		});

		scheduler_.setOnRepeat([&](){
			if (audio_.isEmpty()) {
				AVPacket* packet = stream_.read();
				if (packet != nullptr) {
					if (packet->stream_index == audio_.getStreamIndex()) {
						audio_.write(packet);
						video_.audioSync( audio_.getPTS() ); 
					} else if (packet->stream_index == video_.getStreamIndex()) video_.write(packet);
					else av_packet_free(&packet);
				} else {
					do_close();
					if (on_EOF_ != nullptr) on_EOF_(this);
				}
			}
		});
	}

	void terminateNow()
	{
		video_.terminateNow();
		audio_.terminateNow();
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

	void move(int ms)
	{
		scheduler_.add(TASK_MOVE, "", nullptr, 0, ms);
	}

	void setTargetHandle(void* handle) { video_.setTargetHandle(handle); }

	void setOnEOF(NotifyEvent event) { on_EOF_ = event; }
	void setOnError(ErrorEvent event) { on_error_ = event; }

private:
	Scheduler scheduler_;
	FFStream stream_;
	FFAudio audio_;
	FFVideo video_;

	NotifyEvent on_EOF_ = nullptr;
	ErrorEvent on_error_ = nullptr;

	void do_close()
	{
		scheduler_.stop();

		stream_.close();
		audio_.close();
		video_.close();
	}

};
