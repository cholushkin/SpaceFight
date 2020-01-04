#include "core/io/io_base.h"
#include "core/res/res_base.h"
#include "ext/primitives/r_particles.h"
#include "ext/primitives/r_primitives.h"
#include "ext/math/mt_random.h"

using namespace res;
using namespace r;
using namespace mt;

NAMESPACE_BEGIN(r)

static mt::fast_random_generator g_rnd;

inline f32 Rand(f32 fmin, f32 fmax)
{
  if(fmin < fmax)
    return fmin + g_rnd.frand() * (fmax - fmin);
  else
    return fmax + g_rnd.frand() * (fmin - fmax);
}

inline f32 RandMinMax(f32 fmin, f32 fmax)
{
  return fmin + g_rnd.frand() * (fmax - fmin);
}

#define	BLEND_COLORADD		1
#define	BLEND_COLORMUL		0
#define	BLEND_ALPHABLEND	2
#define	BLEND_ALPHAADD		0

struct HGEPSInfo
{
  int      flags;
  int      nEmission; // particles per sec
  float    fLifetime;

  float    fParticleLifeMin;
  float    fParticleLifeMax;

  float    fDirection;
  float    fSpread;
  bool     bRelative;

  float    fSpeedMin;
  float    fSpeedMax;

  float    fGravityMin;
  float    fGravityMax;

  float    fRadialAccelMin;
  float    fRadialAccelMax;

  float    fTangentialAccelMin;
  float    fTangentialAccelMax;

  float    fSizeStart;
  float    fSizeEnd;
  float    fSizeVar;

  float    fSpinStart;
  float    fSpinEnd;
  float    fSpinVar;

  mt::Vector4<float> colColorStart; // + alpha
  mt::Vector4<float> colColorEnd;
  float    fColorVar;
  float    fAlphaVar;
};

bool LoadPS(ParticleSystemInfo& psi, const char* szName)
{
  io::FileStream f;
  if(!f.Open(szName, true, true))
    return false;
  HGEPSInfo hpsi;
  f.Read((char*)&hpsi, sizeof(hpsi));
  psi.nEmission = hpsi.nEmission;

  psi.fLifetime = hpsi.fLifetime;

  psi.fParticleLifeMin = mt::Min(hpsi.fParticleLifeMin, hpsi.fParticleLifeMax);
  psi.fParticleLifeMax = mt::Max(hpsi.fParticleLifeMin, hpsi.fParticleLifeMax);

  psi.fDirection = hpsi.fDirection;
  psi.fSpread    = hpsi.fSpread;
  psi.bRelative  = hpsi.bRelative;

  psi.fSpeedMin = mt::Min(hpsi.fSpeedMin, hpsi.fSpeedMax);
  psi.fSpeedMax = mt::Max(hpsi.fSpeedMin, hpsi.fSpeedMax);

  psi.fGravityMin = mt::Min(hpsi.fGravityMin, hpsi.fGravityMax);
  psi.fGravityMax = mt::Max(hpsi.fGravityMin, hpsi.fGravityMax);

  psi.fRadialAccelMin = mt::Min(hpsi.fRadialAccelMin, hpsi.fRadialAccelMax);
  psi.fRadialAccelMax = mt::Max(hpsi.fRadialAccelMin, hpsi.fRadialAccelMax);

  psi.fTangentialAccelMin = mt::Min(hpsi.fTangentialAccelMin, hpsi.fTangentialAccelMax);
  psi.fTangentialAccelMax = mt::Max(hpsi.fTangentialAccelMin, hpsi.fTangentialAccelMax);

  psi.fSizeStart = hpsi.fSizeStart;
  psi.fSizeEnd = hpsi.fSizeEnd;
  psi.fSizeVar = hpsi.fSizeVar;

  psi.fSpinStart = hpsi.fSpinStart;
  psi.fSpinEnd = hpsi.fSpinEnd;
  psi.fSpinVar = hpsi.fSpinVar;

  psi.colColorStart = hpsi.colColorStart * 255.0f;
  psi.colColorEnd   = hpsi.colColorEnd   * 255.0f;
  psi.fColorVar = hpsi.fColorVar;
  psi.fAlphaVar = hpsi.fAlphaVar;
  if(0 == (hpsi.flags & (BLEND_ALPHABLEND << 16)))
    psi.m.m_blend = Material::bmAdditive;
  return true;
}

