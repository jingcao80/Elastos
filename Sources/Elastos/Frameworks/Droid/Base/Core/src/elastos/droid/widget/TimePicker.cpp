
#include "elastos/droid/R.h"
#include "elastos/droid/widget/TimePicker.h"
#include "elastos/droid/widget/TimePickerClockDelegate.h"
#include "elastos/droid/widget/TimePickerSpinnerDelegate.h"

using Elastos::Droid::Content::Res::ITypedArray;
using Elastos::Droid::Widget::EIID_ITimePickerAbstractTimePickerDelegate;
using Elastos::Droid::Widget::TimePickerClockDelegate;
using Elastos::Droid::Widget::TimePickerSpinnerDelegate;
using Elastos::Utility::CLocaleHelper;
using Elastos::Utility::ILocale;
using Elastos::Utility::ILocaleHelper;

namespace Elastos {
namespace Droid {
namespace Widget {

//=====================================================================
//                TimePicker::AbstractTimePickerDelegate
//=====================================================================
CAR_INTERFACE_IMPL(TimePicker::AbstractTimePickerDelegate, TimePickerDelegate, ITimePickerAbstractTimePickerDelegate)

TimePicker::AbstractTimePickerDelegate::AbstractTimePickerDelegate()
{
}

ECode TimePicker::AbstractTimePickerDelegate::constructor(
    /* [in] */ ITimePicker* delegator,
    /* [in] */ IContext* context)
{
    // ==================before translated======================
    // mDelegator = delegator;
    // mContext = context;
    //
    // // initialization based on locale
    // setCurrentLocale(Locale.getDefault());

    mDelegator = delegator;
    mContext = context;

    AutoPtr<ILocaleHelper> helper;
    CLocaleHelper::AcquireSingleton((ILocaleHelper**)&helper);
    AutoPtr<ILocale> locale;
    helper->GetDefault((ILocale**)&locale);
    SetCurrentLocale(locale);
    return NOERROR;
}

ECode TimePicker::AbstractTimePickerDelegate::SetCurrentLocale(
    /* [in] */ ILocale* locale)
{
    VALIDATE_NOT_NULL(locale);
    // ==================before translated======================
    // if (locale.equals(mCurrentLocale)) {
    //     return;
    // }
    // mCurrentLocale = locale;

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
    // ==================before translated======================
    // mValidationCallback = callback;

    mValidationCallback = callback;
    return NOERROR;
}

void TimePicker::AbstractTimePickerDelegate::OnValidationChanged(
    /* [in] */ Boolean valid)
{
    // ==================before translated======================
    // if (mValidationCallback != null) {
    //     mValidationCallback.onValidationChanged(valid);
    // }

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
    // ==================before translated======================
    // this(context, null);

    return constructor(context, NULL);
}

ECode TimePicker::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    // ==================before translated======================
    // this(context, attrs, R.attr.timePickerStyle);

    return constructor(context, attrs, R::attr::timePickerStyle);
}

ECode TimePicker::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ Int32 defStyleAttr)
{
    // ==================before translated======================
    // this(context, attrs, defStyleAttr, 0);

    return constructor(context, attrs, defStyleAttr, 0);
}

ECode TimePicker::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ Int32 defStyleAttr,
    /* [in] */ Int32 defStyleRes)
{
    // ==================before translated======================
    // super(context, attrs, defStyleAttr, defStyleRes);
    //
    // final TypedArray a = context.obtainStyledAttributes(
    //         attrs, R.styleable.TimePicker, defStyleAttr, defStyleRes);
    // final int mode = a.getInt(R.styleable.TimePicker_timePickerMode, MODE_SPINNER);
    // a.recycle();
    //
    // switch (mode) {
    //     case MODE_CLOCK:
    //         mDelegate = new TimePickerSpinnerDelegate(
    //                 this, context, attrs, defStyleAttr, defStyleRes);
    //         break;
    //     case MODE_SPINNER:
    //     default:
    //         mDelegate = new TimePickerClockDelegate(
    //                 this, context, attrs, defStyleAttr, defStyleRes);
    //         break;
    // }

    FrameLayout::constructor(context, attrs, defStyleAttr, defStyleRes);
    AutoPtr< ArrayOf<Int32> > styleAttrs = ArrayOf<Int32>::Alloc(const_cast<Int32*>(R::styleable::TimePicker),
        ArraySize(R::styleable::TimePicker));
    AutoPtr<ITypedArray> a;
    context->ObtainStyledAttributes(attrs, styleAttrs, defStyleAttr, defStyleRes, (ITypedArray**)&a);
    Int32 mode = 0;
    a->GetInt32(R::styleable::TimePicker_timePickerMode, MODE_SPINNER, &mode);
    a->Recycle();

    switch (mode) {
        case MODE_CLOCK:
            {
                mDelegate = new TimePickerSpinnerDelegate();
                ((TimePickerSpinnerDelegate*)mDelegate.Get())->constructor(this, context, attrs, defStyleAttr, defStyleRes);
            }
            break;
        case MODE_SPINNER:
        default:
            {
                mDelegate = new TimePickerClockDelegate();
                ((TimePickerClockDelegate*)mDelegate.Get())->constructor(this, context, attrs, defStyleAttr, defStyleRes);
            }
            break;
    }
    return NOERROR;
}

ECode TimePicker::SetCurrentHour(
    /* [in] */ Int32 currentHour)
{
    // ==================before translated======================
    // mDelegate.setCurrentHour(currentHour);

    mDelegate->SetCurrentHour(currentHour);
    return NOERROR;
}

ECode TimePicker::GetCurrentHour(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mDelegate.getCurrentHour();

    mDelegate->GetCurrentHour(result);
    return NOERROR;
}

