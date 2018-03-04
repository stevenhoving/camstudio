#pragma once

class CBuffer
{
public:
    void Erase();
    CBuffer(DWORD size, bool AutoDelete = true);
    CBuffer(void *buffer, DWORD length);
    virtual ~CBuffer();

    union PointerUnion {
        BYTE *b;
        char *c;
        WORD *w;
        short int *s;
        DWORD *d;
        int *i;
        float *f;
        double *r; // real
        void *v;
    } ptr;

    int ByteLen; // length in bytes

private:
    bool m_bAutoDelete;
};
