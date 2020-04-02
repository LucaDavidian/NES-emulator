#include "APU.hpp"

int16_t APU::GetAudioOutput() const
{
	int16_t pulse1 = 200 * (int16_t)pulseWaveChannel1.GetOutput();

	int16_t pulse2 = 200 * (int16_t)pulseWaveChannel2.GetOutput();
	
	int16_t triangle = 500 * (int16_t)triangleWaveChannel.GetOutput();

	int16_t noise =  200 * noiseChannel.GetOutput();

	return pulse1 + pulse2 + triangle + noise;
}

void APU::Reset()
{

}

void APU::Clock()  // called every PPU clock
{
	pulseWaveChannel1.UpdateSweeper();  // sweeper units are updatet every PPU clock
	pulseWaveChannel2.UpdateSweeper();

	if (systemClockCount % 3 == 0)    // every CPU clock - every 3 PPU clocks 
	{
		frameCounter += 0.5;    // frame couter incremented every CPU clock

		triangleWaveChannel.ClockTimer();   // triangle wave channel timer is clocked every CPU clock
		noiseChannel.ClockTimer();          // noise channel timer is clocked every CPU clock 

		if (frameCounterMode == FrameCounterMode::FOUR_STEP)
		{
			if (frameCounter == 3728.5)
			{
				// quarter frame: clock envelopes and triangle wave linear counter
				pulseWaveChannel1.ClockEnvelope();
				pulseWaveChannel2.ClockEnvelope();
				noiseChannel.ClockEnvelope();

				triangleWaveChannel.ClockLinearCounter();
			}
			else if (frameCounter == 7456.5)
			{
				// quarter frame : clock envelopes and triangle wave linear counter
				pulseWaveChannel1.ClockEnvelope();
				pulseWaveChannel2.ClockEnvelope();
				noiseChannel.ClockEnvelope();

				triangleWaveChannel.ClockLinearCounter();

				// half frame: clock length counters and sweeper units
				pulseWaveChannel1.ClockLengthCounter();
				pulseWaveChannel2.ClockLengthCounter();
				triangleWaveChannel.ClockLengthCounter();
				noiseChannel.ClockLengthCounter();

				pulseWaveChannel1.ClockSweeper();
				pulseWaveChannel2.ClockSweeper();
			}
			else if (frameCounter == 11185.5)
			{
				// quarter frame : clock envelopes and triangle wave linear counter
				pulseWaveChannel1.ClockEnvelope();
				pulseWaveChannel2.ClockEnvelope();
				noiseChannel.ClockEnvelope();

				triangleWaveChannel.ClockLinearCounter();
			}
			else if (frameCounter == 14914)
				;
			else if (frameCounter == 14914.5)
			{
				// quarter frame : clock envelopes and triangle wave linear counter
				pulseWaveChannel1.ClockEnvelope();
				pulseWaveChannel2.ClockEnvelope();
				noiseChannel.ClockEnvelope();

				triangleWaveChannel.ClockLinearCounter();

				// half frame: clock length counters and sweeper units
				pulseWaveChannel1.ClockLengthCounter();
				pulseWaveChannel2.ClockLengthCounter();
				triangleWaveChannel.ClockLengthCounter();
				noiseChannel.ClockLengthCounter();

				pulseWaveChannel1.ClockSweeper();
				pulseWaveChannel2.ClockSweeper();
			}
			else if (frameCounter == 14915)
				frameCounter = 0.0;

		}
		else if (frameCounterMode == FrameCounterMode::FIVE_STEP)
		{
			if (frameCounter == 3728.5)
			{
				// quarter frame : clock envelopes and triangle wave linear counter
				pulseWaveChannel1.ClockEnvelope();
				pulseWaveChannel2.ClockEnvelope();
				noiseChannel.ClockEnvelope();

				triangleWaveChannel.ClockLinearCounter();
			}
			else if (frameCounter == 7456.5)
			{
				// quarter frame : clock envelopes and triangle wave linear counter
				pulseWaveChannel1.ClockEnvelope();
				pulseWaveChannel2.ClockEnvelope();
				noiseChannel.ClockEnvelope();

				triangleWaveChannel.ClockLinearCounter();

				// half frame: clock length counters and sweeper units
				pulseWaveChannel1.ClockLengthCounter();
				pulseWaveChannel2.ClockLengthCounter();
				triangleWaveChannel.ClockLengthCounter();
				noiseChannel.ClockLengthCounter();

				pulseWaveChannel1.ClockSweeper();
				pulseWaveChannel2.ClockSweeper();
			}
			else if (frameCounter == 11185.5)
			{
				// quarter frame : clock envelopes and triangle wave linear counter
				pulseWaveChannel1.ClockEnvelope();
				pulseWaveChannel2.ClockEnvelope();
				noiseChannel.ClockEnvelope();

				triangleWaveChannel.ClockLinearCounter();
			}
			else if (frameCounter == 14914.5)
				;
			else if (frameCounter == 18640.5)
			{
				// quarter frame : clock envelopes and triangle wave linear counter
				pulseWaveChannel1.ClockEnvelope();
				pulseWaveChannel2.ClockEnvelope();
				noiseChannel.ClockEnvelope();

				triangleWaveChannel.ClockLinearCounter();

				// half frame: clock length counters and sweeper units
				pulseWaveChannel1.ClockLengthCounter();
				pulseWaveChannel2.ClockLengthCounter();
				triangleWaveChannel.ClockLengthCounter();
				noiseChannel.ClockLengthCounter();

				pulseWaveChannel1.ClockSweeper();
				pulseWaveChannel2.ClockSweeper();
			}
			else if (frameCounter == 18641)
				frameCounter = 0.0;
		}

		if (systemClockCount % 6 == 0)      // every other CPU clock - every 6 PPU clocks
		{
			pulseWaveChannel1.ClockTimer();     //pulse wave channel timer is clocked every APU clock (every other CPU clock)
			pulseWaveChannel2.ClockTimer();
		}
	}

	systemClockCount++;
}

