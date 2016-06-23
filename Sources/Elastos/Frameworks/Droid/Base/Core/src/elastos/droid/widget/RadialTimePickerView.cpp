
#include "Elastos.Droid.Animation.h"
#include "Elastos.Droid.Widget.h"
#include "elastos/droid/widget/RadialTimePickerView.h"
#include "elastos/droid/widget/CTextView.h"
#include "elastos/droid/widget/TimePickerClockDelegate.h"
#include "elastos/droid/animation/CAnimatorSet.h"
#include "elastos/droid/animation/ObjectAnimator.h"
#include "elastos/droid/animation/Keyframe.h"
#include "elastos/droid/animation/PropertyValuesHolder.h"
#include "elastos/droid/content/res/CColorStateList.h"
#include "elastos/droid/graphics/Color.h"
#include "elastos/droid/graphics/CPaint.h"
#include "elastos/droid/graphics/CRectF.h"
#include "elastos/droid/graphics/Typeface.h"
#include "elastos/droid/R.h"
#include "elastos/droid/text/format/CTime.h"
#include "elastos/droid/text/format/DateUtils.h"
#include "elastos/droid/view/CViewGroupLayoutParams.h"
#include "elastos/droid/utility/CTypedValue.h"
#include <elastos/core/StringUtils.h>
#include <elastos/core/Math.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Animation::CAnimatorSet;
using Elastos::Droid::Animation::IAnimator;
using Elastos::Droid::Animation::ObjectAnimator;
using Elastos::Droid::Animation::Keyframe;
using Elastos::Droid::Animation::IKeyframe;
using Elastos::Droid::Animation::IPropertyValuesHolder;
using Elastos::Droid::Animation::PropertyValuesHolder;
using Elastos::Droid::Animation::EIID_IAnimatorUpdateListener;
using Elastos::Droid::Content::Res::CColorStateList;
using Elastos::Droid::Content::Res::IResourcesTheme;
using Elastos::Droid::Graphics::Color;
using Elastos::Droid::Graphics::CPaint;
using Elastos::Droid::Graphics::CRectF;
using Elastos::Droid::Graphics::PaintStyle_STROKE;
using Elastos::Droid::Graphics::Typeface;
using Elastos::Droid::R;
using Elastos::Droid::Text::ITextPaint;
using Elastos::Droid::Text::Format::CTime;
using Elastos::Droid::Text::Format::ITime;
using Elastos::Droid::Text::Format::DateUtils;
using Elastos::Droid::Text::Format::IDateUtils;
using Elastos::Droid::View::Accessibility::IAccessibilityRecord;
using Elastos::Droid::View::Accessibility::IAccessibilityNodeInfo;
using Elastos::Droid::View::CViewGroupLayoutParams;
using Elastos::Droid::View::EIID_IViewOnTouchListener;
using Elastos::Droid::View::IHapticFeedbackConstants;
using Elastos::Droid::View::IViewGroupLayoutParams;
using Elastos::Droid::Utility::CTypedValue;
using Elastos::Droid::Utility::ITypedValue;
using Elastos::Core::CInteger32;
using Elastos::Core::IInteger32;
using Elastos::Core::StringUtils;
using Elastos::Core::CString;
using Elastos::Core::ICharSequence;
using Elastos::Utility::CArrayList;
using Elastos::Utility::ICalendarHelper;
using Elastos::Utility::CCalendarHelper;
using Elastos::Utility::ILocaleHelper;
using Elastos::Utility::CLocaleHelper;
using Elastos::Utility::ICollection;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Widget {

CAR_INTERFACE_IMPL(RadialTimePickerView::InvalidateUpdateListener, Object, IAnimatorUpdateListener);
RadialTimePickerView::InvalidateUpdateListener::InvalidateUpdateListener(
    /* [in] */ RadialTimePickerView* host)
    : mHost(host)
{}

ECode RadialTimePickerView::InvalidateUpdateListener::OnAnimationUpdate(
    /* [in] */ IValueAnimator* animation)
{
    return mHost->Invalidate();
}

RadialTimePickerView::Int32Holder::Int32Holder(
    /* [in] */ Int32 value)
    : mValue(value)
{
}

void RadialTimePickerView::Int32Holder::SetValue(
    /* [in] */ Int32 value)
{
    mValue = value;
}

Int32 RadialTimePickerView::Int32Holder::GetValue()
{
    return mValue;
}

CAR_INTERFACE_IMPL(RadialTimePickerView::RadialTimePickerViewOnTouchListener, Object, IViewOnTouchListener);
RadialTimePickerView::RadialTimePickerViewOnTouchListener::RadialTimePickerViewOnTouchListener(
    /* [in] */ RadialTimePickerView* host)
    : mHost(host)
{}

ECode RadialTimePickerView::RadialTimePickerViewOnTouchListener::OnTouch(
    /* [in] */ IView* v,
    /* [in] */ IMotionEvent* event,
    /* [out] */ Boolean* resValue)
{
    return mHost->OnTouch(v, event, resValue);
}

Boolean RadialTimePickerView::sInit = RadialTimePickerView::InitStatic();
const String RadialTimePickerView::TAG("ClockView");
const Boolean RadialTimePickerView::DEBUG = FALSE;
const Int32 RadialTimePickerView::DEBUG_COLOR = 0x20FF0000;
const Int32 RadialTimePickerView::DEBUG_TEXT_COLOR = 0x60FF0000;
const Int32 RadialTimePickerView::DEBUG_STROKE_WIDTH = 2;
const Int32 RadialTimePickerView::HOURS = 0;
const Int32 RadialTimePickerView::MINUTES = 1;
const Int32 RadialTimePickerView::HOURS_INNER = 2;
const Int32 RadialTimePickerView::AMPM = 3;
const Int32 RadialTimePickerView::SELECTOR_CIRCLE = 0;
const Int32 RadialTimePickerView::SELECTOR_DOT = 1;
const Int32 RadialTimePickerView::SELECTOR_LINE = 2;
const Int32 RadialTimePickerView::AM = 0;
const Int32 RadialTimePickerView::PM = 1;
const Int32 RadialTimePickerView::ALPHA_OPAQUE = 255;
const Int32 RadialTimePickerView::ALPHA_TRANSPARENT = 0;
const Int32 RadialTimePickerView::ALPHA_SELECTOR = 60; // was 51
const Int32 RadialTimePickerView::ALPHA_AMPM_SELECTED = ALPHA_SELECTOR;
const Int32 RadialTimePickerView::ALPHA_AMPM_PRESSED = 255; // was 175
const Float RadialTimePickerView::COSINE_30_DEGREES = ((Float) Elastos::Core::Math::Sqrt(3)) * 0.5;
const Float RadialTimePickerView::SINE_30_DEGREES = 0.5f;
const Int32 RadialTimePickerView::DEGREES_FOR_ONE_HOUR = 30;
const Int32 RadialTimePickerView::DEGREES_FOR_ONE_MINUTE = 6;
const Int32 RadialTimePickerView::CENTER_RADIUS = 2;
const Int32 RadialTimePickerView::HOURS_NUMBERS[] = {12, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11};
const Int32 RadialTimePickerView::HOURS_NUMBERS_24[] = {0, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23};
const Int32 RadialTimePickerView::MINUTES_NUMBERS[] = {0, 5, 10, 15, 20, 25, 30, 35, 40, 45, 50, 55};
AutoPtr<ArrayOf<Int32> > RadialTimePickerView::STATE_SET_SELECTED;

AutoPtr<ArrayOf<Int32> > RadialTimePickerView::sSnapPrefer30sMap;

Boolean RadialTimePickerView::InitStatic()
{
    sSnapPrefer30sMap = ArrayOf<Int32>::Alloc(361);
    STATE_SET_SELECTED = ArrayOf<Int32>::Alloc(1);
    (*STATE_SET_SELECTED)[0] = R::attr::state_selected;

    // Prepare mapping to snap touchable degrees to selectable degrees.
    PreparePrefer30sMap();
    return TRUE;
}

void RadialTimePickerView::PreparePrefer30sMap()
{
    // We'll split up the visible output and the non-visible output such that each visible
    // output will correspond to a range of 14 associated input degrees, and each non-visible
    // output will correspond to a range of 4 associate input degrees, so visible numbers
    // are more than 3 times easier to get than non-visible numbers:
    // {354-359,0-7}:0, {8-11}:6, {12-15}:12, {16-19}:18, {20-23}:24, {24-37}:30, etc.
    //
    // If an output of 30 degrees should correspond to a range of 14 associated degrees, then
    // we'll need any input between 24 - 37 to snap to 30. Working out from there, 20-23 should
    // snap to 24, while 38-41 should snap to 36. This is somewhat counter-intuitive, that you
    // can be touching 36 degrees but have the selection snapped to 30 degrees; however, this
    // inconsistency isn't noticeable at such fine-grained degrees, and it affords us the
    // ability to aggressively prefer the visible values by a factor of more than 3:1, which
    // greatly contributes to the selectability of these values.

    // The first output is 0, and each following output will increment by 6 {0, 6, 12, ...}.
    Int32 snappedOutputDegrees = 0;
    // Count of how many inputs we've designated to the specified output.
    Int32 count = 1;
    // How many input we expect for a specified output. This will be 14 for output divisible
    // by 30, and 4 for the remaining output. We'll special case the outputs of 0 and 360, so
    // the caller can decide which they need.
    Int32 expectedCount = 8;
    // Iterate through the input.
    for (Int32 degrees = 0; degrees < 361; degrees++) {
        // Save the input-output mapping.
        (*sSnapPrefer30sMap)[degrees] = snappedOutputDegrees;
        // If this is the last input for the specified output, calculate the next output and
        // the next expected count.
        if (count == expectedCount) {
            snappedOutputDegrees += 6;
            if (snappedOutputDegrees == 360) {
                expectedCount = 7;
            }
            else if (snappedOutputDegrees % 30 == 0) {
                expectedCount = 14;
            }
            else {
                expectedCount = 4;
            }
            count = 1;
        }
        else {
            count++;
        }
    }
}

Int32 RadialTimePickerView::SnapPrefer30s(
    /* [in] */ Int32 degrees)
{
    if (sSnapPrefer30sMap == NULL) {
        return -1;
    }
    return (*sSnapPrefer30sMap)[degrees];
}

Int32 RadialTimePickerView::SnapOnly30s(
    /* [in] */ Int32 degrees,
    /* [in] */ Int32 forceHigherOrLower)
{
    const Int32 stepSize = DEGREES_FOR_ONE_HOUR;
    Int32 floor = (degrees / stepSize) * stepSize;
    const Int32 ceiling = floor + stepSize;
    if (forceHigherOrLower == 1) {
        degrees = ceiling;
    }
    else if (forceHigherOrLower == -1) {
        if (degrees == floor) {
            floor -= stepSize;
        }
        degrees = floor;
    }
    else {
        if ((degrees - floor) < (ceiling - degrees)) {
            degrees = floor;
        }
        else {
            degrees = ceiling;
        }
    }
    return degrees;
}

CAR_INTERFACE_IMPL_2(RadialTimePickerView, View, IRadialTimePickerView, IViewOnTouchListener);
RadialTimePickerView::RadialTimePickerView()
    : mIs24HourMode(FALSE)
    , mShowHours(FALSE)
    , mIsOnInnerCircle(FALSE)
    , mXCenter(0)
    , mYCenter(0)
    , mMinHypotenuseForInnerNumber(0)
    , mMaxHypotenuseForOuterNumber(0)
    , mHalfwayHypotenusePoint(0)
    , mInnerTextSize(0)
    , mTransitionMidRadiusMultiplier(0)
    , mTransitionEndRadiusMultiplier(0)
    , mSelectionRadiusMultiplier(0)
    , mAmPmCircleRadius(0)
    , mAmPmYCenter(0)
    , mAmPmCircleRadiusMultiplier(0)
    , mAmPmTextColor(0)
    , mLeftIndicatorXCenter(0)
    , mRightIndicatorXCenter(0)
    , mAmPmUnselectedColor(0)
    , mAmPmSelectedColor(0)
    , mAmOrPm(0)
    , mAmOrPmPressed(0)
    , mDisabledAlpha(0)
    , mInputEnabled(TRUE)
{
    mHours12Texts = ArrayOf<String>::Alloc(12);
    mOuterHours24Texts = ArrayOf<String>::Alloc(12);
    mInnerHours24Texts = ArrayOf<String>::Alloc(12);
    mMinutesTexts = ArrayOf<String>::Alloc(12);

    mAmPmText = ArrayOf<String>::Alloc(2);

    mPaint = ArrayOf<IPaint*>::Alloc(2);
    mAlpha = ArrayOf<Int32Holder*>::Alloc(2);

    CPaint::New((IPaint**)&mPaintCenter);

    mPaintSelector = ArrayOf<ArrayOf<IPaint*>*>::Alloc(2);
    mPaintSelector->Set(0, ArrayOf<IPaint*>::Alloc(3));
    mPaintSelector->Set(1, ArrayOf<IPaint*>::Alloc(3));
    mAlphaSelector = ArrayOf<ArrayOf<Int32Holder*>* >::Alloc(2);
    mAlphaSelector->Set(0, ArrayOf<Int32Holder*>::Alloc(3));
    mAlphaSelector->Set(1, ArrayOf<Int32Holder*>::Alloc(3));

    CPaint::New((IPaint**)&mPaintAmPmText);
    mPaintAmPmCircle = ArrayOf<IPaint*>::Alloc(2);

    CPaint::New((IPaint**)&mPaintBackground);
    CPaint::New((IPaint**)&mPaintDisabled);
    CPaint::New((IPaint**)&mPaintDebug);

    memset(mCircleRadius, 0, 3 * sizeof(Float));
    memset(mTextSize, 0, 2 * sizeof(Float));

    memset(mInnerTextGridHeights, 0, 7 * sizeof(Float));
    memset(mInnerTextGridWidths, 0, 7 * sizeof(Float));

    memset(mCircleRadiusMultiplier, 0, 2 * sizeof(Float));
    memset(mNumbersRadiusMultiplier, 0, 3 * sizeof(Float));

    memset(mTextSizeMultiplier, 0, 3 * sizeof(Float));
    memset(mAnimationRadiusMultiplier, 0, 3 * sizeof(Float));
    mInvalidateUpdateListener = new InvalidateUpdateListener(this);

    memset(mLineLength, 0, 3 * sizeof(Int32));
    memset(mSelectionRadius, 0, 3 * sizeof(Int32));
    memset(mSelectionDegrees, 0, 3 * sizeof(Int32));
    CRectF::New((IRectF**)&mRectF);
    CArrayList::New((IArrayList**)&mHoursToMinutesAnims);
    CArrayList::New((IArrayList**)&mMinuteToHoursAnims);
}

ECode RadialTimePickerView::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    return constructor(context, attrs, R::attr::timePickerStyle);
}

