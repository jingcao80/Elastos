
#include "elastos/droid/internal/utility/CVirtualRefBasePtr.h"
#include <utils/RefBase.h>

using android::VirtualLightRefBase;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Utility {

CAR_INTERFACE_IMPL(CVirtualRefBasePtr, Object, IVirtualRefBasePtr)

CAR_OBJECT_IMPL(CVirtualRefBasePtr)

CVirtualRefBasePtr::~CVirtualRefBasePtr()
{
    ReleasePtr();
}

ECode CVirtualRefBasePtr::constructor(
        /* [in] */ Int64 ptr)
{
    mNativePtr = ptr;
    VirtualLightRefBase* obj = reinterpret_cast<VirtualLightRefBase*>(mNativePtr);
    obj->incStrong(0);
    return NOERROR;
}

ECode CVirtualRefBasePtr::Get(
    /* [in] */ Int64* ptr)
{
    VALIDATE_NOT_NULL(ptr)
    *ptr = mNativePtr;
    return NOERROR;
}

ECode CVirtualRefBasePtr::ReleasePtr()
{
    if (mNativePtr != 0) {
        VirtualLightRefBase* obj = reinterpret_cast<VirtualLightRefBase*>(mNativePtr);
        obj->decStrong(0);
        mNativePtr = 0;
    }
    return NOERROR;
}

} // namespace Utility
} // namespace Internal
} // namespace Droid
} // namespace Elastos
