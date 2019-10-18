#pragma once

#include <screen_capture_desktop_duplication/dxgi_adapter.h>
#include <wrl/client.h>
#include <dxgi.h>
#include <dxgi1_2.h>
#include <vector>


/* the purpose of this class is to create a list of outputs that the user (developer) can duplicate */
class dxgi_system
{
public:
	dxgi_system();
	dxgi_system(const dxgi_system&) = delete;
	dxgi_system& operator=(const dxgi_system&) = delete;

	dxgi_system(dxgi_system&&) = default;
	dxgi_system& operator=(dxgi_system&&) = default;

	// returns a list of gpu adapters.
	const std::vector<dxgi_adapter>& get_adapters() const noexcept;

private:
	// interface for generating DXGI objects.
	Microsoft::WRL::ComPtr<IDXGIFactory1> factory_;

	// list of adapters, one represents a display sub-system (including one or more GPU's, DACs and
	// video memory).
	std::vector<dxgi_adapter> adapters_;
};