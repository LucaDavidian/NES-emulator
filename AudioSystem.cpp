#include "AudioSystem.h"
#include "SDL.h""
#include "Error.h"

AudioSystem &AudioSystem::GetInstance()
{
	static AudioSystem instance;

	return instance;
}

AudioSystem::AudioSystem()
{
	SDL_AudioSpec audioDevice;
	audioDevice.freq = 48000;                // sample rate 
	audioDevice.format = AUDIO_S16SYS;       // bit depth 
	audioDevice.samples = 512;               // number of sample frames (one sample frame contains as many samples as channels)
	audioDevice.channels = 1;                // number of channels (one:mono, two: stereo)
	audioDevice.size;                        // calculated: sample byte count * sample frames * channels 
	audioDevice.silence;                     // calculated
	audioDevice.callback = AudioCallback;

	SDL_OpenAudio(&audioDevice, 0);
}

AudioSystem::~AudioSystem()
{
	SDL_CloseAudio();
}

void AudioSystem::StartPlayback()
{
	SDL_PauseAudio(0);
}

void AudioSystem::StopPlayback()
{
	SDL_PauseAudio(1);
}

void AudioSystem::EnqueueSample(int16_t sample) 
{ 
	ringBuffer.InsertLast(sample); 
}

// called by audio thread
void AudioSystem::AudioCallback(void *userData, uint8_t *audioData, int length)
{
	int numSamples = length / 2;

	RingBuffer &buffer = GetInstance().ringBuffer;

	// fill audio driver buffer
	int i;
	for (i = 0; !buffer.Empty() && i < numSamples; i++)
	{
		reinterpret_cast<int16_t*>(audioData)[i] = buffer.First();
		buffer.RemoveFirst();
	}

	while (i < numSamples)
		reinterpret_cast<int16_t*>(audioData)[i++] = 0x0000;

	GetInstance().callback();
}