
#include "elastos/droid/widget/TimePickerSpinnerDelegate.h"
#include "elastos/droid/widget/TimePickerClockDelegate.h"
#include "elastos/droid/widget/CTimePickerSpinnerDelegateSavedState.h"
#include "elastos/droid/widget/CTextView.h"
#include "elastos/droid/content/res/CColorStateList.h"
#include "elastos/droid/text/CTextUtils.h"
#include "elastos/droid/text/format/CDateFormat.h"
#include "elastos/droid/text/format/CDateUtils.h"
#include "elastos/droid/view/CKeyCharacterMapHelper.h"
#include "elastos/droid/view/CViewGroupLayoutParams.h"
#include "elastos/droid/view/View.h"
#include "elastos/droid/R.h"
#include <elastos/core/CoreUtils.h>
#include <elastos/core/Math.h>
#include <elastos/core/StringBuilder.h>
#include <elastos/core/StringUtils.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Content::Res::CColorStateList;
using Elastos::Droid::Text::CTextUtils;
using Elastos::Droid::Text::Format::CDateFormat;
using Elastos::Droid::Text::Format::CDateUtils;
using Elastos::Droid::Text::Format::IDateUtils;
using Elastos::Droid::Text::ITextUtils;
using Elastos::Droid::View::Accessibility::IAccessibilityRecord;
using Elastos::Droid::View::CKeyCharacterMapHelper;
using Elastos::Droid::View::CViewGroupLayoutParams;
using Elastos::Droid::View::EIID_IViewOnClickListener;
using Elastos::Droid::View::EIID_IViewOnFocusChangeListener;
using Elastos::Droid::View::EIID_IViewOnKeyListener;
using Elastos::Droid::View::IHapticFeedbackConstants;
using Elastos::Droid::View::IKeyCharacterMap;
using Elastos::Droid::View::IKeyCharacterMapHelper;
using Elastos::Droid::View::ILayoutInflater;
using Elastos::Droid::View::IViewGroup;
using Elastos::Droid::View::IViewGroupLayoutParams;
using Elastos::Droid::View::View;
using Elastos::Droid::Widget::CTextView;
using Elastos::Droid::Widget::EIID_IOnValueSelectedListener;
using Elastos::Droid::Widget::EIID_ITimePickerAbstractTimePickerDelegate;
using Elastos::Core::CoreUtils;
using Elastos::Core::StringBuilder;
using Elastos::Core::StringUtils;
using Elastos::Utility::CArrayList;
using Elastos::Utility::CCalendarHelper;
using Elastos::Utility::ICalendarHelper;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Widget {

using Elastos::Droid::Text::Format::IDateFormat;

//=====================================================================
//      TimePickerSpinnerDelegate::InnerListener
//=====================================================================
CAR_INTERFACE_IMPL(TimePickerSpinnerDelegate::InnerListener, Object, IOnValueSelectedListener)

TimePickerSpinnerDelegate::InnerListener::InnerListener(
    /* [in] */ TimePickerSpinnerDelegate* owner)
    : mOwner(owner)
{
}

ECode TimePickerSpinnerDelegate::InnerListener::OnValueSelected(
    /* [in] */ Int32 pickerIndex,
    /* [in] */ Int32 newValue,
    /* [in] */ Boolean autoAdvance)
{
    return mOwner->OnValueSelected(pickerIndex, newValue, autoAdvance);
}


//=====================================================================
//                TimePickerSpinnerDelegate::TimePickerSpinnerDelegateSavedState
//=====================================================================
CAR_INTERFACE_IMPL(TimePickerSpinnerDelegate::TimePickerSpinnerDelegateSavedState, Elastos::Droid::View::View::BaseSavedState, ITimePickerSpinnerDelegateSavedState)

TimePickerSpinnerDelegate::TimePickerSpinnerDelegateSavedState::TimePickerSpinnerDelegateSavedState()
    : mHour(0)
    , mMinute(0)
    , mIs24HourMode(FALSE)
    , mInKbMode(FALSE)
    , mCurrentItemShowing(0)
{}

TimePickerSpinnerDelegate::TimePickerSpinnerDelegateSavedState::~TimePickerSpinnerDelegateSavedState()
{}

ECode TimePickerSpinnerDelegate::TimePickerSpinnerDelegateSavedState::constructor()
{
    return Elastos::Droid::View::View::BaseSavedState::constructor();
}

ECode TimePickerSpinnerDelegate::TimePickerSpinnerDelegateSavedState::constructor(
    /* [in] */ IParcelable* superState,
    /* [in] */ Int32 hour,
    /* [in] */ Int32 minute,
    /* [in] */ Boolean is24HourMode,
    /* [in] */ Boolean isKbMode,
    /* [in] */ IArrayList* typedTimes,
    /* [in] */ Int32 currentItemShowing)
{
    FAIL_RETURN(Elastos::Droid::View::View::BaseSavedState::constructor(superState))

    mHour = hour;
    mMinute = minute;
    mIs24HourMode = is24HourMode;
    mInKbMode = isKbMode;
    mTypedTimes = typedTimes;
    mCurrentItemShowing = currentItemShowing;
    return NOERROR;
}

Int32 TimePickerSpinnerDelegate::TimePickerSpinnerDelegateSavedState::GetHour()
{
    return mHour;
}

Int32 TimePickerSpinnerDelegate::TimePickerSpinnerDelegateSavedState::GetMinute()
{
    return mMinute;
}

Boolean TimePickerSpinnerDelegate::TimePickerSpinnerDelegateSavedState::Is24HourMode()
{
    return mIs24HourMode;
}

Boolean TimePickerSpinnerDelegate::TimePickerSpinnerDelegateSavedState::InKbMode()
{
    return mInKbMode;
}

AutoPtr<IArrayList> TimePickerSpinnerDelegate::TimePickerSpinnerDelegateSavedState::GetTypesTimes()
{
    return mTypedTimes;
}

Int32 TimePickerSpinnerDelegate::TimePickerSpinnerDelegateSavedState::GetCurrentItemShowing()
{
    return mCurrentItemShowing;
}

ECode TimePickerSpinnerDelegate::TimePickerSpinnerDelegateSavedState::WriteToParcel(
    /* [in] */ IParcel* dest)
{
    VALIDATE_NOT_NULL(dest);

    FAIL_RETURN(Elastos::Droid::View::View::BaseSavedState::WriteToParcel(dest));
    dest->WriteInt32(mHour);
    dest->WriteInt32(mMinute);
    dest->WriteBoolean(mIs24HourMode);
    dest->WriteBoolean(mInKbMode);
    Int32 size;
    mTypedTimes->GetSize(&size);
    dest->WriteInt32(size);
    for (Int32 i = 0; i < size; i++) {
        AutoPtr<IInterface> obj;
        mTypedTimes->Get(i, (IInterface**)&obj);
        dest->WriteInterfacePtr(obj);
    }

    dest->WriteInt32(mCurrentItemShowing);
    return NOERROR;
}

ECode TimePickerSpinnerDelegate::TimePickerSpinnerDelegateSavedState::ReadFromParcel(
    /* [in] */ IParcel* source)
{
    VALIDATE_NOT_NULL(source);
    FAIL_RETURN(Elastos::Droid::View::View::BaseSavedState::ReadFromParcel(source));
    source->ReadInt32(&mHour);
    source->ReadInt32(&mMinute);
    source->ReadBoolean(&mIs24HourMode);
    source->ReadBoolean(&mInKbMode);
    Int32 size;
    source->ReadInt32(&size);
    while (size > 0) {
        AutoPtr<IInterface> obj;
        source->ReadInterfacePtr((Handle32*)&obj);
        mTypedTimes->Add(obj);
        size--;
    }

    source->ReadInt32(&mCurrentItemShowing);

    return NOERROR;
}

ECode TimePickerSpinnerDelegate::TimePickerSpinnerDelegateSavedState::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str);
    StringBuilder buider;
    buider += "TimePickerSpinnerDelegate.SavedState{";

    buider += " mHour=";
    buider += mHour;
    buider += " mMinute=";
    buider += mMinute;
    buider += " mIs24HourMode=";
    buider += mIs24HourMode;
    buider += " mInKbMode=";
    buider += mInKbMode;
    buider += " mTypedTimes=";
    buider += mTypedTimes;
    buider += " mCurrentItemShowing=";
    buider += mCurrentItemShowing;
    buider += "}";

    *str = buider.ToString();
    return NOERROR;
}

//=====================================================================
//      TimePickerSpinnerDelegate::InnerViewHourOnClickListener
//=====================================================================
CAR_INTERFACE_IMPL(TimePickerSpinnerDelegate::InnerViewHourOnClickListener, Object, IViewOnClickListener)

TimePickerSpinnerDelegate::InnerViewHourOnClickListener::InnerViewHourOnClickListener(
    /* [in] */ TimePickerSpinnerDelegate* owner)
    : mOwner(owner)
{
    assert(mOwner);
}

ECode TimePickerSpinnerDelegate::InnerViewHourOnClickListener::OnClick(
    /* [in] */ IView* v)
{
    VALIDATE_NOT_NULL(v);

    mOwner->SetCurrentItemShowing(HOUR_INDEX, TRUE, TRUE);
    mOwner->TryVibrate();
    return NOERROR;
}

