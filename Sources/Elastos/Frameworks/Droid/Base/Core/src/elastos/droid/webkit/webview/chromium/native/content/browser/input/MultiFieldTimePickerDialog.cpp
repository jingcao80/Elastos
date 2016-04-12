#include "Elastos.CoreLibrary.Utility.h"
#include "Elastos.Droid.View.h"
#include "elastos/droid/webkit/webview/chromium/native/content/browser/input/MultiFieldTimePickerDialog.h"
#include "elastos/droid/webkit/webview/chromium/native/content/R_Content.h"
#include "elastos/core/CoreUtils.h"
#include "elastos/core/StringUtils.h"

using Elastos::Droid::App::EIID_IAlertDialog;
using Elastos::Droid::Content::EIID_IDialogInterfaceOnClickListener;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::EIID_IView;
using Elastos::Droid::Webkit::Webview::Chromium::Content::R;
using Elastos::Droid::Widget::EIID_INumberPickerFormatter;
using Elastos::Core::CString;
using Elastos::Core::ICharSequence;
using Elastos::Core::IInteger32;
using Elastos::Core::CoreUtils;
using Elastos::Core::StringUtils;
using Elastos::Utility::CArrayList;

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Webview {
namespace Chromium {
namespace Content {
namespace Browser {
namespace Input {

//==================================================================
//            MultiFieldTimePickerDialog::NumberFormatter
//==================================================================

CAR_INTERFACE_IMPL(MultiFieldTimePickerDialog::NumberFormatter, Object, INumberPickerFormatter);

MultiFieldTimePickerDialog::NumberFormatter::NumberFormatter(
    /* [in] */ const String& format)
    : mFormat(format)
{
}

//@Override
ECode MultiFieldTimePickerDialog::NumberFormatter::Format(
    /* [in] */ Int32 value,
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str);
    AutoPtr<IInteger32> iv = CoreUtils::Convert(value);
    AutoPtr<ArrayOf<IInterface*> > args = ArrayOf<IInterface*>::Alloc(1);
    args->Set(0, TO_IINTERFACE(iv));
    *str = StringUtils::Format(mFormat, args);
    return NOERROR;
}

//==================================================================
//                 MultiFieldTimePickerDialog
//==================================================================

const Int32 MultiFieldTimePickerDialog::SECOND_IN_MILLIS;
const Int32 MultiFieldTimePickerDialog::MINUTE_IN_MILLIS;
const Int32 MultiFieldTimePickerDialog::HOUR_IN_MILLIS;

CAR_INTERFACE_IMPL(MultiFieldTimePickerDialog, AlertDialog, IDialogInterfaceOnClickListener);

MultiFieldTimePickerDialog::MultiFieldTimePickerDialog(
    /* [in] */ IContext* context,
    /* [in] */ Int32 theme,
    /* [in] */ Int32 hour,
    /* [in] */ Int32 minute,
    /* [in] */ Int32 second,
    /* [in] */ Int32 milli,
    /* [in] */ Int32 min,
    /* [in] */ Int32 max,
    /* [in] */ Int32 step,
    /* [in] */ Boolean is24hourFormat,
    /* [in] */ OnMultiFieldTimeSetListener* listener)
    : mStep(step)
    , mIs24hourFormat(is24hourFormat)
{
    AlertDialog::constructor(context, theme);
    mListener = listener;

    if (min >= max) {
        min = 0;
        max = 24 * HOUR_IN_MILLIS - 1;
    }

    if (step < 0 || step >= 24 * HOUR_IN_MILLIS) {
        step = MINUTE_IN_MILLIS;
    }

    AutoPtr<ILayoutInflater> inflater;
    AutoPtr<IInterface> obj;
    context->GetSystemService(
                    IContext::LAYOUT_INFLATER_SERVICE,
                    (IInterface**)&obj);
    inflater = ILayoutInflater::Probe(obj);
    AutoPtr<IView> view;
    inflater->Inflate(R::layout::multi_field_time_picker_dialog, NULL,
             (IView**)&view);
    SetView(view);

    AutoPtr<IView> tmpView;
    view->FindViewById(R::id::hour, (IView**)&tmpView);
    mHourSpinner = INumberPicker::Probe(tmpView);
    tmpView = NULL;
    view->FindViewById(R::id::minute, (IView**)&tmpView);
    mMinuteSpinner = INumberPicker::Probe(tmpView);
    tmpView = NULL;
    view->FindViewById(R::id::second, (IView**)&tmpView);
    mSecSpinner = INumberPicker::Probe(tmpView);
    tmpView = NULL;
    view->FindViewById(R::id::milli, (IView**)&tmpView);
    mMilliSpinner = INumberPicker::Probe(tmpView);
    tmpView = NULL;
    view->FindViewById(R::id::ampm, (IView**)&tmpView);
    mAmPmSpinner = INumberPicker::Probe(tmpView);
    tmpView = NULL;

    Int32 minHour = min / HOUR_IN_MILLIS;
    Int32 maxHour = max / HOUR_IN_MILLIS;
    min -= minHour * HOUR_IN_MILLIS;
    max -= maxHour * HOUR_IN_MILLIS;

    if (minHour == maxHour) {
        AutoPtr<IView> view = IView::Probe(mHourSpinner);
        view->SetEnabled(FALSE);
        hour = minHour;
    }

    if (is24hourFormat) {
        AutoPtr<IView> view = IView::Probe(mAmPmSpinner);
        view->SetVisibility(IView::GONE);
    }
    else {
        Int32 minAmPm = minHour / 12;
        Int32 maxAmPm = maxHour / 12;
        Int32 amPm = hour / 12;
        mAmPmSpinner->SetMinValue(minAmPm);
        mAmPmSpinner->SetMaxValue(maxAmPm);
        String am, pm;
        context->GetString(R::string::time_picker_dialog_am, &am);
        context->GetString(R::string::time_picker_dialog_pm, &pm);
        AutoPtr< ArrayOf<String> > strs = ArrayOf<String>::Alloc(2);
        (*strs)[0] = am;
        (*strs)[1] = pm;
        mAmPmSpinner->SetDisplayedValues(strs);

        hour %= 12;
        if (hour == 0) {
            hour = 12;
        }

        if (minAmPm == maxAmPm) {
            AutoPtr<IView> view = IView::Probe(mAmPmSpinner);
            view->SetEnabled(FALSE);
            amPm = minAmPm;

            minHour %= 12;
            maxHour %= 12;
            if (minHour == 0 && maxHour == 0) {
                minHour = 12;
                maxHour = 12;
            } else if (minHour == 0) {
                minHour = maxHour;
                maxHour = 12;
            } else if (maxHour == 0) {
                maxHour = 12;
            }
        }
        else {
            minHour = 1;
            maxHour = 12;
        }
        mAmPmSpinner->SetValue(amPm);
    }

    if (minHour == maxHour) {
        AutoPtr<IView> view = IView::Probe(mHourSpinner);
        view->SetEnabled(FALSE);
    }

    mHourSpinner->SetMinValue(minHour);
    mHourSpinner->SetMaxValue(maxHour);
    mHourSpinner->SetValue(hour);

    AutoPtr<NumberFormatter> twoDigitPaddingFormatter = new NumberFormatter(String("%02d"));

    Int32 minMinute = min / MINUTE_IN_MILLIS;
    Int32 maxMinute = max / MINUTE_IN_MILLIS;
    min -= minMinute * MINUTE_IN_MILLIS;
    max -= maxMinute * MINUTE_IN_MILLIS;

    if (minHour == maxHour) {
        mMinuteSpinner->SetMinValue(minMinute);
        mMinuteSpinner->SetMaxValue(maxMinute);
        if (minMinute == maxMinute) {
            // Set this otherwise the box is empty until you stroke it.
            String format;
            twoDigitPaddingFormatter->Format(minMinute, &format);
            AutoPtr< ArrayOf<String> > array = ArrayOf<String>::Alloc(1);
            (*array)[0] = format;

            mMinuteSpinner->SetDisplayedValues(array);
            AutoPtr<IView> view = IView::Probe(mMinuteSpinner);
            view->SetEnabled(FALSE);
            minute = minMinute;
        }
    }
    else {
        mMinuteSpinner->SetMinValue(0);
        mMinuteSpinner->SetMaxValue(59);
    }

    if (step >= HOUR_IN_MILLIS) {
        AutoPtr<IView> view = IView::Probe(mMinuteSpinner);
        view->SetEnabled(FALSE);
    }

    mMinuteSpinner->SetValue(minute);
    mMinuteSpinner->SetFormatter(twoDigitPaddingFormatter);

    if (step >= MINUTE_IN_MILLIS) {
        // Remove the ':' in front of the second spinner as well.
        AutoPtr<IView> secondColon;
        view->FindViewById(R::id::second_colon, (IView**)&secondColon);
        secondColon->SetVisibility(IView::GONE);
        AutoPtr<IView> secSpinnerView = IView::Probe(mSecSpinner);
        secSpinnerView->SetVisibility(IView::GONE);
    }

    Int32 minSecond = min / SECOND_IN_MILLIS;
    Int32 maxSecond = max / SECOND_IN_MILLIS;
    min -= minSecond * SECOND_IN_MILLIS;
    max -= maxSecond * SECOND_IN_MILLIS;

    if (minHour == maxHour && minMinute == maxMinute) {
        mSecSpinner->SetMinValue(minSecond);
        mSecSpinner->SetMaxValue(maxSecond);
        if (minSecond == maxSecond) {
            // Set this otherwise the box is empty until you stroke it.
            AutoPtr< ArrayOf<String> > array = ArrayOf<String>::Alloc(1);
            String str;
            twoDigitPaddingFormatter->Format(minSecond, &str);
            (*array)[0] = str;
            mSecSpinner->SetDisplayedValues(array);
            AutoPtr<IView> view = IView::Probe(mSecSpinner);
            view->SetEnabled(FALSE);
            second = minSecond;
        }
    }
    else {
        mSecSpinner->SetMinValue(0);
        mSecSpinner->SetMaxValue(59);
    }

    mSecSpinner->SetValue(second);
    mSecSpinner->SetFormatter(twoDigitPaddingFormatter);

    if (step >= SECOND_IN_MILLIS) {
        // Remove the '.' in front of the milli spinner as well.
        AutoPtr<IView> secondDot;
        view->FindViewById(R::id::second_dot, (IView**)&secondDot);
        secondDot->SetVisibility(IView::GONE);
        AutoPtr<IView> milliSpinnerView = IView::Probe(mMilliSpinner);
        milliSpinnerView->SetVisibility(IView::GONE);
    }

    // Round to the nearest step.
    milli = ((milli + step / 2) / step) * step;
    if (step == 1 || step == 10 || step == 100) {
        if (minHour == maxHour && minMinute == maxMinute &&
            minSecond == maxSecond) {
            mMilliSpinner->SetMinValue(min / step);
            mMilliSpinner->SetMaxValue(max / step);

            if (min == max) {
                AutoPtr<IView> view = IView::Probe(mMilliSpinner);
                view->SetEnabled(FALSE);
                milli = min;
            }
        }
        else {
            mMilliSpinner->SetMinValue(0);
            mMilliSpinner->SetMaxValue(999 / step);
        }

        if (step == 1) {
            AutoPtr<NumberFormatter> format = new NumberFormatter(String("%03d"));
            mMilliSpinner->SetFormatter(format);
        }
        else if (step == 10) {
            AutoPtr<NumberFormatter> format = new NumberFormatter(String("%02d"));
            mMilliSpinner->SetFormatter(format);
        }
        else if (step == 100) {
            AutoPtr<NumberFormatter> format = new NumberFormatter(String("%d"));
            mMilliSpinner->SetFormatter(format);
        }
        mMilliSpinner->SetValue(milli / step);
        mBaseMilli = 0;
    }
    else if (step < SECOND_IN_MILLIS) {
        // Non-decimal step value.
        AutoPtr<IArrayList> strValue;
        CArrayList::New((IArrayList**)&strValue);
        for (Int32 i = min; i < max; i += step) {
            AutoPtr<IInteger32> iv = CoreUtils::Convert(i);
            AutoPtr<ArrayOf<IInterface*> > args = ArrayOf<IInterface*>::Alloc(1);
            args->Set(0, TO_IINTERFACE(iv));
            String strFormat = StringUtils::Format(String("%03d"), args);
            AutoPtr<ICharSequence> cs;
            CString::New(strFormat, (ICharSequence**)&cs);
            strValue->Add(TO_IINTERFACE(cs));
        }
        Int32 size;
        strValue->GetSize(&size);
        AutoPtr<ArrayOf<String> > strArray = ArrayOf<String>::Alloc(size);
        for(Int32 i = 0; i < size; ++i) {
            AutoPtr<IInterface> obj;
            strValue->Get(i, (IInterface**)&obj);
            ICharSequence* cs = ICharSequence::Probe(obj);
            String str;
            cs->ToString(&str);
            strArray->Set(i, str);
        }
        mMilliSpinner->SetMinValue(0);
        mMilliSpinner->SetMaxValue(size - 1);
        mMilliSpinner->SetValue((milli - min) / step);
        mMilliSpinner->SetDisplayedValues(strArray);
        mBaseMilli = min;
    }
    else {
        mBaseMilli = 0;
    }
}

//@Override
ECode MultiFieldTimePickerDialog::OnClick(
    /* [in] */ IDialogInterface* dialog,
    /* [in] */ Int32 which)
{
    NotifyDateSet();
    return NOERROR;
}

void MultiFieldTimePickerDialog::NotifyDateSet()
{
    Int32 hour;
    mHourSpinner->GetValue(&hour);
    Int32 minute;
    mMinuteSpinner->GetValue(&minute);
    Int32 sec;
    mSecSpinner->GetValue(&sec);
    Int32 spinnerValue;
    mMilliSpinner->GetValue(&spinnerValue);
    Int32 milli = spinnerValue * mStep + mBaseMilli;
    if (!mIs24hourFormat) {
        Int32 ampm;
        mAmPmSpinner->GetValue(&ampm);
        if (hour == 12) {
            hour = 0;
        }
        hour += ampm * 12;
    }

    mListener->OnTimeSet(hour, minute, sec, milli);
}

} // namespace Input
} // namespace Browser
} // namespace Content
} // namespace Chromium
} // namespace Webview
} // namespace Webkit
} // namespace Droid
} // namespace Elastos
