#ifndef APU_H
#define APU_H

#include <cstdint>

class APU
{
public:
	void Reset();
	void Clock();

	void WriteRegister(uint16_t address, uint8_t data);
	uint8_t ReadRegister(uint16_t address);

	int16_t GetAudioOutput() const;
private:
	uint64_t systemClockCount = 0;
	
	double frameCounter = 0.0;
	enum class FrameCounterMode { FOUR_STEP, FIVE_STEP } frameCounterMode;

	uint8_t lengthCounterLookupTable[32]{ 10, 254, 20, 2, 40, 4, 80, 6, 160, 8, 60, 10, 14, 12, 26, 14, 12, 16, 24, 18, 48, 20, 96, 22, 192, 24, 72, 26, 16, 28, 32, 30 };

	/**** APU channels ****/
	struct PulseWaveChannel                 // pulse wave channels 1 and 2
	{ 
		uint16_t timer;                     // 11-bit timer
		uint16_t timerReload;

		void ClockTimer()
		{
			timer--;

			if (timer == 0xFFFF)            // timer underflow 
			{
				timer = timerReload;        // reload value and clock wave generator
				ClockSequencer();                                                                 // TODO: timer < 8 --> silcence channel
			}	
		}

		uint8_t lengthCounter;              // 5-bit length counter 
		uint8_t lengthCounterReload;
		bool lengthCounterHaltFlag;
		bool lengthCounterEnabled;

		void ClockLengthCounter()
		{
			if (!lengthCounterHaltFlag && lengthCounter > 0)
				lengthCounter--;
		}	

		bool constantVolume;                // constant volume/envelope select
		uint8_t volume;                     // constant volume
		
		uint8_t envelopeDivider;            // 4-bit envelope
		uint8_t envelopeDividerReload;
		bool envelopeLoopFlag;

		void ClockEnvelope()
		{

		}

		bool sweepEnabled;                  // sweeper 
		uint8_t sweepDivider;
		bool sweepNegated;
		uint8_t sweepShiftCount;
		bool sweepReloadFlag;

		void ClockSweeper()
		{

		}

		uint8_t sequencer;                  // waveform sequencer/generatror
		uint8_t sequence;
		double dutyCycle;

		void ClockSequencer()
		{
			sequencer = sequencer >> 1 | (sequencer & 0x01) << 7;
		}

		uint8_t GetOutput() const
		{
			return (sequencer & 0x80) >> 7;
		}

	} pulseWave1, pulseWave2;

	struct TriangleWaveChannel              // triangle wave channel
	{
		uint16_t timer;                     // 11-bit timer
		uint16_t timerReload;

		void ClockTimer()
		{
			timer--;

			if (timer == 0xFFFF)            // timer underflow 
			{
				timer = timerReload;        // reload value

				if (linearCounter > 0 && lengthCounter > 0)  // if linear counter and length counter are not zero clock wave generator
					ClockSequencer();
			}
		}
		
		uint8_t lengthCounter;              // 5-bit length counter 
		uint8_t lengthCounterReload;
		bool lengthCounterHaltFlag;
		bool lengthCounterEnabled;

		void ClockLengthCounter()
		{
			if (!lengthCounterHaltFlag && lengthCounter > 0)
				lengthCounter--;
		}

		uint8_t linearCounter;              // 7-bit linear counter
		uint8_t linearCounterReload;
		bool linearCounterReloadFlag;
		bool linearCounterControlFlag;

		void ClockLinearCounter()
		{
			if (linearCounterReloadFlag)
				linearCounter = linearCounterReload;
			else if (linearCounter > 0)
				linearCounter--;

			if (!linearCounterControlFlag)
				linearCounterReloadFlag = false;
		}

		uint8_t sequencer[32]{ 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15 };   // waveform sequencer/generator
		uint8_t sequenceIndex = 0;

		void ClockSequencer()
		{
			sequenceIndex++;

			if (sequenceIndex == 32)
				sequenceIndex = 0;
		}

		uint8_t GetOutput() const
		{
			return sequencer[sequenceIndex];
		}
	} triangleWave;
};

#endif  // APU_H