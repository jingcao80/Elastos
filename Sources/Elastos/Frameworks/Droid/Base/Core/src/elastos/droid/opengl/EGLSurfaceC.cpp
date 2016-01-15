
#include "EGLSurfaceC.h"

namespace Elastos {
namespace Droid {
namespace Opengl {

EGLSurfaceC::EGLSurfaceC(
    /* [in] */ Int32 handle) : EGLObjectHandle(handle)
{}

PInterface EGLSurfaceC::Probe(
    /* [in] */ REIID riid)
{
    if (riid == EIID_IInterface) {
        return this;
    } else if (riid == EIID_IEGLSurface) {
        return this;
    } else if (riid == EIID_IEGLObjectHandle) {
        return this;
    } else if (riid == EIID_EGLObjectHandle) {
        return reinterpret_cast<PInterface>((EGLObjectHandle*)this);
    }
    return NULL;
}

UInt32 EGLSurfaceC::AddRef()
{
    return ElRefBase::AddRef();
}

UInt32 EGLSurfaceC::Release()
{
    return ElRefBase::Release();
}

ECode EGLSurfaceC::GetInterfaceID(
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

ECode EGLSurfaceC::GetHandle(
    /* [out] */ Int32* handle)
{
    return EGLObjectHandle::GetHandle(handle);
}

ECode EGLSurfaceC::GetHashCode(
    /* [out] */ Int32* code)
{
    return EGLObjectHandle::GetHashCode(code);
}

ECode EGLSurfaceC::Equals(
    /* [in] */ IInterface* o,
    /* [out] */ Boolean* result)
{
    if(o == NULL || o->Probe(EIID_IEGLSurface) == NULL) {
        *result = FALSE;
    } else if (Probe(EIID_IInterface) == o || Probe(EIID_EGLObjectHandle) == o) {
        *result = TRUE;
    } else {
        AutoPtr<IEGLSurface> display = IEGLSurface::Probe(o);
        Int32 handle, thisHanlde;
        display->GetHandle(&handle);
        GetHandle(&thisHanlde);
        *result = (handle == thisHanlde);
    }
    return NOERROR;
}

AutoPtr<IEGLSurface> EGLSurfaceC::CreateInstance(
    /* [in] */ Int32 handle)
{
    AutoPtr<IEGLSurface> instance = new EGLSurfaceC(handle);
    return instance;
}


} // namespace Opengl
} // namespace Droid
} // namespace Elastos