//=====================================================================
//      TimePickerSpinnerDelegate::InnerViewMinuteOnClickListener
//=====================================================================
CAR_INTERFACE_IMPL(TimePickerSpinnerDelegate::InnerViewMinuteOnClickListener, Object, IViewOnClickListener)

TimePickerSpinnerDelegate::InnerViewMinuteOnClickListener::InnerViewMinuteOnClickListener(
    /* [in] */ TimePickerSpinnerDelegate* owner)
    : mOwner(owner)
{
    assert(mOwner);
}

ECode TimePickerSpinnerDelegate::InnerViewMinuteOnClickListener::OnClick(
    /* [in] */ IView* v)
{
    VALIDATE_NOT_NULL(v);

    mOwner->SetCurrentItemShowing(MINUTE_INDEX, TRUE, TRUE);
    mOwner->TryVibrate();
    return NOERROR;
}

//=====================================================================
//   TimePickerSpinnerDelegate::InnerViewAmPmDisplayOnClickListener
//=====================================================================
CAR_INTERFACE_IMPL(TimePickerSpinnerDelegate::InnerViewAmPmDisplayOnClickListener, Object, IViewOnClickListener)

TimePickerSpinnerDelegate::InnerViewAmPmDisplayOnClickListener::InnerViewAmPmDisplayOnClickListener(
    /* [in] */ TimePickerSpinnerDelegate* owner)
    : mOwner(owner)
{
    assert(mOwner);
}

ECode TimePickerSpinnerDelegate::InnerViewAmPmDisplayOnClickListener::OnClick(
    /* [in] */ IView* v)
{
    VALIDATE_NOT_NULL(v);

    mOwner->TryVibrate();
    Int32 amOrPm = 0;
    mOwner->mRadialTimePickerView->GetAmOrPm(&amOrPm);
    if (amOrPm == AM) {
        amOrPm = PM;
    }
    else if (amOrPm == PM){
        amOrPm = AM;
    }

    mOwner->UpdateAmPmDisplay(amOrPm);
    mOwner->mRadialTimePickerView->SetAmOrPm(amOrPm);
    return NOERROR;
}

//=====================================================================
//                   TimePickerSpinnerDelegate::Node
//=====================================================================
TimePickerSpinnerDelegate::Node::Node(
    /* [in] */ ArrayOf<Int32>* legalKeys)
    : mLegalKeys(legalKeys)
{
    CArrayList::New((IArrayList**)&mChildren);
    if (!mLegalKeys) {
        mLegalKeys = ArrayOf<Int32>::Alloc(0);
    }
}

ECode TimePickerSpinnerDelegate::Node::AddChild(
    /* [in] */ Node* child)
{
    VALIDATE_NOT_NULL(child);

    mChildren->Add(TO_IINTERFACE(child));
    return NOERROR;
}

ECode TimePickerSpinnerDelegate::Node::ContainsKey(
    /* [in] */ Int32 key,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    for (Int32 i = 0; i < mLegalKeys->GetLength(); ++i) {
        if ((*mLegalKeys)[i] == key) {
            *result = TRUE;
            return NOERROR;
        }
    }
    *result = FALSE;
    return NOERROR;
}

ECode TimePickerSpinnerDelegate::Node::CanReach(
    /* [in] */ Int32 key,
    /* [out] */ Node** result)
{
    VALIDATE_NOT_NULL(result);

    if (mChildren == NULL) {
        *result = NULL;
        return NOERROR;
    }

    Int32 size = 0;
    mChildren->GetSize(&size);
    IObject* objTmp = NULL;
    Node* child = NULL;
    Boolean resTmp = FALSE;
    for (Int32 idx = 0; idx < size; ++idx) {
        AutoPtr<IInterface> interfaceTmp;
        mChildren->Get(idx, (IInterface**)&interfaceTmp);
        objTmp = IObject::Probe(interfaceTmp);
        child = (Node*)objTmp;
        child->ContainsKey(key, &resTmp);
        if (resTmp) {
            *result = child;
            REFCOUNT_ADD(*result);
            return NOERROR;
        }
    }
    *result = NULL;
    return NOERROR;
}

//=====================================================================
//          TimePickerSpinnerDelegate::InnerViewOnKeyListener
//=====================================================================
CAR_INTERFACE_IMPL(TimePickerSpinnerDelegate::InnerViewOnKeyListener, Object, IViewOnKeyListener)

TimePickerSpinnerDelegate::InnerViewOnKeyListener::InnerViewOnKeyListener(
    /* [in] */ TimePickerSpinnerDelegate* owner)
    : mOwner(owner)
{
    assert(mOwner);
}

ECode TimePickerSpinnerDelegate::InnerViewOnKeyListener::OnKey(
    /* [in] */ IView* v,
    /* [in] */ Int32 keyCode,
    /* [in] */ IKeyEvent* event,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    Int32 action = 0;
    event->GetAction(&action);
    if (action == IKeyEvent::ACTION_UP) {
        *result = mOwner->ProcessKeyUp(keyCode);
        return NOERROR;
    }
    *result = FALSE;
    return NOERROR;
}

//=====================================================================
//      TimePickerSpinnerDelegate::InnerViewOnFocusChangeListener
//=====================================================================
CAR_INTERFACE_IMPL(TimePickerSpinnerDelegate::InnerViewOnFocusChangeListener, Object, IViewOnFocusChangeListener)

TimePickerSpinnerDelegate::InnerViewOnFocusChangeListener::InnerViewOnFocusChangeListener(
    /* [in] */ TimePickerSpinnerDelegate* owner)
    : mOwner(owner)
{
    assert(mOwner);
}

ECode TimePickerSpinnerDelegate::InnerViewOnFocusChangeListener::OnFocusChange(
    /* [in] */ IView* v,
    /* [in] */ Boolean hasFocus)
{
    VALIDATE_NOT_NULL(v);

    if (!hasFocus && mOwner->mInKbMode && mOwner->IsTypedTimeFullyLegal()) {
        mOwner->FinishKbMode();
        if (mOwner->mOnTimeChangedListener != NULL) {
            Int32 hour = 0, minute = 0;
            mOwner->mRadialTimePickerView->GetCurrentHour(&hour);
            mOwner->mRadialTimePickerView->GetCurrentMinute(&minute);
            mOwner->mOnTimeChangedListener->OnTimeChanged(mOwner->mDelegator, hour, minute);
        }
    }
    return NOERROR;
}

//=====================================================================
//                      TimePickerSpinnerDelegate
//=====================================================================
const String TimePickerSpinnerDelegate::TAG("TimePickerDelegate");
const Int32 TimePickerSpinnerDelegate::HOUR_INDEX = 0;
const Int32 TimePickerSpinnerDelegate::MINUTE_INDEX = 1;
const Int32 TimePickerSpinnerDelegate::AMPM_INDEX = 2;
const Int32 TimePickerSpinnerDelegate::ENABLE_PICKER_INDEX = 3;
const Int32 TimePickerSpinnerDelegate::AM = 0;
const Int32 TimePickerSpinnerDelegate::PM = 1;
const Boolean TimePickerSpinnerDelegate::DEFAULT_ENABLED_STATE = TRUE;
const Int32 TimePickerSpinnerDelegate::HOURS_IN_HALF_DAY = 12;

CAR_INTERFACE_IMPL_2(TimePickerSpinnerDelegate, TimePicker::AbstractTimePickerDelegate, ITimePickerSpinnerDelegate, IOnValueSelectedListener)

TimePickerSpinnerDelegate::TimePickerSpinnerDelegate()
    : mIsEnabled(DEFAULT_ENABLED_STATE)
    , mAllowAutoAdvance(FALSE)
    , mInitialHourOfDay(0)
    , mInitialMinute(0)
    , mIs24HourView(FALSE)
    , mPlaceholderText(0)
    , mInKbMode(FALSE)
    , mAmKeyCode(0)
    , mPmKeyCode(0)
{
    CArrayList::New((IArrayList**)&mTypedTimes);
}

