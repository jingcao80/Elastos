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

#ifndef __ELASTOS_DROID_INTERNAL_TELEPHONY_UICC_ICCCARDSTATUS_H__
#define __ELASTOS_DROID_INTERNAL_TELEPHONY_UICC_ICCCARDSTATUS_H__

#include "_Elastos.Droid.Internal.h"
#include "elastos/core/Object.h"

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace Uicc {

/**
  * See also RIL_CardStatus in include/telephony/ril.h
  *
  * {@hide}
  */
class IccCardStatus
    : public Object
    , public IIccCardStatus
{
public:
    CAR_INTERFACE_DECL();

    CARAPI SetCardState(
        /* [in] */ Int32 state);

    CARAPI SetUniversalPinState(
        /* [in] */ Int32 state);

    // @Override
    CARAPI ToString(
        /* [out] */ String* result);

public:
    static const Int32 CARD_MAX_APPS = 8;
    CardState mCardState;
    PinState mUniversalPinState;
    Int32 mGsmUmtsSubscriptionAppIndex;
    Int32 mCdmaSubscriptionAppIndex;
    Int32 mImsSubscriptionAppIndex;
    AutoPtr<ArrayOf<IIccCardApplicationStatus*> > mApplications;
};

} // namespace Uicc
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_TELEPHONY_UICC_ICCCARDSTATUS_H__

