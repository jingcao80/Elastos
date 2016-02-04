#include "elastos/droid/opengl/EGLObjectHandle.h"

namespace Elastos {
namespace Droid {
namespace Opengl {

CAR_INTERFACE_IMPL(EGLObjectHandle, Object, IEGLObjectHandle)

ECode EGLObjectHandle::GetHandle(
    /* [out] */ Int32* handle)
{
    VALIDATE_NOT_NULL(handle)

    *handle = mHandle;
    return NOERROR;
}

ECode EGLObjectHandle::GetNativeHandle(
    /* [out] */ Int64* handle)
{
    *handle = mHandle;
    return NOERROR;
}

ECode EGLObjectHandle::GetHashCode(
    /* [out] */ Int32* hashCode)
{
    VALIDATE_NOT_NULL(hashCode)

    if ((mHandle & 0xffffffffL) != mHandle) {
        return E_UNSUPPORTED_OPERATION_EXCEPTION;
    }

    *hashCode = (Int32)mHandle;
    return NOERROR;
}

EGLObjectHandle::EGLObjectHandle(
    /* [in] */ Int32 handle)
{
    mHandle = handle;
}

EGLObjectHandle::EGLObjectHandle(
    /* [in] */ Int64 handle)
{
    mHandle = handle;
}

} // namespace Opengl
} // namespace Droid
} // namespace Elastos