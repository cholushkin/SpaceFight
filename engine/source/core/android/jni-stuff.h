#if !defined(__JNI_STUFF_H)
#define __JNI_STUFF_H

#include <sys/types.h>
#include <string.h>
#include <jni.h>

template <class T>
class LocalRef
{
  T * const m_p;
  JNIEnv&   m_env;
  LocalRef& operator=(const LocalRef&);
public:

  LocalRef(JNIEnv& env, T* p)
    : m_env(env), m_p(p) {}

  ~LocalRef()
  {
    if(m_p)
      m_env.DeleteLocalRef(m_p);
  }
  operator T*() {return m_p;}
  operator bool() const
  {
    return !!m_p;
  }

};

// actually all the same, but maybe I'll add custom Ctors later
typedef LocalRef<_jclass>       ClassRef;
typedef LocalRef<_jobject>      ObjRef;
typedef LocalRef<_jstring>      StrRef;
typedef LocalRef<_jobjectArray> ArrRef;
typedef LocalRef<_jintArray>    ArrIRef;
typedef LocalRef<_jbyteArray>   ArrBRef;

class JString
{
public:
  JString(JNIEnv& env, jstring jstr)
    : env(env)
    , jstr(env, jstr)
    , cstr(NULL) {
    jboolean iscopy;
    if(jstr)
      cstr = env.GetStringUTFChars(jstr, &iscopy);
  }

  ~JString() {
    if(jstr)
      env.ReleaseStringUTFChars(jstr, cstr);
  }

  operator bool() const
  {
    return !!cstr;
  }

  operator const char*() const {
    return c_str();
  }

  const char* c_str() const {
    return cstr;
  }

private:
  const char* cstr;
  StrRef      jstr;
  JNIEnv&     env;
};


extern "C"  jobject  create(JNIEnv& env, const char* classname);

extern "C"  jmethodID getMethod      (JNIEnv& env, jobject obj, const char* name, const char* signature);
extern "C"  jmethodID getStaticMethod(JNIEnv& env, jclass  cls, const char* name, const char* signature);
extern "C"  jfieldID  getField       (JNIEnv& env, jobject obj, const char* fieldname, const char* fieldtype);

extern "C"  void setField      (JNIEnv& env, jobject obj, jfieldID fid, jobject val);
extern "C"  void setStringField(JNIEnv& env, jobject obj, const char* fieldname, const char* str);
extern "C"  void setStringFieldW(JNIEnv& env, jobject obj, const char* fieldname, const jchar* str, jint length);
extern "C"  void setBoolField  (JNIEnv& env, jobject obj, const char* fieldname, jboolean val);
extern "C"  void setIntField   (JNIEnv& env, jobject obj, const char* fieldname, jint val);
extern "C"  void setLongField  (JNIEnv& env, jobject obj, const char* fieldname, jlong val);
extern "C"  void setShortField (JNIEnv& env, jobject obj, const char* fieldname, jshort val);
extern "C"  void setObjectField(JNIEnv& env, jobject obj, const char* fieldname, jobject val, const char* fieldtype);


#endif // __JNI_STUFF_H