#ifndef AUDIO_SYSTEM_H
#define AUDIO_SYSTEM_H

#include <cstdint>
#include "RingBuffer.hpp"

class AudioSystem
{
private:
	using Callback = void (*)();
	using RingBuffer = RingBuffer<int16_t,512>;
public:
	static AudioSystem &GetInstance();

	~AudioSystem();

	void StartPlayback();
	void StopPlayback();

	
	void SetCallback(Callback callback) { this->callback = callback; }

	void EnqueueSample(int16_t sample);
private:
	AudioSystem();

	static void AudioCallback(void *userData, uint8_t *audioData, int length);
	Callback callback;
	RingBuffer ringBuffer;
};

#endif  // AUDIO_SYSTEM_H