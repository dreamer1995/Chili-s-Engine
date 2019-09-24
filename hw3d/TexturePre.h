#pragma once
#include "Bindable.h"

class Surface;

namespace Bind
{
	class TexturePre : public Bindable
	{
	public:
		TexturePre(Graphics& gfx, UINT slot = 0,Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> pTextureViewIn = nullptr);
		void Bind(Graphics& gfx) noexcept override;
		static std::shared_ptr<TexturePre> Resolve(Graphics& gfx, UINT slot = 0, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> pTextureViewIn = nullptr);
		static std::string GenerateUID(UINT slot = 0, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> pTextureView = nullptr);
		std::string GetUID() const noexcept override;
	private:
		unsigned int slot;
	protected:
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> pTextureView;
	};
}
