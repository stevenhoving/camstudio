#include "stdafx.h"
#include "CamAudio/buffer.h"

#include <new>
#include <tchar.h>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

CBuffer::CBuffer(uint32_t size, bool AutoDelete)
{
    m_bAutoDelete = AutoDelete;
    try
    {
        ptr.b = new uint8_t[size];
        if (ptr.b)
            ByteLen = size;
    }
    catch (std::bad_alloc&)
    {
        // ErrorMsg("Out of memory!");
        MessageBox(nullptr, _T("Out of memory for audio buffer!"), _T("Note"), MB_OK);
    }
}

CBuffer::CBuffer(void *buffer, uint32_t length)
    : m_bAutoDelete(false) // we can't auto delete because we do not own the buffer
{
    if (buffer)
    {
        ptr.v = buffer;
        ByteLen = length;
    }
    else
    {
        ptr.b = nullptr;
        ByteLen = 0;
    }
}

CBuffer::~CBuffer()
{
    if (m_bAutoDelete && ptr.b != nullptr)
    {
        delete[] ptr.b;
    }
}

void CBuffer::Erase()
{
    if (ptr.b != nullptr)
    {
        memset(ptr.b, 0, ByteLen);
    }
}
