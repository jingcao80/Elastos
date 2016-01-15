
#include "Elastos.Droid.Os.h"
#include "Elastos.Droid.Text.h"
#include <Elastos.CoreLibrary.Libcore.h>
#include "elastos/droid/R.h"
#include "elastos/droid/text/format/CDateFormat.h"
#include "elastos/droid/text/format/CDateUtils.h"
#include "elastos/droid/view/LayoutInflater.h"
#include "elastos/droid/widget/CNumberPickerHelper.h"
#include "elastos/droid/widget/TimePickerClockDelegate.h"
//#include "libcore/icu/LocaleData.h"

using Elastos::Droid::Text::Format::CDateFormat;
using Elastos::Droid::Text::Format::CDateUtils;
using Elastos::Droid::Text::Format::IDateUtils;
using Elastos::Droid::View::Accessibility::IAccessibilityRecord;
using Elastos::Droid::View::EIID_IViewOnClickListener;
using Elastos::Droid::View::ILayoutInflater;
using Elastos::Droid::View::InputMethod::IInputMethodManager;
using Elastos::Droid::View::IViewGroup;
using Elastos::Droid::View::LayoutInflater;
using Elastos::Droid::Widget::CNumberPickerHelper;
using Elastos::Core::CString;
using Elastos::Utility::CCalendarHelper;
using Elastos::Utility::ICalendar;
using Elastos::Utility::ICalendarHelper;
using Libcore::ICU::CLocaleDataHelper;
using Libcore::ICU::ILocaleData;
using Libcore::ICU::ILocaleDataHelper;
//using Libcore::ICU::LocaleData;

