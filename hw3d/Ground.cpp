#include "Ground.h"
#include "Plane.h"
#include "BindableCommon.h"
#include "TransformCbuf.h"
#include "imgui/imgui.h"

Ground::Ground(Graphics& gfx, float size)
{
	using namespace Bind;
	namespace dx = DirectX;

	auto model = Plane::Make();
	model.Transform(dx::XMMatrixScaling(size, size, 1.0f));
	const auto geometryTag = "$ground." + std::to_string(size);
	AddBind(VertexBuffer::Resolve(gfx, geometryTag, model.vertices));
	AddBind(IndexBuffer::Resolve(gfx, geometryTag, model.indices));

	auto pvs = VertexShader::Resolve(gfx, "LambertBlinnPhongVS.cso");
	auto pvsbc = pvs->GetBytecode();
	AddBind(std::move(pvs));

	AddBind(PixelShader::Resolve(gfx, "LambertBlinnPhongPS.cso"));

	AddBind(PixelConstantBuffer<PSMaterialConstant>::Resolve(gfx, pmc, 5u));

	AddBind(InputLayout::Resolve(gfx, model.vertices.GetLayout(), pvsbc));

	AddBind(Topology::Resolve(gfx, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST));

	AddBind(std::make_shared<TransformCbuf>(gfx, *this, 0u));
}

void Ground::SetPos(DirectX::XMFLOAT3 pos) noexcept
{
	this->pos = pos;
}

void Ground::SetRotation(float roll, float pitch, float yaw) noexcept
{
	this->roll = roll;
	this->pitch = pitch;
	this->yaw = yaw;
}

DirectX::XMMATRIX Ground::GetTransformXM() const noexcept
{
	return DirectX::XMMatrixRotationRollPitchYaw(pitch, yaw, roll) *
		DirectX::XMMatrixTranslation(pos.x, pos.y, pos.z);
}

void Ground::SpawnControlWindow(Graphics& gfx) noexcept
{
	if (ImGui::Begin("Ground"))
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
		bool changed0 = ImGui::SliderFloat( "Spec. Int.",&pmc.specularIntensity,0.0f,1.0f );
		bool changed1 = ImGui::SliderFloat( "Spec. Power",&pmc.specularPower,0.0f,100.0f );
		bool changed2 = ImGui::ColorEdit3("Water Color", &pmc.color.x);
		bool checkState = pmc.normalMappingEnabled == TRUE;
		bool changed3 = ImGui::Checkbox("Enable Normal Map", &checkState);
		pmc.normalMappingEnabled = checkState ? TRUE : FALSE;
		checkState = pmc.basecolorMappingEnabled == TRUE;
		bool changed4 = ImGui::Checkbox("Enable BaseColor Map", &checkState);
		pmc.basecolorMappingEnabled = checkState ? TRUE : FALSE;

		if (changed0 || changed1 || changed2 || changed3 || changed4)
		{
			QueryBindable<Bind::PixelConstantBuffer<PSMaterialConstant>>()->Update(gfx, pmc);
		}
	}
	ImGui::End();
}