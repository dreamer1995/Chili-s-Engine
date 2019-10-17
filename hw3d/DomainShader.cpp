#include "DomainShader.h"
#include "GraphicsThrowMacros.h"
#include "BindableCodex.h"

namespace Bind
{
	DomainShader::DomainShader(Graphics& gfx, const std::string& path)
		:
		path(path)
	{
		INFOMAN(gfx);

		Microsoft::WRL::ComPtr<ID3DBlob> pBlob;
		GFX_THROW_INFO(D3DReadFileToBlob(std::wstring{ path.begin(),path.end() }.c_str(), &pBlob));
		GFX_THROW_INFO(GetDevice(gfx)->CreateDomainShader(pBlob->GetBufferPointer(), pBlob->GetBufferSize(), nullptr, &pDomainShader));
	}

	void DomainShader::Bind(Graphics& gfx) noexcept
	{
		GetContext(gfx)->DSSetShader(pDomainShader.Get(), nullptr, 0u);
	}
	std::shared_ptr<DomainShader> DomainShader::Resolve(Graphics& gfx, const std::string& path)
	{
		return Codex::Resolve<DomainShader>(gfx, path);
	}
	std::string DomainShader::GenerateUID(const std::string& path)
	{
		using namespace std::string_literals;
		return typeid(DomainShader).name() + "#"s + path;
	}
	std::string DomainShader::GetUID() const noexcept
	{
		return GenerateUID(path);
	}
}
