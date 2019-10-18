#include <screen_capture_desktop_duplication/dxgi_output_duplication.h>
#include <screen_capture_desktop_duplication/dxgi_device.h>

#include <fmt/printf.h>

dxgi_output_duplication::dxgi_output_duplication(const dxgi_device& device,
	const Microsoft::WRL::ComPtr<IDXGIOutput1>& output)
	: device_{device}
	, output_{output}
	, duplication_description_{}
	, texture_{device}
{
}

void dxgi_output_duplication::duplicate_output()
{
	if (const auto hr = output_->DuplicateOutput(device_.device(), &duplication_); hr != S_OK)
	{
		fmt::print("Error: failed to create the duplication output.\n");
		assert(false);
	}
	
	duplication_->GetDesc(&duplication_description_);

	DXGI_OUTPUT_DESC output_description = {};
	if (const auto hr = output_->GetDesc(&output_description); hr != S_OK)
	{
		fmt::print("Unable to get description from output\n");
		assert(false);
	}

	// here we can double check that the duplicated output has the same dimensions as the output (monitor).
	fmt::print("The monitor has the following dimensions: left: {}, right: {}, top: {}, bottom: {}.\n"
		, output_description.DesktopCoordinates.left
		, output_description.DesktopCoordinates.right
		, output_description.DesktopCoordinates.top
		, output_description.DesktopCoordinates.bottom
	);
}

void dxgi_output_duplication::report_acquisition_error(HRESULT hr)
{
	switch(hr)
	{
		case DXGI_ERROR_ACCESS_LOST: fmt::print("\nerror, DXGI_ERROR_ACCESS_LOST\n"); return;
		case DXGI_ERROR_WAIT_TIMEOUT: fmt::print("\nerror, DXGI_ERROR_WAIT_TIMEOUT\n"); return;
		case DXGI_ERROR_INVALID_CALL: fmt::print("\nerror, DXGI_ERROR_INVALID_CALL\n"); return;
	}
	fmt::print("\nunknown acquisition error: {}\n", hr);
}

dxgi_texture_staging* dxgi_output_duplication::frame_acquire(unsigned int timeout_in_ms)
{
	// because we rely on external scheduling we do not really have to rely on the dx one.
	timeout_in_ms = 0;

	// frame acquisition
	DXGI_OUTDUPL_FRAME_INFO frame_info{};
	Microsoft::WRL::ComPtr<IDXGIResource> frame_resource;

	const auto error = duplication_->AcquireNextFrame(timeout_in_ms, &frame_info, &frame_resource);
	if (error != S_OK && error != DXGI_ERROR_WAIT_TIMEOUT)
	{
		report_acquisition_error(error);
		//assert(false);
		return nullptr;
	}

	// check if we have a valid frame if not nothing has changed.
	if (error == S_OK && frame_info.AccumulatedFrames > 0 && frame_resource)
	{
		Microsoft::WRL::ComPtr<ID3D11Texture2D> texture;
		if (const auto hr = frame_resource->QueryInterface(__uuidof(ID3D11Texture2D), (void**)&texture); hr != S_OK)
		{
			fmt::print("Error: failed to query the ID3D11Texture2D interface on the IDXGIResource we got.\n");
			assert(false);
			return nullptr;
		}

		texture_.copy_from_texture(frame_info, texture.Get());
		texture_.map();
		return &texture_;
		
	}

	// this is wrong, we need a canvas so we can draw our annotations on them
	// nothing has changed. Don't
	return nullptr;	
}

void dxgi_output_duplication::frame_release()
{
	texture_.unmap();
	duplication_->ReleaseFrame();
}
