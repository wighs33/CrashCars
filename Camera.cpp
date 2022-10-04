//***************************************************************************************
// Camera.h by Frank Luna (C) 2011 All Rights Reserved.
//***************************************************************************************
#include "stdafx.h"
#include "Camera.h"
#include "Player.h"

using namespace DirectX;

PlayerCamera::PlayerCamera()
{
	SetLens(0.25f*MathHelper::Pi, 1.0f, 1.0f, 1000.0f);



	//뷰행렬
	m_xmf4x4View = Matrix4x4::Identity();
	//투영행렬
	m_xmf4x4Projection = Matrix4x4::Identity();
	//뷰포트
	m_d3dViewport = { 0, 0, FRAME_BUFFER_WIDTH , FRAME_BUFFER_HEIGHT, 0.0f, 1.0f };
	//가위
	m_d3dScissorRect = { 0, 0, FRAME_BUFFER_WIDTH , FRAME_BUFFER_HEIGHT };
	//카메라위치
	m_xmf3Position = XMFLOAT3(0.0f, 0.0f, 0.0f);
	//카메라업룩라
	m_xmf3Right = XMFLOAT3(1.0f, 0.0f, 0.0f);
	m_xmf3Look = XMFLOAT3(0.0f, 0.0f, 1.0f);
	m_xmf3Up = XMFLOAT3(0.0f, 1.0f, 0.0f);
	//카메라피치롤요
	m_fPitch = 0.0f;
	m_fRoll = 0.0f;
	m_fYaw = 0.0f;
	//카메라 추가값
	m_xmf3Offset = XMFLOAT3(0.0f, 0.0f, 0.0f);
	// 장면 바뀌는 속도
	m_fTimeLag = 0.0f;
	// 
	m_xmf3LookAtWorld = XMFLOAT3(0.0f, 0.0f, 0.0f);
	//플레이어 포인터
	m_pPlayer = NULL;
}

PlayerCamera::~PlayerCamera()
{
}

XMVECTOR PlayerCamera::GetPosition()const
{
	return XMLoadFloat3(&m_xmf3Position);
}

XMFLOAT3 PlayerCamera::GetPosition3f()const
{
	return m_xmf3Position;
}

//void PlayerCamera::SetPosition(float x, float y, float z)
//{
//	m_xmf3Position = XMFLOAT3(x, y, z);
//	mViewDirty = true;
//}
//
//void PlayerCamera::SetPosition(const XMFLOAT3& v)
//{
//	m_xmf3Position = v;
//	mViewDirty = true;
//}

XMVECTOR PlayerCamera::GetRight()const
{
	return XMLoadFloat3(&m_xmf3Right);
}

XMFLOAT3 PlayerCamera::GetRight3f()const
{
	return m_xmf3Right;
}

XMVECTOR PlayerCamera::GetUp()const
{
	return XMLoadFloat3(&m_xmf3Up);
}

XMFLOAT3 PlayerCamera::GetUp3f()const
{
	return m_xmf3Up;
}

XMVECTOR PlayerCamera::GetLook()const
{
	return XMLoadFloat3(&m_xmf3Look);
}

XMFLOAT3 PlayerCamera::GetLook3f()const
{
	return m_xmf3Look;
}

float PlayerCamera::GetNearZ()const
{
	return mNearZ;
}

float PlayerCamera::GetFarZ()const
{
	return mFarZ;
}

float PlayerCamera::GetAspect()const
{
	return mAspect;
}

float PlayerCamera::GetFovY()const
{
	return mFovY;
}

float PlayerCamera::GetFovX()const
{
	float halfWidth = 0.5f*GetNearWindowWidth();
	return 2.0f*atan(halfWidth / mNearZ);
}

float PlayerCamera::GetNearWindowWidth()const
{
	return mAspect * mNearWindowHeight;
}

float PlayerCamera::GetNearWindowHeight()const
{
	return mNearWindowHeight;
}

float PlayerCamera::GetFarWindowWidth()const
{
	return mAspect * mFarWindowHeight;
}

float PlayerCamera::GetFarWindowHeight()const
{
	return mFarWindowHeight;
}

