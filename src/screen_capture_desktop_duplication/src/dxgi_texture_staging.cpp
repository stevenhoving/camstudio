#include "screen_capture_desktop_duplication/dxgi_texture_staging.h"
#include "screen_capture_desktop_duplication/dxgi_device.h"
#include <fmt/printf.h>

scoped_texture_mapper::scoped_texture_mapper(dxgi_texture_staging& texture)
	: texture_(texture)
{
	texture_.map();
}


scoped_texture_mapper::~scoped_texture_mapper()
{
	texture_.unmap();
}

dxgi_texture_staging::dxgi_texture_staging(const dxgi_device& device)
	: device_(&device)
{
}

dxgi_texture_staging::~dxgi_texture_staging() = default;

void dxgi_texture_staging::initialize(ID3D11Texture2D* texture)
{
	D3D11_TEXTURE2D_DESC texture_description = {};
	texture->GetDesc(&texture_description);

	texture_description.ArraySize = 1;
	texture_description.BindFlags = 0;
	texture_description.CPUAccessFlags = D3D11_CPU_ACCESS_READ;

	// Use 1 for a multisampled texture; or 0 to generate a full set of subtextures.
	texture_description.MipLevels = 1;
	texture_description.MiscFlags = 0;
	texture_description.SampleDesc.Count = 1;
	texture_description.SampleDesc.Quality = 0;
	texture_description.Usage = D3D11_USAGE_STAGING;

	// do we have to recreate the staging texture?
	if (staging_texure_)
	{
		if (memcmp(&texture_description, &current_texture_description_, sizeof(current_texture_description_)) == 0)
		{
			// no need to recreate the staging texture.
			return;
		}
	}

	current_texture_description_ = texture_description;

	if (const auto hr = device_->device()->CreateTexture2D(&current_texture_description_, nullptr, &staging_texure_); hr == E_INVALIDARG)
	{
		fmt::print("Error: received E_INVALIDARG when trying to create the texture.\n");
		assert(false);
	}
	else if (hr != S_OK)
	{
		fmt::print("Error: failed to create the 2D texture, error: {}.\n", hr);
		assert(false);
	}
}

void dxgi_texture_staging::copy_from_texture(const DXGI_OUTDUPL_FRAME_INFO& frame_info, ID3D11Texture2D* texture)
{
	initialize(texture);
	device_->context()->CopyResource(staging_texure_.Get(), texture);
}

void dxgi_texture_staging::map()
{
	constexpr auto subresource = 0;
	constexpr auto map_type = D3D11_MAP_READ;
	constexpr auto map_flags = 0;
	const auto hr = device_->context()->Map(staging_texure_.Get(), subresource, map_type, map_flags, &map_);

	// \todo error handling
	assert(hr == S_OK);
}

void dxgi_texture_staging::unmap()
{
	constexpr auto subresource = 0;
	device_->context()->Unmap(staging_texure_.Get(), subresource);
}

void* dxgi_texture_staging::data() const noexcept
{
	return map_.pData;
}

unsigned int dxgi_texture_staging::row_pitch() const noexcept
{
	return map_.RowPitch;
}

unsigned int dxgi_texture_staging::depth_pitch() const noexcept
{
	return map_.DepthPitch;
}

unsigned int dxgi_texture_staging::width() const noexcept
{
	return current_texture_description_.Width;
}

unsigned int dxgi_texture_staging::height() const noexcept
{
	return current_texture_description_.Height;
}

