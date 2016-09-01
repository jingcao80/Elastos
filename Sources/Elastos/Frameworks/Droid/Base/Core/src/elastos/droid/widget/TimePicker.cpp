
#include "elastos/droid/R.h"
#include "elastos/droid/widget/TimePicker.h"
#include "elastos/droid/widget/CTimePickerClockDelegate.h"
#include "elastos/droid/widget/CTimePickerSpinnerDelegate.h"

using Elastos::Droid::Content::Res::ITypedArray;
using Elastos::Droid::Widget::EIID_ITimePickerDelegate;
using Elastos::Droid::Widget::EIID_ITimePickerAbstractTimePickerDelegate;
using Elastos::Utility::CLocaleHelper;
using Elastos::Utility::ILocale;
using Elastos::Utility::ILocaleHelper;

namespace Elastos {
namespace Droid {
namespace Widget {

//=====================================================================
//                TimePicker::AbstractTimePickerDelegate
//=====================================================================
CAR_INTERFACE_IMPL_2(TimePicker::AbstractTimePickerDelegate, Object, ITimePickerDelegate, ITimePickerAbstractTimePickerDelegate)

TimePicker::AbstractTimePickerDelegate::AbstractTimePickerDelegate()
    : mContext(NULL)
{
}

ECode TimePicker::AbstractTimePickerDelegate::constructor(
    /* [in] */ ITimePicker* delegator,
    /* [in] */ IContext* context)
{
    mDelegator = delegator;
    mContext = context;

    AutoPtr<ILocaleHelper> helper;
    CLocaleHelper::AcquireSingleton((ILocaleHelper**)&helper);
    AutoPtr<ILocale> locale;
    helper->GetDefault((ILocale**)&locale);
    return SetCurrentLocale(locale);
}

ECode TimePicker::AbstractTimePickerDelegate::SetCurrentLocale(
    /* [in] */ ILocale* locale)
{
    VALIDATE_NOT_NULL(locale);

    Boolean resTmp = FALSE;
    locale->Equals(TO_IINTERFACE(mCurrentLocale), &resTmp);
    if (resTmp) {
        return NOERROR;
    }
    mCurrentLocale = locale;
    return NOERROR;
}

ECode TimePicker::AbstractTimePickerDelegate::SetValidationCallback(
    /* [in] */ ITimePickerValidationCallback* callback)
{
    VALIDATE_NOT_NULL(callback);

    mValidationCallback = callback;
    return NOERROR;
}

void TimePicker::AbstractTimePickerDelegate::OnValidationChanged(
    /* [in] */ Boolean valid)
{
    if (mValidationCallback != NULL) {
        mValidationCallback->OnValidationChanged(valid);
    }
}

//=====================================================================
//                              TimePicker
//=====================================================================
const Int32 TimePicker::MODE_SPINNER;
const Int32 TimePicker::MODE_CLOCK;

CAR_INTERFACE_IMPL(TimePicker, FrameLayout, ITimePicker)

TimePicker::TimePicker()
{
}

ECode TimePicker::constructor(
    /* [in] */ IContext* context)
{
    return constructor(context, NULL);
}

ECode TimePicker::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    return constructor(context, attrs, R::attr::timePickerStyle);
}

ECode TimePicker::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ Int32 defStyleAttr)
{
    return constructor(context, attrs, defStyleAttr, 0);
}

ECode TimePicker::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ Int32 defStyleAttr,
    /* [in] */ Int32 defStyleRes)
{
    FrameLayout::constructor(context, attrs, defStyleAttr, defStyleRes);
    AutoPtr< ArrayOf<Int32> > styleAttrs = TO_ATTRS_ARRAYOF(R::styleable::TimePicker);
    AutoPtr<ITypedArray> a;
    context->ObtainStyledAttributes(attrs, styleAttrs, defStyleAttr, defStyleRes,
            (ITypedArray**)&a);
    Int32 mode = 0;
    a->GetInt32(R::styleable::TimePicker_timePickerMode, MODE_SPINNER, &mode);
    a->Recycle();

    switch (mode) {
        case MODE_CLOCK:
                CTimePickerSpinnerDelegate::New(this, context, attrs,
                        defStyleAttr, defStyleRes, (ITimePickerDelegate**)&mDelegate);
            break;
        case MODE_SPINNER:
        default:
                CTimePickerClockDelegate::New(this, context, attrs,
                        defStyleAttr, defStyleRes, (ITimePickerDelegate**)&mDelegate);
            break;
    }
    return NOERROR;
}

