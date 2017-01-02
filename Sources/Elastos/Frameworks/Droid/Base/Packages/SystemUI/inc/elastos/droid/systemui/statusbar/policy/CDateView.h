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

#ifndef __ELASTOS_DROID_SYSTEMUI_STATUSBAR_POLICY_CDATEVIEW_H__
#define __ELASTOS_DROID_SYSTEMUI_STATUSBAR_POLICY_CDATEVIEW_H__

#include "_Elastos_Droid_SystemUI_StatusBar_Policy_CDateView.h"
#include "Elastos.CoreLibrary.Text.h"
#include <elastos/droid/content/BroadcastReceiver.h>
#include <elastos/droid/widget/TextView.h>

using Elastos::Droid::Content::BroadcastReceiver;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Widget::TextView;
using Elastos::Text::ISimpleDateFormat;
using Elastos::Utility::IDate;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {
namespace Policy {

CarClass(CDateView)
    , public TextView
    , public IDateView
{
private:
    class DateViewBroadcastReceiver
        : public BroadcastReceiver
    {
    public:
        TO_STRING_IMPL("CDateView::DateViewBroadcastReceiver")

        DateViewBroadcastReceiver(
            /* [in] */ CDateView* bar);

        CARAPI OnReceive(
            /* [in] */ IContext* context,
            /* [in] */ IIntent* intent);
    private:
        CDateView* mHost;
    };

public:

    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CDateView();

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs);

protected:
    // @Override
    CARAPI OnAttachedToWindow();

    // @Override
    CARAPI OnDetachedFromWindow();

    CARAPI_(void) UpdateClock();

private:
    static const String TAG;

    AutoPtr<IDate> mCurrentTime;

    AutoPtr<ISimpleDateFormat> mDateFormat;
    String mLastText;
    String mDatePattern;
    AutoPtr<IBroadcastReceiver> mIntentReceiver;
};

}// namespace Policy
}// namespace StatusBar
}// namespace SystemUI
}// namespace Droid
}// namespace Elastos

#endif //__ELASTOS_DROID_SYSTEMUI_STATUSBAR_POLICY_CDATEVIEW_H__
