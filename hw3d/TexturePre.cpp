#include "TexturePre.h"
#include "Surface.h"
#include "GraphicsThrowMacros.h"
#include "BindableCodex.h"

namespace Bind
{
	namespace wrl = Microsoft::WRL;

	TexturePre::TexturePre(Graphics& gfx, UINT slot, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> pTextureViewIn)
		:
		slot(slot)
	{
		//INFOMAN(gfx);
		
		pTextureView = pTextureViewIn;

	}

	void TexturePre::Bind(Graphics& gfx) noexcept
	{
		GetContext(gfx)->PSSetShaderResources(slot, 1u, pTextureView.GetAddressOf());
	}
	std::shared_ptr<TexturePre> TexturePre::Resolve(Graphics& gfx, UINT slot, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> pTextureViewIn)
	{
		return Codex::Resolve<TexturePre>(gfx, slot, pTextureViewIn);
	}
	std::string TexturePre::GenerateUID(UINT slot, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> pTextureView)
	{
		using namespace std::string_literals;
		return typeid(TexturePre).name() + "#"s + std::to_string(slot);
	}
	std::string TexturePre::GetUID() const noexcept
	{
		return GenerateUID(slot, pTextureView);
	}
}
