#include "APU.hpp"

APU::APU() : lengthCounterLookupTable{ 10, 254, 20, 2, 40, 4, 80, 6, 160, 8, 60, 10, 14, 12, 26, 14, 12, 16, 24, 18, 48, 20, 96, 22, 192, 24, 72, 26, 16, 28, 32, 30 }
{

}

void APU::Reset()
{
	systemClockCount = 0;
	doubleFrameCounter = 0;
}

void APU::Clock()  // called every CPU clock
{
	if (systemClockCount % 3 == 0)  // frame counter is clocked every other CPU clock, but we keep double the frame counts (CPU clock resolution)
	{
		if (frameCounterMode == 0)  // 4-step sequence mode
		{
			if (doubleFrameCounter == 3728 * 2 + 1)
				;
			else if (doubleFrameCounter == 7456 * 2 + 1)
			{
				pulseWave1.lengthCounter.Clock(); 
				pulseWave1.sweeper.Clock();

				pulseWave2.lengthCounter.Clock();
				pulseWave2.sweeper.Clock();
			}
			else if (doubleFrameCounter == 11185 * 2 + 1)
				;
			else if (doubleFrameCounter == 14914 * 2 + 1)
			{
				pulseWave1.lengthCounter.Clock();
				pulseWave1.sweeper.Clock(); 

				pulseWave2.lengthCounter.Clock();
				pulseWave2.sweeper.Clock();
			}

			doubleFrameCounter++;

			if (doubleFrameCounter == 14915 * 2)
				doubleFrameCounter = 0;
		}
		else // 5-step sequence mode
		{
			if (doubleFrameCounter == 3728 * 2 + 1)
				;
			else if (doubleFrameCounter == 7456 * 2 + 1)
			{
				pulseWave1.lengthCounter.Clock();
				pulseWave1.sweeper.Clock();

				pulseWave2.lengthCounter.Clock(); 
				pulseWave2.sweeper.Clock();
			}
			else if (doubleFrameCounter == 11185 * 2 + 1)
				;
			else if (doubleFrameCounter == 14914 * 2 + 1)
				;
			else if (doubleFrameCounter == 18640 * 2 + 1)
			{
				pulseWave1.lengthCounter.Clock(); 
				pulseWave1.sweeper.Clock(); 

				pulseWave2.lengthCounter.Clock();
				pulseWave2.sweeper.Clock();
			}

			doubleFrameCounter++;

			if (doubleFrameCounter == 18641 * 2)
				doubleFrameCounter = 0;
		}

		if (systemClockCount % 6 == 0)  // APU clock == CPU clock / 2 == PPU clock / 6
		{
			// pulse wave channel 1		
			pulseWave1.waveformSequencer.Clock();    // wave period is (timerLoad + 1) * 8 * 2 / CPUfreq

			if (pulseWave1.lengthCounter.count && pulseWave1.waveformSequencer.timer > 8 && !pulseWave1.sweeper.mute)
				pulseWave1.output = pulseWave1.waveformSequencer.Value();   // audio pin output
			else
				pulseWave1.output = 0x00;

			// pulse wave channel 2
			pulseWave2.waveformSequencer.Clock();    // wave period is 8 * (timerLoad + 1) * 8 * 2 / CPUfreq

			if (pulseWave2.lengthCounter.count && pulseWave2.waveformSequencer.timer > 8 && !pulseWave2.sweeper.mute)
				pulseWave2.output = pulseWave2.waveformSequencer.Value();   // audio pin output
			else
				pulseWave2.output = 0x00;
		}
	}

	systemClockCount++;             // number of PPU clocks
}

int16_t APU::GetAudioSample() const 
{ 
	return 1000 * (pulseWave1.output - 0.5) * 2.0 + 1000 * (pulseWave2.output - 0.5) * 2.0;
}