ECode RadialTimePickerView::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ Int32 defStyle)
{
    View::constructor(context, attrs);

    // Pull disabled alpha from theme.
    AutoPtr<ITypedValue> outValue;
    CTypedValue::New((ITypedValue**)&outValue);
    AutoPtr<IResourcesTheme> theme;
    context->GetTheme((IResourcesTheme**)&theme);
    Boolean isFounded = FALSE;
    theme->ResolveAttribute(R::attr::disabledAlpha, outValue, TRUE, &isFounded);
    Float value = 0;
    outValue->GetFloat(&value);
    mDisabledAlpha = (Int32) (value * 255 + 0.5f);

    // process style attributes
    AutoPtr<IResources> res;
    GetResources((IResources**)&res);
    AutoPtr<ITypedArray> a;
    AutoPtr<ArrayOf<Int32> > attrIds = TO_ATTRS_ARRAYOF(R::styleable::TimePicker);
    mContext->ObtainStyledAttributes(attrs, attrIds, defStyle, 0, (ITypedArray**)&a);

    AutoPtr<IColorStateList> amPmBackgroundColor;
    a->GetColorStateList(R::styleable::TimePicker_amPmBackgroundColor, (IColorStateList**)&amPmBackgroundColor);
    if (amPmBackgroundColor == NULL) {
        res->GetColorStateList(R::color::timepicker_default_ampm_unselected_background_color_material,
            (IColorStateList**)&amPmBackgroundColor);
    }

    // Obtain the backup selected color. If the background color state
    // list doesn't have a state for selected, we'll use this color.
    Int32 amPmSelectedColor = 0, iValue = 0;
    res->GetColor(R::color::timepicker_default_ampm_selected_background_color_material, &iValue);
    a->GetColor(R::styleable::TimePicker_amPmSelectedBackgroundColor, iValue, &amPmSelectedColor);
    amPmBackgroundColor = CColorStateList::AddFirstIfMissing(
            amPmBackgroundColor, R::attr::state_selected, amPmSelectedColor);

    amPmBackgroundColor->GetColorForState(STATE_SET_SELECTED, amPmSelectedColor, &mAmPmSelectedColor);
    amPmBackgroundColor->GetDefaultColor(&mAmPmUnselectedColor);

    res->GetColor(R::color::timepicker_default_text_color_material, &iValue);
    a->GetColor(R::styleable::TimePicker_amPmTextColor, iValue, &mAmPmTextColor);

    Typeface::Create(String("sans-serif"), ITypeface::NORMAL, (ITypeface**)&mTypeface);

    // Initialize all alpha values to opaque.
    for (Int32 i = 0; i < mAlpha->GetLength(); i++) {
        mAlpha->Set(i, new Int32Holder(ALPHA_OPAQUE));
    }
    for (Int32 i = 0; i < mAlphaSelector->GetLength(); i++) {
        for (Int32 j = 0; j < (*mAlphaSelector)[i]->GetLength(); j++) {
            (*mAlphaSelector)[i]->Set(j, new Int32Holder(ALPHA_OPAQUE));
        }
    }

    res->GetColor(R::color::timepicker_default_text_color_material, &iValue);
    Int32 numbersTextColor = 0;
    a->GetColor(R::styleable::TimePicker_numbersTextColor, iValue, &numbersTextColor);

    CPaint::New((IPaint**)&((*mPaint)[HOURS]));
    (*mPaint)[HOURS]->SetAntiAlias(TRUE);
    (*mPaint)[HOURS]->SetTextAlign(Elastos::Droid::Graphics::PaintAlign_CENTER);
    mColor[HOURS] = numbersTextColor;

    CPaint::New((IPaint**)&((*mPaint)[MINUTES]));
    (*mPaint)[MINUTES]->SetAntiAlias(TRUE);
    (*mPaint)[MINUTES]->SetTextAlign(Elastos::Droid::Graphics::PaintAlign_CENTER);
    mColor[MINUTES] = numbersTextColor;

    mPaintCenter->SetColor(numbersTextColor);
    mPaintCenter->SetAntiAlias(TRUE);
    mPaintCenter->SetTextAlign(Elastos::Droid::Graphics::PaintAlign_CENTER);

    AutoPtr<IPaint> paint;
    CPaint::New((IPaint**)&paint);
    paint->SetAntiAlias(TRUE);
    (*mPaintSelector)[HOURS]->Set(SELECTOR_CIRCLE, paint);
    a->GetColor(
            R::styleable::TimePicker_numbersSelectorColor,
            R::color::timepicker_default_selector_color_material, &iValue);
    mColorSelector[HOURS][SELECTOR_CIRCLE] = iValue;

    paint = NULL;
    CPaint::New((IPaint**)&paint);
    paint->SetAntiAlias(TRUE);
    (*mPaintSelector)[HOURS]->Set(SELECTOR_DOT, paint);

    a->GetColor(
            R::styleable::TimePicker_numbersSelectorColor,
            R::color::timepicker_default_selector_color_material, &iValue);
    mColorSelector[HOURS][SELECTOR_DOT] = iValue;

    paint = NULL;
    CPaint::New((IPaint**)&paint);
    paint->SetAntiAlias(TRUE);
    paint->SetStrokeWidth(2);
    (*mPaintSelector)[HOURS]->Set(SELECTOR_LINE, paint);
    a->GetColor(
            R::styleable::TimePicker_numbersSelectorColor,
            R::color::timepicker_default_selector_color_material, &iValue);
    mColorSelector[HOURS][SELECTOR_LINE] = iValue;

    paint = NULL;
    CPaint::New((IPaint**)&paint);
    paint->SetAntiAlias(TRUE);
    (*mPaintSelector)[MINUTES]->Set(SELECTOR_CIRCLE, paint);
    a->GetColor(
            R::styleable::TimePicker_numbersSelectorColor,
            R::color::timepicker_default_selector_color_material, &iValue);
    mColorSelector[MINUTES][SELECTOR_CIRCLE] = iValue;

    paint = NULL;
    CPaint::New((IPaint**)&paint);
    paint->SetAntiAlias(TRUE);
    (*mPaintSelector)[MINUTES]->Set(SELECTOR_DOT, paint);

    a->GetColor(
            R::styleable::TimePicker_numbersSelectorColor,
            R::color::timepicker_default_selector_color_material, &iValue);
    mColorSelector[MINUTES][SELECTOR_DOT] = iValue;

    paint = NULL;
    CPaint::New((IPaint**)&paint);
    paint->SetAntiAlias(TRUE);
    paint->SetStrokeWidth(2);
    (*mPaintSelector)[MINUTES]->Set(SELECTOR_LINE, paint);

    a->GetColor(
            R::styleable::TimePicker_numbersSelectorColor,
            R::color::timepicker_default_selector_color_material, &iValue);
    mColorSelector[MINUTES][SELECTOR_LINE] = iValue;

    mPaintAmPmText->SetColor(mAmPmTextColor);
    mPaintAmPmText->SetTypeface(mTypeface);
    mPaintAmPmText->SetAntiAlias(TRUE);
    mPaintAmPmText->SetTextAlign(Elastos::Droid::Graphics::PaintAlign_CENTER);

    paint = NULL;
    CPaint::New((IPaint**)&paint);
    paint->SetAntiAlias(TRUE);
    mPaintAmPmCircle->Set(AM, paint);

    paint = NULL;
    CPaint::New((IPaint**)&paint);
    paint->SetAntiAlias(TRUE);
    mPaintAmPmCircle->Set(PM, paint);

    Int32 tmpValue = 0;
    res->GetColor(R::color::timepicker_default_numbers_background_color_material, &tmpValue);
    a->GetColor(R::styleable::TimePicker_numbersBackgroundColor, tmpValue, &iValue);
    mPaintBackground->SetColor(iValue);
    mPaintBackground->SetAntiAlias(TRUE);

    if (DEBUG) {
        mPaintDebug->SetColor(DEBUG_COLOR);
        mPaintDebug->SetAntiAlias(TRUE);
        mPaintDebug->SetStrokeWidth(DEBUG_STROKE_WIDTH);
        mPaintDebug->SetStyle(PaintStyle_STROKE);
        mPaintDebug->SetTextAlign(Elastos::Droid::Graphics::PaintAlign_CENTER);
    }

    mShowHours = TRUE;
    mIs24HourMode = FALSE;
    mAmOrPm = AM;
    mAmOrPmPressed = -1;

    InitHoursAndMinutesText();
    InitData();

    String str;
    res->GetString(R::string::timepicker_transition_mid_radius_multiplier, &str);
    mTransitionMidRadiusMultiplier =  StringUtils::ParseFloat(str);
    res->GetString(R::string::timepicker_transition_end_radius_multiplier, &str);
    mTransitionEndRadiusMultiplier = StringUtils::ParseFloat(str);

    memset(mTextGridHeights[HOURS], 0, 7 * sizeof(Float));
    memset(mTextGridHeights[MINUTES], 0, 7 * sizeof(Float));

    res->GetString(R::string::timepicker_selection_radius_multiplier, &str);
    mSelectionRadiusMultiplier = StringUtils::ParseFloat(str);

    a->Recycle();

    AutoPtr<IViewOnTouchListener> listener = new RadialTimePickerViewOnTouchListener(this);
    SetOnTouchListener(listener);
    SetClickable(TRUE);

    // Initial values
    AutoPtr<ICalendarHelper> helper;
    CCalendarHelper::AcquireSingleton((ICalendarHelper**)&helper);
    AutoPtr<ILocaleHelper> localeHelper;
    CLocaleHelper::AcquireSingleton((ILocaleHelper**)&localeHelper);
    AutoPtr<ILocale> locale;
    localeHelper->GetDefault((ILocale**)&locale);
    AutoPtr<ICalendar> calendar;
    helper->GetInstance(locale, (ICalendar**)&calendar);
    Int32 currentHour = 0;
    calendar->Get(ICalendar::HOUR_OF_DAY, &currentHour);
    Int32 currentMinute = 0;
    calendar->Get(ICalendar::MINUTE, &currentMinute);

    SetCurrentHour(currentHour);
    SetCurrentMinute(currentMinute);

    SetHapticFeedbackEnabled(TRUE);
    return NOERROR;
}

