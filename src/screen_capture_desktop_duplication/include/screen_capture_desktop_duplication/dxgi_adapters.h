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

/* the purpose of this class is to create a list of outputs that the user (developer) can duplicate */
class dxgi_system
{
public:
	dxgi_system();
	dxgi_system(const dxgi_system&) = delete;
	dxgi_system& operator=(const dxgi_system&) = delete;

	dxgi_system(dxgi_system&&) = default;
	dxgi_system& operator=(dxgi_system&&) = default;

	const std::vector<dxgi_adapter>& get_adapters() const noexcept;

private:
	// interface for generating DXGI objects.
	Microsoft::WRL::ComPtr<IDXGIFactory1> factory_;

	// list of adapters, one represents a display sub-system (including one or more GPU's, DACs and
	// video memory).
	std::vector<dxgi_adapter> adapters_;	
};