namespace Elastos {
namespace Droid {
namespace Widget {

//=====================================================================
//                 TimePickerClockDelegate::SavedState
//=====================================================================
const AutoPtr<IParcelable> TimePickerClockDelegate::SavedState::CREATOR = TimePickerClockDelegate::SavedState::InitCreator();

ECode TimePickerClockDelegate::SavedState::GetHour(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mHour;

    *result = mHour;
    return NOERROR;
}

ECode TimePickerClockDelegate::SavedState::GetMinute(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mMinute;

    *result = mMinute;
    return NOERROR;
}

ECode TimePickerClockDelegate::SavedState::WriteToParcel(
    /* [in] */ IParcel* dest,
    /* [in] */ Int32 flags)
{
    VALIDATE_NOT_NULL(dest);
    // ==================before translated======================
    // super.writeToParcel(dest, flags);
    // dest.writeInt(mHour);
    // dest.writeInt(mMinute);

    assert(0);
    Elastos::Droid::View::View::BaseSavedState::WriteToParcel(dest/*, flags*/);
    dest->WriteInt32(mHour);
    dest->WriteInt32(mMinute);
    return NOERROR;
}

TimePickerClockDelegate::SavedState::SavedState(
    /* [in] */ IParcelable* superState,
    /* [in] */ Int32 hour,
    /* [in] */ Int32 minute)
    : mHour(hour)
    , mMinute(minute)
{
    // ==================before translated======================
    // super(superState);
    // mHour = hour;
    // mMinute = minute;

    BaseSavedState::constructor(superState);
}

TimePickerClockDelegate::SavedState::SavedState(
    /* [in] */ IParcel* in)
{
    // ==================before translated======================
    // super(in);
    // mHour = in.readInt();
    // mMinute = in.readInt();

    assert(0);
    //BaseSavedState::constructor(in);
    in->ReadInt32(&mHour);
    in->ReadInt32(&mMinute);
}

TimePickerClockDelegate::SavedState::SavedState()
    : mHour(0)
    , mMinute(0)
{
}

AutoPtr<IParcelable> TimePickerClockDelegate::SavedState::InitCreator()
{
    AutoPtr<IParcelable> result = new InnerCreator();
    return result;
}

//=====================================================================
//                TimePickerClockDelegate::InnerCreator
//=====================================================================
CAR_INTERFACE_IMPL(TimePickerClockDelegate::InnerCreator, Object, IParcelable)

TimePickerClockDelegate::InnerCreator::InnerCreator()
{
}

ECode TimePickerClockDelegate::InnerCreator::CreateFromParcel(
    /* [in] */ IParcel* in,
    /* [out] */ SavedState** result)
{
    VALIDATE_NOT_NULL(in);
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return new SavedState(in);

    *result = new SavedState(in);
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode TimePickerClockDelegate::InnerCreator::NewArray(
    /* [in] */ Int32 size,
    /* [out] */ ArrayOf<SavedState*>** result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return new SavedState[size];

    *result = ArrayOf<SavedState*>::Alloc(size);
    for (Int32 idx=0; idx<size; ++idx) {
        AutoPtr<SavedState> item = new SavedState();
        (*result)->Set(idx, item);
    }
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode TimePickerClockDelegate::InnerCreator::ReadFromParcel(
    /* [in] */ IParcel* parcel)
{
    return NOERROR;
}

ECode TimePickerClockDelegate::InnerCreator::WriteToParcel(
    /* [in] */ IParcel* parcel)
{
    return NOERROR;
}

//=====================================================================
//        TimePickerClockDelegate::InnerOnValueChangeListener
//=====================================================================
CAR_INTERFACE_IMPL(TimePickerClockDelegate::InnerOnValueChangeListener, Object, INumberPickerOnValueChangeListener)

TimePickerClockDelegate::InnerOnValueChangeListener::InnerOnValueChangeListener(
    /* [in] */ TimePickerClockDelegate* owner)
    : mOwner(owner)
{
    assert(mOwner);
}

ECode TimePickerClockDelegate::InnerOnValueChangeListener::OnValueChange(
    /* [in] */ INumberPicker* spinner,
    /* [in] */ Int32 oldVal,
    /* [in] */ Int32 newVal)
{
    // ==================before translated======================
    // updateInputState();
    // if (!is24HourView()) {
    //     if ((oldVal == HOURS_IN_HALF_DAY - 1 && newVal == HOURS_IN_HALF_DAY) ||
    //             (oldVal == HOURS_IN_HALF_DAY && newVal == HOURS_IN_HALF_DAY - 1)) {
    //         mIsAm = !mIsAm;
    //         updateAmPmControl();
    //     }
    // }
    // onTimeChanged();

    mOwner->UpdateInputState();
    Boolean is24Hour = FALSE;
    mOwner->Is24HourView(&is24Hour);
    if (!is24Hour) {
        if ((oldVal == HOURS_IN_HALF_DAY - 1 && newVal == HOURS_IN_HALF_DAY) ||
                (oldVal == HOURS_IN_HALF_DAY && newVal == HOURS_IN_HALF_DAY - 1)) {
            mOwner->mIsAm = !mOwner->mIsAm;
            mOwner->UpdateAmPmControl();
        }
    }
    mOwner->OnTimeChanged();
    return NOERROR;
}

//=====================================================================
//        TimePickerClockDelegate::InnerOnValueChangeListener1
//=====================================================================
CAR_INTERFACE_IMPL(TimePickerClockDelegate::InnerOnValueChangeListener1, Object, INumberPickerOnValueChangeListener)

TimePickerClockDelegate::InnerOnValueChangeListener1::InnerOnValueChangeListener1(
    /* [in] */ TimePickerClockDelegate* owner)
    : mOwner(owner)
{
    assert(mOwner);
}

ECode TimePickerClockDelegate::InnerOnValueChangeListener1::OnValueChange(
    /* [in] */ INumberPicker* spinner,
    /* [in] */ Int32 oldVal,
    /* [in] */ Int32 newVal)
{
    // ==================before translated======================
    // updateInputState();
    // int minValue = mMinuteSpinner.getMinValue();
    // int maxValue = mMinuteSpinner.getMaxValue();
    // if (oldVal == maxValue && newVal == minValue) {
    //     int newHour = mHourSpinner.getValue() + 1;
    //     if (!is24HourView() && newHour == HOURS_IN_HALF_DAY) {
    //         mIsAm = !mIsAm;
    //         updateAmPmControl();
    //     }
    //     mHourSpinner.setValue(newHour);
    // } else if (oldVal == minValue && newVal == maxValue) {
    //     int newHour = mHourSpinner.getValue() - 1;
    //     if (!is24HourView() && newHour == HOURS_IN_HALF_DAY - 1) {
    //         mIsAm = !mIsAm;
    //         updateAmPmControl();
    //     }
    //     mHourSpinner.setValue(newHour);
    // }
    // onTimeChanged();

    mOwner->UpdateInputState();
    Int32 minValue = 0, maxValue = 0;
    mOwner->mMinuteSpinner->GetMinValue(&minValue);
    mOwner->mMinuteSpinner->GetMaxValue(&maxValue);
    if (oldVal == maxValue && newVal == minValue) {
        Int32 value = 0;
        mOwner->mHourSpinner->GetValue(&value);
        Int32 newHour = value + 1;

        Boolean resTmp = FALSE;
        mOwner->Is24HourView(&resTmp);
        if (!resTmp && newHour == HOURS_IN_HALF_DAY) {
            mOwner->mIsAm = !mOwner->mIsAm;
            mOwner->UpdateAmPmControl();
        }
        mOwner->mHourSpinner->SetValue(newHour);
    }
    else if (oldVal == minValue && newVal == maxValue) {
        Int32 value = 0;
        mOwner->mHourSpinner->GetValue(&value);
        Int32 newHour = value - 1;
        Boolean resTmp = FALSE;
        mOwner->Is24HourView(&resTmp);
        if (!resTmp && newHour == HOURS_IN_HALF_DAY - 1) {
            mOwner->mIsAm = !mOwner->mIsAm;
            mOwner->UpdateAmPmControl();
        }
        mOwner->mHourSpinner->SetValue(newHour);
    }
    mOwner->OnTimeChanged();
    return NOERROR;
}

//=====================================================================
//        TimePickerClockDelegate::InnerOnValueChangeListener2
//=====================================================================
CAR_INTERFACE_IMPL(TimePickerClockDelegate::InnerOnValueChangeListener2, Object, INumberPickerOnValueChangeListener)

TimePickerClockDelegate::InnerOnValueChangeListener2::InnerOnValueChangeListener2(
    /* [in] */ TimePickerClockDelegate* owner)
    : mOwner(owner)
{
    assert(mOwner);
}

ECode TimePickerClockDelegate::InnerOnValueChangeListener2::OnValueChange(
    /* [in] */ INumberPicker* picker,
    /* [in] */ Int32 oldVal,
    /* [in] */ Int32 newVal)
{
    // ==================before translated======================
    // updateInputState();
    // picker.requestFocus();
    // mIsAm = !mIsAm;
    // updateAmPmControl();
    // onTimeChanged();

    mOwner->UpdateInputState();
    Boolean resTmp;
    (IView::Probe(picker))->RequestFocus(&resTmp);
    mOwner->mIsAm = !mOwner->mIsAm;
    mOwner->UpdateAmPmControl();
    mOwner->OnTimeChanged();
    return NOERROR;
}

//=====================================================================
//          TimePickerClockDelegate::InnerOnClickListener
//=====================================================================
CAR_INTERFACE_IMPL(TimePickerClockDelegate::InnerOnClickListener, Object, IViewOnClickListener)

TimePickerClockDelegate::InnerOnClickListener::InnerOnClickListener(
    /* [in] */ TimePickerClockDelegate* owner)
    : mOwner(owner)
{
    assert(mOwner);
}

ECode TimePickerClockDelegate::InnerOnClickListener::OnClick(
    /* [in] */ IView* button)
{
    VALIDATE_NOT_NULL(button);
    // ==================before translated======================
    // button.requestFocus();
    // mIsAm = !mIsAm;
    // updateAmPmControl();
    // onTimeChanged();

    Boolean resTmp;
    (IView::Probe(button))->RequestFocus(&resTmp);
    mOwner->mIsAm = !mOwner->mIsAm;
    mOwner->UpdateAmPmControl();
    mOwner->OnTimeChanged();
    return NOERROR;
}

//=====================================================================
//                       TimePickerClockDelegate
//=====================================================================
const Boolean TimePickerClockDelegate::DEFAULT_ENABLED_STATE = TRUE;
const Int32 TimePickerClockDelegate::HOURS_IN_HALF_DAY;

CAR_INTERFACE_IMPL(TimePickerClockDelegate, TimePicker::AbstractTimePickerDelegate, ITimePickerClockDelegate)

TimePickerClockDelegate::TimePickerClockDelegate()
    : mIs24HourView(FALSE)
    , mIsAm(FALSE)
    , mIsEnabled(FALSE)
    , mHourWithTwoDigit(FALSE)
    , mHourFormat(0)
{
}

ECode TimePickerClockDelegate::constructor(
    /* [in] */ ITimePicker* delegator,
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ Int32 defStyleAttr,
    /* [in] */ Int32 defStyleRes)
{
    // ==================before translated======================
    // super(delegator, context);
    //
    // // process style attributes
    // final TypedArray a = mContext.obtainStyledAttributes(
    //         attrs, R.styleable.TimePicker, defStyleAttr, defStyleRes);
    // final int layoutResourceId = a.getResourceId(
    //         R.styleable.TimePicker_legacyLayout, R.layout.time_picker_legacy);
    // a.recycle();
    //
    // final LayoutInflater inflater = LayoutInflater.from(mContext);
    // inflater.inflate(layoutResourceId, mDelegator, true);
    //
    // // hour
    // mHourSpinner = (NumberPicker) delegator.findViewById(R.id.hour);
    // mHourSpinner.setOnValueChangedListener(new NumberPicker.OnValueChangeListener() {
    //     public void onValueChange(NumberPicker spinner, int oldVal, int newVal) {
    //         updateInputState();
    //         if (!is24HourView()) {
    //             if ((oldVal == HOURS_IN_HALF_DAY - 1 && newVal == HOURS_IN_HALF_DAY) ||
    //                     (oldVal == HOURS_IN_HALF_DAY && newVal == HOURS_IN_HALF_DAY - 1)) {
    //                 mIsAm = !mIsAm;
    //                 updateAmPmControl();
    //             }
    //         }
    //         onTimeChanged();
    //     }
    // });
    // mHourSpinnerInput = (EditText) mHourSpinner.findViewById(R.id.numberpicker_input);
    // mHourSpinnerInput.setImeOptions(EditorInfo.IME_ACTION_NEXT);
    //
    // // divider (only for the new widget style)
    // mDivider = (TextView) mDelegator.findViewById(R.id.divider);
    // if (mDivider != null) {
    //     setDividerText();
    // }
    //
    // // minute
    // mMinuteSpinner = (NumberPicker) mDelegator.findViewById(R.id.minute);
    // mMinuteSpinner.setMinValue(0);
    // mMinuteSpinner.setMaxValue(59);
    // mMinuteSpinner.setOnLongPressUpdateInterval(100);
    // mMinuteSpinner.setFormatter(NumberPicker.getTwoDigitFormatter());
    // mMinuteSpinner.setOnValueChangedListener(new NumberPicker.OnValueChangeListener() {
    //     public void onValueChange(NumberPicker spinner, int oldVal, int newVal) {
    //         updateInputState();
    //         int minValue = mMinuteSpinner.getMinValue();
    //         int maxValue = mMinuteSpinner.getMaxValue();
    //         if (oldVal == maxValue && newVal == minValue) {
    //             int newHour = mHourSpinner.getValue() + 1;
    //             if (!is24HourView() && newHour == HOURS_IN_HALF_DAY) {
    //                 mIsAm = !mIsAm;
    //                 updateAmPmControl();
    //             }
    //             mHourSpinner.setValue(newHour);
    //         } else if (oldVal == minValue && newVal == maxValue) {
    //             int newHour = mHourSpinner.getValue() - 1;
    //             if (!is24HourView() && newHour == HOURS_IN_HALF_DAY - 1) {
    //                 mIsAm = !mIsAm;
    //                 updateAmPmControl();
    //             }
    //             mHourSpinner.setValue(newHour);
    //         }
    //         onTimeChanged();
    //     }
    // });
    // mMinuteSpinnerInput = (EditText) mMinuteSpinner.findViewById(R.id.numberpicker_input);
    // mMinuteSpinnerInput.setImeOptions(EditorInfo.IME_ACTION_NEXT);
    //
    // // Get the localized am/pm strings and use them in the spinner.
    // mAmPmStrings = getAmPmStrings(context);
    //
    // // am/pm
    // final View amPmView = mDelegator.findViewById(R.id.amPm);
    // if (amPmView instanceof Button) {
    //     mAmPmSpinner = null;
    //     mAmPmSpinnerInput = null;
    //     mAmPmButton = (Button) amPmView;
    //     mAmPmButton.setOnClickListener(new View.OnClickListener() {
    //         public void onClick(View button) {
    //             button.requestFocus();
    //             mIsAm = !mIsAm;
    //             updateAmPmControl();
    //             onTimeChanged();
    //         }
    //     });
    // } else {
    //     mAmPmButton = null;
    //     mAmPmSpinner = (NumberPicker) amPmView;
    //     mAmPmSpinner.setMinValue(0);
    //     mAmPmSpinner.setMaxValue(1);
    //     mAmPmSpinner.setDisplayedValues(mAmPmStrings);
    //     mAmPmSpinner.setOnValueChangedListener(new NumberPicker.OnValueChangeListener() {
    //         public void onValueChange(NumberPicker picker, int oldVal, int newVal) {
    //             updateInputState();
    //             picker.requestFocus();
    //             mIsAm = !mIsAm;
    //             updateAmPmControl();
    //             onTimeChanged();
    //         }
    //     });
    //     mAmPmSpinnerInput = (EditText) mAmPmSpinner.findViewById(R.id.numberpicker_input);
    //     mAmPmSpinnerInput.setImeOptions(EditorInfo.IME_ACTION_DONE);
    // }
    //
    // if (isAmPmAtStart()) {
    //     // Move the am/pm view to the beginning
    //     ViewGroup amPmParent = (ViewGroup) delegator.findViewById(R.id.timePickerLayout);
    //     amPmParent.removeView(amPmView);
    //     amPmParent.addView(amPmView, 0);
    //     // Swap layout margins if needed. They may be not symmetrical (Old Standard Theme
    //     // for example and not for Holo Theme)
    //     ViewGroup.MarginLayoutParams lp =
    //             (ViewGroup.MarginLayoutParams) amPmView.getLayoutParams();
    //     final int startMargin = lp.getMarginStart();
    //     final int endMargin = lp.getMarginEnd();
    //     if (startMargin != endMargin) {
    //         lp.setMarginStart(endMargin);
    //         lp.setMarginEnd(startMargin);
    //     }
    // }
    //
    // getHourFormatData();
    //
    // // update controls to initial state
    // updateHourControl();
    // updateMinuteControl();
    // updateAmPmControl();
    //
    // // set to current time
    // setCurrentHour(mTempCalendar.get(Calendar.HOUR_OF_DAY));
    // setCurrentMinute(mTempCalendar.get(Calendar.MINUTE));
    //
    // if (!isEnabled()) {
    //     setEnabled(false);
    // }
    //
    // // set the content descriptions
    // setContentDescriptions();
    //
    // // If not explicitly specified this view is important for accessibility.
    // if (mDelegator.getImportantForAccessibility() == IMPORTANT_FOR_ACCESSIBILITY_AUTO) {
    //     mDelegator.setImportantForAccessibility(IMPORTANT_FOR_ACCESSIBILITY_YES);
    // }

    assert(0);
    TimePicker::AbstractTimePickerDelegate::constructor(delegator, context);
    AutoPtr< ArrayOf<Int32> > styleAttrs = ArrayOf<Int32>::Alloc(const_cast<Int32*>(R::styleable::TimePicker),
        ArraySize(R::styleable::TimePicker));
    AutoPtr<ITypedArray> a;
    mContext->ObtainStyledAttributes(attrs, styleAttrs, defStyleAttr, defStyleRes, (ITypedArray**)&a);
    Int32 layoutResourceId = 0;
    a->GetResourceId(R::styleable::TimePicker_legacyLayout, R::layout::time_picker_legacy, &layoutResourceId);
    a->Recycle();

    AutoPtr<ILayoutInflater> inflater;
    LayoutInflater::From(mContext, (ILayoutInflater**)&inflater);
    AutoPtr<IView> viewTmp;
    inflater->Inflate(layoutResourceId, IViewGroup::Probe(mDelegator), TRUE, (IView**)&viewTmp);

    AutoPtr<IView> viewTmp1;
    (IView::Probe(delegator))->FindViewById(R::id::hour, (IView**)&viewTmp1);
    mHourSpinner = INumberPicker::Probe(viewTmp1);

    AutoPtr<INumberPickerOnValueChangeListener> valueChangeListener = new InnerOnValueChangeListener(this);
    mHourSpinner->SetOnValueChangedListener(valueChangeListener);

    AutoPtr<IView> viewTmp2;
    (IView::Probe(mHourSpinner))->FindViewById(R::id::numberpicker_input, (IView**)&viewTmp2);
    mHourSpinnerInput = IEditText::Probe(viewTmp2);
    (ITextView::Probe(mHourSpinnerInput))->SetImeOptions(IEditorInfo::IME_ACTION_NEXT);

    AutoPtr<IView> viewTmp3;
    (IView::Probe(mDelegator))->FindViewById(R::id::divider, (IView**)&viewTmp3);
    mDivider = ITextView::Probe(viewTmp3);
    if (mDivider != NULL) {
        SetDividerText();
    }

    AutoPtr<IView> viewTmp4;
    (IView::Probe(mDelegator))->FindViewById(R::id::minute, (IView**)&viewTmp4);
    mMinuteSpinner = INumberPicker::Probe(viewTmp4);
    mMinuteSpinner->SetMinValue(0);
    mMinuteSpinner->SetMaxValue(59);
    mMinuteSpinner->SetOnLongPressUpdateInterval(100);

    AutoPtr<INumberPickerHelper> helper;
    CNumberPickerHelper::AcquireSingleton((INumberPickerHelper**)&helper);
    AutoPtr<INumberPickerFormatter> formatter;
    helper->GetTwoDigitFormatter((INumberPickerFormatter**)&formatter);
    mMinuteSpinner->SetFormatter(formatter);

    AutoPtr<INumberPickerOnValueChangeListener> valueChangeListener1 = new InnerOnValueChangeListener1(this);
    mMinuteSpinner->SetOnValueChangedListener(valueChangeListener1);

    AutoPtr<IView> viewTmp5;
    (IView::Probe(mMinuteSpinner))->FindViewById(R::id::numberpicker_input, (IView**)&viewTmp5);
    mMinuteSpinnerInput = IEditText::Probe(viewTmp5);
    (ITextView::Probe(mMinuteSpinnerInput))->SetImeOptions(IEditorInfo::IME_ACTION_NEXT);

    mAmPmStrings = GetAmPmStrings(context);

    AutoPtr<IView> amPmView;
    (IView::Probe(mDelegator))->FindViewById(R::id::amPm, (IView**)&amPmView);
    IButton* buttonTmp = IButton::Probe(amPmView);
    if (buttonTmp) {
        mAmPmSpinner = NULL;
        mAmPmSpinnerInput = NULL;
        mAmPmButton = buttonTmp;

        AutoPtr<IViewOnClickListener> onClickListener = new InnerOnClickListener(this);
        (IView::Probe(mAmPmButton))->SetOnClickListener(onClickListener);
    }
    else {
        mAmPmButton = NULL;
        mAmPmSpinner = INumberPicker::Probe(amPmView);
        mAmPmSpinner->SetMinValue(0);
        mAmPmSpinner->SetMaxValue(1);
        mAmPmSpinner->SetDisplayedValues(mAmPmStrings);

        AutoPtr<INumberPickerOnValueChangeListener> valueChangeListener2 = new InnerOnValueChangeListener1(this);
        mAmPmSpinner->SetOnValueChangedListener(valueChangeListener2);

        AutoPtr<IView> viewTmp6;
        (IView::Probe(mAmPmSpinner))->FindViewById(R::id::numberpicker_input, (IView**)&viewTmp6);
        mAmPmSpinnerInput = IEditText::Probe(viewTmp6);
        (ITextView::Probe(mAmPmSpinnerInput))->SetImeOptions(IEditorInfo::IME_ACTION_DONE);
    }

    if (IsAmPmAtStart()) {
        AutoPtr<IView> viewTmp7;
        (IView::Probe(delegator))->FindViewById(R::id::timePickerLayout, (IView**)&viewTmp7);
        IViewGroup* amPmParent = IViewGroup::Probe(viewTmp7);
        (IViewGroup::Probe(amPmParent))->RemoveView(amPmView);
        (IViewGroup::Probe(amPmParent))->AddView(amPmView, 0);
        // Swap layout margins if needed. They may be not symmetrical (Old Standard Theme
        // for example and not for Holo Theme)

        AutoPtr<IViewGroupLayoutParams> paramsTmp;
        amPmView->GetLayoutParams((IViewGroupLayoutParams**)&paramsTmp);
        IViewGroupMarginLayoutParams* lp = IViewGroupMarginLayoutParams::Probe(paramsTmp);

        Int32 startMargin = 0, endMargin = 0;
        lp->GetMarginStart(&startMargin);
        lp->GetMarginEnd(&endMargin);
        if (startMargin != endMargin) {
            lp->SetMarginStart(endMargin);
            lp->SetMarginEnd(startMargin);
        }
    }

    GetHourFormatData();

    // update controls to initial state
    UpdateHourControl();
    UpdateMinuteControl();
    UpdateAmPmControl();

    // set to current time
    Int32 hourOfDay = 0, minute = 0;
    mTempCalendar->Get(ICalendar::HOUR_OF_DAY, &hourOfDay);
    mTempCalendar->Get(ICalendar::MINUTE, &minute);
    SetCurrentHour(hourOfDay);
    SetCurrentMinute(minute);

    Boolean isEnabled = FALSE;
    IsEnabled(&isEnabled);
    if (!isEnabled) {
        SetEnabled(FALSE);
    }

    // set the content descriptions
    SetContentDescriptions();

    // If not explicitly specified this view is important for accessibility.
    Int32 accessibility = 0;
    (IView::Probe(mDelegator))->GetImportantForAccessibility(&accessibility);
    if (accessibility == IView::IMPORTANT_FOR_ACCESSIBILITY_AUTO) {
        (IView::Probe(mDelegator))->SetImportantForAccessibility(IView::IMPORTANT_FOR_ACCESSIBILITY_YES);
    }
    return NOERROR;
}

ECode TimePickerClockDelegate::SetCurrentHour(
    /* [in] */ Int32 currentHour)
{
    VALIDATE_NOT_NULL(currentHour);
    // ==================before translated======================
    // setCurrentHour(currentHour, true);

    SetCurrentHour(currentHour, TRUE);
    return NOERROR;
}

ECode TimePickerClockDelegate::GetCurrentHour(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // int currentHour = mHourSpinner.getValue();
    // if (is24HourView()) {
    //     return currentHour;
    // } else if (mIsAm) {
    //     return currentHour % HOURS_IN_HALF_DAY;
    // } else {
    //     return (currentHour % HOURS_IN_HALF_DAY) + HOURS_IN_HALF_DAY;
    // }

    Int32 currentHour = 0;
    mHourSpinner->GetValue(&currentHour);
    Boolean is24Hour = FALSE;
    Is24HourView(&is24Hour);
    if (is24Hour) {
        *result = currentHour;
    }
    else if (mIsAm) {
        *result = currentHour % HOURS_IN_HALF_DAY;
    }
    else {
        *result = (currentHour % HOURS_IN_HALF_DAY) + HOURS_IN_HALF_DAY;
    }
    return NOERROR;
}

ECode TimePickerClockDelegate::SetCurrentMinute(
    /* [in] */ Int32 currentMinute)
{
    // ==================before translated======================
    // if (currentMinute == getCurrentMinute()) {
    //     return;
    // }
    // mMinuteSpinner.setValue(currentMinute);
    // onTimeChanged();

    Int32 minute = 0;
    GetCurrentMinute(&minute);
    if (currentMinute == minute) {
        return NOERROR;
    }
    mMinuteSpinner->SetValue(currentMinute);
    OnTimeChanged();
    return NOERROR;
}

ECode TimePickerClockDelegate::GetCurrentMinute(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mMinuteSpinner.getValue();

    mMinuteSpinner->GetValue(result);
    return NOERROR;
}

ECode TimePickerClockDelegate::SetIs24HourView(
    /* [in] */ Boolean is24HourView)
{
    // ==================before translated======================
    // if (mIs24HourView == is24HourView) {
    //     return;
    // }
    // // cache the current hour since spinner range changes and BEFORE changing mIs24HourView!!
    // int currentHour = getCurrentHour();
    // // Order is important here.
    // mIs24HourView = is24HourView;
    // getHourFormatData();
    // updateHourControl();
    // // set value after spinner range is updated
    // setCurrentHour(currentHour, false);
    // updateMinuteControl();
    // updateAmPmControl();

    if (mIs24HourView == is24HourView) {
        return NOERROR;
    }
    // cache the current hour since spinner range changes and BEFORE changing mIs24HourView!!
    Int32 currentHour = 0;
    GetCurrentHour(&currentHour);
    // Order is important here.
    mIs24HourView = is24HourView;
    GetHourFormatData();
    UpdateHourControl();
    // set value after spinner range is updated
    SetCurrentHour(currentHour, FALSE);
    UpdateMinuteControl();
    UpdateAmPmControl();
    return NOERROR;
}

ECode TimePickerClockDelegate::Is24HourView(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mIs24HourView;

    *result = mIs24HourView;
    return NOERROR;
}

ECode TimePickerClockDelegate::SetOnTimeChangedListener(
    /* [in] */ ITimePickerOnTimeChangedListener* onTimeChangedListener)
{
    // ==================before translated======================
    // mOnTimeChangedListener = onTimeChangedListener;

    mOnTimeChangedListener = onTimeChangedListener;
    return NOERROR;
}

ECode TimePickerClockDelegate::SetEnabled(
    /* [in] */ Boolean enabled)
{
    // ==================before translated======================
    // mMinuteSpinner.setEnabled(enabled);
    // if (mDivider != null) {
    //     mDivider.setEnabled(enabled);
    // }
    // mHourSpinner.setEnabled(enabled);
    // if (mAmPmSpinner != null) {
    //     mAmPmSpinner.setEnabled(enabled);
    // } else {
    //     mAmPmButton.setEnabled(enabled);
    // }
    // mIsEnabled = enabled;

    (IView::Probe(mMinuteSpinner))->SetEnabled(enabled);
    if (mDivider != NULL) {
        (IView::Probe(mDivider))->SetEnabled(enabled);
    }
    (IView::Probe(mHourSpinner))->SetEnabled(enabled);
    if (mAmPmSpinner != NULL) {
        (IView::Probe(mAmPmSpinner))->SetEnabled(enabled);
    }
    else {
        (IView::Probe(mAmPmButton))->SetEnabled(enabled);
    }
    mIsEnabled = enabled;
    return NOERROR;
}

ECode TimePickerClockDelegate::IsEnabled(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mIsEnabled;

    *result = mIsEnabled;
    return NOERROR;
}

ECode TimePickerClockDelegate::GetBaseline(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mHourSpinner.getBaseline();

    (IView::Probe(mHourSpinner))->GetBaseline(result);
    return NOERROR;
}

ECode TimePickerClockDelegate::OnConfigurationChanged(
    /* [in] */ IConfiguration* newConfig)
{
    VALIDATE_NOT_NULL(newConfig);
    // ==================before translated======================
    // setCurrentLocale(newConfig.locale);

    AutoPtr<ILocale> locale;
    newConfig->GetLocale((ILocale**)&locale);
    SetCurrentLocale(locale);
    return NOERROR;
}

ECode TimePickerClockDelegate::OnSaveInstanceState(
    /* [in] */ IParcelable* superState,
    /* [out] */ IParcelable** result)
{
    VALIDATE_NOT_NULL(superState);
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return new SavedState(superState, getCurrentHour(), getCurrentMinute());

    Int32 hour = 0, minute = 0;
    GetCurrentHour(&hour);
    GetCurrentMinute(&minute);
    *result = new SavedState(superState, hour, minute);
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode TimePickerClockDelegate::OnRestoreInstanceState(
    /* [in] */ IParcelable* state)
{
    VALIDATE_NOT_NULL(state);
    // ==================before translated======================
    // SavedState ss = (SavedState) state;
    // setCurrentHour(ss.getHour());
    // setCurrentMinute(ss.getMinute());

    SavedState* ss = (SavedState*)state;
    Int32 hour = 0, minute = 0;
    ss->GetHour(&hour);
    ss->GetMinute(&minute);
    SetCurrentHour(hour);
    SetCurrentMinute(minute);
    return NOERROR;
}

ECode TimePickerClockDelegate::DispatchPopulateAccessibilityEvent(
    /* [in] */ IAccessibilityEvent* event,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(event);
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // onPopulateAccessibilityEvent(event);
    // return true;

    OnPopulateAccessibilityEvent(event);
    *result = TRUE;
    return NOERROR;
}

ECode TimePickerClockDelegate::OnPopulateAccessibilityEvent(
    /* [in] */ IAccessibilityEvent* event)
{
    VALIDATE_NOT_NULL(event);
    // ==================before translated======================
    // int flags = DateUtils.FORMAT_SHOW_TIME;
    // if (mIs24HourView) {
    //     flags |= DateUtils.FORMAT_24HOUR;
    // } else {
    //     flags |= DateUtils.FORMAT_12HOUR;
    // }
    // mTempCalendar.set(Calendar.HOUR_OF_DAY, getCurrentHour());
    // mTempCalendar.set(Calendar.MINUTE, getCurrentMinute());
    // String selectedDateUtterance = DateUtils.formatDateTime(mContext,
    //         mTempCalendar.getTimeInMillis(), flags);
    // event.getText().add(selectedDateUtterance);

    Int32 flags = IDateUtils::FORMAT_SHOW_TIME;
    if (mIs24HourView) {
        flags |= IDateUtils::FORMAT_24HOUR;
    }
    else {
        flags |= IDateUtils::FORMAT_12HOUR;
    }

    Int32 hour = 0, minute = 0;
    GetCurrentHour(&hour);
    GetCurrentMinute(&minute);
    mTempCalendar->Set(ICalendar::HOUR_OF_DAY, hour);
    mTempCalendar->Set(ICalendar::MINUTE, minute);

    AutoPtr<IDateUtils> dateUtils;
    CDateUtils::AcquireSingleton((IDateUtils**)&dateUtils);
    Int64 timeInMillis = 0;
    mTempCalendar->GetTimeInMillis(&timeInMillis);
    String selectedDateUtterance;
    dateUtils->FormatDateTime(mContext, timeInMillis, flags, &selectedDateUtterance);

    AutoPtr<IList> textList;
    (IAccessibilityRecord::Probe(event))->GetText((IList**)&textList);

    AutoPtr<ICharSequence> charSequenceTmp;
    CString::New(selectedDateUtterance, (ICharSequence**)&charSequenceTmp);
    textList->Add(charSequenceTmp);
    return NOERROR;
}

ECode TimePickerClockDelegate::OnInitializeAccessibilityEvent(
    /* [in] */ IAccessibilityEvent* event)
{
    VALIDATE_NOT_NULL(event);
    // ==================before translated======================
    // event.setClassName(TimePicker.class.getName());

    AutoPtr<ICharSequence> charSequence;
    CString::New(String("TimePicker"), (ICharSequence**)&charSequence);
    (IAccessibilityRecord::Probe(event))->SetClassName(charSequence);
    return NOERROR;
}

ECode TimePickerClockDelegate::OnInitializeAccessibilityNodeInfo(
    /* [in] */ IAccessibilityNodeInfo* info)
{
    VALIDATE_NOT_NULL(info);
    // ==================before translated======================
    // info.setClassName(TimePicker.class.getName());

    AutoPtr<ICharSequence> charSequence;
    CString::New(String("TimePicker"), (ICharSequence**)&charSequence);
    info->SetClassName(charSequence);
    return NOERROR;
}

ECode TimePickerClockDelegate::SetCurrentLocale(
    /* [in] */ ILocale* locale)
{
    VALIDATE_NOT_NULL(locale);
    // ==================before translated======================
    // super.setCurrentLocale(locale);
    // mTempCalendar = Calendar.getInstance(locale);

    TimePicker::AbstractTimePickerDelegate::SetCurrentLocale(locale);
    AutoPtr<ICalendarHelper> helper;
    CCalendarHelper::AcquireSingleton((ICalendarHelper**)&helper);
    helper->GetInstance(locale, (ICalendar**)&mTempCalendar);
    return NOERROR;
}

AutoPtr< ArrayOf<String> > TimePickerClockDelegate::GetAmPmStrings(
    /* [in] */ IContext* context)
{
    // ==================before translated======================
    // String[] result = new String[2];
    // LocaleData d = LocaleData.get(context.getResources().getConfiguration().locale);
    // result[0] = d.amPm[0].length() > 2 ? d.narrowAm : d.amPm[0];
    // result[1] = d.amPm[1].length() > 2 ? d.narrowPm : d.amPm[1];
    // return result;

    assert(0);
    AutoPtr< ArrayOf<String> > result = ArrayOf<String>::Alloc(2);
    AutoPtr<IResources> resources;
    context->GetResources((IResources**)&resources);
    AutoPtr<IConfiguration> configuration;
    resources->GetConfiguration((IConfiguration**)&configuration);
    AutoPtr<ILocale> locale;
    configuration->GetLocale((ILocale**)&locale);

    AutoPtr<ILocaleDataHelper> helper;
    CLocaleDataHelper::AcquireSingleton((ILocaleDataHelper**)&helper);
    AutoPtr<ILocaleData> d;
    helper->Get(locale, (ILocaleData**)&d);

    AutoPtr< ArrayOf<String> > amPm;
    d->GetAmPm((ArrayOf<String>**)&amPm);
    String narrowAm;// = (LocaleData*)d->mNarrowAm;
    String narrowPm;// = (LocaleData*)d->mNarrowPm;

    Int32 len = (*amPm)[0].GetLength();
    result->Set(0, len > 2 ? narrowAm : (*amPm)[0]);
    result->Set(1, len > 2 ? narrowPm : (*amPm)[1]);
    return result;
}

void TimePickerClockDelegate::GetHourFormatData()
{
    // ==================before translated======================
    // final String bestDateTimePattern = DateFormat.getBestDateTimePattern(mCurrentLocale,
    //         (mIs24HourView) ? "Hm" : "hm");
    // final int lengthPattern = bestDateTimePattern.length();
    // mHourWithTwoDigit = false;
    // char hourFormat = '\0';
    // // Check if the returned pattern is single or double 'H', 'h', 'K', 'k'. We also save
    // // the hour format that we found.
    // for (int i = 0; i < lengthPattern; i++) {
    //     final char c = bestDateTimePattern.charAt(i);
    //     if (c == 'H' || c == 'h' || c == 'K' || c == 'k') {
    //         mHourFormat = c;
    //         if (i + 1 < lengthPattern && c == bestDateTimePattern.charAt(i + 1)) {
    //             mHourWithTwoDigit = true;
    //         }
    //         break;
    //     }
    // }

    using Elastos::Droid::Text::Format::IDateFormat;
    AutoPtr<IDateFormat> dateFormat;
    CDateFormat::AcquireSingleton((IDateFormat**)&dateFormat);
    String bestDateTimePattern;
    dateFormat->GetBestDateTimePattern(mCurrentLocale, (mIs24HourView) ? String("Hm") : String("hm"), &bestDateTimePattern);
    Int32 lengthPattern = bestDateTimePattern.GetLength();
    mHourWithTwoDigit = FALSE;
    // Check if the returned pattern is single or double 'H', 'h', 'K', 'k'. We also save
    // the hour format that we found.
    Char32 c;
    for (Int32 i = 0; i < lengthPattern; ++i) {
        c = bestDateTimePattern.GetChar(i);
        if (c == 'H' || c == 'h' || c == 'K' || c == 'k') {
            mHourFormat = c;
            if (i + 1 < lengthPattern && c == bestDateTimePattern.GetChar(i + 1)) {
                mHourWithTwoDigit = TRUE;
            }
            break;
        }
    }
}

Boolean TimePickerClockDelegate::IsAmPmAtStart()
{
    // ==================before translated======================
    // final String bestDateTimePattern = DateFormat.getBestDateTimePattern(mCurrentLocale,
    //         "hm" /* skeleton */);
    //
    // return bestDateTimePattern.startsWith("a");

    using Elastos::Droid::Text::Format::IDateFormat;
    AutoPtr<IDateFormat> dateFormat;
    CDateFormat::AcquireSingleton((IDateFormat**)&dateFormat);
    String bestDateTimePattern;
    dateFormat->GetBestDateTimePattern(mCurrentLocale, String("hm"), &bestDateTimePattern);
    return bestDateTimePattern.StartWith("a");
}

void TimePickerClockDelegate::SetDividerText()
{
    // ==================before translated======================
    // final String skeleton = (mIs24HourView) ? "Hm" : "hm";
    // final String bestDateTimePattern = DateFormat.getBestDateTimePattern(mCurrentLocale,
    //         skeleton);
    // final String separatorText;
    // int hourIndex = bestDateTimePattern.lastIndexOf('H');
    // if (hourIndex == -1) {
    //     hourIndex = bestDateTimePattern.lastIndexOf('h');
    // }
    // if (hourIndex == -1) {
    //     // Default case
    //     separatorText = ":";
    // } else {
    //     int minuteIndex = bestDateTimePattern.indexOf('m', hourIndex + 1);
    //     if  (minuteIndex == -1) {
    //         separatorText = Character.toString(bestDateTimePattern.charAt(hourIndex + 1));
    //     } else {
    //         separatorText = bestDateTimePattern.substring(hourIndex + 1, minuteIndex);
    //     }
    // }
    // mDivider.setText(separatorText);

    using Elastos::Droid::Text::Format::IDateFormat;
    String skeleton = (mIs24HourView) ? String("Hm") : String("hm");
    AutoPtr<IDateFormat> dateFormat;
    CDateFormat::AcquireSingleton((IDateFormat**)&dateFormat);
    String bestDateTimePattern;
    dateFormat->GetBestDateTimePattern(mCurrentLocale, skeleton, &bestDateTimePattern);

    String separatorText;
    Int32 hourIndex = bestDateTimePattern.LastIndexOf('H');
    if (hourIndex == -1) {
        hourIndex = bestDateTimePattern.LastIndexOf('h');
    }
    if (hourIndex == -1) {
        // Default case
        separatorText = ":";
    }
    else {
        Int32 minuteIndex = bestDateTimePattern.IndexOf('m', hourIndex + 1);
        if  (minuteIndex == -1) {
            separatorText = "";
            separatorText.Append(bestDateTimePattern.GetChar(hourIndex + 1));
        }
        else {
            separatorText = bestDateTimePattern.Substring(hourIndex + 1, minuteIndex);
        }
    }

    AutoPtr<ICharSequence> charSequence;
    CString::New(separatorText, (ICharSequence**)&charSequence);
    mDivider->SetText(charSequence);
}

void TimePickerClockDelegate::SetCurrentHour(
    /* [in] */ Int32 currentHour,
    /* [in] */ Boolean notifyTimeChanged)
{
    // ==================before translated======================
    // // why was Integer used in the first place?
    // if (currentHour == null || currentHour == getCurrentHour()) {
    //     return;
    // }
    // if (!is24HourView()) {
    //     // convert [0,23] ordinal to wall clock display
    //     if (currentHour >= HOURS_IN_HALF_DAY) {
    //         mIsAm = false;
    //         if (currentHour > HOURS_IN_HALF_DAY) {
    //             currentHour = currentHour - HOURS_IN_HALF_DAY;
    //         }
    //     } else {
    //         mIsAm = true;
    //         if (currentHour == 0) {
    //             currentHour = HOURS_IN_HALF_DAY;
    //         }
    //     }
    //     updateAmPmControl();
    // }
    // mHourSpinner.setValue(currentHour);
    // if (notifyTimeChanged) {
    //     onTimeChanged();
    // }

    Int32 currHour = 0;
    GetCurrentHour(&currHour);
    if (currentHour == currHour) {
        return;
    }

    Boolean is24Hour = FALSE;
    Is24HourView(&is24Hour);
    if (!is24Hour) {
        // convert [0,23] ordinal to wall clock display
        if (currentHour >= HOURS_IN_HALF_DAY) {
            mIsAm = FALSE;
            if (currentHour > HOURS_IN_HALF_DAY) {
                currentHour = currentHour - HOURS_IN_HALF_DAY;
            }
        }
        else {
            mIsAm = TRUE;
            if (currentHour == 0) {
                currentHour = HOURS_IN_HALF_DAY;
            }
        }
        UpdateAmPmControl();
    }
    mHourSpinner->SetValue(currentHour);
    if (notifyTimeChanged) {
        OnTimeChanged();
    }
}

void TimePickerClockDelegate::UpdateInputState()
{
    // ==================before translated======================
    // // Make sure that if the user changes the value and the IME is active
    // // for one of the inputs if this widget, the IME is closed. If the user
    // // changed the value via the IME and there is a next input the IME will
    // // be shown, otherwise the user chose another means of changing the
    // // value and having the IME up makes no sense.
    // InputMethodManager inputMethodManager = InputMethodManager.peekInstance();
    // if (inputMethodManager != null) {
    //     if (inputMethodManager.isActive(mHourSpinnerInput)) {
    //         mHourSpinnerInput.clearFocus();
    //         inputMethodManager.hideSoftInputFromWindow(mDelegator.getWindowToken(), 0);
    //     } else if (inputMethodManager.isActive(mMinuteSpinnerInput)) {
    //         mMinuteSpinnerInput.clearFocus();
    //         inputMethodManager.hideSoftInputFromWindow(mDelegator.getWindowToken(), 0);
    //     } else if (inputMethodManager.isActive(mAmPmSpinnerInput)) {
    //         mAmPmSpinnerInput.clearFocus();
    //         inputMethodManager.hideSoftInputFromWindow(mDelegator.getWindowToken(), 0);
    //     }
    // }

    assert(0);
    //-- has no this interface but need have: AutoPtr<IInputMethodManagerHelper> helper;
    //CInputMethodManagerHelper::AcquireSingleton((IInputMethodManagerHelper**)&helper);
    AutoPtr<IInputMethodManager> inputMethodManager;
    //helper->PeekInstance((IInputMethodManager**)&inputMethodManager);
    if (inputMethodManager != NULL) {
        Boolean isHourSpinnerActive = FALSE;
        inputMethodManager->IsActive(IView::Probe(mHourSpinnerInput), &isHourSpinnerActive);

        Boolean isMinuteSpinnerActive = FALSE;
        inputMethodManager->IsActive(IView::Probe(mMinuteSpinnerInput), &isMinuteSpinnerActive);

        Boolean isAmpmSpinnerActive = FALSE;
        inputMethodManager->IsActive(IView::Probe(mAmPmSpinnerInput), &isAmpmSpinnerActive);

        AutoPtr<IBinder> binder;
        (IView::Probe(mDelegator))->GetWindowToken((IBinder**)&binder);
        Boolean resTmp;
        if (isHourSpinnerActive) {
            IView::Probe(mHourSpinnerInput)->ClearFocus();
            inputMethodManager->HideSoftInputFromWindow(binder, 0, &resTmp);
        }
        else if (isMinuteSpinnerActive) {
            IView::Probe(mMinuteSpinnerInput)->ClearFocus();
            inputMethodManager->HideSoftInputFromWindow(binder, 0, &resTmp);
        }
        else if (isAmpmSpinnerActive) {
            IView::Probe(mAmPmSpinnerInput)->ClearFocus();
            inputMethodManager->HideSoftInputFromWindow(binder, 0, &resTmp);
        }
    }
}

void TimePickerClockDelegate::UpdateAmPmControl()
{
    // ==================before translated======================
    // if (is24HourView()) {
    //     if (mAmPmSpinner != null) {
    //         mAmPmSpinner.setVisibility(View.GONE);
    //     } else {
    //         mAmPmButton.setVisibility(View.GONE);
    //     }
    // } else {
    //     int index = mIsAm ? Calendar.AM : Calendar.PM;
    //     if (mAmPmSpinner != null) {
    //         mAmPmSpinner.setValue(index);
    //         mAmPmSpinner.setVisibility(View.VISIBLE);
    //     } else {
    //         mAmPmButton.setText(mAmPmStrings[index]);
    //         mAmPmButton.setVisibility(View.VISIBLE);
    //     }
    // }
    // mDelegator.sendAccessibilityEvent(AccessibilityEvent.TYPE_VIEW_SELECTED);

    Boolean is24Hour = FALSE;
    Is24HourView(&is24Hour);
    if (is24Hour) {
        if (mAmPmSpinner != NULL) {
            IView::Probe(mAmPmSpinner)->SetVisibility(IView::GONE);
        }
        else {
            IView::Probe(mAmPmButton)->SetVisibility(IView::GONE);
        }
    }
    else {
        Int32 index = mIsAm ? ICalendar::AM : ICalendar::PM;
        if (mAmPmSpinner != NULL) {
            mAmPmSpinner->SetValue(index);
            IView::Probe(mAmPmSpinner)->SetVisibility(IView::VISIBLE);
        }
        else {
            AutoPtr<ICharSequence> charSequence;
            CString::New((*mAmPmStrings)[index], (ICharSequence**)&charSequence);
            (ITextView::Probe(mAmPmButton))->SetText(charSequence);
            IView::Probe(mAmPmButton)->SetVisibility(IView::VISIBLE);
        }
    }
    IView::Probe(mDelegator)->SendAccessibilityEvent(IAccessibilityEvent::TYPE_VIEW_SELECTED);
}

void TimePickerClockDelegate::OnTimeChanged()
{
    // ==================before translated======================
    // mDelegator.sendAccessibilityEvent(AccessibilityEvent.TYPE_VIEW_SELECTED);
    // if (mOnTimeChangedListener != null) {
    //     mOnTimeChangedListener.onTimeChanged(mDelegator, getCurrentHour(),
    //             getCurrentMinute());
    // }

    IView::Probe(mDelegator)->SendAccessibilityEvent(IAccessibilityEvent::TYPE_VIEW_SELECTED);
    if (mOnTimeChangedListener != NULL) {
        Int32 hour = 0, minute = 0;
        GetCurrentHour(&hour);
        GetCurrentMinute(&minute);
        mOnTimeChangedListener->OnTimeChanged(mDelegator, hour, minute);
    }
}

void TimePickerClockDelegate::UpdateHourControl()
{
    // ==================before translated======================
    // if (is24HourView()) {
    //     // 'k' means 1-24 hour
    //     if (mHourFormat == 'k') {
    //         mHourSpinner.setMinValue(1);
    //         mHourSpinner.setMaxValue(24);
    //     } else {
    //         mHourSpinner.setMinValue(0);
    //         mHourSpinner.setMaxValue(23);
    //     }
    // } else {
    //     // 'K' means 0-11 hour
    //     if (mHourFormat == 'K') {
    //         mHourSpinner.setMinValue(0);
    //         mHourSpinner.setMaxValue(11);
    //     } else {
    //         mHourSpinner.setMinValue(1);
    //         mHourSpinner.setMaxValue(12);
    //     }
    // }
    // mHourSpinner.setFormatter(mHourWithTwoDigit ? NumberPicker.getTwoDigitFormatter() : null);

    assert(0);
    Boolean is24Hour = FALSE;
    Is24HourView(&is24Hour);
    if (is24Hour) {
        // 'k' means 1-24 hour
        if (mHourFormat == 'k') {
            mHourSpinner->SetMinValue(1);
            mHourSpinner->SetMaxValue(24);
        }
        else {
            mHourSpinner->SetMinValue(0);
            mHourSpinner->SetMaxValue(23);
        }
    }
    else {
        // 'K' means 0-11 hour
        if (mHourFormat == 'K') {
            mHourSpinner->SetMinValue(0);
            mHourSpinner->SetMaxValue(11);
        }
        else {
            mHourSpinner->SetMinValue(1);
            mHourSpinner->SetMaxValue(12);
        }
    }

    AutoPtr<INumberPickerHelper> helper;
    CNumberPickerHelper::AcquireSingleton((INumberPickerHelper**)&helper);
    AutoPtr<INumberPickerFormatter> formatter;
    helper->GetTwoDigitFormatter((INumberPickerFormatter**)&formatter);
    mHourSpinner->SetFormatter(mHourWithTwoDigit ? formatter: NULL);
}

void TimePickerClockDelegate::UpdateMinuteControl()
{
    // ==================before translated======================
    // if (is24HourView()) {
    //     mMinuteSpinnerInput.setImeOptions(EditorInfo.IME_ACTION_DONE);
    // } else {
    //     mMinuteSpinnerInput.setImeOptions(EditorInfo.IME_ACTION_NEXT);
    // }

    Boolean is24Hour = FALSE;
    Is24HourView(&is24Hour);
    if (is24Hour) {
        ITextView::Probe(mMinuteSpinnerInput)->SetImeOptions(IEditorInfo::IME_ACTION_DONE);
    }
    else {
        ITextView::Probe(mMinuteSpinnerInput)->SetImeOptions(IEditorInfo::IME_ACTION_NEXT);
    }
}

void TimePickerClockDelegate::SetContentDescriptions()
{
    // ==================before translated======================
    // // Minute
    // trySetContentDescription(mMinuteSpinner, R.id.increment,
    //         R.string.time_picker_increment_minute_button);
    // trySetContentDescription(mMinuteSpinner, R.id.decrement,
    //         R.string.time_picker_decrement_minute_button);
    // // Hour
    // trySetContentDescription(mHourSpinner, R.id.increment,
    //         R.string.time_picker_increment_hour_button);
    // trySetContentDescription(mHourSpinner, R.id.decrement,
    //         R.string.time_picker_decrement_hour_button);
    // // AM/PM
    // if (mAmPmSpinner != null) {
    //     trySetContentDescription(mAmPmSpinner, R.id.increment,
    //             R.string.time_picker_increment_set_pm_button);
    //     trySetContentDescription(mAmPmSpinner, R.id.decrement,
    //             R.string.time_picker_decrement_set_am_button);
    // }

    TrySetContentDescription(IView::Probe(mMinuteSpinner), R::id::increment, R::string::time_picker_increment_minute_button);
    TrySetContentDescription(IView::Probe(mMinuteSpinner), R::id::decrement, R::string::time_picker_decrement_minute_button);
    // Hour
    TrySetContentDescription(IView::Probe(mHourSpinner), R::id::increment, R::string::time_picker_increment_hour_button);
    TrySetContentDescription(IView::Probe(mHourSpinner), R::id::decrement, R::string::time_picker_decrement_hour_button);
    // AM/PM
    if (mAmPmSpinner != NULL) {
        TrySetContentDescription(IView::Probe(mAmPmSpinner), R::id::increment, R::string::time_picker_increment_set_pm_button);
        TrySetContentDescription(IView::Probe(mAmPmSpinner), R::id::decrement, R::string::time_picker_decrement_set_am_button);
    }
}

void TimePickerClockDelegate::TrySetContentDescription(
    /* [in] */ IView* root,
    /* [in] */ Int32 viewId,
    /* [in] */ Int32 contDescResId)
{
    // ==================before translated======================
    // View target = root.findViewById(viewId);
    // if (target != null) {
    //     target.setContentDescription(mContext.getString(contDescResId));
    // }

    AutoPtr<IView> target;
    root->FindViewById(viewId, (IView**)&target);
    if (target != NULL) {
        String str;
        mContext->GetString(contDescResId, &str);
        AutoPtr<ICharSequence> charSequence;
        CString::New(str, (ICharSequence**)&charSequence);
        target->SetContentDescription(charSequence);
    }
}

} // namespace Widget
} // namespace Droid
} // namespace Elastos


