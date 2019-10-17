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
public:
	struct VSMaterialConstant
	{
		alignas(16) DirectX::XMFLOAT3 color = { 0.162565f,0.271166f,0.325000f };
		alignas(16) DirectX::XMFLOAT3 attenuation = { 5.0f,5.0f,5.0f };
		alignas(16) DirectX::XMFLOAT3 scatteringKd = { 1.0f,1.0f,1.0f };
		float depth = 1.0f;
		float padding[3];
	} vmc;
	struct PSMaterialConstant
	{
		/*float specularIntensity = 0.18f;
		float specularPower = 18.0f;
		BOOL normalMappingEnabled = TRUE;
		float padding[1];*/
		float metallic = 1.0f;
		float roughness = 1.0f;
		BOOL normalMappingEnabled = TRUE;
		DirectX::XMMATRIX EVRotation = DirectX::XMMatrixIdentity();
		float time = 0.0f;
		float speed = 0.25f;
		float depth = 1.0f;
		float tilling = 1.0f;
		float flatten1 = 0.0f;
		float flatten2 = 0.625f;
		float padding[3];
	} pmc;
private:
	DirectX::XMFLOAT3 pos = { 0.0f,0.0f,0.0f };
	float roll = 0.0f;
	float pitch = 0.0f;
	float yaw = 0.0f;
};