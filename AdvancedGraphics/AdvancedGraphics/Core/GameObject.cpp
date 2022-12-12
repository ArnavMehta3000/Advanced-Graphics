#include "pch.h"
#include "GameObject.h"
#include "Graphics/Direct3D.h"
#include "Utils/DDSTextureLoader.h"
#define TINYOBJLOADER_IMPLEMENTATION
#include "Utils/tiny_obj_loader.h"

GameObject::GameObject()
	:
	m_stride(0),
	m_position(0.0f),
	m_rotation(0.0f),
	m_scale(1.0f),
	m_isObj(false),
	m_mesh(nullptr),
	m_textureDiffRV(nullptr),
	m_textureNormRV(nullptr),
	m_textureHeightRV(nullptr)
{
	m_material                        = MaterialProperties();
	m_material.Material.Diffuse       = sm::Vector4(1.0f, 1.0f, 1.0f, 1.0f);
	m_material.Material.Specular      = sm::Vector4(1.0f, 1.0f, 1.0f, 1.0f);
	m_material.Material.SpecularPower = 32.0f;

	D3D->CreateConstantBuffer(m_materialPropsCB, sizeof(MaterialProperties));
	D3D->CreateConstantBuffer(m_surfacePropsCB, sizeof(SurfaceProperties));
}

GameObject::GameObject(const GODesc& desc)
	:
	m_stride(0),
	m_position(0.0f),
	m_rotation(0.0f),
	m_scale(1.0f),
	m_isObj(false),
	m_mesh(nullptr),
	m_textureDiffRV(nullptr),
	m_textureNormRV(nullptr),
	m_textureHeightRV(nullptr)
{
	using namespace Primitives;
	// Init mesh
	if (desc.HasMesh)
	{
		// Object is predefined primitive
		if (desc.IsPrimitive)
		{
			switch (desc.PrimitiveType)
			{
			case Type::TRIANGLE:
				GO_CREATE_MESH(this, Triangle);
				break;

			case Type::CUBE:
				GO_CREATE_MESH(this, Cube);
				break;

			case Type::FSQUAD:
				GO_CREATE_MESH(this, FSQuad);
				break;
			}
		}
		else  // Object is .obj file
		{
			InitMesh(desc.MeshFile);
		}

		// Set textures
		if (desc.HasDiffuse && desc.HasNormal && desc.HasHeight)
			SetTexture(desc.DiffuseTexture, desc.NormalMap, desc.HeightMap);
		else if (desc.HasDiffuse)
			SetTexture(desc.DiffuseTexture);
	}

	m_material                        = MaterialProperties();
	m_material.Material.Diffuse       = sm::Vector4(1.0f, 1.0f, 1.0f, 1.0f);
	m_material.Material.Specular      = sm::Vector4(1.0f, 1.0f, 1.0f, 1.0f);
	m_material.Material.SpecularPower = 32.0f;

	D3D->CreateConstantBuffer(m_materialPropsCB, sizeof(MaterialProperties));
	D3D->CreateConstantBuffer(m_surfacePropsCB, sizeof(SurfaceProperties));
}

GameObject::~GameObject()
{
	SAFE_DELETE(m_mesh);
	COM_RELEASE(m_textureDiffRV);
	COM_RELEASE(m_textureNormRV);
	COM_RELEASE(m_textureHeightRV);
	COM_RELEASE(m_vertexBuffer);
	COM_RELEASE(m_indexBuffer);
	COM_RELEASE(m_materialPropsCB);
}

// https://github.com/tinyobjloader/tinyobjloader
// Vertex Buffer creation from: https://www.youtube.com/watch?v=jdiPVfIHmEA&t=1227s
void GameObject::InitMesh(const char* objFile)
{
	namespace TO = tinyobj;
	m_isObj = true;
	
	TO::attrib_t attrib;
	std::vector<TO::shape_t> shapes;
	std::vector<TO::material_t> materials;  // Not used
	std::string err;

	LOG("Attempting load OBJ: " << objFile);
	if (!TO::LoadObj(&attrib, &shapes, &materials, &err, objFile, "", false)) { LOG(err); HR(E_PENDING); }  // HRESULT used to halt execution here

	std::vector<SimpleVertex> vertices;
	std::vector<WORD> indices;

	for (const auto& shape : shapes)
	{
		for (const auto& index : shape.mesh.indices)
		{
			SimpleVertex vertex;

			// Position
			if (index.vertex_index >= 0)
			{
				vertex.Pos =
				{
					attrib.vertices[3 * index.vertex_index + 0],
					attrib.vertices[3 * index.vertex_index + 1],
					attrib.vertices[3 * index.vertex_index + 2]

				};
			}

			// Normals
			if (index.normal_index >= 0)
			{
				vertex.Normal =
				{
					attrib.normals[3 * index.normal_index + 0],
					attrib.normals[3 * index.normal_index + 1],
					attrib.normals[3 * index.normal_index + 2]

				};
			}

			// Texture Coordinates (UV)
			if (index.texcoord_index >= 0)
			{
				vertex.TexCoord =
				{
					attrib.texcoords[2 * index.texcoord_index + 0],
					attrib.texcoords[2 * index.texcoord_index + 1],

				};
			}
			
			// Add the created vertex to the vertex buffer vector
			vertices.push_back(vertex);
		}
	}

	m_mesh = new Mesh(vertices);
	LOG("OBJ load successful");

	vertices.clear();
	indices.clear();
	shapes.clear();
	materials.clear();
}

