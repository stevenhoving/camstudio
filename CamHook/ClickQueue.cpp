#include "stdafx.h"
#include "CamHook/ClickQueue.h"

volatile ClickQueue *ClickQueue::_instance = nullptr;

ClickQueue::ClickQueue()
{
    ::InitializeCriticalSectionAndSpinCount(&cs, 0x80000000); // high bit, allocate event in nonpaged pool
}

ClickQueue::~ClickQueue()
{
    ::DeleteCriticalSection(&cs);
}

ClickQueue::Cleanup::~Cleanup()
{
    if (_instance)
    {
        delete _instance;
        _instance = nullptr;
    }
}

ClickQueue &ClickQueue::getInstance()
{
    static Cleanup _Cleanup;
    if (!_instance)
    {
        _instance = new ClickQueue();
    }
    return *(ClickQueue *)_instance; // after singleton is created it won't change anymore
}

void ClickQueue::Enqueue(LPMSLLHOOKSTRUCT pMSG)
{
    //    if (_queue.size() > 10)
    //        _queue.erase(_queue.begin()); // some protection from extensive clicking
    Lock();
    if (_queue.empty() || pMSG->time > _queue.back().time)
    { // the latter shouldn't ever be evaluated if former is
      // true. don't play with compiler options!!!
        _queue.push_back(*pMSG);
    }
    Unlock();
}

void ClickQueue::Lock()
{
    ::EnterCriticalSection(&cs);
}

void ClickQueue::Unlock()
{
    ::LeaveCriticalSection(&cs);
}
