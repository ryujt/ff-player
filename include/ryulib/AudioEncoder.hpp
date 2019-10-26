#ifndef AUDIOENCODER_HPP
#define AUDIOENCODER_HPP


#include <ryulib/AudioZipUtils.hpp>
#include <ryulib/base.hpp>
#include <ryulib/SimpleThread.hpp>
#include <ryulib/SuspensionQueue.hpp>
#include <opus/opus.h>
#include <ryulib/debug_tools.hpp>

using namespace std;

class AudioEncoder {
public:
	/** AudioEncoder 생성자
	@param channels 캡쳐할 오디오의 채널 수. 1: 모노, 2: 스테레오
	@param sampe_rate 캡쳐할 오디오의 ampling rate. 초당 캡쳐할 샘플링(오디오의 데이터) 개수
	*/
	AudioEncoder(int channels, int sampe_rate)
	{
		int errorCode;

		opus_ = opus_encoder_create(sampe_rate, channels, OPUS_APPLICATION_AUDIO, &errorCode);
		if (errorCode < 0) {
			DebugOutput::trace("Error - opus_encoder_create (%d)\n", ERROR_OPEN_ENCODER);
			if (OnError_ != nullptr) OnError_(this, ERROR_OPEN_ENCODER);
		}

		errorCode = opus_encoder_ctl(opus_, OPUS_SET_BITRATE(BITRATE));
		if (errorCode < 0) {
			opus_encoder_destroy(opus_);
			DebugOutput::trace("Error - opus_encoder_ctl (%d)\n", ERROR_OPEN_ENCODER);
			if (OnError_ != nullptr) OnError_(this, ERROR_OPEN_ENCODER);
		}

		queue_ = new SuspensionQueue<Memory*>();
		thread_ = new SimpleThread(thread_OnExecute);
	}

	/** 압축하려는 오디오 데이터를 큐에 추가합니다. 오디오 압축은 내부 스레드에서 비동기로 처리됩니다.
	@param data 오디오 데이터의 주소
	@param size 오디오 데이터의 크기
	*/
	void add(const void* data, int size) 
	{
		Memory* memory = new Memory(data, size);
		queue_->push(memory);
	}

	/** OnError 이벤트 핸들러를 지정한다.
	@param event 에러가 났을 때 실행될 이벤트 핸들러
	*/
	void setOnError(IntegerEvent event) { OnError_ = event; }

	/** OnEncode 이벤트 핸들러를 지정한다.
	@param event 오디오가 압축되었을 때 실행될 이벤트 핸들러
	*/
	void setOnEncode(DataEvent event) { OnEncode_ = event; }

private:
	SimpleThread* thread_;
	SimpleThreadEvent thread_OnExecute = [&](SimpleThread * thread) {
		while (thread->isTerminated() == false) {
			Memory* data = queue_->pop();

			char buffer[FRAMES_PER_BUFFER * SAMPLE_SIZE * CHANNEL];
			int size_out = opus_encode_float(opus_, (float*) data->getData(), FRAMES_PER_BUFFER, (unsigned char*) buffer, sizeof(buffer));
			if (OnEncode_ != nullptr) OnEncode_(this, buffer, size_out);

			delete data;
		}
	};

private:
	SuspensionQueue<Memory*>* queue_;
	OpusEncoder* opus_;
	IntegerEvent OnError_ = nullptr;
	DataEvent OnEncode_ = nullptr;
};


#endif  // AUDIOENCODER_HPP