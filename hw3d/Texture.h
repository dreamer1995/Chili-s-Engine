#pragma once
#include "Bindable.h"

class Surface;

namespace Bind
{
	class Texture : public Bindable
	{
	public:
		Texture(Graphics& gfx, const std::string& path, UINT slot = 0,
			bool cubemap = false, bool toVS = false, bool toDS = false);
		void Bind( Graphics& gfx ) noexcept override;
		static std::shared_ptr<Texture> Resolve(Graphics& gfx, const std::string& path, UINT slot = 0,
			bool cubemap = false, bool toVS = false, bool toDS = false);
		static std::string GenerateUID(const std::string& path, UINT slot = 0,
			bool cubemap = false, bool toVS = false, bool toDS = false);
		std::string GetUID() const noexcept override;
	private:
		unsigned int slot;
	protected:
		std::string path;
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> pTextureView;
		bool cubemap;
		bool toVS;
		bool toDS;
	};
}
