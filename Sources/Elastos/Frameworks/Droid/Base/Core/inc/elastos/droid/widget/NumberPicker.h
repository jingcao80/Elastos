#ifndef __ELASTOS_DROID_WIDGET_NUMBERPICKER_H__
#define __ELASTOS_DROID_WIDGET_NUMBERPICKER_H__

#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/view/VelocityTracker.h"
#include "elastos/droid/view/accessibility/AccessibilityNodeProvider.h"
#include "elastos/droid/text/method/NumberKeyListener.h"
#include "elastos/droid/widget/EditText.h"
#include "elastos/droid/widget/LinearLayout.h"
#include <elastos/core/StringBuilder.h>

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Graphics::IRect;
using Elastos::Droid::Graphics::ICanvas;
using Elastos::Droid::Graphics::IPaint;
using Elastos::Droid::Graphics::Drawable::IDrawable;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Text::ISpanned;
using Elastos::Droid::Text::Method::NumberKeyListener;
using Elastos::Droid::Utility::IAttributeSet;
using Elastos::Droid::Utility::ISparseArray;
using Elastos::Droid::View::Accessibility::IAccessibilityNodeProvider;
using Elastos::Droid::View::Accessibility::AccessibilityNodeProvider;
using Elastos::Droid::View::Accessibility::IAccessibilityNodeInfo;
using Elastos::Droid::View::IViewOnClickListener;
using Elastos::Droid::View::IViewOnLongClickListener;
using Elastos::Droid::View::IViewOnFocusChangeListener;
using Elastos::Droid::View::IMotionEvent;
using Elastos::Droid::View::IKeyEvent;
using Elastos::Droid::View::Accessibility::IAccessibilityEvent;
using Elastos::Droid::View::VelocityTracker;
using Elastos::Droid::Widget::EditText;
using Elastos::Droid::Widget::IEditText;
using Elastos::Droid::Widget::IImageButton;
using Elastos::Droid::Widget::INumberPickerFormatter;
using Elastos::Droid::Widget::INumberPickerOnValueChangeListener;
using Elastos::Droid::Widget::INumberPickerOnScrollListener;
using Elastos::Droid::Widget::IScroller;
using Elastos::Core::ICharSequence;
using Elastos::Core::StringBuilder;
using Elastos::Utility::IFormatter;

