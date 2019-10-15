#include "CausticPlane.h"
#include "Plane.h"
#include "BindableCommon.h"
#include "TransformCbufDoubleboi.h"
#include "imgui/imgui.h"

CausticPlane::CausticPlane(Graphics& gfx, float size)
{
	using namespace Bind;
	namespace dx = DirectX;

	auto model = Plane::Make('T');
	model.Transform(dx::XMMatrixScaling(size, size, 1.0f));
	const auto geometryTag = "$causticplane." + std::to_string(size);
	AddBind(VertexBuffer::Resolve(gfx, geometryTag, model.vertices));
	AddBind(IndexBuffer::Resolve(gfx, geometryTag, model.indices));

	AddBind(Texture::Resolve(gfx, "Images\\T_MediumWaves_N.jpg", 20u, false, true));
	AddBind(Texture::Resolve(gfx, "Images\\T_SmallWaves_N.jpg", 21u, false, true));

	auto pvs = VertexShader::Resolve(gfx, "CausticVS.cso");
	auto pvsbc = pvs->GetBytecode();
	AddBind(std::move(pvs));

	AddBind(PixelShader::Resolve(gfx, "CausticPS.cso"));

	AddBind(VertexConstantBuffer<PSMaterialConstant>::Resolve(gfx, pmc, 2u));

	AddBind(PixelConstantBuffer<PSMaterialConstant>::Resolve(gfx, pmc, 3u));

	AddBind(InputLayout::Resolve(gfx, model.vertices.GetLayout(), pvsbc));

	AddBind(Topology::Resolve(gfx, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST));

	AddBind(std::make_shared<TransformCbuf>(gfx, *this));
}

void CausticPlane::SetPos(DirectX::XMFLOAT3 pos) noexcept
{
	this->pos = pos;
}

void CausticPlane::SetRotation(float roll, float pitch, float yaw) noexcept
{
	this->roll = roll;
	this->pitch = pitch;
	this->yaw = yaw;
}

DirectX::XMMATRIX CausticPlane::GetTransformXM() const noexcept
{
	return DirectX::XMMatrixRotationRollPitchYaw(pitch, yaw, roll) *
		DirectX::XMMatrixTranslation(pos.x, pos.y, pos.z);
}

void CausticPlane::SpawnControlWindow(Graphics& gfx) noexcept
{
	if (ImGui::Begin("CausticPlane"))
	{
		ImGui::Text("Position");
		ImGui::SliderFloat("X", &pos.x, -80.0f, 80.0f, "%.1f");
		ImGui::SliderFloat("Y", &pos.y, -80.0f, 80.0f, "%.1f");
		ImGui::SliderFloat("Z", &pos.z, -80.0f, 80.0f, "%.1f");
		ImGui::Text("Orientation");
		ImGui::SliderAngle("Pitch", &pitch, -180.0f, 180.0f);
		ImGui::SliderAngle("Yaw", &yaw, -180.0f, 180.0f);
		ImGui::SliderAngle("Roll", &roll, -180.0f, 180.0f);
		ImGui::Text("Shading");
		bool changed0 = ImGui::ColorEdit3("LightIntensity", &pmc.color.x);
		bool changed1 = ImGui::SliderFloat("Depth", &pmc.depth, 0.0f, 10.0f);
		//bool changed1 = ImGui::SliderFloat("Roughness", &pmc.roughness, 0.0f, 1.0f);
		//bool checkState = pmc.normalMappingEnabled == TRUE;
		//bool changed2 = ImGui::Checkbox("Enable Normal Map", &checkState);
		//pmc.normalMappingEnabled = checkState ? TRUE : FALSE;
		if (changed0)
		{
			QueryBindable<Bind::PixelConstantBuffer<PSMaterialConstant>>()->Update(gfx, pmc);
		}
		if (changed1)
		{
			QueryBindable<Bind::VertexConstantBuffer<PSMaterialConstant>>()->Update(gfx, pmc);
		}
	}
	ImGui::End();
}

void CausticPlane::Bind(Graphics& gfx, float deltaTime) noexcept
{
	pmc.time += deltaTime;
	QueryBindable<Bind::VertexConstantBuffer<PSMaterialConstant>>()->Update(gfx, pmc);
}

void CausticPlane::Bind(Graphics& gfx, DirectX::XMFLOAT2 offset) noexcept
{
	pmc.offset = offset;
	QueryBindable<Bind::VertexConstantBuffer<PSMaterialConstant>>()->Update(gfx, pmc);
}