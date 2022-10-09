#include "pch.h"
#include "Camera.h"
#include "Core/Core.h"

Camera::Camera(float angle, float clientWidth, float clientHeight, float nearPlane, float farPlane)
	:
	m_position(sm::Vector3(0.0f, 0.0f, -3.0f)),
	m_rotation(sm::Vector3(0.0f, 0.0f, 0.0f)),
	m_target(sm::Vector3(0.0f, 0.0f, 1.0f)),
	m_up(sm::Vector3::Up),
	m_view(sm::Matrix::Identity),
	m_projection(sm::Matrix::Identity),
	m_viewAngle(XMConvertToRadians(angle)),
	m_width(clientWidth),
	m_height(clientHeight),
	m_nearPlane(nearPlane),
	m_farPlane(farPlane)
{
	m_speed = 0.1f;
	m_sensitivity = 0.05f;
}

Camera::~Camera()
{
}

void Camera::Update(double dt, const DirectX::Keyboard::State& kb, const DirectX::Mouse::State& mouse)
{
	Move(dt, kb);
	Rotate(dt, mouse);
	
	sm::Matrix translation = XMMatrixLookToLH(m_position, m_target, m_up);
	sm::Matrix rotation = sm::Matrix::CreateFromYawPitchRoll(m_rotation);

	sm::Matrix camWorld = rotation * translation;

	m_view = camWorld;
	m_projection = XMMatrixPerspectiveFovLH(m_viewAngle, m_width / m_height, m_nearPlane, m_farPlane);
}

void Camera::Move(double dt, const DirectX::Keyboard::State& kb)
{
	sm::Vector3 move = sm::Vector3::Zero;

	if (kb.Up || kb.W)
		move.z += 1.f;

	if (kb.Down || kb.S)
		move.z -= 1.f;

	if (kb.Left || kb.A)
		move.x += 1.f;

	if (kb.Right || kb.D)
		move.x -= 1.f;

	if (kb.PageUp || kb.Space)
		move.y += 1.f;

	if (kb.PageDown || kb.X)
		move.y -= 1.f;

	sm::Quaternion q = sm::Quaternion::CreateFromYawPitchRoll(m_rotation);

	move = sm::Vector3::Transform(move, q);

	move *= m_speed;

	m_position += move;
}

void Camera::Rotate(double dt, const DirectX::Mouse::State& mouse)
{
	if (mouse.positionMode == Mouse::MODE_RELATIVE)  // When rmb is pressed
	{
		m_rotation.y += mouse.x * m_sensitivity;
		m_rotation.x += mouse.y * m_sensitivity;
	}
}
