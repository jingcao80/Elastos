//=========================================================================
// Copyright (C) 2012 The Elastos Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//=========================================================================

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
