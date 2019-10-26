#ifndef AUDIODECODER_HPP
#define AUDIODECODER_HPP


#include <ryulib/AudioZipUtils.hpp>
#include <ryulib/base.hpp>
#include <ryulib/SimpleThread.hpp>
#include <ryulib/SuspensionQueue.hpp>
#include <opus/opus.h>
#include <ryulib/debug_tools.hpp>

using namespace std;

class AudioDecoder {
public:
	AudioDecoder(int channels, int sampe_rate)
	{
		int errorCode;
		opus_ = opus_decoder_create(sampe_rate, channels, &errorCode);
		if (errorCode < 0) {
			DebugOutput::trace("Error - opus_decoder_create (%d)\n", ERROR_OPEN_DECODE);
			if (OnError_ != nullptr) OnError_(this, ERROR_OPEN_DECODE);
		}

		queue_ = new SuspensionQueue<Memory*>();
		thread_ = new SimpleThread(thread_OnExecute);
	}

	void add(const void* data, int size) 
	{
		if (queue_->size() > MAX_DELAY_LIMIT_COUNT) {
			while (queue_->size() > 2) {
				Memory* memory = queue_->pop();
				delete memory;
			}
		}

		Memory* memory = new Memory(data, size);
		queue_->push(memory);
	}

	void setOnError(IntegerEvent event) { OnError_ = event; }
	void setOnDecode(DataEvent event) { OnDecode_ = event; }

private:
	SimpleThread* thread_;
	SimpleThreadEvent thread_OnExecute = [&](SimpleThread * thread) {
		while (thread->isTerminated() == false) {
			Memory* memory = queue_->pop();

			char buffer[FRAMES_PER_BUFFER * SAMPLE_SIZE * CHANNEL];
			int size_out = opus_decode_float(opus_, (unsigned char*) memory->getData(), memory->getSize(), (float*) buffer, sizeof(buffer), 0) * SAMPLE_SIZE;
			if (OnDecode_ != nullptr) OnDecode_(this, buffer, size_out);

			delete memory;
		}
	};

private:
	SuspensionQueue<Memory*>* queue_;
	OpusDecoder* opus_;
	IntegerEvent OnError_ = nullptr;
	DataEvent OnDecode_ = nullptr;
};


#endif  // AUDIODECODER_HPP