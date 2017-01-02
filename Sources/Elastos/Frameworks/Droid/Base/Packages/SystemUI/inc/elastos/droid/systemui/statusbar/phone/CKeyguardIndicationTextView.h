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

#ifndef  __ELASTOS_DROID_SYSTEMUI_STATUSBAR_PHONE_CKEYGUARDINDICATIONTEXTVIEW_H__
#define  __ELASTOS_DROID_SYSTEMUI_STATUSBAR_PHONE_CKEYGUARDINDICATIONTEXTVIEW_H__

#include "_Elastos_Droid_SystemUI_StatusBar_Phone_CKeyguardIndicationTextView.h"
#include <elastos/droid/widget/TextView.h>

using Elastos::Droid::Widget::TextView;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {
namespace Phone {

/**
 * A view to show hints on Keyguard ("Swipe up to unlock", "Tap again to open").
 */
CarClass(CKeyguardIndicationTextView)
    , public TextView
    , public IKeyguardIndicationTextView
{
public:
    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    /**
     * Changes the text with an animation and makes sure a single indication is shown long enough.
     *
     * @param text The text to show.
     */
    CARAPI SwitchIndication(
        /* [in] */ ICharSequence* text);

    /**
     * See {@link #switchIndication}.
     */
    CARAPI SwitchIndication(
        /* [in] */ Int32 textResId);
};

} // namespace Phone
} // namespace StatusBar
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SYSTEMUI_STATUSBAR_PHONE_CKEYGUARDINDICATIONTEXTVIEW_H__
