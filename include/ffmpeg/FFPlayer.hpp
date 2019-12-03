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
	/** FFPlayer 생성자
	*/
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

	/** FFPlayer를 완전히 종료시킨다. terminateNow()를 실행하면 모든 작동이 중지되며 다시 실행할 수 없다. 
	*/
	void terminateNow()
	{
		video_.terminateNow();
		audio_.terminateNow();
		scheduler_.terminateAndWait();
	}

	/** 동영상 파일을 오픈하여 재생할 준비를 한다.
	@param filename 오픈 할 동영상 파일 이름
	*/
	void open(string filename)
	{
		scheduler_.add(TASK_OPEN, filename);
	}

	/** 동영상 파일을 닫는다.
	*/
	void close()
	{
		scheduler_.add(TASK_CLOSE);
	}

	/** 오픈된 동영상 파일을 재생한다.
	*/
	void play()
	{
		scheduler_.add(TASK_PLAY);
	}

	/** 재생을 일시 멈춘다.
	*/
	void pause()
	{
		scheduler_.add(TASK_PAUSE);
	}

	/** 지정된 위치로 이동한다.
	@param ms 이동 할 위치 (ms 시간 단위)
	*/
	void move(int ms)
	{
		scheduler_.add(TASK_MOVE, "", nullptr, 0, ms);
	}

	/** 영상을 표시할 윈도우 핸들을 지정한다. 지정하지 않으면 별도의 윈도우가 생성된다.
	*/
	void setTargetHandle(void* handle) { video_.setTargetHandle(handle); }

	/** 동영상이 완전히 재생되었을 때 처리할 이벤트 핸들러를 지정한다.
	*/
	void setOnEOF(NotifyEvent event) { on_EOF_ = event; }

	/** 에러가 발생했을 때 처리할 이벤트 핸들러를 지정한다.
	*/
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
