#ifndef ClickQueue_hpp
#define ClickQueue_hpp

#include "CamHook/CamHookExport.h"

#include <windows.h>
#include <vector>

// FIXME: make sure that _WIN32_WINNT >= 0x0600 and remove the following
// what is going on with this constants? where they are supposed to be defined?
#ifndef WM_MOUSEHWHEEL
#define WM_MOUSEHWHEEL                  0x020E
#endif

/* singleton that is used to access the clicks globally */
class CAMHOOK_EXPORT ClickQueue
{
public:
    using QueueType = std::vector<MSLLHOOKSTRUCT>;
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
    ClickQueue(const ClickQueue &) = delete;
    ClickQueue & operator=(const ClickQueue &) = delete;

    QueueType _queue; // sounds awkward, but we should be able to access all of the queue elements
    CRITICAL_SECTION cs;

public:
    using Iterator = QueueType::iterator;
    /*__declspec(dllexport) */ static ClickQueue &getInstance();
    /*__declspec(dllexport) */ void Enqueue(LPMSLLHOOKSTRUCT pMSG);
    /*__declspec(dllexport) */ QueueType& getQueue() { return _queue; } // caller must lock
    /*__declspec(dllexport) */ void Lock();
    /*__declspec(dllexport) */ void Unlock();
};

#endif
