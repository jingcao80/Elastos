
#include "Elastos.Droid.Os.h"
#include "Elastos.Droid.Text.h"
#include <Elastos.CoreLibrary.Libcore.h>
#include "elastos/droid/widget/TimePickerClockDelegate.h"
#include "elastos/droid/widget/CTimePickerClockDelegateSavedState.h"
#include "elastos/droid/widget/CNumberPickerHelper.h"
#include "elastos/droid/text/format/CDateFormat.h"
#include "elastos/droid/text/format/CDateUtils.h"
#include "elastos/droid/view/inputmethod/CInputMethodManager.h"
#include "elastos/droid/view/LayoutInflater.h"
#include "elastos/droid/R.h"
#include <elastos/core/StringBuilder.h>
//#include "libcore/icu/LocaleData.h"

using Elastos::Droid::Text::Format::CDateFormat;
using Elastos::Droid::Text::Format::CDateUtils;
using Elastos::Droid::Text::Format::IDateUtils;
using Elastos::Droid::View::Accessibility::IAccessibilityRecord;
using Elastos::Droid::View::EIID_IViewOnClickListener;
using Elastos::Droid::View::ILayoutInflater;
using Elastos::Droid::View::InputMethod::IInputMethodManager;
using Elastos::Droid::View::InputMethod::CInputMethodManager;
using Elastos::Droid::View::IViewGroup;
using Elastos::Droid::View::LayoutInflater;
using Elastos::Droid::Widget::CNumberPickerHelper;
using Elastos::Core::CString;
using Elastos::Core::StringBuilder;
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

CAR_INTERFACE_IMPL(TimePickerClockDelegate::SavedState, Elastos::Droid::View::View::BaseSavedState, ITimePickerClockDelegateSavedState)

TimePickerClockDelegate::SavedState::SavedState()
    : mHour(0)
    , mMinute(0)
{}

TimePickerClockDelegate::SavedState::~SavedState()
{}

ECode TimePickerClockDelegate::SavedState::constructor()
{
    return Elastos::Droid::View::View::BaseSavedState::constructor();
}

ECode TimePickerClockDelegate::SavedState::constructor(
    /* [in] */ IParcelable* superState,
    /* [in] */ Int32 hour,
    /* [in] */ Int32 minute)
{
    FAIL_RETURN(Elastos::Droid::View::View::BaseSavedState::constructor(superState))

    mHour = hour;
    mMinute = minute;
    return NOERROR;
}

Int32 TimePickerClockDelegate::SavedState::GetHour()
{
    return mHour;
}

Int32 TimePickerClockDelegate::SavedState::GetMinute()
{
    return mMinute;
}

ECode TimePickerClockDelegate::SavedState::WriteToParcel(
    /* [in] */ IParcel* dest)
{
    FAIL_RETURN(Elastos::Droid::View::View::BaseSavedState::WriteToParcel(dest));
    dest->WriteInt32(mHour);
    dest->WriteInt32(mMinute);
    return NOERROR;
}

ECode TimePickerClockDelegate::SavedState::ReadFromParcel(
    /* [in] */ IParcel* source)
{
    FAIL_RETURN(Elastos::Droid::View::View::BaseSavedState::ReadFromParcel(source));
    source->ReadInt32(&mHour);
    source->ReadInt32(&mMinute);
    return NOERROR;
}

