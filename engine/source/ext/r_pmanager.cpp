#include "ext/primitives/r_particles.h"

NAMESPACE_BEGIN(r)

ParticleManager::ParticleManager()
{
  nPS=0;
  tX=tY=0.0f;
}

ParticleManager::~ParticleManager()
{
  for(int i = 0; nPS != i;++i)
    delete psList[i];
}

void ParticleManager::Update(float dt)
{
  for(int i = 0; nPS != i;++i)
  {
    psList[i]->Update(dt);
    if(psList[i]->GetAge()==-2.0f && psList[i]->GetParticlesAlive()==0)
    {
      delete psList[i];
      psList[i]=psList[nPS-1];
      nPS--;
      i--;
    }
  }
}

void ParticleManager::Render(r::Render& r)
{
  for(int i = 0; nPS != i;++i)
    psList[i]->Render(r);
}

ParticleSystem* ParticleManager::SpawnPS(const ParticleSystemInfo& psi, float x, float y)
{
  if(nPS == MAX_PSYSTEMS)
    return 0;
  psList[nPS] = new ParticleSystem(psi);
  psList[nPS]->FireAt(x,y);
  psList[nPS]->Transpose(tX,tY);
  nPS++;
  return psList[nPS-1];
}

bool ParticleManager::IsPSAlive(ParticleSystem *ps) const
{
  for(int i = 0; nPS != i;++i)
    if(psList[i] == ps)
      return true;
  return false;
}

void ParticleManager::Transpose(float x, float y)
{
  for(int i = 0; nPS != i;++i)
    psList[i]->Transpose(x,y);
  tX=x; tY=y;
}

void ParticleManager::KillPS(ParticleSystem *ps)
{
  int i;
  for(i=0;i<nPS;i++)
  {
    if(psList[i]==ps)
    {
      delete psList[i];
      psList[i]=psList[nPS-1];
      nPS--;
      return;
    }
  }
}

void ParticleManager::KillAll()
{
  int i;
  for(i=0;i<nPS;i++) delete psList[i];
  nPS=0;
}

NAMESPACE_END(r)