bool LoadPS(ParticleSystemInfo& psi, ResID psid, ResID texid, Render& r, ResourcesPool& pool)
{
  const char* name = pool.GetNameFromID(psid);
  EASSERT(NULL != name);
  if(NULL == name)
    return false;
  if(!r::LoadPS(psi, name))
    return false;
  psi.m.m_pTexture = r.GetTexture(texid, pool);
  EASSERT(NULL != psi.m.m_pTexture);
  return  NULL != psi.m.m_pTexture;
}

ParticleSystem::ParticleSystem(const ParticleSystemInfo& psi)
 : vecGravity(0.0f, 1.0f)
 , vecLocation(0.0f, 0.0f)
 , vecPrevLocation(0.0f, 0.0f)
 , rSpawn(-2.0f, 2.0f,-2.0f, 2.0f)
{
  memcpy(&info, &psi, sizeof(ParticleSystemInfo));
  fTx=fTy=0;
  fScale = 1.0f;
  fEmissionResidue=0.0f;
  nParticlesAlive=0;
  fAge=-2.0;
}

void ParticleSystem::Update(float fDeltaTime)
{
  int i;
  float ang;
  Particle *par;
  mt::v2f vecAccel, vecAccel2;

  if(fAge >= 0)
  {
    fAge += fDeltaTime;
    if(fAge >= info.fLifetime) fAge = -2.0f;
  }

  // update all alive particles
  par = particles;

  for(i=0; i<nParticlesAlive; i++)
  {
    par->fAge += fDeltaTime;
    if(par->fAge >= par->fTerminalAge)
    {
      nParticlesAlive--;
      memcpy(par, &particles[nParticlesAlive], sizeof(Particle));
      i--;
      continue;
    }

    vecAccel = par->vecLocation-vecLocation;
    normalize_fast(vecAccel);
    vecAccel2 = vecAccel;
    vecAccel *= par->fRadialAccel;

    // vecAccel2.Rotate(M_PI_2);
    // the following is faster
    ang = vecAccel2.x;
    vecAccel2.x = -vecAccel2.y;
    vecAccel2.y = ang;

    vecAccel2 *= par->fTangentialAccel;
    par->vecVelocity += (vecAccel+vecAccel2)*fDeltaTime;
    par->vecVelocity += vecGravity * par->fGravity*fDeltaTime;

    par->vecLocation += par->vecVelocity*fDeltaTime;

    par->fSpin += par->fSpinDelta*fDeltaTime;
    par->fSize += par->fSizeDelta*fDeltaTime;

    par->colColor += par->colColorDelta * fDeltaTime;

    par++;
  }

  // generate new particles

  if(fAge != -2.0f)
  {
    float fParticlesNeeded = info.nEmission*fDeltaTime + fEmissionResidue;
    int nParticlesCreated = (unsigned int)fParticlesNeeded;
    fEmissionResidue=fParticlesNeeded-nParticlesCreated;

    par=&particles[nParticlesAlive];

    const v4f col_end  = info.colColorStart + (info.colColorEnd - info.colColorStart) * info.fColorVar;
    const f32     size_end = info.fSizeStart+(info.fSizeEnd-info.fSizeStart) * info.fSizeVar;
    const f32     spin_end = info.fSpinStart+(info.fSpinEnd-info.fSpinStart) * info.fSpinVar;
    
    const v2f movedelta = vecLocation - vecPrevLocation;
    const f32     direction = (-movedelta).angle();

    for(i=0; i<nParticlesCreated; ++i)
    {
      if(nParticlesAlive >= MAX_PARTICLES)
        break;

      par->fAge = 0.0f;
      par->fTerminalAge = RandMinMax(info.fParticleLifeMin, info.fParticleLifeMax);
      const f32 recip_tAge = 1.0f / par->fTerminalAge;

      par->vecLocation = vecPrevLocation + movedelta * g_rnd.frand();
      par->vecLocation.x += RandMinMax(rSpawn.left, rSpawn.right);
      par->vecLocation.y += RandMinMax(rSpawn.top,  rSpawn.bottom);

      ang = info.fDirection - (float)M_PI_2 + Rand(0,info.fSpread) - info.fSpread * 0.5f;
      if(info.bRelative)
        ang += direction + (float)M_PI_2;
      sin_cos_low(ang, par->vecVelocity.y, par->vecVelocity.x);
      par->vecVelocity *= RandMinMax(info.fSpeedMin, info.fSpeedMax);

      par->fGravity         = RandMinMax(info.fGravityMin,         info.fGravityMax);
      par->fRadialAccel     = RandMinMax(info.fRadialAccelMin,     info.fRadialAccelMax);
      par->fTangentialAccel = RandMinMax(info.fTangentialAccelMin, info.fTangentialAccelMax);

      par->fSize = Rand(info.fSizeStart, size_end);
      par->fSizeDelta = (info.fSizeEnd-par->fSize) * recip_tAge;

      par->fSpin = Rand(info.fSpinStart, spin_end);
      par->fSpinDelta = (info.fSpinEnd-par->fSpin) * recip_tAge;

      par->colColor.r = Rand(info.colColorStart.r, col_end.r);
      par->colColor.g = Rand(info.colColorStart.g, col_end.g);
      par->colColor.b = Rand(info.colColorStart.b, col_end.b);
      par->colColor.a = Rand(info.colColorStart.a, col_end.a);

      par->colColorDelta = (info.colColorEnd - par->colColor) * recip_tAge;

      ++nParticlesAlive;
      ++par;
    }
  }
  vecPrevLocation=vecLocation;
}

