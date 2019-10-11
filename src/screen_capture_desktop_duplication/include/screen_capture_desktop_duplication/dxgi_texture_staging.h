#pragma once

#include <wrl/client.h>
#include <d3d11.h>
#include <dxgi1_2.h>

class dxgi_device;

class dxgi_texture_staging;
class scoped_texture_mapper
{
public:
	scoped_texture_mapper(dxgi_texture_staging& texture);
	~scoped_texture_mapper();

	scoped_texture_mapper(const scoped_texture_mapper&) = delete;
	scoped_texture_mapper(scoped_texture_mapper&&) = delete;
	scoped_texture_mapper& operator = (const scoped_texture_mapper&) = delete;
	scoped_texture_mapper& operator = (scoped_texture_mapper&&) = delete;
private:
	dxgi_texture_staging& texture_;
};

/*!
 * texture staging is the processes of coping a gpu bound texture to a texture which the cpu can access.
 */
class dxgi_texture_staging
{
public:
	dxgi_texture_staging(const dxgi_device& device);
	~dxgi_texture_staging();
	
	void copy_from_texture(const DXGI_OUTDUPL_FRAME_INFO& frame_info, ID3D11Texture2D* texture);
	void map();
	void unmap();

	void* data() const noexcept;

	// width in bytes of a row
	unsigned int row_pitch() const noexcept;

	// width in bytes of a single slice. A single slice can contain multiple rows.
	unsigned int depth_pitch() const noexcept;

	unsigned int width() const noexcept;
	unsigned int height() const noexcept;

protected:
	void initialize(ID3D11Texture2D* texture);
private:
	const dxgi_device* device_{nullptr};
	D3D11_MAPPED_SUBRESOURCE map_{}; // dono if this is correct
	Microsoft::WRL::ComPtr<ID3D11Texture2D> staging_texure_;
	D3D11_TEXTURE2D_DESC current_texture_description_ = {};
};