#include "MeshSingle.h"
#include "imgui/imgui.h"
#include "Surface.h"
#include <unordered_map>
#include <sstream>
#include "TransformCbufDoubleboi.h"

namespace dx = DirectX;
using Dvtx::VertexLayout;

MeshSingle::MeshSingle(Graphics& gfx, const std::string fileName, float size)
	:UVPos(std::move(VertexLayout{}.Append(VertexLayout::Texture2D)))
{
	Assimp::Importer imp;
	const auto pScene = imp.ReadFile(fileName.c_str(),
		aiProcess_Triangulate |
		aiProcess_JoinIdenticalVertices |
		aiProcess_ConvertToLeftHanded |
		aiProcess_GenNormals |
		aiProcess_CalcTangentSpace
	);

	const aiMesh& mesh = *pScene->mMeshes[0];

	using namespace Bind;

	Dvtx::VertexBuffer vbuf(std::move(
		VertexLayout{}
		.Append(VertexLayout::Position3D)
		.Append(VertexLayout::Normal)
		.Append(VertexLayout::Tangent)
		.Append(VertexLayout::Binormal)
		.Append(VertexLayout::Texture2D)
	));


	for (unsigned int i = 0; i < mesh.mNumVertices; i++)
	{
		vbuf.EmplaceBack(
			dx::XMFLOAT3(mesh.mVertices[i].x * size, mesh.mVertices[i].y * size, mesh.mVertices[i].z * size),
			*reinterpret_cast<dx::XMFLOAT3*>(&mesh.mNormals[i]),
			*reinterpret_cast<dx::XMFLOAT3*>(&mesh.mTangents[i]),
			*reinterpret_cast<dx::XMFLOAT3*>(&mesh.mBitangents[i]),
			*reinterpret_cast<dx::XMFLOAT2*>(&mesh.mTextureCoords[0][i])
		);

		UVPos.EmplaceBack(*reinterpret_cast<dx::XMFLOAT2*>(&mesh.mTextureCoords[0][i]));
	}

	indices.reserve(mesh.mNumFaces * 3);
	for (unsigned int i = 0; i < mesh.mNumFaces; i++)
	{
		const auto& face = mesh.mFaces[i];
		assert(face.mNumIndices == 3);
		indices.push_back(face.mIndices[0]);
		indices.push_back(face.mIndices[1]);
		indices.push_back(face.mIndices[2]);
	}

	//model.SetTBNIndependentFlat();
	const auto geometryTag = "$meshsingle." + std::to_string(size);
	AddBind(VertexBuffer::Resolve(gfx, geometryTag, vbuf));
	AddBind(IndexBuffer::Resolve(gfx, geometryTag, indices));

	//AddBind(Texture::Resolve(gfx, "Images\\jellybeans1.jpg", 0u, true));
	AddBind(Texture::Resolve(gfx, "Images\\Cerberus_D.jpg"));
	AddBind(Texture::Resolve(gfx, "Images\\Cerberus_N.jpg", 1u));
	AddBind(Texture::Resolve(gfx, "Images\\Cerberus_RMA.jpg", 2u));
	AddBind(TexturePre::Resolve(gfx, 3u, gfx.GetShaderResourceView()));
	AddBind(TexturePre::Resolve(gfx, 4u, gfx.GetShaderResourceView('M')));
	AddBind(TexturePre::Resolve(gfx, 5u, gfx.GetShaderResourceView('L')));

	auto pvs = VertexShader::Resolve(gfx, "PBRTestTexVS.cso");
	auto pvsbc = pvs->GetBytecode();
	AddBind(std::move(pvs));

	AddBind(PixelShader::Resolve(gfx, "PBRTestTexPS.cso"));

	AddBind(PixelConstantBuffer<PSMaterialConstant>::Resolve(gfx, pmc, 3u));

	AddBind(InputLayout::Resolve(gfx, vbuf.GetLayout(), pvsbc));

	AddBind(Topology::Resolve(gfx, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST));

	AddBind(std::make_shared<TransformCbufDoubleboi>(gfx, *this, 0u, 4u));
}

void MeshSingle::SetPos(DirectX::XMFLOAT3 pos) noexcept
{
	this->pos = pos;
}

void MeshSingle::SetRotation(float roll, float pitch, float yaw) noexcept
{
	this->roll = roll;
	this->pitch = pitch;
	this->yaw = wrap_angle(yaw);
}

DirectX::XMMATRIX MeshSingle::GetTransformXM() const noexcept
{
	return DirectX::XMMatrixRotationRollPitchYaw(pitch, yaw, roll) *
		DirectX::XMMatrixTranslation(pos.x, pos.y, pos.z);
}

void MeshSingle::SpawnControlWindow(Graphics& gfx) noexcept
{
	if (ImGui::Begin("Cerberus"))
	{
		ImGui::Text("Position");
		ImGui::SliderFloat("X", &pos.x, -80.0f, 80.0f, "%.1f");
		ImGui::SliderFloat("Y", &pos.y, -80.0f, 80.0f, "%.1f");
		ImGui::SliderFloat("Z", &pos.z, -80.0f, 80.0f, "%.1f");
		ImGui::Text("Orientation");
		ImGui::SliderAngle("Pitch", &pitch, -180.0f, 180.0f);
		ImGui::SliderAngle("Yaw", &yaw, -180.0f, 180.0f);
		ImGui::SliderAngle("Roll", &roll, -180.0f, 180.0f);
		ImGui::Text("Shading");
		bool changed0 = ImGui::SliderFloat("Metallic", &pmc.metallic, 0.0f, 10.0f);
		bool changed1 = ImGui::SliderFloat("Roughness", &pmc.roughness, 0.01f, 10.0f);
		bool checkState = pmc.normalMappingEnabled == TRUE;
		bool changed2 = ImGui::Checkbox("Enable Normal Map", &checkState);
		pmc.normalMappingEnabled = checkState ? TRUE : FALSE;
		if (changed0 || changed1 || changed2)
		{
			QueryBindable<Bind::PixelConstantBuffer<PSMaterialConstant>>()->Update(gfx, pmc);
		}
	}
	ImGui::End();
}

void MeshSingle::ChangeSphereMaterialState(Graphics& gfx, float pitch, float yaw, float roll) noexcept
{
	pmc.EVRotation = DirectX::XMMatrixRotationRollPitchYaw(pitch, yaw, roll);
	QueryBindable<Bind::PixelConstantBuffer<PSMaterialConstant>>()->Update(gfx, pmc);
}