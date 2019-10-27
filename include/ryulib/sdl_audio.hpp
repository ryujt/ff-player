#ifndef RYU_SDL_AUDIO_HPP
#define RYU_SDL_AUDIO_HPP

#include <SDL2/SDL.h>
#include <ryulib/base.hpp>
#include <ryulib/debug_tools.hpp>
#include <ryulib/ThreadQueue.hpp>
#include <ryulib/PacketReader.hpp>

extern "C" {
#include <libswresample/swresample.h>
}

#pragma comment(lib, "sdl2maind.lib")

using namespace std;

class AudioSDL {
public:
	/** 오디오를 출력하기 위한 장치를 오픈합니다.
	@param channels 오디오의 채널 수. 1: 모노, 2: 스테레오
	@param sampe_rate 오디오의 sampling rate.
	@param fpb 한 번에 처리할 프레임의 갯수
	*/
	bool open(int channels, int sample_rate, int fpb)
	{
		channels_ = channels;
		sample_rate_ = sample_rate;
		fpb_ = fpb;
		frame_size_ = fpb * channels * 4;

		SDL_AudioSpec audio_spec;
		SDL_memset(&audio_spec, 0, sizeof(audio_spec));
		audio_spec.freq = sample_rate;
		audio_spec.format = AUDIO_F32LSB;
		audio_spec.channels = channels;
		audio_spec.samples = fpb;
		audio_spec.callback = audio_callback;
		audio_spec.userdata = &queue_;
		if(SDL_OpenAudio(&audio_spec, NULL) < 0) {
			DebugOutput::trace("SDL_OpenAudio: %s", SDL_GetError());
			return false;
		}

		return true;
	}

	/**	오디오를 출력합니다.
	@param data 출력할 오디오 데이터
	@param size 출력할 오디오 데이터의 크기
	*/
	void play(void* data, int size)
	{
		packet_reader_.write(data, size);

		while (packet_reader_.canRead(frame_size_)) {
			queue_.push(new Memory(packet_reader_.read(frame_size_), frame_size_));
			SDL_PauseAudio(0);
		}
	}

	/** 출력이 끝나지 않은 패킷의 갯수 */
	int getDelayCount() { return queue_.size(); }

private:
	int channels_ = 0;
	int sample_rate_ = 0;
	int fpb_ = 0;
	int frame_size_ = 0;
	PacketReader packet_reader_;
	ThreadQueue<Memory*> queue_;

	static void audio_callback(void* udata, Uint8* stream, int len)
	{
		ThreadQueue<Memory*>* queue = (ThreadQueue<Memory*>*) udata;
		Memory* memory; 
		if (queue->pop(memory)) {
			memcpy(stream, memory->getData(), memory->getSize());
			delete memory;
		}
	}

};

#endif  // RYU_SDL_AUDIO_HPP