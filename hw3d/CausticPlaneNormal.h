#pragma once
#include "Drawable.h"

class CausticPlaneNormal : public Drawable
{
public:
	CausticPlaneNormal(Graphics& gfx, float size);
	DirectX::XMMATRIX GetTransformXM() const noexcept override;
	void Bind(Graphics& gfx, float deltaTime) noexcept;
	void Bind(Graphics& gfx, float speed, float roughness, float flatten1, float flatten2,
		DirectX::XMFLOAT4 amplitude, DirectX::XMFLOAT4 _speed, DirectX::XMFLOAT4 wavelength, DirectX::XMFLOAT4 omega,
		DirectX::XMFLOAT4 Q, DirectX::XMFLOAT4 directionX, DirectX::XMFLOAT4 directionZ, BOOL normalMappingEnabled) noexcept;
private:
	struct VSMaterialConstant
	{
		float time = 0.0f;
		alignas(16) DirectX::XMFLOAT4 amplitude = { 0.071f,0.032f,0.048f,0.063f };
		alignas(16) DirectX::XMFLOAT4 _speed = { 0.097f,0.258f,0.179f,0.219f };
		alignas(16) DirectX::XMFLOAT4 wavelength = { 0.887f,0.774f,0.790f,0.844f };
		alignas(16) DirectX::XMFLOAT4 omega = { 0.0f,0.0f,0.0f,0.0f };
		alignas(16) DirectX::XMFLOAT4 Q = { 1.0f,0.871f,0.935f,0.844f };
		alignas(16) DirectX::XMFLOAT4 directionX = { 0.0f,0.113f,0.306f,0.281f };
		alignas(16) DirectX::XMFLOAT4 directionZ = { 0.629f,0.081f,0.484f,0.156f };
		float padding[3];
	} vmc;
	struct PSMaterialConstant
	{
		float time = 0.0f;
		float speed = 0.25f;
		float roughness = 1.0f;
		float flatten1 = 0.0f;
		float flatten2 = 0.625f;
		BOOL normalMappingEnabled = TRUE;
		float padding[2];
	} pmc;
};