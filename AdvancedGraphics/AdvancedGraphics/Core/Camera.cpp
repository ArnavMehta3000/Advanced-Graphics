#include "pch.h"
#include "Camera.h"

Camera::Camera(float angle, float clientWidth, float clientHeight, float nearPlane, float farPlane)
	:
	m_position(sm::Vector3(0.0f, 0.0f, -3.0f)),
	m_target(sm::Vector3(0.0f, 0.0f, 0.0f)),
	m_up(sm::Vector3::Up),
	m_view(sm::Matrix::Identity),
	m_projection(sm::Matrix::Identity),
	m_viewAngle(XMConvertToRadians(angle)),
	m_width(clientWidth),
	m_height(clientHeight),
	m_nearPlane(nearPlane),
	m_farPlane(farPlane)
{
}

Camera::~Camera()
{
}

void Camera::Update(double dt, const DirectX::Keyboard::State& kb, const DirectX::Mouse::State& mouse)
{
	Move(dt);
	Rotate(dt);

	m_view = XMMatrixLookAtLH(m_position, m_target, m_up);
	m_projection = XMMatrixPerspectiveFovLH(m_viewAngle, m_width / m_height, m_nearPlane, m_farPlane);
}

void Camera::Move(double dt)
{
	
}

void Camera::Rotate(double dt)
{
}
