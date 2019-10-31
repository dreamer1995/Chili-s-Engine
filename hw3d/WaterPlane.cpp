#include "WaterPlane.h"
#include "Plane.h"
#include "BindableCommon.h"
#include "TransformCbufDoubleboi.h"
#include "imgui/imgui.h"

WaterPlane::WaterPlane(Graphics& gfx, float size)
{
	using namespace Bind;
	namespace dx = DirectX;

	auto model = Plane::Make('W');
	model.Transform(dx::XMMatrixScaling(size, size, 1.0f));
	const auto geometryTag = "$waterplane." + std::to_string(size);
	AddBind(VertexBuffer::Resolve(gfx, geometryTag, model.vertices));
	AddBind(IndexBuffer::Resolve(gfx, geometryTag, model.indices));

	//AddBind( Texture::Resolve( gfx,"Images\\brickwall.jpg" ) );
	//AddBind( Texture::Resolve( gfx,"Images\\brickwall_normal.jpg",1u ) );

	//auto pvs = VertexShader::Resolve( gfx,"PhongVS.cso" );
	//auto pvsbc = pvs->GetBytecode();
	//AddBind( std::move( pvs ) );

	//AddBind( PixelShader::Resolve( gfx,"PhongPSNormalMapObject.cso" ) );

	//AddBind(Texture::Resolve(gfx, "Images\\jellybeans1.jpg", 0u, true));
	AddBind(Texture::Resolve(gfx, "Images\\T_MediumWaves_H.jpg"));
	AddBind(Texture::Resolve(gfx, "Images\\T_MediumWaves_N.jpg", 1u));
	AddBind(Texture::Resolve(gfx, "Images\\T_SmallWaves_N.jpg", 2u));
	AddBind(TexturePre::Resolve(gfx, 3u, gfx.GetShaderResourceView('C')));
	AddBind(Texture::Resolve(gfx, "Images\\DesertSand_albedo.jpg", 4u));
	AddBind(Texture::Resolve(gfx, "Images\\white.jpg", 5u));
	AddBind(TexturePre::Resolve(gfx, 6u, gfx.GetShaderResourceView('N')));
	AddBind(TexturePre::Resolve(gfx, 10u, gfx.GetShaderResourceView()));
	AddBind(TexturePre::Resolve(gfx, 11u, gfx.GetShaderResourceView('M')));
	AddBind(TexturePre::Resolve(gfx, 12u, gfx.GetShaderResourceView('L')));
	AddBind(Texture::Resolve(gfx, "Images\\white.jpg", 30u, false, true));

	auto pvs = VertexShader::Resolve(gfx, "FluidVS.cso");
	auto pvsbc = pvs->GetBytecode();
	AddBind(std::move(pvs));

	AddBind(PixelShader::Resolve(gfx, "FluidPS.cso"));

	AddBind(VertexConstantBuffer<VSMaterialConstant>::Resolve(gfx, vmc, 2u));

	AddBind(PixelConstantBuffer<PSMaterialConstant>::Resolve(gfx, pmc, 5u));

	AddBind(InputLayout::Resolve(gfx, model.vertices.GetLayout(), pvsbc));

	AddBind(Topology::Resolve(gfx, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST));

	AddBind(std::make_shared<TransformCbufDoubleboi>(gfx, *this, 0u, 4u));
}

void WaterPlane::SetPos(DirectX::XMFLOAT3 pos) noexcept
{
	this->pos = pos;
}

void WaterPlane::SetRotation(float roll, float pitch, float yaw) noexcept
{
	this->roll = roll;
	this->pitch = pitch;
	this->yaw = yaw;
}

DirectX::XMMATRIX WaterPlane::GetTransformXM() const noexcept
{
	return DirectX::XMMatrixRotationRollPitchYaw(pitch, yaw, roll) *
		DirectX::XMMatrixTranslation(pos.x, pos.y, pos.z);
}

