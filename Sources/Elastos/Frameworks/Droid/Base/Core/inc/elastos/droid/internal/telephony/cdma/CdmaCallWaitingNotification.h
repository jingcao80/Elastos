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

#ifndef __ELASTOS_DROID_INTERNAL_TELEPHONY_CDMA_CDMACALLWAITINGNOTIFICATION_H__
#define __ELASTOS_DROID_INTERNAL_TELEPHONY_CDMA_CDMACALLWAITINGNOTIFICATION_H__

#include "elastos/droid/ext/frameworkext.h"
#include "elastos/core/Object.h"

// import android.telephony.Rlog;
// import com.android.internal.telephony.PhoneConstants;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace Cdma {

/**
  * Represents a Supplementary Service Notification received from the network.
  *
  * {@hide}
  */
class CdmaCallWaitingNotification
    : public Object
    , public ICdmaCallWaitingNotification
{
public:
    CAR_INTERFACE_DECL();

    CdmaCallWaitingNotification();

    // @Override
    CARAPI ToString(
        /* [out] */ String* result);

    static CARAPI_(Int32) PresentationFromCLIP(
        /* [in] */ Int32 cli);

public:
    static const String LOGTAG;
    String number;
    Int32 numberPresentation;
    String name;
    Int32 namePresentation;
    Int32 numberType;
    Int32 numberPlan;
    Int32 isPresent;
    Int32 signalType;
    Int32 alertPitch;
    Int32 signal;
};

} // namespace Cdma
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_TELEPHONY_CDMA_CDMACALLWAITINGNOTIFICATION_H__
