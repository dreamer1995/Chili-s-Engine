#pragma once
#include <optional>
#include "Vertex.h"
#include "IndexedTriangleList.h"
#include <DirectXMath.h>
#include "ChiliMath.h"

class Sphere
{
public:
	static IndexedTriangleList MakeTesselated(Dvtx::VertexLayout layout, int latDiv, int longDiv, bool withNormal)
	{
		namespace dx = DirectX;
		assert( latDiv >= 3 );
		assert( longDiv >= 3 );

		constexpr float radius = 1.0f;
		const auto base = dx::XMVectorSet( 0.0f,0.0f,radius,0.0f );
		const float lattitudeAngle = PI / latDiv;
		const float longitudeAngle = 2.0f * PI / longDiv;

		Dvtx::VertexBuffer vb{ std::move( layout ) };
		for( int iLat = 1; iLat < latDiv; iLat++ )
		{
			const auto latBase = dx::XMVector3Transform( 
				base,
				dx::XMMatrixRotationX( lattitudeAngle * iLat ) 
			);
			for( int iLong = 0; iLong < longDiv; iLong++ )
			{
				dx::XMFLOAT3 calculatedPos;
				auto v = dx::XMVector3Transform( 
					latBase,
					dx::XMMatrixRotationZ( longitudeAngle * iLong )
				);
				dx::XMStoreFloat3( &calculatedPos,v );
				if (withNormal)
				{
					dx::XMFLOAT3 calculatedNor;
					dx::XMStoreFloat3(&calculatedNor, dx::XMVector3Normalize(v));
					vb.EmplaceBack(calculatedPos, calculatedNor);
				}
				else
				{
					vb.EmplaceBack( calculatedPos );
				}
			}
		}

		// add the cap vertices
		const auto iNorthPole = (unsigned short)vb.Size();
		{
			dx::XMFLOAT3 northPos;
			dx::XMStoreFloat3( &northPos,base );
			if (withNormal)
			{
				dx::XMFLOAT3 calculatedNor;
				dx::XMStoreFloat3(&calculatedNor, dx::XMVector3Normalize(base));
				vb.EmplaceBack(northPos, calculatedNor);
			}
			else
			{
				vb.EmplaceBack( northPos );
			}
		}
		const auto iSouthPole = (unsigned short)vb.Size();
		{
			dx::XMFLOAT3 southPos;
			dx::XMStoreFloat3( &southPos,dx::XMVectorNegate( base ) );
			if (withNormal)
			{
				dx::XMFLOAT3 calculatedNor;
				dx::XMStoreFloat3(&calculatedNor, dx::XMVector3Normalize(dx::XMVectorNegate(base)));
				vb.EmplaceBack(southPos, calculatedNor);
			}
			else
			{
				vb.EmplaceBack( southPos );
			}
		}
		
		const auto calcIdx = [latDiv,longDiv]( unsigned short iLat,unsigned short iLong )
			{ return iLat * longDiv + iLong; };
		std::vector<unsigned short> indices;
		for( unsigned short iLat = 0; iLat < latDiv - 2; iLat++ )
		{
			for( unsigned short iLong = 0; iLong < longDiv - 1; iLong++ )
			{
				indices.push_back( calcIdx( iLat,iLong ) );
				indices.push_back( calcIdx( iLat + 1,iLong ) );
				indices.push_back( calcIdx( iLat,iLong + 1 ) );
				indices.push_back( calcIdx( iLat,iLong + 1 ) );
				indices.push_back( calcIdx( iLat + 1,iLong ) );
				indices.push_back( calcIdx( iLat + 1,iLong + 1 ) );
			}
			// wrap band
			indices.push_back( calcIdx( iLat,longDiv - 1 ) );
			indices.push_back( calcIdx( iLat + 1,longDiv - 1 ) );
			indices.push_back( calcIdx( iLat,0 ) );
			indices.push_back( calcIdx( iLat,0 ) );
			indices.push_back( calcIdx( iLat + 1,longDiv - 1 ) );
			indices.push_back( calcIdx( iLat + 1,0 ) );			
		}

		// cap fans
		for( unsigned short iLong = 0; iLong < longDiv - 1; iLong++ )
		{
			// north
			indices.push_back( iNorthPole );
			indices.push_back( calcIdx( 0,iLong ) );
			indices.push_back( calcIdx( 0,iLong + 1 ) );
			// south
			indices.push_back( calcIdx( latDiv - 2,iLong + 1 ) );
			indices.push_back( calcIdx( latDiv - 2,iLong ) );
			indices.push_back( iSouthPole );
		}
		// wrap triangles
		// north
		indices.push_back( iNorthPole );
		indices.push_back( calcIdx( 0,longDiv - 1 ) );
		indices.push_back( calcIdx( 0,0 ) );
		// south
		indices.push_back( calcIdx( latDiv - 2,0 ) );
		indices.push_back( calcIdx( latDiv - 2,longDiv - 1 ) );
		indices.push_back( iSouthPole );

		return { std::move( vb ),std::move( indices ) };
	}

