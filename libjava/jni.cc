// jni.cc - JNI implementation, including the jump table.

/* Copyright (C) 1998, 1999, 2000  Red Hat, Inc.

   This file is part of libgcj.

This software is copyrighted work licensed under the terms of the
Libgcj License.  Please consult the file "LIBGCJ_LICENSE" for
details.  */

#include <config.h>

#include <stddef.h>
#include <string.h>

// Define this before including jni.h.
#define __GCJ_JNI_IMPL__

#include <gcj/cni.h>
#include <jvm.h>
#include <java-assert.h>
#include <jni.h>

#include <java/lang/Class.h>
#include <java/lang/ClassLoader.h>
#include <java/lang/Throwable.h>
#include <java/lang/ArrayIndexOutOfBoundsException.h>
#include <java/lang/StringIndexOutOfBoundsException.h>
#include <java/lang/AbstractMethodError.h>
#include <java/lang/InstantiationException.h>
#include <java/lang/NoSuchFieldError.h>
#include <java/lang/NoSuchMethodError.h>
#include <java/lang/reflect/Constructor.h>
#include <java/lang/reflect/Method.h>
#include <java/lang/reflect/Modifier.h>
#include <java/lang/OutOfMemoryError.h>
#include <java/util/Hashtable.h>
#include <java/lang/Integer.h>

#include <gcj/method.h>
#include <gcj/field.h>

#include <java-interp.h>

#define ClassClass _CL_Q34java4lang5Class
extern java::lang::Class ClassClass;
#define ObjectClass _CL_Q34java4lang6Object
extern java::lang::Class ObjectClass;

#define MethodClass _CL_Q44java4lang7reflect6Method
extern java::lang::Class MethodClass;

// This enum is used to select different template instantiations in
// the invocation code.
enum invocation_type
{
  normal,
  nonvirtual,
  static_type,
  constructor
};

// Forward declaration.
extern struct JNINativeInterface _Jv_JNIFunctions;

// Number of slots in the default frame.  The VM must allow at least
// 16.
#define FRAME_SIZE 32

// This structure is used to keep track of local references.
struct _Jv_JNI_LocalFrame
{
  // This is true if this frame object represents a pushed frame (eg
  // from PushLocalFrame).
  int marker :  1;

  // Number of elements in frame.
  int size   : 31;

  // Next frame in chain.
  _Jv_JNI_LocalFrame *next;

  // The elements.  These are allocated using the C "struct hack".
  jobject vec[0];
};

// This holds a reference count for all local and global references.
static java::util::Hashtable *ref_table;



void
_Jv_JNI_Init (void)
{
  ref_table = new java::util::Hashtable;
}

// Tell the GC that a certain pointer is live.
static void
mark_for_gc (jobject obj)
{
  JvSynchronize sync (ref_table);

  using namespace java::lang;
  Integer *refcount = (Integer *) ref_table->get (obj);
  jint val = (refcount == NULL) ? 0 : refcount->intValue ();
  ref_table->put (obj, new Integer (val + 1));
}

// Unmark a pointer.
static void
unmark_for_gc (jobject obj)
{
  JvSynchronize sync (ref_table);

  using namespace java::lang;
  Integer *refcount = (Integer *) ref_table->get (obj);
  JvAssert (refcount);
  jint val = refcount->intValue () - 1;
  if (val == 0)
    ref_table->remove (obj);
  else
    ref_table->put (obj, new Integer (val));
}



static jobject
_Jv_JNI_NewGlobalRef (JNIEnv *, jobject obj)
{
  mark_for_gc (obj);
  return obj;
}

static void
_Jv_JNI_DeleteGlobalRef (JNIEnv *, jobject obj)
{
  unmark_for_gc (obj);
}

static void
_Jv_JNI_DeleteLocalRef (JNIEnv *env, jobject obj)
{
  _Jv_JNI_LocalFrame *frame;

  for (frame = env->locals; frame != NULL; frame = frame->next)
    {
      for (int i = 0; i < FRAME_SIZE; ++i)
	{
	  if (frame->vec[i] == obj)
	    {
	      frame->vec[i] = NULL;
	      unmark_for_gc (obj);
	      return;
	    }
	}

      // Don't go past a marked frame.
      JvAssert (! frame->marker);
    }

  JvAssert (0);
}

static jint
_Jv_JNI_EnsureLocalCapacity (JNIEnv *env, jint size)
{
  // It is easier to just always allocate a new frame of the requested
  // size.  This isn't the most efficient thing, but for now we don't
  // care.  Note that _Jv_JNI_PushLocalFrame relies on this right now.

  _Jv_JNI_LocalFrame *frame
    = (_Jv_JNI_LocalFrame *) _Jv_MallocUnchecked (sizeof (_Jv_JNI_LocalFrame)
						  + size * sizeof (jobject));
  if (frame == NULL)
    {
      // FIXME: exception processing.
      env->ex = new java::lang::OutOfMemoryError;
      return -1;
    }

  frame->marker = true;
  frame->size = size;
  memset (&frame->vec[0], 0, size * sizeof (jobject));
  frame->next = env->locals;
  env->locals = frame;

  return 0;
}

static jint
_Jv_JNI_PushLocalFrame (JNIEnv *env, jint size)
{
  jint r = _Jv_JNI_EnsureLocalCapacity (env, size);
  if (r < 0)
    return r;

  // The new frame is on top.
  env->locals->marker = true;

  return 0;
}

static jobject
_Jv_JNI_NewLocalRef (JNIEnv *env, jobject obj)
{
  // Try to find an open slot somewhere in the topmost frame.
  _Jv_JNI_LocalFrame *frame = env->locals;
  bool done = false, set = false;
  while (frame != NULL && ! done)
    {
      for (int i = 0; i < frame->size; ++i)
	if (frame->vec[i] == NULL)
	  {
	    set = true;
	    done = true;
	    frame->vec[i] = obj;
	    break;
	  }
    }

  if (! set)
    {
      // No slots, so we allocate a new frame.  According to the spec
      // we could just die here.  FIXME: return value.
      _Jv_JNI_EnsureLocalCapacity (env, 16);
      // We know the first element of the new frame will be ok.
      env->locals->vec[0] = obj;
    }

  mark_for_gc (obj);
  return obj;
}

