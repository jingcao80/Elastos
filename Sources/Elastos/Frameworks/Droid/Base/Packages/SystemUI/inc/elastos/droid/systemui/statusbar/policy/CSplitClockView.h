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

#ifndef __ELASTOS_DROID_SYSTEMUI_STATUSBAR_POLICY_CSPLITCLOCKVIEW_H__
#define __ELASTOS_DROID_SYSTEMUI_STATUSBAR_POLICY_CSPLITCLOCKVIEW_H__

#include "_Elastos_Droid_SystemUI_StatusBar_Policy_CSplitClockView.h"
#include "Elastos.Droid.Widget.h"
#include <elastos/droid/content/BroadcastReceiver.h>
#include <elastos/droid/widget/LinearLayout.h>

using Elastos::Droid::Content::BroadcastReceiver;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Widget::ITextClock;
using Elastos::Droid::Widget::LinearLayout;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {
namespace Policy {

/**
 * Container for a clock which has two separate views for the clock itself and AM/PM indicator. This
 * is used to scale the clock independently of AM/PM.
 */
CarClass(CSplitClockView)
    , public LinearLayout
    , public ISplitClockView
{
private:
    class IntentReceiver: public BroadcastReceiver
    {
    public:
        TO_STRING_IMPL("CSplitClockView::IntentReceiver")

        IntentReceiver(
            /* [in] */ CSplitClockView* host);

        // @Override
        CARAPI OnReceive(
            /* [in] */ IContext* context,
            /* [in] */ IIntent* intent);

    private:
        CSplitClockView* mHost;
    };

public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CSplitClockView();

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs);

protected:
    // @Override
    CARAPI OnFinishInflate();

    // @Override
    CARAPI OnAttachedToWindow();

    // @Override
    CARAPI OnDetachedFromWindow();

private:
    CARAPI_(void) UpdatePatterns();

    /**
     * @return the index where the AM/PM part starts at the end in {@code formatString} including
     *         leading white spaces or {@code -1} if no AM/PM part is found or {@code formatString}
     *         doesn't end with AM/PM part
     */
    static CARAPI_(Int32) GetAmPmPartEndIndex(
        /* [in] */ const String& formatString);

private:
    AutoPtr<ITextClock> mTimeView;
    AutoPtr<ITextClock> mAmPmView;
    AutoPtr<IBroadcastReceiver> mIntentReceiver;
};

}// namespace Policy
}// namespace StatusBar
}// namespace SystemUI
}// namespace Droid
}// namespace Elastos

#endif //__ELASTOS_DROID_SYSTEMUI_STATUSBAR_POLICY_CSPLITCLOCKVIEW_H__