void RadialTimePickerView::OnMeasure(
    /* [in] */ Int32 widthMeasureSpec,
    /* [in] */ Int32 heightMeasureSpec)
{
    Int32 measuredWidth = View::MeasureSpec::GetSize(widthMeasureSpec);
    Int32 widthMode = View::MeasureSpec::GetMode(widthMeasureSpec);
    Int32 measuredHeight = View::MeasureSpec::GetSize(heightMeasureSpec);
    Int32 heightMode = View::MeasureSpec::GetMode(heightMeasureSpec);
    Int32 minDimension = Elastos::Core::Math::Min(measuredWidth, measuredHeight);

    View::OnMeasure(View::MeasureSpec::MakeMeasureSpec(minDimension, widthMode),
            View::MeasureSpec::MakeMeasureSpec(minDimension, heightMode));
}

ECode RadialTimePickerView::Initialize(
    /* [in] */ Int32 hour,
    /* [in] */ Int32 minute,
    /* [in] */ Boolean is24HourMode)
{
    mIs24HourMode = is24HourMode;
    SetCurrentHour(hour);
    SetCurrentMinute(minute);
    return NOERROR;
}

ECode RadialTimePickerView::SetCurrentItemShowing(
    /* [in] */ Int32 item,
    /* [in] */ Boolean animate)
{
    switch (item){
        case HOURS:
            ShowHours(animate);
            break;
        case MINUTES:
            ShowMinutes(animate);
            break;
        default:
            Logger::E(TAG, "ClockView does not support showing item " + item);
    }
    return NOERROR;
}

ECode RadialTimePickerView::GetCurrentItemShowing(
    /* [out] */ Int32* showing)
{
    VALIDATE_NOT_NULL(showing);
    *showing = mShowHours ? HOURS : MINUTES;
    return NOERROR;
}

ECode RadialTimePickerView::SetOnValueSelectedListener(
    /* [in] */ IOnValueSelectedListener* listener)
{
    mListener = listener;
    return NOERROR;
}

ECode RadialTimePickerView::SetCurrentHour(
    /* [in] */ Int32 hour)
{
    const Int32 degrees = (hour % 12) * DEGREES_FOR_ONE_HOUR;
    mSelectionDegrees[HOURS] = degrees;
    mSelectionDegrees[HOURS_INNER] = degrees;

    // 0 is 12 AM (midnight) and 12 is 12 PM (noon).
    mAmOrPm = (hour == 0 || (hour % 24) < 12) ? AM : PM;

    if (mIs24HourMode) {
        // Inner circle is 1 through 12.
        mIsOnInnerCircle = hour >= 1 && hour <= 12;
    }
    else {
        mIsOnInnerCircle = FALSE;
    }

    InitData();
    UpdateLayoutData();
    Invalidate();
    return NOERROR;
}

ECode RadialTimePickerView::GetCurrentHour(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    Int32 hour = (mSelectionDegrees[mIsOnInnerCircle ?
            HOURS_INNER : HOURS] / DEGREES_FOR_ONE_HOUR) % 12;
    if (mIs24HourMode) {
        // Convert the 12-hour value into 24-hour time based on where the
        // selector is positioned.
        if (mIsOnInnerCircle && hour == 0) {
            // Inner circle is 1 through 12.
            hour = 12;
        }
        else if (!mIsOnInnerCircle && hour != 0) {
            // Outer circle is 13 through 23 and 0.
            hour += 12;
        }
    }
    else if (mAmOrPm == PM) {
        hour += 12;
    }
    *result = hour;
    return NOERROR;
}

ECode RadialTimePickerView::SetCurrentMinute(
    /* [in] */ Int32 minute)
{
    mSelectionDegrees[MINUTES] = (minute % 60) * DEGREES_FOR_ONE_MINUTE;
    Invalidate();
    return NOERROR;
}

ECode RadialTimePickerView::GetCurrentMinute(
    /* [out] */ Int32* minute)
{
    VALIDATE_NOT_NULL(minute);
    *minute = (mSelectionDegrees[MINUTES] / DEGREES_FOR_ONE_MINUTE);
    return NOERROR;
}

ECode RadialTimePickerView::SetAmOrPm(
    /* [in] */ Int32 val)
{
    mAmOrPm = (val % 2);
    Invalidate();
    return NOERROR;
}

ECode RadialTimePickerView::GetAmOrPm(
    /* [out] */ Int32* amOrPm)
{
    VALIDATE_NOT_NULL(amOrPm);
    *amOrPm = mAmOrPm;
    return NOERROR;
}

ECode RadialTimePickerView::SwapAmPm()
{
    mAmOrPm = (mAmOrPm == AM) ? PM : AM;
    Invalidate();
    return NOERROR;
}

ECode RadialTimePickerView::ShowHours(
    /* [in] */ Boolean animate)
{
    if (mShowHours) return NOERROR;
    mShowHours = TRUE;
    if (animate) {
        StartMinutesToHoursAnimation();
    }
    InitData();
    UpdateLayoutData();
    Invalidate();
    return NOERROR;
}

ECode RadialTimePickerView::ShowMinutes(
    /* [in] */ Boolean animate)
{
    if (!mShowHours) return NOERROR;
    mShowHours = FALSE;
    if (animate) {
        StartHoursToMinutesAnimation();
    }
    InitData();
    UpdateLayoutData();
    Invalidate();
    return NOERROR;
}

