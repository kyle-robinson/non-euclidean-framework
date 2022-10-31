#include "stdafx.h"
#include "Model.h"

bool Model::Initialize(
	const std::string& filePath,
	ID3D11Device* device,
	ID3D11DeviceContext* context,
	ConstantBuffer<Matrices>& cb_vertexShader )
{
	this->device = device;
	this->context = context;
	this->cbMatrices = &cb_vertexShader;

	try
	{
		if ( !LoadModel( filePath ) )
			return false;
	}
	catch ( COMException& exception )
	{
		ErrorLogger::Log( exception );
		return false;
	}

	return true;
}

void Model::Draw( const XMMATRIX& worldMatrix, const XMMATRIX& viewMatrix, const XMMATRIX& projectionMatrix )
{
	cbMatrices->data.mView = viewMatrix;
	cbMatrices->data.mProjection = projectionMatrix;
	context->VSSetConstantBuffers( 0, 1, cbMatrices->GetAddressOf() );
	
	for ( int i = 0; i < meshes.size(); i++ )
	{
		cbMatrices->data.mWorld = meshes[i].GetTransformMatrix() * worldMatrix;
		cbMatrices->ApplyChanges();
		meshes[i].Draw();
	}
}

bool Model::LoadModel( const std::string& filePath )
{
	directory = StringConverter::GetDirectoryFromPath( filePath );
	Assimp::Importer importer;
	const aiScene* pScene = importer.ReadFile( filePath,
		aiProcess_Triangulate | aiProcess_ConvertToLeftHanded );
	if ( pScene == nullptr )
		return false;
	ProcessNode( pScene->mRootNode, pScene, XMMatrixIdentity() );
	return true;
}

void Model::ProcessNode( aiNode* node, const aiScene* scene, const XMMATRIX& parentTransformMatrix )
{
	XMMATRIX nodeTransformMatrix = XMMatrixTranspose( static_cast<XMMATRIX>( &node->mTransformation.a1 ) ) * parentTransformMatrix;
	
	for ( UINT i = 0; i < node->mNumMeshes; i++ )
	{
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		meshes.push_back( ProcessMesh( mesh, scene, nodeTransformMatrix ) );
	}

	for ( UINT i = 0; i < node->mNumChildren; i++ )
		ProcessNode( node->mChildren[i], scene, nodeTransformMatrix );
}

Mesh Model::ProcessMesh( aiMesh* mesh, const aiScene* scene, const XMMATRIX& transformMatrix )
{
	std::vector<VertexOBJ> vertices;
	std::vector<WORD> indices;

	// get vertices
	for ( UINT i = 0; i < mesh->mNumVertices; i++ )
	{
		VertexOBJ vertex;

		vertex.Position.x = mesh->mVertices[i].x;
		vertex.Position.y = mesh->mVertices[i].y;
		vertex.Position.z = mesh->mVertices[i].z;

		if ( mesh->mTextureCoords[0] )
		{
			vertex.TexCoord.x = static_cast<float>( mesh->mTextureCoords[0][i].x );
			vertex.TexCoord.y = static_cast<float>( mesh->mTextureCoords[0][i].y );
		}

		vertex.Normal.x = mesh->mNormals[i].x;
		vertex.Normal.y = mesh->mNormals[i].y;
		vertex.Normal.z = mesh->mNormals[i].z;

		vertices.push_back( vertex );
	}

	for ( UINT i = 0; i < mesh->mNumFaces; i++ )
	{
		aiFace face = mesh->mFaces[i];
		for ( UINT j = 0; j < face.mNumIndices; j++ )
			indices.push_back( face.mIndices[j] );
	}

	std::vector<Texture> textures;
	aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
	std::vector<Texture> diffuseTextures = LoadMaterialTextures( material, aiTextureType_DIFFUSE, scene );
	textures.insert( textures.end(), diffuseTextures.begin(), diffuseTextures.end() );
	std::vector<Texture> specularTextures = LoadMaterialTextures( material, aiTextureType_SPECULAR, scene );
	textures.insert( textures.end(), specularTextures.begin(), specularTextures.end() );

	return Mesh( device, context, vertices, indices, textures, transformMatrix );
}