void PlayerCamera::SetLens(float fovY, float aspect, float zn, float zf)
{
	// cache properties
	mFovY = fovY;
	mAspect = aspect;
	mNearZ = zn;
	mFarZ = zf;

	mNearWindowHeight = 2.0f * mNearZ * tanf( 0.5f*mFovY );
	mFarWindowHeight  = 2.0f * mFarZ * tanf( 0.5f*mFovY );

	XMMATRIX P = XMMatrixPerspectiveFovLH(mFovY, mAspect, mNearZ, mFarZ);
	XMStoreFloat4x4(&m_xmf4x4Projection, P);
}

void PlayerCamera::LookAt(FXMVECTOR pos, FXMVECTOR target, FXMVECTOR worldUp)
{
	XMVECTOR L = XMVector3Normalize(XMVectorSubtract(target, pos));
	XMVECTOR R = XMVector3Normalize(XMVector3Cross(worldUp, L));
	XMVECTOR U = XMVector3Cross(L, R);

	XMStoreFloat3(&m_xmf3Position, pos);
	XMStoreFloat3(&m_xmf3Look, L);
	XMStoreFloat3(&m_xmf3Right, R);
	XMStoreFloat3(&m_xmf3Up, U);

	mViewDirty = true;
}

void PlayerCamera::LookAt(const XMFLOAT3& pos, const XMFLOAT3& target, const XMFLOAT3& up)
{
	XMVECTOR P = XMLoadFloat3(&pos);
	XMVECTOR T = XMLoadFloat3(&target);
	XMVECTOR U = XMLoadFloat3(&up);

	LookAt(P, T, U);

	mViewDirty = true;
}

XMMATRIX PlayerCamera::GetView()const
{
	assert(!mViewDirty);
	return XMLoadFloat4x4(&m_xmf4x4View);
}

XMMATRIX PlayerCamera::GetProj()const
{
	return XMLoadFloat4x4(&m_xmf4x4Projection);
}


XMFLOAT4X4 PlayerCamera::GetView4x4f()const
{
	assert(!mViewDirty);
	return m_xmf4x4View;
}

XMFLOAT4X4 PlayerCamera::GetProj4x4f()const
{
	return m_xmf4x4Projection;
}

void PlayerCamera::Strafe(float d)
{
	// mPosition += d*mRight
	XMVECTOR s = XMVectorReplicate(d);
	XMVECTOR r = XMLoadFloat3(&m_xmf3Right);
	XMVECTOR p = XMLoadFloat3(&m_xmf3Position);
	XMStoreFloat3(&m_xmf3Position, XMVectorMultiplyAdd(s, r, p));

	mViewDirty = true;
}

void PlayerCamera::Walk(float d)
{
	// mPosition += d*mLook
	XMVECTOR s = XMVectorReplicate(d);
	XMVECTOR l = XMLoadFloat3(&m_xmf3Look);
	XMVECTOR p = XMLoadFloat3(&m_xmf3Position);
	XMStoreFloat3(&m_xmf3Position, XMVectorMultiplyAdd(s, l, p));

	mViewDirty = true;
}

void PlayerCamera::Pitch(float angle)
{
	// Rotate up and look vector about the right vector.

	XMMATRIX R = XMMatrixRotationAxis(XMLoadFloat3(&m_xmf3Right), angle);

	XMStoreFloat3(&m_xmf3Up,   XMVector3TransformNormal(XMLoadFloat3(&m_xmf3Up), R));
	XMStoreFloat3(&m_xmf3Look, XMVector3TransformNormal(XMLoadFloat3(&m_xmf3Look), R));

	mViewDirty = true;
}

void PlayerCamera::RotateY(float angle)
{
	// Rotate the basis vectors about the world y-axis.

	XMMATRIX R = XMMatrixRotationY(angle);

	XMStoreFloat3(&m_xmf3Right,   XMVector3TransformNormal(XMLoadFloat3(&m_xmf3Right), R));
	XMStoreFloat3(&m_xmf3Up, XMVector3TransformNormal(XMLoadFloat3(&m_xmf3Up), R));
	XMStoreFloat3(&m_xmf3Look, XMVector3TransformNormal(XMLoadFloat3(&m_xmf3Look), R));

	mViewDirty = true;
}

