#include "APU.hpp"

int16_t APU::GetAudioOutput() const
{
	int16_t pulse1 = pulseWave1.lengthCounter ? 200 * pulseWave1.volume * ((int16_t)pulseWave1.GetOutput() * 2 - 1) : 0;
	int16_t pulse2 = pulseWave2.lengthCounter ? 200 * pulseWave2.volume * ((int16_t)pulseWave2.GetOutput() * 2 - 1) : 0;
	
	int16_t triangle = triangleWave.lengthCounter ? 200 * ((int16_t)triangleWave.GetOutput() - 7) : 0;

	return pulse1 + pulse2 + triangle;
}

void APU::Reset()
{

}

void APU::Clock()  // called every PPU clock
{
	if (systemClockCount % 3 == 0)    // every CPU clock - every 3 PPU clocks 
	{
		triangleWave.ClockTimer();   // triangle wave timer is clocked every CPU clock

		if (systemClockCount % 6 == 0)   // every other CPU clock - every 6 PPU clocks
		{
			pulseWave1.ClockTimer();     //pulse wave timer is clocked every APU clock (every other CPU clock)
			pulseWave2.ClockTimer();
		}

		if (frameCounterMode == FrameCounterMode::FOUR_STEP)
		{
			if (frameCounter == 3728.5)
			{
				triangleWave.ClockLinearCounter();
			}
			else if (frameCounter == 7456.5)
			{
				triangleWave.ClockLinearCounter();
				triangleWave.ClockLengthCounter();
			}
			else if (frameCounter == 11185.5)
			{
				triangleWave.ClockLinearCounter();
			}
			else if (frameCounter == 14914)
				;
			else if (frameCounter == 14914.5)
			{
				triangleWave.ClockLinearCounter();
				triangleWave.ClockLengthCounter();
			}
			else if (frameCounter == 14915)
				frameCounter = 0.0;

		}
		else if (frameCounterMode == FrameCounterMode::FIVE_STEP)
		{
			if (frameCounter == 3728.5)
			{
				triangleWave.ClockLinearCounter();
			}
			else if (frameCounter == 7456.5)
			{
				triangleWave.ClockLinearCounter();
				triangleWave.ClockLengthCounter();
			}
			else if (frameCounter == 11185.5)
			{
				triangleWave.ClockLinearCounter();
			}
			else if (frameCounter == 14914.5)
				;
			else if (frameCounter == 18640.5)
			{
				triangleWave.ClockLinearCounter();
				triangleWave.ClockLengthCounter();
			}
			else if (frameCounter == 18641)
				frameCounter = 0.0;
		}

		frameCounter += 0.5;  // frame couter incremented every CPU clock
	}

	systemClockCount++;
}