static jobject
_Jv_JNI_PopLocalFrame (JNIEnv *env, jobject result)
{
  _Jv_JNI_LocalFrame *rf = env->locals;

  bool done = false;
  while (rf != NULL && ! done)
    {  
      for (int i = 0; i < rf->size; ++i)
	if (rf->vec[i] != NULL)
	  unmark_for_gc (rf->vec[i]);

      // If the frame we just freed is the marker frame, we are done.
      done = rf->marker;

      _Jv_JNI_LocalFrame *n = rf->next;
      // When N==NULL, we've reached the stack-allocated frame, and we
      // must not free it.  However, we must be sure to clear all its
      // elements, since we might conceivably reuse it.
      if (n == NULL)
	{
	  memset (&rf->vec[0], 0, rf->size * sizeof (jobject));
	  break;
	}

      _Jv_Free (rf);
      rf = n;
    }

  return result == NULL ? NULL : _Jv_JNI_NewLocalRef (env, result);
}

// This function is used from other template functions.  It wraps the
// return value appropriately; we specialize it so that object returns
// are turned into local references.
template<typename T>
static T
wrap_value (JNIEnv *, T value)
{
  return value;
}

template<>
static jobject
wrap_value (JNIEnv *env, jobject value)
{
  return _Jv_JNI_NewLocalRef (env, value);
}



static jint
_Jv_JNI_GetVersion (JNIEnv *)
{
  return JNI_VERSION_1_2;
}

static jclass
_Jv_JNI_DefineClass (JNIEnv *env, jobject loader, 
		     const jbyte *buf, jsize bufLen)
{
  jbyteArray bytes = JvNewByteArray (bufLen);
  jbyte *elts = elements (bytes);
  memcpy (elts, buf, bufLen * sizeof (jbyte));

  java::lang::ClassLoader *l
    = reinterpret_cast<java::lang::ClassLoader *> (loader);

  // FIXME: exception processing.
  jclass result = l->defineClass (bytes, 0, bufLen);
  return (jclass) _Jv_JNI_NewLocalRef (env, result);
}

static jclass
_Jv_JNI_FindClass (JNIEnv *env, const char *name)
{
  // FIXME: assume that NAME isn't too long.
  int len = strlen (name);
  char s[len + 1];
  for (int i = 0; i <= len; ++i)
    s[i] = (name[i] == '/') ? '.' : name[i];
  jstring n = JvNewStringUTF (s);

  java::lang::ClassLoader *loader;
  if (env->klass == NULL)
    {
      // FIXME: should use getBaseClassLoader, but we don't have that
      // yet.
      loader = java::lang::ClassLoader::getSystemClassLoader ();
    }
  else
    loader = env->klass->getClassLoader ();

  // FIXME: exception processing.
  jclass r = loader->findClass (n);

  return (jclass) _Jv_JNI_NewLocalRef (env, r);
}

static jclass
_Jv_JNI_GetSuperclass (JNIEnv *env, jclass clazz)
{
  return (jclass) _Jv_JNI_NewLocalRef (env, clazz->getSuperclass ());
}

static jboolean
_Jv_JNI_IsAssignableFrom(JNIEnv *, jclass clazz1, jclass clazz2)
{
  return clazz1->isAssignableFrom (clazz2);
}

static jint
_Jv_JNI_Throw (JNIEnv *env, jthrowable obj)
{
  env->ex = obj;
  return 0;
}

static jint
_Jv_JNI_ThrowNew (JNIEnv *env, jclass clazz, const char *message)
{
  using namespace java::lang::reflect;

  JArray<jclass> *argtypes
    = (JArray<jclass> *) JvNewObjectArray (1, &ClassClass, NULL);

  jclass *elts = elements (argtypes);
  elts[0] = &StringClass;

  // FIXME: exception processing.
  Constructor *cons = clazz->getConstructor (argtypes);

  jobjectArray values = JvNewObjectArray (1, &StringClass, NULL);
  jobject *velts = elements (values);
  velts[0] = JvNewStringUTF (message);

  // FIXME: exception processing.
  jobject obj = cons->newInstance (values);

  env->ex = reinterpret_cast<jthrowable> (obj);
  return 0;
}

static jthrowable
_Jv_JNI_ExceptionOccurred (JNIEnv *env)
{
  return (jthrowable) _Jv_JNI_NewLocalRef (env, env->ex);
}

static void
_Jv_JNI_ExceptionDescribe (JNIEnv *env)
{
  if (env->ex != NULL)
    env->ex->printStackTrace();
}

static void
_Jv_JNI_ExceptionClear (JNIEnv *env)
{
  env->ex = NULL;
}

static jboolean
_Jv_JNI_ExceptionCheck (JNIEnv *env)
{
  return env->ex != NULL;
}

static void
_Jv_JNI_FatalError (JNIEnv *, const char *message)
{
  JvFail (message);
}



static jboolean
_Jv_JNI_IsSameObject (JNIEnv *, jobject obj1, jobject obj2)
{
  return obj1 == obj2;
}

static jobject
_Jv_JNI_AllocObject (JNIEnv *env, jclass clazz)
{
  jobject obj = NULL;
  using namespace java::lang::reflect;
  if (clazz->isInterface() || Modifier::isAbstract(clazz->getModifiers()))
    env->ex = new java::lang::InstantiationException ();
  else
    {
      // FIXME: exception processing.
      // FIXME: will this work for String?
      obj = JvAllocObject (clazz);
    }

  return _Jv_JNI_NewLocalRef (env, obj);
}

static jclass
_Jv_JNI_GetObjectClass (JNIEnv *env, jobject obj)
{
  return (jclass) _Jv_JNI_NewLocalRef (env, obj->getClass());
}

static jboolean
_Jv_JNI_IsInstanceOf (JNIEnv *, jobject obj, jclass clazz)
{
  return clazz->isInstance(obj);
}



//
// This section concerns method invocation.
//

template<jboolean is_static>
static jmethodID
_Jv_JNI_GetAnyMethodID (JNIEnv *env, jclass clazz,
			const char *name, const char *sig)
{
  // FIXME: exception processing.
  _Jv_InitClass (clazz);

  _Jv_Utf8Const *name_u = _Jv_makeUtf8Const ((char *) name, -1);
  _Jv_Utf8Const *sig_u = _Jv_makeUtf8Const ((char *) sig, -1);

  JvAssert (! clazz->isPrimitive());

  using namespace java::lang::reflect;

  while (clazz != NULL)
    {
      jint count = JvNumMethods (clazz);
      jmethodID meth = JvGetFirstMethod (clazz);

      for (jint i = 0; i < count; ++i)
	{
	  if (((is_static && Modifier::isStatic (meth->accflags))
	       || (! is_static && ! Modifier::isStatic (meth->accflags)))
	      && _Jv_equalUtf8Consts (meth->name, name_u)
	      && _Jv_equalUtf8Consts (meth->signature, sig_u))
	    return meth;

	  meth = meth->getNextMethod();
	}

      clazz = clazz->getSuperclass ();
    }

  env->ex = new java::lang::NoSuchMethodError ();
  return NULL;
}

