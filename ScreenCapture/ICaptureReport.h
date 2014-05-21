/**********************************************
 *  File: ICaptureReport.h
 *  Desc: Capture statistics API for CamStudio Screen Capture
 *  Author: Alberto A. Heredia (bertoso)
 *
 **********************************************/
#pragma once
// {31B7DAB1-2CBF-4b8f-8386-4DFB13F6C3C5}
static const GUID IID_ICaptureReport = 
{ 0x31b7dab1, 0x2cbf, 0x4b8f, { 0x83, 0x86, 0x4d, 0xfb, 0x13, 0xf6, 0xc3, 0xc5 } };

DECLARE_INTERFACE_(ICaptureReport, IUnknown)
{
	STDMETHOD_(unsigned long, GetCurrentCaptureSize) (void) const PURE;
	STDMETHOD_(int, GetFramesWritten) (void) const PURE;
	STDMETHOD_(float, GetActualFPS) (void) PURE;
	STDMETHOD_(unsigned long, GetTimeElapsed) (void) const PURE;
	STDMETHOD_(RECT, GetCaptureRect) (void) PURE;
	STDMETHOD_(POINT, GetPoint) (void) PURE;
	STDMETHOD_(int, GetStatus) (void) PURE;
};