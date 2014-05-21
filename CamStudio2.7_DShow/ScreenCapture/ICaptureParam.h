/**********************************************
 *  File: ICaptureParams.h
 *  Desc: Capture APIs for CamStudio Screen Capture
 *  Author: Alberto A. Heredia (bertoso)
 *
 **********************************************/
#pragma once

// {77F36C8D-DAB1-4356-8273-3FBE6063F257}
static const GUID IID_ICaptureParam = 
{ 0x77f36c8d, 0xdab1, 0x4356, { 0x82, 0x73, 0x3f, 0xbe, 0x60, 0x63, 0xf2, 0x57 } };

DECLARE_INTERFACE_(ICaptureParam, IUnknown)
{
	//STDMETHOD(StopCapture) (void) PURE;
	STDMETHOD(CaptureFix) (HWND hWnd, RECT rc) PURE;
	STDMETHOD(CaptureVariable) (HWND hWnd, RECT rc) PURE;
	STDMETHOD(CaptureAllScreens) (RECT rc) PURE;
	STDMETHOD(CaptureWindow) (HWND hWnd, HWND hOwner) PURE;
	STDMETHOD(CaptureFullScreen) (RECT rc) PURE;
	STDMETHOD(SetFPS)(int nFPS) PURE;
	STDMETHOD(EnableAutoPan)(bool bEnable, int iSpeed) PURE;
	STDMETHOD(SetFlashingWindow)(HWND hwnd) PURE;
	//STDMETHOD(SetParentWindow) (HWND hWnd) PURE;
	// When setting up capture params, call the method below before anyone else
	STDMETHOD(DisplayFlashingWindow)(bool bDisplay) PURE;
};