ECode TimePickerSpinnerDelegate::constructor(
    /* [in] */ ITimePicker* delegator,
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ Int32 defStyleAttr,
    /* [in] */ Int32 defStyleRes)
{
    TimePicker::AbstractTimePickerDelegate::constructor(delegator, context);

    // process style attributes
    AutoPtr<ITypedArray> a;
    AutoPtr< ArrayOf<Int32> > styleAttrs = TO_ATTRS_ARRAYOF(R::styleable::TimePicker);
    mContext->ObtainStyledAttributes(attrs, styleAttrs, defStyleAttr, defStyleRes, (ITypedArray**)&a);

    AutoPtr<IInterface> interfaceTmp;
    mContext->GetSystemService(IContext::LAYOUT_INFLATER_SERVICE, (IInterface**)&interfaceTmp);
    ILayoutInflater* inflater = ILayoutInflater::Probe(interfaceTmp);

    AutoPtr<IResources> res;
    mContext->GetResources((IResources**)&res);

    res->GetString(R::string::hour_picker_description, &mHourPickerDescription);
    res->GetString(R::string::select_hours, &mSelectHours);
    res->GetString(R::string::minute_picker_description, &mMinutePickerDescription);
    res->GetString(R::string::select_minutes, &mSelectMinutes);

    AutoPtr< ArrayOf<String> > amPmStrings = TimePickerClockDelegate::GetAmPmStrings(context);
    mAmText = (*amPmStrings)[0];
    mPmText = (*amPmStrings)[1];

    Int32 layoutResourceId = 0;
    a->GetResourceId(R::styleable::TimePicker_internalLayout, R::layout::time_picker_holo, &layoutResourceId);
    AutoPtr<IView> mainView;
    inflater->Inflate(layoutResourceId, NULL, (IView**)&mainView);
    IViewGroup::Probe(mDelegator)->AddView(mainView);

    AutoPtr<IView> viewTmp;
    mainView->FindViewById(R::id::hours, (IView**)&viewTmp);
    mHourView = ITextView::Probe(viewTmp);

    AutoPtr<IView> viewTmp1;
    mainView->FindViewById(R::id::separator, (IView**)&viewTmp1);
    mSeparatorView = ITextView::Probe(viewTmp1);

    AutoPtr<IView> viewTmp2;
    mainView->FindViewById(R::id::minutes, (IView**)&viewTmp2);
    mMinuteView = ITextView::Probe(viewTmp2);

    AutoPtr<IView> viewTmp3;
    mainView->FindViewById(R::id::ampm_label, (IView**)&viewTmp3);
    mAmPmTextView = ITextView::Probe(viewTmp3);

    // Set up text appearances from style.
    Int32 headerTimeTextAppearance = 0;
    a->GetResourceId(R::styleable::TimePicker_headerTimeTextAppearance, 0, &headerTimeTextAppearance);
    if (headerTimeTextAppearance != 0) {
        mHourView->SetTextAppearance(context, headerTimeTextAppearance);
        mSeparatorView->SetTextAppearance(context, headerTimeTextAppearance);
        mMinuteView->SetTextAppearance(context, headerTimeTextAppearance);
    }

    Int32 color = 0;
    res->GetColor(R::color::timepicker_default_selector_color_material, &color);
    Int32 headerSelectedTextColor = 0;
    a->GetColor(R::styleable::TimePicker_headerSelectedTextColor, color, &headerSelectedTextColor);

    AutoPtr<IColorStateList> colorStateList;
    mHourView->GetTextColors((IColorStateList**)&colorStateList);
    mHourView->SetTextColor(CColorStateList::AddFirstIfMissing(colorStateList, R::attr::state_selected, headerSelectedTextColor));

    AutoPtr<IColorStateList> colorStateList1;
    mMinuteView->GetTextColors((IColorStateList**)&colorStateList1);
    mMinuteView->SetTextColor(CColorStateList::AddFirstIfMissing(colorStateList1, R::attr::state_selected, headerSelectedTextColor));

    Int32 headerAmPmTextAppearance = 0;
    a->GetResourceId(R::styleable::TimePicker_headerAmPmTextAppearance, 0, &headerAmPmTextAppearance);
    if (headerAmPmTextAppearance != 0) {
        mAmPmTextView->SetTextAppearance(context, headerAmPmTextAppearance);
    }

    AutoPtr<IView> viewTmp4;
    mainView->FindViewById(R::id::time_header, (IView**)&viewTmp4);
    mHeaderView = IView::Probe(viewTmp4);

    AutoPtr<IDrawable> drawable;
    a->GetDrawable(R::styleable::TimePicker_headerBackground, (IDrawable**)&drawable);
    mHeaderView->SetBackground(drawable);
    a->Recycle();

    AutoPtr<IView> viewTmp5;
    mainView->FindViewById(R::id::radial_picker, (IView**)&viewTmp5);
    mRadialTimePickerView = IRadialTimePickerView::Probe(viewTmp5);

    SetupListeners();
    mAllowAutoAdvance = TRUE;

    // Set up for keyboard mode.
    res->GetString(R::string::time_placeholder, &mDoublePlaceholderText);
    res->GetString(R::string::deleted_key, &mDeletedKeyFormat);
    mPlaceholderText = mDoublePlaceholderText.GetChar(0);
    mAmKeyCode = mPmKeyCode = -1;
    GenerateLegalTimesTree();

    // Initialize with current time
    AutoPtr<ICalendarHelper> helper;
    CCalendarHelper::AcquireSingleton((ICalendarHelper**)&helper);
    AutoPtr<ICalendar> calendar;
    helper->GetInstance(mCurrentLocale, (ICalendar**)&calendar);

    Int32 currentHour = 0, currentMinute = 0;
    calendar->Get(ICalendar::HOUR_OF_DAY, &currentHour);
    calendar->Get(ICalendar::MINUTE, &currentMinute);
    Initialize(currentHour, currentMinute, FALSE /* 12h */, HOUR_INDEX);
    return NOERROR;
}

ECode TimePickerSpinnerDelegate::SetCurrentHour(
    /* [in] */ Int32 currentHour)
{
    if (mInitialHourOfDay == currentHour) {
        return NOERROR;
    }
    mInitialHourOfDay = currentHour;
    UpdateHeaderHour(currentHour, TRUE);
    UpdateHeaderAmPm();
    mRadialTimePickerView->SetCurrentHour(currentHour);
    mRadialTimePickerView->SetAmOrPm(mInitialHourOfDay < 12 ? AM : PM);
    IView::Probe(mDelegator)->Invalidate();
    OnTimeChanged();
    return NOERROR;
}

ECode TimePickerSpinnerDelegate::GetCurrentHour(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);

    Int32 currentHour = 0;
    mRadialTimePickerView->GetCurrentHour(&currentHour);
    if (mIs24HourView) {
        *result = currentHour;
        return NOERROR;
    }
    else {
        Int32 intTmp = 0;
        mRadialTimePickerView->GetAmOrPm(&intTmp);
        switch(intTmp) {
            case PM:
                *result = (currentHour % HOURS_IN_HALF_DAY) + HOURS_IN_HALF_DAY;
                return NOERROR;
            case AM:
            default:
                *result = currentHour % HOURS_IN_HALF_DAY;
                return NOERROR;
        }
    }
}

ECode TimePickerSpinnerDelegate::SetCurrentMinute(
    /* [in] */ Int32 currentMinute)
{
    if (mInitialMinute == currentMinute) {
        return NOERROR;
    }
    mInitialMinute = currentMinute;
    UpdateHeaderMinute(currentMinute);
    mRadialTimePickerView->SetCurrentMinute(currentMinute);
    IView::Probe(mDelegator)->Invalidate();
    OnTimeChanged();
    return NOERROR;
}

ECode TimePickerSpinnerDelegate::GetCurrentMinute(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);

    mRadialTimePickerView->GetCurrentMinute(result);
    return NOERROR;
}

ECode TimePickerSpinnerDelegate::SetIs24HourView(
    /* [in] */ Boolean is24HourView)
{
    if (is24HourView == mIs24HourView) {
        return NOERROR;
    }
    mIs24HourView = is24HourView;
    GenerateLegalTimesTree();
    Int32 hour = 0;
    mRadialTimePickerView->GetCurrentHour(&hour);
    mInitialHourOfDay = hour;
    UpdateHeaderHour(hour, FALSE /* no accessibility announce */);
    UpdateHeaderAmPm();
    Int32 radialPicker = 0;
    mRadialTimePickerView->GetCurrentItemShowing(&radialPicker);
    UpdateRadialPicker(radialPicker);
    IView::Probe(mDelegator)->Invalidate();
    return NOERROR;
}

ECode TimePickerSpinnerDelegate::Is24HourView(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    *result = mIs24HourView;
    return NOERROR;
}

ECode TimePickerSpinnerDelegate::SetOnTimeChangedListener(
    /* [in] */ ITimePickerOnTimeChangedListener* callback)
{
    VALIDATE_NOT_NULL(callback);

    mOnTimeChangedListener = callback;
    return NOERROR;
}

ECode TimePickerSpinnerDelegate::SetEnabled(
    /* [in] */ Boolean enabled)
{
    IView::Probe(mHourView)->SetEnabled(enabled);
    IView::Probe(mMinuteView)->SetEnabled(enabled);
    IView::Probe(mAmPmTextView)->SetEnabled(enabled);
    IView::Probe(mRadialTimePickerView)->SetEnabled(enabled);
    mIsEnabled = enabled;
    return NOERROR;
}

ECode TimePickerSpinnerDelegate::IsEnabled(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    *result = mIsEnabled;
    return NOERROR;
}

ECode TimePickerSpinnerDelegate::GetBaseline(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    // does not support baseline alignment
    *result = -1;
    return NOERROR;
}

ECode TimePickerSpinnerDelegate::OnConfigurationChanged(
    /* [in] */ IConfiguration* newConfig)
{
    VALIDATE_NOT_NULL(newConfig);

    Int32 item = 0;
    mRadialTimePickerView->GetCurrentItemShowing(&item);
    UpdateUI(item);
    return NOERROR;
}

ECode TimePickerSpinnerDelegate::OnSaveInstanceState(
    /* [in] */ IParcelable* superState,
    /* [out] */ IParcelable** result)
{
    VALIDATE_NOT_NULL(superState);
    VALIDATE_NOT_NULL(result);

    Int32 hour = 0, minute = 0;
    GetCurrentHour(&hour);
    GetCurrentMinute(&minute);
    Boolean is24Hour = FALSE;
    Is24HourView(&is24Hour);
    Boolean isInKb = InKbMode();

    return CTimePickerSpinnerDelegateSavedState::New(
            superState, hour, minute, is24Hour, isInKb, GetTypedTimes(),
            GetCurrentItemShowing(), result);
}

