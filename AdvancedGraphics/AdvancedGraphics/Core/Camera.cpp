#include "pch.h"
#include "Camera.h"
#include "Core/Core.h"

#define SPEED_GAIN 0.00001f
#define MAX_SPEED 0.5f

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
	m_farPlane(farPlane),
	m_speed(0.025f),
	m_sensitivity(0.01f)
{
	// Create the view and projection matrices on creation
	UpdateMatrices();
}

Camera::~Camera()
{
}

void Camera::Update(double dt, const DirectX::Keyboard::State& kb, const DirectX::Mouse::State& mouse)
{
	if (mouse.positionMode == Mouse::MODE_RELATIVE)  // Only update camera when rmb is down
	{
		Move(dt, kb);
		Rotate(dt, mouse);
	}
	UpdateMatrices();
}

void Camera::UpdateMatrices()
{
	sm::Matrix translation = sm::Matrix::CreateTranslation(m_position);
	sm::Matrix rotation    = sm::Matrix::CreateFromYawPitchRoll(m_rotation);
	sm::Matrix camWorld    = rotation * translation;

	// View matrix is inverse of the world matrix
	m_view       = camWorld.Invert();
	m_projection = XMMatrixPerspectiveFovLH(m_viewAngle, m_width / m_height, m_nearPlane, m_farPlane);
}

void Camera::Move(double dt, const DirectX::Keyboard::State& kb)
{
	// Stores the amount/direction to move this frame
	sm::Vector3 move = sm::Vector3::Zero;
	float height = 0;

	if (kb.W)
		move.z += m_speed;
	if (kb.S)
		move.z -= m_speed;
	if (kb.A)
		move.x -= m_speed;
	if (kb.D)
		move.x += m_speed;

	if (kb.Q)
		height -= m_speed;
	if (kb.E)
		height += m_speed;

	sm::Quaternion q = sm::Quaternion::CreateFromYawPitchRoll(m_rotation);

	move = sm::Vector3::Transform(move, q);
	move *= m_speed;
	m_position += move;
	m_position.y += height * m_speed;
}

void Camera::Rotate(double dt, const DirectX::Mouse::State& mouse)
{
	
	m_rotation.y += mouse.x * m_sensitivity;
	m_rotation.x += mouse.y * m_sensitivity;

	float change = mouse.scrollWheelValue * SPEED_GAIN;
	m_speed += change;
	m_speed = std::clamp(m_speed, 0.0f, MAX_SPEED);
}