// This is a helper function which turns a va_list into an array of
// `jvalue's.  It needs signature information in order to do its work.
// The array of values must already be allocated.
static void
array_from_valist (jvalue *values, JArray<jclass> *arg_types, va_list vargs)
{
  jclass *arg_elts = elements (arg_types);
  for (int i = 0; i < arg_types->length; ++i)
    {
      if (arg_elts[i] == JvPrimClass (byte))
	values[i].b = va_arg (vargs, jbyte);
      else if (arg_elts[i] == JvPrimClass (short))
	values[i].s = va_arg (vargs, jshort);
      else if (arg_elts[i] == JvPrimClass (int))
	values[i].i = va_arg (vargs, jint);
      else if (arg_elts[i] == JvPrimClass (long))
	values[i].j = va_arg (vargs, jlong);
      else if (arg_elts[i] == JvPrimClass (float))
	values[i].f = va_arg (vargs, jfloat);
      else if (arg_elts[i] == JvPrimClass (double))
	values[i].d = va_arg (vargs, jdouble);
      else if (arg_elts[i] == JvPrimClass (boolean))
	values[i].z = va_arg (vargs, jboolean);
      else if (arg_elts[i] == JvPrimClass (char))
	values[i].c = va_arg (vargs, jchar);
      else
	{
	  // An object.
	  values[i].l = va_arg (vargs, jobject);
	}
    }
}

// This can call any sort of method: virtual, "nonvirtual", static, or
// constructor.
template<typename T, invocation_type style>
static T
_Jv_JNI_CallAnyMethodV (JNIEnv *env, jobject obj, jclass klass,
			jmethodID id, va_list vargs)
{
  if (style == normal)
    id = _Jv_LookupDeclaredMethod (obj->getClass (), id->name, id->signature);

  jclass decl_class = klass ? klass : obj->getClass ();
  JvAssert (decl_class != NULL);

  jclass return_type;
  JArray<jclass> *arg_types;
  // FIXME: exception processing.
  _Jv_GetTypesFromSignature (id, decl_class,
			     &arg_types, &return_type);

  jvalue args[arg_types->length];
  array_from_valist (args, arg_types, vargs);

  jvalue result;
  jthrowable ex = _Jv_CallAnyMethodA (obj, return_type, id,
				      style == constructor,
				      arg_types, args, &result);

  if (ex != NULL)
    env->ex = ex;

  // We cheat a little here.  FIXME.
  return wrap_value (env, * (T *) &result);
}

template<typename T, invocation_type style>
static T
_Jv_JNI_CallAnyMethod (JNIEnv *env, jobject obj, jclass klass,
		       jmethodID method, ...)
{
  va_list args;
  T result;

  va_start (args, method);
  result = _Jv_JNI_CallAnyMethodV<T, style> (env, obj, klass, method, args);
  va_end (args);

  return result;
}

template<typename T, invocation_type style>
static T
_Jv_JNI_CallAnyMethodA (JNIEnv *env, jobject obj, jclass klass,
			jmethodID id, jvalue *args)
{
  if (style == normal)
    id = _Jv_LookupDeclaredMethod (obj->getClass (), id->name, id->signature);

  jclass decl_class = klass ? klass : obj->getClass ();
  JvAssert (decl_class != NULL);

  jclass return_type;
  JArray<jclass> *arg_types;
  // FIXME: exception processing.
  _Jv_GetTypesFromSignature (id, decl_class,
			     &arg_types, &return_type);

  jvalue result;
  jthrowable ex = _Jv_CallAnyMethodA (obj, return_type, id,
				      style == constructor,
				      arg_types, args, &result);

  if (ex != NULL)
    env->ex = ex;

  // We cheat a little here.  FIXME.
  return wrap_value (env, * (T *) &result);
}

template<invocation_type style>
static void
_Jv_JNI_CallAnyVoidMethodV (JNIEnv *env, jobject obj, jclass klass,
			    jmethodID id, va_list vargs)
{
  if (style == normal)
    id = _Jv_LookupDeclaredMethod (obj->getClass (), id->name, id->signature);

  jclass decl_class = klass ? klass : obj->getClass ();
  JvAssert (decl_class != NULL);

  jclass return_type;
  JArray<jclass> *arg_types;
  // FIXME: exception processing.
  _Jv_GetTypesFromSignature (id, decl_class,
			     &arg_types, &return_type);

  jvalue args[arg_types->length];
  array_from_valist (args, arg_types, vargs);

  jthrowable ex = _Jv_CallAnyMethodA (obj, return_type, id,
				      style == constructor,
				      arg_types, args, NULL);

  if (ex != NULL)
    env->ex = ex;
}

template<invocation_type style>
static void
_Jv_JNI_CallAnyVoidMethod (JNIEnv *env, jobject obj, jclass klass,
			   jmethodID method, ...)
{
  va_list args;

  va_start (args, method);
  _Jv_JNI_CallAnyVoidMethodV<style> (env, obj, klass, method, args);
  va_end (args);
}

template<invocation_type style>
static void
_Jv_JNI_CallAnyVoidMethodA (JNIEnv *env, jobject obj, jclass klass,
			    jmethodID id, jvalue *args)
{
  if (style == normal)
    id = _Jv_LookupDeclaredMethod (obj->getClass (), id->name, id->signature);

  jclass decl_class = klass ? klass : obj->getClass ();
  JvAssert (decl_class != NULL);

  jclass return_type;
  JArray<jclass> *arg_types;
  // FIXME: exception processing.
  _Jv_GetTypesFromSignature (id, decl_class,
			     &arg_types, &return_type);

  jthrowable ex = _Jv_CallAnyMethodA (obj, return_type, id,
				      style == constructor,
				      arg_types, args, NULL);

  if (ex != NULL)
    env->ex = ex;
}

// Functions with this signature are used to implement functions in
// the CallMethod family.
template<typename T>
static T
_Jv_JNI_CallMethodV (JNIEnv *env, jobject obj, jmethodID id, va_list args)
{
  return _Jv_JNI_CallAnyMethodV<T, normal> (env, obj, NULL, id, args);
}

// Functions with this signature are used to implement functions in
// the CallMethod family.
template<typename T>
static T
_Jv_JNI_CallMethod (JNIEnv *env, jobject obj, jmethodID id, ...)
{
  va_list args;
  T result;

  va_start (args, id);
  result = _Jv_JNI_CallAnyMethodV<T, normal> (env, obj, NULL, id, args);
  va_end (args);

  return result;
}

// Functions with this signature are used to implement functions in
// the CallMethod family.
template<typename T>
static T
_Jv_JNI_CallMethodA (JNIEnv *env, jobject obj, jmethodID id, jvalue *args)
{
  return _Jv_JNI_CallAnyMethodA<T, normal> (env, obj, NULL, id, args);
}

