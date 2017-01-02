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

#include "Elastos.Droid.Internal.h"
#include "elastos/droid/internal/telephony/uicc/IccIoResult.h"
#include "elastos/droid/internal/telephony/uicc/CIccUtils.h"
#include "elastos/droid/internal/telephony/uicc/IccException.h"
#include "elastos/droid/internal/telephony/uicc/IccFileTypeMismatch.h"
#include "elastos/droid/internal/telephony/uicc/IccFileNotFound.h"

#include <elastos/core/StringUtils.h>

using Elastos::Core::StringUtils;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace Uicc {

//=====================================================================
//                             IccIoResult
//=====================================================================
CAR_INTERFACE_IMPL(IccIoResult, Object, IIccIoResult);

IccIoResult::IccIoResult()
{
}

ECode IccIoResult::constructor(
    /* [in] */ Int32 sw1,
    /* [in] */ Int32 sw2,
    /* [in] */ ArrayOf<Byte>* payload)
{
    mSw1 = sw1;
    mSw2 = sw2;
    mPayload = payload;
    return NOERROR;
}

ECode IccIoResult::constructor(
    /* [in] */ Int32 sw1,
    /* [in] */ Int32 sw2,
    /* [in] */ const String& hexString)
{
    AutoPtr<IIccUtils> iccu;
    CIccUtils::AcquireSingleton((IIccUtils**)&iccu);
    AutoPtr<ArrayOf<Byte> > res;
    iccu->HexStringToBytes(hexString, (ArrayOf<Byte>**)&res);
    constructor(sw1, sw2, res);
    return NOERROR;
}

ECode IccIoResult::ToString(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    *result = String("IccIoResponse sw1:0x") + StringUtils::ToString(mSw1)
            + String(" sw2:0x") + StringUtils::ToString(mSw2);
    return NOERROR;
}

ECode IccIoResult::Success(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mSw1 == 0x90 || mSw1 == 0x91 || mSw1 == 0x9e || mSw1 == 0x9f;
    return NOERROR;
}

ECode IccIoResult::GetException(
    /* [out] */ IIccException** result)
{
    VALIDATE_NOT_NULL(result);
    Boolean b = FALSE;
    Success(&b);
    if (b) {
        *result = NULL;
        return NOERROR;
    }

    switch (mSw1) {
        case 0x94:
            if (mSw2 == 0x08) {
                AutoPtr<IccFileTypeMismatch> p = new IccFileTypeMismatch();
                *result = IIccException::Probe(p);
                REFCOUNT_ADD(*result)
                return NOERROR;
            }
            else {
                AutoPtr<IccFileNotFound> p = new IccFileNotFound();
                *result = IIccException::Probe(p);
                REFCOUNT_ADD(*result)
                return NOERROR;
            }
        default: {
            AutoPtr<IccException> p = new IccException(String("sw1:") + StringUtils::ToString(mSw1)
                                        + String(" sw2:") + StringUtils::ToString(mSw2));
            *result = p;
            REFCOUNT_ADD(*result)
            return NOERROR;
        }
    }
    return NOERROR;
}

} // namespace Uicc
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos
