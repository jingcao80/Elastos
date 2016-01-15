
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/mtp/CMtpPropertyList.h"

using Elastos::Droid::Mtp::IMtpConstants;

namespace Elastos {
namespace Droid {
namespace Mtp {

CMtpPropertyList::CMtpPropertyList()
    : mResult(0)
    , mCount(0)
    , mMaxCount(0)
{}

CAR_INTERFACE_IMPL(CMtpPropertyList, Object, IMtpPropertyList)

CAR_OBJECT_IMPL(CMtpPropertyList)

ECode CMtpPropertyList::constructor(
    /* [in] */ Int32 maxCount,
    /* [in] */ Int32 result)
{
    mMaxCount = maxCount;
    mResult = result;
    mObjectHandles = ArrayOf<Int32>::Alloc(maxCount);
    mPropertyCodes = ArrayOf<Int32>::Alloc(maxCount);
    mDataTypes = ArrayOf<Int32>::Alloc(maxCount);
    // mLongValues and mStringValues are created lazily since both might not be necessary
    return NOERROR;
}

ECode CMtpPropertyList::Append(
        /* [in] */ Int32 handle,
        /* [in] */ Int32 property,
        /* [in] */ Int32 type,
        /* [in] */ Int64 value)
{
    Int32 index = mCount++;
    if (mLongValues == NULL) {
        mLongValues = ArrayOf<Int64>::Alloc(mMaxCount);
    }
    mObjectHandles->Set(index, handle);
    mPropertyCodes->Set(index, property);
    mDataTypes->Set(index, type);
    mLongValues->Set(index, value);

    return NOERROR;
}

ECode CMtpPropertyList::Append(
        /* [in] */ Int32 handle,
        /* [in] */ Int32 property,
        /* [in] */ const String& value)
{
    Int32 index = mCount++;
    if (mStringValues == NULL) {
        mStringValues = ArrayOf<String>::Alloc(mMaxCount);
    }
    mObjectHandles->Set(index, handle);
    mPropertyCodes->Set(index, property);
    mDataTypes->Set(index, IMtpConstants::TYPE_STR);
    mStringValues->Set(index, value);

    return NOERROR;
}

ECode CMtpPropertyList::SetResult(
        /* [in] */ Int32 result)
{
    VALIDATE_NOT_NULL(result);
    mResult = result;

    return NOERROR;
}

} // namespace Mtp
} // namepsace Droid
} // namespace Elastos
