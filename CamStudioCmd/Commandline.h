// CamStudio Open Source - Commandline Header File
// License: GPL
// Coded by karol(dot)toth(at)gmail(dot)com

#pragma once

#include <windows.h>

struct screen
{
    screen() = default;

    int index{ 0 };
    int left{ 0 };
    int right{ 0 };
    int top{ 0 };
    int bottom{ 0 };
    int width{ 0 };
    int height{ 0 };
    char outFile[MAX_PATH]{0};
    char dispName[MAX_PATH]{0};

    bool SetDimensions(int _left, int _right, int _top, int _bottom)
    {
        if (_right <= _left || _bottom <= _top)
            return false;

        this->left = _left;
        this->right = _right;
        this->top = _top;
        this->bottom = _bottom;
        this->width = (_right - _left) - 1;
        this->height = (_bottom - _top) - 1;
        return true;
    }
    //bool SetName(string name);
protected:
};