ECode TimePickerSpinnerDelegate::OnRestoreInstanceState(
    /* [in] */ IParcelable* state)
{
    VALIDATE_NOT_NULL(state);

    AutoPtr<CTimePickerSpinnerDelegateSavedState> ss =
            (CTimePickerSpinnerDelegateSavedState*)ITimePickerSpinnerDelegateSavedState::Probe(state);
    if (!ss) return NOERROR;

    SetInKbMode(ss->InKbMode());
    SetTypedTimes(ss->GetTypesTimes());
    Initialize(ss->GetHour(), ss->GetMinute(), ss->Is24HourMode(), ss->GetCurrentItemShowing());
    IView::Probe(mRadialTimePickerView)->Invalidate();
    if (mInKbMode) {
        TryStartingKbMode(-1);
        IView::Probe(mHourView)->Invalidate();
    }
    return NOERROR;
}

ECode TimePickerSpinnerDelegate::SetCurrentLocale(
    /* [in] */ ILocale* locale)
{
    VALIDATE_NOT_NULL(locale);

    TimePicker::AbstractTimePickerDelegate::SetCurrentLocale(locale);
    AutoPtr<ICalendarHelper> helper;
    CCalendarHelper::AcquireSingleton((ICalendarHelper**)&helper);
    helper->GetInstance(locale, (ICalendar**)&mTempCalendar);
    return NOERROR;
}

ECode TimePickerSpinnerDelegate::DispatchPopulateAccessibilityEvent(
    /* [in] */ IAccessibilityEvent* event,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(event);
    VALIDATE_NOT_NULL(result);

    OnPopulateAccessibilityEvent(event);
    *result = TRUE;
    return NOERROR;
}

ECode TimePickerSpinnerDelegate::OnPopulateAccessibilityEvent(
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
    Int64 millis = 0;
    mTempCalendar->GetTimeInMillis(&millis);
    String selectedDate;
    dateUtils->FormatDateTime(mContext, millis, flags, &selectedDate);

    AutoPtr<IList> text;
    IAccessibilityRecord::Probe(event)->GetText((IList**)&text);

    text->Add(CoreUtils::Convert(selectedDate));
    return NOERROR;
}

ECode TimePickerSpinnerDelegate::OnInitializeAccessibilityEvent(
    /* [in] */ IAccessibilityEvent* event)
{
    VALIDATE_NOT_NULL(event);
    IAccessibilityRecord::Probe(event)->SetClassName(CoreUtils::Convert("CTimePicker"));
    return NOERROR;
}

ECode TimePickerSpinnerDelegate::OnInitializeAccessibilityNodeInfo(
    /* [in] */ IAccessibilityNodeInfo* info)
{
    VALIDATE_NOT_NULL(info);
    info->SetClassName(CoreUtils::Convert("CTimePicker"));
    return NOERROR;
}

ECode TimePickerSpinnerDelegate::OnValueSelected(
    /* [in] */ Int32 pickerIndex,
    /* [in] */ Int32 newValue,
    /* [in] */ Boolean autoAdvance)
{
    if (pickerIndex == HOUR_INDEX) {
        UpdateHeaderHour(newValue, FALSE);
        String announcement("");
        announcement.AppendFormat("%d", newValue);
        if (mAllowAutoAdvance && autoAdvance) {
            SetCurrentItemShowing(MINUTE_INDEX, TRUE, FALSE);
            announcement += String(". ") + mSelectMinutes;
        }
        else {
            StringBuilder buider("");
            buider += mHourPickerDescription;
            buider += ": ";
            buider += newValue;
            IView::Probe(mRadialTimePickerView)->SetContentDescription(buider.ToCharSequence());
        }

        IView::Probe(mRadialTimePickerView)->AnnounceForAccessibility(CoreUtils::Convert(announcement));
    }
    else if (pickerIndex == MINUTE_INDEX){
        UpdateHeaderMinute(newValue);
        StringBuilder buider("");
        buider += mMinutePickerDescription;
        buider += ": ";
        buider += newValue;
        IView::Probe(mRadialTimePickerView)->SetContentDescription(buider.ToCharSequence());
    }
    else if (pickerIndex == AMPM_INDEX) {
        UpdateAmPmDisplay(newValue);
    }
    else if (pickerIndex == ENABLE_PICKER_INDEX) {
        if (!IsTypedTimeFullyLegal()) {
            mTypedTimes->Clear();
        }
        FinishKbMode();
    }
    return NOERROR;
}

void TimePickerSpinnerDelegate::Initialize(
    /* [in] */ Int32 hourOfDay,
    /* [in] */ Int32 minute,
    /* [in] */ Boolean is24HourView,
    /* [in] */ Int32 index)
{
    mInitialHourOfDay = hourOfDay;
    mInitialMinute = minute;
    mIs24HourView = is24HourView;
    mInKbMode = FALSE;
    UpdateUI(index);
}

void TimePickerSpinnerDelegate::SetupListeners()
{
    mHeaderView->SetOnKeyListener(mKeyListener);
    mHeaderView->SetOnFocusChangeListener(mFocusListener);
    mHeaderView->SetFocusable(TRUE);

    AutoPtr<InnerListener> listener = new InnerListener(this);
    mRadialTimePickerView->SetOnValueSelectedListener(listener);

    AutoPtr<IViewOnClickListener> hourOnClickListener = new InnerViewHourOnClickListener(this);
    IView::Probe(mHourView)->SetOnClickListener(hourOnClickListener);

    AutoPtr<IViewOnClickListener> minuteOnClickListener = new InnerViewMinuteOnClickListener(this);
    IView::Probe(mMinuteView)->SetOnClickListener(minuteOnClickListener);
}

void TimePickerSpinnerDelegate::UpdateUI(
    /* [in] */ Int32 index)
{
    // Update RadialPicker values
    UpdateRadialPicker(index);
    // Enable or disable the AM/PM view.
    UpdateHeaderAmPm();
    // Update Hour and Minutes
    UpdateHeaderHour(mInitialHourOfDay, TRUE);
    // Update time separator
    UpdateHeaderSeparator();
    // Update Minutes
    UpdateHeaderMinute(mInitialMinute);
    // Invalidate everything
    IView::Probe(mDelegator)->Invalidate();
}

void TimePickerSpinnerDelegate::UpdateRadialPicker(
    /* [in] */ Int32 index)
{
    mRadialTimePickerView->Initialize(mInitialHourOfDay, mInitialMinute, mIs24HourView);
    SetCurrentItemShowing(index, FALSE, TRUE);
}

Int32 TimePickerSpinnerDelegate::ComputeMaxWidthOfNumbers(
    /* [in] */ Int32 max)
{
    AutoPtr<ITextView> tempView;
    CTextView::New(mContext, (ITextView**)&tempView);
    tempView->SetTextAppearance(mContext, R::style::TextAppearance_Material_TimePicker_TimeLabel);

    AutoPtr<IViewGroupLayoutParams> lp;
    CViewGroupLayoutParams::New(IViewGroupLayoutParams::WRAP_CONTENT,
            IViewGroupLayoutParams::WRAP_CONTENT, (IViewGroupLayoutParams**)&lp);
    IView* _tempView = IView::Probe(tempView);
    _tempView->SetLayoutParams(lp);

    Int32 maxWidth = 0;
    String text;
    Int32 measuredWidth = 0;
    for (Int32 minutes = 0; minutes < max; ++minutes) {
        AutoPtr< ArrayOf<IInterface*> > args = ArrayOf<IInterface*>::Alloc(1);
        args->Set(0, CoreUtils::Convert(minutes));
        text = StringUtils::Format(String("%02d"), args);

        tempView->SetText(CoreUtils::Convert(text));
        _tempView->Measure(Elastos::Droid::View::View::MeasureSpec::UNSPECIFIED,
                Elastos::Droid::View::View::MeasureSpec::UNSPECIFIED);

        _tempView->GetMeasuredWidth(&measuredWidth);
        maxWidth = Elastos::Core::Math::Max(maxWidth, measuredWidth);
    }
    return maxWidth;
}

