#include "jni-stuff.h"

void handleJavaException(JNIEnv& env)
{
  jthrowable exc = env.ExceptionOccurred();
  if (!exc)
    return;
  env.ExceptionDescribe();
  env.ExceptionClear();
}

jobject  create(JNIEnv& env, const char* classname)
{
  ClassRef cls(env, env.FindClass(classname));
  jmethodID mid = env.GetMethodID(cls, "<init>", "()V");
  return env.NewObject(cls, mid);
}
 
jfieldID  getField(JNIEnv& env, jobject obj, const char* fieldname, const char* fieldtype)
{
  ClassRef cls(env, env.GetObjectClass(obj));
  return env.GetFieldID(cls, fieldname, fieldtype);
}

jmethodID getMethod(JNIEnv& env, jobject obj, const char* name, const char* signature)
{
  ClassRef cls(env, env.GetObjectClass(obj));
  return env.GetMethodID(cls, name, signature);
}

jmethodID getStaticMethod(JNIEnv& env, jclass cls, const char* name, const char* signature)
{
  return env.GetStaticMethodID(cls, name, signature);
}

void setField(JNIEnv& env, jobject obj, jfieldID fid, jobject val)
{
  env.SetObjectField(obj, fid, val);
  handleJavaException(env);
}

// Note: we use NewStringUTF because wchar is not UTF-16 but cropped UTF-32
void setStringField(JNIEnv& env, jobject obj, const char* fieldname, const char* str)
{
  jfieldID fid = getField(env, obj, fieldname, "Ljava/lang/String;");
  setField(env, obj, fid, str ? (jstring)StrRef(env, env.NewStringUTF(str)) : NULL);
  handleJavaException(env);
}

// Note: we use NewStringUTF because wchar is not UTF-16 but cropped UTF-32
void setStringFieldW(JNIEnv& env, jobject obj, const char* fieldname, const jchar* str, jint length)
{
  jfieldID fid = getField(env, obj, fieldname, "Ljava/lang/String;");
  setField(env, obj, fid, str ? (jstring)StrRef(env, env.NewString(str, length)) : NULL);
  handleJavaException(env);
}

void setBoolField(JNIEnv& env, jobject obj, const char* fieldname, jboolean val)
{
  jfieldID fid = getField(env, obj, fieldname, "Z");
  env.SetBooleanField(obj, fid, val);
  handleJavaException(env);
}

void setIntField(JNIEnv& env, jobject obj, const char* fieldname, jint val)
{
  jfieldID fid = getField(env, obj, fieldname, "I");
  env.SetIntField(obj, fid, val);
}

void setLongField(JNIEnv& env, jobject obj, const char* fieldname, jlong val)
{
  jfieldID fid = getField(env, obj, fieldname, "J");
  env.SetLongField(obj, fid, val);
}

void setShortField(JNIEnv& env, jobject obj, const char* fieldname, jshort val)
{
  jfieldID fid = getField(env, obj, fieldname, "S");
  env.SetShortField(obj, fid, val);
}

void setObjectField(JNIEnv& env, jobject obj, const char* fieldname, jobject val, const char* fieldtype)
{
  jfieldID fid = getField(env, obj, fieldname, fieldtype);
  setField(env, obj, fid, val);
  handleJavaException(env);
}