void RadialTimePickerView::InitHoursAndMinutesText()
{
    // Initialize the hours and minutes numbers.
    for (Int32 i = 0; i < 12; i++) {
        AutoPtr<IInteger32> obj;
        CInteger32::New(HOURS_NUMBERS[i], (IInteger32**)&obj);
        AutoPtr<ArrayOf<IInterface*> > array = ArrayOf<IInterface*>::Alloc(1);
        array->Set(0, obj);
        (*mHours12Texts)[i] = StringUtils::Format(String("%d"), array);
        (*mInnerHours24Texts)[i] = StringUtils::Format(String("%d"), array);

        obj = NULL;
        CInteger32::New(HOURS_NUMBERS_24[i], (IInteger32**)&obj);
        array->Set(0, obj);
        (*mOuterHours24Texts)[i] = StringUtils::Format(String("%02d"), array);

        obj = NULL;
        CInteger32::New(MINUTES_NUMBERS[i], (IInteger32**)&obj);
        array->Set(0, obj);
        (*mMinutesTexts)[i] = StringUtils::Format(String("%02d"), array);
    }

    AutoPtr<ArrayOf<String> > amPmStrings;
    amPmStrings = TimePickerClockDelegate::GetAmPmStrings(mContext);
    (*mAmPmText)[AM] = (*amPmStrings)[0];
    (*mAmPmText)[PM] = (*amPmStrings)[1];
}

void RadialTimePickerView::InitData()
{
    if (mIs24HourMode) {
        mOuterTextHours = mOuterHours24Texts;
        mInnerTextHours = mInnerHours24Texts;
    }
    else {
        mOuterTextHours = mHours12Texts;
        mInnerTextHours = NULL;
    }

    mOuterTextMinutes = mMinutesTexts;

    AutoPtr<IResources> res;
    GetResources((IResources**)&res);

    String value;
    if (mShowHours) {
        if (mIs24HourMode) {
            res->GetString(R::string::timepicker_circle_radius_multiplier_24HourMode, &value);
            mCircleRadiusMultiplier[HOURS] = StringUtils::ParseFloat(value);
            res->GetString(R::string::timepicker_numbers_radius_multiplier_outer, &value);
            mNumbersRadiusMultiplier[HOURS] = StringUtils::ParseFloat(value);
            res->GetString(R::string::timepicker_text_size_multiplier_outer, &value);
            mTextSizeMultiplier[HOURS] = StringUtils::ParseFloat(value);

            res->GetString(R::string::timepicker_numbers_radius_multiplier_inner, &value);
            mNumbersRadiusMultiplier[HOURS_INNER] = StringUtils::ParseFloat(value);
            res->GetString(R::string::timepicker_text_size_multiplier_inner, &value);
            mTextSizeMultiplier[HOURS_INNER] = StringUtils::ParseFloat(value);
        }
        else {
            res->GetString(R::string::timepicker_circle_radius_multiplier, &value);
            mCircleRadiusMultiplier[HOURS] = StringUtils::ParseFloat(value);
            res->GetString(R::string::timepicker_numbers_radius_multiplier_normal, &value);
            mNumbersRadiusMultiplier[HOURS] = StringUtils::ParseFloat(value);
            res->GetString(R::string::timepicker_text_size_multiplier_normal, &value);
            mTextSizeMultiplier[HOURS] = StringUtils::ParseFloat(value);
        }
    }
    else {
        res->GetString(R::string::timepicker_circle_radius_multiplier, &value);
        mCircleRadiusMultiplier[MINUTES] = StringUtils::ParseFloat(value);
        res->GetString(R::string::timepicker_numbers_radius_multiplier_normal, &value);
        mNumbersRadiusMultiplier[MINUTES] = StringUtils::ParseFloat(value);
        res->GetString(R::string::timepicker_text_size_multiplier_normal, &value);
        mTextSizeMultiplier[MINUTES] = StringUtils::ParseFloat(value);
    }

    mAnimationRadiusMultiplier[HOURS] = 1;
    mAnimationRadiusMultiplier[HOURS_INNER] = 1;
    mAnimationRadiusMultiplier[MINUTES] = 1;

    res->GetString(R::string::timepicker_ampm_circle_radius_multiplier, &value);
    mAmPmCircleRadiusMultiplier = StringUtils::ParseFloat(value);

    (*mAlpha)[HOURS]->SetValue(mShowHours ? ALPHA_OPAQUE : ALPHA_TRANSPARENT);
    (*mAlpha)[MINUTES]->SetValue(mShowHours ? ALPHA_TRANSPARENT : ALPHA_OPAQUE);

    (*(*mAlphaSelector)[HOURS])[SELECTOR_CIRCLE]->SetValue(
            mShowHours ? ALPHA_SELECTOR : ALPHA_TRANSPARENT);
    (*(*mAlphaSelector)[HOURS])[SELECTOR_DOT]->SetValue(
            mShowHours ? ALPHA_OPAQUE : ALPHA_TRANSPARENT);
    (*(*mAlphaSelector)[HOURS])[SELECTOR_LINE]->SetValue(
            mShowHours ? ALPHA_SELECTOR : ALPHA_TRANSPARENT);

    (*(*mAlphaSelector)[MINUTES])[SELECTOR_CIRCLE]->SetValue(
            mShowHours ? ALPHA_TRANSPARENT : ALPHA_SELECTOR);
    (*(*mAlphaSelector)[MINUTES])[SELECTOR_DOT]->SetValue(
            mShowHours ? ALPHA_TRANSPARENT : ALPHA_OPAQUE);
    (*(*mAlphaSelector)[MINUTES])[SELECTOR_LINE]->SetValue(
            mShowHours ? ALPHA_TRANSPARENT : ALPHA_SELECTOR);
}

ECode RadialTimePickerView::OnLayout(
    /* [in] */ Boolean changed,
    /* [in] */ Int32 left,
    /* [in] */ Int32 top,
    /* [in] */ Int32 right,
    /* [in] */ Int32 bottom)
{
    UpdateLayoutData();
    return NOERROR;
}

void RadialTimePickerView::UpdateLayoutData()
{
    Int32 value = 0;
    GetWidth(&value);
    mXCenter = value / 2;
    GetHeight(&value);
    mYCenter = value / 2;

    const Int32 min = Elastos::Core::Math::Min(mXCenter, mYCenter);

    mCircleRadius[HOURS] = min * mCircleRadiusMultiplier[HOURS];
    mCircleRadius[HOURS_INNER] = min * mCircleRadiusMultiplier[HOURS];
    mCircleRadius[MINUTES] = min * mCircleRadiusMultiplier[MINUTES];

    if (!mIs24HourMode) {
        // We'll need to draw the AM/PM circles, so the main circle will need to have
        // a slightly higher center. To keep the entire view centered vertically, we'll
        // have to push it up by half the radius of the AM/PM circles.
        Int32 amPmCircleRadius = (Int32) (mCircleRadius[HOURS] * mAmPmCircleRadiusMultiplier);
        mYCenter -= amPmCircleRadius / 2;
    }

    mMinHypotenuseForInnerNumber = (Int32) (mCircleRadius[HOURS]
            * mNumbersRadiusMultiplier[HOURS_INNER]) - mSelectionRadius[HOURS];
    mMaxHypotenuseForOuterNumber = (Int32) (mCircleRadius[HOURS]
            * mNumbersRadiusMultiplier[HOURS]) + mSelectionRadius[HOURS];
    mHalfwayHypotenusePoint = (Int32) (mCircleRadius[HOURS]
            * ((mNumbersRadiusMultiplier[HOURS] + mNumbersRadiusMultiplier[HOURS_INNER]) / 2));

    mTextSize[HOURS] = mCircleRadius[HOURS] * mTextSizeMultiplier[HOURS];
    mTextSize[MINUTES] = mCircleRadius[MINUTES] * mTextSizeMultiplier[MINUTES];

    if (mIs24HourMode) {
        mInnerTextSize = mCircleRadius[HOURS] * mTextSizeMultiplier[HOURS_INNER];
    }

    CalculateGridSizesHours();
    CalculateGridSizesMinutes();

    mSelectionRadius[HOURS] = (Int32) (mCircleRadius[HOURS] * mSelectionRadiusMultiplier);
    mSelectionRadius[HOURS_INNER] = mSelectionRadius[HOURS];
    mSelectionRadius[MINUTES] = (Int32) (mCircleRadius[MINUTES] * mSelectionRadiusMultiplier);

    mAmPmCircleRadius = (Int32) (mCircleRadius[HOURS] * mAmPmCircleRadiusMultiplier);
    mPaintAmPmText->SetTextSize(mAmPmCircleRadius * 3 / 4);

    // Line up the vertical center of the AM/PM circles with the bottom of the main circle.
    mAmPmYCenter = mYCenter + mCircleRadius[HOURS];

    // Line up the horizontal edges of the AM/PM circles with the horizontal edges
    // of the main circle
    mLeftIndicatorXCenter = mXCenter - mCircleRadius[HOURS] + mAmPmCircleRadius;
    mRightIndicatorXCenter = mXCenter + mCircleRadius[HOURS] - mAmPmCircleRadius;
}

void RadialTimePickerView::OnDraw(
    /* [in] */ ICanvas* canvas)
{
    if (!mInputEnabled) {
        Int32 width = 0, height = 0, tmp = 0;
        GetWidth(&width);
        GetHeight(&height);
        canvas->SaveLayerAlpha(0, 0, width, height, mDisabledAlpha, &tmp);
    }
    else {
        Int32 result = 0;
        canvas->Save(&result);
    }

    CalculateGridSizesHours();
    CalculateGridSizesMinutes();

    DrawCircleBackground(canvas);
    DrawSelector(canvas);

    DrawTextElements(canvas, mTextSize[HOURS], mTypeface, mOuterTextHours,
            mTextGridWidths[HOURS], mTextGridHeights[HOURS], (*mPaint)[HOURS],
            mColor[HOURS], (*mAlpha)[HOURS]->GetValue());

    if (mIs24HourMode && mInnerTextHours != NULL) {
        DrawTextElements(canvas, mInnerTextSize, mTypeface, mInnerTextHours,
                mInnerTextGridWidths, mInnerTextGridHeights, (*mPaint)[HOURS],
                mColor[HOURS], (*mAlpha)[HOURS]->GetValue());
    }

    DrawTextElements(canvas, mTextSize[MINUTES], mTypeface, mOuterTextMinutes,
            mTextGridWidths[MINUTES], mTextGridHeights[MINUTES], (*mPaint)[MINUTES],
            mColor[MINUTES], (*mAlpha)[MINUTES]->GetValue());

    DrawCenter(canvas);
    if (!mIs24HourMode) {
        DrawAmPm(canvas);
    }

    if (DEBUG) {
        DrawDebug(canvas);
    }

    canvas->Restore();
}