void PlayerCamera::UpdateViewMatrix()
{
	if(mViewDirty)
	{
		XMVECTOR R = XMLoadFloat3(&m_xmf3Right);
		XMVECTOR U = XMLoadFloat3(&m_xmf3Up);
		XMVECTOR L = XMLoadFloat3(&m_xmf3Look);
		XMVECTOR P = XMLoadFloat3(&m_xmf3Position);

		// Keep camera's axes orthogonal to each other and of unit length.
		L = XMVector3Normalize(L);
		U = XMVector3Normalize(XMVector3Cross(L, R));

		// U, L already ortho-normal, so no need to normalize cross product.
		R = XMVector3Cross(U, L);

		// Fill in the view matrix entries.
		float x = -XMVectorGetX(XMVector3Dot(P, R));
		float y = -XMVectorGetX(XMVector3Dot(P, U));
		float z = -XMVectorGetX(XMVector3Dot(P, L));

		XMStoreFloat3(&m_xmf3Right, R);
		XMStoreFloat3(&m_xmf3Up, U);
		XMStoreFloat3(&m_xmf3Look, L);

		m_xmf4x4View(0, 0) = m_xmf3Right.x;
		m_xmf4x4View(1, 0) = m_xmf3Right.y;
		m_xmf4x4View(2, 0) = m_xmf3Right.z;
		m_xmf4x4View(3, 0) = x;

		m_xmf4x4View(0, 1) = m_xmf3Up.x;
		m_xmf4x4View(1, 1) = m_xmf3Up.y;
		m_xmf4x4View(2, 1) = m_xmf3Up.z;
		m_xmf4x4View(3, 1) = y;

		m_xmf4x4View(0, 2) = m_xmf3Look.x;
		m_xmf4x4View(1, 2) = m_xmf3Look.y;
		m_xmf4x4View(2, 2) = m_xmf3Look.z;
		m_xmf4x4View(3, 2) = z;

		m_xmf4x4View(0, 3) = 0.0f;
		m_xmf4x4View(1, 3) = 0.0f;
		m_xmf4x4View(2, 3) = 0.0f;
		m_xmf4x4View(3, 3) = 1.0f;

		mViewDirty = false;
	}
}












//labproject에서 가져옴
void PlayerCamera::SetViewport(int xTopLeft, int yTopLeft, int nWidth, int nHeight, float fMinZ, float fMaxZ)
{
	m_d3dViewport.TopLeftX = float(xTopLeft);
	m_d3dViewport.TopLeftY = float(yTopLeft);
	m_d3dViewport.Width = float(nWidth);
	m_d3dViewport.Height = float(nHeight);
	m_d3dViewport.MinDepth = fMinZ;
	m_d3dViewport.MaxDepth = fMaxZ;
}

void PlayerCamera::SetScissorRect(LONG xLeft, LONG yTop, LONG xRight, LONG yBottom)
{
	m_d3dScissorRect.left = xLeft;
	m_d3dScissorRect.top = yTop;
	m_d3dScissorRect.right = xRight;
	m_d3dScissorRect.bottom = yBottom;
}

void PlayerCamera::GenerateProjectionMatrix(float fNearPlaneDistance, float fFarPlaneDistance, float fAspectRatio, float fFOVAngle)
{
	//	m_xmf4x4Projection = Matrix4x4::PerspectiveFovLH(XMConvertToRadians(fFOVAngle), fAspectRatio, fNearPlaneDistance, fFarPlaneDistance);
	XMMATRIX xmmtxProjection = XMMatrixPerspectiveFovLH(XMConvertToRadians(fFOVAngle), fAspectRatio, fNearPlaneDistance, fFarPlaneDistance);
	XMStoreFloat4x4(&m_xmf4x4Projection, xmmtxProjection);
}

void PlayerCamera::GenerateViewMatrix(XMFLOAT3 xmf3Position, XMFLOAT3 xmf3LookAt, XMFLOAT3 xmf3Up)
{
	m_xmf3Position = xmf3Position;
	m_xmf3LookAtWorld = xmf3LookAt;
	m_xmf3Up = xmf3Up;

	GenerateViewMatrix();
}

