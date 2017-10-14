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

#ifndef __ELASTOS_DROID_TELEPHONY_CELLINFO_H__
#define __ELASTOS_DROID_TELEPHONY_CELLINFO_H__

#include "Elastos.Droid.Telephony.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

namespace Elastos {
namespace Droid {
namespace Telephony {

class CellInfo
    : public Object
    , public ICellInfo
    , public IParcelable
{
public:

    CAR_INTERFACE_DECL()

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ ICellInfo* ci);

    CARAPI IsRegistered(
        /* [out] */ Boolean* registered);

    CARAPI SetRegistered(
        /* [in] */ Boolean registered);

    CARAPI GetTimeStamp(
        /* [out] */ Int64* timeStamp);

    CARAPI SetTimeStamp(
        /* [in] */ Int64 timeStamp);

    CARAPI GetTimeStampType(
        /* [out] */ Int32* timeStampType);

    CARAPI SetTimeStampType(
        /* [in] */ Int32 timeStampType);

    CARAPI GetHashCode(
        /* [out] */ Int32* hashCode);

    CARAPI Equals(
        /* [in] */ IInterface* other,
        /* [out] */ Boolean* result);

    CARAPI ToString(
        /* [out] */ String* str);

    CARAPI ReadFromParcel(
        /* [in] */ IParcel* source);

    CARAPI WriteToParcel(
        /* [in] */ IParcel* dest);

protected:
    CellInfo();

private:
    static CARAPI_(String) TimeStampTypeToString(
        /* [in] */ Int32 type);

protected:
    // Type fields for parceling
    /** @hide */
    static const Int32 TYPE_GSM;
    /** @hide */
    static const Int32 TYPE_CDMA;
    /** @hide */
    static const Int32 TYPE_LTE;
    /** @hide */
    static const Int32 TYPE_WCDMA;

private:
    // True if device is mRegistered to the mobile network
    Boolean mRegistered;

    // Observation time stamped as type in nanoseconds since boot
    Int64 mTimeStamp;

    // Where time stamp gets recorded.
    // Value of TIMESTAMP_TYPE_XXXX
    Int32 mTimeStampType;
};

} // namespace Telephony
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_TELEPHONY_CELLINFO_H__