static void
_Jv_JNI_CallVoidMethodV (JNIEnv *env, jobject obj, jmethodID id, va_list args)
{
  _Jv_JNI_CallAnyVoidMethodV<normal> (env, obj, NULL, id, args);
}

static void
_Jv_JNI_CallVoidMethod (JNIEnv *env, jobject obj, jmethodID id, ...)
{
  va_list args;

  va_start (args, id);
  _Jv_JNI_CallAnyVoidMethodV<normal> (env, obj, NULL, id, args);
  va_end (args);
}

static void
_Jv_JNI_CallVoidMethodA (JNIEnv *env, jobject obj, jmethodID id, jvalue *args)
{
  _Jv_JNI_CallAnyVoidMethodA<normal> (env, obj, NULL, id, args);
}

// Functions with this signature are used to implement functions in
// the CallStaticMethod family.
template<typename T>
static T
_Jv_JNI_CallStaticMethodV (JNIEnv *env, jclass klass,
			   jmethodID id, va_list args)
{
  return _Jv_JNI_CallAnyMethodV<T, static_type> (env, NULL, klass, id, args);
}

// Functions with this signature are used to implement functions in
// the CallStaticMethod family.
template<typename T>
static T
_Jv_JNI_CallStaticMethod (JNIEnv *env, jclass klass, jmethodID id, ...)
{
  va_list args;
  T result;

  va_start (args, id);
  result = _Jv_JNI_CallAnyMethodV<T, static_type> (env, NULL, klass,
						   id, args);
  va_end (args);

  return result;
}

// Functions with this signature are used to implement functions in
// the CallStaticMethod family.
template<typename T>
static T
_Jv_JNI_CallStaticMethodA (JNIEnv *env, jclass klass, jmethodID id,
			   jvalue *args)
{
  return _Jv_JNI_CallAnyMethodA<T, static_type> (env, NULL, klass, id, args);
}

static void
_Jv_JNI_CallStaticVoidMethodV (JNIEnv *env, jclass klass, jmethodID id,
			       va_list args)
{
  _Jv_JNI_CallAnyVoidMethodV<static_type> (env, NULL, klass, id, args);
}

static void
_Jv_JNI_CallStaticVoidMethod (JNIEnv *env, jclass klass, jmethodID id, ...)
{
  va_list args;

  va_start (args, id);
  _Jv_JNI_CallAnyVoidMethodV<static_type> (env, NULL, klass, id, args);
  va_end (args);
}

static void
_Jv_JNI_CallStaticVoidMethodA (JNIEnv *env, jclass klass, jmethodID id,
			       jvalue *args)
{
  _Jv_JNI_CallAnyVoidMethodA<static_type> (env, NULL, klass, id, args);
}

static jobject
_Jv_JNI_NewObjectV (JNIEnv *env, jclass klass,
		    jmethodID id, va_list args)
{
  return _Jv_JNI_CallAnyMethodV<jobject, constructor> (env, NULL, klass,
						       id, args);
}

static jobject
_Jv_JNI_NewObject (JNIEnv *env, jclass klass, jmethodID id, ...)
{
  va_list args;
  jobject result;

  va_start (args, id);
  result = _Jv_JNI_CallAnyMethodV<jobject, constructor> (env, NULL, klass,
							 id, args);
  va_end (args);

  return result;
}

static jobject
_Jv_JNI_NewObjectA (JNIEnv *env, jclass klass, jmethodID id,
		    jvalue *args)
{
  return _Jv_JNI_CallAnyMethodA<jobject, constructor> (env, NULL, klass,
						       id, args);
}



template<typename T>
static T
_Jv_JNI_GetField (JNIEnv *env, jobject obj, jfieldID field) 
{
  T *ptr = (T *) ((char *) obj + field->getOffset ());
  return wrap_value (env, *ptr);
}

template<typename T>
static void
_Jv_JNI_SetField (JNIEnv *, jobject obj, jfieldID field, T value)
{
  T *ptr = (T *) ((char *) obj + field->getOffset ());
  *ptr = value;
}

template<jboolean is_static>
static jfieldID
_Jv_JNI_GetAnyFieldID (JNIEnv *env, jclass clazz,
		       const char *name, const char *sig)
{
  // FIXME: exception processing.
  _Jv_InitClass (clazz);

  _Jv_Utf8Const *a_name = _Jv_makeUtf8Const ((char *) name, -1);

  jclass field_class = NULL;
  if (sig[0] == '[')
    field_class = _Jv_FindClassFromSignature ((char *) sig, NULL);
  else
    {
      _Jv_Utf8Const *sig_u = _Jv_makeUtf8Const ((char *) sig, -1);
      field_class = _Jv_FindClass (sig_u, NULL);
    }

  // FIXME: what if field_class == NULL?

  while (clazz != NULL)
    {
      jint count = (is_static
		    ? JvNumStaticFields (clazz)
		    : JvNumInstanceFields (clazz));
      jfieldID field = (is_static
			? JvGetFirstStaticField (clazz)
			: JvGetFirstInstanceField (clazz));
      for (jint i = 0; i < count; ++i)
	{
	  // The field is resolved as a side effect of class
	  // initialization.
	  JvAssert (field->isResolved ());

	  _Jv_Utf8Const *f_name = field->getNameUtf8Const(clazz);

	  if (_Jv_equalUtf8Consts (f_name, a_name)
	      && field->getClass() == field_class)
	    return field;

	  field = field->getNextField ();
	}

      clazz = clazz->getSuperclass ();
    }

  env->ex = new java::lang::NoSuchFieldError ();
  return NULL;
}

template<typename T>
static T
_Jv_JNI_GetStaticField (JNIEnv *env, jclass, jfieldID field)
{
  T *ptr = (T *) field->u.addr;
  return wrap_value (env, *ptr);
}

template<typename T>
static void
_Jv_JNI_SetStaticField (JNIEnv *, jclass, jfieldID field, T value)
{
  T *ptr = (T *) field->u.addr;
  *ptr = value;
}

static jstring
_Jv_JNI_NewString (JNIEnv *env, const jchar *unichars, jsize len)
{
  // FIXME: exception processing.
  jstring r = _Jv_NewString (unichars, len);
  return (jstring) _Jv_JNI_NewLocalRef (env, r);
}

static jsize
_Jv_JNI_GetStringLength (JNIEnv *, jstring string)
{
  return string->length();
}

static const jchar *
_Jv_JNI_GetStringChars (JNIEnv *, jstring string, jboolean *isCopy)
{
  jchar *result = _Jv_GetStringChars (string);
  mark_for_gc (string);
  if (isCopy)
    *isCopy = false;
  return (const jchar *) result;
}

