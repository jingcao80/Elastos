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

#ifndef __ELASTOS_DROID_SETTINGS_WIDGET_CTOGGLESWITCH_H__
#define __ELASTOS_DROID_SETTINGS_WIDGET_CTOGGLESWITCH_H__

#include "_Elastos_Droid_Settings_Widget_CToggleSwitch.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/widget/Switch.h"
#include "_Elastos.Droid.Settings.h"

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Utility::IAttributeSet;
using Elastos::Droid::Widget::Switch;

namespace Elastos {
namespace Droid {
namespace Settings {
namespace Widget {

CarClass(CToggleSwitch)
    , public Switch
    , public IToggleSwitch
{
public:
    TO_STRING_IMPL("CToggleSwitch")

    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CToggleSwitch();

    ~CToggleSwitch();

    CARAPI constructor(
        /* [in] */ IContext* context);

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs);

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs,
        /* [in] */ Int32 defStyleAttr);

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs,
        /* [in] */ Int32 defStyleAttr,
        /* [in] */ Int32 defStyleRes);

    CARAPI SetOnBeforeCheckedChangeListener(
        /* [in] */ IToggleSwitchOnBeforeCheckedChangeListener* listener);

    //@Override
    CARAPI SetChecked(
        /* [in] */ Boolean checked);

    virtual CARAPI SetCheckedInternal(
        /* [in] */ Boolean checked);

private:
    AutoPtr<IToggleSwitchOnBeforeCheckedChangeListener> mOnBeforeListener;
};

} // namespace Widget
} // namespace Settings
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SETTINGS_WIDGET_CTOGGLESWITCH_H__
