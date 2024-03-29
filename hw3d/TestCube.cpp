#include "TestCube.h"
#include "Cube.h"
#include "BindableCommon.h"
#include "TransformCbufDoubleboi.h"
#include "imgui/imgui.h"

TestCube::TestCube( Graphics& gfx,float size )
{
	using namespace Bind;
	namespace dx = DirectX;

	auto model = Cube::MakeIndependentTextured();
	model.Transform( dx::XMMatrixScaling( size,size,size ) );
	model.SetTBNIndependentFlat();
	const auto geometryTag = "$cube." + std::to_string( size );
	AddBind( VertexBuffer::Resolve( gfx,geometryTag,model.vertices ) );
	AddBind( IndexBuffer::Resolve( gfx,geometryTag,model.indices ) );

	AddBind(Texture::Resolve(gfx, "Images\\brickwall.jpg"));
	AddBind(Texture::Resolve(gfx, "Images\\brickwall_normal.jpg", 1u));
	AddBind(TexturePre::Resolve(gfx, 10u, gfx.GetShaderResourceView()));
	AddBind(TexturePre::Resolve(gfx, 11u, gfx.GetShaderResourceView('M')));
	AddBind(TexturePre::Resolve(gfx, 12u, gfx.GetShaderResourceView('L')));

	auto pvs = VertexShader::Resolve( gfx,"PBRTestVS.cso" );
	auto pvsbc = pvs->GetBytecode();
	AddBind( std::move( pvs ) );

	AddBind( PixelShader::Resolve( gfx,"PBRTestPS.cso" ) );

	AddBind( PixelConstantBuffer<PSMaterialConstant>::Resolve( gfx,pmc,3u ) );

	AddBind( InputLayout::Resolve( gfx,model.vertices.GetLayout(),pvsbc ) );

	AddBind( Topology::Resolve( gfx,D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST ) );

	AddBind( std::make_shared<TransformCbufDoubleboi>( gfx,*this,0u,4u ) );
}

void TestCube::SetPos( DirectX::XMFLOAT3 pos ) noexcept
{
	this->pos = pos;
}

void TestCube::SetRotation( float roll,float pitch,float yaw ) noexcept
{
	this->roll = roll;
	this->pitch = pitch;
	this->yaw = yaw;
}

DirectX::XMMATRIX TestCube::GetTransformXM() const noexcept
{
	return DirectX::XMMatrixRotationRollPitchYaw(pitch, yaw, roll) *
		DirectX::XMMatrixTranslation( pos.x,pos.y,pos.z );
}

void TestCube::SpawnControlWindow( Graphics& gfx ) noexcept
{
	if( ImGui::Begin( "Cube" ) )
	{
		ImGui::Text( "Position" );
		ImGui::SliderFloat( "X",&pos.x,-80.0f,80.0f,"%.1f" );
		ImGui::SliderFloat( "Y",&pos.y,-80.0f,80.0f,"%.1f" );
		ImGui::SliderFloat( "Z",&pos.z,-80.0f,80.0f,"%.1f" );
		ImGui::Text( "Orientation" );
		ImGui::SliderAngle( "Pitch",&pitch,-180.0f,180.0f );
		ImGui::SliderAngle( "Yaw",&yaw,-180.0f,180.0f );
		ImGui::SliderAngle("Roll", &roll, -180.0f, 180.0f);
		ImGui::Text( "Shading" );
		bool changed0 = ImGui::SliderFloat("Metallic", &pmc.metallic, 0.0f, 1.0f);
		bool changed1 = ImGui::SliderFloat("Roughness", &pmc.roughness, 0.0f, 1.0f);
		bool checkState = pmc.normalMappingEnabled == TRUE;
		bool changed2 = ImGui::Checkbox( "Enable Normal Map",&checkState );
		pmc.normalMappingEnabled = checkState ? TRUE : FALSE;
		if( changed0 || changed1 || changed2 )
		{
			QueryBindable<Bind::PixelConstantBuffer<PSMaterialConstant>>()->Update( gfx,pmc );
		}
	}
	ImGui::End();
}
