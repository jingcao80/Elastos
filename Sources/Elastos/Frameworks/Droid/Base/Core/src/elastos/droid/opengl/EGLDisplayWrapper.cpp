
#include "elastos/droid/opengl/EGLDisplayWrapper.h"

namespace Elastos {
namespace Droid {
namespace Opengl {

CAR_INTERFACE_IMPL(EGLDisplayWrapper, EGLObjectHandle, IEGLDisplay)

ECode EGLDisplayWrapper::Equals(
    /* [in] */ IInterface* o,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    if(o == NULL || o->Probe(EIID_IEGLDisplay) == NULL) {
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

AutoPtr<IEGLDisplay> EGLDisplayWrapper::CreateInstance(
    /* [in] */ Int64 handle)
{
    AutoPtr<IEGLDisplay> instance = new EGLDisplayWrapper(handle);
    return instance;
}

EGLDisplayWrapper::EGLDisplayWrapper(
    /* [in] */ Int64 handle)
    : EGLObjectHandle(handle)
{}

} // namespace Opengl
} // namespace Droid
} // namespace Elastos