#pragma once
#ifndef MESH_H
#define MESH_H

#include "Texture.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "ConstantBuffer.h"
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <vector>

struct VertexOBJ
{
	XMFLOAT3 Position;
	XMFLOAT2 TexCoord;
	XMFLOAT3 Normal;
};

class Mesh
{
public:
	Mesh( ID3D11Device* device,
		ID3D11DeviceContext* context,
		std::vector<VertexOBJ>& vertices,
		std::vector<WORD>& indices,
		std::vector<Texture>& textures,
		const DirectX::XMMATRIX& transformMatrix );
	const DirectX::XMMATRIX& GetTransformMatrix();
	Mesh( const Mesh& mesh );
	void Draw();
private:
	VertexBuffer<VertexOBJ> vertexBuffer;
	IndexBuffer indexBuffer;
	ID3D11DeviceContext* context;
	std::vector<Texture> textures;
	DirectX::XMMATRIX transformMatrix;
};

#endif