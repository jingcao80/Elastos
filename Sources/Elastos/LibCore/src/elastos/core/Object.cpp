
#include "Object.h"
#include "Thread.h"

using Elastos::Core::Thread;
using Elastos::Core::IThread;
using Elastos::Core::EIID_ISynchronize;

namespace Elastos {
namespace Core {

Object::Object()
{
    IncrementDllLockCount();
    mNativeObject = NativeCreateObject();
    mNativeObject->mObjectObj = reinterpret_cast<Int32>(this);
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
    *result = THIS_PROBE(IObject) == IObject::Probe(other);
    return NOERROR;
}

ECode Object::ToString(
    /* [out] */ String* info)
{
    VALIDATE_NOT_NULL(info);
    AutoPtr<IClassInfo> classInfo;
    _CObject_ReflectClassInfo(THIS_PROBE(IInterface), (IClassInfo**)&classInfo);
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
    *weakReference = new WeakReferenceImpl(THIS_PROBE(IInterface), CreateWeak(this));
    (*weakReference)->AddRef();
    return NOERROR;
}

} // namespace Core
} // namespace Elastos