void RadialTimePickerView::DrawCircleBackground(
    /* [in] */ ICanvas* canvas)
{
    canvas->DrawCircle(mXCenter, mYCenter, mCircleRadius[HOURS], mPaintBackground);
}

void RadialTimePickerView::DrawCenter(
    /* [in] */ ICanvas* canvas)
{
    canvas->DrawCircle(mXCenter, mYCenter, CENTER_RADIUS, mPaintCenter);
}

void RadialTimePickerView::DrawSelector(
    /* [in] */ ICanvas* canvas)
{
    DrawSelector(canvas, mIsOnInnerCircle ? HOURS_INNER : HOURS);
    DrawSelector(canvas, MINUTES);
}

void RadialTimePickerView::DrawAmPm(
    /* [in] */ ICanvas* canvas)
{
    Boolean isLayoutRtl = FALSE;
    IsLayoutRtl(&isLayoutRtl);

    Int32 amColor = mAmPmUnselectedColor;
    Int32 amAlpha = ALPHA_OPAQUE;
    Int32 pmColor = mAmPmUnselectedColor;
    Int32 pmAlpha = ALPHA_OPAQUE;
    if (mAmOrPm == AM) {
        amColor = mAmPmSelectedColor;
        amAlpha = ALPHA_AMPM_SELECTED;
    }
    else if (mAmOrPm == PM) {
        pmColor = mAmPmSelectedColor;
        pmAlpha = ALPHA_AMPM_SELECTED;
    }
    if (mAmOrPmPressed == AM) {
        amColor = mAmPmSelectedColor;
        amAlpha = ALPHA_AMPM_PRESSED;
    }
    else if (mAmOrPmPressed == PM) {
        pmColor = mAmPmSelectedColor;
        pmAlpha = ALPHA_AMPM_PRESSED;
    }

    // Draw the two circles
    (*mPaintAmPmCircle)[AM]->SetColor(amColor);
    (*mPaintAmPmCircle)[AM]->SetAlpha(GetMultipliedAlpha(amColor, amAlpha));
    canvas->DrawCircle(isLayoutRtl ? mRightIndicatorXCenter : mLeftIndicatorXCenter,
            mAmPmYCenter, mAmPmCircleRadius, (*mPaintAmPmCircle)[AM]);

    (*mPaintAmPmCircle)[PM]->SetColor(pmColor);
    (*mPaintAmPmCircle)[PM]->SetAlpha(GetMultipliedAlpha(pmColor, pmAlpha));
    canvas->DrawCircle(isLayoutRtl ? mLeftIndicatorXCenter : mRightIndicatorXCenter,
            mAmPmYCenter, mAmPmCircleRadius, (*mPaintAmPmCircle)[PM]);

    // Draw the AM/PM texts on top
    mPaintAmPmText->SetColor(mAmPmTextColor);
    Float descent = 0, ascent = 0;
    mPaintAmPmText->Descent(&descent);
    mPaintAmPmText->Ascent(&ascent);
    Float textYCenter = mAmPmYCenter - (Int32) (descent + ascent) / 2;

    canvas->DrawText(isLayoutRtl ? (*mAmPmText)[PM] : (*mAmPmText)[AM], mLeftIndicatorXCenter,
            textYCenter, mPaintAmPmText);
    canvas->DrawText(isLayoutRtl ? (*mAmPmText)[AM] : (*mAmPmText)[PM], mRightIndicatorXCenter,
            textYCenter, mPaintAmPmText);
}

Int32 RadialTimePickerView::GetMultipliedAlpha(
    /* [in] */ Int32 argb,
    /* [in] */ Int32 alpha)
{
    return (Int32) (Color::Alpha(argb) * (alpha / 255.0) + 0.5);
}

void RadialTimePickerView::DrawSelector(
    /* [in] */ ICanvas* canvas,
    /* [in] */ Int32 index)
{
    // Calculate the current radius at which to place the selection circle.
    mLineLength[index] = (Int32) (mCircleRadius[index]
            * mNumbersRadiusMultiplier[index] * mAnimationRadiusMultiplier[index]);

    Double selectionRadians = Elastos::Core::Math::ToRadians(mSelectionDegrees[index]);

    Int32 pointX = mXCenter + (Int32) (mLineLength[index] * Elastos::Core::Math::Sin(selectionRadians));
    Int32 pointY = mYCenter - (Int32) (mLineLength[index] * Elastos::Core::Math::Cos(selectionRadians));

    Int32 color = 0;
    Int32 alpha = 0;
    AutoPtr<IPaint> paint;

    // Draw the selection circle
    color = mColorSelector[index % 2][SELECTOR_CIRCLE];
    alpha = (*(*mAlphaSelector)[index % 2])[SELECTOR_CIRCLE]->GetValue();
    paint = (*(*mPaintSelector)[index % 2])[SELECTOR_CIRCLE];
    paint->SetColor(color);
    paint->SetAlpha(GetMultipliedAlpha(color, alpha));
    canvas->DrawCircle(pointX, pointY, mSelectionRadius[index], paint);

    // Draw the dot if needed
    if (mSelectionDegrees[index] % 30 != 0) {
        // We're not on a direct tick
        color = mColorSelector[index % 2][SELECTOR_DOT];
        alpha = (*(*mAlphaSelector)[index % 2])[SELECTOR_DOT]->GetValue();
        paint = (*(*mPaintSelector)[index % 2])[SELECTOR_DOT];
        paint->SetColor(color);
        paint->SetAlpha(GetMultipliedAlpha(color, alpha));
        canvas->DrawCircle(pointX, pointY, (mSelectionRadius[index] * 2 / 7), paint);
    }
    else {
        // We're not drawing the dot, so shorten the line to only go as far as the edge of the
        // selection circle
        Int32 lineLength = mLineLength[index] - mSelectionRadius[index];
        pointX = mXCenter + (Int32) (lineLength * Elastos::Core::Math::Sin(selectionRadians));
        pointY = mYCenter - (Int32) (lineLength * Elastos::Core::Math::Cos(selectionRadians));
    }

    // Draw the line
    color = mColorSelector[index % 2][SELECTOR_LINE];
    alpha = (*(*mAlphaSelector)[index % 2])[SELECTOR_LINE]->GetValue();
    paint = (*(*mPaintSelector)[index % 2])[SELECTOR_LINE];
    paint->SetColor(color);
    paint->SetAlpha(GetMultipliedAlpha(color, alpha));
    canvas->DrawLine(mXCenter, mYCenter, pointX, pointY, paint);
}

void RadialTimePickerView::DrawDebug(
    /* [in] */ ICanvas* canvas)
{
    // Draw outer numbers circle
    const Float outerRadius = mCircleRadius[HOURS] * mNumbersRadiusMultiplier[HOURS];
    canvas->DrawCircle(mXCenter, mYCenter, outerRadius, mPaintDebug);

    // Draw inner numbers circle
    const Float innerRadius = mCircleRadius[HOURS] * mNumbersRadiusMultiplier[HOURS_INNER];
    canvas->DrawCircle(mXCenter, mYCenter, innerRadius, mPaintDebug);

    // Draw outer background circle
    canvas->DrawCircle(mXCenter, mYCenter, mCircleRadius[HOURS], mPaintDebug);

    // Draw outer rectangle for circles
    Float left = mXCenter - outerRadius;
    Float top = mYCenter - outerRadius;
    Float right = mXCenter + outerRadius;
    Float bottom = mYCenter + outerRadius;
    CRectF::New(left, top, right, bottom, (IRectF**)&mRectF);
    canvas->DrawRect(mRectF, mPaintDebug);

    // Draw outer rectangle for background
    left = mXCenter - mCircleRadius[HOURS];
    top = mYCenter - mCircleRadius[HOURS];
    right = mXCenter + mCircleRadius[HOURS];
    bottom = mYCenter + mCircleRadius[HOURS];
    mRectF->Set(left, top, right, bottom);
    canvas->DrawRect(mRectF, mPaintDebug);

    // Draw outer view rectangle
    Int32 w = 0, h = 0;
    GetWidth(&w);
    GetHeight(&h);
    mRectF->Set(0, 0, w, h);
    canvas->DrawRect(mRectF, mPaintDebug);

    // Draw selected time
    Int32 hour = 0, minute = 0;
    GetCurrentHour(&hour);
    GetCurrentMinute(&minute);

    AutoPtr<IInteger32> hObj, mObj;
    CInteger32::New(hour, (IInteger32**)&hObj);
    CInteger32::New(minute, (IInteger32**)&mObj);
    AutoPtr<ArrayOf<IInterface*> > array = ArrayOf<IInterface*>::Alloc(1);
    array->Set(0, hObj);
    array->Set(1, mObj);
    String selected = StringUtils::Format(String("%02d:%02d"), array);

    AutoPtr<IViewGroupLayoutParams> lp;
    CViewGroupLayoutParams::New(IViewGroupLayoutParams::WRAP_CONTENT,
            IViewGroupLayoutParams::WRAP_CONTENT, (IViewGroupLayoutParams**)&lp);

    AutoPtr<IContext> context;
    GetContext((IContext**)&context);
    AutoPtr<ITextView> tv;
    CTextView::New(context, (ITextView**)&tv);
    IView::Probe(tv)->SetLayoutParams(lp);

    AutoPtr<ICharSequence> seq;
    CString::New(selected, (ICharSequence**)&seq);
    tv->SetText(seq);
    IView::Probe(tv)->Measure(View::MeasureSpec::UNSPECIFIED, View::MeasureSpec::UNSPECIFIED);
    AutoPtr<ITextPaint> paint;
    tv->GetPaint((ITextPaint**)&paint);
    IPaint::Probe(paint)->SetColor(DEBUG_TEXT_COLOR);

    Int32 width = 0;
    IView::Probe(tv)->GetMeasuredWidth(&width);

    Float descent = 0, ascent = 0;
    IPaint::Probe(paint)->Descent(&descent);
    IPaint::Probe(paint)->Ascent(&ascent);
    Float height = descent - ascent;
    Float x = mXCenter - width / 2;
    Float y = mYCenter + 1.5f * height;

    canvas->DrawText(selected/*.toString()*/, x, y, IPaint::Probe(paint));
}