void APU::WriteRegister(uint16_t address, uint8_t data)
{
	switch (address)
	{
		case 0x4000:            // square pulse wave channel 1
			pulseWaveChannel1.envelopeVolume = data & 0x0F;
			pulseWaveChannel1.constantVolume = data & 0x10;
			pulseWaveChannel1.lengthCounterHaltFlag = data & 0x20;
			pulseWaveChannel1.envelopeLoopFlag = data & 0x20;
			switch ((data & 0xC0) >> 6)
			{
				case 0:
					pulseWaveChannel1.sequence = 0x40;  // 0100.0000
					pulseWaveChannel1.dutyCycle = 0.125;
					break;
				case 1:
					pulseWaveChannel1.sequence = 0x60;  // 0110.0000
					pulseWaveChannel1.dutyCycle = 0.25;
					break;
				case 2:
					pulseWaveChannel1.sequence = 0x78;  // 0111.1000
					pulseWaveChannel1.dutyCycle = 0.50;
					break;
				case 3:
					pulseWaveChannel1.sequence = 0x9F;  // 1001.1111
					pulseWaveChannel1.dutyCycle = 0.75;
					break;
			}
			break;
		case 0x4001:
			pulseWaveChannel1.sweeperShiftCount = data & 0x07;
			pulseWaveChannel1.sweeperNegated = data & 0x08;
			pulseWaveChannel1.sweeperDividerReload = (data & 0x70) >> 4;
			pulseWaveChannel1.sweeperEnabled = data & 0x80;
			pulseWaveChannel1.sweeperReloadFlag = true;
			break;
		case 0x4002:
			pulseWaveChannel1.timerReload = pulseWaveChannel1.timerReload & 0xFF00 | data;
			break;
		case 0x4003:
			pulseWaveChannel1.timerReload = pulseWaveChannel1.timerReload & 0x00FF | (uint16_t)(data & 0x07) << 8;
			if (pulseWaveChannel1.lengthCounterEnabled)
				pulseWaveChannel1.lengthCounter = lengthCounterLookupTable[(data & 0xF8) >> 3];  // length counter is reloaded (if enabled)
			pulseWaveChannel1.sequencer = pulseWaveChannel1.sequence;                            // sequencer is restarted
			pulseWaveChannel1.envelopeStartFlag = true;				     	                     // envelope is restarted
			break;
		case 0x4004:            // square pulse wave channel 2
			pulseWaveChannel2.envelopeVolume = data & 0x0F;
			pulseWaveChannel2.constantVolume = data & 0x10;
			pulseWaveChannel2.lengthCounterHaltFlag = data & 0x20;
			pulseWaveChannel2.envelopeLoopFlag = data & 0x20;
			switch ((data & 0xC0) >> 6)
			{
			case 0:
				pulseWaveChannel2.sequence = 0x40;  // 0100.0000
				pulseWaveChannel2.dutyCycle = 0.125;
				break;
			case 1:
				pulseWaveChannel2.sequence = 0x60;  // 0110.0000
				pulseWaveChannel2.dutyCycle = 0.25;
				break;
			case 2:
				pulseWaveChannel2.sequence = 0x78;  // 0111.1000
				pulseWaveChannel2.dutyCycle = 0.50;
				break;
			case 3:
				pulseWaveChannel2.sequence = 0x9F;  // 1001.1111
				pulseWaveChannel2.dutyCycle = 0.75;
				break;
			}
			break;
		case 0x4005:
			pulseWaveChannel2.sweeperShiftCount = data & 0x07;
			pulseWaveChannel2.sweeperNegated = data & 0x08;
			pulseWaveChannel2.sweeperDividerReload = (data & 0x70) >> 4;
			pulseWaveChannel2.sweeperEnabled = data & 0x80;
			pulseWaveChannel2.sweeperReloadFlag = true;
			break;
		case 0x4006:
			pulseWaveChannel2.timerReload = pulseWaveChannel2.timerReload & 0xFF00 | data;
			break;
		case 0x4007:
			pulseWaveChannel2.timerReload = pulseWaveChannel2.timerReload & 0x00FF | (uint16_t)(data & 0x07) << 8;
			if (pulseWaveChannel2.lengthCounterEnabled)
				pulseWaveChannel2.lengthCounter = lengthCounterLookupTable[(data & 0xF8) >> 3];  // length couunter is reloaded (if enabled)
			pulseWaveChannel2.sequencer = pulseWaveChannel2.sequence;                            // sequencer is restarted
			pulseWaveChannel2.envelopeStartFlag = true;				     	                     // envelope is restarted
			break;
		case 0x4008:            // triangle wave channel
			triangleWaveChannel.linearCounterReload = data & 0x7F;
			triangleWaveChannel.linearCounterControlFlag = data & 0x80;
			triangleWaveChannel.lengthCounterHaltFlag = data & 0x80;
			break;
		case 0x400A:
			triangleWaveChannel.timerReload = triangleWaveChannel.timerReload & 0xFF00 | data;
			break;
		case 0x400B:
			triangleWaveChannel.timerReload = triangleWaveChannel.timerReload & 0x00FF | (uint16_t)(data & 0x07) << 8;
			if (triangleWaveChannel.lengthCounterEnabled)
				triangleWaveChannel.lengthCounter = lengthCounterLookupTable[(data & 0xF8) >> 3];
			triangleWaveChannel.linearCounterReloadFlag = true;
			break;
		case 0x400C:            // noise channel
			noiseChannel.envelopeVolume = data & 0x0F;
			noiseChannel.constantVolume = data & 0x10;
			noiseChannel.lengthCounterHaltFlag = data & 0x20;
			noiseChannel.envelopeLoopFlag = data & 0x20;
			break;
		case 0x400E:
			noiseChannel.timerReload = noiseChannel.timerPeriodLookupTable[data & 0x0F];
			noiseChannel.mode = data & 0x80 ? NoiseChannel::Mode::B : NoiseChannel::Mode::A;
			break;
		case 0x400F:
			if (noiseChannel.lengthCounterEnabled)
				noiseChannel.lengthCounter = lengthCounterLookupTable[(data & 0xF8) >> 3];
			// restart envelope
			break;
		case 0x4010:            // DMC - delta modulation channel
			break;
		case 0x4011:
			break;
		case 0x4012:
			break;
		case 0x4013:
			break;
		case 0x4015:   // status register
			// enable pulse wave channel 1 length counter
			if (!(data & 0x01))
			{
				pulseWaveChannel1.lengthCounterEnabled = false;
				pulseWaveChannel1.lengthCounter = 0x00;
			}
			else
				pulseWaveChannel1.lengthCounterEnabled = true;

			// enable pulse wave channel 2 length counter
			if (!(data & 0x02))
			{
				pulseWaveChannel2.lengthCounterEnabled = false;
				pulseWaveChannel2.lengthCounter = 0x00;
			}
			else
				pulseWaveChannel2.lengthCounterEnabled = true;

			// enable triangle wave channel length counter
			if (!(data & 0x04))
			{
				triangleWaveChannel.lengthCounterEnabled = false;
				triangleWaveChannel.lengthCounter = 0x00;
			}
			else
				triangleWaveChannel.lengthCounterEnabled = true;

			// enable noise channel length counter
			if (!(data & 0x08))
			{
				noiseChannel.lengthCounterEnabled = false;
				noiseChannel.lengthCounter = 0x00;
			}
			else
				noiseChannel.lengthCounterEnabled = true;
			
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
		case 0x4013:
			break;
		case 0x4015:            // status register
			break;
		case 0x4017:            // frame counter register
			break;
	}

	return data;
}

