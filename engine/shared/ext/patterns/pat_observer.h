#ifndef observer_h__
#define observer_h__

#include <vector>
#include "core/common/com_types.h"
#include "core/common/com_misc.h"

NAMESPACE_BEGIN(pat)

class Observable;
class IObserver;

class Event {
public:
  Event( Observable* source, i32 id ):  m_src(source), m_id(id){}
  virtual ~Event(){}
  virtual i32 GetID() const { 
    return m_id; 
  }
  virtual Observable* GetSource() const { 
    return m_src; 
  }
protected:
  Observable*   m_src;
  i32           m_id;
};


class Observable {
public:
  virtual ~Observable(){}
  bool AddObserver( IObserver* obs );
  bool RemoveObserver( IObserver* obs );
  u32 GetObserverCount() const;
  bool NotifyObservers( Event* evnt );
protected:
  std::vector<IObserver*> m_observers;
};


class IObserver {
public:
  virtual ~IObserver(){}
  virtual void OnEvent( Event* evnt ) = 0;
};

NAMESPACE_END(pat)
#endif // observer_h__