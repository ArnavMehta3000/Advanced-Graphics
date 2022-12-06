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
	void DoForwardRendering();
	void DoDeferredRendering();

	void InitGBuffer();
	void SetGBuffer();
	void DoGeometryPass();
	// Set & clear back buffer
	// Set back buffer shader resources
	void DoLightingPass();


	void OnGui();

private:
	Timer           m_appTimer;
	Window*         m_window;
	Camera          m_camera;

	RenderTechnique m_technique;

	RenderTarget    m_colorTarget;
	RenderTarget    m_normalTarget;
	RenderTarget    m_positionTarget;

	Shader          m_geometryShader;
	Shader          m_lightingShader;

	float           m_imageScale = 1.0f;

	sm::Vector3     m_lightPosition;
	sm::Vector3     m_lightDiffuse;
	sm::Vector3     m_lightSpecular;
	sm::Vector3     m_lightAttenuation;
	sm::Vector4     m_parallaxData;
	sm::Vector4     m_biasData;
};