#pragma once
#include "Drawable.h"

class CausticPlane : public Drawable
{
public:
	CausticPlane(Graphics& gfx, float size);
	void SetPos(DirectX::XMFLOAT3 pos) noexcept;
	void SetRotation(float roll, float pitch, float yaw) noexcept;
	DirectX::XMMATRIX GetTransformXM() const noexcept override;
	void SpawnControlWindow(Graphics& gfx) noexcept;
	void Bind(Graphics& gfx, float deltaTime) noexcept;
private:
	struct PSMaterialConstant
	{
		DirectX::XMFLOAT3 color = { 0.2f,0.2f,0.2f };
		float depth = 0.0f;
		float time = 0.0f;
		float padding[3];
	} pmc;
	DirectX::XMFLOAT3 pos = { 0.0f,0.0f,0.0f };
	float roll = 0.0f;
	float pitch = 0.0f;
	float yaw = 0.0f;
};