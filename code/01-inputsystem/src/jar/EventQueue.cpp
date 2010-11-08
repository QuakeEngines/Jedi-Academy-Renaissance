#include "jar/EventQueue.hpp"
#include "jar/Event.hpp"

namespace jar {

EventQueue::EventQueue()
{
    //ctor
}

EventQueue::~EventQueue()
{
    //dtor
}

void EventQueue::ReceiveEvent(const Event& event)
{
    mQueueMutex.Lock();
    mEventQueue.push(event);
    mQueueMutex.Unlock();
}

const bool EventQueue::GetEvent(Event& event)
{
    mQueueMutex.Lock();
    if(mEventQueue.size())
    {
        event = mEventQueue.front();
        mEventQueue.pop();
        mQueueMutex.Unlock();
        return true;
    }
    mQueueMutex.Unlock();
    return false;
}

} // namespace jar
