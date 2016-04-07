
#ifndef __ELASTOS_DROID_WIDGET_RADIATIMEPICKERVIEW_H__
#define __ELASTOS_DROID_WIDGET_RADIATIMEPICKERVIEW_H__

#include "Elastos.Droid.Widget.h"
#include "elastos/droid/view/View.h"
#include "Elastos.Droid.Animation.h"
#include <Elastos.CoreLibrary.Utility.h>

using Elastos::Droid::Animation::IAnimatorSet;
using Elastos::Droid::Animation::IObjectAnimator;
using Elastos::Droid::Animation::IValueAnimator;
using Elastos::Droid::Animation::IAnimatorUpdateListener;
using Elastos::Droid::Graphics::ITypeface;
using Elastos::Droid::View::Accessibility::IAccessibilityEvent;
using Elastos::Droid::View::IMotionEvent;
using Elastos::Droid::View::IViewOnTouchListener;
using Elastos::Droid::View::IView;

namespace Elastos {
namespace Droid {
namespace Widget {

using Elastos::Droid::View::View;

/**
 * View to show a clock circle picker (with one or two picking circles)
 *
 * @hide
 */
class RadialTimePickerView
    : public Elastos::Droid::View::View
    , public IRadialTimePickerView
    , public IViewOnTouchListener
{
private:
    class InvalidateUpdateListener
        : public Object
        , public IAnimatorUpdateListener
    {
    public:
        CAR_INTERFACE_DECL();

        InvalidateUpdateListener(
            /* [in] */ RadialTimePickerView* host);

        // @Override
        CARAPI OnAnimationUpdate(
            /* [in] */ IValueAnimator* animation);

    private:
        RadialTimePickerView* mHost;
    };

    class Int32Holder: public Object
    {
    public:
        Int32Holder(
            /* [in] */ Int32 value);

        CARAPI_(void) SetValue(
            /* [in] */ Int32 value);

        CARAPI_(Int32) GetValue();

    private:
        Int32 mValue;
    };

    class RadialTimePickerViewOnTouchListener
        : public Object
        , public IViewOnTouchListener
    {
    public:
        CAR_INTERFACE_DECL();

        RadialTimePickerViewOnTouchListener(
            /* [in] */ RadialTimePickerView* host);

        CARAPI OnTouch(
            /* [in] */ IView* v,
            /* [in] */ IMotionEvent* event,
            /* [out] */ Boolean* resValue);

    private:
        RadialTimePickerView* mHost;
    };

public:
    CAR_INTERFACE_DECL();

    RadialTimePickerView();

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs);

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs,
        /* [in] */ Int32 defStyle);

    /**
     * Measure the view to end up as a square, based on the minimum of the height and width.
     */
    // @Override
    CARAPI_(void) OnMeasure(
        /* [in] */ Int32 widthMeasureSpec,
        /* [in] */ Int32 heightMeasureSpec);

    CARAPI Initialize(
        /* [in] */ Int32 hour,
        /* [in] */ Int32 minute,
        /* [in] */ Boolean is24HourMode);

    CARAPI SetCurrentItemShowing(
        /* [in] */ Int32 item,
        /* [in] */ Boolean animate);

    CARAPI GetCurrentItemShowing(
        /* [out] */ Int32* showing);

    CARAPI SetOnValueSelectedListener(
        /* [in] */ IOnValueSelectedListener* listener);

    /**
     * Sets the current hour in 24-hour time.
     *
     * @param hour the current hour between 0 and 23 (inclusive)
     */
    CARAPI SetCurrentHour(
        /* [in] */ Int32 hour);

    /**
     * Returns the current hour in 24-hour time.
     *
     * @return the current hour between 0 and 23 (inclusive)
     */
    CARAPI GetCurrentHour(
        /* [out] */ Int32* result);

    CARAPI SetCurrentMinute(
        /* [in] */ Int32 minute);

    // Returns minutes in 0-59 range
    CARAPI GetCurrentMinute(
        /* [out] */ Int32* minute);

    CARAPI SetAmOrPm(
        /* [in] */ Int32 val);

    CARAPI GetAmOrPm(
        /* [out] */ Int32* amOrPm);

    CARAPI SwapAmPm();

    CARAPI ShowHours(
        /* [in] */ Boolean animate);

    CARAPI ShowMinutes(
        /* [in] */ Boolean animate);

