#pragma once
#include "Drawable.h"

class PrePlane : public Drawable
{
public:
	PrePlane(Graphics& gfx, float size);
	DirectX::XMMATRIX GetTransformXM() const noexcept override;
};