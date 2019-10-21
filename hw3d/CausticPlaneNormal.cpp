#include "CausticPlaneNormal.h"
#include "Plane.h"
#include "BindableCommon.h"
#include "TransformCbufDoubleboi.h"
#include "imgui/imgui.h"

CausticPlaneNormal::CausticPlaneNormal(Graphics& gfx, float size)
{
	using namespace Bind;
	namespace dx = DirectX;

	auto model = Plane::Make();
	model.Transform(dx::XMMatrixScaling(size, size, 1.0f));
	const auto geometryTag = "$causticnormal." + std::to_string(size);
	AddBind(VertexBuffer::Resolve(gfx, geometryTag, model.vertices));
	AddBind(IndexBuffer::Resolve(gfx, geometryTag, model.indices));

	AddBind(Texture::Resolve(gfx, "Images\\T_MediumWaves_H.jpg"));
	AddBind(Texture::Resolve(gfx, "Images\\T_MediumWaves_N.jpg", 1u));
	AddBind(Texture::Resolve(gfx, "Images\\T_SmallWaves_N.jpg", 2u));

	auto pvs = VertexShader::Resolve(gfx, "CausticBakeNVS.cso");
	auto pvsbc = pvs->GetBytecode();
	AddBind(std::move(pvs));

	AddBind(PixelShader::Resolve(gfx, "CausticBakeNPS.cso"));

	AddBind(VertexConstantBuffer<VSMaterialConstant>::Resolve(gfx, vmc, 2u));

	AddBind(PixelConstantBuffer<PSMaterialConstant>::Resolve(gfx, pmc, 5u));

	AddBind(InputLayout::Resolve(gfx, model.vertices.GetLayout(), pvsbc));

	AddBind(Topology::Resolve(gfx, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST));

	AddBind(std::make_shared<TransformCbufDoubleboi>(gfx, *this, 0u, 4u));
}

DirectX::XMMATRIX CausticPlaneNormal::GetTransformXM() const noexcept
{
	return DirectX::XMMatrixRotationRollPitchYaw(PI * 0.5f, 0.0f, 0.0f);
}

void CausticPlaneNormal::Bind(Graphics& gfx, float deltaTime) noexcept
{
	pmc.time += deltaTime;
	QueryBindable<Bind::PixelConstantBuffer<PSMaterialConstant>>()->Update(gfx, pmc);
	vmc.time += deltaTime;
	QueryBindable<Bind::VertexConstantBuffer<VSMaterialConstant>>()->Update(gfx, vmc);
}

void CausticPlaneNormal::Bind(Graphics& gfx, float speed, float roughness, float flatten1, float flatten2,
	DirectX::XMFLOAT4 amplitude, DirectX::XMFLOAT4 _speed, DirectX::XMFLOAT4 wavelength, DirectX::XMFLOAT4 omega,
	DirectX::XMFLOAT4 Q, DirectX::XMFLOAT4 directionX, DirectX::XMFLOAT4 directionZ, BOOL normalMappingEnabled) noexcept
{
	vmc.amplitude = amplitude;
	vmc._speed = _speed;
	vmc.wavelength = wavelength;
	vmc.omega = omega;
	vmc.Q = Q;
	vmc.directionX = directionX;
	vmc.directionZ = directionZ;
	QueryBindable<Bind::VertexConstantBuffer<VSMaterialConstant>>()->Update(gfx, vmc);
	pmc.speed = speed;
	pmc.roughness = roughness;
	pmc.flatten1 = flatten1;
	pmc.flatten2 = flatten2;
	pmc.normalMappingEnabled = normalMappingEnabled;
	QueryBindable<Bind::PixelConstantBuffer<PSMaterialConstant>>()->Update(gfx, pmc);
}
