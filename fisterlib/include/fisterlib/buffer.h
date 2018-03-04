#pragma once

#include <cstdint>

// TODO: This needs serious memory check
class CBuffer
{
public:
    CBuffer(uint32_t size, bool AutoDelete = true);
    CBuffer(void* buffer, uint32_t length);
    virtual ~CBuffer();

    void Erase();

    union PointerUnion
    {
        uint8_t* b;
        char* c;
        uint16_t* w;
        short int* s;
        uint32_t* d;
        int* i;
        float* f;
        double* r;
        void* v;
    } ptr;

    int ByteLen; // length in bytes
private:
    bool m_bAutoDelete;
};
