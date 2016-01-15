#include "EGLObjectHandle.h"

namespace Elastos {
namespace Droid {
namespace Opengl {

// {ad22a807-37df-4a06-a749-b6a7b4a9ce18}
extern "C" const InterfaceID EIID_EGLObjectHandle =
        { 0xad22a807, 0x37df, 0x4a06, { 0xa7, 0x49, 0xb6, 0xa7, 0xb4, 0xa9, 0xce, 0x18 } };

ECode EGLObjectHandle::GetHandle(
    /* [out] */ Int32* handle)
{
    *handle = mHandle;
    return NOERROR;
}

ECode EGLObjectHandle::GetHashCode(
    /* [out] */ Int32* hashCode)
{
    *hashCode = mHandle;
    return NOERROR;
}

EGLObjectHandle::EGLObjectHandle(
    /* [in] */ Int32 handle)
{
    mHandle = handle;
}

} // namespace Opengl
} // namespace Droid
} // namespace Elastos