void APU::WriteRegister(uint16_t address, uint8_t data)
{
	switch (address)
	{
		case 0x4000:            // square pulse wave channel 1
			pulseWave1.volume = data & 0x0F;
			pulseWave1.envelopeDivider = data & 0x0F;
			pulseWave1.constantVolume = data & 0x10;
			pulseWave1.lengthCounterHaltFlag = data & 0x20;
			switch ((data & 0xC0) >> 6)
			{
				case 0:
					pulseWave1.sequence = 0x40;  // 0100.0000
					pulseWave1.dutyCycle = 0.125;
					break;
				case 1:
					pulseWave1.sequence = 0x60;  // 0110.0000
					pulseWave1.dutyCycle = 0.25;
					break;
				case 2:
					pulseWave1.sequence = 0x78;  // 0111.1000
					pulseWave1.dutyCycle = 0.50;
					break;
				case 3:
					pulseWave1.sequence = 0x9F;  // 1001.1111
					pulseWave1.dutyCycle = 0.75;
					break;
			}
			break;
		case 0x4001:
			pulseWave1.sweepShiftCount = 0x07;
			pulseWave1.sweepNegated = 0x08;
			pulseWave1.sweepDivider = 0x70;
			pulseWave1.sweepEnabled = data & 0x80;
			pulseWave1.sweepReloadFlag = true;
			break;
		case 0x4002:
			pulseWave1.timerReload = pulseWave1.timerReload & 0xFF00 | data;
			break;
		case 0x4003:
			pulseWave1.timerReload = pulseWave1.timerReload & 0x00FF | (uint16_t)(data & 0x07) << 8;
			pulseWave1.lengthCounterReload = (data & 0xF8) >> 3;
			if (pulseWave1.lengthCounterEnabled)
				pulseWave1.lengthCounter = lengthCounterLookupTable[pulseWave1.lengthCounterReload];  // length couunter is reloaded (if enabled)
			pulseWave1.sequencer = pulseWave1.sequence;                                               // sequencer is restarted
																	     	                          // envelope is restarted
			break;
		case 0x4004:            // square pulse wave channel 2
			pulseWave2.volume = data & 0x0F;
			pulseWave2.envelopeDivider = data & 0x0F;
			pulseWave2.constantVolume = data & 0x10;
			pulseWave2.lengthCounterHaltFlag = data & 0x20;
			switch ((data & 0xC0) >> 6)
			{
			case 0:
				pulseWave2.sequence = 0x40;  // 0100.0000
				pulseWave2.dutyCycle = 0.125;
				break;
			case 1:
				pulseWave2.sequence = 0x60;  // 0110.0000
				pulseWave2.dutyCycle = 0.25;
				break;
			case 2:
				pulseWave2.sequence = 0x78;  // 0111.1000
				pulseWave2.dutyCycle = 0.50;
				break;
			case 3:
				pulseWave2.sequence = 0x9F;  // 1001.1111
				pulseWave2.dutyCycle = 0.75;
				break;
			}
			break;
		case 0x4005:
			pulseWave2.sweepShiftCount = 0x07;
			pulseWave2.sweepNegated = 0x08;
			pulseWave2.sweepDivider = 0x70;
			pulseWave2.sweepEnabled = data & 0x80;
			pulseWave2.sweepReloadFlag = true;
			break;
		case 0x4006:
			pulseWave2.timerReload = pulseWave2.timerReload & 0xFF00 | data;
			break;
		case 0x4007:
			pulseWave2.timerReload = pulseWave2.timerReload & 0x00FF | (uint16_t)(data & 0x07) << 8;
			pulseWave2.lengthCounterReload = (data & 0xF8) >> 3;
			if (pulseWave2.lengthCounterEnabled)
				pulseWave2.lengthCounter = lengthCounterLookupTable[pulseWave2.lengthCounterReload];  // length couunter is reloaded (if enabled)
			pulseWave2.sequencer = pulseWave2.sequence;                                               // sequencer is restarted
																									  // envelope is restarted
			break;
		case 0x4008:            // triangle wave channel
			triangleWave.linearCounterReload = data & 0x7F;
			triangleWave.linearCounterControlFlag = data & 0x80;
			triangleWave.lengthCounterHaltFlag = data & 0x80;
			break;
		case 0x400A:
			triangleWave.timerReload = triangleWave.timerReload & 0xFF00 | data;
			break;
		case 0x400B:
			triangleWave.timerReload = triangleWave.timerReload & 0x00FF | (uint16_t)(data & 0x07) << 8;
			triangleWave.lengthCounterReload = (data & 0xF8) >> 3;
			if (triangleWave.lengthCounterEnabled)
				triangleWave.lengthCounter = lengthCounterLookupTable[triangleWave.lengthCounterReload];
			triangleWave.linearCounterReloadFlag = true;
			break;
		case 0x400C:            // noise channel
			break;
		case 0x400E:
			break;
		case 0x400F:
			break;
		case 0x4010:            // DMC - delta modulation channel
			break;
		case 0x4011:
			break;
		case 0x4012:
			break;
		case 0x40013:
			break;
		case 0x4015:   // status register
			// enable pulse wave channel 1 length counter
			if (!(data & 0x01))
			{
				pulseWave1.lengthCounterEnabled = false;
				pulseWave1.lengthCounter = 0x00;
			}
			else
				pulseWave1.lengthCounterEnabled = true;

			// enable pulse wave channel 2 length counter
			if (!(data & 0x02))
			{
				pulseWave2.lengthCounterEnabled = false;
				pulseWave2.lengthCounter = 0x00;
			}
			else
				pulseWave2.lengthCounterEnabled = true;

			// enable triangle wave channel length counter
			if (!(data & 0x04))
			{
				triangleWave.lengthCounterEnabled = false;
				triangleWave.lengthCounter = 0x00;
			}
			else
				triangleWave.lengthCounterEnabled = true;
			
			break;
		case 0x4017:            // frame counter register
			frameCounterMode = data & 0x80 ? FrameCounterMode::FIVE_STEP : FrameCounterMode::FOUR_STEP;
			break;
	}
}

uint8_t APU::ReadRegister(uint16_t address)
{
	uint8_t data = 0x00;

	switch (address)
	{
		case 0x4000:            // square pulse wave channel 1
			break;
		case 0x4001:
			break;
		case 0x4002:
			break;
		case 0x4003:
			break;
		case 0x4004:            // square pulse wave channel 2
			break;
		case 0x4005:
			break;
		case 0x4006:
			break;
		case 0x4007:
			break;
		case 0x4008:            // triangle wave channel
			break;
		case 0x400A:
			break;
		case 0x400B:
			break;
		case 0x400C:            // noise channel
			break;
		case 0x400E:
			break;
		case 0x400F:
			break;
		case 0x4010:            // DMC - delta modulation channel
			break;
		case 0x4011:
			break;
		case 0x4012:
			break;
		case 0x40013:
			break;
		case 0x4015:            // status register
			break;
		case 0x4017:            // frame counter register
			break;
	}

	return data;
}

