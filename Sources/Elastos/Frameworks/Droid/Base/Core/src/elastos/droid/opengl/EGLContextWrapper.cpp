
#include "elastos/droid/opengl/EGLContextWrapper.h"

namespace Elastos {
namespace Droid {
namespace Opengl {
CAR_INTERFACE_IMPL(EGLContextWrapper, EGLObjectHandle, IEGLContext)

EGLContextWrapper::EGLContextWrapper(
    /* [in] */ Int64 handle)
    : EGLObjectHandle(handle)
{}

ECode EGLContextWrapper::Equals(
    /* [in] */ IInterface* o,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    if(o == NULL || o->Probe(EIID_IEGLContext) == NULL) {
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

AutoPtr<IEGLContext> EGLContextWrapper::CreateInstance(
    /* [in] */ Int64 handle)
{
    AutoPtr<IEGLContext> instance = new EGLContextWrapper(handle);
    return instance;
}

} // namespace Opengl
} // namespace Droid
} // namespace Elastos

