#ifndef RYU_SDL_AUDIO_HPP
#define RYU_SDL_AUDIO_HPP

#include <SDL2/SDL.h>
#include <ryulib/debug_tools.hpp>
#include <ryulib/ThreadQueue.hpp>

extern "C" {
#include <libswresample/swresample.h>
}

#pragma comment(lib, "sdl2maind.lib")

using namespace std;

class AudioSDL {
public:
	bool open(int channels, int sample_rate, int fpb)
	{
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

	void play(void* data, int size)
	{
		// TODO: 데이터크기가 sample 크기와 맞지 않은 경우 처리
		queue_.push(new Memory(data, size));
		SDL_PauseAudio(0);
	}

	/** 출력이 끝나지 않은 패킷의 갯수 */
	int getDelayCount() { return queue_.size(); }

private:
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