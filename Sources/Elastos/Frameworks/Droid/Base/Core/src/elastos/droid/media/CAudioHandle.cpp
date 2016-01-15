#include "elastos/droid/media/CAudioHandle.h"
#include <elastos/core/StringUtils.h>

using Elastos::Core::StringUtils;

namespace Elastos {
namespace Droid {
namespace Media {

CAR_INTERFACE_IMPL(CAudioHandle, Object, IAudioHandle)

CAR_OBJECT_IMPL(CAudioHandle)

CAudioHandle::CAudioHandle()
    : mId(0)
{
}

CAudioHandle::~CAudioHandle()
{
}

ECode CAudioHandle::constructor(
    /* [in] */ Int32 id)
{
    mId = id;
    return NOERROR;
}

ECode CAudioHandle::Id(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mId;
    return NOERROR;
}

ECode CAudioHandle::GetHashCode(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mId;
    return NOERROR;
}

ECode CAudioHandle::Equals(
    /* [in] */ IInterface* o,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    if (o == NULL || IAudioHandle::Probe(o) == NULL) {
        *result = FALSE;
        return NOERROR;
    }
    AutoPtr<CAudioHandle> ah = (CAudioHandle*)(IObject*)o;
    *result = mId == ah->mId;
    return NOERROR;
}

ECode CAudioHandle::ToString(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    *result = StringUtils::ToString(mId);
    return NOERROR;
}

} // namespace Media
} // namepsace Droid
} // namespace Elastos
