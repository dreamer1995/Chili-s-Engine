#include "PreSkyBox.h"
#include "Cube.h"
#include "BindableCommon.h"
#include "imgui/imgui.h"

PreSkyBox::PreSkyBox(Graphics& gfx, float size, char type)
{
	using namespace Bind;
	namespace dx = DirectX;

	auto model = Cube::Make();
	model.Transform(dx::XMMatrixScaling(size, size, size));
	const auto geometryTag = "$preskybox." + std::to_string(size);
	AddBind(VertexBuffer::Resolve(gfx, geometryTag, model.vertices));
	AddBind(IndexBuffer::Resolve(gfx, geometryTag, model.indices));

	auto pvs = VertexShader::Resolve(gfx, "SkyBoxVS.cso");
	auto pvsbc = pvs->GetBytecode();
	AddBind(std::move(pvs));

	if (type == 'B')
	{
		AddBind(TexturePre::Resolve(gfx, 0u, gfx.GetShaderResourceView('H')));

		AddBind(PixelShader::Resolve(gfx, "SkyboxConvolutionPS.cso"));
	}
	else if (type == 'M')
	{
		AddBind(TexturePre::Resolve(gfx, 0u, gfx.GetShaderResourceView('H')));

		AddBind(PixelShader::Resolve(gfx, "PrefilterMapPixelShader.cso"));

		AddBind(PixelConstantBuffer<PSMaterialConstant>::Resolve(gfx, pmc, 3u));
	}
	else
	{
		AddBind(Texture::Resolve(gfx, "Images\\EpicQuadPanorama_CC+EV1.jpg"));

		AddBind(PixelShader::Resolve(gfx, "SphereToCubePS.cso"));
	}
	
	AddBind(InputLayout::Resolve(gfx, model.vertices.GetLayout(), pvsbc));

	AddBind(Topology::Resolve(gfx, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST));

	AddBind(std::make_shared<TransformCbuf>(gfx, *this));

	AddBind(Sampler::Resolve(gfx));

	AddBind(Sampler::Resolve(gfx, 'C'));
}

void PreSkyBox::SetRotation(float roll, float pitch, float yaw) noexcept
{
	this->pitch = pitch;
	this->yaw = yaw;
	this->roll = roll;
}

DirectX::XMMATRIX PreSkyBox::GetTransformXM() const noexcept
{
	return DirectX::XMMatrixRotationRollPitchYaw(pitch, yaw, roll);
}

void PreSkyBox::Reset() noexcept
{
	pitch = 0.0f;
	yaw = 0.0f;
	roll = 0.0f;
}

void PreSkyBox::ChangeSphereMaterialState(Graphics& gfx) noexcept
{
	QueryBindable<Bind::PixelConstantBuffer<PSMaterialConstant>>()->Update(gfx, pmc);
}