ECode TimePicker::SetCurrentMinute(
    /* [in] */ Int32 currentMinute)
{
    // ==================before translated======================
    // mDelegate.setCurrentMinute(currentMinute);

    mDelegate->SetCurrentMinute(currentMinute);
    return NOERROR;
}

ECode TimePicker::GetCurrentMinute(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mDelegate.getCurrentMinute();

    mDelegate->GetCurrentMinute(result);
    return NOERROR;
}

ECode TimePicker::SetIs24HourView(
    /* [in] */ Boolean is24HourView)
{
    // ==================before translated======================
    // mDelegate.setIs24HourView(is24HourView);

    mDelegate->SetIs24HourView(is24HourView);
    return NOERROR;
}

ECode TimePicker::Is24HourView(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mDelegate.is24HourView();

    mDelegate->Is24HourView(result);
    return NOERROR;
}

ECode TimePicker::SetOnTimeChangedListener(
    /* [in] */ ITimePickerOnTimeChangedListener* onTimeChangedListener)
{
    VALIDATE_NOT_NULL(onTimeChangedListener);
    // ==================before translated======================
    // mDelegate.setOnTimeChangedListener(onTimeChangedListener);

    mDelegate->SetOnTimeChangedListener(onTimeChangedListener);
    return NOERROR;
}

ECode TimePicker::SetValidationCallback(
    /* [in] */ ITimePickerValidationCallback* callback)
{
    // ==================before translated======================
    // mDelegate.setValidationCallback(callback);

    mDelegate->SetValidationCallback(callback);
    return NOERROR;
}

ECode TimePicker::SetEnabled(
    /* [in] */ Boolean enabled)
{
    // ==================before translated======================
    // super.setEnabled(enabled);
    // mDelegate.setEnabled(enabled);

    FrameLayout::SetEnabled(enabled);
    mDelegate->SetEnabled(enabled);
    return NOERROR;
}

Boolean TimePicker::IsEnabled()
{
    // ==================before translated======================
    // return mDelegate.isEnabled();

    Boolean result = FALSE;
    mDelegate->IsEnabled(&result);
    return result;
}

Int32 TimePicker::GetBaseline()
{
    // ==================before translated======================
    // return mDelegate.getBaseline();

    Int32 result = 0;
    mDelegate->GetBaseline(&result);
    return result;
}

Boolean TimePicker::DispatchPopulateAccessibilityEvent(
    /* [in] */ IAccessibilityEvent* event)
{
    // ==================before translated======================
    // return mDelegate.dispatchPopulateAccessibilityEvent(event);

    Boolean result = FALSE;
    mDelegate->DispatchPopulateAccessibilityEvent(event, &result);
    return result;
}

ECode TimePicker::OnPopulateAccessibilityEvent(
    /* [in] */ IAccessibilityEvent* event)
{
    VALIDATE_NOT_NULL(event);
    // ==================before translated======================
    // super.onPopulateAccessibilityEvent(event);
    // mDelegate.onPopulateAccessibilityEvent(event);

    FrameLayout::OnPopulateAccessibilityEvent(event);
    mDelegate->OnPopulateAccessibilityEvent(event);
    return NOERROR;
}

ECode TimePicker::OnInitializeAccessibilityEvent(
    /* [in] */ IAccessibilityEvent* event)
{
    VALIDATE_NOT_NULL(event);
    // ==================before translated======================
    // super.onInitializeAccessibilityEvent(event);
    // mDelegate.onInitializeAccessibilityEvent(event);

    FrameLayout::OnInitializeAccessibilityEvent(event);
    mDelegate->OnInitializeAccessibilityEvent(event);
    return NOERROR;
}

ECode TimePicker::OnInitializeAccessibilityNodeInfo(
    /* [in] */ IAccessibilityNodeInfo* info)
{
    VALIDATE_NOT_NULL(info);
    // ==================before translated======================
    // super.onInitializeAccessibilityNodeInfo(info);
    // mDelegate.onInitializeAccessibilityNodeInfo(info);

    FrameLayout::OnInitializeAccessibilityNodeInfo(info);
    mDelegate->OnInitializeAccessibilityNodeInfo(info);
    return NOERROR;
}

void TimePicker::OnConfigurationChanged(
    /* [in] */ IConfiguration* newConfig)
{
    // ==================before translated======================
    // super.onConfigurationChanged(newConfig);
    // mDelegate.onConfigurationChanged(newConfig);

    FrameLayout::OnConfigurationChanged(newConfig);
    mDelegate->OnConfigurationChanged(newConfig);
}

AutoPtr<IParcelable> TimePicker::OnSaveInstanceState()
{
    // ==================before translated======================
    // Parcelable superState = super.onSaveInstanceState();
    // return mDelegate.onSaveInstanceState(superState);

    AutoPtr<IParcelable> superState = FrameLayout::OnSaveInstanceState();
    AutoPtr<IParcelable> result;
    mDelegate->OnSaveInstanceState(superState, (IParcelable**)&result);
    return result;
}

void TimePicker::OnRestoreInstanceState(
    /* [in] */ IParcelable* state)
{
    // ==================before translated======================
    // BaseSavedState ss = (BaseSavedState) state;
    // super.onRestoreInstanceState(ss.getSuperState());
    // mDelegate.onRestoreInstanceState(ss);

    assert(0);
    View::BaseSavedState* ss = (View::BaseSavedState*)state;
    FrameLayout::OnRestoreInstanceState(ss/*ss->SetSuperState()*/);
    mDelegate->OnRestoreInstanceState(ss);
}

} // namespace Widget
} // namespace Droid
} // namespace Elastos