namespace Elastos {
namespace Droid {
namespace Widget {

class NumberPicker
    : public LinearLayout
    , public INumberPicker
{
public:
    class CustomEditText
        : public EditText
        , public INumberPickerCustomEditText
    {
    public:
        CustomEditText();

        ~CustomEditText();

        CAR_INTERFACE_DECL();

        CARAPI constructor(
            /* [in] */ IContext* context,
            /* [in] */ IAttributeSet* attrs);

        CARAPI OnEditorAction(
            /* [in] */ Int32 actionCode);
    };

protected:
    class InputTextFilter
        : public NumberKeyListener
    {
    public:
        InputTextFilter(
            /* [in] */ NumberPicker* host);

        CARAPI GetInputType(
            /* [out] */ Int32* type);

        CARAPI Filter(
            /* [in] */ ICharSequence* source,
            /* [in] */ Int32 start,
            /* [in] */ Int32 end,
            /* [in] */ ISpanned* dest,
            /* [in] */ Int32 dstart,
            /* [in] */ Int32 dend,
            /* [out] */ ICharSequence** sou);

        // @override
        CARAPI ClearMetaKeyState(
            /* [in] */ IView* view,
            /* [in] */ IEditable* content,
            /* [in] */ Int32 states);

        // @override
        CARAPI OnKeyUp(
            /* [in] */ IView* view,
            /* [in] */ IEditable* content,
            /* [in] */ Int32 keyCode,
            /* [in] */ IKeyEvent* event,
            /* [out] */ Boolean* ret);

    protected:
        CARAPI_(AutoPtr<ArrayOf<Char32> >) GetAcceptedChars();

    private:
        NumberPicker* mHost;
    };

    class PressedStateHelper
        : public Runnable
    {
    public:
        PressedStateHelper(
            /* [in] */ NumberPicker* host);

        CARAPI_(void) Cancel();

        CARAPI_(void) ButtonPressDelayed(
            /* [in] */ Int32 button);

        CARAPI_(void) ButtonTapped(
            /* [in] */ Int32 button);

        CARAPI Run();

    public:
        const static Int32 BUTTON_INCREMENT = 1;
        const static Int32 BUTTON_DECREMENT = 2;

    private:
        const static Int32 MODE_PRESS = 1;
        const static Int32 MODE_TAPPED = 2;

        Int32 mManagedButton;
        Int32 mMode;

        NumberPicker* mHost;
    };

    class SetSelectionCommand
        : public Runnable
    {
    friend class NumberPicker;

    public:
        SetSelectionCommand(
            /* [in] */ INumberPicker* host);

        CARAPI Run();

    private:
        Int32 mSelectionStart;
        Int32 mSelectionEnd;

        AutoPtr<IWeakReference> mWeakHost;
    };

    class ChangeCurrentByOneFromLongPressCommand
        : public Runnable
    {
    public:
        ChangeCurrentByOneFromLongPressCommand(
            /* [in] */ NumberPicker* host);

        CARAPI_(void) SetStep(
            /* [in] */ Boolean increment);

        CARAPI Run();

    private:
        Boolean mIncrement;

        NumberPicker* mHost;
    };

    class BeginSoftInputOnLongPressCommand
        : public Runnable
    {
    public:
        BeginSoftInputOnLongPressCommand(
            /* [in] */ NumberPicker* host);

        CARAPI Run();

    private:
        NumberPicker* mHost;
    };

    class AccessibilityNodeProviderImpl
        : public AccessibilityNodeProvider
    {
    public:
        AccessibilityNodeProviderImpl(
            /* [in] */ NumberPicker* host);

        //@Override
        CARAPI CreateAccessibilityNodeInfo(
            /* [in] */ Int32 virtualViewId,
            /* [out] */ IAccessibilityNodeInfo** info);

        //@Override
        CARAPI FindAccessibilityNodeInfosByText(
            /* [in] */ const String& searched,
            /* [in] */ Int32 virtualViewId,
            /* [out] */ IList** list);

        //@Override
        CARAPI PerformAction(
            /* [in] */ Int32 virtualViewId,
            /* [in] */ Int32 action,
            /* [in] */ IBundle* arguments,
            /* [out] */ Boolean* res);

        CARAPI_(void) SendAccessibilityEventForVirtualView(
            /* [in] */ Int32 virtualViewId,
            /* [in] */ Int32 eventType);

    private:
        CARAPI_(void) SendAccessibilityEventForVirtualText(
            /* [in] */ Int32 eventType);

        CARAPI_(void) SendAccessibilityEventForVirtualButton(
            /* [in] */ Int32 virtualViewId,
            /* [in] */ Int32 eventType,
            /* [in] */ const String& text);

        CARAPI_(void) FindAccessibilityNodeInfosByTextInChild(
            /* [in] */ const String& searchedLowerCase,
            /* [in] */ Int32 virtualViewId,
            /* [in] */ IList* outResult);

        CARAPI_(AutoPtr<IAccessibilityNodeInfo>) CreateAccessibiltyNodeInfoForInputText(
            /* [in] */ Int32 left,
            /* [in] */ Int32 top,
            /* [in] */ Int32 right,
            /* [in] */ Int32 bottom);

        CARAPI_(AutoPtr<IAccessibilityNodeInfo>) CreateAccessibilityNodeInfoForVirtualButton(
            /* [in] */ Int32 virtualViewId,
            /* [in] */ const String& text,
            /* [in] */ Int32 left,
            /* [in] */ Int32 top,
            /* [in] */ Int32 right,
            /* [in] */ Int32 bottom);

        CARAPI_(AutoPtr<IAccessibilityNodeInfo>) CreateAccessibilityNodeInfoForNumberPicker(
            /* [in] */ Int32 left,
            /* [in] */ Int32 top,
            /* [in] */ Int32 right,
            /* [in] */ Int32 bottom);

        CARAPI_(Boolean) HasVirtualDecrementButton();

        CARAPI_(Boolean) HasVirtualIncrementButton();

        CARAPI_(String) GetVirtualDecrementButtonText();

        CARAPI_(String) GetVirtualIncrementButtonText();

        static CARAPI_(String) FormatNumberWithLocale(
            /* [in] */ Int32 value);

    private:
        const static Int32 UNDEFINED; // = Math::INT32_MIN_VALUE;
        const static Int32 VIRTUAL_VIEW_ID_INCREMENT = 1;
        const static Int32 VIRTUAL_VIEW_ID_INPUT = 2;
        const static Int32 VIRTUAL_VIEW_ID_DECREMENT = 3;

        AutoPtr<IRect> mTempRect;
        AutoPtr<ArrayOf<Int32> > mTempArray;

        Int32 mAccessibilityFocusedView;

        NumberPicker* mHost;

    friend class NumberPicker;
    };

private:
    class TwoDigitFormatter
        : public Object
        , public INumberPickerFormatter
    {
    public:
        TwoDigitFormatter();

        CAR_INTERFACE_DECL();

        CARAPI Format(
            /* [in] */ Int32 value,
            /* [out] */ String* str);

    private:
        CARAPI Init(
            /* [in] */ ILocale* locale);

        static CARAPI_(Char32) GetZeroDigit(
            /* [in] */ ILocale* locale);

        CARAPI_(AutoPtr<IFormatter>) CreateFormatter(
            /* [in] */ ILocale* locale);

    private:
        AutoPtr<StringBuilder> mBuilder;
        Char32 mZeroDigit;
        AutoPtr<IFormatter> mFmt;
        AutoPtr< ArrayOf<IInterface*> > mArgs;
    };

    class NumberPickerOnClickListener
        : public Object
        , public IViewOnClickListener
    {
    public:
        NumberPickerOnClickListener(
            /* [in] */ NumberPicker* host);

        CAR_INTERFACE_DECL();

        CARAPI OnClick(
            /* [in] */ IView* v);
    private:
        NumberPicker* mHost;
    };

    class NumberPickerOnLongCliskListener
        : public Object
        , public IViewOnLongClickListener
    {
    public:
        NumberPickerOnLongCliskListener(
            /* [in] */ NumberPicker* host);

        CAR_INTERFACE_DECL();

        CARAPI OnLongClick(
            /* [in] */ IView* v,
            /* [out] */ Boolean* result);
    private:
        NumberPicker* mHost;
    };

    class NumberPickerOnFocusChangeListener
        : public Object
        , public IViewOnFocusChangeListener
    {
    public:
        NumberPickerOnFocusChangeListener(
            /* [in] */ NumberPicker* host);

        CAR_INTERFACE_DECL();

        CARAPI OnFocusChange(
            /* [in] */ IView* v,
            /* [in] */ Boolean hasFocus);
    private:
        NumberPicker* mHost;
    };

public:
    CAR_INTERFACE_DECL();

    NumberPicker();

    ~NumberPicker();

    /**
     * Create a new number picker.
     *
     * @param context The application environment.
     */
    CARAPI constructor(
        /* [in] */ IContext* context);

    /**
     * Create a new number picker.
     *
     * @param context The application environment.
     * @param attrs A collection of attributes.
     */
    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs);

