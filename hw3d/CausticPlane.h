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
	void Bind(Graphics& gfx, DirectX::XMFLOAT2 offset) noexcept;
	void Bind(Graphics& gfx, float speed, float depth, float roughness, float flatten1, float flatten2) noexcept;
private:
	struct HSMaterialConstant
	{
		int tessellation = 6;
		float padding[3];
	} hmc;
	struct DSMaterialConstant
	{
		float depth = 1.0f;
		float time = 0.0f;
		DirectX::XMFLOAT2 offset = { 0.0f,0.0f };
		float speed = 0.5f;
		float roughness = 1.0f;
		float flatten1 = 0.0f;
		float flatten2 = 0.0f;
	} dmc;
	struct PSMaterialConstant
	{
		DirectX::XMFLOAT3 color = { 0.2f,0.2f,0.2f };
		float padding[1];
	} pmc;
	DirectX::XMFLOAT3 pos = { 0.0f,0.0f,0.0f };
	float roll = 0.0f;
	float pitch = 0.0f;
	float yaw = 0.0f;
};