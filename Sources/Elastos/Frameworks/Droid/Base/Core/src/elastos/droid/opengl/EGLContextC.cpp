
#include "EGLContextC.h"

namespace Elastos {
namespace Droid {
namespace Opengl {

EGLContextC::EGLContextC(
    /* [in] */ Int32 handle) : EGLObjectHandle(handle)
{}

PInterface EGLContextC::Probe(
    /* [in] */ REIID riid)
{
    if (riid == EIID_IInterface) {
        return this;
    } else if (riid == EIID_IEGLContext) {
        return this;
    } else if (riid == EIID_IEGLObjectHandle) {
        return this;
    } else if (riid == EIID_EGLObjectHandle) {
        return reinterpret_cast<PInterface>((EGLObjectHandle*)this);
    }
    return NULL;
}

UInt32 EGLContextC::AddRef()
{
    return ElRefBase::AddRef();
}

UInt32 EGLContextC::Release()
{
    return ElRefBase::Release();
}

ECode EGLContextC::GetInterfaceID(
    /* [in] */ IInterface* object,
    /* [out] */ InterfaceID* iid)
{
    if (object == (IInterface*)(this)) {
        *iid = EIID_IEGLContext;
        return NOERROR;
    } else if (object == reinterpret_cast<PInterface>((EGLObjectHandle*)this)) {
        *iid = EIID_EGLObjectHandle;
        return NOERROR;
    }
    return E_ILLEGAL_ARGUMENT_EXCEPTION;
}

ECode EGLContextC::GetHandle(
    /* [out] */ Int32* handle)
{
    return EGLObjectHandle::GetHandle(handle);
}

ECode EGLContextC::GetHashCode(
    /* [out] */ Int32* code)
{
    return EGLObjectHandle::GetHashCode(code);
}

ECode EGLContextC::Equals(
    /* [in] */ IInterface* o,
    /* [out] */ Boolean* result)
{
    if(o == NULL || o->Probe(EIID_IEGLContext) == NULL) {
        *result = FALSE;
    } else if (Probe(EIID_IInterface) == o || Probe(EIID_EGLObjectHandle) == o) {
        *result = TRUE;
    } else {
        AutoPtr<IEGLContext> display = IEGLContext::Probe(o);
        Int32 handle, thisHanlde;
        display->GetHandle(&handle);
        GetHandle(&thisHanlde);
        *result = (handle == thisHanlde);
    }
    return NOERROR;
}

AutoPtr<IEGLContext> EGLContextC::CreateInstance(
    /* [in] */ Int32 handle)
{
    AutoPtr<IEGLContext> instance = new EGLContextC(handle);
    return instance;
}

} // namespace Opengl
} // namespace Droid
} // namespace Elastos

