#pragma once
#include "Bindable.h"

class Surface;

namespace Bind
{
	class Texture : public Bindable
	{
	public:
		Texture(Graphics& gfx, const std::string& path, UINT slot = 0, bool cubemap = false,
			Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> pTextureViewIn = nullptr);
		void Bind( Graphics& gfx ) noexcept override;
		static std::shared_ptr<Texture> Resolve(Graphics& gfx, const std::string& path, UINT slot = 0, bool cubemap = false,
			Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> pTextureViewIn = nullptr);
		static std::string GenerateUID(const std::string& path, UINT slot = 0, bool cubemap = false,
			Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> pTextureView = nullptr);
		std::string GetUID() const noexcept override;
	private:
		unsigned int slot;
	protected:
		std::string path;
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> pTextureView;
		bool cubemap;
	};
}
