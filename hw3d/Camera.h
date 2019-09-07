#pragma once
#include "Graphics.h"

class Camera
{
public:
	Camera() noexcept;
	DirectX::XMMATRIX GetMatrix() const noexcept;
	void SpawnControlWindow() noexcept;
	void Reset() noexcept;
	void Rotate( float dx,float dy ) noexcept;
	void Translate( DirectX::XMFLOAT3 translation ) noexcept;
	void LookZero(DirectX::XMFLOAT3 position) noexcept;
	void RotateAround(float dx, float dy, DirectX::XMFLOAT3 centralPoint) noexcept;
	DirectX::XMFLOAT3 pos;
private:
	float pitch;
	float yaw;
	static constexpr float travelSpeed = 12.0f;
	static constexpr float rotationSpeed = 0.004f;
	void KeepLookFront(DirectX::XMFLOAT3 position) noexcept;
	float yaw_;
};