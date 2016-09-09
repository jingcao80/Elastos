
#ifndef __ELASTOS_DROID_SETTINGS_WIDGET_SWITCHBAR_H__
#define __ELASTOS_DROID_SETTINGS_WIDGET_SWITCHBAR_H__

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

class SwitchBar
    : public LinearLayout
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

        TO_STRING_IMPL("SwitchBar::InnerListener")

        InnerListener(
            /* [in] */ SwitchBar* host);

        //@Override
        CARAPI OnClick(
            /* [in] */ IView* v);

        //@Override
        CARAPI OnCheckedChanged(
            /* [in] */ ICompoundButton* buttonView,
            /* [in] */ Boolean isChecked);

    private:
        SwitchBar* mHost;
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
            /* [in] */ SwitchBar* host);

        ~SwitchBarOnSwitchChangeListener();

        //@Override
        CARAPI OnSwitchChanged(
            /* [in] */ ISwitch* switchView,
            /* [in] */ Boolean isChecked);

    private:
        SwitchBar* mHost;
    };

public:
    CAR_INTERFACE_DECL();

    TO_STRING_IMPL("SwitchBar");

    SwitchBar();

    ~SwitchBar();

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

#endif //__ELASTOS_DROID_SETTINGS_WIDGET_SWITCHBAR_H__
