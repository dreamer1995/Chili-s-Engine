#include "Camera.h"
#include "imgui/imgui.h"
#include "ChiliMath.h"

namespace dx = DirectX;

Camera::Camera() noexcept
{
	Reset();
}

DirectX::XMMATRIX Camera::GetMatrix() const noexcept
{
	using namespace dx;

	const dx::XMVECTOR forwardBaseVector = XMVectorSet( 0.0f,0.0f,1.0f,0.0f );
	// apply the camera rotations to a base vector
	const auto lookVector = XMVector3Transform( forwardBaseVector,
		XMMatrixRotationRollPitchYaw( pitch,yaw,0.0f )
	);
	// generate camera transform (applied to all objects to arrange them relative
	// to camera position/orientation in world) from cam position and direction
	// camera "top" always faces towards +Y (cannot do a barrel roll)
	const auto camPosition = XMLoadFloat3( &pos );
	const auto camTarget = camPosition + lookVector;
	XMVECTOR upVector;
	if (-PI / 2.0f >= pitch || pitch >= PI / 2.0f)
	{
		upVector = XMVectorSet(0.0f, -1.0f, 0.0f, 0.0f);
	}
	else
	{
		upVector = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	}
	return XMMatrixLookAtLH(camPosition,camTarget, upVector);
}

void Camera::SpawnControlWindow() noexcept
{
	if( ImGui::Begin( "Camera" ) )
	{
		ImGui::Text( "Position" );
		ImGui::SliderFloat( "X",&pos.x,-80.0f,80.0f,"%.1f" );
		ImGui::SliderFloat( "Y",&pos.y,-80.0f,80.0f,"%.1f" );
		ImGui::SliderFloat( "Z",&pos.z,-80.0f,80.0f,"%.1f" );
		ImGui::Text( "Orientation" );
		ImGui::SliderAngle( "Pitch",&pitch,-180.0f,180.0f );
		ImGui::SliderAngle( "Yaw",&yaw,-180.0f,180.0f );
		if( ImGui::Button( "Reset" ) )
		{
			Reset();
		}
	}
	ImGui::End();
}

void Camera::Reset() noexcept
{
	pos = { 0.0f,7.5f,-18.0f };
	pitch = 0.0f;
	yaw = 0.0f;
	yaw_ = 0;
}

void Camera::LookZero(DirectX::XMFLOAT3 position) noexcept
{
	DirectX::XMFLOAT3 delta = { pos.x - position.x,pos.y - position.y, pos.z - position.z };
	pitch = wrap_angle(atan2(delta.y, sqrt(delta.x * delta.x + delta.z * delta.z)));
	yaw = wrap_angle(atan2(delta.x, delta.z) + PI);
	yaw_ = wrap_angle(atan2(delta.x, delta.z) + PI);
}

void Camera::KeepLookFront(DirectX::XMFLOAT3 position) noexcept
{
	DirectX::XMFLOAT3 delta = { pos.x - position.x,pos.y - position.y, pos.z - position.z };
	if (-PI / 2.0f >= pitch || pitch >= PI / 2.0f)
	{
		if (0.3 * PI < abs(yaw_ - wrap_angle(atan2(delta.x, delta.z))) && abs(yaw_ - wrap_angle(atan2(delta.x, delta.z))) < 0.9 * PI * 2)
		{
			pitch = wrap_angle(atan2(delta.y, sqrt(delta.x * delta.x + delta.z * delta.z)));
			yaw = wrap_angle(atan2(delta.x, delta.z) + PI);
			yaw_ = yaw;
		}
		else
		{
			pitch = wrap_angle(-atan2(delta.y, sqrt(delta.x * delta.x + delta.z * delta.z)) - PI);
			yaw = wrap_angle(atan2(delta.x, delta.z));
			yaw_ = yaw;
		}
	}
	else
	{
		if (0.3 * PI < abs(yaw_ - wrap_angle(atan2(delta.x, delta.z) + PI)) && abs(yaw_ - wrap_angle(atan2(delta.x, delta.z) + PI)) < 0.9 * PI * 2)
		{
			pitch = wrap_angle(-atan2(delta.y, sqrt(delta.x * delta.x + delta.z * delta.z)) - PI);
			yaw = wrap_angle(atan2(delta.x, delta.z));
			yaw_ = yaw;
		}
		else
		{
			pitch = wrap_angle(atan2(delta.y, sqrt(delta.x*delta.x + delta.z* delta.z)));
			yaw = wrap_angle(atan2(delta.x, delta.z) + PI);
			yaw_ = yaw;
		}
	}
}

void Camera::Rotate( float dx,float dy ) noexcept
{
	if (pitch <= -PI / 2.0f || pitch > PI / 2.0f)
	{
		dx = -dx;
	}
	yaw = wrap_angle( yaw + dx * rotationSpeed );
	pitch = wrap_angle(pitch + dy * rotationSpeed);
}

void Camera::RotateAround(float dx, float dy, DirectX::XMFLOAT3 centralPoint) noexcept
{
	using namespace dx;
	// Rotate camera around a point
	{
		DirectX::XMFLOAT3 lookVector, destination;
		XMVECTOR rotateVector = XMVectorSubtract(XMLoadFloat3(&pos), XMLoadFloat3(&centralPoint));
		dx::XMStoreFloat3(&lookVector, XMVector3Transform(XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f), XMMatrixRotationRollPitchYaw(pitch, yaw, 0.0f)));
		XMFLOAT3 finalRatationViewVector;
		XMStoreFloat3(&finalRatationViewVector, dx::XMVector3Transform(rotateVector,
			dx::XMMatrixTranslation(lookVector.x, lookVector.y, lookVector.z) *
			XMMatrixRotationQuaternion(XMQuaternionRotationAxis(XMVector3Transform(XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f),
				XMMatrixRotationRollPitchYaw(0, yaw, 0.0f)), dy * rotationSpeed))
			* XMMatrixRotationQuaternion(XMQuaternionRotationAxis(XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f), dx * rotationSpeed))
		));
		dx::XMStoreFloat3(&destination,
			XMVector3Transform(XMLoadFloat3(&centralPoint), XMMatrixTranslation(finalRatationViewVector.x, finalRatationViewVector.y, finalRatationViewVector.z)));
		XMFLOAT3 finalRatationVector;
		XMStoreFloat3(&finalRatationVector, XMVector3Transform(rotateVector,
			XMMatrixRotationQuaternion(XMQuaternionRotationAxis(XMVector3Transform(XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f),
				XMMatrixRotationRollPitchYaw(0, yaw, 0.0f)), dy * rotationSpeed))
			* XMMatrixRotationQuaternion(XMQuaternionRotationAxis(XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f), dx * rotationSpeed))
		));
		XMStoreFloat3(&pos,
			XMVector3Transform(XMLoadFloat3(&centralPoint), XMMatrixTranslation(finalRatationVector.x, finalRatationVector.y, finalRatationVector.z)));
		KeepLookFront(destination);
	}
}

void Camera::Translate( DirectX::XMFLOAT3 translation ) noexcept
{
	dx::XMStoreFloat3( &translation,dx::XMVector3Transform(
		dx::XMLoadFloat3( &translation ),
		dx::XMMatrixRotationRollPitchYaw( pitch,yaw,0.0f ) *
		dx::XMMatrixScaling( travelSpeed,travelSpeed,travelSpeed )
	) );
	pos = {
		pos.x + translation.x,
		pos.y + translation.y,
		pos.z + translation.z
	};
}
