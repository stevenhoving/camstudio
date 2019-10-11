#pragma once

#include <wrl/client.h>
#include <d3d11.h>

class dxgi_device
{
public:
	dxgi_device(const Microsoft::WRL::ComPtr<IDXGIAdapter1>& adapter);

	ID3D11Device* device() const noexcept;
	ID3D11DeviceContext* context() const noexcept;
private:
	Microsoft::WRL::ComPtr<ID3D11Device> device_;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> context_;
	D3D_FEATURE_LEVEL feature_level_ = {};
};