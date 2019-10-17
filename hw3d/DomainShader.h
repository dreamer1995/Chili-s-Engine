#pragma once
#include "Bindable.h"

namespace Bind
{
	class DomainShader : public Bindable
	{
	public:
		DomainShader(Graphics& gfx, const std::string& path);
		void Bind(Graphics& gfx) noexcept override;
		static std::shared_ptr<DomainShader> Resolve(Graphics& gfx, const std::string& path);
		static std::string GenerateUID(const std::string& path);
		std::string GetUID() const noexcept override;
	protected:
		std::string path;
		Microsoft::WRL::ComPtr<ID3D11DomainShader> pDomainShader;
	};
}