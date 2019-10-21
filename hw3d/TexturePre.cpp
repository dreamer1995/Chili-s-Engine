#include "TexturePre.h"
#include "Surface.h"
#include "GraphicsThrowMacros.h"
#include "BindableCodex.h"

namespace Bind
{
	namespace wrl = Microsoft::WRL;

	TexturePre::TexturePre(Graphics& gfx, UINT slot, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> pTextureViewIn, bool toDS)
		:
		slot(slot),
		toDS(toDS)
	{
		//INFOMAN(gfx);
		
		pTextureView = pTextureViewIn;

	}

	void TexturePre::Bind(Graphics& gfx) noexcept
	{
		if (toDS)
		{
			GetContext(gfx)->DSSetShaderResources(slot, 1u, pTextureView.GetAddressOf());
		}
		else
		{
			GetContext(gfx)->PSSetShaderResources(slot, 1u, pTextureView.GetAddressOf());
		}
	}
	std::shared_ptr<TexturePre> TexturePre::Resolve(Graphics& gfx, UINT slot,
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> pTextureViewIn, bool toDS)
	{
		return Codex::Resolve<TexturePre>(gfx, slot, pTextureViewIn, toDS);
	}
	std::string TexturePre::GenerateUID(UINT slot,
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> pTextureView, bool toDS)
	{
		using namespace std::string_literals;
		return typeid(TexturePre).name() + "#"s + std::to_string(slot);
	}
	std::string TexturePre::GetUID() const noexcept
	{
		return GenerateUID(slot, pTextureView, toDS);
	}
}
