#include "SolidArrow.h"
#include "BindableCommon.h"
#include "GraphicsThrowMacros.h"
#include "Vertex.h"
#include "Arrow.h"


SolidArrow::SolidArrow(Graphics& gfx, float size)
{
	using namespace Bind;
	namespace dx = DirectX;

	auto model = Arrow::Make(1.0f);
	model.Transform(dx::XMMatrixScaling(size, size, size));
	const auto geometryTag = "$arrow." + std::to_string(size);
	AddBind(VertexBuffer::Resolve(gfx, geometryTag, model.vertices));
	AddBind(IndexBuffer::Resolve(gfx, geometryTag, model.indices));

	auto pvs = VertexShader::Resolve(gfx, "SolidVS.cso");
	auto pvsbc = pvs->GetBytecode();
	AddBind(std::move(pvs));

	AddBind(PixelShader::Resolve(gfx, "SolidPS.cso"));

	AddBind(PixelConstantBuffer<PSMaterialConstant>::Resolve(gfx, pmc, 3u));

	AddBind(InputLayout::Resolve(gfx, model.vertices.GetLayout(), pvsbc));

	AddBind(Topology::Resolve(gfx, D3D11_PRIMITIVE_TOPOLOGY_LINELIST));

	AddBind(std::make_shared<TransformCbuf>(gfx, *this));
}

void SolidArrow::SetTransform(DirectX::XMFLOAT3 pos, float pitch, float yaw, float roll, float length) noexcept
{
	this->pos = pos;
	this->pitch = pitch;
	this->yaw = yaw;
	this->roll = roll;
	this->length = length;
}

DirectX::XMMATRIX SolidArrow::GetTransformXM() const noexcept
{
	return DirectX::XMMatrixScaling(1.0f, length, 1.0f) *
		DirectX::XMMatrixRotationRollPitchYaw(pitch, yaw, roll) *
		DirectX::XMMatrixTranslation(pos.x, pos.y, pos.z);
}

void SolidArrow::ChangeArrowColor(Graphics& gfx) noexcept
{
	QueryBindable<Bind::PixelConstantBuffer<PSMaterialConstant>>()->Update(gfx, pmc);
}

