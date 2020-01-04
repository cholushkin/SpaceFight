#include "ext/patterns/pat_observer.h"
#include "platform.h"
#include <algorithm>

NAMESPACE_BEGIN(pat)

bool Observable::AddObserver( IObserver* obs ) {
  EASSERT( obs );
  if(std::find(m_observers.begin(),m_observers.end(),obs)==m_observers.end()) {
    m_observers.push_back(obs);
    return true;
  }
  return false;
}

bool Observable::RemoveObserver( IObserver* obs ) {
  EASSERT(obs);
  std::vector<IObserver*>::iterator todel = std::find(m_observers.begin(),m_observers.end(),obs);
  if(todel == m_observers.end())
    return false;
  m_observers.erase(todel);
  return true;
}

u32  Observable::GetObserverCount() const {
  return (u32)m_observers.size();
}

bool Observable::NotifyObservers( Event* evnt ) {
  EASSERT(evnt);
  bool res = false;
  std::vector<IObserver*>::iterator iter;
  for(iter = m_observers.begin(); iter != m_observers.end(); ++iter ) {
    (*iter)->OnEvent( evnt );
    res =  true;
  }
  return res;
}

NAMESPACE_END(pat)