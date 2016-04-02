
#ifndef __ELASTOS_DROID_PREFERENCE_TWOSTATEPREFERENCE_H__
#define __ELASTOS_DROID_PREFERENCE_TWOSTATEPREFERENCE_H__

#include "elastos/droid/preference/Preference.h"
#include "elastos/droid/text/TextUtils.h"

using Elastos::Droid::Text::TextUtils;
using Elastos::Droid::View::Accessibility::IAccessibilityEvent;
using Elastos::Droid::View::Accessibility::IAccessibilityManager;
using Elastos::Core::ICharSequence;

namespace Elastos {
namespace Droid {
namespace Preference {

class ECO_PUBLIC TwoStatePreference
    : public Preference
    , public ITwoStatePreference
{
public:
    CAR_INTERFACE_DECL()

    TwoStatePreference();

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs,
        /* [in] */ Int32 defStyleAttr,
        /* [in] */ Int32 defStyleRes);

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs,
        /* [in] */ Int32 defStyleAttr);

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs);

    CARAPI constructor(
        /* [in] */ IContext* context);

    virtual CARAPI OnClick();

    virtual CARAPI SetChecked(
        /* [in] */ Boolean checked);

    virtual CARAPI IsChecked(
        /* [out] */ Boolean* isChecked);

    virtual CARAPI ShouldDisableDependents(
            /* [out] */ Boolean* shouldDisableDependents);

    virtual CARAPI SetSummaryOn(
        /* [in] */ ICharSequence* summary);

    virtual CARAPI SetSummaryOn(
        /* [in] */ Int32 summaryResId);

    virtual CARAPI GetSummaryOn(
        /* [out] */ ICharSequence** summary);

    virtual CARAPI SetSummaryOff(
        /* [in] */ ICharSequence* summary);

    virtual CARAPI SetSummaryOff(
        /* [in] */ Int32 summaryResId);

    virtual CARAPI GetSummaryOff(
        /* [out] */ ICharSequence** summary);

    virtual CARAPI GetDisableDependentsState(
        /* [in] */ Boolean* state);

    virtual CARAPI SetDisableDependentsState(
        /* [in] */ Boolean disableDependentsState);

    /**
     * Sync a summary view contained within view's subhierarchy with the correct summary text.
     * @param view View where a summary should be located
     */
    virtual CARAPI SyncSummaryView(
        /* [in] */ IView* view);

protected:
    virtual CARAPI OnGetDefaultValue(
        /* [in] */ ITypedArray* a,
        /* [in] */ Int32 index,
        /* [out] */ IInterface** value);

    virtual CARAPI OnSetInitialValue(
        /* [in] */ Boolean restorePersistedValue,
        /* [in] */ IInterface* defaultValue);

    virtual CARAPI OnSaveInstanceState(
            /* [out] */ IParcelable** state);

    virtual CARAPI OnRestoreInstanceState(
            /* [in] */ IParcelable* state);

protected:
    Boolean mChecked;

private:
    AutoPtr<ICharSequence> mSummaryOn;
    AutoPtr<ICharSequence> mSummaryOff;
    Boolean mCheckedSet;
    Boolean mDisableDependentsState;
};

}
}
}

#endif // __ELASTOS_DROID_PREFERENCE_TWOSTATEPREFERENCE_H__