void TimePickerSpinnerDelegate::UpdateHeaderAmPm()
{
    if (mIs24HourView) {
        IView::Probe(mAmPmTextView)->SetVisibility(IView::GONE);
    }
    else {
        IView* amPmTextView = IView::Probe(mAmPmTextView);
        amPmTextView->SetVisibility(IView::VISIBLE);

        AutoPtr<IDateFormat> dateFormat;
        CDateFormat::AcquireSingleton((IDateFormat**)&dateFormat);
        String bestDateTimePattern;
        dateFormat->GetBestDateTimePattern(mCurrentLocale, String("hm"), &bestDateTimePattern);

        Boolean amPmOnLeft = bestDateTimePattern.StartWith("a");

        AutoPtr<ITextUtils> textUtils;
        CTextUtils::AcquireSingleton((ITextUtils**)&textUtils);
        Int32 layoutDirect = 0;
        textUtils->GetLayoutDirectionFromLocale(mCurrentLocale, &layoutDirect);
        if (layoutDirect == IView::LAYOUT_DIRECTION_RTL) {
            amPmOnLeft = !amPmOnLeft;
        }

        AutoPtr<IViewGroupLayoutParams> layoutParamTmp;
        amPmTextView->GetLayoutParams((IViewGroupLayoutParams**)&layoutParamTmp);
        IRelativeLayoutLayoutParams* layoutParamsTmp1 = IRelativeLayoutLayoutParams::Probe(layoutParamTmp);
        IViewGroupMarginLayoutParams* layoutParamsTmp2 = IViewGroupMarginLayoutParams::Probe(layoutParamsTmp1);

        if (amPmOnLeft) {
            layoutParamsTmp2->SetRightMargin(ComputeMaxWidthOfNumbers(12 /* for hours */));
            layoutParamsTmp1->RemoveRule(IRelativeLayout::RIGHT_OF);
            layoutParamsTmp1->AddRule(IRelativeLayout::LEFT_OF, R::id::separator);
        }
        else {
            layoutParamsTmp2->SetLeftMargin(ComputeMaxWidthOfNumbers(60 /* for minutes */));
            layoutParamsTmp1->RemoveRule(IRelativeLayout::LEFT_OF);
            layoutParamsTmp1->AddRule(IRelativeLayout::RIGHT_OF, R::id::separator);
        }

        UpdateAmPmDisplay(mInitialHourOfDay < 12 ? AM : PM);
        AutoPtr<IViewOnClickListener> ampmOnClickListener = new InnerViewAmPmDisplayOnClickListener(this);
        amPmTextView->SetOnClickListener(ampmOnClickListener);
    }
}

void TimePickerSpinnerDelegate::SetInKbMode(
    /* [in] */ Boolean inKbMode)
{
    mInKbMode = inKbMode;
}

Boolean TimePickerSpinnerDelegate::InKbMode()
{
    return mInKbMode;
}

void TimePickerSpinnerDelegate::SetTypedTimes(
    /* [in] */ IArrayList* typeTimes)
{
    mTypedTimes = typeTimes;
}

AutoPtr<IArrayList> TimePickerSpinnerDelegate::GetTypedTimes()
{
    return mTypedTimes;
}

Int32 TimePickerSpinnerDelegate::GetCurrentItemShowing()
{
    Int32 currItemShowing = 0;
    mRadialTimePickerView->GetCurrentItemShowing(&currItemShowing);
    return currItemShowing;
}

void TimePickerSpinnerDelegate::OnTimeChanged()
{
    IView::Probe(mDelegator)->SendAccessibilityEvent(IAccessibilityEvent::TYPE_VIEW_SELECTED);
    if (mOnTimeChangedListener != NULL) {
        Int32 hour = 0, minute = 0;
        GetCurrentHour(&hour);
        GetCurrentMinute(&minute);
        mOnTimeChangedListener->OnTimeChanged(mDelegator, hour, minute);
    }
}

void TimePickerSpinnerDelegate::TryVibrate()
{
    Boolean resTmp;
    IView::Probe(mDelegator)->PerformHapticFeedback(IHapticFeedbackConstants::CLOCK_TICK, &resTmp);
}

void TimePickerSpinnerDelegate::UpdateAmPmDisplay(
    /* [in] */ Int32 amOrPm)
{
    if (amOrPm == AM) {
        AutoPtr<ICharSequence> charSequenceTmp = CoreUtils::Convert(mAmText);
        mAmPmTextView->SetText(charSequenceTmp);
        IView::Probe(mRadialTimePickerView)->AnnounceForAccessibility(charSequenceTmp);
    }
    else if (amOrPm == PM){
        AutoPtr<ICharSequence> charSequenceTmp = CoreUtils::Convert(mPmText);
        mAmPmTextView->SetText(charSequenceTmp);
        IView::Probe(mRadialTimePickerView)->AnnounceForAccessibility(charSequenceTmp);
    }
    else {
        mAmPmTextView->SetText(CoreUtils::Convert(mDoublePlaceholderText));
    }
}

void TimePickerSpinnerDelegate::UpdateHeaderHour(
    /* [in] */ Int32 value,
    /* [in] */ Boolean announce)
{
    AutoPtr<IDateFormat> dateFormat;
    CDateFormat::AcquireSingleton((IDateFormat**)&dateFormat);
    String strIs24Hour = mIs24HourView ? String("Hm") : String("hm");
    String bestDateTimePattern;
    dateFormat->GetBestDateTimePattern(mCurrentLocale, strIs24Hour, &bestDateTimePattern);
    Int32 lengthPattern = bestDateTimePattern.GetLength();
    Boolean hourWithTwoDigit = FALSE;
    Char32 hourFormat = '\0';
    Char32 c;
    for (Int32 i = 0; i < lengthPattern; ++i) {
        c = bestDateTimePattern.GetChar(i);
        if (c == 'H' || c == 'h' || c == 'K' || c == 'k') {
            hourFormat = c;
            if (i + 1 < lengthPattern && c == bestDateTimePattern.GetChar(i + 1)) {
                hourWithTwoDigit = TRUE;
            }
            break;
        }
    }

    String format;
    if (hourWithTwoDigit) {
        format = "%02d";
    }
    else {
        format = "%d";
    }

    if (mIs24HourView) {
        // 'k' means 1-24 hour
        if (hourFormat == 'k' && value == 0) {
            value = 24;
        }
    }
    else {
        // 'K' means 0-11 hour
        value = Modulo12(value, hourFormat == 'K');
    }

    AutoPtr< ArrayOf<IInterface*> > args = ArrayOf<IInterface*>::Alloc(1);
    args->Set(0, CoreUtils::Convert(value));
    String textTmp = StringUtils::Format(format, args);
    AutoPtr<ICharSequence> text = CoreUtils::Convert(textTmp);

    mHourView->SetText(text);
    if (announce) {
        IView::Probe(mRadialTimePickerView)->AnnounceForAccessibility(text);
    }
}

Int32 TimePickerSpinnerDelegate::Modulo12(
    /* [in] */ Int32 n,
    /* [in] */ Boolean startWithZero)
{
    Int32 value = n % 12;
    if (value == 0 && !startWithZero) {
        value = 12;
    }
    return value;
}

void TimePickerSpinnerDelegate::UpdateHeaderSeparator()
{
    AutoPtr<IDateFormat> dateFormat;
    CDateFormat::AcquireSingleton((IDateFormat**)&dateFormat);
    String strIs24Hour = mIs24HourView ? String("Hm") : String("hm");
    String bestDateTimePattern;
    dateFormat->GetBestDateTimePattern(mCurrentLocale, strIs24Hour, &bestDateTimePattern);

    String separatorText;
    // See http://www.unicode.org/reports/tr35/tr35-dates.html for hour formats
    Char32 hourFormatsTmp[] = {'H', 'h', 'K', 'k'};
    AutoPtr< ArrayOf<Char32> > hourFormats = ArrayOf<Char32>::Alloc(static_cast<Char32*>(hourFormatsTmp), 4);
    Int32 hIndex = LastIndexOfAny(bestDateTimePattern, hourFormats);
    if (hIndex == -1) {
        // Default case
        separatorText = ":";
    }
    else {
        separatorText = "";
        separatorText.Append(bestDateTimePattern.GetChar(hIndex + 1));
    }
    mSeparatorView->SetText(CoreUtils::Convert(separatorText));
}

Int32 TimePickerSpinnerDelegate::LastIndexOfAny(
    /* [in] */ const String& str,
    /* [in] */ ArrayOf<Char32>* any)
{
    Int32 lengthAny = any->GetLength();
    if (lengthAny > 0) {
        Char32 c;
        for (Int32 i = str.GetLength() - 1; i >= 0; --i) {
            c = str.GetChar(i);
            for (Int32 j = 0; j < lengthAny; ++j) {
                if (c == (*any)[j]) {
                    return i;
                }
            }
        }
    }
    return -1;
}

void TimePickerSpinnerDelegate::UpdateHeaderMinute(
    /* [in] */ Int32 value)
{
    if (value == 60) {
        value = 0;
    }

    AutoPtr< ArrayOf<IInterface*> > args = ArrayOf<IInterface*>::Alloc(1);
    args->Set(0, CoreUtils::Convert(value));
    String textTmp = StringUtils::Format(mCurrentLocale, String("%02d"), args);
    AutoPtr<ICharSequence> text = CoreUtils::Convert(textTmp);
    IView::Probe(mRadialTimePickerView)->AnnounceForAccessibility(text);
    mMinuteView->SetText(text);
}

void TimePickerSpinnerDelegate::SetCurrentItemShowing(
    /* [in] */ Int32 index,
    /* [in] */ Boolean animateCircle,
    /* [in] */ Boolean announce)
{
    mRadialTimePickerView->SetCurrentItemShowing(index, animateCircle);

    if (index == HOUR_INDEX) {
        Int32 hours = 0;
        mRadialTimePickerView->GetCurrentHour(&hours);
        if (!mIs24HourView) {
            hours = hours % 12;
        }

        StringBuilder buider("");
        buider += mHourPickerDescription;
        buider += ": ";
        buider += hours;
        IView* radialTimePickerView = IView::Probe(mRadialTimePickerView);
        radialTimePickerView->SetContentDescription(buider.ToCharSequence());
        if (announce) {
            radialTimePickerView->AnnounceForAccessibility(CoreUtils::Convert(mSelectHours));
        }
    }
    else {
        Int32 minutes = 0;
        mRadialTimePickerView->GetCurrentMinute(&minutes);
        StringBuilder buider("");
        buider += mMinutePickerDescription;
        buider += ": ";
        buider += minutes;
        IView* radialTimePickerView = IView::Probe(mRadialTimePickerView);
        radialTimePickerView->SetContentDescription(buider.ToCharSequence());
        if (announce) {
            radialTimePickerView->AnnounceForAccessibility(CoreUtils::Convert(mSelectMinutes));
        }
    }

    IView::Probe(mHourView)->SetSelected(index == HOUR_INDEX);
    IView::Probe(mMinuteView)->SetSelected(index == MINUTE_INDEX);
}