static void
_Jv_JNI_ReleaseStringChars (JNIEnv *, jstring string, const jchar *)
{
  unmark_for_gc (string);
}

static jstring
_Jv_JNI_NewStringUTF (JNIEnv *env, const char *bytes)
{
  // FIXME: exception processing.
  jstring result = JvNewStringUTF (bytes);
  return (jstring) _Jv_JNI_NewLocalRef (env, result);
}

static jsize
_Jv_JNI_GetStringUTFLength (JNIEnv *, jstring string)
{
  return JvGetStringUTFLength (string);
}

static const char *
_Jv_JNI_GetStringUTFChars (JNIEnv *, jstring string, jboolean *isCopy)
{
  jsize len = JvGetStringUTFLength (string);
  // FIXME: exception processing.
  char *r = (char *) _Jv_Malloc (len + 1);
  JvGetStringUTFRegion (string, 0, len, r);
  r[len] = '\0';

  if (isCopy)
    *isCopy = true;

  return (const char *) r;
}

static void
_Jv_JNI_ReleaseStringUTFChars (JNIEnv *, jstring, const char *utf)
{
  _Jv_Free ((void *) utf);
}

static void
_Jv_JNI_GetStringRegion (JNIEnv *env, jstring string, jsize start, jsize len,
			 jchar *buf)
{
  jchar *result = _Jv_GetStringChars (string);
  if (start < 0 || start > string->length ()
      || len < 0 || start + len > string->length ())
    env->ex = new java::lang::StringIndexOutOfBoundsException ();
  else
    memcpy (buf, &result[start], len * sizeof (jchar));
}

static void
_Jv_JNI_GetStringUTFRegion (JNIEnv *env, jstring str, jsize start,
			    jsize len, char *buf)
{
  if (start < 0 || start > str->length ()
      || len < 0 || start + len > str->length ())
    env->ex = new java::lang::StringIndexOutOfBoundsException ();
  else
    _Jv_GetStringUTFRegion (str, start, len, buf);
}

static const jchar *
_Jv_JNI_GetStringCritical (JNIEnv *, jstring str, jboolean *isCopy)
{
  jchar *result = _Jv_GetStringChars (str);
  if (isCopy)
    *isCopy = false;
  return result;
}

static void
_Jv_JNI_ReleaseStringCritical (JNIEnv *, jstring, const jchar *)
{
  // Nothing.
}

static jsize
_Jv_JNI_GetArrayLength (JNIEnv *, jarray array)
{
  return array->length;
}

static jarray
_Jv_JNI_NewObjectArray (JNIEnv *env, jsize length, jclass elementClass,
			jobject init)
{
  // FIXME: exception processing.
  jarray result = JvNewObjectArray (length, elementClass, init);
  return (jarray) _Jv_JNI_NewLocalRef (env, result);
}

static jobject
_Jv_JNI_GetObjectArrayElement (JNIEnv *env, jobjectArray array, jsize index)
{
  jobject *elts = elements (array);
  return _Jv_JNI_NewLocalRef (env, elts[index]);
}

static void
_Jv_JNI_SetObjectArrayElement (JNIEnv *, jobjectArray array, jsize index,
			       jobject value)
{
  // FIXME: exception processing.
  _Jv_CheckArrayStore (array, value);
  jobject *elts = elements (array);
  elts[index] = value;
}

template<typename T, jclass K>
static JArray<T> *
_Jv_JNI_NewPrimitiveArray (JNIEnv *env, jsize length)
{
  // FIXME: exception processing.
  return (JArray<T> *) _Jv_JNI_NewLocalRef (env,
					    _Jv_NewPrimArray (K, length));
}

template<typename T>
static T *
_Jv_JNI_GetPrimitiveArrayElements (JNIEnv *, JArray<T> *array,
				   jboolean *isCopy)
{
  T *elts = elements (array);
  if (isCopy)
    {
      // We elect never to copy.
      *isCopy = false;
    }
  mark_for_gc (array);
  return elts;
}

template<typename T>
static void
_Jv_JNI_ReleasePrimitiveArrayElements (JNIEnv *, JArray<T> *array,
				       T *, jint /* mode */)
{
  // Note that we ignore MODE.  We can do this because we never copy
  // the array elements.  My reading of the JNI documentation is that
  // this is an option for the implementor.
  unmark_for_gc (array);
}

template<typename T>
static void
_Jv_JNI_GetPrimitiveArrayRegion (JNIEnv *env, JArray<T> *array,
				 jsize start, jsize len,
				 T *buf)
{
  if (start < 0 || len >= array->length || start + len >= array->length)
    {
      // FIXME: index.
      env->ex = new java::lang::ArrayIndexOutOfBoundsException ();
    }
  else
    {
      T *elts = elements (array) + start;
      memcpy (buf, elts, len * sizeof (T));
    }
}

template<typename T>
static void
_Jv_JNI_SetPrimitiveArrayRegion (JNIEnv *env, JArray<T> *array, 
				 jsize start, jsize len, T *buf)
{
  if (start < 0 || len >= array->length || start + len >= array->length)
    {
      // FIXME: index.
      env->ex = new java::lang::ArrayIndexOutOfBoundsException ();
    }
  else
    {
      T *elts = elements (array) + start;
      memcpy (elts, buf, len * sizeof (T));
    }
}

static void *
_Jv_JNI_GetPrimitiveArrayCritical (JNIEnv *, jarray array,
				   jboolean *isCopy)
{
  // FIXME: does this work?
  jclass klass = array->getClass()->getComponentType();
  JvAssert (klass->isPrimitive ());
  char *r = _Jv_GetArrayElementFromElementType (array, klass);
  if (isCopy)
    *isCopy = false;
  return r;
}

static void
_Jv_JNI_ReleasePrimitiveArrayCritical (JNIEnv *, jarray, void *, jint)
{
  // Nothing.
}

static jint
_Jv_JNI_MonitorEnter (JNIEnv *, jobject obj)
{
  // FIXME: exception processing.
  jint r = _Jv_MonitorEnter (obj);
  return r;
}

static jint
_Jv_JNI_MonitorExit (JNIEnv *, jobject obj)
{
  // FIXME: exception processing.
  jint r = _Jv_MonitorExit (obj);
  return r;
}

// JDK 1.2
jobject
_Jv_JNI_ToReflectedField (JNIEnv *env, jclass cls, jfieldID fieldID,
			  jboolean)
{
  // FIXME: exception processing.
  java::lang::reflect::Field *field = new java::lang::reflect::Field();
  field->declaringClass = cls;
  field->offset = (char*) fieldID - (char *) cls->fields;
  field->name = _Jv_NewStringUtf8Const (fieldID->getNameUtf8Const (cls));
  return _Jv_JNI_NewLocalRef (env, field);
}

