/********************************************************************
created:	2005/02/27
created:	27:2:2005   21:13
filename: 	c:\Projects\FlyX\source\math\mt_camera.cpp
file path:	c:\Projects\FlyX\source\math
file base:	mt_camera
file ext:	cpp
author:		Ctor

purpose:	FlyX Camera implementation
*********************************************************************/

#include "ext/math/mt_camera.h"

NAMESPACE_BEGIN(mt)

Camera::Camera()
: m_fFOV(45.0f)
, m_fAspect(4.0f / 3.0f)
, m_fNearPlane(0.1f)
, m_fFarPlane(200.0f)
//, m_viewMatrix(mt::array16_id)
, m_vecForward(v3f(0.0f,0.0f,-1.0f))
, m_vecRight  (v3f(1.0f,0.0f,0.0f))
, m_vecUp     (v3f(0.0f,1.0f,0.0f))
, m_vecPos    (v3f(0.0f,0.0f,0.0f))
, m_bViewMatrixValid(false)
, m_bProjMatrixValid(false)
{
}

const Matrix4f& Camera::GetProjMatrix() const {
  if(!m_bProjMatrixValid)
  {
    perspective(m_projMatrix, m_fFOV, m_fAspect, m_fNearPlane, m_fFarPlane);
    m_bProjMatrixValid= true;
  }
  return m_projMatrix;
}

const Matrix4f& Camera::GetViewMatrix() const {
  if(!m_bViewMatrixValid)
  {
    look_at(m_viewMatrix, m_vecPos, m_vecPos + m_vecForward, m_vecUp);
    m_bViewMatrixValid = true;
  }
  return m_viewMatrix;
}

NAMESPACE_END(mt)