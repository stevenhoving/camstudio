#include "stdafx.h"
#include "CamLib/CStudioLib.h"

// todo: unreferenced
int matchSpecial(int keyCtrl, int keyShift, int keyAlt, int ctrlDown, int shiftDown, int altDown)
{
    int ret = 1;

    if ((keyCtrl) && (!ctrlDown))
    {
        ret = 0;
    }

    if ((keyShift) && (!shiftDown))
    {
        ret = 0;
    }

    if ((keyAlt) && (!altDown))
    {
        ret = 0;
    }

    if ((!keyCtrl) && (ctrlDown))
    {
        ret = 0;
    }

    if ((!keyShift) && (shiftDown))
    {
        ret = 0;
    }

    if ((!keyAlt) && (altDown))
    {
        ret = 0;
    }

    // if ((keyCtrl) && (ctrlDown)) {
    //    if ((keyShift) && (shiftDown)) {
    //        if ((keyAlt) && (altDown)) {
    //        } else if ((!keyAlt) && (!altDown)) {
    //        } else if (keyAlt) {
    //            ret = 0;
    //        }
    //    } else if ((!keyShift) && (!shiftDown)) {
    //        if ((keyAlt) && (altDown)) {
    //        } else if ((!keyAlt) && (!altDown)) {
    //        } else if (keyAlt) {
    //            ret = 0;
    //        }
    //    } else if (keyShift) {
    //        ret = 0;
    //    }
    //} else if ((!keyCtrl) && (!ctrlDown)) {
    //    if ((keyShift) && (shiftDown)) {
    //        if ((keyAlt) && (altDown)) {
    //        } else if ((!keyAlt) && (!altDown)) {
    //        } else if (keyAlt) {
    //            ret = 0;
    //        }
    //    } else if ((!keyShift) && (!shiftDown)) {
    //        if ((keyAlt) && (altDown)) {
    //        } else if ((!keyAlt) && (!altDown)) {
    //        } else if (keyAlt) {
    //            ret = 0;
    //        }
    //    } else if (keyShift) {
    //        ret = 0;
    //    }
    //} else if (keyCtrl) {
    //    ret = 0;
    //}

    return ret;
}

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
