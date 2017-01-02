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

#ifndef __ELASTOS_DROID_INCALLUI_TELECOMADAPTER_H__
#define __ELASTOS_DROID_INCALLUI_TELECOMADAPTER_H__

#include "_Elastos.Droid.Dialer.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Telecom::IPhone;
using Elastos::Droid::Telecom::IPhoneAccountHandle;
using Elastos::Core::Object;

namespace Elastos {
namespace Droid {
namespace InCallUI {

/** Wrapper around {@link InCallAdapter} that only forwards calls to the adapter when it's valid. */
class TelecomAdapter
    : public Object
    , public IInCallPhoneListener
{
public:
    CAR_INTERFACE_DECL()

    static CARAPI_(AutoPtr<TelecomAdapter>) GetInstance();

    CARAPI_(void) SetContext(
        /* [in] */ IContext* context);

    // @Override
    CARAPI SetPhone(
        /* [in] */ IPhone* phone);

    // @Override
    CARAPI ClearPhone();

    CARAPI_(void) AnswerCall(
        /* [in] */ const String& callId,
        /* [in] */ Int32 videoState);

    CARAPI_(void) RejectCall(
        /* [in] */ const String& callId,
        /* [in] */ Boolean rejectWithMessage,
        /* [in] */ const String& message);

    CARAPI_(void) DisconnectCall(
        /* [in] */ const String& callId);

    CARAPI_(void) HoldCall(
        /* [in] */ const String& callId);

    CARAPI_(void) UnholdCall(
        /* [in] */ const String& callId);

    CARAPI_(void) Mute(
        /* [in] */ Boolean shouldMute);

    CARAPI_(void) SetAudioRoute(
        /* [in] */ Int32 route);

    CARAPI_(void) TurnOnProximitySensor();

    CARAPI_(void) TurnOffProximitySensor(
        /* [in] */ Boolean screenOnImmediately);

    CARAPI_(void) SeparateCall(
        /* [in] */ const String& callId);

    CARAPI_(void) Merge(
        /* [in] */ const String& callId);

    CARAPI_(void) Swap(
        /* [in] */ const String& callId);

    CARAPI_(void) AddCall();

    CARAPI_(void) PlayDtmfTone(
        /* [in] */ const String& callId,
        /* [in] */ Char32 digit);

    CARAPI_(void) StopDtmfTone(
        /* [in] */ const String& callId);

    CARAPI_(void) PostDialContinue(
        /* [in] */ const String& callId,
        /* [in] */ Boolean proceed);

    CARAPI_(void) PhoneAccountSelected(
        /* [in] */ const String& callId,
        /* [in] */ IPhoneAccountHandle* accountHandle);

private:
    TelecomAdapter();

    CARAPI_(AutoPtr<Elastos::Droid::Telecom::ICall>) GetTelecommCallById(
        /* [in] */ const String& callId);

private:
    static const String ADD_CALL_MODE_KEY;

    static AutoPtr<TelecomAdapter> sInstance;
    AutoPtr<IContext> mContext;
    AutoPtr<IPhone> mPhone;
};

} // namespace InCallUI
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INCALLUI_TELECOMADAPTER_H__