// APU registers are write-only (apart from status register 0x4015)
void APU::WriteRegister(uint16_t address, uint8_t data)
{
	switch (address)
	{
		// pulse wave channel 1
		case 0x4000:
			pulseWave1.volume = data & 0x0F;
			pulseWave1.usingEnvelope = data & 0x10;
			pulseWave1.lengthCounter.halted = data & 0x20;
			switch ((data & 0xC0) >> 6)
			{
				case 0:
					//squareWaveOscillator.dutyCycle = 0.125;
					pulseWave1.waveformSequencer.sequence = 0x40;  // 0100.0000
					break;
				case 1:
					//squareWaveOscillator.dutyCycle = 0.25;
					pulseWave1.waveformSequencer.sequence = 0x60;  // 0110.0000
					break;
				case 2:
					//squareWaveOscillator.dutyCycle = 0.50;
					pulseWave1.waveformSequencer.sequence = 0x78;  // 0111.1000
					break;
				case 3:
					//squareWaveOscillator.dutyCycle = 0.750;
					pulseWave1.waveformSequencer.sequence = 0x9F;  // 1001.1111
					break;
			}
			break;
		case 0x4001:
			pulseWave1.sweeper.shift = data & 0x07;
			pulseWave1.sweeper.negated = data & 0x08;
			pulseWave1.sweeper.reloadValue = (data & 0x70) >> 4;
			pulseWave1.sweeper.enabled = data & 0x80;
			// reload flag
			break;
		case 0x4002:
			pulseWave1.waveformSequencer.timerLoad = pulseWave1.waveformSequencer.timerLoad & 0xFF00 | data;
			break;
		case 0x4003:
			pulseWave1.waveformSequencer.timerLoad = pulseWave1.waveformSequencer.timerLoad & 0x00FF | (uint16_t)(data & 0x07) << 8;
			//pulseWave1.waveformSequencer.timer = pulseWave1.waveformSequencer.timerLoad;
			if (pulseWave1.lengthCounter.enabled)
				pulseWave1.lengthCounter.count = lengthCounterLookupTable[(data & 0xF8) >> 3];     // load length counter from lookup table
			pulseWave1.waveformSequencer.sequencer = pulseWave1.waveformSequencer.sequence;        // reset the phase of the wave generator/sequencer
			// restart envelope
			break;
		// pulse wave channel 2
		case 0x4004:
			pulseWave2.volume = data & 0x0F;
			pulseWave2.usingEnvelope = data & 0x10;
			pulseWave2.lengthCounter.halted = data & 0x20;
			switch ((data & 0xC0) >> 6)
			{
			case 0:
				//squareWaveOscillator.dutyCycle = 0.125;
				pulseWave2.waveformSequencer.sequence = 0x40;  // 0100.0000
				break;
			case 1:
				//squareWaveOscillator.dutyCycle = 0.25;
				pulseWave2.waveformSequencer.sequence = 0x60;  // 0110.0000
				break;
			case 2:
				//squareWaveOscillator.dutyCycle = 0.50;
				pulseWave2.waveformSequencer.sequence = 0x78;  // 0111.1000
				break;
			case 3:
				//squareWaveOscillator.dutyCycle = 0.750;
				pulseWave2.waveformSequencer.sequence = 0x9F;  // 1001.1111
				break;
			}
			break;
		case 0x4005:
			pulseWave2.sweeper.shift = data & 0x07;
			pulseWave2.sweeper.negated = data & 0x08;
			pulseWave2.sweeper.reloadValue = (data & 0x70) >> 4;
			pulseWave2.sweeper.enabled = data & 0x80;
			break;
		case 0x4006:
			pulseWave2.waveformSequencer.timerLoad = pulseWave2.waveformSequencer.timerLoad & 0xFF00 | data;
			break;
		case 0x4007:
			pulseWave2.waveformSequencer.timerLoad = pulseWave2.waveformSequencer.timerLoad & 0x00FF | (uint16_t)(data & 0x07) << 8;
			//pulseWave2.waveformSequencer.timer = pulseWave2.waveformSequencer.timerLoad;
			if (pulseWave2.lengthCounter.enabled) 
				pulseWave2.lengthCounter.count = lengthCounterLookupTable[(data & 0xF8) >> 3];    // load length counter from look up table
			pulseWave2.waveformSequencer.sequencer = pulseWave2.waveformSequencer.sequence;       // reset the phase of the wave generator/sequencer
			// restart evelope
			break;
		// triangle wave channel 
		case 0x4008:
			break;
		case 0x4009:
			break;
		case 0x400A:
			break;
		case 0x400B:
			break;
		// noise channel 
		case 0x400C:
			break;
		case 0x400D:
			break;
		case 0x400E:
			break;
		case 0x400F:
			break;
		// DMC - delta modulation channel
		case 0x4010:
			break;
		case 0x4011:
			break;
		case 0x4012:
			break;
		case 0x4013:
			break;
		// status register
		case 0x4015:
			if (data & 0x01)  // enable pulse wave 1
				pulseWave1.lengthCounter.enabled = true;
			else  // disable pulse wave 1
			{
				pulseWave1.lengthCounter.enabled = false;
				pulseWave1.lengthCounter.count = 0x00;  // set pulse wave 1 length counter to zero   
			}
			if (data & 0x02)  // enable pulse wave 2
				pulseWave2.lengthCounter.enabled = true;
			else  // disable pulse wave 2 
			{
				pulseWave2.lengthCounter.count = 0x00;  // set pulse wave 2 length counter to zero   
				pulseWave2.lengthCounter.enabled = false;
			}
			break;
		// frame counter register
		case 0x4017:
			frameCounterMode = data & 0x80 ? 1 : 0;
			break;
	}
}

uint8_t APU::ReadRegister(uint16_t address)
{
	uint8_t data = 0x00;

	switch (address)
	{
		default:
			break;
	}

	return data;
}