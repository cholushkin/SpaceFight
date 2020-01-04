#ifndef R_PARTICLE_H
#define R_PARTICLE_H

#include "core/common/com_misc.h"
#include "core/render/r_render.h"
#include "core/res/res_base.h"

NAMESPACE_BEGIN(r)

struct Particle
{
  mt::v2f vecLocation;
  mt::v2f vecVelocity;

  f32 fGravity;
  f32 fRadialAccel;
  f32 fTangentialAccel;

  f32 fSpin;
  f32 fSpinDelta;

  f32 fSize;
  f32 fSizeDelta;

  mt::v4f colColor;
  mt::v4f colColorDelta;

  f32 fAge;
  f32 fTerminalAge;
};

struct ParticleSystemInfo
{
  Material m;    // texture + blend mode
  int  nEmission; // particles per sec
  f32  fLifetime;

  f32  fParticleLifeMin;
  f32  fParticleLifeMax;

  f32  fDirection;
  f32  fSpread;
  bool bRelative;

  f32  fSpeedMin;
  f32  fSpeedMax;

  f32  fGravityMin;
  f32  fGravityMax;

  f32  fRadialAccelMin;
  f32  fRadialAccelMax;

  f32  fTangentialAccelMin;
  f32  fTangentialAccelMax;

  f32  fSizeStart;
  f32  fSizeEnd;
  f32  fSizeVar;

  f32  fSpinStart;
  f32  fSpinEnd;
  f32  fSpinVar;

  mt::v4f  colColorStart; // + alpha
  mt::v4f  colColorEnd;
  f32  fColorVar;
  f32  fAlphaVar;
};

class ParticleSystem
{
public:
  ParticleSystem(const ParticleSystemInfo&);

  void    Render(r::Render& r);
  void    Render(r::Render& r, float dx, float dy);
  void    FireAt(float x, float y);
  void    Fire();
  void    Stop(bool bKillParticles=false);
  void    Update(float fDeltaTime);
  void    MoveTo(float x, float y, bool bMoveParticles=false);
  void    Transpose(float x, float y) { fTx=x; fTy=y; }
  void    SetScale(float scale) { fScale = scale; }
  void    SetGravity(f32 x, f32 y);
  void    SetSpawnRect(const mt::Rectf& r);

  int     GetParticlesAlive() const { return nParticlesAlive; }
  f32     GetAge() const { return fAge; }
  void    GetPosition(float *x, float *y) const { *x=vecLocation.x; *y=vecLocation.y; }
  void    GetTransposition(float *x, float *y) const { *x=fTx; *y=fTy; }
  f32     GetScale() { return fScale; }

private:
  ParticleSystemInfo info;

  f32     fAge;
  f32     fEmissionResidue;

  mt::v2f vecPrevLocation;
  mt::v2f vecLocation;
  mt::v2f vecGravity;
  f32     fTx, fTy;
  f32     fScale;

  int     nParticlesAlive;

  mt::Rectf rSpawn;

  const static int MAX_PARTICLES = 500;
  Particle particles[MAX_PARTICLES];
};

class ParticleManager
{
public:
  ParticleManager();
  ~ParticleManager();

  void        Update(f32 dt);
  void        Render(r::Render& r);

  ParticleSystem*  SpawnPS(const ParticleSystemInfo& psi, f32 x, f32 y);
  bool        IsPSAlive(ParticleSystem *ps) const;
  void        Transpose(f32 x, f32 y);
  void        GetTransposition(f32 *dx, f32 *dy) const {*dx=tX; *dy=tY;}
  void        KillPS(ParticleSystem *ps);
  void        KillAll();

private:
  ParticleManager(const ParticleManager &);
  ParticleManager&  operator= (const ParticleManager &);

  int          nPS;
  f32        tX;
  f32        tY;

  const static int MAX_PSYSTEMS = 100;
  ParticleSystem*  psList[MAX_PSYSTEMS];
};

bool LoadPS(ParticleSystemInfo& psi, const char* szName);
 // Note: ParticleSystemInfo by itself is not a managed resource
bool LoadPS(ParticleSystemInfo& psi, res::ResID psid, res::ResID texid, Render& r, res::ResourcesPool& pool);

NAMESPACE_END(r)

#endif // HGEPARTICLE_H
