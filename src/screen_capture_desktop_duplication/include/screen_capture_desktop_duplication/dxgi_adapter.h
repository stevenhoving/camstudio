#pragma once

#include <wrl/client.h>
#include <dxgi.h>
#include <dxgi1_2.h>
#include <vector>

class dxgi_adapter
{
public:
	dxgi_adapter() = default;
	dxgi_adapter(Microsoft::WRL::ComPtr<IDXGIAdapter1> adapter);

	IDXGIAdapter1* get_adapter() const;

	// the purpose of this function is to return the list of outputs that the user could duplicate.
	const std::vector<Microsoft::WRL::ComPtr<IDXGIOutput1>>& get_outputs() const noexcept;

private:
	Microsoft::WRL::ComPtr<IDXGIAdapter1> adapter_;

	// list of outputs, one represents an adapter output (such as a monitor).
	std::vector<Microsoft::WRL::ComPtr<IDXGIOutput1>> outputs_;
};