ECode TimePickerClockDelegate::SavedState::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str);
    StringBuilder buider;
    buider += "TimePickerClockDelegate.SavedState{";

    buider += " mHour=";
    buider += mHour;
    buider += " mMinute=";
    buider += mMinute;
    buider += "}";

    *str = buider.ToString();
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
    mOwner->UpdateInputState();
    Boolean resTmp;
    IView::Probe(picker)->RequestFocus(&resTmp);
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

    Boolean resTmp;
    IView::Probe(button)->RequestFocus(&resTmp);
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
    , mIsEnabled(DEFAULT_ENABLED_STATE)
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
    TimePicker::AbstractTimePickerDelegate::constructor(delegator, context);

    // process style attributes
    AutoPtr< ArrayOf<Int32> > styleAttrs =
            ArrayOf<Int32>::Alloc(const_cast<Int32*>(R::styleable::TimePicker),
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

    // hour
    AutoPtr<IView> viewTmp1;
    IView::Probe(delegator)->FindViewById(R::id::hour, (IView**)&viewTmp1);
    mHourSpinner = INumberPicker::Probe(viewTmp1);

    AutoPtr<INumberPickerOnValueChangeListener> valueChangeListener = new InnerOnValueChangeListener(this);
    mHourSpinner->SetOnValueChangedListener(valueChangeListener);

    AutoPtr<IView> viewTmp2;
    IView::Probe(mHourSpinner)->FindViewById(R::id::numberpicker_input, (IView**)&viewTmp2);
    mHourSpinnerInput = IEditText::Probe(viewTmp2);
    ITextView::Probe(mHourSpinnerInput)->SetImeOptions(IEditorInfo::IME_ACTION_NEXT);

    AutoPtr<IView> viewDelegator = IView::Probe(mDelegator);
    // divider (only for the new widget style)
    AutoPtr<IView> viewTmp3;
    viewDelegator->FindViewById(R::id::divider, (IView**)&viewTmp3);
    mDivider = ITextView::Probe(viewTmp3);
    if (mDivider != NULL) {
        SetDividerText();
    }

    // minute
    AutoPtr<IView> viewTmp4;
    viewDelegator->FindViewById(R::id::minute, (IView**)&viewTmp4);
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
    IView::Probe(mMinuteSpinner)->FindViewById(R::id::numberpicker_input, (IView**)&viewTmp5);
    mMinuteSpinnerInput = IEditText::Probe(viewTmp5);
    ITextView::Probe(mMinuteSpinnerInput)->SetImeOptions(IEditorInfo::IME_ACTION_NEXT);

    // Get the localized am/pm strings and use them in the spinner.
    mAmPmStrings = GetAmPmStrings(context);

    // am/pm
    AutoPtr<IView> amPmView;
    viewDelegator->FindViewById(R::id::amPm, (IView**)&amPmView);
    IButton* buttonTmp = IButton::Probe(amPmView);
    if (buttonTmp) {
        mAmPmSpinner = NULL;
        mAmPmSpinnerInput = NULL;
        mAmPmButton = buttonTmp;

        AutoPtr<IViewOnClickListener> onClickListener = new InnerOnClickListener(this);
        IView::Probe(mAmPmButton)->SetOnClickListener(onClickListener);
    }
    else {
        mAmPmButton = NULL;
        mAmPmSpinner = INumberPicker::Probe(amPmView);
        mAmPmSpinner->SetMinValue(0);
        mAmPmSpinner->SetMaxValue(1);
        mAmPmSpinner->SetDisplayedValues(mAmPmStrings);

        AutoPtr<INumberPickerOnValueChangeListener> valueChangeListener2 = new InnerOnValueChangeListener2(this);
        mAmPmSpinner->SetOnValueChangedListener(valueChangeListener2);

        AutoPtr<IView> viewTmp6;
        IView::Probe(mAmPmSpinner)->FindViewById(R::id::numberpicker_input, (IView**)&viewTmp6);
        mAmPmSpinnerInput = IEditText::Probe(viewTmp6);
        ITextView::Probe(mAmPmSpinnerInput)->SetImeOptions(IEditorInfo::IME_ACTION_DONE);
    }

    if (IsAmPmAtStart()) {
        // Move the am/pm view to the beginning
        AutoPtr<IView> viewTmp7;
        IView::Probe(delegator)->FindViewById(R::id::timePickerLayout, (IView**)&viewTmp7);
        IViewGroup* amPmParent = IViewGroup::Probe(viewTmp7);
        amPmParent->RemoveView(amPmView);
        amPmParent->AddView(amPmView, 0);
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
    viewDelegator->GetImportantForAccessibility(&accessibility);
    if (accessibility == IView::IMPORTANT_FOR_ACCESSIBILITY_AUTO) {
        viewDelegator->SetImportantForAccessibility(IView::IMPORTANT_FOR_ACCESSIBILITY_YES);
    }
    return NOERROR;
}

ECode TimePickerClockDelegate::SetCurrentHour(
    /* [in] */ Int32 currentHour)
{
    VALIDATE_NOT_NULL(currentHour);

    SetCurrentHour(currentHour, TRUE);
    return NOERROR;
}

ECode TimePickerClockDelegate::GetCurrentHour(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);

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

    return mMinuteSpinner->GetValue(result);
}