    /**
     * Create a new number picker
     *
     * @param context the application environment.
     * @param attrs a collection of attributes.
     * @param defStyleAttr An attribute in the current theme that contains a
     *        reference to a style resource that supplies default values for
     *        the view. Can be 0 to not look for defaults.
     */
    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs,
        /* [in] */ Int32 defStyleAttr);

    /**
     * Create a new number picker
     *
     * @param context the application environment.
     * @param attrs a collection of attributes.
     * @param defStyleAttr An attribute in the current theme that contains a
     *        reference to a style resource that supplies default values for
     *        the view. Can be 0 to not look for defaults.
     * @param defStyleRes A resource identifier of a style resource that
     *        supplies default values for the view, used only if
     *        defStyleAttr is 0 or can not be found in the theme. Can be 0
     *        to not look for defaults.
     */
    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs,
        /* [in] */ Int32 defStyleAttr,
        /* [in] */ Int32 defStyleRes);

    static CARAPI_(AutoPtr<INumberPickerFormatter>) GetTwoDigitFormatter();

    // @Override
    CARAPI OnInterceptTouchEvent(
        /* [in] */ IMotionEvent* event,
        /* [out] */ Boolean* res);

    // @Override
    CARAPI OnTouchEvent(
        /* [in] */ IMotionEvent* event,
        /* [out] */ Boolean* res);

