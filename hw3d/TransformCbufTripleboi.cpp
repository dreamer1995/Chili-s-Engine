#include "TransformCbufTripleboi.h"

namespace Bind
{
	TransformCbufTripleboi::TransformCbufTripleboi(Graphics& gfx, const Drawable& parent, UINT slotV, UINT slotP, UINT slotD)
		:
		TransformCbufDoubleboi(gfx, parent, slotV, slotP)
	{
		if (!pDcbuf)
		{
			pDcbuf = std::make_unique<DomainConstantBuffer<Transforms>>(gfx, slotD);
		}
	}

	void Bind::TransformCbufTripleboi::Bind(Graphics& gfx) noexcept
	{
		const auto tf = GetTransforms(gfx);
		TransformCbuf::UpdateBindImpl(gfx, tf);
		UpdateBindImpl(gfx, tf);
	}

	void TransformCbufTripleboi::UpdateBindImpl(Graphics& gfx, const Transforms& tf) noexcept
	{
		pDcbuf->Update(gfx, tf);
		pDcbuf->Bind(gfx);
	}

	std::unique_ptr<DomainConstantBuffer<TransformCbuf::Transforms>> TransformCbufTripleboi::pDcbuf;
}