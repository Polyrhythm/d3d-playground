#include "cameraclass.h"

CameraClass::CameraClass()
    : _positionX(0.0f)
    , _positionY(0.0f)
    , _positionZ(0.0f)
    , _rotationX(0.0f)
    , _rotationY(0.0f)
    , _rotationZ(0.0f)
{

}

CameraClass::CameraClass(const CameraClass& other)
{

}

CameraClass::~CameraClass()
{

}

void CameraClass::SetPosition(float x, float y, float z)
{
    _positionX = x;
    _positionY = y;
    _positionZ = z;
}

void CameraClass::SetRotation(float x, float y, float z)
{
    _rotationX = x;
    _rotationY = y;
    _rotationZ = z;
}

XMFLOAT3 CameraClass::GetPosition()
{
    return XMFLOAT3(_positionX, _positionY, _positionZ);
}

XMFLOAT3 CameraClass::GetRotation()
{
    return XMFLOAT3(_rotationX, _rotationY, _rotationZ);
}

void CameraClass::Render()
{
    XMFLOAT3 up, position, lookAt;
    XMVECTOR upVector, positionVector, lookAtVector;
    float yaw, pitch, roll;
    XMMATRIX rotationMatrix;

    up.x = 0.0f;
    up.y = 1.0f;
    up.z = 0.0f;
    upVector = XMLoadFloat3(&up);

    position.x = _positionX;
    position.y = _positionY;
    position.z = _positionZ;
    positionVector = XMLoadFloat3(&position);

    lookAt.x = 0.0f;
    lookAt.y = 0.0f;
    lookAt.z = 1.0f;
    lookAtVector = XMLoadFloat3(&lookAt);

    pitch = XMConvertToRadians(_rotationX);
    yaw = XMConvertToRadians(_rotationY);
    roll = XMConvertToRadians(_rotationZ);

    rotationMatrix = XMMatrixRotationRollPitchYaw(pitch, yaw, roll);

    lookAtVector = XMVector3TransformCoord(lookAtVector, rotationMatrix);
    upVector = XMVector2TransformCoord(upVector, rotationMatrix);

    lookAtVector = XMVectorAdd(positionVector, lookAtVector);

    _viewMatrix = XMMatrixLookAtLH(positionVector, lookAtVector, upVector);
}

void CameraClass::GetViewMatrix(XMMATRIX& viewMatrix)
{
    viewMatrix = _viewMatrix;
}