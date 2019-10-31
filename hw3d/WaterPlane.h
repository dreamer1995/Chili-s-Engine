#pragma once
#include "Drawable.h"

class WaterPlane : public Drawable
{
public:
	WaterPlane(Graphics& gfx, float size);
	void SetPos(DirectX::XMFLOAT3 pos) noexcept;
	void SetRotation(float roll, float pitch, float yaw) noexcept;
	DirectX::XMMATRIX GetTransformXM() const noexcept override;
	void SpawnControlWindow(Graphics& gfx) noexcept;
	void ChangeSphereMaterialState(Graphics& gfx, float pitch, float yaw, float roll) noexcept;
	void Bind(Graphics& gfx, float deltaTime) noexcept;
public:
	struct VSMaterialConstant
	{
		float time;
		alignas(16) DirectX::XMFLOAT4 amplitude = { 0.071f,0.032f,0.048f,0.063f };
		alignas(16) DirectX::XMFLOAT4 speed = { 0.097f,0.258f,0.179f,0.219f };
		alignas(16) DirectX::XMFLOAT4 wavelength = { 0.887f,0.774f,0.790f,0.844f };
		alignas(16) DirectX::XMFLOAT4 omega = { 0.0f,0.0f,0.0f,0.0f };
		alignas(16) DirectX::XMFLOAT4 Q = { 1.0f,0.871f,0.935f,0.844f };
		alignas(16) DirectX::XMFLOAT4 directionX = { 0.0f,0.113f,0.306f,0.281f };
		alignas(16) DirectX::XMFLOAT4 directionZ = { 0.629f,0.081f,0.484f,0.156f };
		alignas(16) DirectX::XMFLOAT3 color = { 0.0f,0.384313f,0.580392f };
		alignas(16) DirectX::XMFLOAT3 attenuation = { 5.0f,5.0f,5.0f };
		alignas(16) DirectX::XMFLOAT3 scatteringKd = { 1.0f,1.0f,1.0f };
		float depth = 1.0f;
		float padding[2];
	} vmc;
	struct PSMaterialConstant
	{
		/*float specularIntensity = 0.18f;
		float specularPower = 18.0f;
		BOOL normalMappingEnabled = TRUE;
		float padding[1];*/
		float metallic = 0.572f;
		float roughness = 0.321f;
		BOOL normalMappingEnabled = TRUE;
		DirectX::XMMATRIX EVRotation = DirectX::XMMatrixIdentity();
		float time = 0.0f;
		float speed = 0.25f;
		float depth = 2.471;
		float tilling = 1.0f;
		float flatten1 = 0.182f;
		float flatten2 = 0.0f;
		float padding[3];
	} pmc;
private:
	DirectX::XMFLOAT3 pos = { 0.0f,0.0f,0.0f };
	float roll = 0.0f;
	float pitch = 0.0f;
	float yaw = 0.0f;
};