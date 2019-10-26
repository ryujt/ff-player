#ifndef AUDIOZIP_HPP
#define AUDIOZIP_HPP


#include <ryulib/base.hpp>
#include <ryulib/AudioZipUtils.hpp>
#include <ryulib/AudioIO.hpp>
#include <ryulib/AudioEncoder.hpp>
#include <ryulib/debug_tools.hpp>

using namespace std;

class AudioZip {
public:
	/** AudioZip 생성자
	@param channels 오디오의 채널 수. 1: 모노, 2: 스테레오
	@param sampe_rate 오디오의 sampling rate. 초당 캡쳐할 샘플링(오디오의 데이터) 개수
	*/
	AudioZip(int channels, int sampe_rate)
		: audio_input_(channels, sampe_rate), audio_encoder_(channels, sampe_rate)
	{
		audio_encoder_.setOnError(
			[&](const void* obj, int error_code) {
				if (OnError_ != nullptr) OnError_(this, error_code);
			}
		);

		audio_input_.setOnError(
			[&](const void* obj, int error_code) {
				if (OnError_ != nullptr) OnError_(this, error_code);
			}
		);
		audio_input_.setOnData(
			[&](const void* obj, const void* buffer, int buffer_size) {
				audio_encoder_.add(buffer, buffer_size);
			}
		);
	}

	/** 오디오 캡쳐 및 압축을 시작한다. */
	void start() 
	{
		audio_input_.open();
	}

	/** 오디오 캡쳐 및 압축을 중단한다. */
	void stop() 
	{
		audio_input_.close();
	}

	/** OnError 이벤트 핸들러를 지정한다.
	@param event 에러가 났을 때 실행될 이벤트 핸들러
	*/
	void setOnError(IntegerEvent event) { OnError_ = event; }

	/** OnEncode 이벤트 핸들러를 지정한다.
	@param event 오디오가 압축되었을 때 실행될 이벤트 핸들러
	*/
	void setOnEncode(DataEvent event) { audio_encoder_.setOnEncode(event); }

private:
	AudioInput audio_input_; 
	AudioEncoder audio_encoder_;
	IntegerEvent OnError_ = nullptr;
};


#endif  // AUDIOZIP_HPP