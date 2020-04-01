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

	/**** pulse wave channels 1 and 2 ****/
	struct PulseWaveChannel                 
	{ 
		PulseWaveChannel(uint8_t channel) : channel(channel) {}

		uint8_t channel;

		uint16_t timer;                     // 11-bit timer
		uint16_t timerReload;               // reload value P: timer period is P + 1

		void ClockTimer()
		{
			timer--;

			if (timer == 0xFFFF)            // timer period is reload value + 1 
			{
				timer = timerReload;        // reload value and clock wave generator
				ClockSequencer();                                                                 
			}	
		}

		uint8_t lengthCounter;              // 5-bit length counter 
		bool lengthCounterHaltFlag;
		bool lengthCounterEnabled;

		void ClockLengthCounter()
		{
			if (!lengthCounterHaltFlag && lengthCounter > 0)
				lengthCounter--;
		}	

		bool constantVolume;                // constant volume/envelope select
		uint8_t envelopeDivider;            // 4-bit envelope
		uint8_t envelopeVolume;             // constant volume/envelope period
		uint8_t envelopeDecayLevel;
		const uint8_t envelopeDecayLevelReload = 0x0F;
		bool envelopeLoopFlag;
		bool envelopeStartFlag;
		
		void ClockEnvelope()
		{
			if (envelopeStartFlag)
			{
				envelopeStartFlag = false;
				envelopeDecayLevel = envelopeDecayLevelReload;
				envelopeDivider = envelopeVolume;
			}
			else
			{
				if (envelopeDivider == 0xFF)
				{
					envelopeDivider = envelopeVolume;

					if (envelopeDecayLevel > 0)
						envelopeDecayLevel--;
					else
						if (envelopeLoopFlag)
							envelopeDecayLevel = envelopeDecayLevelReload;
				}
				else envelopeDivider--;
			}
		}

		bool sweeperEnabled;                  // sweeper 
		uint8_t sweeperDivider;
		uint8_t sweeperDividerReload;
		uint8_t sweeperShiftCount;
		uint16_t sweeperTargetPeriod;
		bool sweeperNegated;
		bool sweeperReloadFlag;
		bool sweeperSilenced;

		void UpdateSweeper()
		{

		}

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
			uint8_t output = 0x00;

			if (lengthCounter > 0 && timerReload > 8 && !sweeperSilenced)
				if (constantVolume)
					output = envelopeVolume * ((sequencer & 0x80) >> 7);        // return constant volume
				else
					output = envelopeDecayLevel * ((sequencer & 0x80) >> 7);    // return envelope

			return  output;
		}

	} pulseWaveChannel1{ 1 }, pulseWaveChannel2{ 2 };

	/**** triangle wave channel ****/
	struct TriangleWaveChannel               
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
			uint8_t output = 0x00;

			if (timerReload > 2)  // eliminate ultrasound frequencies
				output = sequencer[sequenceIndex];

			return output;
		}
	} triangleWaveChannel;

	/**** noise channel ****/
	struct NoiseChannel                     
	{
		enum class Mode { A, B } mode;

		uint16_t timer;                   // 11-bit timer
		uint16_t timerReload;
		uint32_t timerPeriodLookupTable[16]{ 4, 8, 16, 32, 64, 96, 128, 160, 202, 254, 380, 508, 762, 1016, 2034, 4068 };

		void ClockTimer()
		{
			timer--;

			if (timer == 0xFFFF)
			{
				timer = timerReload;
				ClockShiftRegister();
			}
		}

		uint8_t lengthCounter;
		bool lengthCounterHaltFlag;
		bool lengthCounterEnabled;

		void ClockLengthCounter()
		{
			if (!lengthCounterHaltFlag && lengthCounter > 0)
				lengthCounter--;
		}

		bool constantVolume;                // constant volume/envelope select
		uint8_t envelopeDivider;            // 4-bit envelope
		uint8_t envelopeVolume;             // constant volume/envelope period
		uint8_t envelopeDecayLevel;
		const uint8_t envelopeDecayLevelReload = 0x0F;
		bool envelopeLoopFlag;
		bool envelopeStartFlag;

		void ClockEnvelope()
		{
			if (envelopeStartFlag)
			{
				envelopeStartFlag = false;
				envelopeDecayLevel = envelopeDecayLevelReload;
				envelopeDivider = envelopeVolume;
			}
			else
			{
				if (envelopeDivider == 0xFF)
				{
					envelopeDivider = envelopeVolume;

					if (envelopeDecayLevel > 0)
						envelopeDecayLevel--;
					else
						if (envelopeLoopFlag)
							envelopeDecayLevel = envelopeDecayLevelReload;
				}
				else envelopeDivider--;
			}
		}

		uint16_t shiftRegister = 0x0001;

		void ClockShiftRegister()
		{
			uint8_t feedback = 0x00;

			if (mode == Mode::A)
				feedback = shiftRegister & 0x0001 ^ (shiftRegister & 0x0002) >> 1;
			else if (mode == Mode::B)
				feedback = shiftRegister & 0x0001 ^ (shiftRegister & 0x0040) >> 6;

			shiftRegister >>= 1;

			if (feedback)
				shiftRegister |= 1 << 14;
		}

		uint8_t GetOutput() const
		{
			uint8_t output = 0x00;

			if (lengthCounter > 0)
				if (constantVolume)
					output = envelopeVolume * (shiftRegister & 0x01);        // return constant volume
				else
					output = envelopeDecayLevel * (shiftRegister & 0x01);    // return envelope

			return output;
		}
	} noiseChannel;
};

#endif  // APU_H