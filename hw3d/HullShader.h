#pragma once
#include "Bindable.h"

namespace Bind
{
	class HullShader : public Bindable
	{
	public:
		HullShader(Graphics& gfx, const std::string& path);
		void Bind(Graphics& gfx) noexcept override;
		static std::shared_ptr<HullShader> Resolve(Graphics& gfx, const std::string& path);
		static std::string GenerateUID(const std::string& path);
		std::string GetUID() const noexcept override;
	protected:
		std::string path;
		Microsoft::WRL::ComPtr<ID3D11HullShader> pHullShader;
	};
}