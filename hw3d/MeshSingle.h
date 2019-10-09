#pragma once
#include "Drawable.h"
#include "BindableCommon.h"
#include "Vertex.h"
#include <optional>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "ConditionalNoexcept.h"
#include "ChiliMath.h"

class MeshSingle : public Drawable
{
public:
	MeshSingle(Graphics& gfx, const std::string fileName, float size);
	void SetPos(DirectX::XMFLOAT3 pos) noexcept;
	void SetRotation(float roll, float pitch, float yaw) noexcept;
	DirectX::XMMATRIX GetTransformXM() const noexcept override;
	void SpawnControlWindow(Graphics& gfx) noexcept;
	void ChangeSphereMaterialState(Graphics& gfx, float pitch, float yaw, float roll) noexcept;

public:
	Dvtx::VertexBuffer UVPos;
	std::vector<unsigned short> indices;
	float roll = PI / 2.0f;
	float pitch = PI / 2.0f;
	float yaw = 0.0f;

private:
	struct PSMaterialConstant
	{
		float metallic = 1.0f;
		float roughness = 1.0f;
		BOOL normalMappingEnabled = TRUE;
		DirectX::XMMATRIX EVRotation = DirectX::XMMatrixIdentity();
		float padding[1];
	} pmc;
	DirectX::XMFLOAT3 pos = { 0.0f,1.0f,0.0f };
};