// CStudioLib.h	- include file for CamStudio Library
/////////////////////////////////////////////////////////////////////////////
#ifndef CSTUDIOLIB_H
#define CSTUDIOLIB_H

#pragma once

#include "CamRect.h"	// generic rectangle
#include "CamWindow.h"	// generic window
#include "CamFile.h"	// generic file
#include "CamSys.h"		// generic system
#include "CamImage.h"	// generic image
#include "Picture.h"
#include "CBitmapEx.h"

// numeric
long rounddouble(double dVal);

// keyboard
// TODO: matchSpecial unreferenced
int matchSpecial(int keyCtrl, int keyShift, int keyAlt, int ctrlDown, int shiftDown, int altDown);

#endif	// CSTUDIOLIB_H