Boolean TimePickerSpinnerDelegate::ProcessKeyUp(
    /* [in] */ Int32 keyCode)
{
    if (keyCode == IKeyEvent::KEYCODE_DEL) {
        if (mInKbMode) {
            Boolean isEmpty = FALSE;
            mTypedTimes->IsEmpty(&isEmpty);
            if (!isEmpty) {
                Int32 deleted = DeleteLastTypedKey();
                String deletedKeyStr;
                if (deleted == GetAmOrPmKeyCode(AM)) {
                    deletedKeyStr = mAmText;
                }
                else if (deleted == GetAmOrPmKeyCode(PM)) {
                    deletedKeyStr = mPmText;
                }
                else {
                    AutoPtr< ArrayOf<IInterface*> > args = ArrayOf<IInterface*>::Alloc(1);
                    args->Set(0, CoreUtils::Convert(GetValFromKeyCode(deleted)));
                    deletedKeyStr = StringUtils::Format(String("%d"), args);
                }

                AutoPtr< ArrayOf<IInterface*> > args = ArrayOf<IInterface*>::Alloc(1);
                args->Set(0, CoreUtils::Convert(deletedKeyStr));

                String strTmp = StringUtils::Format(mDeletedKeyFormat, args);
                IView::Probe(mRadialTimePickerView)->AnnounceForAccessibility(CoreUtils::Convert(strTmp));
                UpdateDisplay(TRUE);
            }
        }
    }
    else if (keyCode == IKeyEvent::KEYCODE_0 || keyCode == IKeyEvent::KEYCODE_1
            || keyCode == IKeyEvent::KEYCODE_2 || keyCode == IKeyEvent::KEYCODE_3
            || keyCode == IKeyEvent::KEYCODE_4 || keyCode == IKeyEvent::KEYCODE_5
            || keyCode == IKeyEvent::KEYCODE_6 || keyCode == IKeyEvent::KEYCODE_7
            || keyCode == IKeyEvent::KEYCODE_8 || keyCode == IKeyEvent::KEYCODE_9
            || (!mIs24HourView &&
            (keyCode == GetAmOrPmKeyCode(AM) || keyCode == GetAmOrPmKeyCode(PM)))) {
        if (!mInKbMode) {
            if (mRadialTimePickerView == NULL) {
                // Something's wrong, because time picker should definitely not be null.
                Logger::E(TAG, "Unable to initiate keyboard mode, TimePicker was null.");
                return TRUE;
            }
            mTypedTimes->Clear();
            TryStartingKbMode(keyCode);
            return TRUE;
        }
        // We're already in keyboard mode.
        if (AddKeyIfLegal(keyCode)) {
            UpdateDisplay(FALSE);
        }
        return TRUE;
    }
    return FALSE;
}

void TimePickerSpinnerDelegate::TryStartingKbMode(
    /* [in] */ Int32 keyCode)
{
    if (keyCode == -1 || AddKeyIfLegal(keyCode)) {
        mInKbMode = TRUE;
        OnValidationChanged(FALSE);
        UpdateDisplay(FALSE);
        mRadialTimePickerView->SetInputEnabled(FALSE);
    }
}

Boolean TimePickerSpinnerDelegate::AddKeyIfLegal(
    /* [in] */ Int32 keyCode)
{
    // If we're in 24hour mode, we'll need to check if the input is full. If in AM/PM mode,
    // we'll need to see if AM/PM have been typed.
    Int32 typedTimesSize = 0;
    if ((mIs24HourView && (mTypedTimes->GetSize(&typedTimesSize), typedTimesSize) == 4) || (!mIs24HourView && IsTypedTimeFullyLegal())) {
        return FALSE;
    }

    mTypedTimes->Add(CoreUtils::Convert(keyCode));
    if (!IsTypedTimeLegalSoFar()) {
        DeleteLastTypedKey();
        return FALSE;
    }

    Int32 val = GetValFromKeyCode(keyCode);
    AutoPtr< ArrayOf<IInterface*> > args = ArrayOf<IInterface*>::Alloc(1);
    args->Set(0, CoreUtils::Convert(val));
    String formatStr = StringUtils::Format(String("%d"), args);
    IView::Probe(mRadialTimePickerView)->AnnounceForAccessibility(CoreUtils::Convert(formatStr));
    // Automatically fill in 0's if AM or PM was legally entered.
    if (IsTypedTimeFullyLegal()) {
        if (!mIs24HourView && (mTypedTimes->GetSize(&typedTimesSize), typedTimesSize) <= 3) {
            mTypedTimes->Add(typedTimesSize - 1, CoreUtils::Convert(IKeyEvent::KEYCODE_0));
            mTypedTimes->GetSize(&typedTimesSize);
            mTypedTimes->Add(typedTimesSize - 1, CoreUtils::Convert(IKeyEvent::KEYCODE_0));
        }
        OnValidationChanged(TRUE);
    }
    return TRUE;
}

Boolean TimePickerSpinnerDelegate::IsTypedTimeLegalSoFar()
{
    AutoPtr<Node> node = mLegalTimesTree;
    Int32 size = 0;
    mTypedTimes->GetSize(&size);
    Int32 key = 0;
    for (Int32 idx = 0; idx < size; ++idx) {
        AutoPtr<IInterface> interfaceTmp;
        mTypedTimes->Get(idx, (IInterface**)&interfaceTmp);
        IInteger32* intTmp = IInteger32::Probe(interfaceTmp);
        intTmp->GetValue(&key);
        AutoPtr<Node> nodeTmp;
        node->CanReach(key, (Node**)&nodeTmp);
        node = NULL;
        node = nodeTmp;
        if (node == NULL) {
            return FALSE;
        }
    }
    return TRUE;
}

Boolean TimePickerSpinnerDelegate::IsTypedTimeFullyLegal()
{
    if (mIs24HourView) {
        // For 24-hour mode, the time is legal if the hours and minutes are each legal. Note:
        // getEnteredTime() will ONLY call isTypedTimeFullyLegal() when NOT in 24hour mode.
        AutoPtr< ArrayOf<Int32> > values = GetEnteredTime(NULL);
        return ((*values)[0] >= 0 && (*values)[1] >= 0 && (*values)[1] < 60);
    }
    else {
        // For AM/PM mode, the time is legal if it contains an AM or PM, as those can only be
        // legally added at specific times based on the tree's algorithm.
        Boolean resTmp = FALSE, resTmp1 = FALSE;
        return ((mTypedTimes->Contains(CoreUtils::Convert(GetAmOrPmKeyCode(AM)), &resTmp), resTmp) ||
                (mTypedTimes->Contains(CoreUtils::Convert(GetAmOrPmKeyCode(PM)), &resTmp1), resTmp1));
    }
}

Int32 TimePickerSpinnerDelegate::DeleteLastTypedKey()
{
    Int32 size = 0;
    mTypedTimes->GetSize(&size);
    AutoPtr<IInterface> obj;
    mTypedTimes->Remove((size - 1), (IInterface**)&obj);
    Int32 deleted;
    IInteger32::Probe(obj)->GetValue(&deleted);
    if (!IsTypedTimeFullyLegal()) {
        OnValidationChanged(FALSE);
    }
    return deleted;
}

void TimePickerSpinnerDelegate::FinishKbMode()
{
    mInKbMode = FALSE;
    Boolean isEmpty = FALSE;
    mTypedTimes->IsEmpty(&isEmpty);
    if (!isEmpty) {
        AutoPtr< ArrayOf<Int32> > values = GetEnteredTime(NULL);
        mRadialTimePickerView->SetCurrentHour((*values)[0]);
        mRadialTimePickerView->SetCurrentMinute((*values)[1]);
        if (!mIs24HourView) {
            mRadialTimePickerView->SetAmOrPm((*values)[2]);
        }
        mTypedTimes->Clear();
    }
    UpdateDisplay(FALSE);
    mRadialTimePickerView->SetInputEnabled(TRUE);
}

