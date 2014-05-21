#pragma once
#include "VideoCaptureDevice.h"
class CVideoAnnotationCG
{
public:
	CVideoAnnotationCG(CVideoCaptureDevice *pVideoCap, const HWND hOwner);
	~CVideoAnnotationCG(void);
	HRESULT BuildCaptureGraph();
	HRESULT InitFilters();
	void StartPreview();
	void StopPreview();
private:

	void ShowError(HRESULT hr, LPCTSTR lpCaption);

	HWND m_hOwner;
	ICaptureGraphBuilder2 *m_pBuilder;
	IBaseFilter *m_pVCD;
	IGraphBuilder *m_pGB;
	IVideoWindow *m_pVW;
};

