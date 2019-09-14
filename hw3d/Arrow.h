#pragma once

#include <optional>
#include "Vertex.h"
#include "IndexedTriangleList.h"
#include <DirectXMath.h>
#include "ChiliMath.h"
#include <array>

class Arrow
{
public:
	//template<class V>
	static IndexedLineList MakeDynamicArrow(Dvtx::VertexLayout layout, float length)
	{
		namespace dx = DirectX;
		float expansion = 0.2;
		Dvtx::VertexBuffer vb{ std::move(layout) };
		vb.EmplaceBack(dx::XMFLOAT3{ 0.0f,0.0f, 0.0f });
		vb.EmplaceBack(dx::XMFLOAT3{ 0.0f,-length, 0.0f });
		vb.EmplaceBack(dx::XMFLOAT3{ 0.0f,-length + expansion, 0.0f - expansion });
		vb.EmplaceBack(dx::XMFLOAT3{ 0.0f + expansion,-length + expansion, 0.0f });
		vb.EmplaceBack(dx::XMFLOAT3{ 0.0f,-length + expansion, 0.0f + expansion });
		vb.EmplaceBack(dx::XMFLOAT3{ 0.0f - expansion,-length + expansion, 0.0f });

		return{
			std::move(vb),{
				0,1,
				1,2,   1,3,
				1,4,   1,5
			}
		};
	}

	static IndexedLineList Make(float length)
	{
		using Dvtx::VertexLayout;
		VertexLayout vl;
		vl.Append(VertexLayout::Position3D);

		return MakeDynamicArrow(std::move(vl), length);
	}
};