
#ifndef __ELASTOS_DROID_WEBKIT_CONTENT_BROWSER_INPUT_TWOFIELDDATEPICKER_H__
#define __ELASTOS_DROID_WEBKIT_CONTENT_BROWSER_INPUT_TWOFIELDDATEPICKER_H__

#include "Elastos.Droid.Widget.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Text::Format::IDateUtils;
using Elastos::Droid::View::ILayoutInflater;
using Elastos::Droid::View::Accessibility::IAccessibilityEvent;
using Elastos::Droid::Widget::IFrameLayout;
using Elastos::Droid::Widget::INumberPicker;
using Elastos::Droid::Widget::INumberPickerOnValueChangeListener;

// import org.chromium.content.R;

using Elastos::Utility::ICalendar;
using Elastos::Utility::ITimeZone;

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Content {
namespace Browser {
namespace Input {

/**
 * This class is heavily based on android.widget.DatePicker.
 */
class TwoFieldDatePicker
    : public Object
    , public IFrameLayout
{
public:
    /**
     * The callback used to indicate the user changes\d the date.
     */
    class OnMonthOrWeekChangedListener : public Object
    {
    public:
        /**
         * Called upon a date change.
         *
         * @param view The view associated with this listener.
         * @param year The year that was set.
         * @param positionInYear The month or week in year.
         */
        virtual CARAPI OnMonthOrWeekChanged(
            /* [in] */ TwoFieldDatePicker* view,
            /* [in] */ Int32 year,
            /* [in] */ Int32 positionInYear) = 0;
    };

private:
    class InnerNumberPickerOnValueChangeListener
        : public Object
        , public INumberPickerOnValueChangeListener
    {
    public:
        CAR_INTERFACE_DECL();

        InnerNumberPickerOnValueChangeListener(
            /* [in] */ TwoFieldDatePicker* owner);

        CARAPI OnValueChange(
            /* [in] */ INumberPicker* picker,
            /* [in] */ Int32 oldVal,
            /* [in] */ Int32 newVal);

    private:
        TwoFieldDatePicker* mOwner;
    };

public:
    CAR_INTERFACE_DECL();

    TwoFieldDatePicker(
        /* [in] */ IContext* context,
        /* [in] */ Double minValue,
        /* [in] */ Double maxValue);

    /**
     * Initialize the state. If the provided values designate an inconsistent
     * date the values are normalized before updating the spinners.
     *
     * @param year The initial year.
     * @param positionInYear The initial month <strong>starting from zero</strong> or week in year.
     * @param onMonthOrWeekChangedListener How user is notified date is changed by
     *            user, can be null.
     */
    virtual CARAPI_(void) Init(
        /* [in] */ Int32 year,
        /* [in] */ Int32 positionInYear,
        /* [in] */ OnMonthOrWeekChangedListener* onMonthOrWeekChangedListener);

    virtual CARAPI_(Boolean) IsNewDate(
        /* [in] */ Int32 year,
        /* [in] */ Int32 positionInYear);

    /**
     * Updates the current date.
     *
     * @param year The year.
     * @param positionInYear The month or week in year.
     */
    virtual CARAPI_(void) UpdateDate(
        /* [in] */ Int32 year,
        /* [in] */ Int32 positionInYear);

    //@Override
    CARAPI DispatchPopulateAccessibilityEvent(
        /* [in] */ IAccessibilityEvent* event,
        /* [out] */ Boolean* result);

    //@Override
    CARAPI OnPopulateAccessibilityEvent(
        /* [in] */ IAccessibilityEvent* event);

    /**
     * @return The selected year.
     */
    virtual CARAPI_(Int32) GetYear();

    /**
     * @return The selected month or week.
     */
    virtual CARAPI_(Int32) GetPositionInYear() = 0;

protected:
    /**
     * Subclasses know the semantics of @value, and need to return
     * a Calendar corresponding to it.
     */
    virtual CARAPI_(AutoPtr<ICalendar>) GetDateForValue(
        /* [in] */ Double value) = 0;

    /**
     * Subclasses know the semantics of @positionInYear, and need to update @mCurrentDate to the
     * appropriate date.
     */
    virtual CARAPI SetCurrentDate(
        /* [in] */ Int32 year,
        /* [in] */ Int32 positionInYear) = 0;

    virtual CARAPI SetCurrentDate(
        /* [in] */ ICalendar* date);

    virtual CARAPI_(Int32) GetMaxYear() = 0;

    virtual CARAPI_(Int32) GetMinYear() = 0;

    virtual CARAPI_(Int32) GetMaxPositionInYear(
        /* [in] */ Int32 year) = 0;

    virtual CARAPI_(Int32) GetMinPositionInYear(
        /* [in] */ Int32 year) = 0;

    virtual CARAPI_(AutoPtr<ICalendar>) GetMaxDate();

    virtual CARAPI_(AutoPtr<ICalendar>) GetMinDate();

    virtual CARAPI_(AutoPtr<ICalendar>) GetCurrentDate();

    virtual CARAPI_(AutoPtr<INumberPicker>) GetPositionInYearSpinner();

    virtual CARAPI_(AutoPtr<INumberPicker>) GetYearSpinner();

    /**
     * This method should be subclassed to update the spinners based on mCurrentDate.
     */
    virtual CARAPI_(void) UpdateSpinners();

    /**
     * Notifies the listener, if such, for a change in the selected date.
     */
    virtual CARAPI_(void) NotifyDateChanged();

private:
    const AutoPtr<INumberPicker> mPositionInYearSpinner;

    const AutoPtr<INumberPicker> mYearSpinner;

    AutoPtr<OnMonthOrWeekChangedListener> mMonthOrWeekChangedListener;

    // It'd be nice to use android.text.Time like in other Dialogs but
    // it suffers from the 2038 effect so it would prevent us from
    // having dates over 2038.
    AutoPtr<ICalendar> mMinDate;

    AutoPtr<ICalendar> mMaxDate;

    AutoPtr<ICalendar> mCurrentDate;
};

} // namespace Input
} // namespace Browser
} // namespace Content
} // namespace Webkit
} // namespace Droid
} // namespace Elastos

#endif//__ELASTOS_DROID_WEBKIT_CONTENT_BROWSER_INPUT_TWOFIELDDATEPICKER_H__
