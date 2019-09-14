#include "PhongSphere.h"
#include "BindableCommon.h"
#include "GraphicsThrowMacros.h"
#include "Vertex.h"
#include "Sphere.h"
#include "TransformCbufDoubleboi.h"

PhongSphere::PhongSphere(Graphics& gfx, float radius)
{
	using namespace Bind;
	namespace dx = DirectX;
	using Dvtx::VertexLayout;
	VertexLayout vl;
	vl.Append(VertexLayout::Position3D);
	vl.Append(VertexLayout::Normal);
	auto model = Sphere::Make(vl, true);
	model.Transform(dx::XMMatrixScaling(radius, radius, radius));
	const auto geometryTag = "$phongsphere." + std::to_string(radius);
	AddBind(VertexBuffer::Resolve(gfx, geometryTag, model.vertices));
	AddBind(IndexBuffer::Resolve(gfx, geometryTag, model.indices));

	auto pvs = VertexShader::Resolve(gfx, "PhongVSDirectional.cso");
	auto pvsbc = pvs->GetBytecode();
	AddBind(std::move(pvs));

	AddBind(PixelShader::Resolve(gfx, "PhongPSDirectional.cso"));

	AddBind(PixelConstantBuffer<PSMaterialConstant>::Resolve(gfx, pmc, 3u));

	AddBind(InputLayout::Resolve(gfx, model.vertices.GetLayout(), pvsbc));

	AddBind(Topology::Resolve(gfx, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST));

	AddBind(std::make_shared<TransformCbufDoubleboi>(gfx, *this, 0u, 4u));
}

void PhongSphere::SetPos(DirectX::XMFLOAT3 pos) noexcept
{
	this->pos = pos;
}

DirectX::XMMATRIX PhongSphere::GetTransformXM() const noexcept
{
	return DirectX::XMMatrixTranslation(pos.x, pos.y, pos.z);
}

void PhongSphere::ChangeSphereMaterialState(Graphics& gfx) noexcept
{
	QueryBindable<Bind::PixelConstantBuffer<PSMaterialConstant>>()->Update(gfx, pmc);
}