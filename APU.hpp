#ifndef APU_H
#define APU_H

#include <cstdint>
#include <cmath>

class APU
{
public:
	APU();

	void Reset();
	void Clock();

	void WriteRegister(uint16_t address, uint8_t data);
	uint8_t ReadRegister(uint16_t address);

	int16_t GetAudioSample() const;
private:
	/**** APU sub units ****/
	struct WaveformSequencer
	{
		uint16_t timer = 0x0000;             // 11-bit timer unit (sets the wave frequency)
		uint16_t timerLoad = 0x0000;         // timer reload value

		uint8_t sequence = 0x00;             // waveform sequencer/generator sequence (sets the wave duty cycle)
		uint8_t sequencer = 0x00;            // 8-bit waveform sequencer/generator

		void Clock()
		{
			timer--;

			if (timer == 0xFFFF)   // timer underflow
			{
				sequencer = (sequencer & 0xFE) >> 1 | (sequencer & 0x01) << 7;    // rotate sequencer pattern
				timer = timerLoad;                                                // reload timer 
			}
		}

		uint8_t Value() const
		{
			return (sequencer & 0x80) >> 7;
		}
	};

	// frame counter/ sequencer
	uint8_t frameCounterMode;   // frame sequencer mode - 0: 4-step sequence, 1: 5-step sequence
	uint16_t doubleFrameCounter;

	struct LengthCounter
	{
		bool enabled = false;
		bool halted = true;             // halt length counter
		uint8_t count = 0x00;           // 8-bit length counter unit

		void Clock()
		{
			if (!halted && count)     // TODO: update status register
				count--;
		}
	};

	struct Envelope
	{

	};

	struct Sweeper
	{
		Sweeper(uint16_t &timer, uint8_t channel) : timer(timer), channel(channel) {}

		uint8_t channel;
		bool enabled;
		uint8_t reloadValue;             // sweep divider relod value P - sweep period is P + 1
		uint8_t divider;                 // 4-bit sweep divider
		bool negated;
		uint8_t shift;
		bool mute;
		uint16_t &timer;

		void Clock()
		{
			if (enabled)
				if (negated)
				{
					if (timer < 8)
						mute = true;
					else
					{
						timer -= timer >> shift;

						if (channel == 2)
							timer -= 1;

						mute = false;
					}
				}
				else   // not negated
				{
					uint16_t targetPeriod = timer + (timer >> shift);    

					if (targetPeriod & 0xF800)
						mute = true;
					else
					{
						timer += timer >> shift;
						mute = false;
					}
				}
		}
	};

	/**** pulse wave channel 1 and 2 ****/
	struct PulseWaveChannel
	{
		PulseWaveChannel(uint8_t channel) : sweeper(waveformSequencer.timer, channel) {}

		WaveformSequencer waveformSequencer;
		LengthCounter lengthCounter;
		Sweeper sweeper;

		bool usingEnvelope;
		uint8_t envelope;                     // 4-bit envelope unit
		uint8_t volume;                       // 4-bit constant volume
		double output;                        // channel output

	} pulseWave1{1}, pulseWave2{2};           // pulse wave channel 1 and 2

	uint8_t lengthCounterLookupTable[32];

	uint64_t systemClockCount;

	/**** oscillators ****/
	//struct SquareWaveOscillator
	//{
	//	uint16_t frequency;
	//	uint16_t amplitude = 1;
	//	double dutyCycle;                     // wave duty cycle - 0: 12.5%, 1: 25%, 2: 50%, 3: 25% (inverted)
	//	double harmonics = 20;

	//	double Sample(double t)
	//	{
	//		double a = 0;
	//		double b = 0;
	//		double p = dutyCycle * 2.0 * 3.14159;

	//		auto approxsin = [](float t)
	//		{
	//			float j = t * 0.15915;
	//			j = j - (int)j;
	//			return 20.785 * j * (j - 0.5) * (j - 1.0f);
	//		};

	//		for (double n = 1; n < harmonics; n++)
	//		{
	//			double c = n * frequency * 2.0 * 3.14159 * t;
	//			a += -approxsin(c) / n;
	//			b += -approxsin(c - p * n) / n;

	//			/*a += -sin(c) / n;
	//			b += -sin(c - p * n) / n;*/
	//		}

	//		return (2.0 * amplitude / 3.14159) * (a - b);
	//	}

	//} squareWaveOscillator;   // TODO: one oscillator per channel?
};

#endif  // APU_H