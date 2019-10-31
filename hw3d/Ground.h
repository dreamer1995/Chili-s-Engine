#pragma once
#include "Drawable.h"

class Ground : public Drawable
{
public:
	Ground(Graphics& gfx, float size);
	void SetPos(DirectX::XMFLOAT3 pos) noexcept;
	void SetRotation(float roll, float pitch, float yaw) noexcept;
	DirectX::XMMATRIX GetTransformXM() const noexcept override;
	void SpawnControlWindow(Graphics& gfx) noexcept;
private:
	struct PSMaterialConstant
	{
		float specularIntensity = 0.0f;
		float specularPower = 18.0f;
		alignas(16) DirectX::XMFLOAT3 color = { 0.75f,0.75f,0.75f };
		BOOL normalMappingEnabled = FALSE;
		BOOL basecolorMappingEnabled = FALSE;
		//float padding[2];
	} pmc;
private:
	DirectX::XMFLOAT3 pos = { 0.0f,-4.0f,0.0f };
	float roll = 0.0f;
	float pitch = 0.0f;
	float yaw = 0.0f;
};