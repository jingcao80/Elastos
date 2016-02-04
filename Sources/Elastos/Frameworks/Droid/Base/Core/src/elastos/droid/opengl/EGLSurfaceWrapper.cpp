
#include "elastos/droid/opengl/EGLSurfaceWrapper.h"

namespace Elastos {
namespace Droid {
namespace Opengl {

CAR_INTERFACE_IMPL(EGLSurfaceWrapper, EGLObjectHandle, IEGLSurface)

EGLSurfaceWrapper::EGLSurfaceWrapper(
    /* [in] */ Int64 handle) : EGLObjectHandle(handle)
{}

ECode EGLSurfaceWrapper::Equals(
    /* [in] */ IInterface* o,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    if(o == NULL || o->Probe(EIID_IEGLSurface) == NULL) {
        *result = FALSE;
    } else if (Probe(EIID_IInterface) == o) {
        *result = TRUE;
    } else {
        AutoPtr<IEGLObjectHandle> display = IEGLObjectHandle::Probe(o);
        Int64 handle, thisHanlde;
        display->GetNativeHandle(&handle);
        GetNativeHandle(&thisHanlde);
        *result = (handle == thisHanlde);
    }
    return NOERROR;
}

AutoPtr<IEGLSurface> EGLSurfaceWrapper::CreateInstance(
    /* [in] */ Int64 handle)
{
    AutoPtr<IEGLSurface> instance = new EGLSurfaceWrapper(handle);
    return instance;
}


} // namespace Opengl
} // namespace Droid
} // namespace Elastos