// JDK 1.2
static jfieldID
_Jv_JNI_FromReflectedField (JNIEnv *, jobject f)
{
  using namespace java::lang::reflect;

  Field *field = reinterpret_cast<Field *> (f);
  return _Jv_FromReflectedField (field);
}

jobject
_Jv_JNI_ToReflectedMethod (JNIEnv *env, jclass klass, jmethodID id,
			   jboolean)
{
  using namespace java::lang::reflect;

  // FIXME.
  static _Jv_Utf8Const *init_name = _Jv_makeUtf8Const ("<init>", 6);

  jobject result;
  if (_Jv_equalUtf8Consts (id->name, init_name))
    {
      // A constructor.
      Constructor *cons = new Constructor ();
      cons->offset = (char *) id - (char *) &klass->methods;
      cons->declaringClass = klass;
      result = cons;
    }
  else
    {
      Method *meth = new Method ();
      meth->offset = (char *) id - (char *) &klass->methods;
      meth->declaringClass = klass;
      result = meth;
    }

  return _Jv_JNI_NewLocalRef (env, result);
}

static jmethodID
_Jv_JNI_FromReflectedMethod (JNIEnv *, jobject method)
{
  using namespace java::lang::reflect;
  if ((&MethodClass)->isInstance (method))
    return _Jv_FromReflectedMethod (reinterpret_cast<Method *> (method));
  return
    _Jv_FromReflectedConstructor (reinterpret_cast<Constructor *> (method));
}



#ifdef INTERPRETER

// Add a character to the buffer, encoding properly.
static void
add_char (char *buf, jchar c, int *here)
{
  if (c == '_')
    {
      buf[(*here)++] = '_';
      buf[(*here)++] = '1';
    }
  else if (c == ';')
    {
      buf[(*here)++] = '_';
      buf[(*here)++] = '2';
    }
  else if (c == '[')
    {
      buf[(*here)++] = '_';
      buf[(*here)++] = '3';
    }
  else if (c == '/')
    buf[(*here)++] = '_';
  if ((c >= '0' && c <= '9')
      || (c >= 'a' && c <= 'z')
      || (c >= 'A' && c <= 'Z'))
    buf[(*here)++] = (char) c;
  else
    {
      // "Unicode" character.
      buf[(*here)++] = '_';
      buf[(*here)++] = '0';
      for (int i = 0; i < 4; ++i)
	{
	  int val = c & 0x0f;
	  buf[(*here) + 4 - i] = (val > 10) ? ('a' + val - 10) : ('0' + val);
	  c >>= 4;
	}
      *here += 4;
    }
}

// Compute a mangled name for a native function.  This computes the
// long name, and also returns an index which indicates where a NUL
// can be placed to create the short name.  This function assumes that
// the buffer is large enough for its results.
static void
mangled_name (jclass klass, _Jv_Utf8Const *func_name,
	      _Jv_Utf8Const *signature, char *buf, int *long_start)
{
  strcpy (buf, "Java_");
  int here = 5;

  // Add fully qualified class name.
  jchar *chars = _Jv_GetStringChars (klass->getName ());
  jint len = klass->getName ()->length ();
  for (int i = 0; i < len; ++i)
    add_char (buf, chars[i], &here);

  // Don't use add_char because we need a literal `_'.
  buf[here++] = '_';

  const unsigned char *fn = (const unsigned char *) func_name->data;
  const unsigned char *limit = fn + func_name->length;
  for (int i = 0; ; ++i)
    {
      int ch = UTF8_GET (fn, limit);
      if (ch < 0)
	break;
      add_char (buf, ch, &here);
    }

  // This is where the long signature begins.
  *long_start = here;
  buf[here++] = '_';
  buf[here++] = '_';

  const unsigned char *sig = (const unsigned char *) signature->data;
  limit = sig + signature->length;
  JvAssert (signature[0] == '(');
  for (int i = 1; ; ++i)
    {
      int ch = UTF8_GET (sig, limit);
      if (ch == ')' || ch < 0)
	break;
      add_char (buf, ch, &here);
    }

  buf[here] = '\0';
}

// This function is the stub which is used to turn an ordinary (CNI)
// method call into a JNI call.
void
_Jv_JNIMethod::call (ffi_cif *cif, void *ret, ffi_raw *args, void *__this)
{
  _Jv_JNIMethod* _this = (_Jv_JNIMethod *) __this;

  JNIEnv env;
  _Jv_JNI_LocalFrame *frame
    = (_Jv_JNI_LocalFrame *) alloca (sizeof (_Jv_JNI_LocalFrame)
				     + FRAME_SIZE * sizeof (jobject));

  env.p = &_Jv_JNIFunctions;
  env.ex = NULL;
  env.klass = _this->defining_class;
  env.locals = frame;

  frame->marker = true;
  frame->next = NULL;
  frame->size = FRAME_SIZE;
  for (int i = 0; i < frame->size; ++i)
    frame->vec[i] = NULL;

  // FIXME: we should mark every reference parameter as a local.  For
  // now we assume a conservative GC, and we assume that the
  // references are on the stack somewhere.

  // We cache the value that we find, of course, but if we don't find
  // a value we don't cache that fact -- we might subsequently load a
  // library which finds the function in question.
  if (_this->function == NULL)
    {
      char buf[10 + 6 * (_this->self->name->length
			 + _this->self->signature->length)];
      int long_start;
      mangled_name (_this->defining_class, _this->self->name,
		    _this->self->signature, buf, &long_start);
      char c = buf[long_start];
      buf[long_start] = '\0';
      _this->function = _Jv_FindSymbolInExecutable (buf);
      if (_this->function == NULL)
	{
	  buf[long_start] = c;
	  _this->function = _Jv_FindSymbolInExecutable (buf);
	  if (_this->function == NULL)
	    {
	      jstring str = JvNewStringUTF (_this->self->name->data);
	      JvThrow (new java::lang::AbstractMethodError (str));
	    }
	}
    }

  // The actual call to the JNI function.
  // FIXME: if this is a static function we must include the class!
  ffi_raw_call (cif, (void (*) (...)) _this->function, ret, args);

  do
    {
      _Jv_JNI_PopLocalFrame (&env, NULL);
    }
  while (env.locals != frame);

  if (env.ex)
    JvThrow (env.ex);
}

#endif /* INTERPRETER */



#define NOT_IMPL NULL
#define RESERVED NULL

