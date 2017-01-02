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

#ifndef __ELASTOS_DROID_SYSTEMUI_STATUSBAR_PHONE_CPANELHOLDER_H__
#define __ELASTOS_DROID_SYSTEMUI_STATUSBAR_PHONE_CPANELHOLDER_H__

#include "_Elastos_Droid_SystemUI_StatusBar_Phone_CPanelHolder.h"
#include <elastos/droid/widget/FrameLayout.h>

using Elastos::Droid::View::IMotionEvent;
using Elastos::Droid::Widget::FrameLayout;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {
namespace Phone {

CarClass(CPanelHolder)
    , public FrameLayout
    , public IPanelHolder
{
public:
    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    CPanelHolder();

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs);

    CARAPI GetPanelIndex(
        /* [in] */ IPanelView* pv,
        /* [out] */ Int32* result);

    CARAPI SetSelectedPanel(
        /* [in] */ IPanelView* pv);

    // @Override
    CARAPI OnTouchEvent(
        /* [in] */ IMotionEvent* event,
        /* [out] */ Boolean* result);

    CARAPI SetBar(
        /* [in] */ IPanelBar* panelBar);

protected:
    // @Override
    CARAPI_(Int32) GetChildDrawingOrder(
        /* [in] */ Int32 childCount,
        /* [in] */ Int32 i);

    // @Override
    CARAPI OnFinishInflate();

public:
    static const Boolean DEBUG_GESTURES;

private:
    Int32 mSelectedPanelIndex;
    AutoPtr<IPanelBar> mBar;
};

}// namespace Phone
}// namespace StatusBar
}// namespace SystemUI
}// namespace Droid
}// namespace Elastos

#endif //__ELASTOS_DROID_SYSTEMUI_STATUSBAR_PHONE_CPANELHOLDER_H__
