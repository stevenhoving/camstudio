#include "stdafx.h"
#include "CamLib/CStudioLib.h"

// Round positive numbers
long rounddouble(double dVal)
{
    auto num = static_cast<long>(dVal);
    double diff = dVal - static_cast<double>(num);
    if (0.5 <= diff)
    {
        num++;
    }

    return num;
}
