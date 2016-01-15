
#include "EGLConfigC.h"

namespace Elastos {
namespace Droid {
namespace Opengl {

EGLConfigC::EGLConfigC(
    /* [in] */ Int32 handle) : EGLObjectHandle(handle)
{}

PInterface EGLConfigC::Probe(
    /* [in] */ REIID riid)
{
    if (riid == EIID_IInterface) {
        return this;
    } else if (riid == EIID_IEGLConfig) {
        return this;
    } else if (riid == EIID_IEGLObjectHandle) {
        return this;
    } else if (riid == EIID_EGLObjectHandle) {
        return reinterpret_cast<PInterface>((EGLObjectHandle*)this);
    }
    return NULL;
}

UInt32 EGLConfigC::AddRef()
{
    return ElRefBase::AddRef();
}

UInt32 EGLConfigC::Release()
{
    return ElRefBase::Release();
}

ECode EGLConfigC::GetInterfaceID(
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

ECode EGLConfigC::GetHandle(
    /* [out] */ Int32* handle)
{
    return EGLObjectHandle::GetHandle(handle);
}

ECode EGLConfigC::GetHashCode(
    /* [out] */ Int32* code)
{
    return EGLObjectHandle::GetHashCode(code);
}

ECode EGLConfigC::Equals(
    /* [in] */ IInterface* o,
    /* [out] */ Boolean* result)
{
    if(o == NULL || o->Probe(EIID_IEGLConfig) == NULL) {
        *result = FALSE;
    } else if (Probe(EIID_IInterface) == o || Probe(EIID_EGLObjectHandle) == o) {
        *result = TRUE;
    } else {
        AutoPtr<IEGLConfig> display = IEGLConfig::Probe(o);
        Int32 handle, thisHanlde;
        display->GetHandle(&handle);
        GetHandle(&thisHanlde);
        *result = (handle == thisHanlde);
    }
    return NOERROR;
}

AutoPtr<IEGLConfig> EGLConfigC::CreateInstance(
    /* [in] */ Int32 handle)
{
    AutoPtr<IEGLConfig> instance = new EGLConfigC(handle);
    return instance;
}

} // namespace Opengl
} // namespace Droid
} // namespace Elastos
