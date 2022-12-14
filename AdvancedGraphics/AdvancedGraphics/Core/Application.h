#pragma once
#include "Core/Timer.h"
#include "Core/Window.h"
#include "Graphics/Direct3D.h"
#include "Core/GameObject.h"
#include "Core/Camera.h"

#define TO_VEC4(vec, val) sm::Vector4(vec.x, vec.y, vec.z, val)

enum class RenderTechnique
{
	Forward,
	Deferred
};

class Application
{
#define KEYBOARD m_window->GetKbState()
#define MOUSE m_window->GetMouseState()

public:
	Application(HINSTANCE hInst, UINT width, UINT height);
	~Application();

	bool Init();
	void Run();
	void Shutdown();

	void SetTechnique(RenderTechnique technique) { m_technique = technique; }
	
private:
	void CreateQuad();
	void ClearAllCB();  // Unbinds all constant buffers
	void UpdateWorld(double dt);
	void InitConstantBuffers();
	void DoForwardRendering();
	void DoDeferredRendering();
	void DoPostProcess();

	void InitGBuffer();
	void SetGBuffer();
	void DoGeometryPass();
	void DoLightingPass();

	void OnGui(double dt);

private:
	Timer                    m_appTimer;
	Window*                  m_window;
	Camera                   m_camera;
	RenderTechnique          m_technique;

	std::vector<GameObject*> m_gameObjects;

	RenderTarget             m_colorTarget;
	RenderTarget             m_normalTarget;
	RenderTarget             m_depthRenderTarget;

	Shader                   m_forwardShader;
	Shader                   m_geometryShader;
	Shader                   m_lightingShader;

	ComPtr<ID3D11Buffer>     m_wvpCBuffer;
	ComPtr<ID3D11Buffer>     m_cameraBuffer;
	ComPtr<ID3D11Buffer>     m_lightPropsCB;
	ComPtr<ID3D11Buffer>     m_postProcessCB;
	ComPtr<ID3D11Buffer>     m_quadVB;
	ComPtr<ID3D11Buffer>     m_quadIB;

	float                    m_imageScale = 1.0f;
	float                    m_lightRadius;
	float                    m_lightIntensity;
	float                    m_specularPower;
	UINT                     m_stride;
	UINT                     m_offset;
	UINT                     m_quadIndicesCount;

	bool                     m_showPreview;
	bool                     m_enableVignette;
	bool                     m_enableGrayscale;
	sm::Vector2              m_vigRadSoft;

	sm::Vector3              m_lightPosition;
	sm::Vector3              m_lightDiffuse;
	sm::Vector3              m_lightSpecular;
	sm::Vector4              m_parallaxData;
	sm::Vector4              m_biasData;
	sm::Vector4              m_globalAmbient;
};