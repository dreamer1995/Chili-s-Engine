#include "ShadowSphere.h"
#include "Sphere.h"
#include "BindableCommon.h"
#include "TransformCbufDoubleboi.h"
#include "imgui/imgui.h"
#include "Vertex.h"

ShadowSphere::ShadowSphere(Graphics& gfx, float size)
{
	using namespace Bind;
	namespace dx = DirectX;

	using Dvtx::VertexLayout;
	VertexLayout vl;
	vl.Append(VertexLayout::Position3D);
	auto model = Sphere::MakeNormalUVed(vl, true);
	model.Transform(dx::XMMatrixScaling(size, size, size));
	//model.SetTBNIndependentFlat();
	const auto geometryTag = "$sphere." + std::to_string(size);
	AddBind(VertexBuffer::Resolve(gfx, geometryTag, model.vertices));
	AddBind(IndexBuffer::Resolve(gfx, geometryTag, model.indices));

	auto pvs = VertexShader::Resolve(gfx, "PBRTestVS.cso");
	auto pvsbc = pvs->GetBytecode();
	AddBind(std::move(pvs));

	AddBind(PixelShader::Resolve(gfx, "PBRTestPS.cso"));

	AddBind(InputLayout::Resolve(gfx, model.vertices.GetLayout(), pvsbc));

	AddBind(Topology::Resolve(gfx, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST));

	AddBind(std::make_shared<TransformCbufDoubleboi>(gfx, *this, 0u, 4u));
}

void ShadowSphere::SetPos(DirectX::XMFLOAT3 pos) noexcept
{
	this->pos = pos;
}

void ShadowSphere::SetRotation(float roll, float pitch, float yaw) noexcept
{
	this->roll = roll;
	this->pitch = pitch;
	this->yaw = yaw;
}

DirectX::XMMATRIX ShadowSphere::GetTransformXM() const noexcept
{
	return DirectX::XMMatrixRotationRollPitchYaw(pitch, yaw, roll) *
		DirectX::XMMatrixTranslation(pos.x, pos.y, pos.z);
}
