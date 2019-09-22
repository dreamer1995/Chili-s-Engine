#pragma once
#include "Drawable.h"

class PreSkyBoxBlur : public Drawable
{
public:
	PreSkyBoxBlur(Graphics& gfx, float size);
	void SetRotation(float roll, float pitch, float yaw) noexcept;
	DirectX::XMMATRIX GetTransformXM() const noexcept override;

private:
	float roll = 0.0f;
	float pitch = 0.0f;
	float yaw = 0.0f;
	void Reset() noexcept;
};