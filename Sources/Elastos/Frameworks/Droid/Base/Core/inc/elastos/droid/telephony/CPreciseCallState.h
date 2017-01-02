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

#ifndef __ELASTOS_DROID_TELEPHONY_CPRECISECALLSTATE_H__
#define __ELASTOS_DROID_TELEPHONY_CPRECISECALLSTATE_H__

#include "_Elastos_Droid_Telephony_CPreciseCallState.h"
#include "elastos/droid/ext/frameworkdef.h"
#include <elastos/core/Object.h>

namespace Elastos {
namespace Droid {
namespace Telephony {

CarClass(CPreciseCallState)
    , public Object
    , public IPreciseCallState
    , public IParcelable
{
public:
    CPreciseCallState();

    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ Int32 ringingCall,
        /* [in] */ Int32 foregroundCall,
        /* [in] */ Int32 backgroundCall,
        /* [in] */ Int32 disconnectCause,
        /* [in] */ Int32 preciseDisconnectCause);

    CARAPI ReadFromParcel(
        /* [in] */ IParcel* source);

    CARAPI WriteToParcel(
        /* [in] */ IParcel* dest);

    CARAPI GetRingingCallState(
        /* [out] */ Int32* ringingCallState);

    CARAPI GetForegroundCallState(
        /* [out] */ Int32* foregroundCallState);

    CARAPI GetBackgroundCallState(
        /* [out] */ Int32* backgroundCallState);

    CARAPI GetDisconnectCause(
        /* [out] */ Int32* disconnectCause);

    CARAPI GetPreciseDisconnectCause(
        /* [out] */ Int32* preciseDisconnectCause);

    CARAPI GetHashCode(
        /* [out] */ Int32* hashCode);

    CARAPI Equals(
        /* [in] */ IInterface* other,
        /* [out] */ Boolean* res);

    CARAPI ToString(
        /* [out] */ String* str);

private:
    Int32 mRingingCallState;
    Int32 mForegroundCallState;
    Int32 mBackgroundCallState;
    Int32 mDisconnectCause;
    Int32 mPreciseDisconnectCause;
};

} // namespace Telephony
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_TELEPHONY_CPRECISECALLSTATE_H__
