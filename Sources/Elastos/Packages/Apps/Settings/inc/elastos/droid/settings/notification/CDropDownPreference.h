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

#ifndef __ELASTOS_DROID_SETTINGS_NOTIFICATION_CDROPDOWNPREFERENCE_H__
#define __ELASTOS_DROID_SETTINGS_NOTIFICATION_CDROPDOWNPREFERENCE_H__

#include "Elastos.Droid.Widget.h"
#include "Elastos.CoreLibrary.Utility.h"
#include "_Elastos_Droid_Settings_Notification_CDropDownPreference.h"
#include "elastos/droid/preference/Preference.h"
#include "_Elastos.Droid.Settings.h"

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Preference::IPreferenceOnPreferenceClickListener;
using Elastos::Droid::Utility::IAttributeSet;
using Elastos::Droid::View::IView;
using Elastos::Droid::Widget::IAdapterView;
using Elastos::Droid::Widget::IArrayAdapter;
using Elastos::Droid::Widget::ISpinner;
using Elastos::Droid::Widget::IAdapterViewOnItemSelectedListener;
using Elastos::Utility::IArrayList;

namespace Elastos {
namespace Droid {
namespace Settings {
namespace Notification {

CarClass(CDropDownPreference)
    , public Elastos::Droid::Preference::Preference
    , public IDropDownPreference
{
private:
    class MyOnItemSelectedListener
        : public Object
        , public IAdapterViewOnItemSelectedListener
    {
    public:
        TO_STRING_IMPL("CDropDownPreference::MyOnItemSelectedListener")

        CAR_INTERFACE_DECL()

        MyOnItemSelectedListener(
            /* [in] */ CDropDownPreference* host);

        //@Override
        CARAPI OnItemSelected(
            /* [in] */ IAdapterView* parent,
            /* [in] */ IView* v,
            /* [in] */ Int32 position,
            /* [in] */ Int64 id);

        //@Override
        CARAPI OnNothingSelected(
            /* [in] */ IAdapterView* parent);

    private:
        CDropDownPreference* mHost;
    };

    class MyOnPreferenceClickListener
        : public Object
        , public IPreferenceOnPreferenceClickListener
    {
    public:
        TO_STRING_IMPL("CDropDownPreference::MyOnPreferenceClickListener")

        CAR_INTERFACE_DECL()

        MyOnPreferenceClickListener(
            /* [in] */ CDropDownPreference* host);

        //@Override
        CARAPI OnPreferenceClick(
            /* [in] */ IPreference* preference,
            /* [out] */ Boolean* result);

    private:
        CDropDownPreference* mHost;
    };

public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    TO_STRING_IMPL("CDropDownPreference")

    CDropDownPreference();

    CARAPI constructor(
        /* [in] */ IContext* context);

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs);

    CARAPI SetDropDownWidth(
        /* [in] */ Int32 dimenResId);

    virtual CARAPI SetCallback(
        /* [in] */ IDropDownPreferenceCallback* callback);

    virtual CARAPI SetSelectedItem(
        /* [in] */ Int32 position);

    virtual CARAPI SetSelectedValue(
        /* [in] */ IInterface* value);

    virtual CARAPI AddItem(
        /* [in] */ Int32 captionResid,
        /* [in] */ IInterface* value);

    virtual CARAPI AddItem(
        /* [in] */ const String& caption,
        /* [in] */ IInterface* value);

    virtual CARAPI ClearItems();

protected:
    //@Override
    CARAPI OnBindView(
        /* [in] */ IView* view);

private:
    AutoPtr<IContext> mContext;
    AutoPtr<IArrayAdapter> /*ArrayAdapter<String>*/mAdapter;
    AutoPtr<ISpinner> mSpinner;
    // ArrayList<Object> mValues = new ArrayList<Object>();
    AutoPtr<IArrayList> mValues;

    AutoPtr<IDropDownPreferenceCallback> mCallback;
};

} // namespace Applications
} // namespace Settings
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SETTINGS_NOTIFICATION_CDROPDOWNPREFERENCE_H__