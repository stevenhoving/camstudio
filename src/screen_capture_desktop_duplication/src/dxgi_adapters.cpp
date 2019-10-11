#include "screen_capture_desktop_duplication/dxgi_adapters.h"
#include <fmt/printf.h>

const std::vector<Microsoft::WRL::ComPtr<IDXGIOutput1>>& dxgi_adapter::get_outputs() const noexcept
{
	return outputs_;
}

dxgi_adapter::dxgi_adapter(Microsoft::WRL::ComPtr<IDXGIAdapter1> adapter)
	: adapter_(adapter)
{
	for (UINT i = 0;;)
	{
		Microsoft::WRL::ComPtr<IDXGIOutput> output;

		// EnumOutputs returns DXGI_ERROR_NOT_FOUND when we reached the end of the outputs.
		// We actually don't care and just break on anything that isn't OK.
		if (const auto hr = adapter_->EnumOutputs(i, &output); hr != S_OK)
			break;

		Microsoft::WRL::ComPtr<IDXGIOutput1> output1;
		if (const auto hr = output->QueryInterface(__uuidof(IDXGIOutput1), (void**)&output1); hr != S_OK)
		{
			fmt::print("Error: failed to query the IDXGIOutput1 interface.\n");
			assert(false);
		}

		outputs_.push_back(output1);
		++i;
	}
}

IDXGIAdapter1* dxgi_adapter::get_adapter() const
{
	return adapter_.Get();
}

dxgi_system::dxgi_system()
{
	if (const auto hr = CreateDXGIFactory1(__uuidof(IDXGIFactory1), (void**)(&factory_)); hr != S_OK)
	{
		fmt::print("Unable to create factory\n");

		// \todo error handling
		assert(false);
	}

	// what to do with this snippet of code... for now we leave it here...
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