struct JNINativeInterface _Jv_JNIFunctions =
{
  RESERVED,
  RESERVED,
  RESERVED,
  RESERVED,
  _Jv_JNI_GetVersion,
  _Jv_JNI_DefineClass,
  _Jv_JNI_FindClass,
  _Jv_JNI_FromReflectedMethod,
  _Jv_JNI_FromReflectedField,
  _Jv_JNI_ToReflectedMethod,
  _Jv_JNI_GetSuperclass,
  _Jv_JNI_IsAssignableFrom,
  _Jv_JNI_ToReflectedField,
  _Jv_JNI_Throw,
  _Jv_JNI_ThrowNew,
  _Jv_JNI_ExceptionOccurred,
  _Jv_JNI_ExceptionDescribe,
  _Jv_JNI_ExceptionClear,
  _Jv_JNI_FatalError,

  _Jv_JNI_PushLocalFrame,
  _Jv_JNI_PopLocalFrame,
  _Jv_JNI_NewGlobalRef,
  _Jv_JNI_DeleteGlobalRef,
  _Jv_JNI_DeleteLocalRef,

  _Jv_JNI_IsSameObject,

  _Jv_JNI_NewLocalRef,
  _Jv_JNI_EnsureLocalCapacity,

  _Jv_JNI_AllocObject,
  _Jv_JNI_NewObject,
  _Jv_JNI_NewObjectV,
  _Jv_JNI_NewObjectA,
  _Jv_JNI_GetObjectClass,
  _Jv_JNI_IsInstanceOf,
  _Jv_JNI_GetAnyMethodID<false>,

  _Jv_JNI_CallMethod<jobject>,
  _Jv_JNI_CallMethodV<jobject>,
  _Jv_JNI_CallMethodA<jobject>,
  _Jv_JNI_CallMethod<jboolean>,
  _Jv_JNI_CallMethodV<jboolean>,
  _Jv_JNI_CallMethodA<jboolean>,
  _Jv_JNI_CallMethod<jbyte>,
  _Jv_JNI_CallMethodV<jbyte>,
  _Jv_JNI_CallMethodA<jbyte>,
  _Jv_JNI_CallMethod<jchar>,
  _Jv_JNI_CallMethodV<jchar>,
  _Jv_JNI_CallMethodA<jchar>,
  _Jv_JNI_CallMethod<jshort>,
  _Jv_JNI_CallMethodV<jshort>,
  _Jv_JNI_CallMethodA<jshort>,
  _Jv_JNI_CallMethod<jint>,
  _Jv_JNI_CallMethodV<jint>,
  _Jv_JNI_CallMethodA<jint>,
  _Jv_JNI_CallMethod<jlong>,
  _Jv_JNI_CallMethodV<jlong>,
  _Jv_JNI_CallMethodA<jlong>,
  _Jv_JNI_CallMethod<jfloat>,
  _Jv_JNI_CallMethodV<jfloat>,
  _Jv_JNI_CallMethodA<jfloat>,
  _Jv_JNI_CallMethod<jdouble>,
  _Jv_JNI_CallMethodV<jdouble>,
  _Jv_JNI_CallMethodA<jdouble>,
  _Jv_JNI_CallVoidMethod,
  _Jv_JNI_CallVoidMethodV,
  _Jv_JNI_CallVoidMethodA,

  // Nonvirtual method invocation functions follow.
  _Jv_JNI_CallAnyMethod<jobject, nonvirtual>,
  _Jv_JNI_CallAnyMethodV<jobject, nonvirtual>,
  _Jv_JNI_CallAnyMethodA<jobject, nonvirtual>,
  _Jv_JNI_CallAnyMethod<jboolean, nonvirtual>,
  _Jv_JNI_CallAnyMethodV<jboolean, nonvirtual>,
  _Jv_JNI_CallAnyMethodA<jboolean, nonvirtual>,
  _Jv_JNI_CallAnyMethod<jbyte, nonvirtual>,
  _Jv_JNI_CallAnyMethodV<jbyte, nonvirtual>,
  _Jv_JNI_CallAnyMethodA<jbyte, nonvirtual>,
  _Jv_JNI_CallAnyMethod<jchar, nonvirtual>,
  _Jv_JNI_CallAnyMethodV<jchar, nonvirtual>,
  _Jv_JNI_CallAnyMethodA<jchar, nonvirtual>,
  _Jv_JNI_CallAnyMethod<jshort, nonvirtual>,
  _Jv_JNI_CallAnyMethodV<jshort, nonvirtual>,
  _Jv_JNI_CallAnyMethodA<jshort, nonvirtual>,
  _Jv_JNI_CallAnyMethod<jint, nonvirtual>,
  _Jv_JNI_CallAnyMethodV<jint, nonvirtual>,
  _Jv_JNI_CallAnyMethodA<jint, nonvirtual>,
  _Jv_JNI_CallAnyMethod<jlong, nonvirtual>,
  _Jv_JNI_CallAnyMethodV<jlong, nonvirtual>,
  _Jv_JNI_CallAnyMethodA<jlong, nonvirtual>,
  _Jv_JNI_CallAnyMethod<jfloat, nonvirtual>,
  _Jv_JNI_CallAnyMethodV<jfloat, nonvirtual>,
  _Jv_JNI_CallAnyMethodA<jfloat, nonvirtual>,
  _Jv_JNI_CallAnyMethod<jdouble, nonvirtual>,
  _Jv_JNI_CallAnyMethodV<jdouble, nonvirtual>,
  _Jv_JNI_CallAnyMethodA<jdouble, nonvirtual>,
  _Jv_JNI_CallAnyVoidMethod<nonvirtual>,
  _Jv_JNI_CallAnyVoidMethodV<nonvirtual>,
  _Jv_JNI_CallAnyVoidMethodA<nonvirtual>,

  _Jv_JNI_GetAnyFieldID<false>,
  _Jv_JNI_GetField<jobject>,
  _Jv_JNI_GetField<jboolean>,
  _Jv_JNI_GetField<jbyte>,
  _Jv_JNI_GetField<jchar>,
  _Jv_JNI_GetField<jshort>,
  _Jv_JNI_GetField<jint>,
  _Jv_JNI_GetField<jlong>,
  _Jv_JNI_GetField<jfloat>,
  _Jv_JNI_GetField<jdouble>,
  _Jv_JNI_SetField,
  _Jv_JNI_SetField,
  _Jv_JNI_SetField,
  _Jv_JNI_SetField,
  _Jv_JNI_SetField,
  _Jv_JNI_SetField,
  _Jv_JNI_SetField,
  _Jv_JNI_SetField,
  _Jv_JNI_SetField,
  _Jv_JNI_GetAnyMethodID<true>,