void PlayerCamera::GenerateViewMatrix()
{
	m_xmf4x4View = Matrix4x4::LookAtLH(m_xmf3Position, m_xmf3LookAtWorld, m_xmf3Up);
}

void PlayerCamera::RegenerateViewMatrix()
{
	m_xmf3Look = Vector3::Normalize(m_xmf3Look);
	m_xmf3Right = Vector3::CrossProduct(m_xmf3Up, m_xmf3Look, true);
	m_xmf3Up = Vector3::CrossProduct(m_xmf3Look, m_xmf3Right, true);

	m_xmf4x4View._11 = m_xmf3Right.x; m_xmf4x4View._12 = m_xmf3Up.x; m_xmf4x4View._13 = m_xmf3Look.x;
	m_xmf4x4View._21 = m_xmf3Right.y; m_xmf4x4View._22 = m_xmf3Up.y; m_xmf4x4View._23 = m_xmf3Look.y;
	m_xmf4x4View._31 = m_xmf3Right.z; m_xmf4x4View._32 = m_xmf3Up.z; m_xmf4x4View._33 = m_xmf3Look.z;
	m_xmf4x4View._41 = -Vector3::DotProduct(m_xmf3Position, m_xmf3Right);
	m_xmf4x4View._42 = -Vector3::DotProduct(m_xmf3Position, m_xmf3Up);
	m_xmf4x4View._43 = -Vector3::DotProduct(m_xmf3Position, m_xmf3Look);
}


void PlayerCamera::Update(XMFLOAT3& xmf3LookAt, float fTimeElapsed)
{
		XMFLOAT4X4 xmf4x4Rotate = Matrix4x4::Identity();
		XMFLOAT3 xmf3Right = m_pPlayer->GetRightVector();
		XMFLOAT3 xmf3Up = m_pPlayer->GetUpVector();
		XMFLOAT3 xmf3Look = m_pPlayer->GetLookVector();
		xmf4x4Rotate._11 = xmf3Right.x; xmf4x4Rotate._21 = xmf3Up.x; xmf4x4Rotate._31 = xmf3Look.x;
		xmf4x4Rotate._12 = xmf3Right.y; xmf4x4Rotate._22 = xmf3Up.y; xmf4x4Rotate._32 = xmf3Look.y;
		xmf4x4Rotate._13 = xmf3Right.z; xmf4x4Rotate._23 = xmf3Up.z; xmf4x4Rotate._33 = xmf3Look.z;

		XMFLOAT3 xmf3Offset = Vector3::TransformCoord(m_xmf3Offset, xmf4x4Rotate);
		XMFLOAT3 xmf3Position = Vector3::Add(m_pPlayer->GetPosition(), xmf3Offset);
		XMFLOAT3 xmf3Direction = Vector3::Subtract(xmf3Position, m_xmf3Position);
		float fLength = Vector3::Length(xmf3Direction);
		xmf3Direction = Vector3::Normalize(xmf3Direction);
		float fTimeLagScale = (m_fTimeLag) ? fTimeElapsed * (1.0f / m_fTimeLag) : 1.0f;
		float fDistance = fLength * fTimeLagScale;
		if (fDistance > fLength) fDistance = fLength;
		if (fLength < 0.01f) fDistance = fLength;
		if (fDistance > 0)
		{
			m_xmf3Position = Vector3::Add(m_xmf3Position, xmf3Direction, fDistance);
			SetLookAt(xmf3LookAt);
		}
}

void PlayerCamera::SetLookAt(XMFLOAT3& xmf3LookAt)
{
	XMFLOAT4X4 mtxLookAt = Matrix4x4::LookAtLH(m_xmf3Position, xmf3LookAt, m_pPlayer->GetUpVector());
	m_xmf3Right = XMFLOAT3(mtxLookAt._11, mtxLookAt._21, mtxLookAt._31);
	m_xmf3Up = XMFLOAT3(mtxLookAt._12, mtxLookAt._22, mtxLookAt._32);
	m_xmf3Look = XMFLOAT3(mtxLookAt._13, mtxLookAt._23, mtxLookAt._33);
}