void RadialTimePickerView::CalculateGridSizesHours()
{
    // Calculate the text positions
    Float numbersRadius = mCircleRadius[HOURS]
            * mNumbersRadiusMultiplier[HOURS] * mAnimationRadiusMultiplier[HOURS];

    // Calculate the positions for the 12 numbers in the main circle.
    CalculateGridSizes((*mPaint)[HOURS], numbersRadius, mXCenter, mYCenter,
            mTextSize[HOURS], mTextGridHeights[HOURS], mTextGridWidths[HOURS]);

    // If we have an inner circle, calculate those positions too.
    if (mIs24HourMode) {
        Float innerNumbersRadius = mCircleRadius[HOURS_INNER]
                * mNumbersRadiusMultiplier[HOURS_INNER]
                * mAnimationRadiusMultiplier[HOURS_INNER];

        CalculateGridSizes((*mPaint)[HOURS], innerNumbersRadius, mXCenter, mYCenter,
                mInnerTextSize, mInnerTextGridHeights, mInnerTextGridWidths);
    }
}

void RadialTimePickerView::CalculateGridSizesMinutes()
{
    // Calculate the text positions
    Float numbersRadius = mCircleRadius[MINUTES]
            * mNumbersRadiusMultiplier[MINUTES] * mAnimationRadiusMultiplier[MINUTES];

    // Calculate the positions for the 12 numbers in the main circle.
    CalculateGridSizes((*mPaint)[MINUTES], numbersRadius, mXCenter, mYCenter,
            mTextSize[MINUTES], mTextGridHeights[MINUTES], mTextGridWidths[MINUTES]);
}

void RadialTimePickerView::CalculateGridSizes(
    /* [in] */ IPaint* paint,
    /* [in] */ Float numbersRadius,
    /* [in] */ Float xCenter,
    /* [in] */ Float yCenter,
    /* [in] */ Float textSize,
    /* [in] */ Float* textGridHeights,
    /* [in] */ Float* textGridWidths)
{
    /*
     * The numbers need to be drawn in a 7x7 grid, representing the points on the Unit Circle.
     */
    const Float offset1 = numbersRadius;
    // cos(30) = a / r => r * cos(30)
    const Float offset2 = numbersRadius * COSINE_30_DEGREES;
    // sin(30) = o / r => r * sin(30)
    const Float offset3 = numbersRadius * SINE_30_DEGREES;

    paint->SetTextSize(textSize);
    Float descent = 0, ascent = 0;
    paint->Descent(&descent);
    paint->Ascent(&ascent);
    // We'll need yTextBase to be slightly lower to account for the text's baseline.
    yCenter -= (descent + ascent) / 2;

    textGridHeights[0] = yCenter - offset1;
    textGridWidths[0] = xCenter - offset1;
    textGridHeights[1] = yCenter - offset2;
    textGridWidths[1] = xCenter - offset2;
    textGridHeights[2] = yCenter - offset3;
    textGridWidths[2] = xCenter - offset3;
    textGridHeights[3] = yCenter;
    textGridWidths[3] = xCenter;
    textGridHeights[4] = yCenter + offset3;
    textGridWidths[4] = xCenter + offset3;
    textGridHeights[5] = yCenter + offset2;
    textGridWidths[5] = xCenter + offset2;
    textGridHeights[6] = yCenter + offset1;
    textGridWidths[6] = xCenter + offset1;
}

void RadialTimePickerView::DrawTextElements(
    /* [in] */ ICanvas* canvas,
    /* [in] */ Float textSize,
    /* [in] */ ITypeface* typeface,
    /* [in] */ ArrayOf<String>* texts,
    /* [in] */ Float* textGridWidths,
    /* [in] */ Float* textGridHeights,
    /* [in] */ IPaint* paint,
    /* [in] */ Int32 color,
    /* [in] */ Int32 alpha)
{
    paint->SetTextSize(textSize);
    paint->SetTypeface(typeface);
    paint->SetColor(color);
    paint->SetAlpha(GetMultipliedAlpha(color, alpha));
    canvas->DrawText((*texts)[0], textGridWidths[3], textGridHeights[0], paint);
    canvas->DrawText((*texts)[1], textGridWidths[4], textGridHeights[1], paint);
    canvas->DrawText((*texts)[2], textGridWidths[5], textGridHeights[2], paint);
    canvas->DrawText((*texts)[3], textGridWidths[6], textGridHeights[3], paint);
    canvas->DrawText((*texts)[4], textGridWidths[5], textGridHeights[4], paint);
    canvas->DrawText((*texts)[5], textGridWidths[4], textGridHeights[5], paint);
    canvas->DrawText((*texts)[6], textGridWidths[3], textGridHeights[6], paint);
    canvas->DrawText((*texts)[7], textGridWidths[2], textGridHeights[5], paint);
    canvas->DrawText((*texts)[8], textGridWidths[1], textGridHeights[4], paint);
    canvas->DrawText((*texts)[9], textGridWidths[0], textGridHeights[3], paint);
    canvas->DrawText((*texts)[10], textGridWidths[1], textGridHeights[2], paint);
    canvas->DrawText((*texts)[11], textGridWidths[2], textGridHeights[1], paint);
}

void RadialTimePickerView::SetAnimationRadiusMultiplierHours(
    /* [in] */ Float animationRadiusMultiplier)
{
    mAnimationRadiusMultiplier[HOURS] = animationRadiusMultiplier;
    mAnimationRadiusMultiplier[HOURS_INNER] = animationRadiusMultiplier;
}

void RadialTimePickerView::SetAnimationRadiusMultiplierMinutes(
    /* [in] */ Float animationRadiusMultiplier)
{
    mAnimationRadiusMultiplier[MINUTES] = animationRadiusMultiplier;
}

AutoPtr<IObjectAnimator> RadialTimePickerView::GetRadiusDisappearAnimator(
    /* [in] */ IInterface* target,
    /* [in] */ const String& radiusPropertyName,
    /* [in] */ InvalidateUpdateListener* updateListener,
    /* [in] */ Float midRadiusMultiplier,
    /* [in] */ Float endRadiusMultiplier)
{
    AutoPtr<IKeyframe> kf0, kf1, kf2;
    Float midwayPoint = 0.2f;
    Int32 duration = 500;

    Keyframe::OfFloat(0.f, 1, (IKeyframe**)&kf0);
    Keyframe::OfFloat(midwayPoint, midRadiusMultiplier, (IKeyframe**)&kf1);
    Keyframe::OfFloat(1.f, endRadiusMultiplier, (IKeyframe**)&kf2);
    AutoPtr<ArrayOf<IKeyframe*> > array = ArrayOf<IKeyframe*>::Alloc(3);
    array->Set(0, kf0);
    array->Set(1, kf1);
    array->Set(2, kf2);
    AutoPtr<IPropertyValuesHolder> radiusDisappear = PropertyValuesHolder::OfKeyframe(
            radiusPropertyName, array);

    AutoPtr<ArrayOf<IPropertyValuesHolder*> > values = ArrayOf<IPropertyValuesHolder*>::Alloc(1);
    values->Set(0, radiusDisappear);
    AutoPtr<IObjectAnimator> animator = ObjectAnimator::OfPropertyValuesHolder(target, values);
    IAnimator::Probe(animator)->SetDuration(duration);
    IValueAnimator::Probe(animator)->AddUpdateListener(updateListener);
    return animator;
}

AutoPtr<IObjectAnimator> RadialTimePickerView::GetRadiusReappearAnimator(
    /* [in] */ IInterface* target,
    /* [in] */ const String& radiusPropertyName,
    /* [in] */ InvalidateUpdateListener* updateListener,
    /* [in] */ Float midRadiusMultiplier,
    /* [in] */ Float endRadiusMultiplier)
{
    AutoPtr<IKeyframe> kf0, kf1, kf2, kf3;
    Float midwayPoint = 0.2f;
    Int32 duration = 500;

    // Set up animator for reappearing.
    Float delayMultiplier = 0.25f;
    Float transitionDurationMultiplier = 1.f;
    Float totalDurationMultiplier = transitionDurationMultiplier + delayMultiplier;
    Int32 totalDuration = (Int32) (duration * totalDurationMultiplier);
    Float delayPoint = (delayMultiplier * duration) / totalDuration;
    midwayPoint = 1 - (midwayPoint * (1 - delayPoint));

    Keyframe::OfFloat(0.f, endRadiusMultiplier, (IKeyframe**)&kf0);
    Keyframe::OfFloat(delayPoint, endRadiusMultiplier, (IKeyframe**)&kf1);
    Keyframe::OfFloat(midwayPoint, midRadiusMultiplier, (IKeyframe**)&kf2);
    Keyframe::OfFloat(1.f, 1, (IKeyframe**)&kf3);

    AutoPtr<ArrayOf<IKeyframe*> > array = ArrayOf<IKeyframe*>::Alloc(4);
    array->Set(0, kf0);
    array->Set(1, kf1);
    array->Set(2, kf2);
    array->Set(3, kf3);
    AutoPtr<IPropertyValuesHolder> radiusReappear = PropertyValuesHolder::OfKeyframe(
            radiusPropertyName, array);

    AutoPtr<ArrayOf<IPropertyValuesHolder*> > values = ArrayOf<IPropertyValuesHolder*>::Alloc(1);
    values->Set(0, radiusReappear);
    AutoPtr<IObjectAnimator> animator = ObjectAnimator::OfPropertyValuesHolder(target, values);
    IAnimator::Probe(animator)->SetDuration(totalDuration);
    IValueAnimator::Probe(animator)->AddUpdateListener(updateListener);
    return animator;
}

