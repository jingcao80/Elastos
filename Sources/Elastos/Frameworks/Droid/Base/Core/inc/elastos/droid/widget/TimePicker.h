
#ifndef __ELASTOS_DROID_WIDGET_TIMEPICKER_H__
#define __ELASTOS_DROID_WIDGET_TIMEPICKER_H__

#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/widget/FrameLayout.h"

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::Res::IConfiguration;
using Elastos::Droid::Utility::IAttributeSet;
using Elastos::Droid::View::Accessibility::IAccessibilityEvent;
using Elastos::Droid::View::Accessibility::IAccessibilityNodeInfo;
using Elastos::Droid::Widget::FrameLayout;
using Elastos::Droid::Widget::ITimePickerAbstractTimePickerDelegate;
using Elastos::Core::IInteger32;
using Elastos::Utility::ILocale;

namespace Elastos {
namespace Droid {
namespace Widget {

// @Widget
class TimePicker
    : public FrameLayout
    , public ITimePicker
{
public:
    class TimePickerDelegate
        : public Object
    {
    public:
        virtual CARAPI SetCurrentHour(
            /* [in] */ Int32 currentHour) = 0;

        virtual CARAPI GetCurrentHour(
            /* [out] */ Int32* result) = 0;

        virtual CARAPI SetCurrentMinute(
            /* [in] */ Int32 currentMinute) = 0;

        virtual CARAPI GetCurrentMinute(
            /* [out] */ Int32* result) = 0;

        virtual CARAPI SetIs24HourView(
            /* [in] */ Boolean is24HourView) = 0;

        virtual CARAPI Is24HourView(
            /* [out] */ Boolean* result) = 0;

        virtual CARAPI SetOnTimeChangedListener(
            /* [in] */ ITimePickerOnTimeChangedListener* onTimeChangedListener) = 0;

        virtual CARAPI SetValidationCallback(
            /* [in] */ ITimePickerValidationCallback* callback) = 0;

        virtual CARAPI SetEnabled(
            /* [in] */ Boolean enabled) = 0;

        virtual CARAPI IsEnabled(
            /* [out] */ Boolean* result) = 0;

        virtual CARAPI GetBaseline(
            /* [out] */ Int32* result) = 0;

        virtual CARAPI OnConfigurationChanged(
            /* [in] */ IConfiguration* newConfig) = 0;

        virtual CARAPI OnSaveInstanceState(
            /* [in] */ IParcelable* superState,
            /* [out] */ IParcelable** result) = 0;

        virtual CARAPI OnRestoreInstanceState(
            /* [in] */ IParcelable* state) = 0;

        virtual CARAPI DispatchPopulateAccessibilityEvent(
            /* [in] */ IAccessibilityEvent* event,
            /* [out] */ Boolean* result) = 0;

        virtual CARAPI OnPopulateAccessibilityEvent(
            /* [in] */ IAccessibilityEvent* event) = 0;

        virtual CARAPI OnInitializeAccessibilityEvent(
            /* [in] */ IAccessibilityEvent* event) = 0;

        virtual CARAPI OnInitializeAccessibilityNodeInfo(
            /* [in] */ IAccessibilityNodeInfo* info) = 0;
    };

    /**
      * An abstract class which can be used as a start for TimePicker implementations
      */
    class AbstractTimePickerDelegate
        : public TimePickerDelegate
        , public ITimePickerAbstractTimePickerDelegate
    {
    public:
        CAR_INTERFACE_DECL()

        AbstractTimePickerDelegate();

        CARAPI constructor(
            /* [in] */ ITimePicker* delegator,
            /* [in] */ IContext* context);

        virtual CARAPI SetCurrentLocale(
            /* [in] */ ILocale* locale);

        // @Override
        CARAPI SetValidationCallback(
            /* [in] */ ITimePickerValidationCallback* callback);

    protected:
        virtual CARAPI_(void) OnValidationChanged(
            /* [in] */ Boolean valid);

    protected:
        // The delegator
        AutoPtr<ITimePicker> mDelegator;
        // The context
        AutoPtr<IContext> mContext;
        // The current locale
        AutoPtr<ILocale> mCurrentLocale;
        // Callbacks
        AutoPtr<ITimePickerOnTimeChangedListener> mOnTimeChangedListener;
        AutoPtr<ITimePickerValidationCallback> mValidationCallback;
    };

public:
    CAR_INTERFACE_DECL()

    TimePicker();

    CARAPI constructor(
        /* [in] */ IContext* context);

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs);

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs,
        /* [in] */ Int32 defStyleAttr);

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs,
        /* [in] */ Int32 defStyleAttr,
        /* [in] */ Int32 defStyleRes);

    /**
      * Set the current hour.
      */
    virtual CARAPI SetCurrentHour(
        /* [in] */ Int32 currentHour);

    /**
      * @return The current hour in the range (0-23).
      */
    virtual CARAPI GetCurrentHour(
        /* [out] */ Int32* result);

    /**
      * Set the current minute (0-59).
      */
    virtual CARAPI SetCurrentMinute(
        /* [in] */ Int32 currentMinute);

    /**
      * @return The current minute.
      */
    virtual CARAPI GetCurrentMinute(
        /* [out] */ Int32* result);

    /**
      * Set whether in 24 hour or AM/PM mode.
      *
      * @param is24HourView True = 24 hour mode. False = AM/PM.
      */
    virtual CARAPI SetIs24HourView(
        /* [in] */ Boolean is24HourView);

    /**
      * @return true if this is in 24 hour view else false.
      */
    virtual CARAPI Is24HourView(
        /* [out] */ Boolean* result);

    /**
      * Set the callback that indicates the time has been adjusted by the user.
      *
      * @param onTimeChangedListener the callback, should not be null.
      */
    virtual CARAPI SetOnTimeChangedListener(
        /* [in] */ ITimePickerOnTimeChangedListener* onTimeChangedListener);

    /**
      * Sets the callback that indicates the current time is valid.
      *
      * @param callback the callback, may be null
      * @hide
      */
    virtual CARAPI SetValidationCallback(
        /* [in] */ ITimePickerValidationCallback* callback);

    // @Override
    CARAPI SetEnabled(
        /* [in] */ Boolean enabled);

    // @Override
    CARAPI_(Boolean) IsEnabled();

    // @Override
    CARAPI_(Int32) GetBaseline();

    // @Override
    CARAPI_(Boolean) DispatchPopulateAccessibilityEvent(
        /* [in] */ IAccessibilityEvent* event);

    // @Override
    CARAPI OnPopulateAccessibilityEvent(
        /* [in] */ IAccessibilityEvent* event);

    // @Override
    CARAPI OnInitializeAccessibilityEvent(
        /* [in] */ IAccessibilityEvent* event);

    // @Override
    CARAPI OnInitializeAccessibilityNodeInfo(
        /* [in] */ IAccessibilityNodeInfo* info);

protected:
    // @Override
    CARAPI_(void) OnConfigurationChanged(
        /* [in] */ IConfiguration* newConfig);

    // @Override
    CARAPI_(AutoPtr<IParcelable>) OnSaveInstanceState();

    // @Override
    CARAPI_(void) OnRestoreInstanceState(
        /* [in] */ IParcelable* state);

private:
    static const Int32 MODE_SPINNER = 1;
    static const Int32 MODE_CLOCK = 2;
    /*const*/ AutoPtr<TimePickerDelegate> mDelegate;
};

} // namespace Widget
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_WIDGET_TIMEPICKER_H__

