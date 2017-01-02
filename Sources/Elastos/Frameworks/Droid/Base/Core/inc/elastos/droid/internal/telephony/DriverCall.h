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

#ifndef __ELASTOS_DROID_INTERNAL_TELEPHONY_DRIVERCALL_H__
#define __ELASTOS_DROID_INTERNAL_TELEPHONY_DRIVERCALL_H__

#include "_Elastos.Droid.Internal.h"
#include <elastos/core/Object.h>

using Elastos::Core::IComparable;
using Elastos::Core::Object;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {

/**
 * {@hide}
 */
class DriverCall
    : public Object
    , public IComparable
    , public IDriverCall
{
public:
    //enum State {
    //    ACTIVE,
    //    HOLDING,
    //    DIALING,    // MO call only
    //    ALERTING,   // MO call only
    //    INCOMING,   // MT call only
    //    WAITING    // MT call only
    //    // If you add a state, make sure to look for the switch()
    //    // statements that use this enum
    //};

public:
    CAR_INTERFACE_DECL()

    /** returns null on error */
    static CARAPI_(AutoPtr<DriverCall>) FromCLCCLine(
        /* [in] */ const String& line);

    DriverCall();

    // @Override
    CARAPI ToString(
        /* [out] */ String* str);

    static CARAPI StateFromCLCC(
        /* [in] */ Int32 state,
        /* [out] */ IDriverCallState* result) /*throws ATParseEx*/;

    static CARAPI PresentationFromCLIP(
        /* [in] */ Int32 cli,
        /* [out] */ Int32* result) /*throws ATParseEx*/;

    //***** Comparable Implementation

    /** For sorting by index */
    // @Override
    CARAPI CompareTo(
        /* [in] */ IInterface* _dc,
        /* [out] */ Int32* result);

public:
    static const String TAG;
    Int32 mIndex;
    Boolean mIsMT;
    IDriverCallState mState;     // May be null if unavail
    Boolean mIsMpty;
    String mNumber;
    Int32 mTOA;
    Boolean mIsVoice;
    Boolean mIsVoicePrivacy;
    Int32 mAls;
    Int32 mNumberPresentation;
    String mName;
    Int32 mNamePresentation;
    AutoPtr<IUUSInfo> mUusInfo;
};

} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_TELEPHONY_DRIVERCALL_H__
