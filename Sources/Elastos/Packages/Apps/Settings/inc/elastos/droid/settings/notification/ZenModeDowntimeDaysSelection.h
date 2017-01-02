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

#ifndef __ELASTOS_DROID_SETTINGS_NOTIFICATION_ZENMODEDOWNTIMEDAYSSELECTION_H__
#define __ELASTOS_DROID_SETTINGS_NOTIFICATION_ZENMODEDOWNTIMEDAYSSELECTION_H__

#include "Elastos.CoreLibrary.Text.h"
#include "elastos/droid/widget/ScrollView.h"

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Utility::ISparseBooleanArray;
using Elastos::Droid::Widget::ICompoundButton;
using Elastos::Droid::Widget::ScrollView;
using Elastos::Droid::Widget::ICompoundButtonOnCheckedChangeListener;
using Elastos::Droid::Widget::ILinearLayout;
using Elastos::Text::ISimpleDateFormat;

namespace Elastos {
namespace Droid {
namespace Settings {
namespace Notification {

class ZenModeDowntimeDaysSelection
    : public ScrollView
{
private:
    class MyOnCheckedChangeListener
        : public Object
        , public ICompoundButtonOnCheckedChangeListener
    {
    public:
        TO_STRING_IMPL("ZenModeDowntimeDaysSelection::MyOnCheckedChangeListener")

        CAR_INTERFACE_DECL()

        MyOnCheckedChangeListener(
            /* [in] */ ZenModeDowntimeDaysSelection* host,
            /* [in] */ Int32 day);

        //@Override
        CARAPI OnCheckedChanged(
            /* [in] */ ICompoundButton* buttonView,
            /* [in] */ Boolean isChecked);

    private:
        ZenModeDowntimeDaysSelection* mHost;
        Int32 mDay;
    };

public:
    TO_STRING_IMPL("ZenModeDowntimeDaysSelection")

    ZenModeDowntimeDaysSelection();

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ const String& mode);

protected:
    virtual CARAPI OnChanged(
        /* [in] */ const String& mode);

private:
    CARAPI_(String) GetMode();

public:
    static const AutoPtr< ArrayOf<Int32> > DAYS;

private:
    static const AutoPtr<ISimpleDateFormat> DAY_FORMAT;

    AutoPtr<ISparseBooleanArray> mDays;
    AutoPtr<ILinearLayout> mLayout;
};

} // namespace Applications
} // namespace Settings
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SETTINGS_NOTIFICATION_ZENMODEDOWNTIMEDAYSSELECTION_H__