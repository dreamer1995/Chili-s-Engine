#include "TransformCbuf.h"

namespace Bind
{
	TransformCbuf::TransformCbuf( Graphics& gfx,const Drawable& parent,UINT slot )
		:
		parent( parent )
	{
		if( !pVcbuf )
		{
			pVcbuf = std::make_unique<VertexConstantBuffer<Transforms>>( gfx,slot );
		}
	}

	void TransformCbuf::Bind( Graphics& gfx ) noexcept
	{
		UpdateBindImpl( gfx,GetTransforms( gfx ) );
	}

	void TransformCbuf::UpdateBindImpl( Graphics& gfx,const Transforms& tf ) noexcept
	{
		pVcbuf->Update( gfx,tf );
		pVcbuf->Bind( gfx );
	}

	TransformCbuf::Transforms TransformCbuf::GetTransforms( Graphics& gfx ) noexcept
	{
		const auto matrix_W2M = parent.GetTransformXM();
		const auto matrix_M2W = DirectX::XMMatrixTranspose(matrix_W2M);
		const auto matrix_V = DirectX::XMMatrixTranspose(gfx.GetCamera());
		const auto matrix_MV = matrix_V * matrix_M2W;
		const auto matrix_P = DirectX::XMMatrixTranspose(gfx.GetProjection());
		const auto matrix_VP = matrix_P * matrix_V;
		const auto matrix_MVP = matrix_P * matrix_MV;
		const auto matrix_T_MV = matrix_W2M * gfx.GetCamera();
		const auto matrix_IT_MV = DirectX::XMMatrixTranspose(DirectX::XMMatrixInverse(nullptr, matrix_T_MV));
		
		return {matrix_MVP,
				matrix_MV,
				matrix_V,
				matrix_P,
				matrix_VP,
				matrix_T_MV,
				matrix_IT_MV,
				matrix_M2W,
				matrix_W2M
			   };
	}

	std::unique_ptr<VertexConstantBuffer<TransformCbuf::Transforms>> TransformCbuf::pVcbuf;
}