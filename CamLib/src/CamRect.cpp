#include "stdafx.h"
#include "CamLib/CamRect.h"

BOOL isRectEqual(RECT a, RECT b)
{
    CRect rectA(a);
    return rectA == b;

    // if ((a.left == b.left)
    //    && (a.right == b.right)
    //    && (a.top == b.top)
    //    && (a.bottom == b.bottom))
    //    return TRUE;
    // else
    //    return false;
}

// The rectangle is normalized for fourth-quadrant positioning, which Windows typically uses for coordinates.
// this is dubious
void NormalizeRect(LPRECT prc)
{
    CRect rectSrc(*prc);
    rectSrc.NormalizeRect();

    /* make sure that width and height can be divided by 2 */
    if (rectSrc.Width() & 1)
    {
        if (rectSrc.left & 1)
        {
            rectSrc.right |= 1;
        }
        else
        {
            rectSrc.right &= ~1;
        }
    }

    if (rectSrc.Height() & 1)
    {
        if (rectSrc.top & 1)
        {
            rectSrc.bottom |= 1;
        }
        else
        {
            rectSrc.bottom &= ~1;
        }
    }

    ::CopyRect(prc, rectSrc);
}

void FixRectSizePos(LPRECT prc, int maxxScreen, int maxyScreen, int minxScreen, int minyScreen)
{
    // TRACE( _T("## FixRectSizePos # PRE  # prc: top=%d bottom=%d left=%d right=%d\n"), prc->top, prc->bottom,
    // prc->left, prc->right );
    NormalizeRect(prc);

    int width = prc->right - prc->left + 1;
    int height = prc->bottom - prc->top + 1;
    // TRACE( _T("## FixRectSizePos #      # prc: width=%d height=%d \n"), width, height );

    // check that rectangle isn't too wide
    if (width > abs(maxxScreen - minxScreen))
    {
        prc->left = minxScreen;
        prc->right = maxxScreen - 1;
    }
    else
    {
        // adjust left and right to make sure its on the screen
        // First position on screen is mostly zero not one
        if (prc->left < minxScreen)
        {
            prc->left = minxScreen;
        }
        // Last position on screen is not screenwidth but one less because firts is zero
        if (prc->right > maxxScreen - 1)
        {
            prc->right = maxxScreen - 1;
        }
    }

    // check that rectangle isn't too tall
    if (height > abs(maxyScreen - minyScreen))
    {
        prc->top = minyScreen;
        prc->bottom = maxyScreen - 1;
    }
    else
    {
        // adjust top and bottom to make sure its on the screen
        if (prc->top < minyScreen)
        {
            prc->top = minyScreen;
        }
        if (prc->bottom > maxyScreen - 1)
        {
            prc->bottom = maxyScreen - 1;
        }
    }
    // TRACE( _T("## FixRectSizePos # POST # prc: top=%d bottom=%d left=%d right=%d\n"), prc->top, prc->bottom,
    // prc->left, prc->right );
}
