#pragma once
#include "Drawable.h"

class SolidArrow : public Drawable
{
public:
	SolidArrow(Graphics& gfx, float size);
	void SetTransform(DirectX::XMFLOAT3 pos, float pitch, float yaw, float roll, float length) noexcept;
	DirectX::XMMATRIX GetTransformXM() const noexcept override;
	void ChangeArrowColor(Graphics& gfx) noexcept;
public:
	struct PSMaterialConstant
	{
		DirectX::XMFLOAT3 color = { 1.0f,0.0f,0.0f };
		float padding;
	} pmc;
private:
	DirectX::XMFLOAT3 pos;
	float pitch;
	float yaw;
	float roll;
	float length;
};