#pragma once
#include "Drawable.h"

class SkyBox : public Drawable
{
public:
	SkyBox(Graphics& gfx, float size);
	void SetPos(DirectX::XMFLOAT3 pos) noexcept;
	void SetRotation(float roll, float pitch, float yaw) noexcept;
	DirectX::XMMATRIX GetTransformXM() const noexcept override;
	void SpawnControlWindow(Graphics& gfx) noexcept;
	bool show = true;
private:
	struct PSMaterialConstant
	{
		DirectX::XMFLOAT3 color = { 1.0f,1.0f,1.0f };
		float padding;
	} pmc;
private:
	DirectX::XMFLOAT3 pos = { 0.0f,0.0f,0.0f };
	float roll = 0.0f;
	float pitch = 0.0f;
	float yaw = 0.0f;
	void Reset() noexcept;
};