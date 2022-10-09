#pragma once
#include <Keyboard.h>
#include <Mouse.h>

class Camera
{
public:
	Camera(float angle, float clientWidth, float clientHeight, float nearPlane = 0.01f, float farPlane = 100.0f);
	~Camera();

	void Update(double dt, const DirectX::Keyboard::State& kb, const DirectX::Mouse::State& mouse);

	void Position(sm::Vector3 pos) { m_position = pos; }
	const sm::Vector3& Position() { return m_position; }

	const sm::Matrix& GetView()           const { return m_view; }
	const sm::Matrix& GetProjection()     const { return m_projection; }
	const sm::Matrix& GetViewProjection() const { return m_view * m_projection; }

private:
	void Move(double dt, const DirectX::Keyboard::State& kb);
	void Rotate(double dt, const DirectX::Mouse::State& mouse);

private:
	sm::Vector3 m_position;
	sm::Vector3 m_rotation;
	sm::Vector3 m_target;
	sm::Vector3 m_up;

	float m_viewAngle;
	float m_width;
	float m_height;
	float m_nearPlane;
	float m_farPlane;

	float m_speed;
	float m_sensitivity;

	sm::Matrix m_view;
	sm::Matrix m_projection;
};