
#include "EGLDisplayC.h"

namespace Elastos {
namespace Droid {
namespace Opengl {

EGLDisplayC::EGLDisplayC(
    /* [in] */ Int32 handle) : EGLObjectHandle(handle)
{}

PInterface EGLDisplayC::Probe(
    /* [in] */ REIID riid)
{
    if (riid == EIID_IInterface) {
        return this;
    } else if (riid == EIID_IEGLDisplay) {
        return this;
    } else if (riid == EIID_IEGLObjectHandle) {
        return this;
    } else if (riid == EIID_EGLObjectHandle) {
        return reinterpret_cast<PInterface>((EGLObjectHandle*)this);
    }
    return NULL;
}

UInt32 EGLDisplayC::AddRef()
{
    return ElRefBase::AddRef();
}

UInt32 EGLDisplayC::Release()
{
    return ElRefBase::Release();
}

ECode EGLDisplayC::GetInterfaceID(
    /* [in] */ IInterface* object,
    /* [out] */ InterfaceID* iid)
{
    if (object == (IInterface*)(this)) {
        *iid = EIID_IEGLObjectHandle;
        return NOERROR;
    } else if (object == reinterpret_cast<PInterface>((EGLObjectHandle*)this)) {
        *iid = EIID_EGLObjectHandle;
        return NOERROR;
    }
    return E_ILLEGAL_ARGUMENT_EXCEPTION;
}

ECode EGLDisplayC::GetHandle(
    /* [out] */ Int32* handle)
{
    return EGLObjectHandle::GetHandle(handle);
}

ECode EGLDisplayC::GetHashCode(
    /* [out] */ Int32* code)
{
    return EGLObjectHandle::GetHashCode(code);
}

ECode EGLDisplayC::Equals(
    /* [in] */ IInterface* o,
    /* [out] */ Boolean* result)
{
    if(o == NULL || o->Probe(EIID_IEGLDisplay) == NULL) {
        *result = FALSE;
    } else if (Probe(EIID_IInterface) == o || Probe(EIID_EGLObjectHandle) == o) {
        *result = TRUE;
    } else {
        AutoPtr<IEGLDisplay> display = IEGLDisplay::Probe(o);
        Int32 handle, thisHanlde;
        display->GetHandle(&handle);
        GetHandle(&thisHanlde);
        *result = (handle == thisHanlde);
    }
    return NOERROR;
}

AutoPtr<IEGLDisplay> EGLDisplayC::CreateInstance(
    /* [in] */ Int32 handle)
{
    AutoPtr<IEGLDisplay> instance = new EGLDisplayC(handle);
    return instance;
}

} // namespace Opengl
} // namespace Droid
} // namespace Elastos