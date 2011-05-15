#include "ClickQueue.hpp"

volatile ClickQueue* ClickQueue::_instance = NULL;

ClickQueue::ClickQueue()
{
	::InitializeCriticalSectionAndSpinCount(&cs, 0x80000000); // high bit, allocate event in nonpaged pool
}

ClickQueue::~ClickQueue() {
	::DeleteCriticalSection(&cs);
}

ClickQueue::Cleanup::~Cleanup() {
	if(_instance) {
		delete _instance;
		_instance = NULL;
	}
}

__declspec(dllexport) ClickQueue &ClickQueue::getInstance()
{
	static Cleanup _Cleanup;
	if(!_instance) {
		_instance = new ClickQueue();
	}
	return *(ClickQueue*)_instance; // after singleton is created it won't change anymore
}

__declspec(dllexport) void ClickQueue::Enqueue(LPMSLLHOOKSTRUCT pMSG)
{
//	if (_queue.size() > 10)
//		_queue.erase(_queue.begin()); // some protection from extensive clicking
	Lock();
	if (_queue.size() == 0 || pMSG->time > _queue.back().time) // the latter shouldn't ever be evaluated if former is true. don't play with compiler options!!!
		_queue.push_back(*pMSG);
	Unlock();
}

__declspec(dllexport) void ClickQueue::Lock(){
	::EnterCriticalSection(&cs);
}

__declspec(dllexport) void ClickQueue::Unlock(){
	::LeaveCriticalSection(&cs);
}