    // @Override
    CARAPI DispatchTouchEvent(
        /* [in] */ IMotionEvent* event,
        /* [out] */ Boolean* res);

    // @Override
    CARAPI DispatchKeyEvent(
        /* [in] */ IKeyEvent* event,
        /* [out] */ Boolean* res);

    // @Override
    CARAPI DispatchTrackballEvent(
        /* [in] */ IMotionEvent* event,
        /* [out] */ Boolean* res);

    // @Override
    CARAPI ComputeScroll();

    // @Override
    CARAPI SetEnabled(
        /* [in] */ Boolean enabled);

    // @Override
    CARAPI ScrollBy(
        /* [in] */ Int32 x,
        /* [in] */ Int32 y);

    // @Override
    CARAPI GetSolidColor(
        /* [out] */ Int32* color);

    CARAPI SetOnValueChangedListener(
        /* [in] */ INumberPickerOnValueChangeListener* onValueChangedListener);

    CARAPI SetOnScrollListener(
        /* [in] */ INumberPickerOnScrollListener* onScrollListener);

    CARAPI SetFormatter(
        /* [in] */ INumberPickerFormatter* formatter);

    CARAPI SetValue(
        /* [in] */ Int32 value);

    // @Override
    CARAPI PerformClick(
        /* [out] */ Boolean* res);

    // @Override
    CARAPI PerformLongClick(
        /* [out] */ Boolean* res);

    CARAPI GetWrapSelectorWheel(
        /* [out] */ Boolean* res);

    CARAPI SetWrapSelectorWheel(
        /* [in] */ Boolean wrapSelectorWheel);

    CARAPI SetOnLongPressUpdateInterval(
        /* [in] */ Int64 intervalMillis);

    CARAPI GetValue(
        /* [out] */ Int32* value);

    CARAPI GetMinValue(
        /* [out] */ Int32* minValue);

    CARAPI SetMinValue(
        /* [in] */ Int32 minValue);

    CARAPI GetMaxValue(
        /* [out] */ Int32* maxValue);

    CARAPI SetMaxValue(
        /* [in] */ Int32 maxValue);

    CARAPI GetDisplayedValues(
        /* [out, callee] */ ArrayOf<String>** displayedValues);

    CARAPI SetDisplayedValues(
        /* [in] */ ArrayOf<String>* displayedValues);

    // @Override
    CARAPI OnInitializeAccessibilityEvent(
        /* [in] */ IAccessibilityEvent* event);

    // Override
    CARAPI GetAccessibilityNodeProvider(
        /* [out] */ IAccessibilityNodeProvider** provider);

protected:
    // @Override
    virtual CARAPI OnLayout(
        /* [in] */ Boolean changed,
        /* [in] */ Int32 letf,
        /* [in] */ Int32 top,
        /* [in] */ Int32 right,
        /* [in] */ Int32 bottom);

    // @Override
    virtual CARAPI_(void) OnMeasure(
        /* [in] */ Int32 widthMeasureSpec,
        /* [in] */ Int32 heightMeasureSpec);

    // @Override
    virtual CARAPI_(Float) GetTopFadingEdgeStrength();

    // @Override
    virtual CARAPI_(Float) GetBottomFadingEdgeStrength();

