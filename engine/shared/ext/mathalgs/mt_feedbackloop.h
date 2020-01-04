#ifndef feedbackloop_h__
#define feedbackloop_h__

/* A PropFeedBackLoop (proportional feedback loop) is a simple class which
moves a system's current state towards a goal, using the resulting error
difference between goal and state as feedback on the next run. */

template<class T> class PropFeedBackLoop {
public:
  PropFeedBackLoop(); 
  void      Reset(f32 mTime, f32 stepSize, f32 gain, const T& curState);
  void      SetGain(f32 g);
  f32       GetGain() const;
  void      SetGoal(const T& c);
  const T&  GetGoal() const;
  void      SetState(const T& s);
  const T&  GetState() const;
  void      Update(float dt);

private:
  f32       m_time;         
  f32       m_stepSize;
  f32       m_gain;
  T         m_goal;
  T         m_state;
};

template<class T>
PropFeedBackLoop<T>::PropFeedBackLoop()
:  m_time(0.0)
,  m_stepSize(0.001f)
,  m_gain(-1.0f) 
{}

template<class T>
void PropFeedBackLoop<T>::Reset(f32 t, f32 s, f32 g, const T& curState) {
  m_time  = t;
  m_stepSize = s;
  m_gain  = g;
  m_state = curState;
  m_goal  = curState;
}

template<class T>
void PropFeedBackLoop<T>::SetGain(f32 g) {
  m_gain = g;
}

template<class T>
f32 PropFeedBackLoop<T>::GetGain() const {
  return m_gain;
}

template<class T>
void PropFeedBackLoop<T>::SetGoal(const T& g) {
  m_goal = g;
}

template<class T>
const T& PropFeedBackLoop<T>::GetGoal() const {
  return m_goal;
}

template<class T>
void PropFeedBackLoop<T>::SetState(const T& s) {
  m_state = s;
}

template<class T>
const T& PropFeedBackLoop<T>::GetState() const {
  return m_state;
}

template<class T>
void PropFeedBackLoop<T>::Update(f32 dt) {
  const f32 curTime = m_time + dt;

  // catch time exceptions
  if (dt < 0.f)
    m_time = curTime;
  else if (dt > 0.5f)
    m_time = curTime - 0.5f;

  while (m_time < curTime) {
    m_state = m_state + (m_state - m_goal) * m_gain * m_stepSize;
    m_time += m_stepSize;
  }
}

#endif // feedbackloop_h__