ECode TimePickerClockDelegate::SetIs24HourView(
    /* [in] */ Boolean is24HourView)
{
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
    *result = mIs24HourView;
    return NOERROR;
}

ECode TimePickerClockDelegate::SetOnTimeChangedListener(
    /* [in] */ ITimePickerOnTimeChangedListener* onTimeChangedListener)
{
    mOnTimeChangedListener = onTimeChangedListener;
    return NOERROR;
}

ECode TimePickerClockDelegate::SetEnabled(
    /* [in] */ Boolean enabled)
{
    IView::Probe(mMinuteSpinner)->SetEnabled(enabled);
    if (mDivider != NULL) {
        IView::Probe(mDivider)->SetEnabled(enabled);
    }
    IView::Probe(mHourSpinner)->SetEnabled(enabled);
    if (mAmPmSpinner != NULL) {
        IView::Probe(mAmPmSpinner)->SetEnabled(enabled);
    }
    else {
        IView::Probe(mAmPmButton)->SetEnabled(enabled);
    }
    mIsEnabled = enabled;
    return NOERROR;
}

ECode TimePickerClockDelegate::IsEnabled(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    *result = mIsEnabled;
    return NOERROR;
}

ECode TimePickerClockDelegate::GetBaseline(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);

    return IView::Probe(mHourSpinner)->GetBaseline(result);
}

