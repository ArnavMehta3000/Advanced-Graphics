#include "pch.h"
#include "GameObject.h"
#include "Graphics/Primitives.h"
#include "Graphics/Direct3D.h"
#include "Utils/DDSTextureLoader.h"
#define TINYOBJLOADER_IMPLEMENTATION
#include "Utils/tiny_obj_loader.h"

GameObject::GameObject()
	:
	m_stride(0),
	m_position(0.0f),
	m_rotation(0.0f),
	m_scale(1.0f)

{
	// Create material constant buffer
	m_material.Material.Diffuse = sm::Vector4(1.0f, 1.0f, 1.0f, 1.0f);
	m_material.Material.Specular = sm::Vector4(1.0f, 0.2f, 0.2f, 1.0f);
	m_material.Material.SpecularPower = 32.0f;
	m_material.Material.UseTexture = true;

	D3D->CreateConstantBuffer(m_materialCBuffer, sizeof(MaterialProperties));
}

GameObject::~GameObject()
{
	COM_RELEASE(m_textureRV);
	COM_RELEASE(m_vertexBuffer);
	COM_RELEASE(m_indexBuffer);
	COM_RELEASE(m_materialCBuffer);
}

// https://github.com/tinyobjloader/tinyobjloader
void GameObject::InitMesh(const char* objFile, const wchar_t* textureFile)
{
	tinyobj::attrib_t attributes;
	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> material;
	std::string err;
	
	if (!tinyobj::LoadObj(&attributes, &shapes, &material, &err, objFile, "", false))
	{
		LOG(err);
		HR(E_FAIL);  // HRESULT fail used to halt execution here
	}

	std::vector<sm::Vector3> vertices;
	std::vector<sm::Vector3> normals;
	std::vector<sm::Vector2> uvs;
	std::vector<DWORD> indices;  // Tiny obj uses uint32_t -> DWORD


	// Loop over shapes
	for (size_t s = 0; s < shapes.size(); s++)
	{
		// Loop over faces
		size_t indexOffset = 0;
		for (size_t f = 0; f < shapes[s].mesh.num_face_vertices.size(); f++)
		{
			size_t fv = size_t(shapes[s].mesh.num_face_vertices[f]);

			// Loop over vertices in the face
			for (size_t v = 0; v < fv; v++)
			{
				// access to vertex
				tinyobj::index_t idx = shapes[s].mesh.indices[indexOffset + v];
				tinyobj::real_t vx = attributes.vertices[3 * size_t(idx.vertex_index) + 0];
				tinyobj::real_t vy = attributes.vertices[3 * size_t(idx.vertex_index) + 1];
				tinyobj::real_t vz = attributes.vertices[3 * size_t(idx.vertex_index) + 2];

				vertices.push_back(sm::Vector3(vx, vy, vz));
				indices.push_back(idx.vertex_index);

				// Check if `normal_index` is zero or positive. negative = no normal data
				if (idx.normal_index >= 0)
				{
					tinyobj::real_t nx = attributes.normals[3 * size_t(idx.normal_index) + 0];
					tinyobj::real_t ny = attributes.normals[3 * size_t(idx.normal_index) + 1];
					tinyobj::real_t nz = attributes.normals[3 * size_t(idx.normal_index) + 2];

					normals.push_back(sm::Vector3(nx, ny, nz));
				}

				// Check if `texcoord_index` is zero or positive. negative = no texcoord data
				if (idx.texcoord_index >= 0)
				{
					tinyobj::real_t tx = attributes.texcoords[2 * size_t(idx.texcoord_index) + 0];
					tinyobj::real_t ty = attributes.texcoords[2 * size_t(idx.texcoord_index) + 1];
					uvs.push_back(sm::Vector2(tx, ty));
				}
			}
			indexOffset += fv;
		}
	}

	LOG("Vertices");
	std::vector<SimpleVertex> vertexBuffer;
	for (size_t i = 0; i < vertices.size(); i++)
	{
		SimpleVertex sv;
		sv.Pos = vertices[i];
		sv.Normal = normals[i];
		sv.TexCoord = uvs[i];
		LOG(i << ": " << sv.Pos.x << " " << sv.Pos.y << " " << sv.Pos.z << "\tindex: " << indices[i]);
		vertexBuffer.push_back(sv);
	}

	CREATE_ZERO(D3D11_BUFFER_DESC, vbd);
	vbd.Usage          = D3D11_USAGE_DEFAULT;
	vbd.ByteWidth      = sizeof(SimpleVertex) * (UINT)vertices.size();
	vbd.BindFlags      = D3D11_BIND_VERTEX_BUFFER;
	vbd.CPUAccessFlags = 0;
	CREATE_ZERO(D3D11_SUBRESOURCE_DATA, vertexInitData);
	vertexInitData.pSysMem = &vertices[0];
	HR(D3D_DEVICE->CreateBuffer(&vbd, &vertexInitData, m_vertexBuffer.ReleaseAndGetAddressOf()));

	CREATE_ZERO(D3D11_BUFFER_DESC, ibd);
	ibd.Usage = D3D11_USAGE_DEFAULT;
	ibd.ByteWidth = sizeof(DWORD) * (UINT)indices.size();
	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibd.CPUAccessFlags = 0;
	CREATE_ZERO(D3D11_SUBRESOURCE_DATA, indexInitData);
	indexInitData.pSysMem = &indices[0];
	HR(D3D_DEVICE->CreateBuffer(&ibd, &indexInitData, m_indexBuffer.ReleaseAndGetAddressOf()));
}

void GameObject::InitMesh(const void* vertices, const void* indices, UINT vertexTypeSize, UINT vertexByteWidth, UINT indexByteWidth, UINT indicesCount, const wchar_t* textureFile)
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

	

	// Load texture 
	HR(CreateDDSTextureFromFile(D3D_DEVICE, D3D_CONTEXT, textureFile, nullptr, m_textureRV.ReleaseAndGetAddressOf()));
}

void GameObject::Set()
{
	D3D_CONTEXT->PSSetSamplers(0, 1, D3D_DEFAULT_SAMPLER.GetAddressOf());
	D3D_CONTEXT->PSSetShaderResources(0, 1, m_textureRV.GetAddressOf());
	D3D_CONTEXT->PSSetConstantBuffers(1, 1, m_materialCBuffer.GetAddressOf());


	D3D_CONTEXT->IASetVertexBuffers(0, 1, m_vertexBuffer.GetAddressOf(), &m_stride, &m_offset);
	D3D_CONTEXT->IASetIndexBuffer(m_indexBuffer.Get(), DXGI_FORMAT_R16_UINT, 0);
}

void GameObject::Update(double dt)
{
	sm::Matrix scale       = sm::Matrix::CreateScale(m_scale);
	sm::Matrix rotation    = sm::Matrix::CreateFromYawPitchRoll(m_rotation);
	sm::Matrix translation = sm::Matrix::CreateTranslation(m_position);

	m_worldTransform = scale * rotation * translation;

	D3D_CONTEXT->UpdateSubresource(m_materialCBuffer.Get(), 0, nullptr, &m_material, 0, 0);
}

void GameObject::Draw()
{
	D3D_CONTEXT->DrawIndexed(m_indexCount, 0, 0);
}