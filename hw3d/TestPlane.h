#pragma once
#include "Drawable.h"

class TestPlane : public Drawable
{
public:
	TestPlane( Graphics& gfx,float size );
	void SetPos( DirectX::XMFLOAT3 pos ) noexcept;
	void SetRotation( float roll,float pitch,float yaw ) noexcept;
	DirectX::XMMATRIX GetTransformXM() const noexcept override;
	void SpawnControlWindow( Graphics& gfx ) noexcept;
	void ChangeSphereMaterialState(Graphics& gfx, float pitch, float yaw, float roll) noexcept;
	void Bind(Graphics& gfx, float deltaTime) noexcept;
private:
	struct PSMaterialConstant
	{
		/*float specularIntensity = 0.18f;
		float specularPower = 18.0f;
		BOOL normalMappingEnabled = TRUE;
		float padding[1];*/
		float metallic = 1.0f;
		float roughness = 1.0f;
		BOOL normalMappingEnabled = FALSE;
		DirectX::XMMATRIX EVRotation = DirectX::XMMatrixIdentity();
		float time = 0.0f;
	} pmc;
	DirectX::XMFLOAT3 pos = { 0.0f,0.0f,0.0f };
	float roll = 0.0f;
	float pitch = 0.0f;
	float yaw = 0.0f;
};