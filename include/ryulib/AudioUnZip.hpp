#ifndef AUDIOUNZIP_HPP
#define AUDIOUNZIP_HPP


#include <ryulib/AudioZipUtils.hpp>
#include <ryulib/AudioIO.hpp>
#include <ryulib/AudioDecoder.hpp>
#include <ryulib/debug_tools.hpp>

using namespace std;

class AudioUnZip {
public:
	/** AudioUnZip 생성자
	@param channels 오디오의 채널 수. 1: 모노, 2: 스테레오
	@param sampe_rate 오디오의 sampling rate. 초당 캡쳐할 샘플링(오디오의 데이터) 개수
	*/
	AudioUnZip(int channels, int sampe_rate)
		: audio_output_(channels, sampe_rate), audio_decoder_(channels, sampe_rate)
	{
		audio_output_.setOnError(
			[&](const void* obj, int error_code) {
				if (OnError_ != nullptr) OnError_(this, error_code);
			}
		);
		audio_output_.open();

		audio_decoder_.setOnError(
			[&](const void* obj, int error_code) {
				if (OnError_ != nullptr) OnError_(this, error_code);
			}
		);
		audio_decoder_.setOnDecode(
			[&](const void* obj, const void* data, int size) {
				audio_output_.play(data, size);
			}
		);
	}

	~AudioUnZip()
	{
		audio_output_.close();
	}

	/** 압축된 데이터를 압축해제하고 재생을 한다.
	@param data 압축된 오디오 데이터의 주소
	@param size 압축된 오디오 데이터의 크기
	*/
	void play(const void* data, int size) 
	{
		audio_decoder_.add(data, size);
	}

	/** OnError 이벤트 핸들러를 지정한다.
	@param event 에러가 났을 때 실행될 이벤트 핸들러
	*/
	void setOnError(IntegerEvent event) { OnError_ = event; }

private:
	AudioOutput audio_output_; 
	AudioDecoder audio_decoder_;
	IntegerEvent OnError_ = nullptr;
};


#endif  // AUDIOUNZIP_HPP