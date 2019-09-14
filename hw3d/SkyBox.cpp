#include "SkyBox.h"
#include "Cube.h"
#include "BindableCommon.h"
#include "imgui/imgui.h"

SkyBox::SkyBox(Graphics& gfx, float size)
{
	using namespace Bind;
	namespace dx = DirectX;

	auto model = Cube::Make();
	model.Transform(dx::XMMatrixScaling(size, size, size));
	const auto geometryTag = "$skybox." + std::to_string(size);
	AddBind(VertexBuffer::Resolve(gfx, geometryTag, model.vertices));
	AddBind(IndexBuffer::Resolve(gfx, geometryTag, model.indices));

	AddBind(Texture::Resolve(gfx, "Images\\CubeMap.jpg", 0u, true));

	auto pvs = VertexShader::Resolve(gfx, "SkyBoxVS.cso");
	auto pvsbc = pvs->GetBytecode();
	AddBind(std::move(pvs));

	AddBind(PixelShader::Resolve(gfx, "SkyBoxPS.cso"));

	AddBind(PixelConstantBuffer<PSMaterialConstant>::Resolve(gfx, pmc, 3u));

	AddBind(InputLayout::Resolve(gfx, model.vertices.GetLayout(), pvsbc));

	AddBind(Topology::Resolve(gfx, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST));

	AddBind(std::make_shared<TransformCbuf>(gfx, *this));
}

void SkyBox::SetPos(DirectX::XMFLOAT3 pos) noexcept
{
	this->pos = pos;
}

void SkyBox::SetRotation(float roll, float pitch, float yaw) noexcept
{
	this->pitch = pitch;
	this->yaw = yaw;
	this->roll = roll;
}

DirectX::XMMATRIX SkyBox::GetTransformXM() const noexcept
{
	return DirectX::XMMatrixRotationRollPitchYaw(pitch, yaw, roll) *
		DirectX::XMMatrixTranslation(pos.x, pos.y, pos.z);
}

void SkyBox::Reset() noexcept
{
	pitch = 0.0f;
	yaw = 0.0f;
	roll = 0.0f;
}

void SkyBox::SpawnControlWindow(Graphics& gfx) noexcept
{
	if (ImGui::Begin("SkyBox"))
	{
		ImGui::Text("Orientation");
		ImGui::SliderAngle("Pitch", &pitch, -180.0f, 180.0f);
		ImGui::SliderAngle("Yaw", &yaw, -180.0f, 180.0f);
		ImGui::SliderAngle("Roll", &roll, -180.0f, 180.0f);
		ImGui::Text("Tint");
		bool changed0 = ImGui::ColorEdit3("Tint Color", &pmc.color.x);
		ImGui::Text("Visibility");
		ImGui::Checkbox("Show CubeMap", &show);
		if (changed0)
		{
			QueryBindable<Bind::PixelConstantBuffer<PSMaterialConstant>>()->Update(gfx, pmc);
		}
		if (ImGui::Button("Reset"))
		{
			Reset();
		}
	}
	ImGui::End();
}