AutoPtr<IObjectAnimator> RadialTimePickerView::GetFadeOutAnimator(
    /* [in] */ Int32Holder* target,
    /* [in] */ Int32 startAlpha,
    /* [in] */ Int32 endAlpha,
    /* [in] */ InvalidateUpdateListener* updateListener)
{
    Int32 duration = 500;
    AutoPtr<ArrayOf<Int32> > values = ArrayOf<Int32>::Alloc(2);
    (*values)[0] = startAlpha;
    (*values)[1] = endAlpha;
    AutoPtr<IObjectAnimator> animator = ObjectAnimator::OfInt32(target->Probe(EIID_IInterface), String("value"), values);
    IAnimator::Probe(animator)->SetDuration(duration);
    IValueAnimator::Probe(animator)->AddUpdateListener(updateListener);

    return animator;
}

AutoPtr<IObjectAnimator> RadialTimePickerView::GetFadeInAnimator(
    /* [in] */ Int32Holder* target,
    /* [in] */ Int32 startAlpha,
    /* [in] */ Int32 endAlpha,
    /* [in] */ InvalidateUpdateListener* updateListener)
{
    AutoPtr<IKeyframe> kf0, kf1, kf2;
    Int32 duration = 500;

    // Set up animator for reappearing.
    Float delayMultiplier = 0.25f;
    Float transitionDurationMultiplier = 1.f;
    Float totalDurationMultiplier = transitionDurationMultiplier + delayMultiplier;
    Int32 totalDuration = (Int32) (duration * totalDurationMultiplier);
    Float delayPoint = (delayMultiplier * duration) / totalDuration;

    Keyframe::OfInt32(0.f, startAlpha, (IKeyframe**)&kf0);
    Keyframe::OfInt32(delayPoint, startAlpha, (IKeyframe**)&kf1);
    Keyframe::OfInt32(1.f, endAlpha, (IKeyframe**)&kf2);

    AutoPtr<ArrayOf<IKeyframe*> > array = ArrayOf<IKeyframe*>::Alloc(3);
    array->Set(0, kf0);
    array->Set(1, kf1);
    array->Set(2, kf2);
    AutoPtr<IPropertyValuesHolder> fadeIn = PropertyValuesHolder::OfKeyframe(String("value"), array);

    AutoPtr<ArrayOf<IPropertyValuesHolder*> > values = ArrayOf<IPropertyValuesHolder*>::Alloc(1);
    values->Set(0, fadeIn);
    AutoPtr<IObjectAnimator> animator = ObjectAnimator::OfPropertyValuesHolder(target->Probe(EIID_IInterface), values);
    IAnimator::Probe(animator)->SetDuration(totalDuration);
    IValueAnimator::Probe(animator)->AddUpdateListener(updateListener);
    return animator;
}

void RadialTimePickerView::StartHoursToMinutesAnimation()
{
    Int32 size = 0;
    if ((mHoursToMinutesAnims->GetSize(&size), size) == 0) {
        AutoPtr<IObjectAnimator> animator = GetRadiusDisappearAnimator(TO_IINTERFACE(this),
                String("animationRadiusMultiplierHours"), mInvalidateUpdateListener,
                mTransitionMidRadiusMultiplier, mTransitionEndRadiusMultiplier);
        mHoursToMinutesAnims->Add(animator);
        animator = GetFadeOutAnimator((*mAlpha)[HOURS], ALPHA_OPAQUE, ALPHA_TRANSPARENT, mInvalidateUpdateListener);
        mHoursToMinutesAnims->Add(animator);

        animator = GetFadeOutAnimator((*((*mAlphaSelector)[HOURS]))[SELECTOR_CIRCLE],
                ALPHA_SELECTOR, ALPHA_TRANSPARENT, mInvalidateUpdateListener);
        mHoursToMinutesAnims->Add(animator);

        animator = GetFadeOutAnimator((*((*mAlphaSelector)[HOURS]))[SELECTOR_DOT],
                ALPHA_OPAQUE, ALPHA_TRANSPARENT, mInvalidateUpdateListener);
        mHoursToMinutesAnims->Add(animator);

        animator = GetFadeOutAnimator((*((*mAlphaSelector)[HOURS]))[SELECTOR_LINE],
                ALPHA_SELECTOR, ALPHA_TRANSPARENT, mInvalidateUpdateListener);
        mHoursToMinutesAnims->Add(animator);

        animator = GetRadiusReappearAnimator(TO_IINTERFACE(this),
                String("animationRadiusMultiplierMinutes"), mInvalidateUpdateListener,
                mTransitionMidRadiusMultiplier, mTransitionEndRadiusMultiplier);
        mHoursToMinutesAnims->Add(animator);

        animator = GetFadeInAnimator((*mAlpha)[MINUTES],
                ALPHA_TRANSPARENT, ALPHA_OPAQUE, mInvalidateUpdateListener);
        mHoursToMinutesAnims->Add(animator);

        animator = GetFadeInAnimator((*((*mAlphaSelector)[MINUTES]))[SELECTOR_CIRCLE],
                ALPHA_TRANSPARENT, ALPHA_SELECTOR, mInvalidateUpdateListener);
        mHoursToMinutesAnims->Add(animator);

        animator = GetFadeInAnimator((*((*mAlphaSelector)[MINUTES]))[SELECTOR_DOT],
                ALPHA_TRANSPARENT, ALPHA_OPAQUE, mInvalidateUpdateListener);
        mHoursToMinutesAnims->Add(animator);

        animator = GetFadeInAnimator((*((*mAlphaSelector)[MINUTES]))[SELECTOR_LINE],
                ALPHA_TRANSPARENT, ALPHA_SELECTOR, mInvalidateUpdateListener);
        mHoursToMinutesAnims->Add(animator);
    }

    Boolean running = FALSE;
    if (mTransition != NULL && (IAnimator::Probe(mTransition)->IsRunning(&running), running)) {
        IAnimator::Probe(mTransition)->End();
    }
    CAnimatorSet::New((IAnimatorSet**)&mTransition);
    mTransition->PlayTogether(ICollection::Probe(mHoursToMinutesAnims));
    IAnimator::Probe(mTransition)->Start();
}

void RadialTimePickerView::StartMinutesToHoursAnimation()
{
    Int32 size = 0;
    if ((mMinuteToHoursAnims->GetSize(&size), size) == 0) {
        AutoPtr<IObjectAnimator> animator = GetRadiusDisappearAnimator(TO_IINTERFACE(this),
                String("animationRadiusMultiplierMinutes"), mInvalidateUpdateListener,
                mTransitionMidRadiusMultiplier, mTransitionEndRadiusMultiplier);
        mMinuteToHoursAnims->Add(animator);

        animator = GetFadeOutAnimator((*mAlpha)[MINUTES],
                ALPHA_OPAQUE, ALPHA_TRANSPARENT, mInvalidateUpdateListener);
        mMinuteToHoursAnims->Add(animator);

        animator = GetFadeOutAnimator((*((*mAlphaSelector)[MINUTES]))[SELECTOR_CIRCLE],
                ALPHA_SELECTOR, ALPHA_TRANSPARENT, mInvalidateUpdateListener);
        mMinuteToHoursAnims->Add(animator);

        animator = GetFadeOutAnimator((*((*mAlphaSelector)[MINUTES]))[SELECTOR_DOT],
                ALPHA_OPAQUE, ALPHA_TRANSPARENT, mInvalidateUpdateListener);
        mMinuteToHoursAnims->Add(animator);

        animator = GetFadeOutAnimator((*((*mAlphaSelector)[MINUTES]))[SELECTOR_LINE],
                ALPHA_SELECTOR, ALPHA_TRANSPARENT, mInvalidateUpdateListener);
        mMinuteToHoursAnims->Add(animator);

        animator = GetRadiusReappearAnimator(TO_IINTERFACE(this),
                String("animationRadiusMultiplierHours"), mInvalidateUpdateListener,
                mTransitionMidRadiusMultiplier, mTransitionEndRadiusMultiplier);
        mMinuteToHoursAnims->Add(animator);

        animator = GetFadeInAnimator((*mAlpha)[HOURS],
                ALPHA_TRANSPARENT, ALPHA_OPAQUE, mInvalidateUpdateListener);
        mMinuteToHoursAnims->Add(animator);

        animator = GetFadeInAnimator((*((*mAlphaSelector)[HOURS]))[SELECTOR_CIRCLE],
                ALPHA_TRANSPARENT, ALPHA_SELECTOR, mInvalidateUpdateListener);
        mMinuteToHoursAnims->Add(animator);

        animator = GetFadeInAnimator((*((*mAlphaSelector)[HOURS]))[SELECTOR_DOT],
                ALPHA_TRANSPARENT, ALPHA_OPAQUE, mInvalidateUpdateListener);
        mMinuteToHoursAnims->Add(animator);

        animator = GetFadeInAnimator((*((*mAlphaSelector)[HOURS]))[SELECTOR_LINE],
                ALPHA_TRANSPARENT, ALPHA_SELECTOR, mInvalidateUpdateListener);
        mMinuteToHoursAnims->Add(animator);
    }

    Boolean running = FALSE;
    if (mTransition != NULL && (IAnimator::Probe(mTransition)->IsRunning(&running), running)) {
        IAnimator::Probe(mTransition)->End();
    }
    CAnimatorSet::New((IAnimatorSet**)&mTransition);
    mTransition->PlayTogether(ICollection::Probe(mMinuteToHoursAnims));
    IAnimator::Probe(mTransition)->Start();
}