void TimePickerSpinnerDelegate::UpdateDisplay(
    /* [in] */ Boolean allowEmptyDisplay)
{
    Boolean isEmpty = FALSE;
    if (!allowEmptyDisplay && (mTypedTimes->IsEmpty(&isEmpty), isEmpty)) {
        Int32 hour = 0;
        mRadialTimePickerView->GetCurrentHour(&hour);
        Int32 minute = 0;
        mRadialTimePickerView->GetCurrentMinute(&minute);
        UpdateHeaderHour(hour, TRUE);
        UpdateHeaderMinute(minute);
        if (!mIs24HourView) {
            UpdateAmPmDisplay(hour < 12 ? AM : PM);
        }

        Int32 currItemShowing = 0;
        mRadialTimePickerView->GetCurrentItemShowing(&currItemShowing);
        SetCurrentItemShowing(currItemShowing, TRUE, TRUE);
        OnValidationChanged(TRUE);
    }
    else {
        Boolean enteredZerosTmp[] = {FALSE, FALSE};
        AutoPtr< ArrayOf<Boolean> > enteredZeros = ArrayOf<Boolean>::Alloc(static_cast<Boolean*>(enteredZerosTmp), 2);
        AutoPtr< ArrayOf<Int32> > values = GetEnteredTime(enteredZeros);
        String hourFormat = (*enteredZeros)[0] ? String("%02d") : String("%2d");
        String minuteFormat = ((*enteredZeros)[1]) ? String("%02d") : String("%2d");

        AutoPtr< ArrayOf<IInterface*> > args = ArrayOf<IInterface*>::Alloc(1);
        String hourStr = mDoublePlaceholderText;
        if ((*values)[0] != -1) {
            args->Set(0, CoreUtils::Convert((*values)[0]));
            String formatStr = StringUtils::Format(hourFormat, args);
            hourStr = formatStr.Replace(Char32(' '), mPlaceholderText);
        }

        String minuteStr = mDoublePlaceholderText;
        if ((*values)[1] != -1) {
            args->Set(0, CoreUtils::Convert((*values)[1]));
            String formatStr = StringUtils::Format(minuteFormat, args);
            minuteStr = formatStr.Replace(Char32(' '), mPlaceholderText);
        }

        mHourView->SetText(CoreUtils::Convert(hourStr));
        IView::Probe(mHourView)->SetSelected(FALSE);
        mMinuteView->SetText(CoreUtils::Convert(minuteStr));
        IView::Probe(mMinuteView)->SetSelected(FALSE);
        if (!mIs24HourView) {
            UpdateAmPmDisplay((*values)[2]);
        }
    }
}

Int32 TimePickerSpinnerDelegate::GetValFromKeyCode(
    /* [in] */ Int32 keyCode)
{
    switch (keyCode) {
        case IKeyEvent::KEYCODE_0:
            return 0;
        case IKeyEvent::KEYCODE_1:
            return 1;
        case IKeyEvent::KEYCODE_2:
            return 2;
        case IKeyEvent::KEYCODE_3:
            return 3;
        case IKeyEvent::KEYCODE_4:
            return 4;
        case IKeyEvent::KEYCODE_5:
            return 5;
        case IKeyEvent::KEYCODE_6:
            return 6;
        case IKeyEvent::KEYCODE_7:
            return 7;
        case IKeyEvent::KEYCODE_8:
            return 8;
        case IKeyEvent::KEYCODE_9:
            return 9;
        default:
            return -1;
    }
}

AutoPtr< ArrayOf<Int32> > TimePickerSpinnerDelegate::GetEnteredTime(
    /* [in] */ ArrayOf<Boolean>* enteredZeros)
{
    Int32 amOrPm = -1;
    Int32 startIndex = 1;
    Int32 typedTimesSize = 0;
    mTypedTimes->GetSize(&typedTimesSize);
    if (!mIs24HourView && IsTypedTimeFullyLegal()) {
        AutoPtr<IInterface> interfaceTmp;
        mTypedTimes->Get(typedTimesSize - 1, (IInterface**)&interfaceTmp);
        Int32 keyCode = 0;
        IInteger32::Probe(interfaceTmp)->GetValue(&keyCode);
        if (keyCode == GetAmOrPmKeyCode(AM)) {
            amOrPm = AM;
        }
        else if (keyCode == GetAmOrPmKeyCode(PM)){
            amOrPm = PM;
        }
        startIndex = 2;
    }

    Int32 minute = -1;
    Int32 hour = -1;
    for (Int32 i = startIndex; i <= typedTimesSize; ++i) {
        AutoPtr<IInterface> interfaceTmp;
        mTypedTimes->Get(typedTimesSize - i, (IInterface**)&interfaceTmp);
        Int32 key = 0;
        IInteger32::Probe(interfaceTmp)->GetValue(&key);
        Int32 val = GetValFromKeyCode(key);

        if (i == startIndex) {
            minute = val;
        }
        else if (i == startIndex+1) {
            minute += 10 * val;
            if (enteredZeros != NULL && val == 0) {
                (*enteredZeros)[1] = TRUE;
            }
        }
        else if (i == startIndex+2) {
            hour = val;
        }
        else if (i == startIndex+3) {
            hour += 10 * val;
            if (enteredZeros != NULL && val == 0) {
                (*enteredZeros)[0] = TRUE;
            }
        }
    }

    AutoPtr< ArrayOf<Int32> > result = ArrayOf<Int32>::Alloc(3);
    result->Set(0, hour);
    result->Set(1, minute);
    result->Set(2, amOrPm);
    return result;
}

Int32 TimePickerSpinnerDelegate::GetAmOrPmKeyCode(
    /* [in] */ Int32 amOrPm)
{
    // Cache the codes.
    if (mAmKeyCode == -1 || mPmKeyCode == -1) {
        // Find the first character in the AM/PM text that is unique.
        AutoPtr<IKeyCharacterMapHelper> helper;
        CKeyCharacterMapHelper::AcquireSingleton((IKeyCharacterMapHelper**)&helper);
        AutoPtr<IKeyCharacterMap> kcm;
        helper->Load(IKeyCharacterMap::VIRTUAL_KEYBOARD, (IKeyCharacterMap**)&kcm);

        Char32 amChar;
        Char32 pmChar;
        Int32 maxLength = Elastos::Core::Math::Max(mAmText.GetLength(), mPmText.GetLength());
        for (Int32 i = 0; i < maxLength; ++i) {
            amChar = mAmText.ToLowerCase(/*mCurrentLocale*/).GetChar(i);
            pmChar = mPmText.ToLowerCase(/*mCurrentLocale*/).GetChar(i);
            if (amChar != pmChar) {
                AutoPtr< ArrayOf<Char32> > args = ArrayOf<Char32>::Alloc(2);
                args->Set(0, amChar);
                args->Set(1, pmChar);
                AutoPtr< ArrayOf<IKeyEvent*> > events;
                kcm->GetEvents(args, (ArrayOf<IKeyEvent*>**)&events);
                // There should be 4 events: a down and up for both AM and PM.
                if (events != NULL && events->GetLength() == 4) {
                    (*events)[0]->GetKeyCode(&mAmKeyCode);
                    (*events)[2]->GetKeyCode(&mPmKeyCode);
                }
                else {
                    Logger::E(TAG, "Unable to find keycodes for AM and PM.");
                }
                break;
            }
        }
    }

    if (amOrPm == AM) {
        return mAmKeyCode;
    }
    else if (amOrPm == PM) {
        return mPmKeyCode;
    }
    return -1;
}

