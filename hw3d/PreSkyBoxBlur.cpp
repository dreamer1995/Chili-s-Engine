#include "PreSkyBoxBlur.h"
#include "Cube.h"
#include "BindableCommon.h"
#include "imgui/imgui.h"

PreSkyBoxBlur::PreSkyBoxBlur(Graphics& gfx, float size)
{
	using namespace Bind;
	namespace dx = DirectX;

	auto model = Cube::Make();
	model.Transform(dx::XMMatrixScaling(size, size, size));
	const auto geometryTag = "$preskybox." + std::to_string(size);
	AddBind(VertexBuffer::Resolve(gfx, geometryTag, model.vertices));
	AddBind(IndexBuffer::Resolve(gfx, geometryTag, model.indices));

	//AddBind(Texture::Resolve(gfx, "Images\\jellybeans1.jpg", 0u, true));
	AddBind(TexturePre::Resolve(gfx, 0u, gfx.GetShaderResourceViewH()));

	auto pvs = VertexShader::Resolve(gfx, "SkyBoxVS.cso");
	auto pvsbc = pvs->GetBytecode();
	AddBind(std::move(pvs));
	
	AddBind(PixelShader::Resolve(gfx, "SkyboxConvolutionPS.cso"));

	AddBind(InputLayout::Resolve(gfx, model.vertices.GetLayout(), pvsbc));

	AddBind(Topology::Resolve(gfx, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST));

	AddBind(std::make_shared<TransformCbuf>(gfx, *this));

	AddBind(Sampler::Resolve(gfx));
}

void PreSkyBoxBlur::SetRotation(float roll, float pitch, float yaw) noexcept
{
	this->pitch = pitch;
	this->yaw = yaw;
	this->roll = roll;
}

DirectX::XMMATRIX PreSkyBoxBlur::GetTransformXM() const noexcept
{
	return DirectX::XMMatrixRotationRollPitchYaw(pitch, yaw, roll);
}

void PreSkyBoxBlur::Reset() noexcept
{
	pitch = 0.0f;
	yaw = 0.0f;
	roll = 0.0f;
}
