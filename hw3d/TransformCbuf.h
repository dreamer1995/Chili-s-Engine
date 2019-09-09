#pragma once
#include "ConstantBuffers.h"
#include "Drawable.h"
#include <DirectXMath.h>

namespace Bind
{
	class TransformCbuf : public Bindable
	{
	protected:
		struct Transforms
		{
			DirectX::XMMATRIX matrix_MVP;
			DirectX::XMMATRIX matrix_MV;
			DirectX::XMMATRIX matrix_V;
			DirectX::XMMATRIX matrix_P;
			DirectX::XMMATRIX matrix_VP;
			DirectX::XMMATRIX matrix_T_MV;
			DirectX::XMMATRIX matrix_IT_MV;
			DirectX::XMMATRIX matrix_M2W;
			DirectX::XMMATRIX matrix_W2M;
		};
	public:
		TransformCbuf( Graphics& gfx,const Drawable& parent,UINT slot = 0u );
		void Bind( Graphics& gfx ) noexcept override;
	protected:
		void UpdateBindImpl( Graphics& gfx,const Transforms& tf ) noexcept;
		Transforms GetTransforms( Graphics& gfx ) noexcept;
	private:
		static std::unique_ptr<VertexConstantBuffer<Transforms>> pVcbuf;
		const Drawable& parent;
	};
}