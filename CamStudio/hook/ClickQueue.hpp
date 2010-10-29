#ifndef ClickQueue_hpp
#define ClickQueue_hpp

#include "StdAfx.h"
#include <windows.h>
#include <vector>

// FIXME: make sure that _WIN32_WINNT >= 0x0600 and remove the following
// what is going on with this constants? where they are supposed to be defined?
#ifndef WM_MOUSEHWHEEL
#define WM_MOUSEHWHEEL                  0x020E
#endif

class DrawClick;

// For the sake of not exporting data. Is it bad?
class ClickQueue
{
public:
	typedef std::vector<MSLLHOOKSTRUCT> QueueType; // lazy boy
private:
	friend class Cleanup;
	class Cleanup
	{
		public:
		~Cleanup();
	};
	//should be std::atom
	volatile static ClickQueue* _instance;
 
	ClickQueue();
	~ClickQueue();
	ClickQueue(const ClickQueue &);             // intentionally undefined
	ClickQueue & operator=(const ClickQueue &); // intentionally undefined

	QueueType _queue; // sounds awkward, but we should be able to access all of the queue elements
	CRITICAL_SECTION cs;
 
public:
	typedef QueueType::iterator Iterator;
	__declspec(dllexport) static ClickQueue &getInstance();
	__declspec(dllexport) void Enqueue(LPMSLLHOOKSTRUCT pMSG);
	__declspec(dllexport) QueueType& getQueue() {return _queue;} // caller must lock
	__declspec(dllexport) void Lock();
	__declspec(dllexport) void Unlock();
};

#endif