    // @Override
    CARAPI_(void) OnDraw(
        /* [in] */ ICanvas* canvas);

    // @Override
    CARAPI OnTouch(
        /* [in] */ IView* v,
        /* [in] */ IMotionEvent* event,
        /* [out] */ Boolean* resValue);

    /**
     * Necessary for accessibility, to ensure we support "scrolling" forward and backward
     * in the circle.
     */
    // @Override
    CARAPI OnInitializeAccessibilityNodeInfo(
        /* [in] */ IAccessibilityNodeInfo* info);

    /**
     * Announce the currently-selected time when launched.
     */
    // @Override
    CARAPI DispatchPopulateAccessibilityEvent(
        /* [in] */ IAccessibilityEvent* event,
        /* [out] */ Boolean* result);

    /**
     * When scroll forward/backward events are received, jump the time to the higher/lower
     * discrete, visible value on the circle.
     */
    // @SuppressLint("NewApi")
    // @Override
    CARAPI PerformAccessibilityAction(
        /* [in] */ Int32 action,
        /* [in] */ IBundle* arguments,
        /* [out] */ Boolean* result);

    CARAPI SetInputEnabled(
        /* [in] */ Boolean inputEnabled);

protected:
    // @Override
    CARAPI OnLayout(
        /* [in] */ Boolean changed,
        /* [in] */ Int32 left,
        /* [in] */ Int32 top,
        /* [in] */ Int32 right,
        /* [in] */ Int32 bottom);

private:
    static CARAPI_(Boolean) InitStatic();

    /**
     * Split up the 360 degrees of the circle among the 60 selectable values. Assigns a larger
     * selectable area to each of the 12 visible values, such that the ratio of space apportioned
     * to a visible value : space apportioned to a non-visible value will be 14 : 4.
     * E.g. the output of 30 degrees should have a higher range of input associated with it than
     * the output of 24 degrees, because 30 degrees corresponds to a visible number on the clock
     * circle (5 on the minutes, 1 or 13 on the hours).
     */
    static CARAPI_(void) PreparePrefer30sMap();

    /**
     * Returns mapping of any input degrees (0 to 360) to one of 60 selectable output degrees,
     * where the degrees corresponding to visible numbers (i.e. those divisible by 30) will be
     * weighted heavier than the degrees corresponding to non-visible numbers.
     * See {@link #preparePrefer30sMap()} documentation for the rationale and generation of the
     * mapping.
     */
    static CARAPI_(Int32) SnapPrefer30s(
        /* [in] */ Int32 degrees);

    /**
     * Returns mapping of any input degrees (0 to 360) to one of 12 visible output degrees (all
     * multiples of 30), where the input will be "snapped" to the closest visible degrees.
     * @param degrees The input degrees
     * @param forceHigherOrLower The output may be forced to either the higher or lower step, or may
     * be allowed to snap to whichever is closer. Use 1 to force strictly higher, -1 to force
     * strictly lower, and 0 to snap to the closer one.
     * @return output degrees, will be a multiple of 30
     */
    static CARAPI_(Int32) SnapOnly30s(
        /* [in] */ Int32 degrees,
        /* [in] */ Int32 forceHigherOrLower);

    CARAPI_(void) InitHoursAndMinutesText();

    CARAPI_(void) InitData();

    CARAPI_(void) UpdateLayoutData();

    CARAPI_(void) DrawCircleBackground(
        /* [in] */ ICanvas* canvas);

    CARAPI_(void) DrawCenter(
        /* [in] */ ICanvas* canvas);

    CARAPI_(void) DrawSelector(
        /* [in] */ ICanvas* canvas);

    CARAPI_(void) DrawAmPm(
        /* [in] */ ICanvas* canvas);

    CARAPI_(Int32) GetMultipliedAlpha(
        /* [in] */ Int32 argb,
        /* [in] */ Int32 alpha);

    CARAPI_(void) DrawSelector(
        /* [in] */ ICanvas* canvas,
        /* [in] */ Int32 index);

    CARAPI_(void) DrawDebug(
        /* [in] */ ICanvas* canvas);

    CARAPI_(void) CalculateGridSizesHours();

    CARAPI_(void) CalculateGridSizesMinutes();

