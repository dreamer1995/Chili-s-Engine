#pragma once
#include "Drawable.h"

class UVPannel : public Drawable
{
public:
	UVPannel(Graphics& gfx, short int num);
	DirectX::XMMATRIX GetTransformXM() const noexcept override;
	//void SpawnControlWindow(Graphics& gfx) noexcept;
};