/********************************************************************
created:	2005/02/27
created:	27:2:2005   21:02
filename: 	c:\Projects\FlyX\include\math\mt_camera.h
file path:	c:\Projects\FlyX\include\math
file base:	mt_camera
file ext:	h
author:		Ctor

purpose:	FlyX camera. Methods are inlined to get more performance
*********************************************************************/
#ifndef _MT_CAMERA_H_
#define _MT_CAMERA_H_

#include "core/math/mt_base.h"

NAMESPACE_BEGIN(mt)
//  Comment this line to disable camera basis validation
#define CAMERA_BASIS_VALIDATION

class Camera
{
public:
  Camera();

  const mt::Matrix4f& GetProjMatrix() const;
  const mt::Matrix4f& GetViewMatrix() const;

  const mt::Vector3f& GetPos() const
  {
    return m_vecPos;
  }

  void SetNP(float np)
  {
    //  Sets camera position
    m_fNearPlane = np;
    m_bProjMatrixValid = false;
  }
  void SetFP(float fp)
  {
    //  Sets camera position
    m_fFarPlane = fp;
    m_bProjMatrixValid = false;
  }


  void SetPos(const mt::Vector3f& pos)
  {
    //  Sets camera position
    m_vecPos = pos;
    m_bViewMatrixValid = false;
  }

  void MoveForward( float dist )
  {
    //  Moves forward and backward
    m_vecPos += (m_vecForward * dist);
    m_bViewMatrixValid = false;
  }

  void MoveRight( float dist )
  {
    //  Moves right and left
    m_vecPos += (m_vecRight*dist);
    m_bViewMatrixValid = false;
  }

  void MoveUp( float dist )
  {
    //  Moves up and down
    m_vecPos += (m_vecUp*dist);
    m_bViewMatrixValid = false;
    m_bProjMatrixValid = false;
  }

  //  Performing camera basis validation
#ifdef CAMERA_BASIS_VALIDATION
#define VALIDATE_VECS\
  m_vecRight = m_vecForward.crossProduct(m_vecUp);\
  m_vecUp = m_vecRight.crossProduct(m_vecForward);\
  m_vecUp.normalize();\
  m_vecForward.normalize();\
  m_vecRight.normalize();
#else
#define VALIDATE_VECS
#endif


  void LookAt( const mt::Vector3f& pos, const mt::Vector3f& up )
  {
    m_vecForward = pos - m_vecPos;
    m_vecUp = up;
    m_vecRight = m_vecForward.crossProduct(m_vecUp);
    m_vecForward.normalize();
    m_vecRight.normalize();
    m_vecUp.normalize();
    m_bViewMatrixValid = false;
    m_bProjMatrixValid = false;
  }

  void LookAt( const mt::Vector3f& pos )
  {
    m_vecForward = pos - m_vecPos;
    m_vecRight = m_vecForward.crossProduct(m_vecUp);
    m_vecUp = m_vecRight.crossProduct(m_vecForward);
    m_vecForward.normalize();
    m_vecRight.normalize();
    m_vecUp.normalize();
    m_bViewMatrixValid = false;
    m_bProjMatrixValid = false;
  }

  void RotateRight( float angle ) {             //  Yaws camera
    mt::Quaternionf q = mt::Quaternionf::fromAxisRot(m_vecUp, angle);
    mt::Matrix3f m = q.rotMatrix();
    m_vecUp = m * m_vecUp;
    m_vecRight = m * m_vecRight;
    m_vecForward = m * m_vecForward;
    m_bViewMatrixValid = false;
  }

  //void RotateUp(float angle) {                  //  Pitches camera
  //  Matrix4f matRot;
  //  matRot.set_rot(-angle, m_vecRight);
  //  mt::mult_dir(m_vecUp, matRot, Vector3f(m_vecUp));
  //  mt::mult_dir(m_vecForward, matRot, Vector3f(m_vecForward));
  //  VALIDATE_VECS;
  //  m_bViewMatrixValid = false;
  //  m_bFrustumValid = false;
  //}

  //void RotateCosmonaut(float angle) {           //  Rolls camera
  //  Matrix4f matRot;
  //  matRot.set_rot(-angle, m_vecForward);
  //  mt::mult_dir(m_vecUp, matRot, Vector3f(m_vecUp));
  //  mt::mult_dir(m_vecRight, matRot, Vector3f(m_vecRight));
  //  VALIDATE_VECS
  //    m_bViewMatrixValid = false;
  //  m_bFrustumValid = false;
  //}
#undef VALIDATE_VECS

  void SetFOV( float fov )
  {
    //  Sets vertical FOV in degrees
    if( fov != m_fFOV ) 
    {
      m_fFOV = fov;
      m_bProjMatrixValid = false;
    }
  }

  void SetAspect( float aspect ) 
  {
    if( m_fAspect != aspect ) 
    {
      m_fAspect = aspect;
      m_bProjMatrixValid = false;
    }
  }

  void SetViewPort( size_t width, size_t height ) 
  {
    //  We are setting aspect instead
    SetAspect( (float)width / (float)height );
  }

  void Project(float x, float y, mt::Vector3f& res) const
  {
    const mt::Matrix4f& matView = GetViewMatrix();
    const mt::Matrix4f& matProj = GetProjMatrix();

    // Compute the vector of the pick ray in screen space
    mt::Vector3f v;
    v.x =  ( ( ( 2.0f * x ) ) - 1 ) / matProj.at(0,0);
    v.y = -( ( ( 2.0f * y ) ) - 1 ) / matProj.at(1,1);
    v.z = -1.0f;

    // Get the inverse view matrix
    mt::Matrix4f m = matView.inverse();
    // Transform the screen space pick ray into 3D space
    res = m * v;
    res.normalize();
  }

  const mt::v3f& GetDirection() const { return m_vecForward; };

private:
  f32 m_fFOV;           //  Vertical field of view
  f32 m_fAspect;        //  Aspect ratio
  f32 m_fNearPlane;     //  Near clipping plane
  f32 m_fFarPlane;      //  Far clipping plane

  mt::v3f m_vecForward;
  mt::v3f m_vecRight;
  mt::v3f m_vecUp;
  mt::v3f m_vecPos;

  mutable bool m_bViewMatrixValid;
  mutable bool m_bProjMatrixValid;

  mutable mt::Matrix4f m_viewMatrix;     //  View matrix
  mutable mt::Matrix4f m_projMatrix;     //  Perspective projection matrix
};

NAMESPACE_END(mt)

#endif    //  _MT_CAMERA_H_
