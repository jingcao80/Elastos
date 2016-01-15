
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.View.h"
#include "elastos/droid/preference/SwitchPreference.h"
#include "elastos/droid/R.h"

using Elastos::Droid::R;
using Elastos::Droid::Widget::EIID_ICompoundButtonOnCheckedChangeListener;
using Elastos::Droid::Widget::ICheckable;
using Elastos::Droid::Widget::ISwitch;
using Elastos::Core::IBoolean;
using Elastos::Core::CBoolean;
using Elastos::Core::CString;

namespace Elastos {
namespace Droid {
namespace Preference {

/////////////////////////////////////////////////////
// SwitchPreference::Listener
/////////////////////////////////////////////////////

CAR_INTERFACE_IMPL(SwitchPreference::Listener, Object, ICompoundButtonOnCheckedChangeListener)

SwitchPreference::Listener::Listener(
    /* [in] */ SwitchPreference* host)
    : mHost(host)
{}

ECode SwitchPreference::Listener::OnCheckedChanged(
    /* [in] */ ICompoundButton* buttonView,
    /* [in] */ Boolean isChecked)
{
    AutoPtr<IBoolean> b;
    CBoolean::New(isChecked, (IBoolean**)&b);
    Boolean result;
    if (mHost->CallChangeListener(b, &result), !result) {
        // Listener didn't like it, change it back.
        // CompoundButton will make sure we don't recurse.
        ICheckable::Probe(buttonView)->SetChecked(!isChecked);
        return NOERROR;
    }

    mHost->SetChecked(isChecked);
    return NOERROR;
}

/////////////////////////////////////////////////////
// SwitchPreference
/////////////////////////////////////////////////////

CAR_INTERFACE_IMPL(SwitchPreference, TwoStatePreference, ISwitchPreference)

SwitchPreference::SwitchPreference()
{
    mListener = new Listener(this);
}

ECode SwitchPreference::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ Int32 defStyleAttr,
    /* [in] */ Int32 defStyleRes)
{
    FAIL_RETURN(TwoStatePreference::constructor(context, attrs, defStyleAttr, defStyleRes));

    AutoPtr<ArrayOf<Int32> > attrIds = ArrayOf<Int32>::Alloc(
            const_cast<Int32 *>(R::styleable::SwitchPreference),
            ArraySize(R::styleable::SwitchPreference));
    AutoPtr<ITypedArray> a;
    context->ObtainStyledAttributes(attrs, attrIds, defStyleAttr, defStyleRes, (ITypedArray**)&a);
    String summaryOn;
    a->GetString(R::styleable::SwitchPreference_summaryOn, &summaryOn);
    AutoPtr<ICharSequence> summaryOnCs;
    CString::New(summaryOn, (ICharSequence**)&summaryOnCs);
    SetSummaryOn(summaryOnCs);

    String summaryOff;
    a->GetString(R::styleable::SwitchPreference_summaryOff, &summaryOff);
    AutoPtr<ICharSequence> summaryOffCs;
    CString::New(summaryOff, (ICharSequence**)&summaryOffCs);
    SetSummaryOff(summaryOffCs);

    String switchTextOn;
    a->GetString(R::styleable::SwitchPreference_switchTextOn, &switchTextOn);
    AutoPtr<ICharSequence> textOn;
    CString::New(switchTextOn, (ICharSequence**)&textOn);
    SetSwitchTextOn(textOn);

    String switchTextOff;
    a->GetString(R::styleable::SwitchPreference_switchTextOff, &switchTextOff);
    AutoPtr<ICharSequence> textOff;
    CString::New(switchTextOff, (ICharSequence**)&textOff);
    SetSwitchTextOff(textOff);

    Boolean disable;
    a->GetBoolean(R::styleable::SwitchPreference_disableDependentsState, FALSE, &disable);
    SetDisableDependentsState(disable);
    a->Recycle();
    return NOERROR;
}

ECode SwitchPreference::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ Int32 defStyleAttr)
{
    return constructor(context, attrs, defStyleAttr, 0);
}

ECode SwitchPreference::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    return constructor(context, attrs, R::attr::switchPreferenceStyle);
}

ECode SwitchPreference::constructor(
    /* [in] */ IContext* context)
{
    return constructor(context, NULL);
}

ECode SwitchPreference::OnBindView(
    /* [in] */ IView* view)
{
    FAIL_RETURN(TwoStatePreference::OnBindView(view))

    AutoPtr<IView> checkableView;
    view->FindViewById(R::id::switchWidget, (IView**)&checkableView);
    if (checkableView != NULL && ICheckable::Probe(checkableView) != NULL) {
        if (ISwitch::Probe(checkableView) != NULL) {
            AutoPtr<ISwitch> switchView = ISwitch::Probe(checkableView);
            AutoPtr<ICompoundButton> cb = ICompoundButton::Probe(switchView);
            cb->SetOnCheckedChangeListener(NULL);
        }

        AutoPtr<ICheckable> checkable = ICheckable::Probe(checkableView);
        checkable->SetChecked(mChecked);

        if (ISwitch::Probe(checkableView) != NULL) {
            AutoPtr<ISwitch> switchView = ISwitch::Probe(checkableView);
            switchView->SetTextOn(mSwitchOn);
            switchView->SetTextOff(mSwitchOff);
            AutoPtr<ICompoundButton> cb = ICompoundButton::Probe(switchView);
            cb->SetOnCheckedChangeListener(mListener);
        }
    }

    SyncSummaryView(view);
    return NOERROR;
}

ECode SwitchPreference::SetSwitchTextOn(
    /* [in] */ ICharSequence* onText)
{
    mSwitchOn = onText;
    return NotifyChanged();
}

ECode SwitchPreference::SetSwitchTextOff(
    /* [in] */ ICharSequence* offText)
{
    mSwitchOff = offText;
    return NotifyChanged();
}

ECode SwitchPreference::SetSwitchTextOn(
    /* [in] */ Int32 resId)
{
    AutoPtr<IContext> context;
    GetContext((IContext**)&context);
    String textOnStr;
    context->GetString(resId, &textOnStr);
    AutoPtr<ICharSequence> textOn;
    CString::New(textOnStr, (ICharSequence**)&textOn);
    return SetSwitchTextOn(textOn);
}

ECode SwitchPreference::SetSwitchTextOff(
    /* [in] */ Int32 resId)
{
    AutoPtr<IContext> context;
    GetContext((IContext**)&context);
    String textOffStr;
    context->GetString(resId, &textOffStr);
    AutoPtr<ICharSequence> textOff;
    CString::New(textOffStr, (ICharSequence**)&textOff);
    return SetSwitchTextOn(textOff);
}

ECode SwitchPreference::GetSwitchTextOn(
    /* [out] */ ICharSequence** textOn)
{
    VALIDATE_NOT_NULL(textOn)
    *textOn = mSwitchOn;
    REFCOUNT_ADD(*textOn)
    return NOERROR;
}

ECode SwitchPreference::GetSwitchTextOff(
    /* [out] */ ICharSequence** textOff)
{
    VALIDATE_NOT_NULL(textOff)
    *textOff = mSwitchOff;
    REFCOUNT_ADD(*textOff)
    return NOERROR;
}

}
}
}
