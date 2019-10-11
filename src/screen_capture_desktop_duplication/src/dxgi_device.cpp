#include "screen_capture_desktop_duplication/dxgi_device.h"
#include <fmt/printf.h>

dxgi_device::dxgi_device(const Microsoft::WRL::ComPtr<IDXGIAdapter1>& adapter)
{
	// DriverType: We use the GPU as backing device.
	constexpr auto driver_type = D3D_DRIVER_TYPE_UNKNOWN;
	constexpr auto software = nullptr;
	constexpr auto flags = 0;
	constexpr auto feature_levels = nullptr;
	constexpr auto feature_levels_count = 0;
	constexpr auto sdk_version = D3D11_SDK_VERSION;

	const auto hr = D3D11CreateDevice(adapter.Get(), driver_type, software, flags, feature_levels, feature_levels_count,
		sdk_version, &device_, &feature_level_, &context_);

	if (hr != S_OK)
	{
		fmt::print("Error: failed to create the D3D11 Device.\n");
		if (E_INVALIDARG == hr)
			fmt::print("Got INVALID arg passed into D3D11CreateDevice. Did you pass a adapter + a driver which is not the UNKNOWN driver?.\n");

		assert(false);
	}
}

ID3D11Device* dxgi_device::device() const noexcept
{
	return device_.Get();
}

ID3D11DeviceContext* dxgi_device::context() const noexcept
{
	return context_.Get();
}