    /**
     * Using the trigonometric Unit Circle, calculate the positions that the text will need to be
     * drawn at based on the specified circle radius. Place the values in the textGridHeights and
     * textGridWidths parameters.
     */
    static CARAPI_(void) CalculateGridSizes(
        /* [in] */ IPaint* paint,
        /* [in] */ Float numbersRadius,
        /* [in] */ Float xCenter,
        /* [in] */ Float yCenter,
        /* [in] */ Float textSize,
        /* [in] */ Float* textGridHeights,
        /* [in] */ Float* textGridWidths);

    /**
     * Draw the 12 text values at the positions specified by the textGrid parameters.
     */
    CARAPI_(void) DrawTextElements(
        /* [in] */ ICanvas* canvas,
        /* [in] */ Float textSize,
        /* [in] */ ITypeface* typeface,
        /* [in] */ ArrayOf<String>* texts,
        /* [in] */ Float* textGridWidths,
        /* [in] */ Float* textGridHeights,
        /* [in] */ IPaint* paint,
        /* [in] */ Int32 color,
        /* [in] */ Int32 alpha);

    // Used for animating the hours by changing their radius
    CARAPI_(void) SetAnimationRadiusMultiplierHours(
        /* [in] */ Float animationRadiusMultiplier);

    // Used for animating the minutes by changing their radius
    CARAPI_(void) SetAnimationRadiusMultiplierMinutes(
        /* [in] */ Float animationRadiusMultiplier);

    static CARAPI_(AutoPtr<IObjectAnimator>) GetRadiusDisappearAnimator(
        /* [in] */ IInterface* target,
        /* [in] */ const String& radiusPropertyName,
        /* [in] */ InvalidateUpdateListener* updateListener,
        /* [in] */ Float midRadiusMultiplier,
        /* [in] */ Float endRadiusMultiplier);

    static CARAPI_(AutoPtr<IObjectAnimator>) GetRadiusReappearAnimator(
        /* [in] */ IInterface* target,
        /* [in] */ const String& radiusPropertyName,
        /* [in] */ InvalidateUpdateListener* updateListener,
        /* [in] */ Float midRadiusMultiplier,
        /* [in] */ Float endRadiusMultiplier);

    static CARAPI_(AutoPtr<IObjectAnimator>) GetFadeOutAnimator(
        /* [in] */ Int32Holder* target,
        /* [in] */ Int32 startAlpha,
        /* [in] */ Int32 endAlpha,
        /* [in] */ InvalidateUpdateListener* updateListener);

    static CARAPI_(AutoPtr<IObjectAnimator>) GetFadeInAnimator(
        /* [in] */ Int32Holder* target,
        /* [in] */ Int32 startAlpha,
        /* [in] */ Int32 endAlpha,
        /* [in] */ InvalidateUpdateListener* updateListener);

    CARAPI_(void) StartHoursToMinutesAnimation();

    CARAPI_(void) StartMinutesToHoursAnimation();

    CARAPI_(Int32) GetDegreesFromXY(
        /* [in] */ Float x,
        /* [in] */ Float y);

    CARAPI_(Int32) GetIsTouchingAmOrPm(
        /* [in] */ Float x,
        /* [in] */ Float y);

private:
    static Boolean sInit;
    static const String TAG;
    static const Boolean DEBUG;

    static const Int32 DEBUG_COLOR;
    static const Int32 DEBUG_TEXT_COLOR;
    static const Int32 DEBUG_STROKE_WIDTH;

    static const Int32 HOURS;
    static const Int32 MINUTES;
    static const Int32 HOURS_INNER;
    static const Int32 AMPM;

    static const Int32 SELECTOR_CIRCLE;
    static const Int32 SELECTOR_DOT;
    static const Int32 SELECTOR_LINE;

    static const Int32 AM;
    static const Int32 PM;

    // Opaque alpha level
    static const Int32 ALPHA_OPAQUE;

    // Transparent alpha level
    static const Int32 ALPHA_TRANSPARENT;

    // Alpha level of color for selector.
    static const Int32 ALPHA_SELECTOR;

    // Alpha level of color for selected circle.
    static const Int32 ALPHA_AMPM_SELECTED;

    // Alpha level of color for pressed circle.
    static const Int32 ALPHA_AMPM_PRESSED;

    static const Float COSINE_30_DEGREES;
    static const Float SINE_30_DEGREES;

    static const Int32 DEGREES_FOR_ONE_HOUR;
    static const Int32 DEGREES_FOR_ONE_MINUTE;

