#include "UVPannel.h"
#include "Sphere.h"
#include "BindableCommon.h"
#include "TransformCbufDoubleboi.h"
#include "imgui/imgui.h"
#include "Vertex.h"

UVPannel::UVPannel(Graphics& gfx, short int num, Dvtx::VertexBuffer vertices, std::vector<unsigned short> indices)
{
	using namespace Bind;
	namespace dx = DirectX;

	const auto geometryTag = "$uvpannel." + std::to_string(num);
	AddBind(VertexBuffer::Resolve(gfx, geometryTag, vertices));
	AddBind(IndexBuffer::Resolve(gfx, geometryTag, indices));

	auto pvs = VertexShader::Resolve(gfx, "UVPannelVS.cso");
	auto pvsbc = pvs->GetBytecode();
	AddBind(std::move(pvs));

	AddBind(PixelShader::Resolve(gfx, "UVPannelPS.cso"));

	AddBind(InputLayout::Resolve(gfx, vertices.GetLayout(), pvsbc));

	AddBind(Topology::Resolve(gfx, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST));
}

DirectX::XMMATRIX UVPannel::GetTransformXM() const noexcept
{
	return DirectX::XMMatrixIdentity();
}

//void TestSphere::SpawnControlWindow(Graphics& gfx) noexcept
//{
//	if (ImGui::Begin("Sphere"))
//	{
//		ImGui::Text("Position");
//		ImGui::SliderFloat("X", &pos.x, -80.0f, 80.0f, "%.1f");
//		ImGui::SliderFloat("Y", &pos.y, -80.0f, 80.0f, "%.1f");
//		ImGui::SliderFloat("Z", &pos.z, -80.0f, 80.0f, "%.1f");
//		ImGui::Text("Orientation");
//		ImGui::SliderAngle("Pitch", &pitch, -180.0f, 180.0f);
//		ImGui::SliderAngle("Yaw", &yaw, -180.0f, 180.0f);
//		ImGui::SliderAngle("Roll", &roll, -180.0f, 180.0f);
//		ImGui::Text("Shading");
//		bool changed0 = ImGui::SliderFloat("Metallic", &pmc.metallic, 0.0f, 1.0f);
//		bool changed1 = ImGui::SliderFloat("Roughness", &pmc.roughness, 0.01f, 0.99f);
//		bool checkState = pmc.normalMappingEnabled == TRUE;
//		bool changed2 = ImGui::Checkbox("Enable Normal Map", &checkState);
//		pmc.normalMappingEnabled = checkState ? TRUE : FALSE;
//		if (changed0 || changed1 || changed2)
//		{
//			QueryBindable<Bind::PixelConstantBuffer<PSMaterialConstant>>()->Update(gfx, pmc);
//		}
//	}
//	ImGui::End();
//}
