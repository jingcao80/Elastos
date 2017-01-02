//=========================================================================
// Copyright (C) 2012 The Elastos Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//=========================================================================

#ifndef __ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_CONTENT_BROWSER_INPUT_TWOFIELDDATEPICKER_H__
#define __ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_CONTENT_BROWSER_INPUT_TWOFIELDDATEPICKER_H__

#include "Elastos.Droid.Widget.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>
#include "elastos/droid/widget/FrameLayout.h"

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Text::Format::IDateUtils;
using Elastos::Droid::View::ILayoutInflater;
using Elastos::Droid::View::Accessibility::IAccessibilityEvent;
using Elastos::Droid::Widget::FrameLayout;
using Elastos::Droid::Widget::INumberPicker;
using Elastos::Droid::Widget::INumberPickerOnValueChangeListener;

// import org.chromium.content.R;

using Elastos::Utility::ICalendar;
using Elastos::Utility::ITimeZone;

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Webview {
namespace Chromium {
namespace Content {
namespace Browser {
namespace Input {

/**
 * This class is heavily based on android.widget.DatePicker.
 */
class TwoFieldDatePicker
    : public FrameLayout
{
public:
    /**
     * The callback used to indicate the user changes\d the date.
     */
    class OnMonthOrWeekChangedListener
    {
    public:
        virtual CARAPI_(UInt32) AddRef() = 0;

        virtual CARAPI_(UInt32) Release() = 0;

        virtual CARAPI_(PInterface) Probe(
            /* [in] */ REIID riid) = 0;

        virtual CARAPI GetInterfaceID(
            /* [in] */ IInterface* object,
            /* [out] */ InterfaceID* iid) = 0;

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
        CAR_INTERFACE_DECL()

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
    AutoPtr<INumberPicker> mPositionInYearSpinner;

    AutoPtr<INumberPicker> mYearSpinner;

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
} // namespace Chromium
} // namespace Webview
} // namespace Webkit
} // namespace Droid
} // namespace Elastos

#endif//__ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_CONTENT_BROWSER_INPUT_TWOFIELDDATEPICKER_H__