  _Jv_JNI_CallStaticMethod<jobject>,
  _Jv_JNI_CallStaticMethodV<jobject>,
  _Jv_JNI_CallStaticMethodA<jobject>,
  _Jv_JNI_CallStaticMethod<jboolean>,
  _Jv_JNI_CallStaticMethodV<jboolean>,
  _Jv_JNI_CallStaticMethodA<jboolean>,
  _Jv_JNI_CallStaticMethod<jbyte>,
  _Jv_JNI_CallStaticMethodV<jbyte>,
  _Jv_JNI_CallStaticMethodA<jbyte>,
  _Jv_JNI_CallStaticMethod<jchar>,
  _Jv_JNI_CallStaticMethodV<jchar>,
  _Jv_JNI_CallStaticMethodA<jchar>,
  _Jv_JNI_CallStaticMethod<jshort>,
  _Jv_JNI_CallStaticMethodV<jshort>,
  _Jv_JNI_CallStaticMethodA<jshort>,
  _Jv_JNI_CallStaticMethod<jint>,
  _Jv_JNI_CallStaticMethodV<jint>,
  _Jv_JNI_CallStaticMethodA<jint>,
  _Jv_JNI_CallStaticMethod<jlong>,
  _Jv_JNI_CallStaticMethodV<jlong>,
  _Jv_JNI_CallStaticMethodA<jlong>,
  _Jv_JNI_CallStaticMethod<jfloat>,
  _Jv_JNI_CallStaticMethodV<jfloat>,
  _Jv_JNI_CallStaticMethodA<jfloat>,
  _Jv_JNI_CallStaticMethod<jdouble>,
  _Jv_JNI_CallStaticMethodV<jdouble>,
  _Jv_JNI_CallStaticMethodA<jdouble>,
  _Jv_JNI_CallStaticVoidMethod,
  _Jv_JNI_CallStaticVoidMethodV,
  _Jv_JNI_CallStaticVoidMethodA,

  _Jv_JNI_GetAnyFieldID<true>,
  _Jv_JNI_GetStaticField<jobject>,
  _Jv_JNI_GetStaticField<jboolean>,
  _Jv_JNI_GetStaticField<jbyte>,
  _Jv_JNI_GetStaticField<jchar>,
  _Jv_JNI_GetStaticField<jshort>,
  _Jv_JNI_GetStaticField<jint>,
  _Jv_JNI_GetStaticField<jlong>,
  _Jv_JNI_GetStaticField<jfloat>,
  _Jv_JNI_GetStaticField<jdouble>,
  _Jv_JNI_SetStaticField,
  _Jv_JNI_SetStaticField,
  _Jv_JNI_SetStaticField,
  _Jv_JNI_SetStaticField,
  _Jv_JNI_SetStaticField,
  _Jv_JNI_SetStaticField,
  _Jv_JNI_SetStaticField,
  _Jv_JNI_SetStaticField,
  _Jv_JNI_SetStaticField,
  _Jv_JNI_NewString,
  _Jv_JNI_GetStringLength,
  _Jv_JNI_GetStringChars,
  _Jv_JNI_ReleaseStringChars,
  _Jv_JNI_NewStringUTF,
  _Jv_JNI_GetStringUTFLength,
  _Jv_JNI_GetStringUTFChars,
  _Jv_JNI_ReleaseStringUTFChars,
  _Jv_JNI_GetArrayLength,
  _Jv_JNI_NewObjectArray,
  _Jv_JNI_GetObjectArrayElement,
  _Jv_JNI_SetObjectArrayElement,
  _Jv_JNI_NewPrimitiveArray<jboolean, JvPrimClass (boolean)>,
  _Jv_JNI_NewPrimitiveArray<jbyte, JvPrimClass (byte)>,
  _Jv_JNI_NewPrimitiveArray<jchar, JvPrimClass (char)>,
  _Jv_JNI_NewPrimitiveArray<jshort, JvPrimClass (short)>,
  _Jv_JNI_NewPrimitiveArray<jint, JvPrimClass (int)>,
  _Jv_JNI_NewPrimitiveArray<jlong, JvPrimClass (long)>,
  _Jv_JNI_NewPrimitiveArray<jfloat, JvPrimClass (float)>,
  _Jv_JNI_NewPrimitiveArray<jdouble, JvPrimClass (double)>,
  _Jv_JNI_GetPrimitiveArrayElements,
  _Jv_JNI_GetPrimitiveArrayElements,
  _Jv_JNI_GetPrimitiveArrayElements,
  _Jv_JNI_GetPrimitiveArrayElements,
  _Jv_JNI_GetPrimitiveArrayElements,
  _Jv_JNI_GetPrimitiveArrayElements,
  _Jv_JNI_GetPrimitiveArrayElements,
  _Jv_JNI_GetPrimitiveArrayElements,
  _Jv_JNI_ReleasePrimitiveArrayElements,
  _Jv_JNI_ReleasePrimitiveArrayElements,
  _Jv_JNI_ReleasePrimitiveArrayElements,
  _Jv_JNI_ReleasePrimitiveArrayElements,
  _Jv_JNI_ReleasePrimitiveArrayElements,
  _Jv_JNI_ReleasePrimitiveArrayElements,
  _Jv_JNI_ReleasePrimitiveArrayElements,
  _Jv_JNI_ReleasePrimitiveArrayElements,
  _Jv_JNI_GetPrimitiveArrayRegion,
  _Jv_JNI_GetPrimitiveArrayRegion,
  _Jv_JNI_GetPrimitiveArrayRegion,
  _Jv_JNI_GetPrimitiveArrayRegion,
  _Jv_JNI_GetPrimitiveArrayRegion,
  _Jv_JNI_GetPrimitiveArrayRegion,
  _Jv_JNI_GetPrimitiveArrayRegion,
  _Jv_JNI_GetPrimitiveArrayRegion,
  _Jv_JNI_SetPrimitiveArrayRegion,
  _Jv_JNI_SetPrimitiveArrayRegion,
  _Jv_JNI_SetPrimitiveArrayRegion,
  _Jv_JNI_SetPrimitiveArrayRegion,
  _Jv_JNI_SetPrimitiveArrayRegion,
  _Jv_JNI_SetPrimitiveArrayRegion,
  _Jv_JNI_SetPrimitiveArrayRegion,
  _Jv_JNI_SetPrimitiveArrayRegion,
  NOT_IMPL /* RegisterNatives */,
  NOT_IMPL /* UnregisterNatives */,
  _Jv_JNI_MonitorEnter,
  _Jv_JNI_MonitorExit,
  NOT_IMPL /* GetJavaVM */,

  _Jv_JNI_GetStringRegion,
  _Jv_JNI_GetStringUTFRegion,
  _Jv_JNI_GetPrimitiveArrayCritical,
  _Jv_JNI_ReleasePrimitiveArrayCritical,
  _Jv_JNI_GetStringCritical,
  _Jv_JNI_ReleaseStringCritical,

  NOT_IMPL /* newweakglobalref */,
  NOT_IMPL /* deleteweakglobalref */,

  _Jv_JNI_ExceptionCheck
};
