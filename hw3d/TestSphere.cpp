#include "TestSphere.h"
#include "Sphere.h"
#include "BindableCommon.h"
#include "TransformCbufDoubleboi.h"
#include "imgui/imgui.h"
#include "Vertex.h"

TestSphere::TestSphere(Graphics& gfx, float size)
{
	using namespace Bind;
	namespace dx = DirectX;

	using Dvtx::VertexLayout;
	VertexLayout vl;
	vl.Append(VertexLayout::Position3D);
	vl.Append(VertexLayout::Normal);
	vl.Append(VertexLayout::Tangent);
	vl.Append(VertexLayout::Binormal);
	vl.Append(VertexLayout::Texture2D);
	auto model = Sphere::MakeNormalUVed(vl, true);
	model.Transform(dx::XMMatrixScaling(size, size, size));
	//model.SetTBNIndependentFlat();
	const auto geometryTag = "$sphere." + std::to_string(size);
	AddBind(VertexBuffer::Resolve(gfx, geometryTag, model.vertices));
	AddBind(IndexBuffer::Resolve(gfx, geometryTag, model.indices));

	//AddBind(Texture::Resolve(gfx, "Images\\jellybeans1.jpg", 0u, true));
	//AddBind(Texture::Resolve(gfx, "Images\\rustediron2_basecolor.png"));
	//AddBind(Texture::Resolve(gfx, "Images\\rustediron2_normal.png", 1u));
	//AddBind(Texture::Resolve(gfx, "Images\\rustediron2_RMA.png", 2u));
	AddBind(TexturePre::Resolve(gfx, 3u, gfx.GetShaderResourceView()));
	AddBind(TexturePre::Resolve(gfx, 4u, gfx.GetShaderResourceView('M')));
	AddBind(TexturePre::Resolve(gfx, 5u, gfx.GetShaderResourceView('L')));

	auto pvs = VertexShader::Resolve(gfx, "PBRTestVS.cso");
	auto pvsbc = pvs->GetBytecode();
	AddBind(std::move(pvs));

	AddBind(PixelShader::Resolve(gfx, "PBRTestPS.cso"));

	AddBind(PixelConstantBuffer<PSMaterialConstant>::Resolve(gfx, pmc, 3u));

	AddBind(InputLayout::Resolve(gfx, model.vertices.GetLayout(), pvsbc));

	AddBind(Topology::Resolve(gfx, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST));

	AddBind(std::make_shared<TransformCbufDoubleboi>(gfx, *this, 0u, 4u));
}

void TestSphere::SetPos(DirectX::XMFLOAT3 pos) noexcept
{
	this->pos = pos;
}

void TestSphere::SetRotation(float roll, float pitch, float yaw) noexcept
{
	this->roll = roll;
	this->pitch = pitch;
	this->yaw = yaw;
}

DirectX::XMMATRIX TestSphere::GetTransformXM() const noexcept
{
	return DirectX::XMMatrixRotationRollPitchYaw(pitch, yaw, roll) *
		DirectX::XMMatrixTranslation(pos.x, pos.y, pos.z);
}

void TestSphere::SpawnControlWindow(Graphics& gfx) noexcept
{
	if (ImGui::Begin("Sphere"))
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
		bool changed0 = ImGui::SliderFloat("Metallic", &pmc.metallic, 0.0f, 1.0f);
		bool changed1 = ImGui::SliderFloat("Roughness", &pmc.roughness, 0.0f, 1.0f);
		bool checkState = pmc.normalMappingEnabled == TRUE;
		bool changed2 = ImGui::Checkbox("Enable Normal Map", &checkState);
		pmc.normalMappingEnabled = checkState ? TRUE : FALSE;
		if (changed0 || changed1 || changed2)
		{
			QueryBindable<Bind::PixelConstantBuffer<PSMaterialConstant>>()->Update(gfx, pmc);
		}
	}
	ImGui::End();
}

void TestSphere::ChangeSphereMaterialState(Graphics& gfx, float pitch, float yaw, float roll) noexcept
{
	pmc.EVRotation = DirectX::XMMatrixRotationRollPitchYaw(pitch, yaw, roll);
	QueryBindable<Bind::PixelConstantBuffer<PSMaterialConstant>>()->Update(gfx, pmc);
}