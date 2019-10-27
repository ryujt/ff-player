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