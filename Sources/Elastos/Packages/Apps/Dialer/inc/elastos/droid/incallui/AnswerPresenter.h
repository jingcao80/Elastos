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

#ifndef __ELASTOS_DROID_INCALLUI_ANSWERPRESENTER_H__
#define __ELASTOS_DROID_INCALLUI_ANSWERPRESENTER_H__

#include "_Elastos.Droid.Dialer.h"
#include "elastos/droid/incallui/Call.h"
#include "elastos/droid/incallui/Presenter.h"

namespace Elastos {
namespace Droid {
namespace InCallUI {

/**
 * Presenter for the Incoming call widget.
 */
class AnswerPresenter
    : public Presenter
    , public ICallUpdateListener
    , public ICallListListener
{
public:
    CAR_INTERFACE_DECL();

    AnswerPresenter();

    // @Override
    CARAPI OnUiReady(
        /* [in] */ IUi* ui);

    // @Override
    CARAPI OnUiUnready(
        /* [in] */ IUi* ui);

    // @Override
    CARAPI OnCallListChange(
        /* [in] */ ICallList* callList);

    // @Override
    CARAPI OnDisconnect(
        /* [in] */ ICall* call);

    // @Override
    CARAPI OnIncomingCall(
        /* [in] */ ICall* call);

    // @Override
    CARAPI OnCallChanged(
        /* [in] */ ICall* call);

    CARAPI OnAnswer(
        /* [in] */ Int32 videoState,
        /* [in] */ IContext* context);

    /**
     * TODO: We are using reject and decline interchangeably. We should settle on
     * reject since it seems to be more prevalent.
     */
    CARAPI OnDecline();

    CARAPI OnText();

    CARAPI_(void) RejectCallWithMessage(
        /* [in] */ const String& message);

    CARAPI OnDismissDialog();

private:
    CARAPI_(void) ProcessIncomingCall(
        /* [in] */ ICall* call);

    CARAPI_(void) ProcessVideoUpgradeRequestCall(
        /* [in] */ ICall* call);

    CARAPI_(void) ConfigureAnswerTargetsForSms(
        /* [in] */ ICall* call,
        /* [in] */ IList* textMsgs);

private:
    static const String TAG;

    String mCallId;
    AutoPtr<Call> mCall;
    Boolean mHasTextMessages;
};

} // namespace InCallUI
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INCALLUI_ANSWERPRESENTER_H__
