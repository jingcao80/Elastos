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

#ifndef __ELASTOS_DROID_TELEPHONY_CCELLINFO_H__
#define __ELASTOS_DROID_TELEPHONY_CCELLINFO_H__

#include "_Elastos_Droid_Telephony_CCellInfo.h"
#include "elastos/droid/ext/frameworkdef.h"

using Elastos::Droid::Net::IUri;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Database::ICursor;
using Elastos::Droid::Graphics::IBitmap;
using Elastos::Droid::Graphics::Drawable::IDrawable;

namespace Elastos {
namespace Droid {
namespace Telephony {

CarClass(CCellInfo)
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI ReadFromParcel(
        /* [in] */ IParcel* source);

    CARAPI WriteToParcel(
        /* [in] */ IParcel* dest);

    CARAPI IsRegistered(
        /* [out] */ Boolean* res);

    CARAPI SetRegisterd(
        /* [in] */ Boolean registered);

    CARAPI GetTimeStamp(
        /* [out] */ Int64* res);

    CARAPI SetTimeStamp(
        /* [in] */ Int64 timeStamp);

    CARAPI GetTimeStampType(
        /* [out] */ Int32* res);

    CARAPI SetTimeStampType(
        /* [in] */ Int32 timeStampType);

    CARAPI GetHashCode(
        /* [out] */ Int32* res);

    CARAPI Equals(
        /* [in] */ IInterface* other,
        /* [out] */ Boolean* res);

    CARAPI ToString(
        /* [out] */ String* res);

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ ICellInfo* ci);

private:
    static TimeStampTypeToString(
        /* [in] */ Int32 type,
        /* [out] */ String* res);

    // True if device is mRegistered to the mobile network
    Boolean mRegistered;

    // Observation time stamped as type in nanoseconds since boot
    Int64 mTimeStamp;

    // Where time stamp gets recorded.
    // Value of TIMESTAMP_TYPE_XXXX
    Int32 mTimeStampType;
};

}
}
}

#endif // __ELASTOS_DROID_TELEPHONY_CCELLINFO_H__
