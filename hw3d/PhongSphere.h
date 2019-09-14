#pragma once
#include "Drawable.h"

class PhongSphere : public Drawable
{
public:
	PhongSphere(Graphics& gfx, float radius);
	void SetPos(DirectX::XMFLOAT3 pos) noexcept;
	DirectX::XMMATRIX GetTransformXM() const noexcept override;
	void ChangeSphereMaterialState(Graphics& gfx) noexcept;
public:
	struct PSMaterialConstant
	{
		alignas(16) DirectX::XMFLOAT3 color = { 1.0f,1.0f,1.0f };
		float specularIntensity = 0.3f;
		float specularPower = 30.0f;
		float padding[2];
	} pmc;
private:
	DirectX::XMFLOAT3 pos = { 1.0f,1.0f,1.0f };
};