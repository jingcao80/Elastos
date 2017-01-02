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

#ifndef  __ELASTOS_DROID_SYSTEMUI_STATUSBAR_PHONE_DEMOSTATUSICONS_H__
#define  __ELASTOS_DROID_SYSTEMUI_STATUSBAR_PHONE_DEMOSTATUSICONS_H__

#include "_Elastos.Droid.SystemUI.h"
#include "Elastos.Droid.Widget.h"
#include <elastos/droid/widget/LinearLayout.h>

using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Widget::ILinearLayout;
using Elastos::Droid::Widget::LinearLayout;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {
namespace Phone {

class DemoStatusIcons
    : public LinearLayout
    , public IDemoStatusIcons
    , public IDemoMode
{
public:
    CAR_INTERFACE_DECL()

    DemoStatusIcons(
        /* [in] */ ILinearLayout* statusIcons,
        /* [in] */ Int32 iconSize);

    // @Override
    CARAPI DispatchDemoCommand(
        /* [in] */ const String& command,
        /* [in] */ IBundle* args);

private:
    CARAPI_(void) UpdateSlot(
        /* [in] */ const String& slot,
        /* [in] */ const String& iconPkg,
        /* [in] */ Int32 iconId);

private:
    AutoPtr<ILinearLayout> mStatusIcons;
    Int32 mIconSize;

    Boolean mDemoMode;
};

} // namespace Phone
} // namespace StatusBar
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SYSTEMUI_STATUSBAR_PHONE_DEMOSTATUSICONS_H__