ECode TimePicker::SetCurrentHour(
    /* [in] */ Int32 currentHour)
{
    mDelegate->SetCurrentHour(currentHour);
    return NOERROR;
}

ECode TimePicker::GetCurrentHour(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);

    mDelegate->GetCurrentHour(result);
    return NOERROR;
}

ECode TimePicker::SetCurrentMinute(
    /* [in] */ Int32 currentMinute)
{
    mDelegate->SetCurrentMinute(currentMinute);
    return NOERROR;
}

ECode TimePicker::GetCurrentMinute(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);

    mDelegate->GetCurrentMinute(result);
    return NOERROR;
}

ECode TimePicker::SetIs24HourView(
    /* [in] */ Boolean is24HourView)
{
    mDelegate->SetIs24HourView(is24HourView);
    return NOERROR;
}

ECode TimePicker::Is24HourView(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    mDelegate->Is24HourView(result);
    return NOERROR;
}

ECode TimePicker::SetOnTimeChangedListener(
    /* [in] */ ITimePickerOnTimeChangedListener* onTimeChangedListener)
{
    VALIDATE_NOT_NULL(onTimeChangedListener);

    mDelegate->SetOnTimeChangedListener(onTimeChangedListener);
    return NOERROR;
}

ECode TimePicker::SetValidationCallback(
    /* [in] */ ITimePickerValidationCallback* callback)
{
    mDelegate->SetValidationCallback(callback);
    return NOERROR;
}

ECode TimePicker::SetEnabled(
    /* [in] */ Boolean enabled)
{
    FrameLayout::SetEnabled(enabled);
    mDelegate->SetEnabled(enabled);
    return NOERROR;
}

ECode TimePicker::IsEnabled(
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res)

    return mDelegate->IsEnabled(res);
}

ECode TimePicker::GetBaseline(
    /* [out] */ Int32* baseline)
{
    VALIDATE_NOT_NULL(baseline)

    return mDelegate->GetBaseline(baseline);
}

ECode TimePicker::DispatchPopulateAccessibilityEvent(
    /* [in] */ IAccessibilityEvent* event,
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res)

    return mDelegate->DispatchPopulateAccessibilityEvent(event, res);
}

ECode TimePicker::OnPopulateAccessibilityEvent(
    /* [in] */ IAccessibilityEvent* event)
{
    VALIDATE_NOT_NULL(event);

    FrameLayout::OnPopulateAccessibilityEvent(event);
    mDelegate->OnPopulateAccessibilityEvent(event);
    return NOERROR;
}

ECode TimePicker::OnInitializeAccessibilityEvent(
    /* [in] */ IAccessibilityEvent* event)
{
    VALIDATE_NOT_NULL(event);

    FrameLayout::OnInitializeAccessibilityEvent(event);
    mDelegate->OnInitializeAccessibilityEvent(event);
    return NOERROR;
}

ECode TimePicker::OnInitializeAccessibilityNodeInfo(
    /* [in] */ IAccessibilityNodeInfo* info)
{
    VALIDATE_NOT_NULL(info);

    FrameLayout::OnInitializeAccessibilityNodeInfo(info);
    mDelegate->OnInitializeAccessibilityNodeInfo(info);
    return NOERROR;
}

ECode TimePicker::OnConfigurationChanged(
    /* [in] */ IConfiguration* newConfig)
{
    FrameLayout::OnConfigurationChanged(newConfig);
    mDelegate->OnConfigurationChanged(newConfig);
    return NOERROR;
}

AutoPtr<IParcelable> TimePicker::OnSaveInstanceState()
{
    AutoPtr<IParcelable> superState = FrameLayout::OnSaveInstanceState();
    AutoPtr<IParcelable> result;
    mDelegate->OnSaveInstanceState(superState, (IParcelable**)&result);
    return result;
}

ECode TimePicker::OnRestoreInstanceState(
    /* [in] */ IParcelable* state)
{
    View::BaseSavedState* ss = (View::BaseSavedState*)state;
    AutoPtr<IParcelable> superState;
    ss->GetSuperState((IParcelable**)&superState);
    FrameLayout::OnRestoreInstanceState(superState);
    mDelegate->OnRestoreInstanceState(state);
    return NOERROR;
}

} // namespace Widget
} // namespace Droid
} // namespace Elastos
