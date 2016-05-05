
#ifndef __ELASTOS_DROID_WIDGET_TIMEPICKERSPINNERDELEGATE_H__
#define __ELASTOS_DROID_WIDGET_TIMEPICKERSPINNERDELEGATE_H__

#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/view/View.h"
#include "elastos/droid/widget/TimePicker.h"

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::Res::IConfiguration;
using Elastos::Droid::Utility::IAttributeSet;
using Elastos::Droid::View::Accessibility::IAccessibilityEvent;
using Elastos::Droid::View::Accessibility::IAccessibilityNodeInfo;
using Elastos::Droid::View::IKeyEvent;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::IViewOnClickListener;
using Elastos::Droid::View::IViewOnFocusChangeListener;
using Elastos::Droid::View::IViewOnKeyListener;
using Elastos::Droid::View::View;
using Elastos::Droid::Widget::IOnValueSelectedListener;
using Elastos::Droid::Widget::IRadialTimePickerView;
using Elastos::Droid::Widget::ITextView;
using Elastos::Droid::Widget::TimePicker;
using Elastos::Core::IInteger32;
using Elastos::Utility::ICalendar;
using Elastos::Utility::ILocale;

namespace Elastos {
namespace Droid {
namespace Widget {

/**
  * A view for selecting the time of day, in either 24 hour or AM/PM mode.
  */
class TimePickerSpinnerDelegate
    : public TimePicker::AbstractTimePickerDelegate
    , public ITimePickerSpinnerDelegate
    , public IOnValueSelectedListener
{
public:
    /**
      * Used to save / restore state of time picker
      */
    class SavedState
        : public Elastos::Droid::View::View::BaseSavedState
        , public ITimePickerSpinnerDelegateSavedState
    {
        friend class TimePickerSpinnerDelegate;
    public:
        CAR_INTERFACE_DECL()

        SavedState();

        ~SavedState();

        CARAPI constructor();

        CARAPI constructor(
            /* [in] */ IParcelable* superState,
            /* [in] */ Int32 hour,
            /* [in] */ Int32 minute,
            /* [in] */ Boolean is24HourMode,
            /* [in] */ Boolean isKbMode,
            /* [in] */ IArrayList* typedTimes,
            /* [in] */ Int32 currentItemShowing);

        virtual CARAPI_(Int32) GetHour();

        virtual CARAPI_(Int32) GetMinute();

        virtual CARAPI_(Boolean) Is24HourMode();

        virtual CARAPI_(Boolean) InKbMode();

        virtual CARAPI_(AutoPtr<IArrayList>) GetTypesTimes();

        virtual CARAPI_(Int32) GetCurrentItemShowing();

        // @Override
        CARAPI WriteToParcel(
            /* [in] */ IParcel* dest);

        // @Override
        CARAPI ReadFromParcel(
            /* [in] */ IParcel* source);

        // @Override
        CARAPI ToString(
            /* [out] */ String* str);

    private:
        /*const*/ Int32 mHour;
        /*const*/ Int32 mMinute;
        /*const*/ Boolean mIs24HourMode;
        /*const*/ Boolean mInKbMode;
        /*const*/ AutoPtr<IArrayList> mTypedTimes;
        /*const*/ Int32 mCurrentItemShowing;
    };

private:
    class InnerViewHourOnClickListener
        : public Object
        , public IViewOnClickListener
    {
    public:
        CAR_INTERFACE_DECL()

        InnerViewHourOnClickListener(
            /* [in] */ TimePickerSpinnerDelegate* owner);

        // @Override
        CARAPI OnClick(
            /* [in] */ IView* v);

    private:
        TimePickerSpinnerDelegate* mOwner;
    };

    class InnerViewMinuteOnClickListener
        : public Object
        , public IViewOnClickListener
    {
    public:
        CAR_INTERFACE_DECL()

        InnerViewMinuteOnClickListener(
            /* [in] */ TimePickerSpinnerDelegate* owner);

        // @Override
        CARAPI OnClick(
            /* [in] */ IView* v);

    private:
        TimePickerSpinnerDelegate* mOwner;
    };

    class InnerViewAmPmDisplayOnClickListener
        : public Object
        , public IViewOnClickListener
    {
    public:
        CAR_INTERFACE_DECL()

        InnerViewAmPmDisplayOnClickListener(
            /* [in] */ TimePickerSpinnerDelegate* owner);

        // @Override
        CARAPI OnClick(
            /* [in] */ IView* v);

    private:
        TimePickerSpinnerDelegate* mOwner;
    };

    /**
      * Simple node class to be used for traversal to check for legal times.
      * mLegalKeys represents the keys that can be typed to get to the node.
      * mChildren are the children that can be reached from this node.
      */
    class Node
        : public Object
    {
    public:
        Node(
            /* [in] */ ArrayOf<Int32>* legalKeys);

        virtual CARAPI AddChild(
            /* [in] */ Node* child);

        virtual CARAPI ContainsKey(
            /* [in] */ Int32 key,
            /* [out] */ Boolean* result);

        virtual CARAPI CanReach(
            /* [in] */ Int32 key,
            /* [out] */ Node** result);

    private:
        AutoPtr< ArrayOf<Int32> > mLegalKeys;
        AutoPtr<IArrayList> mChildren;
    };

    class InnerViewOnKeyListener
        : public Object
        , public IViewOnKeyListener
    {
    public:
        CAR_INTERFACE_DECL()

        InnerViewOnKeyListener(
            /* [in] */ TimePickerSpinnerDelegate* owner);

        // @Override
        CARAPI OnKey(
            /* [in] */ IView* v,
            /* [in] */ Int32 keyCode,
            /* [in] */ IKeyEvent* event,
            /* [out] */ Boolean* result);

    private:
        TimePickerSpinnerDelegate* mOwner;
    };

    class InnerViewOnFocusChangeListener
        : public Object
        , public IViewOnFocusChangeListener
    {
    public:
        CAR_INTERFACE_DECL()

        InnerViewOnFocusChangeListener(
            /* [in] */ TimePickerSpinnerDelegate* owner);

        // @Override
        CARAPI OnFocusChange(
            /* [in] */ IView* v,
            /* [in] */ Boolean hasFocus);

    private:
        TimePickerSpinnerDelegate* mOwner;
    };

public:
    CAR_INTERFACE_DECL()

    TimePickerSpinnerDelegate();

    CARAPI constructor(
        /* [in] */ ITimePicker* delegator,
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs,
        /* [in] */ Int32 defStyleAttr,
        /* [in] */ Int32 defStyleRes);

    /**
      * Set the current hour.
      */
    // @Override
    CARAPI SetCurrentHour(
        /* [in] */ Int32 currentHour);

    /**
      * @return The current hour in the range (0-23).
      */
    // @Override
    CARAPI GetCurrentHour(
        /* [out] */ Int32* result);

    /**
      * Set the current minute (0-59).
      */
    // @Override
    CARAPI SetCurrentMinute(
        /* [in] */ Int32 currentMinute);

    /**
      * @return The current minute.
      */
    // @Override
    CARAPI GetCurrentMinute(
        /* [out] */ Int32* result);

    /**
      * Set whether in 24 hour or AM/PM mode.
      *
      * @param is24HourView True = 24 hour mode. False = AM/PM.
      */
    // @Override
    CARAPI SetIs24HourView(
        /* [in] */ Boolean is24HourView);

    /**
      * @return true if this is in 24 hour view else false.
      */
    // @Override
    CARAPI Is24HourView(
        /* [out] */ Boolean* result);

    // @Override
    CARAPI SetOnTimeChangedListener(
        /* [in] */ ITimePickerOnTimeChangedListener* callback);

    // @Override
    CARAPI SetEnabled(
        /* [in] */ Boolean enabled);

    // @Override
    CARAPI IsEnabled(
        /* [out] */ Boolean* result);

    // @Override
    CARAPI GetBaseline(
        /* [out] */ Int32* result);

    // @Override
    CARAPI OnConfigurationChanged(
        /* [in] */ IConfiguration* newConfig);

    // @Override
    CARAPI OnSaveInstanceState(
        /* [in] */ IParcelable* superState,
        /* [out] */ IParcelable** result);

    // @Override
    CARAPI OnRestoreInstanceState(
        /* [in] */ IParcelable* state);

    // @Override
    CARAPI SetCurrentLocale(
        /* [in] */ ILocale* locale);

    // @Override
    CARAPI DispatchPopulateAccessibilityEvent(
        /* [in] */ IAccessibilityEvent* event,
        /* [out] */ Boolean* result);

    // @Override
    CARAPI OnPopulateAccessibilityEvent(
        /* [in] */ IAccessibilityEvent* event);

    // @Override
    CARAPI OnInitializeAccessibilityEvent(
        /* [in] */ IAccessibilityEvent* event);

    // @Override
    CARAPI OnInitializeAccessibilityNodeInfo(
        /* [in] */ IAccessibilityNodeInfo* info);

    /**
      * Called by the picker for updating the header display.
      */
    // @Override
    CARAPI OnValueSelected(
        /* [in] */ Int32 pickerIndex,
        /* [in] */ Int32 newValue,
        /* [in] */ Boolean autoAdvance);

private:
    CARAPI_(void) Initialize(
        /* [in] */ Int32 hourOfDay,
        /* [in] */ Int32 minute,
        /* [in] */ Boolean is24HourView,
        /* [in] */ Int32 index);

    CARAPI_(void) SetupListeners();

    CARAPI_(void) UpdateUI(
        /* [in] */ Int32 index);

    CARAPI_(void) UpdateRadialPicker(
        /* [in] */ Int32 index);

    CARAPI_(Int32) ComputeMaxWidthOfNumbers(
        /* [in] */ Int32 max);

    CARAPI_(void) UpdateHeaderAmPm();

    /**
      * Set whether in keyboard mode or not.
      *
      * @param inKbMode True means in keyboard mode.
      */
    CARAPI_(void) SetInKbMode(
        /* [in] */ Boolean inKbMode);

    /**
      * @return true if in keyboard mode
      */
    CARAPI_(Boolean) InKbMode();

    CARAPI_(void) SetTypedTimes(
        /* [in] */ IArrayList* typeTimes);

    /**
      * @return an array of typed times
      */
    CARAPI_(AutoPtr<IArrayList>) GetTypedTimes();

    /**
      * @return the index of the current item showing
      */
    CARAPI_(Int32) GetCurrentItemShowing();

    /**
      * Propagate the time change
      */
    CARAPI_(void) OnTimeChanged();

    CARAPI_(void) TryVibrate();

    CARAPI_(void) UpdateAmPmDisplay(
        /* [in] */ Int32 amOrPm);

    CARAPI_(void) UpdateHeaderHour(
        /* [in] */ Int32 value,
        /* [in] */ Boolean announce);

    static CARAPI_(Int32) Modulo12(
        /* [in] */ Int32 n,
        /* [in] */ Boolean startWithZero);

    /**
      * The time separator is defined in the Unicode CLDR and cannot be supposed to be ":".
      *
      * See http://unicode.org/cldr/trac/browser/trunk/common/main
      *
      * We pass the correct "skeleton" depending on 12 or 24 hours view and then extract the
      * separator as the character which is just after the hour marker in the returned pattern.
      */
    CARAPI_(void) UpdateHeaderSeparator();

    static CARAPI_(Int32) LastIndexOfAny(
        /* [in] */ const String& str,
        /* [in] */ ArrayOf<Char32>* any);

    CARAPI_(void) UpdateHeaderMinute(
        /* [in] */ Int32 value);

    /**
      * Show either Hours or Minutes.
      */
    CARAPI_(void) SetCurrentItemShowing(
        /* [in] */ Int32 index,
        /* [in] */ Boolean animateCircle,
        /* [in] */ Boolean announce);

    /**
      * For keyboard mode, processes key events.
      *
      * @param keyCode the pressed key.
      *
      * @return true if the key was successfully processed, false otherwise.
      */
    CARAPI_(Boolean) ProcessKeyUp(
        /* [in] */ Int32 keyCode);

    /**
      * Try to start keyboard mode with the specified key.
      *
      * @param keyCode The key to use as the first press. Keyboard mode will not be started if the
      * key is not legal to start with. Or, pass in -1 to get into keyboard mode without a starting
      * key.
      */
    CARAPI_(void) TryStartingKbMode(
        /* [in] */ Int32 keyCode);

    CARAPI_(Boolean) AddKeyIfLegal(
        /* [in] */ Int32 keyCode);

    /**
      * Traverse the tree to see if the keys that have been typed so far are legal as is,
      * or may become legal as more keys are typed (excluding backspace).
      */
    CARAPI_(Boolean) IsTypedTimeLegalSoFar();

    /**
      * Check if the time that has been typed so far is completely legal, as is.
      */
    CARAPI_(Boolean) IsTypedTimeFullyLegal();

    CARAPI_(Int32) DeleteLastTypedKey();

    /**
      * Get out of keyboard mode. If there is nothing in typedTimes, revert to TimePicker's time.
      */
    CARAPI_(void) FinishKbMode();

    /**
      * Update the hours, minutes, and AM/PM displays with the typed times. If the typedTimes is
      * empty, either show an empty display (filled with the placeholder text), or update from the
      * timepicker's values.
      *
      * @param allowEmptyDisplay if true, then if the typedTimes is empty, use the placeholder text.
      * Otherwise, revert to the timepicker's values.
      */
    CARAPI_(void) UpdateDisplay(
        /* [in] */ Boolean allowEmptyDisplay);

    CARAPI_(Int32) GetValFromKeyCode(
        /* [in] */ Int32 keyCode);

    /**
      * Get the currently-entered time, as integer values of the hours and minutes typed.
      *
      * @param enteredZeros A size-2 boolean array, which the caller should initialize, and which
      * may then be used for the caller to know whether zeros had been explicitly entered as either
      * hours of minutes. This is helpful for deciding whether to show the dashes, or actual 0's.
      *
      * @return A size-3 int array. The first value will be the hours, the second value will be the
      * minutes, and the third will be either AM or PM.
      */
    CARAPI_(AutoPtr< ArrayOf<Int32> >) GetEnteredTime(
        /* [in] */ ArrayOf<Boolean>* enteredZeros);

    /**
      * Get the keycode value for AM and PM in the current language.
      */
    CARAPI_(Int32) GetAmOrPmKeyCode(
        /* [in] */ Int32 amOrPm);

    /**
      * Create a tree for deciding what keys can legally be typed.
      */
    CARAPI_(void) GenerateLegalTimesTree();

private:
    static const String TAG;
    // Index used by RadialPickerLayout
    static const Int32 HOUR_INDEX = 0;
    static const Int32 MINUTE_INDEX = 1;
    // NOT a real index for the purpose of what's showing.
    static const Int32 AMPM_INDEX = 2;
    // Also NOT a real index, just used for keyboard mode.
    static const Int32 ENABLE_PICKER_INDEX = 3;
    static const Int32 AM = 0;
    static const Int32 PM = 1;
    static const Boolean DEFAULT_ENABLED_STATE;
    Boolean mIsEnabled;
    static const Int32 HOURS_IN_HALF_DAY = 12;
    AutoPtr<IView> mHeaderView;
    AutoPtr<ITextView> mHourView;
    AutoPtr<ITextView> mMinuteView;
    AutoPtr<ITextView> mAmPmTextView;
    AutoPtr<IRadialTimePickerView> mRadialTimePickerView;
    AutoPtr<ITextView> mSeparatorView;
    String mAmText;
    String mPmText;
    Boolean mAllowAutoAdvance;
    Int32 mInitialHourOfDay;
    Int32 mInitialMinute;
    Boolean mIs24HourView;
    // For hardware IME input.
    Char32 mPlaceholderText;
    String mDoublePlaceholderText;
    String mDeletedKeyFormat;
    Boolean mInKbMode;
    AutoPtr<IArrayList> mTypedTimes;
    AutoPtr<Node> mLegalTimesTree;
    Int32 mAmKeyCode;
    Int32 mPmKeyCode;
    // Accessibility strings.
    String mHourPickerDescription;
    String mSelectHours;
    String mMinutePickerDescription;
    String mSelectMinutes;
    AutoPtr<ICalendar> mTempCalendar;
    /*const*/ AutoPtr<IViewOnKeyListener> mKeyListener;
    /*const*/ AutoPtr<IViewOnFocusChangeListener> mFocusListener;
};

} // namespace Widget
} // namespace Droid
} // namespace Elastos

template <>
struct Conversion<Elastos::Droid::Widget::TimePickerSpinnerDelegate::SavedState*, IInterface*>
{
    enum { exists = TRUE, exists2Way = FALSE, sameType = FALSE };
};

#endif // __ELASTOS_DROID_WIDGET_TIMEPICKERSPINNERDELEGATE_H__

