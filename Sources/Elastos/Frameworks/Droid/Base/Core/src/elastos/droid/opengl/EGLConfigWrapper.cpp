
#include "elastos/droid/opengl/EGLConfigWrapper.h"

namespace Elastos {
namespace Droid {
namespace Opengl {
CAR_INTERFACE_IMPL(EGLConfigWrapper, EGLObjectHandle, IEGLConfig)

EGLConfigWrapper::EGLConfigWrapper(
    /* [in] */ Int64 handle)
    : EGLObjectHandle(handle)
{}

ECode EGLConfigWrapper::Equals(
    /* [in] */ IInterface* o,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    if(o == NULL || o->Probe(EIID_IEGLConfig) == NULL) {
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

AutoPtr<IEGLConfig> EGLConfigWrapper::CreateInstance(
    /* [in] */ Int64 handle)
{
    AutoPtr<IEGLConfig> instance = new EGLConfigWrapper(handle);
    return instance;
}

} // namespace Opengl
} // namespace Droid
} // namespace Elastos
