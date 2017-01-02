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

#ifndef __ELASTOS_DROID_APP_TIMEPICKER_DIALOG_H__
#define __ELASTOS_DROID_APP_TIMEPICKER_DIALOG_H__

#include "Elastos.Droid.Widget.h"
#include "elastos/droid/app/AlertDialog.h"

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IDialogInterfaceOnClickListener;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Widget::ITimePicker;
using Elastos::Droid::Widget::ITimePickerOnTimeChangedListener;
using Elastos::Droid::Widget::ITimePickerValidationCallback;

namespace Elastos {
namespace Droid {
namespace App {

/**
 * A dialog that prompts the user for the time of day using a {@link TimePicker}.
 *
 * <p>See the <a href="{@docRoot}guide/topics/ui/controls/pickers.html">Pickers</a>
 * guide.</p>
 */
class TimePickerDialog
    : public AlertDialog
    , public ITimePickerDialog
{
private:
    class InnerListener
        : public Object
        , public ITimePickerValidationCallback
        , public IDialogInterfaceOnClickListener
        , public ITimePickerOnTimeChangedListener
    {
    public:
        CAR_INTERFACE_DECL()

        InnerListener(
            /* [in] */ TimePickerDialog* host);

        //@Override
        CARAPI OnValidationChanged(
            /* [in] */ Boolean valid);

        //@Override
        CARAPI OnTimeChanged(
            /* [in] */ ITimePicker* view,
            /* [in] */ Int32 hourOfDay,
            /* [in] */ Int32 minute);

        //@Override
        CARAPI OnClick(
            /* [in] */ IDialogInterface* dialog,
            /* [in] */ Int32 which);

    private:
        TimePickerDialog* mHost;
    };

public:
    CAR_INTERFACE_DECL()

    TimePickerDialog();

    virtual ~TimePickerDialog();

    /**
     * @param context Parent.
     * @param callBack How parent is notified.
     * @param hourOfDay The initial hour.
     * @param minute The initial minute.
     * @param is24HourView Whether this is a 24 hour view, or AM/PM.
     */
    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ ITimePickerDialogOnTimeSetListener* callBack,
        /* [in] */ Int32 hourOfDay,
        /* [in] */ Int32 minute,
        /* [in] */ Boolean is24HourView);

    /**
     * @param context Parent.
     * @param callBack How parent is notified.
     * @param hourOfDay The initial hour.
     * @param minute The initial minute.
     * @param is24HourView Whether this is a 24 hour view, or AM/PM.
     */
    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ Int32 theme,
        /* [in] */ ITimePickerDialogOnTimeSetListener* callBack,
        /* [in] */ Int32 hourOfDay,
        /* [in] */ Int32 minute,
        /* [in] */ Boolean is24HourView);

    static Int32 ResolveDialogTheme(
        /* [in] */ IContext* context,
        /* [in] */ Int32 resid);

    //@Override
    CARAPI OnTimeChanged(
        /* [in] */ ITimePicker* view,
        /* [in] */ Int32 hourOfDay,
        /* [in] */ Int32 minute);

    //@Override
    CARAPI OnClick(
        /* [in] */ IDialogInterface* dialog,
        /* [in] */ Int32 which);

    /**
     * Sets the current time.
     *
     * @param hourOfDay The current hour within the day.
     * @param minuteOfHour The current minute within the hour.
     */
    CARAPI UpdateTime(
        /* [in] */ Int32 hourOfDay,
        /* [in] */ Int32 minuteOfHour);

    //@Override
    CARAPI OnSaveInstanceState(
        /* [out] */ IBundle** result);

    //@Override
    CARAPI OnRestoreInstanceState(
        /* [in] */ IBundle* savedInstanceState);

private:
    AutoPtr<InnerListener> mListener;

    static const String HOUR;
    static const String MINUTE;
    static const String IS_24_HOUR;

    AutoPtr<ITimePicker> mTimePicker;
    AutoPtr<ITimePickerDialogOnTimeSetListener> mTimeSetCallback;

    Int32 mInitialHourOfDay;
    Int32 mInitialMinute;
    Boolean mIs24HourView;
};

} // namespace App
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_APP_TIMEPICKER_DIALOG_H__
