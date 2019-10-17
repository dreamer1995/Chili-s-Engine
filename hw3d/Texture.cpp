#include "Texture.h"
#include "Surface.h"
#include "GraphicsThrowMacros.h"
#include "BindableCodex.h"

namespace Bind
{
	namespace wrl = Microsoft::WRL;

	Texture::Texture( Graphics& gfx,const std::string& path,UINT slot,bool cubemap, bool toVS, bool toDS)
		:
		path( path ),
		slot( slot ),
		cubemap(cubemap),
		toVS(toVS),
		toDS(toDS)
	{
		INFOMAN( gfx );

		Surface s(0, 0);

		// load surface
		D3D11_TEXTURE2D_DESC textureDesc = {};
		if (cubemap)
		{
			s = Surface::FromFile(path, true);
			textureDesc.ArraySize = 6;
			textureDesc.MiscFlags = D3D11_RESOURCE_MISC_TEXTURECUBE;
		}
		else
		{
			s = Surface::FromFile(path);
			textureDesc.ArraySize = 1;
			textureDesc.MiscFlags = 0;
		}
		// create texture resource

		textureDesc.Width = s.GetWidth();
		textureDesc.Height = s.GetHeight();
		textureDesc.MipLevels = 1;
		textureDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
		textureDesc.SampleDesc.Count = 1;
		textureDesc.SampleDesc.Quality = 0;
		textureDesc.Usage = D3D11_USAGE_DEFAULT;
		textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		textureDesc.CPUAccessFlags = 0;

		wrl::ComPtr<ID3D11Texture2D> pTexture;
		if (cubemap)
		{
			D3D11_SUBRESOURCE_DATA sd[6] = {};
			for (char i = 0; i < 6; i++)
			{
				sd[i].pSysMem = s.GetBufferPtrByIndex(s.GetWidth() * s.GetHeight() * i);
				sd[i].SysMemPitch = s.GetWidth() * sizeof(Surface::Color);
				sd[i].SysMemSlicePitch = 0;
			}

			GFX_THROW_INFO(GetDevice(gfx)->CreateTexture2D(
				&textureDesc, &sd[0], &pTexture
			));
		}
		else
		{
			D3D11_SUBRESOURCE_DATA sd = {};
			sd.pSysMem = s.GetBufferPtr();
			sd.SysMemPitch = s.GetWidth() * sizeof(Surface::Color);

			GFX_THROW_INFO(GetDevice(gfx)->CreateTexture2D(
				&textureDesc, &sd, &pTexture
			));
		}

		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		srvDesc.Format = textureDesc.Format;
		if (cubemap)
		{
			srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
			srvDesc.TextureCube.MostDetailedMip = 0;
			srvDesc.TextureCube.MipLevels = 1;
		}
		else
		{
			srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
			srvDesc.Texture2D.MostDetailedMip = 0;
			srvDesc.Texture2D.MipLevels = 1;
		}

		GFX_THROW_INFO(GetDevice(gfx)->CreateShaderResourceView(
			pTexture.Get(), &srvDesc, &pTextureView
		));
	}

	void Texture::Bind( Graphics& gfx ) noexcept
	{
		if (toVS && !toDS)
		{
			GetContext(gfx)->VSSetShaderResources(slot, 1u, pTextureView.GetAddressOf());
		}
		else if (!toVS && toDS)
		{
			GetContext(gfx)->DSSetShaderResources(slot, 1u, pTextureView.GetAddressOf());
		}
		else
		{
			GetContext( gfx )->PSSetShaderResources( slot,1u,pTextureView.GetAddressOf() );
		}
	}
	std::shared_ptr<Texture> Texture::Resolve(Graphics& gfx, const std::string& path, UINT slot, bool cubemap, bool toVS, bool toDS)
	{
		return Codex::Resolve<Texture>(gfx, path, slot, cubemap, toVS, toDS);
	}
	std::string Texture::GenerateUID(const std::string& path, UINT slot, bool cubemap, bool toVS, bool toDS)
	{
		using namespace std::string_literals;
		return typeid(Texture).name() + "#"s + path + "#" + std::to_string( slot );
	}
	std::string Texture::GetUID() const noexcept
	{
		return GenerateUID(path, slot, cubemap, toVS, toDS);
	}
}
