#include "screen_capture_desktop_duplication/dxgi_adapter.h"
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

