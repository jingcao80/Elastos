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

#ifndef __ELASTOS_DROID_TELEPHONY_CDATACONNECTIONREALTIMEINFO_H__
#define __ELASTOS_DROID_TELEPHONY_CDATACONNECTIONREALTIMEINFO_H__

#include "_Elastos_Droid_Telephony_CDataConnectionRealTimeInfo.h"
#include "elastos/droid/ext/frameworkdef.h"
#include <elastos/core/Object.h>

namespace Elastos {
namespace Droid {
namespace Telephony {

CarClass(CDataConnectionRealTimeInfo)
    , public Object
    , public IDataConnectionRealTimeInfo
    , public IParcelable
{
public:
    CDataConnectionRealTimeInfo();

    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ Int64 time,
        /* [in] */ Int32 dcPowerState);

    CARAPI ReadFromParcel(
        /* [in] */ IParcel* source);

    CARAPI WriteToParcel(
        /* [in] */ IParcel* dest);

    CARAPI GetTime(
        /* [out] */ Int64* result);

    CARAPI GetDcPowerState(
        /* [out] */ Int32* result);

    CARAPI GetHashCode(
        /* [out] */ Int32* hashCode);

    CARAPI Equals(
        /* [in] */ IInterface* other,
        /* [out] */ Boolean* res);

    CARAPI ToString(
        /* [out] */ String* str);

private:
    Int64 mTime;             // Time the info was collected since boot in nanos;

    Int32 mDcPowerState;      // DC_POWER_STATE_[LOW | MEDIUM | HIGH | UNKNOWN]
};

} // namespace Telephony
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_TELEPHONY_CDATACONNECTIONREALTIMEINFO_H__
