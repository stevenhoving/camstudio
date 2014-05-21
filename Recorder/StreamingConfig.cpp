#include "StdAfx.h"
#include "StreamingConfig.h"


CStreamingConfig::CStreamingConfig(void)
{
}


CStreamingConfig::~CStreamingConfig(void)
{
}
HRESULT CStreamingConfig::GetStreamingCaps(ACamstudioFilter *pFilter)
{
	IEnumPins *pEnum = NULL;
	HRESULT hr = pFilter->GetFilter()->EnumPins(&pEnum);
	if (SUCCEEDED(hr))
	{
		IPin *pPin = NULL;
		while(S_OK == pEnum->Next(1, &pPin, NULL))
		{
			/*IEnumMediaTypes *pEnumMT = NULL;
			AM_MEDIA_TYPE *p = NULL;
			pPin->EnumMediaTypes(&pEnumMT);
			while(S_OK == pEnumMT->Next(1,&p, NULL))
			{
			}*/
			PIN_INFO info;
			hr = pPin->QueryPinInfo(&info);
			if(SUCCEEDED(hr))
			{
				if(info.dir == PINDIR_OUTPUT)
				{
					IAMStreamConfig *pConf;
					hr = pPin->QueryInterface(IID_IAMStreamConfig, (void**)&pConf);
					if (SUCCEEDED(hr))
					{
						CString sFormat;
						int iCount, iSize;
						BYTE *pSCC = NULL;
						AM_MEDIA_TYPE *pmt;
						float fSample;
						// TODO : FAILS WHEN PIN IS NOT CONNECTED
						//hr = pConf->GetFormat(&pmt);
						//if(SUCCEEDED(hr))
						//{
						//	WAVEFORMATEX *pCurrentFormat = reinterpret_cast<WAVEFORMATEX*>(pmt->pbFormat);
						//	if(pCurrentFormat)
						//	{
						//		fSample = pCurrentFormat->nSamplesPerSec/1000;
						//		sFormat.Format("%.2f kHz, %s, %d-bits", fSample, (pCurrentFormat->nChannels == 1) ? "mono" : "stereo", pCurrentFormat->wBitsPerSample);
						//		MessageBox(NULL, sFormat, "", 0);
						//	}
						//	else
						//		MessageBox(NULL, "null", "", 0);
						//	//DeleteMediaType(pmt);
						//}
						//else
						//{
						//	MessageBox(NULL, "fail", "", 0);
						//	if(hr == E_OUTOFMEMORY)
						//		MessageBox(NULL,"E_OUTOFMEMORY", "", 0);
						//	else if(hr == E_POINTER)
						//		MessageBox(NULL, "E_POINTER", "", 0);
						//	else if(hr == VFW_E_NOT_CONNECTED)
						//		MessageBox(NULL, "VFW_E_NOT_CONNECTED", "", 0);
						//}
						hr = pConf->GetNumberOfCapabilities(&iCount, &iSize);
						if(FAILED(hr))
						{
							MessageBox(NULL, "failed", "", 0);
						}
						pSCC = new BYTE[iSize];
						if (pSCC == NULL)
						{
							return E_POINTER;
						}
						if (iSize == sizeof(AUDIO_STREAM_CONFIG_CAPS))
						{
							// Use the video capabilities structure.
							for (int iFormat = 0; iFormat < iCount; iFormat++)
							{
								AUDIO_STREAM_CONFIG_CAPS scc;
								AM_MEDIA_TYPE *pmtConfig;
								hr = pConf->GetStreamCaps(iFormat, &pmtConfig, (BYTE*)&scc);
								if (SUCCEEDED(hr))
								{
									WAVEFORMATEX *pFormat = reinterpret_cast<WAVEFORMATEX*>(pmtConfig->pbFormat);
									if(pFormat)
									{
									}
								
									//FreeMediaType(*pmtConfig); 
									//CoTaskMemFree(pmtConfig);
								}
							}
						}
						else if(iSize == sizeof(VIDEO_STREAM_CONFIG_CAPS))
						{
							CString s;
							
							s.Format("%d", iCount);
							MessageBox(NULL, s, "", 0);
							for (int iFormat = 0; iFormat < iCount; iFormat++)
							{
								VIDEO_STREAM_CONFIG_CAPS scc;
								AM_MEDIA_TYPE *pmtConfig;
								hr = pConf->GetFormat(&pmtConfig);
								if(SUCCEEDED(hr))
								{
									if(pmtConfig->formattype == FORMAT_VideoInfo)
									{
										if (pmt->cbFormat >= sizeof(VIDEOINFOHEADER))
										{
											VIDEOINFOHEADER *pVih = 
												reinterpret_cast<VIDEOINFOHEADER*>(pmt->pbFormat);
											MessageBox(NULL, "got !", "", 0);
											/* Access VIDEOINFOHEADER members through pVih. */
										}
									}
								}
								else if(hr ==VFW_E_NOT_CONNECTED)
								{
									MessageBox(NULL, "VFW_E_NOT_CONNECTED 1	", "", 0);
								}
								hr = pConf->GetStreamCaps(iFormat, &pmtConfig, (BYTE*)&scc);
								if (SUCCEEDED(hr))
								{
									 VIDEO_STREAM_CONFIG_CAPS *pFormat = reinterpret_cast< VIDEO_STREAM_CONFIG_CAPS*>(pmtConfig->pbFormat);
									if(pFormat)
									{
										int min = pFormat->MinFrameInterval;
										int max = pFormat->MaxFrameInterval;
										CString s;
										s.Format("%d->%d", min, max);
										MessageBox(NULL, s, "", 0);
									}
								
									//FreeMediaType(*pmtConfig); 
									//CoTaskMemFree(pmtConfig);
								}
								else if(hr == VFW_E_NOT_CONNECTED)
									MessageBox(NULL, "VFW_E_NOT_CONNECTED", "", 0);

							}
						}
						else
						{

						}
						delete pSCC;
						pConf->Release();
					}
				}
			}
			pPin->Release();
		}
	pEnum->Release();
	}
}