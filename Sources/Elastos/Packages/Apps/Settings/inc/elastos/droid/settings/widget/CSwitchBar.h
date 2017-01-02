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

#ifndef __ELASTOS_DROID_SETTINGS_WIDGET_CSWITCHBAR_H__
#define __ELASTOS_DROID_SETTINGS_WIDGET_CSWITCHBAR_H__

#include "_Elastos_Droid_Settings_Widget_CSwitchBar.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/widget/LinearLayout.h"
#include "elastos/droid/view/View.h"
#include "_Elastos.Droid.Settings.h"

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Utility::IAttributeSet;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::IViewOnClickListener;
using Elastos::Droid::Widget::ICompoundButton;
using Elastos::Droid::Widget::ICompoundButtonOnCheckedChangeListener;
using Elastos::Droid::Widget::LinearLayout;
using Elastos::Droid::Widget::ISwitch;
using Elastos::Droid::Widget::ITextView;
using Elastos::Utility::IArrayList;

namespace Elastos {
namespace Droid {
namespace Settings {
namespace Widget {

CarClass(CSwitchBar)
    , public LinearLayout
    , public ISwitchBar
{
public:
    class InnerListener
        : public Object
        , public IViewOnClickListener
        , public ICompoundButtonOnCheckedChangeListener
    {
    public:
        CAR_INTERFACE_DECL()

        TO_STRING_IMPL("CSwitchBar::InnerListener")

        InnerListener(
            /* [in] */ CSwitchBar* host);

        //@Override
        CARAPI OnClick(
            /* [in] */ IView* v);

        //@Override
        CARAPI OnCheckedChanged(
            /* [in] */ ICompoundButton* buttonView,
            /* [in] */ Boolean isChecked);

    private:
        CSwitchBar* mHost;
    };

    class SavedState
        : public View::BaseSavedState
        , public ISwitchBarSavedState
    {
    public:
        CAR_INTERFACE_DECL();

        SavedState();

        CARAPI constructor();

        CARAPI constructor(
            /* [in] */ IParcelable* superState);

        //@Override
        CARAPI ReadFromParcel(
            /* [in] */ IParcel* source);

        //@Override
        CARAPI WriteToParcel(
            /* [in] */ IParcel* out);

        //@Override
        CARAPI ToString(
            /* [out] */ String* str);

    public:
        Boolean mChecked;
        Boolean mVisible;
    };

private:
    class SwitchBarOnSwitchChangeListener
        : public Object
        , public ISwitchBarOnSwitchChangeListener
    {
    public:
        CAR_INTERFACE_DECL();

        SwitchBarOnSwitchChangeListener(
            /* [in] */ CSwitchBar* host);

        ~SwitchBarOnSwitchChangeListener();

        //@Override
        CARAPI OnSwitchChanged(
            /* [in] */ ISwitch* switchView,
            /* [in] */ Boolean isChecked);

    private:
        CSwitchBar* mHost;
    };

public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    TO_STRING_IMPL("CSwitchBar")

    CSwitchBar();

    ~CSwitchBar();

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

    CARAPI SetTextViewLabel(
        /* [in] */ Boolean isChecked);

    CARAPI SetChecked(
        /* [in] */ Boolean checked);

    CARAPI SetCheckedInternal(
        /* [in] */ Boolean checked);

    CARAPI IsChecked(
        /* [out] */ Boolean* result);

    CARAPI SetEnabled(
        /* [in] */ Boolean enabled);

    CARAPI_(AutoPtr<IToggleSwitch>) GetSwitch();

    CARAPI Show();

    CARAPI Hide();

    CARAPI_(Boolean) IsShowing();

    //@Override
    CARAPI OnClick(
        /* [in] */ IView* v);

    CARAPI PropagateChecked(
        /* [in] */ Boolean isChecked);

    //@Override
    CARAPI OnCheckedChanged(
        /* [in] */ ICompoundButton* buttonView,
        /* [in] */ Boolean isChecked);

    CARAPI AddOnSwitchChangeListener(
        /* [in] */ ISwitchBarOnSwitchChangeListener* listener);

    CARAPI RemoveOnSwitchChangeListener(
        /* [in] */ ISwitchBarOnSwitchChangeListener* listener);

    //@Override
    CARAPI_(AutoPtr<IParcelable>) OnSaveInstanceState();

    //@Override
    CARAPI OnRestoreInstanceState(
        /* [in] */ IParcelable* state);

private:
    AutoPtr<IToggleSwitch> mSwitch;
    AutoPtr<ITextView> mTextView;

    // ArrayList<OnSwitchChangeListener> mSwitchChangeListeners =
    //     new ArrayList<OnSwitchChangeListener>();
    AutoPtr<IArrayList> mSwitchChangeListeners;

    static AutoPtr< ArrayOf<Int32> > MARGIN_ATTRIBUTES;

    AutoPtr<InnerListener> mListener;
};

} // namespace Widget
} // namespace Settings
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SETTINGS_WIDGET_CSWITCHBAR_H__
