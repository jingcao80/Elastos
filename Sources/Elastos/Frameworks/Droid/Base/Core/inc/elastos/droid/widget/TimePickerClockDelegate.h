
#ifndef __ELASTOS_DROID_WIDGET_TIMEPICKERCLOCKDELEGATE_H__
#define __ELASTOS_DROID_WIDGET_TIMEPICKERCLOCKDELEGATE_H__

#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/view/View.h"
#include "elastos/droid/widget/TimePicker.h"

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::Res::IConfiguration;
using Elastos::Droid::Utility::IAttributeSet;
using Elastos::Droid::View::Accessibility::IAccessibilityEvent;
using Elastos::Droid::View::Accessibility::IAccessibilityNodeInfo;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::IViewOnClickListener;
using Elastos::Droid::View::View;
using Elastos::Droid::Widget::IAbsListViewSavedState;
using Elastos::Droid::Widget::IButton;
using Elastos::Droid::Widget::IEditText;
using Elastos::Droid::Widget::INumberPicker;
using Elastos::Droid::Widget::ITextView;
using Elastos::Droid::Widget::TimePicker;
using Elastos::Utility::ICalendar;
using Elastos::Utility::ILocale;

namespace Elastos {
namespace Droid {
namespace Widget {

/**
  * A delegate implementing the basic TimePicker
  */
class TimePickerClockDelegate
    : public TimePicker::AbstractTimePickerDelegate
    , public ITimePickerClockDelegate
{
public:
    /**
      * Used to save / restore state of time picker
      */
    class SavedState
        : public Elastos::Droid::View::View::BaseSavedState
        , public ITimePickerClockDelegateSavedState
    {
        friend class TimePickerClockDelegate;
    public:
        CAR_INTERFACE_DECL()

        SavedState();

        ~SavedState();

        CARAPI constructor(
            /* [in] */ IParcelable* superState,
            /* [in] */ Int32 hour,
            /* [in] */ Int32 minute);

        CARAPI constructor();

        virtual CARAPI_(Int32) GetHour();

        virtual CARAPI_(Int32) GetMinute();

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
        Int32 mHour;
        Int32 mMinute;
    };

private:
    class InnerOnValueChangeListener
        : public Object
        , public INumberPickerOnValueChangeListener
    {
    public:
        CAR_INTERFACE_DECL()

        InnerOnValueChangeListener(
            /* [in] */ TimePickerClockDelegate* owner);

        virtual CARAPI OnValueChange(
            /* [in] */ INumberPicker* spinner,
            /* [in] */ Int32 oldVal,
            /* [in] */ Int32 newVal);

    private:
        TimePickerClockDelegate* mOwner;
    };

    class InnerOnValueChangeListener1
        : public Object
        , public INumberPickerOnValueChangeListener
    {
    public:
        CAR_INTERFACE_DECL()

        InnerOnValueChangeListener1(
            /* [in] */ TimePickerClockDelegate* owner);

        virtual CARAPI OnValueChange(
            /* [in] */ INumberPicker* spinner,
            /* [in] */ Int32 oldVal,
            /* [in] */ Int32 newVal);

    private:
        TimePickerClockDelegate* mOwner;
    };

    class InnerOnValueChangeListener2
        : public Object
        , public INumberPickerOnValueChangeListener
    {
    public:
        CAR_INTERFACE_DECL()

        InnerOnValueChangeListener2(
            /* [in] */ TimePickerClockDelegate* owner);

        virtual CARAPI OnValueChange(
            /* [in] */ INumberPicker* spinner,
            /* [in] */ Int32 oldVal,
            /* [in] */ Int32 newVal);

    private:
        TimePickerClockDelegate* mOwner;
    };

    class InnerOnClickListener
        : public Object
        , public IViewOnClickListener
    {
    public:
        CAR_INTERFACE_DECL()

        InnerOnClickListener(
            /* [in] */ TimePickerClockDelegate* owner);

        virtual CARAPI OnClick(
            /* [in] */ IView* button);

    private:
        TimePickerClockDelegate* mOwner;
    };

public:
    CAR_INTERFACE_DECL()

    TimePickerClockDelegate();

    CARAPI constructor(
        /* [in] */ ITimePicker* delegator,
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs,
        /* [in] */ Int32 defStyleAttr,
        /* [in] */ Int32 defStyleRes);

    // @Override
    CARAPI SetCurrentHour(
        /* [in] */ Int32 currentHour);

    // @Override
    CARAPI GetCurrentHour(
        /* [out] */ Int32* result);

    // @Override
    CARAPI SetCurrentMinute(
        /* [in] */ Int32 currentMinute);

    // @Override
    CARAPI GetCurrentMinute(
        /* [out] */ Int32* result);

    // @Override
    CARAPI SetIs24HourView(
        /* [in] */ Boolean is24HourView);

    // @Override
    CARAPI Is24HourView(
        /* [out] */ Boolean* result);

    // @Override
    CARAPI SetOnTimeChangedListener(
        /* [in] */ ITimePickerOnTimeChangedListener* onTimeChangedListener);

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
      * Sets the current locale.
      *
      * @param locale The current locale.
      */
    // @Override
    CARAPI SetCurrentLocale(
        /* [in] */ ILocale* locale);

    static CARAPI_(AutoPtr< ArrayOf<String> >) GetAmPmStrings(
        /* [in] */ IContext* context);

private:
    CARAPI_(void) GetHourFormatData();

    CARAPI_(Boolean) IsAmPmAtStart();

    /**
      * The time separator is defined in the Unicode CLDR and cannot be supposed to be ":".
      *
      * See http://unicode.org/cldr/trac/browser/trunk/common/main
      *
      * We pass the correct "skeleton" depending on 12 or 24 hours view and then extract the
      * separator as the character which is just after the hour marker in the returned pattern.
      */
    CARAPI_(void) SetDividerText();

    CARAPI_(void) SetCurrentHour(
        /* [in] */ Int32 currentHour,
        /* [in] */ Boolean notifyTimeChanged);

    CARAPI_(void) UpdateInputState();

    CARAPI_(void) UpdateAmPmControl();

    CARAPI_(void) OnTimeChanged();

    CARAPI_(void) UpdateHourControl();

    CARAPI_(void) UpdateMinuteControl();

    CARAPI_(void) SetContentDescriptions();

    CARAPI_(void) TrySetContentDescription(
        /* [in] */ IView* root,
        /* [in] */ Int32 viewId,
        /* [in] */ Int32 contDescResId);

private:
    static const Boolean DEFAULT_ENABLED_STATE;
    static const Int32 HOURS_IN_HALF_DAY = 12;
    // state
    Boolean mIs24HourView;
    Boolean mIsAm;
    // ui components
    /*const*/ AutoPtr<INumberPicker> mHourSpinner;
    /*const*/ AutoPtr<INumberPicker> mMinuteSpinner;
    /*const*/ AutoPtr<INumberPicker> mAmPmSpinner;
    /*const*/ AutoPtr<IEditText> mHourSpinnerInput;
    /*const*/ AutoPtr<IEditText> mMinuteSpinnerInput;
    /*const*/ AutoPtr<IEditText> mAmPmSpinnerInput;
    /*const*/ AutoPtr<ITextView> mDivider;
    // Note that the legacy implementation of the TimePicker is
    // using a button for toggling between AM/PM while the new
    // version uses a NumberPicker spinner. Therefore the code
    // accommodates these two cases to be backwards compatible.
    /*const*/ AutoPtr<IButton> mAmPmButton;
    /*const*/ AutoPtr< ArrayOf<String> > mAmPmStrings;
    Boolean mIsEnabled;
    AutoPtr<ICalendar> mTempCalendar;
    Boolean mHourWithTwoDigit;
    Char32 mHourFormat;
};

} // namespace Widget
} // namespace Droid
} // namespace Elastos

template <>
struct Conversion<Elastos::Droid::Widget::TimePickerClockDelegate::SavedState*, IInterface*>
{
    enum { exists = TRUE, exists2Way = FALSE, sameType = FALSE };
};

#endif // __ELASTOS_DROID_WIDGET_TIMEPICKERCLOCKDELEGATE_H__

