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
	void Bind(Graphics& gfx, float depth,
		DirectX::XMFLOAT4 amplitude, DirectX::XMFLOAT4 _speed, DirectX::XMFLOAT4 wavelength, DirectX::XMFLOAT4 omega,
		DirectX::XMFLOAT4 Q, DirectX::XMFLOAT4 directionX, DirectX::XMFLOAT4 directionZ) noexcept;
private:
	struct HSMaterialConstant
	{
		int tessellation = 6;
		float padding[3];
	} hmc;
	struct DSMaterialConstant
	{
		alignas(16) DirectX::XMFLOAT4 amplitude = { 0.071f,0.032f,0.048f,0.063f };
		alignas(16) DirectX::XMFLOAT4 _speed = { 0.097f,0.258f,0.179f,0.219f };
		alignas(16) DirectX::XMFLOAT4 wavelength = { 0.887f,0.774f,0.790f,0.844f };
		alignas(16) DirectX::XMFLOAT4 omega = { 0.0f,0.0f,0.0f,0.0f };
		alignas(16) DirectX::XMFLOAT4 Q = { 1.0f,0.871f,0.935f,0.844f };
		alignas(16) DirectX::XMFLOAT4 directionX = { 0.0f,0.113f,0.306f,0.281f };
		alignas(16) DirectX::XMFLOAT4 directionZ = { 0.629f,0.081f,0.484f,0.156f };
		float depth = 1.0f;
		float time = 0.0f;
		float padding[2];
	} dmc;
	struct PSMaterialConstant
	{
		DirectX::XMFLOAT3 color = { 0.258823f,0.156862f,0.062745f };
		float padding[1];
	} pmc;
	DirectX::XMFLOAT3 pos = { 0.0f,0.0f,0.0f };
	float roll = 0.0f;
	float pitch = 0.0f;
	float yaw = 0.0f;
};