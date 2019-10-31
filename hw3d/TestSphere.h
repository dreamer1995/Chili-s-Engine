#pragma once
#include "Drawable.h"

class TestSphere : public Drawable
{
public:
	TestSphere(Graphics& gfx, float size);
	void SetPos(DirectX::XMFLOAT3 pos) noexcept;
	void SetRotation(float roll, float pitch, float yaw) noexcept;
	DirectX::XMMATRIX GetTransformXM() const noexcept override;
	void SpawnControlWindow(Graphics& gfx) noexcept;
	void ChangeSphereMaterialState(Graphics& gfx, float pitch, float yaw, float roll) noexcept;
private:
	struct PSMaterialConstant
	{
		float metallic = 0.0f;
		float roughness = 0.045f;
		BOOL normalMappingEnabled = FALSE;
		DirectX::XMMATRIX EVRotation = DirectX::XMMatrixIdentity();
		float padding[1];
	} pmc;
	DirectX::XMFLOAT3 pos = { 0.0f,0.0f,0.0f };
	float roll = 0.0f;
	float pitch = 0.0f;
	float yaw = 0.0f;
};