void GameObject::InitMesh(const void* vertices, const void* indices, UINT vertexTypeSize, UINT vertexByteWidth, UINT indexByteWidth, UINT indicesCount)
{
	// Create vertex buffer
	CREATE_ZERO(D3D11_BUFFER_DESC, vbd);
	vbd.Usage             = D3D11_USAGE_DEFAULT;
	vbd.ByteWidth         = vertexByteWidth;
	vbd.BindFlags         = D3D11_BIND_VERTEX_BUFFER;
	vbd.CPUAccessFlags    = 0;
	CREATE_ZERO(D3D11_SUBRESOURCE_DATA, vertexInitData);
	vertexInitData.pSysMem = vertices;
	HR(D3D_DEVICE->CreateBuffer(&vbd, &vertexInitData, m_vertexBuffer.ReleaseAndGetAddressOf()));

	// Save vertex/index  buffer data
	m_stride     = vertexTypeSize;
	m_offset     = 0;
	m_indexCount = indicesCount;
	
	// Create index buffer
	CREATE_ZERO(D3D11_BUFFER_DESC, ibd);
	ibd.Usage             = D3D11_USAGE_DEFAULT;
	ibd.ByteWidth         = indexByteWidth;
	ibd.BindFlags         = D3D11_BIND_INDEX_BUFFER;
	ibd.CPUAccessFlags    = 0;
	CREATE_ZERO(D3D11_SUBRESOURCE_DATA, indexInitData);
	indexInitData.pSysMem      = indices;	
	HR(D3D_DEVICE->CreateBuffer(&ibd, &indexInitData, m_indexBuffer.ReleaseAndGetAddressOf()));
}

void GameObject::SetTexture(const wchar_t* diffuse, const wchar_t* normal, const wchar_t* height)
{
	HR(CreateDDSTextureFromFile(D3D_DEVICE, D3D_CONTEXT, diffuse, nullptr, m_textureDiffRV.ReleaseAndGetAddressOf()));
	HR(CreateDDSTextureFromFile(D3D_DEVICE, D3D_CONTEXT, normal, nullptr, m_textureNormRV.ReleaseAndGetAddressOf()));
	HR(CreateDDSTextureFromFile(D3D_DEVICE, D3D_CONTEXT, height, nullptr, m_textureHeightRV.ReleaseAndGetAddressOf()));
	m_material.Material.UseTexture = true;
	m_material.Material.UseNormals = true;
	m_material.Material.UseHeight  = true;
}

void GameObject::SetTexture(const wchar_t* diffuse)
{
	HR(CreateDDSTextureFromFile(D3D_DEVICE, D3D_CONTEXT, diffuse, nullptr, m_textureDiffRV.ReleaseAndGetAddressOf()));
	m_material.Material.UseTexture = true;
	m_material.Material.UseNormals = false;
	m_material.Material.UseHeight = false;
}

void GameObject::Set()
{
	D3D_CONTEXT->IASetVertexBuffers(0, 1, m_vertexBuffer.GetAddressOf(), &m_stride, &m_offset);
	D3D_CONTEXT->IASetIndexBuffer(m_indexBuffer.Get(), DXGI_FORMAT_R16_UINT, 0);
}



void GameObject::Update(double dt)
{
	sm::Matrix scale       = sm::Matrix::CreateScale(m_scale);
	sm::Matrix rotation    = sm::Matrix::CreateFromYawPitchRoll(m_rotation);
	sm::Matrix translation = sm::Matrix::CreateTranslation(m_position);

	m_worldTransform = scale * rotation * translation;
}

void GameObject::Draw()
{	
	Set();

	if (m_isObj)
		m_mesh->Draw();
	else
		D3D_CONTEXT->DrawIndexed(m_indexCount, 0, 0);
}