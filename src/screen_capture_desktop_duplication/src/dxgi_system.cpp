#include "screen_capture_desktop_duplication/dxgi_system.h"
#include <fmt/printf.h>

dxgi_system::dxgi_system()
{
	if (const auto hr = CreateDXGIFactory1(__uuidof(IDXGIFactory1), (void**)(&factory_)); hr != S_OK)
	{
		fmt::print("Unable to create factory\n");

		// \todo error handling
		assert(false);
	}

	for (UINT i = 0;;)
	{
		Microsoft::WRL::ComPtr<IDXGIAdapter1> adapter;

		// EnumAdapters1 returns DXGI_ERROR_NOT_FOUND or DXGI_ERROR_INVALID_CALL on error.
		// We actually don't care and we just break on anything that isn't OK.
		if (const auto hr = factory_->EnumAdapters1(i, &adapter); hr != S_OK)
			break;

		adapters_.push_back(adapter);
		i++;
	}
}

const std::vector<dxgi_adapter>& dxgi_system::get_adapters() const noexcept
{
	return adapters_;
}
