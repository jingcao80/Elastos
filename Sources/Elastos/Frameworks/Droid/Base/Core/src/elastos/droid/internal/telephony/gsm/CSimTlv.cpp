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

#include "elastos/droid/internal/telephony/gsm/CSimTlv.h"

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace Gsm {

CAR_INTERFACE_IMPL(CSimTlv, Object, ISimTlv)

CAR_OBJECT_IMPL(CSimTlv)

CSimTlv::CSimTlv()
{
}

ECode CSimTlv::constructor()
{
    return NOERROR;
}

ECode CSimTlv::constructor(
    /* [in] */ ArrayOf<Byte>* record,
    /* [in] */ Int32 offset,
    /* [in] */ Int32 length)
{
    mRecord = record;

    mTlvOffset = offset;
    mTlvLength = length;
    mCurOffset = offset;

    mHasValidTlvObject = ParseCurrentTlvObject();
    return NOERROR;
}

ECode CSimTlv::NextObject(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    if (!mHasValidTlvObject) {
        *result = FALSE;
    }

    mCurOffset = mCurDataOffset + mCurDataLength;
    mHasValidTlvObject = ParseCurrentTlvObject();
    *result = mHasValidTlvObject;
    return NOERROR;
}

ECode CSimTlv::IsValidObject(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mHasValidTlvObject;
    return NOERROR;
}

ECode CSimTlv::GetTag(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    if (!mHasValidTlvObject) {
        *result = 0;
    }
    *result = (*mRecord)[mCurOffset] & 0xff;
    return NOERROR;
}

ECode CSimTlv::GetData(
    /* [out, callee] */ ArrayOf<Byte>** result)
{
    VALIDATE_NOT_NULL(result)
    if (!mHasValidTlvObject) {
        *result = NULL;
        return NOERROR;
    }

    AutoPtr<ArrayOf<Byte> > ret = ArrayOf<Byte>::Alloc(mCurDataLength);
    ret->Copy(0, mRecord, mCurDataOffset, mCurDataLength);
    *result = ret;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

Boolean CSimTlv::ParseCurrentTlvObject()
{
    // 0x00 and 0xff are invalid tag values

    if (mCurOffset >= mRecord->GetLength()) {
        return FALSE;
    }
    // try {
    if ((*mRecord)[mCurOffset] == 0 || ((*mRecord)[mCurOffset] & 0xff) == 0xff) {
        return FALSE;
    }

    if (((*mRecord)[mCurOffset + 1] & 0xff) < 0x80) {
        // one byte length 0 - 0x7f
        mCurDataLength = (*mRecord)[mCurOffset + 1] & 0xff;
        mCurDataOffset = mCurOffset + 2;
    }
    else if (((*mRecord)[mCurOffset + 1] & 0xff) == 0x81) {
        // two byte length 0x80 - 0xff
        mCurDataLength = (*mRecord)[mCurOffset + 2] & 0xff;
        mCurDataOffset = mCurOffset + 3;
    }
    else {
        return FALSE;
    }
    // } catch (ArrayIndexOutOfBoundsException ex) {
    //     return FALSE;
    // }

    if (mCurDataLength + mCurDataOffset > mTlvOffset + mTlvLength) {
        return FALSE;
    }

    return TRUE;
}



} // namespace Gsm
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos