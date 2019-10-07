#pragma once
#include "Drawable.h"
#include "Vertex.h"

class UVPannel : public Drawable
{
public:
	UVPannel(Graphics& gfx, short int num, Dvtx::VertexBuffer vertices, std::vector<unsigned short> indices);
	DirectX::XMMATRIX GetTransformXM() const noexcept override;
	//void SpawnControlWindow(Graphics& gfx) noexcept;
	bool showUV = true;
};