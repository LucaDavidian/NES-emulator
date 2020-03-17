#include "Timer.h"
#include <Windows.h>

Timer &Timer::GetInstance()
{
	static Timer instance;

	return instance;
}

Timer::Timer() : mDeltaTime(0.0), mCountsPerSecond(0), mCurrentTime(0), mPreviousTime(0), mBaseTime(0), mStopTime(0), mPauseDuration(0), mStopped(false)
{
	QueryPerformanceFrequency((LARGE_INTEGER*)&mCountsPerSecond);
}

// initialize timer
void Timer::Reset()
{
	QueryPerformanceCounter((LARGE_INTEGER*)&mCurrentTime);
	mPreviousTime = mCurrentTime;
	mBaseTime = mCurrentTime;
	mDeltaTime = 0.0;
	mStopped = false;
}

void Timer::Tick()
{
	if (mStopped)
		mDeltaTime = 0.0;
	else
	{
		QueryPerformanceCounter((LARGE_INTEGER*)&mCurrentTime);
		mDeltaTime = (mCurrentTime - mPreviousTime) / (double)mCountsPerSecond;
		mPreviousTime = mCurrentTime;
	}

	//std::vector<TimerEventBase*>::const_iterator cit = mTimerEvents.cbegin();

	//while (cit != mTimerEvents.cend())
	//{
	//	if ((*cit)->Check())
	//	{
	//		(*cit)->Trigger();
	//		cit = mTimerEvents.erase(cit);  // one-shot event
	//	}
	//	else
	//		cit++;
	//}
}

void Timer::Start()
{
	long long int startTime;
	QueryPerformanceCounter((LARGE_INTEGER*)&startTime);

	if (mStopped)
	{
		mPauseDuration += startTime - mStopTime;
		mPreviousTime = startTime;
		mStopped = false;
	}
}

void Timer::Stop()
{
	if (!mStopped)
	{
		QueryPerformanceCounter((LARGE_INTEGER*)&mStopTime);
		mStopped = true;
	}
}

// to be called after Tick()
double Timer::GetDeltaTime() const
{
	return mDeltaTime;
}

// to be called after Tick()
double Timer::GetTotalTime() const
{
	if (mStopped)
		return ((mStopTime - mBaseTime) - mPauseDuration) / (double)mCountsPerSecond;
	else
		return ((mCurrentTime - mBaseTime) - mPauseDuration) / (double)mCountsPerSecond;
}

bool Timer::TimerEventBase::Check()
{
	return Timer::GetInstance().GetTotalTime() - mTimeStamp >= mDelay;
}