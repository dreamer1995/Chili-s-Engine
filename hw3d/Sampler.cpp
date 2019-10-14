#include "Sampler.h"
#include "GraphicsThrowMacros.h"
#include "BindableCodex.h"

namespace Bind
{
	Sampler::Sampler(Graphics& gfx, char type)
		:type(type)
	{
		INFOMAN( gfx );

		D3D11_SAMPLER_DESC samplerDesc = {};
		samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		if (type == 'C')
		{
			samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
			samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
			samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
			GFX_THROW_INFO(GetDevice(gfx)->CreateSamplerState(&samplerDesc, &pSampler));
		}
		else
		{
			samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
			samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
			samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
			samplerDesc.MaxAnisotropy = 16;
			samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
			GFX_THROW_INFO(GetDevice(gfx)->CreateSamplerState(&samplerDesc, &pSampler));
		}
	}

	void Sampler::Bind( Graphics& gfx ) noexcept
	{
		UINT slot = 0u;
		if (type == 'C')
		{
			slot = 1u;
		}
		GetContext(gfx)->VSSetSamplers(slot, 1, pSampler.GetAddressOf());
		GetContext( gfx )->PSSetSamplers(slot,1,pSampler.GetAddressOf() );
	}
	std::shared_ptr<Sampler> Sampler::Resolve(Graphics& gfx, char type)
	{
		return Codex::Resolve<Sampler>(gfx, type);
	}
	std::string Sampler::GenerateUID(char type)
	{
		using namespace std::string_literals;
		return typeid(Sampler).name() + "#"s + std::to_string(type);
	}
	std::string Sampler::GetUID() const noexcept
	{
		return GenerateUID(type);
	}
}