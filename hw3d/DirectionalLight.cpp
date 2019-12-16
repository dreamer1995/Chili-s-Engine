#include "DirectionalLight.h"
#include "imgui/imgui.h"
#include "ChiliMath.h"

DirectionalLight::DirectionalLight(Graphics& gfx, float radius, float size)
	:
	sphere(gfx, radius),
	arrow(gfx, size),
	cbufVS(gfx, 1u),
	cbuf(gfx, 1u)
{
	Reset();
}

void DirectionalLight::SpawnControlWindow(Graphics& gfx) noexcept
{
	if (ImGui::Begin("DirectionalLight"))
	{
		ImGui::Text("Position");
		ImGui::SliderFloat("X", &pos.x, -60.0f, 60.0f, "%.1f");
		ImGui::SliderFloat("Y", &pos.y, -60.0f, 60.0f, "%.1f");
		ImGui::SliderFloat("Z", &pos.z, -60.0f, 60.0f, "%.1f");
		ImGui::Text("Orientation");
		ImGui::SliderAngle("Pitch", &pitch, -180.0f, 180.0f);
		ImGui::SliderAngle("Yaw", &yaw, -180.0f, 180.0f);
		ImGui::SliderAngle("Roll", &roll, -180.0f, 180.0f);
		ImGui::Text("Intensity/Color");
		ImGui::SliderFloat("Intensity", &cbData.diffuseIntensity, 0.01f, 100.0f, "%.1f", 2);
		ImGui::ColorEdit3("Diffuse Color", &cbData.diffuseColor.x);
		ImGui::Text("Arrow");
		ImGui::SliderFloat("Arrow Length", &length, 0.0f, 5.0, "%.1f");
		bool changed0 = ImGui::ColorEdit3("Arrow Color", &arrow.pmc.color.x);
		if (changed0)
		{
			arrow.ChangeArrowColor(gfx);
		}
		bool changed1 = ImGui::ColorEdit3("Sphere Color", &sphere.pmc.color.x);
		bool changed2 = ImGui::SliderFloat("Sphere Spec. Int.", &sphere.pmc.specularIntensity, 0.0f, 1.0f);
		bool changed3 = ImGui::SliderFloat("Sphere Spec. Power", &sphere.pmc.specularPower, 0.0f, 100.0f);
		if (changed1 || changed2 || changed3)
		{
			sphere.ChangeSphereMaterialState(gfx);
		}
		if (ImGui::Button("Reset"))
		{
			Reset();
		}
	}
	ImGui::End();
}

void DirectionalLight::Reset() noexcept
{
	cbData = {
		{ 0.0f,-1.0f,0.0f },
		{ 1.0f,1.0f,1.0f },
		1.0f,
	};
	pos = { 0.0f,10.0f,0.0f };
	pitch = -PI / 4;
	yaw = -PI / 4;
	//pitch = 62.0f * PI / 180.0f;
	//yaw = 38.0f * PI / 180.0f;
	roll = 0.0f;
}

void DirectionalLight::Draw(Graphics& gfx) const noxnd
{
	sphere.SetPos(pos);
	sphere.Draw(gfx);
	arrow.SetTransform(pos, pitch, yaw, roll, length);
	arrow.Draw(gfx);
}

void DirectionalLight::Bind(Graphics& gfx) const noexcept
{
	auto dataCopy = cbData;

	namespace dx = DirectX;
	using namespace dx;
	XMStoreFloat3(&dataCopy.direction,
		XMVector3Normalize(
		XMVectorNegate(
		XMVector3Transform(XMVectorSet(0.0f, -1.0f, 0.0f, 0.0f),
		XMMatrixRotationRollPitchYaw(pitch, yaw, roll)
	))));
	const auto lightPos = XMVectorSet(pos.x, pos.y, pos.z, 0.0f);

	/*const auto lightTarget = lightPos + XMVectorNegate(XMVectorSet(dataCopy.direction.x, dataCopy.direction.y, dataCopy.direction.z, 0.0f));
	dataCopy.lightSpaceVP = XMMatrixTranspose(projection) * XMMatrixTranspose(XMMatrixLookAtLH(lightPos, lightTarget, XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f)));*/

	cbufVS.Update(gfx, dataCopy);
	cbufVS.Bind(gfx);
	cbuf.Update(gfx, dataCopy);
	cbuf.Bind(gfx);
}

DirectX::XMFLOAT3 DirectionalLight::GetDirection() noexcept
{
	return cbData.direction;
}

void DirectionalLight::Rotate(float dx, float dy) noexcept
{
	yaw = wrap_angle(yaw + dx * rotationSpeed);
	pitch = wrap_angle(pitch + dy * rotationSpeed);
}