void ParticleSystem::SetGravity(f32 x, f32 y)
{
  vecGravity = v2f(x, y);
}

void ParticleSystem::SetSpawnRect(const mt::Rectf& r)
{
  rSpawn = r;
}

void ParticleSystem::MoveTo(float x, float y, bool bMoveParticles)
{
  int i;
  float dx,dy;
  
  if(bMoveParticles)
  {
    dx=x-vecLocation.x;
    dy=y-vecLocation.y;

    for(i=0;i<nParticlesAlive;i++)
    {
      particles[i].vecLocation.x += dx;
      particles[i].vecLocation.y += dy;
    }

    vecPrevLocation.x=vecPrevLocation.x + dx;
    vecPrevLocation.y=vecPrevLocation.y + dy;
  }
  else
  {
    if(fAge==-2.0) { vecPrevLocation.x=x; vecPrevLocation.y=y; }
    else { vecPrevLocation.x=vecLocation.x;  vecPrevLocation.y=vecLocation.y; }
  }

  vecLocation.x=x;
  vecLocation.y=y;
}

void ParticleSystem::FireAt(float x, float y)
{
  Stop();
  MoveTo(x,y);
  Fire();
}

void ParticleSystem::Fire()
{
  if(info.fLifetime==-1.0f) fAge=-1.0f;
  else fAge=0.0f;
}

void ParticleSystem::Stop(bool bKillParticles)
{
  fAge = -2.0f;
  if(bKillParticles) 
    nParticlesAlive=0;
}

void ParticleSystem::Render(::Render& r, float dx, float dy)
{
  const Particle* par = particles;
  const f32 scale_size_premult = fScale * 0.5f * 32.0f;
  for(int i=0; i<nParticlesAlive; ++i, ++par)
  {
    if(par->colColor.a < 1.0f) // will zero out in ARGB
      continue;
    const f32 size = par->fSize * scale_size_premult;
    if(size <= 0.5f) // less than pixel
      continue;
    const u32 color = ARGB(flt_to_byte_as_uint(par->colColor.a), flt_to_byte_as_uint(par->colColor.r), flt_to_byte_as_uint(par->colColor.g), flt_to_byte_as_uint(par->colColor.b));
    const u32 colors[4] = {color, color, color, color};
    const f32 x = par->vecLocation.x*fScale+fTx + dx;
    const f32 y = par->vecLocation.y*fScale+fTy + dy;
    const f32 lx = x - size; const f32 hx = x + size;
    const f32 ly = y - size; const f32 hy = y + size;
    const v2f coords[] = 
    {
      v2f(lx, ly),
      v2f(hx, ly),
      v2f(hx, hy),
      v2f(lx, hy)
    };
    // $$$ Add rotation
    //info.sprite->RenderEx(par->vecLocation.x*fScale+fTx, par->vecLocation.y*fScale+fTy, par->fSpin*par->fAge, par->fSize*fScale);
    r.PostQuad(coords, g_def_uv, colors, info.m);
  }
}

void ParticleSystem::Render(::Render& r)
{
  Render(r, 0.0f, 0.0f);
}
NAMESPACE_END(r)