    // @Override
    virtual CARAPI OnDetachedFromWindow();

    // @Override
    virtual CARAPI_(void) OnDraw(
        /* [in] */ ICanvas* canvas);

    // @Override
    virtual CARAPI_(Boolean) DispatchHoverEvent(
        /* [in] */ IMotionEvent* event);

    // @Override
    virtual CARAPI_(Int32) ComputeVerticalScrollRange();

    // @Override
    virtual CARAPI_(Int32) ComputeVerticalScrollOffset();

    // @Override
    virtual CARAPI_(Int32) ComputeVerticalScrollExtent();

private:
    CARAPI_(Boolean) MoveToFinalScrollerPosition(
        /* [in] */ IScroller* scroller);

    CARAPI_(void) ShowSoftInput();

    CARAPI_(void) HideSoftInput();

    CARAPI_(void) TryComputeMaxWidth();

    CARAPI_(Int32) MakeMeasureSpec(
        /* [in] */ Int32 measureSpec,
        /* [in] */ Int32 maxSize);

    CARAPI_(Int32) ResolveSizeAndStateRespectingMinSize(
        /* [in] */ Int32 minSize,
        /* [in] */ Int32 measuredSize,
        /* [in] */ Int32 measureSpec);

    CARAPI_(void) InitializeSelectorWheelIndices();

    CARAPI_(void) SetValueInternal(
        /* [in] */ Int32 current,
        /* [in] */ Boolean notifyChange);

    CARAPI_(void) ChangeValueByOne(
        /* [in] */ Boolean increment);

    CARAPI_(void) InitializeSelectorWheel();

    CARAPI_(void) InitializeFadingEdges();

    CARAPI_(void) OnScrollerFinished(
        /* [in] */ IScroller* scroller);

    CARAPI_(void) OnScrollStateChange(
        /* [in] */ Int32 scrollState);

    CARAPI_(void) Fling(
        /* [in] */ Int32 velocityY);

    CARAPI_(Int32) GetWrappedSelectorIndex(
        /* [in] */ Int32 selectorIndex);

    CARAPI_(void) IncrementSelectorIndices(
        /* [in] */ ArrayOf<Int32>* selectorIndices);

    CARAPI_(void) DecrementSelectorIndices(
        /* [in] */ ArrayOf<Int32>* selectorIndices);

    CARAPI_(void) EnsureCachedScrollSelectorValue(
        /* [in] */ Int32 selectorIndex);

    CARAPI_(String) FormatNumber(
        /* [in] */ Int32 value);

    CARAPI_(void) ValidateInputTextView(
        /* [in] */ IView* v);

    CARAPI_(Boolean) UpdateInputTextView();

    CARAPI_(void) NotifyChange(
        /* [in] */ Int32 previous,
        /* [in] */ Int32 current);

    CARAPI_(void) PostChangeCurrentByOneFromLongPress(
        /* [in] */ Boolean increment,
        /* [in] */ Int64 delayMillis);

    CARAPI_(void) RemoveChangeCurrentByOneFromLongPress();

    CARAPI_(void) PostBeginSoftInputOnLongPressCommand();

    CARAPI_(void) RemoveBeginSoftInputCommand();

    CARAPI_(void) RemoveAllCallbacks();

    CARAPI_(Int32) GetSelectedPos(
        /* [in] */ const String& value);

    CARAPI_(void) PostSetSelectionCommand(
        /* [in] */ Int32 selectionStart,
        /* [in] */ Int64 selectionEnd);

    CARAPI_(Boolean) EnsureScrollWheelAdjusted();

private:
    friend class InputTextFilter;

    const static String NUMBERPICKER_NAME;

    const static Int32 SELECTOR_WHEEL_ITEM_COUNT;

    const static Int64 DEFAULT_LONG_PRESS_UPDATE_INTERVAL;

    const static Int32 SELECTOR_MIDDLE_ITEM_INDEX;

    const static Int32 SELECTOR_MAX_FLING_VELOCITY_ADJUSTMENT;

