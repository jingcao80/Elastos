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

#ifndef __ELASTOS_DROID_INCALLUI_DIALPADPRESENTER_H__
#define __ELASTOS_DROID_INCALLUI_DIALPADPRESENTER_H__

#include "elastos/droid/incallui/Presenter.h"
#include "elastos/droid/incallui/Call.h"

namespace Elastos {
namespace Droid {
namespace InCallUI {

class DialpadPresenter
    : public Presenter
    , public IInCallStateListener
{
public:
    CAR_INTERFACE_DECL();

    // @Override
    CARAPI OnUiReady(
        /* [in] */ IUi* ui);

    // @Override
    CARAPI OnUiUnready(
        /* [in] */ IUi* ui);

    CARAPI OnStateChange(
        /* [in] */ InCallState oldState,
        /* [in] */ InCallState newState,
        /* [in] */ ICallList* callList);

    /**
     * Processes the specified digit as a DTMF key, by playing the
     * appropriate DTMF tone, and appending the digit to the EditText
     * field that displays the DTMF digits sent so far.
     *
     */
    CARAPI_(void) ProcessDtmf(
      /* [in] */ Char32 c);

    /**
     * Stops the local tone based on the phone type.
     */
    CARAPI_(void) StopDtmf();

private:
    AutoPtr<Call> mCall;
};

} // namespace InCallUI
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INCALLUI_DIALPADPRESENTER_H__