    static const Int32 HOURS_NUMBERS[];
    static const Int32 HOURS_NUMBERS_24[];
    static const Int32 MINUTES_NUMBERS[];

    static const Int32 CENTER_RADIUS;

    static AutoPtr<ArrayOf<Int32> > STATE_SET_SELECTED;

    static AutoPtr<ArrayOf<Int32> > sSnapPrefer30sMap;

    AutoPtr<ArrayOf<String> > mHours12Texts;
    AutoPtr<ArrayOf<String> > mOuterHours24Texts;
    AutoPtr<ArrayOf<String> > mInnerHours24Texts;
    AutoPtr<ArrayOf<String> > mMinutesTexts;

    AutoPtr<ArrayOf<String> > mAmPmText;

    AutoPtr<ArrayOf<IPaint*> > mPaint;
    Int32 mColor[2];
    AutoPtr<ArrayOf<Int32Holder*> > mAlpha;

    AutoPtr<IPaint> mPaintCenter;

    AutoPtr<ArrayOf<ArrayOf<IPaint*>*> > mPaintSelector;
    Int32 mColorSelector[2][3];
    AutoPtr<ArrayOf<ArrayOf<Int32Holder*>* > > mAlphaSelector;

    AutoPtr<IPaint> mPaintAmPmText;
    AutoPtr<ArrayOf<IPaint*> > mPaintAmPmCircle;

    AutoPtr<IPaint> mPaintBackground;
    AutoPtr<IPaint> mPaintDisabled;
    AutoPtr<IPaint> mPaintDebug;

    AutoPtr<ITypeface> mTypeface;

    Boolean mIs24HourMode;
    Boolean mShowHours;

    /**
     * When in 24-hour mode, indicates that the current hour is between
     * 1 and 12 (inclusive).
     */
    Boolean mIsOnInnerCircle;

    Int32 mXCenter;
    Int32 mYCenter;

    Float mCircleRadius[3];

    Int32 mMinHypotenuseForInnerNumber;
    Int32 mMaxHypotenuseForOuterNumber;
    Int32 mHalfwayHypotenusePoint;

    Float mTextSize[2];
    Float mInnerTextSize;

    Float mTextGridHeights[2][7];
    Float mTextGridWidths[2][7];

    Float mInnerTextGridHeights[7];
    Float mInnerTextGridWidths[7];

    AutoPtr<ArrayOf<String> > mOuterTextHours;
    AutoPtr<ArrayOf<String> > mInnerTextHours;
    AutoPtr<ArrayOf<String> > mOuterTextMinutes;

    Float mCircleRadiusMultiplier[2];
    Float mNumbersRadiusMultiplier[3];

    Float mTextSizeMultiplier[3];

    Float mAnimationRadiusMultiplier[3];

    Float mTransitionMidRadiusMultiplier;
    Float mTransitionEndRadiusMultiplier;

    AutoPtr<IAnimatorSet> mTransition;
    AutoPtr<InvalidateUpdateListener> mInvalidateUpdateListener;

    Int32 mLineLength[3];
    Int32 mSelectionRadius[3];
    Float mSelectionRadiusMultiplier;
    Int32 mSelectionDegrees[3];

    Int32 mAmPmCircleRadius;
    Float mAmPmYCenter;

    Float mAmPmCircleRadiusMultiplier;
    Int32 mAmPmTextColor;

    Float mLeftIndicatorXCenter;
    Float mRightIndicatorXCenter;

    Int32 mAmPmUnselectedColor;
    Int32 mAmPmSelectedColor;

    Int32 mAmOrPm;
    Int32 mAmOrPmPressed;

    Int32 mDisabledAlpha;

    AutoPtr<IRectF> mRectF;
    Boolean mInputEnabled;
    AutoPtr<IOnValueSelectedListener> mListener;

    AutoPtr<IArrayList> mHoursToMinutesAnims; /*<Animator*/
    AutoPtr<IArrayList> mMinuteToHoursAnims; /*<Animator*/
};

}// namespace Widget
}// namespace Droid
}// namespace Elastos

DEFINE_CONVERSION_FOR(Elastos::Droid::Widget::RadialTimePickerView::Int32Holder, IInterface);

#endif // __ELASTOS_DROID_WIDGET_RADIATIMEPICKERVIEW_H__