TextureStorageType Model::GetTextureStorageType( const aiScene* pScene, aiMaterial* pMaterial, unsigned int index, aiTextureType textureType )
{
	if ( pMaterial->GetTextureCount( textureType ) == 0 )
		return TextureStorageType::None;

	aiString path;
	pMaterial->GetTexture( textureType, index, &path );
	std::string texturePath = path.C_Str();
	if ( texturePath[0] == '*' )
	{
		if ( pScene->mTextures[0]->mHeight == 0 )
		{
			return TextureStorageType::EmbeddedIndexCompressed;
		}
		else
		{
			assert( "Support does not exist for indexed non-compressed textures!" && 0 );
			return TextureStorageType::EmbeddedIndexNonCompressed;
		}
	}
	if ( auto pTex = pScene->GetEmbeddedTexture( texturePath.c_str() ) )
	{
		if ( pTex->mHeight == 0 )
		{
			return TextureStorageType::EmbeddedCompressed;
		}
		else
		{
			assert( "Support does not exist for embedded non-compressed textures!" && 0 );
			return TextureStorageType::EmbeddedNonCompressed;
		}
	}
	if ( texturePath.find( '.' ) != std::string::npos )
	{
		return TextureStorageType::Disk;
	}
	return TextureStorageType::None;
}

std::vector<Texture> Model::LoadMaterialTextures( aiMaterial* pMaterial, aiTextureType textureType, const aiScene* pScene )
{
	std::vector<Texture> materialTextures;
	TextureStorageType storeType = TextureStorageType::Invalid;
	unsigned int textureCount = pMaterial->GetTextureCount( textureType );

	if ( textureCount == 0 )
	{
		storeType = TextureStorageType::None;
		aiColor3D aiColor( 0.0f, 0.0f, 0.0f );
		switch ( textureType )
		{
		case aiTextureType_DIFFUSE:
			pMaterial->Get( AI_MATKEY_COLOR_DIFFUSE, aiColor );
			if ( aiColor.IsBlack() )
			{
				materialTextures.push_back( Texture( device, Colours::UnloadedTextureColour, textureType ) );
				return materialTextures;
			}
			materialTextures.push_back( Texture( device, Colour( aiColor.r * 255, aiColor.g * 255, aiColor.b * 255 ), textureType ) );
			return materialTextures;
		case aiTextureType_SPECULAR:
			pMaterial->Get( AI_MATKEY_COLOR_SPECULAR, aiColor );
			if ( aiColor.IsBlack() )
			{
				materialTextures.push_back( Texture( device, Colours::UnloadedTextureColour, textureType ) );
				return materialTextures;
			}
			materialTextures.push_back( Texture( device, Colour( aiColor.r * 255, aiColor.g * 255, aiColor.b * 255 ), textureType ) );
			return materialTextures;
		}
	}
	else
	{
		for ( UINT i = 0; i < textureCount; i++ )
		{
			aiString path;
			pMaterial->GetTexture( textureType, i, &path );
			TextureStorageType storageType = GetTextureStorageType( pScene, pMaterial, i, textureType );
			switch ( storageType )
			{
				case TextureStorageType::Disk:
				{
					std::string fileName = directory + '\\' + path.C_Str();
					Texture diskTexture( device, fileName, textureType );
					materialTextures.push_back( diskTexture );
					break;
				}
				case TextureStorageType::EmbeddedCompressed:
				{
					const aiTexture* pTexture = pScene->GetEmbeddedTexture( path.C_Str() );
					Texture embeddedTexture( device, reinterpret_cast<uint8_t*>( pTexture->pcData ),
						pTexture->mWidth, textureType );
					materialTextures.push_back( embeddedTexture );
					break;
				}
				case TextureStorageType::EmbeddedIndexCompressed:
				{
					int index = GetTextureIndex( &path );
					Texture embeddedTextureIndexed( device, reinterpret_cast<uint8_t*>( pScene->mTextures[index]->pcData ),
						pScene->mTextures[index]->mWidth, textureType );
					materialTextures.push_back( embeddedTextureIndexed );
					break;
				}
			}
		}
	}

	if ( materialTextures.size() == 0 )
		materialTextures.push_back( Texture( device, Colours::UnhandledTextureColour, aiTextureType_DIFFUSE ) );

	return materialTextures;
}

int Model::GetTextureIndex( aiString* pStr )
{
	assert( pStr->length >= 2 );
	return atoi( &pStr->C_Str()[1] );
}