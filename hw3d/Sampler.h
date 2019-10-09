#pragma once
#include "Bindable.h"

namespace Bind
{
	class Sampler : public Bindable
	{
	public:
		Sampler(Graphics& gfx, char type = 'D');
		void Bind( Graphics& gfx ) noexcept override;
		static std::shared_ptr<Sampler> Resolve(Graphics& gfx, char type = 'D');
		static std::string GenerateUID(char type);
		std::string GetUID() const noexcept override;
	protected:
		Microsoft::WRL::ComPtr<ID3D11SamplerState> pSampler;
	private:
		char type;
	};
}