void WaterPlane::SpawnControlWindow(Graphics& gfx) noexcept
{
	if (ImGui::Begin("WaterPlane"))
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
		//bool changed0 = ImGui::SliderFloat( "Spec. Int.",&pmc.specularIntensity,0.0f,1.0f );
		//bool changed1 = ImGui::SliderFloat( "Spec. Power",&pmc.specularPower,0.0f,100.0f );
		bool changed0 = ImGui::SliderFloat("Metallic", &pmc.metallic, 0.0f, 2.0f);
		bool changed1 = ImGui::SliderFloat("Roughness", &pmc.roughness, 0.0f, 10.0f);
		bool checkState = pmc.normalMappingEnabled == TRUE;
		bool changed2 = ImGui::Checkbox("Enable Normal Map", &checkState);
		pmc.normalMappingEnabled = checkState ? TRUE : FALSE;
		bool changed3 = ImGui::SliderFloat("Speed", &pmc.speed, 0.0f, 1.0f);
		bool changed4 = ImGui::SliderFloat("Depth", &pmc.depth, 0.0f, 4.0f);
		bool changed5 = ImGui::SliderFloat("Tilling", &pmc.tilling, 0.0f, 2.0f);
		bool changed6 = ImGui::SliderFloat("Flatten1", &pmc.flatten1, -1.0f, 1.0f);
		bool changed7 = ImGui::SliderFloat("Flatten2", &pmc.flatten2, -1.0f, 1.0f);
		bool changed8 = ImGui::ColorEdit3("Water Color", &vmc.color.x);
		bool changed9 = ImGui::DragFloat3("Attenuation", &vmc.attenuation.x);
		bool changed10 = ImGui::DragFloat3("ScatteringKd", &vmc.scatteringKd.x);
		bool changed11 = ImGui::SliderFloat4("Amplitude", &vmc.amplitude.x, 0.0f, 1.0f);
		bool changed12 = ImGui::SliderFloat4("Speed", &vmc.speed.x, 0.0f, 1.0f);
		bool changed13 = ImGui::SliderFloat4("Wavelength", &vmc.wavelength.x, 0.0f, 1.0f);
		bool changed14 = ImGui::SliderFloat4("Q", &vmc.Q.x, 0.0f, 1.0f);
		bool changed15 = ImGui::SliderFloat4("DirectionX", &vmc.directionX.x, -1.0f, 1.0f);
		bool changed16 = ImGui::SliderFloat4("DirectionZ", &vmc.directionZ.x, -1.0f, 1.0f);

		if (changed0 || changed1 || changed2 || changed3 || changed4 || changed5 || changed6 || changed7)
		{
			QueryBindable<Bind::PixelConstantBuffer<PSMaterialConstant>>()->Update(gfx, pmc);
		}
		if (changed4 || changed8 || changed9 || changed10 || changed11 || changed12 || changed13 || changed14 || changed15 || changed16)
		{
			vmc.depth = pmc.depth;
			QueryBindable<Bind::VertexConstantBuffer<VSMaterialConstant>>()->Update(gfx, vmc);
		}
	}
	ImGui::End();
}

void WaterPlane::ChangeSphereMaterialState(Graphics& gfx, float pitch, float yaw, float roll) noexcept
{
	pmc.EVRotation = DirectX::XMMatrixRotationRollPitchYaw(pitch, yaw, roll);
	QueryBindable<Bind::PixelConstantBuffer<PSMaterialConstant>>()->Update(gfx, pmc);
}

void WaterPlane::Bind(Graphics& gfx, float deltaTime) noexcept
{
	pmc.time += deltaTime;
	QueryBindable<Bind::PixelConstantBuffer<PSMaterialConstant>>()->Update(gfx, pmc);
	vmc.time += deltaTime;
	vmc.omega = { 2 * PI / vmc.wavelength.x,2 * PI / vmc.wavelength.y,2 * PI / vmc.wavelength.z,2 * PI / vmc.wavelength.w };
	QueryBindable<Bind::VertexConstantBuffer<VSMaterialConstant>>()->Update(gfx, vmc);
}