ECode TimePickerClockDelegate::OnConfigurationChanged(
    /* [in] */ IConfiguration* newConfig)
{
    VALIDATE_NOT_NULL(newConfig);

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

    Int32 hour = 0, minute = 0;
    GetCurrentHour(&hour);
    GetCurrentMinute(&minute);

    AutoPtr<ITimePickerClockDelegateSavedState> ss;
    CTimePickerClockDelegateSavedState::New(
            superState, hour, minute, (ITimePickerClockDelegateSavedState**)&ss);

    *result = IParcelable::Probe(ss);
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode TimePickerClockDelegate::OnRestoreInstanceState(
    /* [in] */ IParcelable* state)
{
    VALIDATE_NOT_NULL(state);

    AutoPtr<CTimePickerClockDelegateSavedState> ss =
            (CTimePickerClockDelegateSavedState*)ITimePickerClockDelegateSavedState::Probe(state);
    if (!ss) return NOERROR;

    SetCurrentHour(ss->GetHour());
    SetCurrentMinute(ss->GetMinute());

    return NOERROR;
}

ECode TimePickerClockDelegate::DispatchPopulateAccessibilityEvent(
    /* [in] */ IAccessibilityEvent* event,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(event);
    VALIDATE_NOT_NULL(result);

    OnPopulateAccessibilityEvent(event);
    *result = TRUE;
    return NOERROR;
}

ECode TimePickerClockDelegate::OnPopulateAccessibilityEvent(
    /* [in] */ IAccessibilityEvent* event)
{
    VALIDATE_NOT_NULL(event);

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
    IAccessibilityRecord::Probe(event)->GetText((IList**)&textList);

    AutoPtr<ICharSequence> charSequenceTmp;
    CString::New(selectedDateUtterance, (ICharSequence**)&charSequenceTmp);
    textList->Add(charSequenceTmp);
    return NOERROR;
}

ECode TimePickerClockDelegate::OnInitializeAccessibilityEvent(
    /* [in] */ IAccessibilityEvent* event)
{
    VALIDATE_NOT_NULL(event);

    AutoPtr<ICharSequence> charSequence;
    CString::New(String("CTimePicker"), (ICharSequence**)&charSequence);
    (IAccessibilityRecord::Probe(event))->SetClassName(charSequence);
    return NOERROR;
}

ECode TimePickerClockDelegate::OnInitializeAccessibilityNodeInfo(
    /* [in] */ IAccessibilityNodeInfo* info)
{
    VALIDATE_NOT_NULL(info);

    AutoPtr<ICharSequence> charSequence;
    CString::New(String("CTimePicker"), (ICharSequence**)&charSequence);
    info->SetClassName(charSequence);
    return NOERROR;
}

ECode TimePickerClockDelegate::SetCurrentLocale(
    /* [in] */ ILocale* locale)
{
    VALIDATE_NOT_NULL(locale);

    TimePicker::AbstractTimePickerDelegate::SetCurrentLocale(locale);
    AutoPtr<ICalendarHelper> helper;
    CCalendarHelper::AcquireSingleton((ICalendarHelper**)&helper);
    helper->GetInstance(locale, (ICalendar**)&mTempCalendar);
    return NOERROR;
}

AutoPtr< ArrayOf<String> > TimePickerClockDelegate::GetAmPmStrings(
    /* [in] */ IContext* context)
{
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
    String narrowAm;
    d->GetNarrowAm(&narrowAm);
    String narrowPm;
    d->GetNarrowPm(&narrowPm);

    Int32 len1 = (*amPm)[0].GetLength();
    Int32 len2 = (*amPm)[1].GetLength();
    result->Set(0, len1 > 2 ? narrowAm : (*amPm)[0]);
    result->Set(1, len2 > 2 ? narrowPm : (*amPm)[1]);
    return result;
}

void TimePickerClockDelegate::GetHourFormatData()
{
    using Elastos::Droid::Text::Format::IDateFormat;
    AutoPtr<IDateFormat> dateFormat;
    CDateFormat::AcquireSingleton((IDateFormat**)&dateFormat);
    String bestDateTimePattern;
    dateFormat->GetBestDateTimePattern(mCurrentLocale,
            (mIs24HourView) ? String("Hm") : String("hm"), &bestDateTimePattern);
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
    using Elastos::Droid::Text::Format::IDateFormat;
    AutoPtr<IDateFormat> dateFormat;
    CDateFormat::AcquireSingleton((IDateFormat**)&dateFormat);
    String bestDateTimePattern;
    dateFormat->GetBestDateTimePattern(mCurrentLocale, String("hm"), &bestDateTimePattern);
    return bestDateTimePattern.StartWith("a");
}

void TimePickerClockDelegate::SetDividerText()
{
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
    AutoPtr<IInputMethodManager> inputMethodManager = CInputMethodManager::PeekInstance();
    if (inputMethodManager != NULL) {
        Boolean isHourSpinnerActive = FALSE;
        inputMethodManager->IsActive(IView::Probe(mHourSpinnerInput), &isHourSpinnerActive);

        Boolean isMinuteSpinnerActive = FALSE;
        inputMethodManager->IsActive(IView::Probe(mMinuteSpinnerInput), &isMinuteSpinnerActive);

        Boolean isAmpmSpinnerActive = FALSE;
        inputMethodManager->IsActive(IView::Probe(mAmPmSpinnerInput), &isAmpmSpinnerActive);

        AutoPtr<IBinder> binder;
        IView::Probe(mDelegator)->GetWindowToken((IBinder**)&binder);
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
            ITextView::Probe(mAmPmButton)->SetText(charSequence);
            IView::Probe(mAmPmButton)->SetVisibility(IView::VISIBLE);
        }
    }
    IView::Probe(mDelegator)->SendAccessibilityEvent(IAccessibilityEvent::TYPE_VIEW_SELECTED);
}

void TimePickerClockDelegate::OnTimeChanged()
{
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
    IView* viewMinuteSpinner = IView::Probe(mMinuteSpinner);
    TrySetContentDescription(viewMinuteSpinner, R::id::increment,
            R::string::time_picker_increment_minute_button);
    TrySetContentDescription(viewMinuteSpinner, R::id::decrement,
            R::string::time_picker_decrement_minute_button);
    // Hour
    IView* viewHourSpinner = IView::Probe(mHourSpinner);
    TrySetContentDescription(viewHourSpinner, R::id::increment,
            R::string::time_picker_increment_hour_button);
    TrySetContentDescription(viewHourSpinner, R::id::decrement,
            R::string::time_picker_decrement_hour_button);
    // AM/PM
    if (mAmPmSpinner != NULL) {
        IView* viewAmPmSpinner = IView::Probe(mAmPmSpinner);
        TrySetContentDescription(viewAmPmSpinner, R::id::increment,
                R::string::time_picker_increment_set_pm_button);
        TrySetContentDescription(viewAmPmSpinner, R::id::decrement,
                R::string::time_picker_decrement_set_am_button);
    }
}

void TimePickerClockDelegate::TrySetContentDescription(
    /* [in] */ IView* root,
    /* [in] */ Int32 viewId,
    /* [in] */ Int32 contDescResId)
{
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
