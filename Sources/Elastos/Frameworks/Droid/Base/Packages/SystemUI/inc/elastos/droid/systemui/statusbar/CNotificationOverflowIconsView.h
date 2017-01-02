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

#ifndef  __ELASTOS_DROID_SYSTEMUI_STATUSBAR_CNOTIFICATIONOVERFLOWICONSVIEW_H__
#define  __ELASTOS_DROID_SYSTEMUI_STATUSBAR_CNOTIFICATIONOVERFLOWICONSVIEW_H__

#include "_Elastos_Droid_SystemUI_StatusBar_CNotificationOverflowIconsView.h"
#include "elastos/droid/systemui/statusbar/phone/IconMerger.h"
#include "Elastos.Droid.App.h"
#include "Elastos.Droid.Internal.h"
#include "Elastos.Droid.Widget.h"

using Elastos::Droid::App::INotification;
using Elastos::Droid::Internal::Utility::INotificationColorUtil;
using Elastos::Droid::SystemUI::StatusBar::Phone::IconMerger;
using Elastos::Droid::Widget::ITextView;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {

/**
 * A view to display all the overflowing icons on Keyguard.
 */
CarClass(CNotificationOverflowIconsView)
    , public IconMerger
    , public INotificationOverflowIconsView
{
public:
    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    CNotificationOverflowIconsView();

    CARAPI SetMoreText(
        /* [in] */ ITextView* moreText);

    CARAPI AddNotification(
        /* [in] */ INotificationDataEntry* notification);

protected:
    // @Override
    CARAPI OnFinishInflate();

private:
    CARAPI_(void) ApplyColor(
        /* [in] */ INotification* notification,
        /* [in] */ IStatusBarIconView* view);

    CARAPI_(void) UpdateMoreText();

private:
    AutoPtr<ITextView> mMoreText;
    Int32 mTintColor;
    Int32 mIconSize;
    AutoPtr<INotificationColorUtil> mNotificationColorUtil;
};

} // namespace StatusBar
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SYSTEMUI_STATUSBAR_CNOTIFICATIONOVERFLOWICONSVIEW_H__
