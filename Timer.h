#pragma once

#include <vector>

class Timer
{
private:
	class TimerEventBase;	
	template <typename T>
	class TimerEvent;
public:
	static Timer &GetInstance();

	void Tick();
	void Reset();
	void Start();
	void Stop();

	double GetDeltaTime() const;
	double GetTotalTime() const;

	template <typename T>
	void AddTimerEvent(T*, void (T::*)(), double delay);
private:
	Timer();
	double mDeltaTime;
	long long int mCountsPerSecond;
	long long int mCurrentTime;
	long long int mPreviousTime;
	long long int mBaseTime;
	long long int mStopTime;
	long long int mPauseDuration;
	bool mStopped;
	std::vector<TimerEventBase*> mTimerEvents;
};

class Timer::TimerEventBase
{
public:
	virtual ~TimerEventBase() = default;
	bool Check();
	virtual void Trigger() = 0;
protected:
	TimerEventBase(double delay) : mTimeStamp(Timer::GetInstance().GetTotalTime()), mDelay(delay) {}
private:
	double mTimeStamp;
	double mDelay;
};

template <typename T>
class Timer::TimerEvent : public TimerEventBase
{
public:
	TimerEvent(T *instance, void (T::*memFunPtr)(), double delay) : TimerEventBase(delay), mInstance(instance), mMemFunPtr(memFunPtr) {}
	void Trigger() override;
private:
	T *mInstance;
	void (T::*mMemFunPtr)();
};

template <typename T>
void Timer::TimerEvent<T>::Trigger()
{
	(mInstance->*mMemFunPtr)();
}

template <typename T>
void Timer::AddTimerEvent(T *instance, void (T::*memFunPtr)(), double delay)
{
	mTimerEvents.push_back(new TimerEvent<T>(instance, memFunPtr, delay));
}