Int32 RadialTimePickerView::GetDegreesFromXY(
    /* [in] */ Float x,
    /* [in] */ Float y)
{
    const Double hypotenuse = Elastos::Core::Math::Sqrt(
            (y - mYCenter) * (y - mYCenter) + (x - mXCenter) * (x - mXCenter));

    // Basic check if we're outside the range of the disk
    if (hypotenuse > mCircleRadius[HOURS]) {
        return -1;
    }
    // Check
    if (mIs24HourMode && mShowHours) {
        if (hypotenuse >= mMinHypotenuseForInnerNumber
                && hypotenuse <= mHalfwayHypotenusePoint) {
            mIsOnInnerCircle = TRUE;
        }
        else if (hypotenuse <= mMaxHypotenuseForOuterNumber
                && hypotenuse >= mHalfwayHypotenusePoint) {
            mIsOnInnerCircle = FALSE;
        }
        else {
            return -1;
        }
    }
    else {
        const Int32 index =  (mShowHours) ? HOURS : MINUTES;
        const Float length = (mCircleRadius[index] * mNumbersRadiusMultiplier[index]);
        const Int32 distanceToNumber = (Int32) Elastos::Core::Math::Abs(hypotenuse - length);
        const Int32 maxAllowedDistance =
                (Int32) (mCircleRadius[index] * (1 - mNumbersRadiusMultiplier[index]));
        if (distanceToNumber > maxAllowedDistance) {
            return -1;
        }
    }

    const Float opposite = Elastos::Core::Math::Abs(y - mYCenter);
    Double degrees = Elastos::Core::Math::ToDegrees(Elastos::Core::Math::Asin(opposite / hypotenuse));

    // Now we have to translate to the correct quadrant.
    Boolean rightSide = (x > mXCenter);
    Boolean topSide = (y < mYCenter);
    if (rightSide && topSide) {
        degrees = 90 - degrees;
    }
    else if (rightSide && !topSide) {
        degrees = 90 + degrees;
    }
    else if (!rightSide && !topSide) {
        degrees = 270 - degrees;
    }
    else if (!rightSide && topSide) {
        degrees = 270 + degrees;
    }
    return (Int32) degrees;
}

Int32 RadialTimePickerView::GetIsTouchingAmOrPm(
    /* [in] */ Float x,
    /* [in] */ Float y)
{
    Boolean isLayoutRtl = FALSE;
    IsLayoutRtl(&isLayoutRtl);
    Int32 squaredYDistance = (Int32) ((y - mAmPmYCenter) * (y - mAmPmYCenter));

    Int32 distanceToAmCenter = (Int32) Elastos::Core::Math::Sqrt(
            (x - mLeftIndicatorXCenter) * (x - mLeftIndicatorXCenter) + squaredYDistance);
    if (distanceToAmCenter <= mAmPmCircleRadius) {
        return (isLayoutRtl ? PM : AM);
    }

    Int32 distanceToPmCenter = (Int32) Elastos::Core::Math::Sqrt(
            (x - mRightIndicatorXCenter) * (x - mRightIndicatorXCenter) + squaredYDistance);
    if (distanceToPmCenter <= mAmPmCircleRadius) {
        return (isLayoutRtl ? AM : PM);
    }

    // Neither was close enough.
    return -1;
}

ECode RadialTimePickerView::OnTouch(
    /* [in] */ IView* v,
    /* [in] */ IMotionEvent* event,
    /* [out] */ Boolean* resValue)
{
    VALIDATE_NOT_NULL(resValue);
    if(!mInputEnabled) {
        *resValue = TRUE;
        return NOERROR;
    }

    Float eventX = 0;
    event->GetX(&eventX);
    Float eventY = 0;
    event->GetY(&eventY);

    Int32 degrees;
    Int32 snapDegrees;
    Boolean result = FALSE, tmp = FALSE;

    Int32 action = 0, value = 0;
    switch(event->GetAction(&action), action) {
        case IMotionEvent::ACTION_DOWN:
        case IMotionEvent::ACTION_MOVE:
            mAmOrPmPressed = GetIsTouchingAmOrPm(eventX, eventY);
            if (mAmOrPmPressed != -1) {
                result = TRUE;
            }
            else {
                degrees = GetDegreesFromXY(eventX, eventY);
                if (degrees != -1) {
                    snapDegrees = (mShowHours ?
                            SnapOnly30s(degrees, 0) : SnapPrefer30s(degrees)) % 360;
                    if (mShowHours) {
                        mSelectionDegrees[HOURS] = snapDegrees;
                        mSelectionDegrees[HOURS_INNER] = snapDegrees;
                    }
                    else {
                        mSelectionDegrees[MINUTES] = snapDegrees;
                    }
                    PerformHapticFeedback(IHapticFeedbackConstants::CLOCK_TICK, &tmp);
                    if (mListener != NULL) {
                        if (mShowHours) {
                            GetCurrentHour(&value);
                            mListener->OnValueSelected(HOURS, value, FALSE);
                        }
                        else  {
                            GetCurrentMinute(&value);
                            mListener->OnValueSelected(MINUTES, value, FALSE);
                        }
                    }
                    result = TRUE;
                }
            }
            Invalidate();
            *resValue = result;
            return NOERROR;

        case IMotionEvent::ACTION_UP:
            mAmOrPmPressed = GetIsTouchingAmOrPm(eventX, eventY);
            if (mAmOrPmPressed != -1) {
                if (mAmOrPm != mAmOrPmPressed) {
                    SwapAmPm();
                }
                mAmOrPmPressed = -1;
                if (mListener != NULL) {
                    GetCurrentHour(&value);
                    mListener->OnValueSelected(AMPM, value, TRUE);
                }
                result = TRUE;
            }
            else {
                degrees = GetDegreesFromXY(eventX, eventY);
                if (degrees != -1) {
                    snapDegrees = (mShowHours ?
                            SnapOnly30s(degrees, 0) : SnapPrefer30s(degrees)) % 360;
                    if (mShowHours) {
                        mSelectionDegrees[HOURS] = snapDegrees;
                        mSelectionDegrees[HOURS_INNER] = snapDegrees;
                    }
                    else {
                        mSelectionDegrees[MINUTES] = snapDegrees;
                    }
                    if (mListener != NULL) {
                        if (mShowHours) {
                            GetCurrentHour(&value);
                            mListener->OnValueSelected(HOURS, value, TRUE);
                        }
                        else  {
                            GetCurrentMinute(&value);
                            mListener->OnValueSelected(MINUTES, value, TRUE);
                        }
                    }
                    result = TRUE;
                }
            }
            if (result) {
                Invalidate();
            }
            *resValue = result;
            return NOERROR;

        default:
            break;
    }
    *resValue = FALSE;
    return NOERROR;
}

ECode RadialTimePickerView::OnInitializeAccessibilityNodeInfo(
    /* [in] */ IAccessibilityNodeInfo* info)
{
    View::OnInitializeAccessibilityNodeInfo(info);
    info->AddAction(IAccessibilityNodeInfo::ACTION_SCROLL_FORWARD);
    info->AddAction(IAccessibilityNodeInfo::ACTION_SCROLL_BACKWARD);
    return NOERROR;
}

ECode RadialTimePickerView::DispatchPopulateAccessibilityEvent(
    /* [in] */ IAccessibilityEvent* event,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    Int32 type = 0;
    if ((event->GetEventType(&type), type) == IAccessibilityEvent::TYPE_WINDOW_STATE_CHANGED) {
        // Clear the event's current text so that only the current time will be spoken.
        AutoPtr<IList> list;
        IAccessibilityRecord::Probe(event)->GetText((IList**)&list);
        list->Clear();
        AutoPtr<ITime> time;
        CTime::New((ITime**)&time);
        Int32 value = 0;
        GetCurrentHour(&value);
        time->SetHour(value);
        GetCurrentMinute(&value);
        time->SetMinute(value);
        Int64 millis = 0;
        time->Normalize(TRUE, &millis);
        Int32 flags = IDateUtils::FORMAT_SHOW_TIME;
        if (mIs24HourMode) {
            flags |= IDateUtils::FORMAT_24HOUR;
        }
        AutoPtr<IContext> context;
        GetContext((IContext**)&context);
        String timeString = DateUtils::FormatDateTime(context, millis, flags);
        AutoPtr<ICharSequence> seq;
        CString::New(timeString, (ICharSequence**)&seq);
        list->Add(seq);
        *result = TRUE;
        return NOERROR;
    }
    return View::DispatchPopulateAccessibilityEvent(event, result);
}

ECode RadialTimePickerView::PerformAccessibilityAction(
    /* [in] */ Int32 action,
    /* [in] */ IBundle* arguments,
    /* [out] */ Boolean* result)
{
    Boolean tmp = FALSE;
    if (View::PerformAccessibilityAction(action, arguments, &tmp), tmp) {
        *result = TRUE;
        return NOERROR;
    }

    Int32 changeMultiplier = 0;
    if (action == IAccessibilityNodeInfo::ACTION_SCROLL_FORWARD) {
        changeMultiplier = 1;
    }
    else if (action == IAccessibilityNodeInfo::ACTION_SCROLL_BACKWARD) {
        changeMultiplier = -1;
    }

    if (changeMultiplier != 0) {
        Int32 value = 0, iValue = 0;
        Int32 stepSize = 0;
        if (mShowHours) {
            stepSize = DEGREES_FOR_ONE_HOUR;
            GetCurrentHour(&iValue);
            value = iValue % 12;
        }
        else {
            stepSize = DEGREES_FOR_ONE_MINUTE;
            GetCurrentMinute(&value);
        }

        Int32 degrees = value * stepSize;
        degrees = SnapOnly30s(degrees, changeMultiplier);
        value = degrees / stepSize;
        Int32 maxValue = 0;
        Int32 minValue = 0;
        if (mShowHours) {
            if (mIs24HourMode) {
                maxValue = 23;
            }
            else {
                maxValue = 12;
                minValue = 1;
            }
        }
        else {
            maxValue = 55;
        }
        if (value > maxValue) {
            // If we scrolled forward past the highest number, wrap around to the lowest.
            value = minValue;
        }
        else if (value < minValue) {
            // If we scrolled backward past the lowest number, wrap around to the highest.
            value = maxValue;
        }
        if (mShowHours) {
            SetCurrentHour(value);
            if (mListener != NULL) {
                mListener->OnValueSelected(HOURS, value, FALSE);
            }
        }
        else {
            SetCurrentMinute(value);
            if (mListener != NULL) {
                mListener->OnValueSelected(MINUTES, value, FALSE);
            }
        }
        *result = TRUE;
        return NOERROR;
    }

    *result = FALSE;
    return NOERROR;
}

ECode RadialTimePickerView::SetInputEnabled(
    /* [in] */ Boolean inputEnabled)
{
    mInputEnabled = inputEnabled;
    Invalidate();
    return NOERROR;
}

}// namespace Widget
}// namespace Droid
}// namespace Elastos
