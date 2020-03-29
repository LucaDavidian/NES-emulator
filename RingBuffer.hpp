#ifndef RING_BUFFER_H
#define RING_BUFFER_H

#include <cstddef>
#include <utility>
#include <exception>

class BufferEmptyException : public std::exception {};
class BufferFullException : public std::exception {};

template <typename T, unsigned int N>
class RingBuffer
{
public:
    RingBuffer() : mFirst(mBuffer), mLast(mBuffer), mCount(0) {}

    bool Empty() const { return mFirst == mLast; }
    bool Full() const;
    std::size_t Size() const;

    template <typename U>
    void InsertFirst(U &&element);
    template <typename U>
    void InsertLast(U &&element);

    void RemoveFirst();
    void RemoveLast();

    T &First() { return const_cast<T&>(static_cast<const RingBuffer&>(*this).First()); }
    const T &First() const { return *mFirst; }
    T &Last() { return const_cast<T&>(static_cast<const RingBuffer&>(*this).Last()); }
    const T &Last() const { return *mLast; }
private:
    static constexpr unsigned int M = N + 1;
    T mBuffer[M];
    T *mFirst;
    T *mLast;
    std::size_t mCount;
};

template <typename T, unsigned int N>
bool RingBuffer<T, N>::Full() const
{
    const T *p = mLast + 1;

    if (p == mBuffer + M)
        p = mBuffer;

    return p == mFirst;
}

template <typename T, unsigned int N>
std::size_t RingBuffer<T, N>::Size() const
{
    if (mFirst <= mLast)
        return mLast - mFirst;
    else  // mHead > mTail
        return M - (mFirst - mLast);

}

template <typename T, unsigned int N>
template <typename U>
void RingBuffer<T, N>::InsertFirst(U &&element)
{
    if (Full())
        throw BufferFullException();

    if (mFirst == mBuffer)
        mFirst = mBuffer + M - 1;
    else
        mFirst--;

    *mFirst = std::forward<U>(element);

    mCount++;
}

template <typename T, unsigned int N>
template <typename U>
void RingBuffer<T, N>::InsertLast(U &&element)
{
    if (Full())
        throw BufferFullException();

    *mLast = std::forward<U>(element);

    mLast++;

    if (mLast == mBuffer + M)
        mLast = mBuffer;

    mCount++;
}

template <typename T, unsigned int N>
void RingBuffer<T, N>::RemoveFirst()
{
    if (Empty())
        throw BufferEmptyException();

    mFirst++;

    if (mFirst == mBuffer + M)
        mFirst = mBuffer;

    mCount--;
}

template <typename T, unsigned int N>
void RingBuffer<T, N>::RemoveLast()
{
    if (Empty())
        throw BufferEmptyException();

    if (mLast == mBuffer)
        mLast = mBuffer + M - 1;
    else
        mLast--;

    mCount--;
}

#endif  // RING_BUFFER_H