void TimePickerSpinnerDelegate::GenerateLegalTimesTree()
{
    // Create a quick cache of numbers to their keycodes.
    Int32 k0 = IKeyEvent::KEYCODE_0;
    Int32 k1 = IKeyEvent::KEYCODE_1;
    Int32 k2 = IKeyEvent::KEYCODE_2;
    Int32 k3 = IKeyEvent::KEYCODE_3;
    Int32 k4 = IKeyEvent::KEYCODE_4;
    Int32 k5 = IKeyEvent::KEYCODE_5;
    Int32 k6 = IKeyEvent::KEYCODE_6;
    Int32 k7 = IKeyEvent::KEYCODE_7;
    Int32 k8 = IKeyEvent::KEYCODE_8;
    Int32 k9 = IKeyEvent::KEYCODE_9;

    // The root of the tree doesn't contain any numbers.
    mLegalTimesTree = new Node(NULL);
    if (mIs24HourView) {
        // We'll be re-using these nodes, so we'll save them.
        AutoPtr< ArrayOf<Int32> > nodeInitList = ArrayOf<Int32>::Alloc(6);
        nodeInitList->Set(0, k0);
        nodeInitList->Set(1, k1);
        nodeInitList->Set(2, k2);
        nodeInitList->Set(3, k3);
        nodeInitList->Set(4, k4);
        nodeInitList->Set(5, k5);
        AutoPtr<Node> minuteFirstDigit = new Node(nodeInitList);

        AutoPtr< ArrayOf<Int32> > nodeInitList1 = ArrayOf<Int32>::Alloc(10);
        nodeInitList1->Set(0, k0);
        nodeInitList1->Set(1, k1);
        nodeInitList1->Set(2, k2);
        nodeInitList1->Set(3, k3);
        nodeInitList1->Set(4, k4);
        nodeInitList1->Set(5, k5);
        nodeInitList1->Set(6, k6);
        nodeInitList1->Set(7, k7);
        nodeInitList1->Set(8, k8);
        nodeInitList1->Set(9, k9);
        AutoPtr<Node> minuteSecondDigit = new Node(nodeInitList1);
        // The first digit must be followed by the second digit.
        minuteFirstDigit->AddChild(minuteSecondDigit);

        // The first digit may be 0-1.
        AutoPtr< ArrayOf<Int32> > nodeInitList2 = ArrayOf<Int32>::Alloc(2);
        nodeInitList2->Set(0, k0);
        nodeInitList2->Set(1, k1);
        AutoPtr<Node> firstDigit = new Node(nodeInitList2);
        mLegalTimesTree->AddChild(firstDigit);

        // When the first digit is 0-1, the second digit may be 0-5.
        AutoPtr<Node> secondDigit = new Node(nodeInitList);
        firstDigit->AddChild(secondDigit);
        // We may now be followed by the first minute digit. E.g. 00:09, 15:58.
        secondDigit->AddChild(minuteFirstDigit);

        // When the first digit is 0-1, and the second digit is 0-5, the third digit may be 6-9.
        AutoPtr< ArrayOf<Int32> > nodeInitList4 = ArrayOf<Int32>::Alloc(4);
        nodeInitList4->Set(0, k6);
        nodeInitList4->Set(1, k7);
        nodeInitList4->Set(2, k8);
        nodeInitList4->Set(3, k9);
        AutoPtr<Node> thirdDigit = new Node(nodeInitList4);
        // The time must now be finished. E.g. 0:55, 1:08.
        secondDigit->AddChild(thirdDigit);

        // When the first digit is 0-1, the second digit may be 6-9.
        secondDigit = new Node(nodeInitList4);
        firstDigit->AddChild(secondDigit);
        // We must now be followed by the first minute digit. E.g. 06:50, 18:20.
        secondDigit->AddChild(minuteFirstDigit);

        // The first digit may be 2.
        AutoPtr< ArrayOf<Int32> > nodeInitList6 = ArrayOf<Int32>::Alloc(1);
        nodeInitList6->Set(0, k2);
        firstDigit = new Node(nodeInitList6);
        mLegalTimesTree->AddChild(firstDigit);

        // When the first digit is 2, the second digit may be 0-3.
        AutoPtr< ArrayOf<Int32> > nodeInitList7 = ArrayOf<Int32>::Alloc(4);
        nodeInitList7->Set(0, k0);
        nodeInitList7->Set(1, k1);
        nodeInitList7->Set(2, k2);
        nodeInitList7->Set(3, k3);
        secondDigit = new Node(nodeInitList7);
        firstDigit->AddChild(secondDigit);
        // We must now be followed by the first minute digit. E.g. 20:50, 23:09.
        secondDigit->AddChild(minuteFirstDigit);

        // When the first digit is 2, the second digit may be 4-5.
        AutoPtr< ArrayOf<Int32> > nodeInitList8 = ArrayOf<Int32>::Alloc(2);
        nodeInitList8->Set(0, k4);
        nodeInitList8->Set(1, k5);
        secondDigit = new Node(nodeInitList8);
        firstDigit->AddChild(secondDigit);
        // We must now be followd by the last minute digit. E.g. 2:40, 2:53.
        secondDigit->AddChild(minuteSecondDigit);

        // The first digit may be 3-9.
        AutoPtr< ArrayOf<Int32> > nodeInitList9 = ArrayOf<Int32>::Alloc(7);
        nodeInitList9->Set(0, k3);
        nodeInitList9->Set(1, k4);
        nodeInitList9->Set(2, k5);
        nodeInitList9->Set(3, k6);
        nodeInitList9->Set(4, k7);
        nodeInitList9->Set(5, k8);
        nodeInitList9->Set(6, k9);
        firstDigit = new Node(nodeInitList9);
        mLegalTimesTree->AddChild(firstDigit);
        // We must now be followed by the first minute digit. E.g. 3:57, 8:12.
        firstDigit->AddChild(minuteFirstDigit);
    }
    else {
        // We'll need to use the AM/PM node a lot.
        // Set up AM and PM to respond to "a" and "p".
        AutoPtr< ArrayOf<Int32> > nodeInitList = ArrayOf<Int32>::Alloc(2);
        nodeInitList->Set(0, GetAmOrPmKeyCode(AM));
        nodeInitList->Set(1, GetAmOrPmKeyCode(PM));
        AutoPtr<Node> ampm = new Node(nodeInitList);

        // The first hour digit may be 1.
        AutoPtr< ArrayOf<Int32> > nodeInitList1 = ArrayOf<Int32>::Alloc(1);
        nodeInitList1->Set(0, k1);
        AutoPtr<Node> firstDigit = new Node(nodeInitList1);
        mLegalTimesTree->AddChild(firstDigit);
        // We'll allow quick input of on-the-hour times. E.g. 1pm.
        firstDigit->AddChild(ampm);

        // When the first digit is 1, the second digit may be 0-2.
        AutoPtr< ArrayOf<Int32> > nodeInitList2 = ArrayOf<Int32>::Alloc(3);
        nodeInitList2->Set(0, k0);
        nodeInitList2->Set(1, k1);
        nodeInitList2->Set(2, k2);
        AutoPtr<Node> secondDigit = new Node(nodeInitList2);
        firstDigit->AddChild(secondDigit);
        // Also for quick input of on-the-hour times. E.g. 10pm, 12am.
        secondDigit->AddChild(ampm);

        // When the first digit is 1, and the second digit is 0-2, the third digit may be 0-5.
        AutoPtr< ArrayOf<Int32> > nodeInitList3 = ArrayOf<Int32>::Alloc(6);
        nodeInitList3->Set(0, k0);
        nodeInitList3->Set(1, k1);
        nodeInitList3->Set(2, k2);
        nodeInitList3->Set(3, k3);
        nodeInitList3->Set(4, k4);
        nodeInitList3->Set(5, k5);
        AutoPtr<Node> thirdDigit = new Node(nodeInitList3);
        secondDigit->AddChild(thirdDigit);
        // The time may be finished now. E.g. 1:02pm, 1:25am.
        thirdDigit->AddChild(ampm);

        // When the first digit is 1, the second digit is 0-2, and the third digit is 0-5,
        // the fourth digit may be 0-9.
        AutoPtr< ArrayOf<Int32> > nodeInitList4 = ArrayOf<Int32>::Alloc(10);
        nodeInitList4->Set(0, k0);
        nodeInitList4->Set(1, k1);
        nodeInitList4->Set(2, k2);
        nodeInitList4->Set(3, k3);
        nodeInitList4->Set(4, k4);
        nodeInitList4->Set(5, k5);
        nodeInitList4->Set(6, k6);
        nodeInitList4->Set(7, k7);
        nodeInitList4->Set(8, k8);
        nodeInitList4->Set(9, k9);
        AutoPtr<Node> fourthDigit = new Node(nodeInitList4);
        thirdDigit->AddChild(fourthDigit);
        // The time must be finished now. E.g. 10:49am, 12:40pm.
        fourthDigit->AddChild(ampm);

        // When the first digit is 1, and the second digit is 0-2, the third digit may be 6-9.
        AutoPtr< ArrayOf<Int32> > nodeInitList5 = ArrayOf<Int32>::Alloc(4);
        nodeInitList5->Set(0, k6);
        nodeInitList5->Set(1, k7);
        nodeInitList5->Set(2, k8);
        nodeInitList5->Set(3, k9);
        thirdDigit = new Node(nodeInitList5);
        secondDigit->AddChild(thirdDigit);
        // The time must be finished now. E.g. 1:08am, 1:26pm.
        thirdDigit->AddChild(ampm);

        // When the first digit is 1, the second digit may be 3-5.
        AutoPtr< ArrayOf<Int32> > nodeInitList6 = ArrayOf<Int32>::Alloc(3);
        nodeInitList6->Set(0, k3);
        nodeInitList6->Set(1, k4);
        nodeInitList6->Set(2, k5);
        secondDigit = new Node(nodeInitList6);
        firstDigit->AddChild(secondDigit);

        // When the first digit is 1, and the second digit is 3-5, the third digit may be 0-9.
        thirdDigit = new Node(nodeInitList4);
        secondDigit->AddChild(thirdDigit);
        // The time must be finished now. E.g. 1:39am, 1:50pm.
        thirdDigit->AddChild(ampm);

        // The hour digit may be 2-9.
        AutoPtr< ArrayOf<Int32> > nodeInitList8 = ArrayOf<Int32>::Alloc(8);
        nodeInitList8->Set(0, k2);
        nodeInitList8->Set(1, k3);
        nodeInitList8->Set(2, k4);
        nodeInitList8->Set(3, k5);
        nodeInitList8->Set(4, k6);
        nodeInitList8->Set(5, k7);
        nodeInitList8->Set(6, k8);
        nodeInitList8->Set(7, k9);
        firstDigit = new Node(nodeInitList8);
        mLegalTimesTree->AddChild(firstDigit);
        // We'll allow quick input of on-the-hour-times. E.g. 2am, 5pm.
        firstDigit->AddChild(ampm);

        // When the first digit is 2-9, the second digit may be 0-5.
        AutoPtr< ArrayOf<Int32> > nodeInitList9 = ArrayOf<Int32>::Alloc(6);
        nodeInitList9->Set(0, k0);
        nodeInitList9->Set(1, k1);
        nodeInitList9->Set(2, k2);
        nodeInitList9->Set(3, k3);
        nodeInitList9->Set(4, k4);
        nodeInitList9->Set(5, k5);
        secondDigit = new Node(nodeInitList9);
        firstDigit->AddChild(secondDigit);

        // When the first digit is 2-9, and the second digit is 0-5, the third digit may be 0-9.
        thirdDigit = new Node(nodeInitList4);
        secondDigit->AddChild(thirdDigit);
        // The time must be finished now. E.g. 2:57am, 9:30pm.
        thirdDigit->AddChild(ampm);
    }
}

} // namespace Widget
} // namespace Droid
} // namespace Elastos
