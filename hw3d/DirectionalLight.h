#pragma once
#include "Graphics.h"
#include "PhongSphere.h"
#include "SolidArrow.h"
#include "ConstantBuffers.h"
#include "ConditionalNoexcept.h"

class DirectionalLight
{
public:
	DirectionalLight(Graphics& gfx, float radius = 0.5f, float size = 1.0f);
	void SpawnControlWindow(Graphics& gfx) noexcept;
	void Reset() noexcept;
	void Draw(Graphics& gfx) const noxnd;
	void Bind(Graphics& gfx) const noexcept;
	DirectX::XMFLOAT3 GetDirection() noexcept;
	void Rotate(float dx, float dy) noexcept;
private:
	struct DirectionalLightCBuf
	{
		alignas(16) DirectX::XMFLOAT3 direction;
		alignas(16) DirectX::XMFLOAT3 diffuseColor;
		float diffuseIntensity;
		float padding[3];
	};
private:
	float length = 1.0f;
	DirectionalLightCBuf cbData;
	mutable PhongSphere sphere;
	mutable SolidArrow arrow;
	mutable Bind::PixelConstantBuffer<DirectionalLightCBuf> cbuf;
	DirectX::XMFLOAT3 pos = { 6.0f,18.0f,-2.0f };
	float pitch = 0.0f;
	float yaw = 0.0f;
	float roll = 0.0f;
	static constexpr float rotationSpeed = 0.004f;
};