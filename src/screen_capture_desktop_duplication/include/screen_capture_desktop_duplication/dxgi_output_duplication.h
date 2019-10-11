#pragma once

#include "dxgi_texture_staging.h"
#include <wrl/client.h>
#include <d3d11.h>
#include <dxgi1_2.h>

class dxgi_device;
class dxgi_output_duplication
{
public:
	dxgi_output_duplication(const dxgi_device& device, const Microsoft::WRL::ComPtr<IDXGIOutput1>& output);
	void duplicate_output();

	dxgi_texture_staging* frame_acquire(unsigned int timeout_in_ms);
	void frame_release();
protected:
	void report_acquisition_error(HRESULT hr);
private:
	const dxgi_device& device_;
	const Microsoft::WRL::ComPtr<IDXGIOutput1> output_;
	Microsoft::WRL::ComPtr<IDXGIOutputDuplication> duplication_;
	DXGI_OUTDUPL_DESC duplication_description_;

	// currently we only support staging textures. But it is possible to use desktop mapped textures (igp).
	dxgi_texture_staging texture_;
	
};
