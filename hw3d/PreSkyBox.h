#pragma once
#include "Drawable.h"

class PreSkyBox : public Drawable
{
public:
	PreSkyBox(Graphics& gfx, float size, char type = 'D');
	void SetRotation(float roll, float pitch, float yaw) noexcept;
	DirectX::XMMATRIX GetTransformXM() const noexcept override;

private:
	float roll = 0.0f;
	float pitch = 0.0f;
	float yaw = 0.0f;
	void Reset() noexcept;
};