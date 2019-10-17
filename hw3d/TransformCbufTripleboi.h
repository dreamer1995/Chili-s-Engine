#pragma once
#include "TransformCbufDoubleboi.h"

namespace Bind
{
	class TransformCbufTripleboi : public TransformCbufDoubleboi
	{
	public:
		TransformCbufTripleboi(Graphics& gfx, const Drawable& parent, UINT slotV = 0u, UINT slotP = 0u, UINT slotD = 0u);
		void Bind(Graphics& gfx) noexcept override;
	protected:
		void UpdateBindImpl(Graphics& gfx, const Transforms& tf) noexcept;
	private:
		static std::unique_ptr<DomainConstantBuffer<Transforms>> pDcbuf;
	};
}