    const static Int32 SELECTOR_ADJUSTMENT_DURATION_MILLIS;

    const static Int32 SNAP_SCROLL_DURATION;

    const static Float TOP_AND_BOTTOM_FADING_EDGE_STRENGTH;

    const static Int32 UNSCALED_DEFAULT_SELECTION_DIVIDER_HEIGHT;

    const static Int32 UNSCALED_DEFAULT_SELECTION_DIVIDERS_DISTANCE;

    const static Int32 DEFAULT_LAYOUT_RESOURCE_ID;

    const static Int32 SIZE_UNSPECIFIED;

    static AutoPtr<INumberPickerFormatter> sTwoDigitFormatter;

    const static Char32 DIGIT_CHARACTERS[];

    AutoPtr<IImageButton> mInCrementButton;

    AutoPtr<IImageButton> mDecrementButton;

    AutoPtr<IEditText> mInputText;

    Int32 mSelectionDividersDistance;

    Int32 mMinHeight;

    Int32 mMaxHeight;

    Int32 mMinWidth;

    Int32 mMaxWidth;

    Boolean mComputeMaxWidth;

    Int32 mTextSize;

    Int32 mSelectorTextGapHeight;

    AutoPtr<ArrayOf<String> > mDisplayedValues;

    Int32 mMinValue;

    Int32 mMaxValue;

    Int32 mValue;

    AutoPtr<INumberPickerOnValueChangeListener> mOnValueChangeListener;

    AutoPtr<INumberPickerOnScrollListener> mOnScrollListener;

    AutoPtr<INumberPickerFormatter> mFormatter;

    Int64 mLongPressUpdateInterval;

    AutoPtr<ISparseArray> mSelectorIndexToStringCache;

    AutoPtr<ArrayOf<Int32> > mSelectorIndices;

    AutoPtr<IPaint> mSelectorWheelPaint;

    AutoPtr<IDrawable> mVirtualButtonPressedDrawable;

    Int32 mSelectorElementHeight;

    Int32 mInitialScrollOffset;

    Int32 mCurrentScrollOffset;

    AutoPtr<IScroller> mFlingScroller;

    AutoPtr<IScroller> mAdjustScroller;

    Int32 mPreviousScrollerY;

    AutoPtr<SetSelectionCommand> mSetSelectionCommand;

    AutoPtr<ChangeCurrentByOneFromLongPressCommand> mChangeCurrentByOneFromLongPressCommand;

    AutoPtr<BeginSoftInputOnLongPressCommand> mBeginSoftInputOnLongPressCommand;

    Float mLastDownEventY;

    Int64 mLastDownEventTime;

    Float mLastDownOrMoveEventY;

    AutoPtr<VelocityTracker> mVelocityTracker;

    Int32 mTouchSlop;

    Int32 mMinimumFlingVelocity;

    Int32 mMaximumFlingVelocity;

    Boolean mWrapSelectorWheel;

    Int32 mSolidColor;

    Boolean mHasSelectorWheel;

    AutoPtr<IDrawable> mSelectionDivider;

    Int32 mSelectionDividerHeight;

    Int32 mScrollState;

    Boolean mIgnoreMoveEvents;

    Boolean mPerformClickOnTap;

    Int32 mTopSelectionDividerTop;

    Int32 mBottomSelectionDividerBottom;

    Int32 mLastHoveredChildVirtualViewId;

    Boolean mIncrementVirtualButtonPressed;

    Boolean mDecrementVirtualButtonPressed;

    AutoPtr<AccessibilityNodeProviderImpl> mAccessibilityNodeProvider;

    AutoPtr<PressedStateHelper> mPressedStateHelper;

    /**
     * The keycode of the last handled DPAD down event.
     */
    Int32 mLastHandledDownDpadKeyCode;

    /**
     * If true then the selector wheel is hidden until the picker has focus.
     */
    Boolean mHideWheelUntilFocused;
};

} // namespace Widget
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_WIDGET_NUMBERPICKER_H__
