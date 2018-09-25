#include "stdafx.h"
#include "CamLib/CamRect.h"

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
