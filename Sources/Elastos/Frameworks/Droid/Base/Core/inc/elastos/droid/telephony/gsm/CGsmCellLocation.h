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

#ifndef __ELASTOS_DROID_TELEPHONY_GSM_CGSMCELLLOCATION_H__
#define __ELASTOS_DROID_TELEPHONY_GSM_CGSMCELLLOCATION_H__

#include "Elastos.Droid.Os.h"
#include "_Elastos_Droid_Telephony_Gsm_CGsmCellLocation.h"
#include "elastos/droid/telephony/CellLocation.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Os::IBundle;

namespace Elastos {
namespace Droid {
namespace Telephony {
namespace Gsm {

CarClass(CGsmCellLocation)
    , public CellLocation
    , public IGsmCellLocation
{
public:
    CGsmCellLocation();

    virtual ~CGsmCellLocation();

    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ IBundle* bundle);

    CARAPI FillInNotifierBundle(
        /* [in] */ IBundle* bundle);

    CARAPI IsEmpty(
        /* [out] */ Boolean* result);

    CARAPI GetLac(
        /* [out] */ Int32* lac);

    CARAPI GetCid(
        /* [out] */ Int32* cid);

    CARAPI GetPsc(
        /* [out] */ Int32* psc);

    CARAPI SetLac(
        /* [in] */ Int32 lac);

    CARAPI SetCid(
        /* [in] */ Int32 cid);

    CARAPI SetPsc(
        /* [in] */ Int32 psc);

    CARAPI SetStateInvalid();

    CARAPI SetLacAndCid(
        /* [in] */ Int32 lac,
        /* [in] */ Int32 cid);

    CARAPI GetHashCode(
        /* [out] */ Int32* hashCode);

    CARAPI Equals(
        /* [in] */ IInterface* o,
        /* [out] */ Boolean* result);

    CARAPI ToString(
        /* [out] */ String* str);

private:

    static CARAPI_(Boolean) EqualsHandlesNulls(
        /* [in] */ Int32 a,
        /* [in] */ Int32 b);

    Int32 mLac;
    Int32 mCid;
    Int32 mPsc;
};

} // namespace Gsm
} // namespace Telephony
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_TELEPHONY_GSM_CGSMCELLLOCATION_H__