	static IndexedTriangleList MakeTesselatedNormalUV(Dvtx::VertexLayout layout, int latDiv, int longDiv)
	{
		namespace dx = DirectX;
		assert( latDiv >= 3 );
		assert( longDiv >= 3 );

		constexpr float radius = 1.0f;
		const auto base = dx::XMVectorSet(0.0f, radius, 0.0f, 0.0f);
		const float lattitudeAngle = PI / latDiv;
		const float intervalV = 1.0f / latDiv;
		const float longitudeAngle = 2.0f * PI / longDiv;
		const float intervalU = 1.0f / longDiv;

		Dvtx::VertexBuffer vb{ std::move( layout ) };
		for( int iLat = 1; iLat < latDiv; iLat++ )
		{
			const auto latBase = dx::XMVector3Transform( 
				base,
				dx::XMMatrixRotationX( lattitudeAngle * iLat ) 
			);
			const float vSum = intervalV * iLat;
			DirectX::XMFLOAT3 _calculatedPos, _calculatedNor;
			for( int iLong = 0; iLong < longDiv; iLong++ )
			{
				auto v = dx::XMVector3Transform(
					latBase,
					dx::XMMatrixRotationY(longitudeAngle * iLong)
				);
				if (iLong != 0)
				{
					dx::XMFLOAT3 calculatedPos;
					dx::XMStoreFloat3(&calculatedPos, v);
					dx::XMFLOAT3 calculatedNor;
					dx::XMStoreFloat3(&calculatedNor, dx::XMVector3Normalize(v));
					dx::XMFLOAT2 calculatedUV = { intervalU * iLong,vSum };
					vb.EmplaceBack(calculatedPos, calculatedNor, calculatedUV);
				}
				else
				{
					dx::XMStoreFloat3(&_calculatedPos, v);
					dx::XMStoreFloat3(&_calculatedNor, dx::XMVector3Normalize(v));
					dx::XMFLOAT2 calculatedUV = { 0.0f,vSum };
					vb.EmplaceBack(_calculatedPos, _calculatedNor, calculatedUV);
				}
			}
			dx::XMFLOAT2 calculatedUV = { 1.0f,vSum };
			vb.EmplaceBack(_calculatedPos, _calculatedNor, calculatedUV);
		}

		// add the cap vertices
		const auto iNorthPole = (unsigned short)vb.Size();
		{
			dx::XMFLOAT3 northPos;
			dx::XMStoreFloat3( &northPos,base );
			dx::XMFLOAT3 calculatedNor;
			dx::XMStoreFloat3(&calculatedNor, dx::XMVector3Normalize(base));
			dx::XMFLOAT2 calculatedUV = { 0.5f,0.0f };
			vb.EmplaceBack(northPos, calculatedNor, calculatedUV);

		}
		const auto iSouthPole = (unsigned short)vb.Size();
		{
			dx::XMFLOAT3 southPos;
			dx::XMStoreFloat3( &southPos,dx::XMVectorNegate( base ) );
			dx::XMFLOAT3 calculatedNor;
			dx::XMStoreFloat3(&calculatedNor, dx::XMVector3Normalize(dx::XMVectorNegate(base)));
			dx::XMFLOAT2 calculatedUV = { 0.5f,1.0f };
			vb.EmplaceBack(southPos, calculatedNor, calculatedUV);
		}
		
		const auto calcIdx = [latDiv,longDiv]( unsigned short iLat,unsigned short iLong )
			{ return iLat * (longDiv + 1) + iLong; };
		std::vector<unsigned short> indices;
		for( unsigned short iLat = 0; iLat < latDiv - 2; iLat++ )
		{
			for( unsigned short iLong = 0; iLong < longDiv; iLong++ )
			{
				indices.push_back( calcIdx( iLat,iLong ) );
				indices.push_back( calcIdx( iLat + 1,iLong ) );
				indices.push_back( calcIdx( iLat,iLong + 1 ) );
				indices.push_back( calcIdx( iLat,iLong + 1 ) );
				indices.push_back( calcIdx( iLat + 1,iLong ) );
				indices.push_back( calcIdx( iLat + 1,iLong + 1 ) );
			}		
		}

		// cap fans
		for( unsigned short iLong = 0; iLong < longDiv; iLong++ )
		{
			// north
			indices.push_back( iNorthPole );
			indices.push_back( calcIdx( 0,iLong ) );
			indices.push_back( calcIdx( 0,iLong + 1 ) );
			// south
			indices.push_back( calcIdx( latDiv - 2,iLong + 1 ) );
			indices.push_back( calcIdx( latDiv - 2,iLong ) );
			indices.push_back( iSouthPole );
		}

		return { std::move( vb ),std::move( indices ) };
	}

	static IndexedTriangleList Make(std::optional<Dvtx::VertexLayout> layout = std::nullopt, bool withNormal = false)
	{
		using Element = Dvtx::VertexLayout::ElementType;
		if( !layout )
		{
			layout = Dvtx::VertexLayout{}.Append( Element::Position3D );
		}
		return MakeTesselated(std::move(*layout), 12, 24, withNormal);
	}

	static IndexedTriangleList MakeNormalUVed(std::optional<Dvtx::VertexLayout> layout = std::nullopt)
	{
		using Dvtx::VertexLayout;
		VertexLayout vl;
		vl.Append(VertexLayout::Position3D);
		vl.Append(VertexLayout::Normal);
		vl.Append(VertexLayout::Texture2D);

		return MakeTesselatedNormalUV(std::move(vl), 12, 24);
	}
};