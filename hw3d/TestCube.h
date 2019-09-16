#pragma once
#include "Drawable.h"

class TestCube : public Drawable
{
public:
	TestCube( Graphics& gfx,float size );
	void SetPos( DirectX::XMFLOAT3 pos ) noexcept;
	void SetRotation( float roll,float pitch,float yaw ) noexcept;
	DirectX::XMMATRIX GetTransformXM() const noexcept override;
	void SpawnControlWindow( Graphics& gfx ) noexcept;
private:
	struct PSMaterialConstant
	{
		/*float specularIntensity = 0.18f;
		float specularPower = 18.0f;
		BOOL normalMappingEnabled = TRUE;
		float padding[1];*/
		float metallic = 0.0f;
		float roughness = 0.5f;
		BOOL normalMappingEnabled = TRUE;
		float padding[1];
	} pmc;
	DirectX::XMFLOAT3 pos = { 1.0f,1.0f,1.0f };
	float roll = 0.0f;
	float pitch = 0.0f;
	float yaw = 0.0f;
};