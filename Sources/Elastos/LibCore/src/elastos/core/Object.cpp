//=========================================================================
// Copyright (C) 2012 The Elastos Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//=========================================================================

#include "Object.h"
#include "Thread.h"
#include "ClassLoader.h"
#include <utils/Log.h>

using Elastos::Core::Thread;
using Elastos::Core::IThread;
using Elastos::Core::EIID_ISynchronize;

namespace Elastos {
namespace Core {

static Boolean DEBUG = FALSE;

Object::Object()
{
    IncrementDllLockCount();
    mNativeObject = NativeCreateObject();
    mNativeObject->mObjectObj = reinterpret_cast<Int64>(this);
}

Object::~Object()
{
    NativeDestroyObject(mNativeObject);
    DecrementDllLockCount();
}

ECode Object::constructor()
{
    return NOERROR;
}

UInt32 Object::AddRef()
{
    return ElRefBase::AddRef();
}

UInt32 Object::Release()
{
    return ElRefBase::Release();
}

PInterface Object::Probe(
    /* [in] */ REIID riid)
{
    if (riid == EIID_IInterface) {
        return (IInterface*)(IObject*)this;
    }
    else if (riid == EIID_IObject) {
        return (IObject*)this;
    }
    else if (riid == EIID_ISynchronize) {
        return (ISynchronize*)this;
    }
    else if (riid == EIID_IWeakReferenceSource) {
        return (IWeakReferenceSource*)this;
    }
    return NULL;
}

ECode Object::GetInterfaceID(
    /* [in] */ IInterface* object,
    /* [out] */ InterfaceID* iid)
{
    VALIDATE_NOT_NULL(iid);
    if (object == (IInterface*)(IObject*)this) {
        *iid = EIID_IObject;
    }
    else if (object == (IInterface*)(ISynchronize*)this) {
        *iid = EIID_ISynchronize;
    }
    else if (object == (IInterface*)(IWeakReferenceSource*)this) {
        *iid = EIID_IWeakReferenceSource;
    }
    else {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    return NOERROR;
}

ECode Object::Aggregate(
    /* [in] */ AggregateType type,
    /* [in] */ IInterface* object)
{
    return E_NOT_IMPLEMENTED;
}

ECode Object::GetDomain(
    /* [out] */ IInterface** object)
{
    return E_NOT_IMPLEMENTED;
}

ECode Object::GetClassID(
    /* [out] */ ClassID* clsid)
{
    return E_NOT_IMPLEMENTED;
}

ECode Object::GetClassInfo(
    /* [out] */ IInterface** clsInfo)
{
    VALIDATE_NOT_NULL(clsInfo);

    if (mClassInfo == NULL) {
        _CObject_ReflectClassInfo((IObject*)this, (IClassInfo**)&mClassInfo);
        if (mClassInfo == NULL) {
            if (DEBUG) ALOGD("error: failed to get class info of Object[0x%08x]."
                    " It is not a Car class.", this);
        }
    }

    *clsInfo = mClassInfo;
    REFCOUNT_ADD(*clsInfo);
    return NOERROR;
}

ECode Object::GetHashCode(
    /* [out] */ Int32* hashCode)
{
    VALIDATE_NOT_NULL(hashCode);
    *hashCode = (Int32)this;
    return NOERROR;
}

ECode Object::Equals(
    /* [in] */ IInterface* other,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = (IObject*)this == IObject::Probe(other);
    return NOERROR;
}

ECode Object::ToString(
    /* [out] */ String* info)
{
    VALIDATE_NOT_NULL(info);
    AutoPtr<IClassInfo> classInfo = GetClassInfo((IObject*)this);
    String className("--");
    if (classInfo != NULL) {
        classInfo->GetName(&className);
    }
    info->AppendFormat("Object[0x%08x], Class[%s]", this, className.string());
    return NOERROR;
}

ECode Object::Lock()
{
    // the following codes should be remove
    AutoPtr<IThread> t;
    Thread::Attach((IThread**)&t);
    return NativeLockObject(mNativeObject);
}

ECode Object::Unlock()
{
    // the following codes should be remove
    AutoPtr<IThread> t;
    Thread::Attach((IThread**)&t);
    return NativeUnlockObject(mNativeObject);
}

ECode Object::Notify()
{
    return NativeObjectNotify(mNativeObject);
}

ECode Object::NotifyAll()
{
    return NativeObjectNotifyAll(mNativeObject);
}

ECode Object::Wait()
{
    return Wait(0, 0);
}

ECode Object::Wait(
    /* [in] */ Int64 millis)
{
    return Wait(millis, 0);
}

ECode Object::Wait(
    /* [in] */ Int64 millis,
    /* [in] */ Int32 nanos)
{
    return NativeObjectWait(mNativeObject, millis, nanos, TRUE);
}

ECode Object::GetWeakReference(
    /* [out] */ IWeakReference** weakReference)
{
    VALIDATE_NOT_NULL(weakReference);
    *weakReference = new WeakReferenceImpl((IObject*)this, CreateWeak(this));
    REFCOUNT_ADD(*weakReference)
    return NOERROR;
}

AutoPtr<IClassInfo> Object::GetClassInfo(
    /* [in] */ IInterface* obj)
{
    IObject* iObject = IObject::Probe(obj);
    if (iObject == NULL) {
        if (DEBUG) ALOGD("error: failed to get class info of Object[0x%08x]. It is not a Car class.", obj);
        return NULL;
    }

    AutoPtr<IClassInfo> classInfo;
    iObject->GetClassInfo((IInterface**)&classInfo);
    return classInfo;
}

AutoPtr<IClassLoader> Object::GetClassLoader(
    /* [in] */ IInterface* obj)
{
    AutoPtr<IClassInfo> classInfo = GetClassInfo(obj);
    if (classInfo == NULL) return NULL;

    AutoPtr<IClassLoader> classLoader;
    classLoader = ClassLoader::GetClassLoader(classInfo);
    return classLoader;
}

String Object::GetClassName(
    /* [in] */ IInterface* obj)
{
    String className;

    AutoPtr<IClassInfo> classInfo = GetClassInfo(obj);
    if (classInfo == NULL) return className;

    classInfo->GetName(&className);
    return className;
}

String Object::GetFullClassName(
    /* [in] */ IInterface* obj)
{
    String fullClassName;

    AutoPtr<IClassInfo> classInfo = GetClassInfo(obj);
    if (classInfo == NULL) return fullClassName;

    String className, ns;
    classInfo->GetName(&className);
    classInfo->GetNamespace(&ns);
    fullClassName = ns + String(".") + className;
    return fullClassName;
}

String Object::GetNamespace(
    /* [in] */ IInterface* obj)
{
    String ns;

    AutoPtr<IClassInfo> classInfo = GetClassInfo(obj);
    if (classInfo == NULL) return ns;

    classInfo->GetNamespace(&ns);
    return ns;
}

AutoPtr<IModuleInfo> Object::GetModuleInfo(
    /* [in] */ IInterface* obj)
{
    AutoPtr<IModuleInfo> moduleInfo;
    _CObject_ReflectModuleInfo(obj, (IModuleInfo**)&moduleInfo);
    return moduleInfo;
}

String Object::GetModulePath(
    /* [in] */ IInterface* obj)
{
    AutoPtr<IModuleInfo> moduleInfo = GetModuleInfo(obj);
    String path;
    if (moduleInfo != NULL) {
        moduleInfo->GetPath(&path);
    }
    else {
        if (DEBUG) ALOGD("error: failed to GetModulePath with %s. It is not a Car class.", TO_CSTR(obj));
    }
    return path;
}


} // namespace Core
} // namespace Elastos
