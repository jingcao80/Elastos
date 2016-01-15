#include "Elastos.Droid.Content.h"
#include "elastos/droid/R.h"
#include "elastos/droid/text/format/Formatter.h"
#include "elastos/droid/net/NetworkUtils.h"
#include <Elastos.CoreLibrary.Core.h>
#include <Elastos.CoreLibrary.Net.h>
#include <elastos/core/CoreUtils.h>

using Elastos::Core::CString;
using Elastos::Core::ICharSequence;
using Elastos::Core::IInteger32;
using Elastos::Core::CoreUtils;
using Elastos::Droid::Net::NetworkUtils;

namespace Elastos {
namespace Droid {
namespace Text {
namespace Format {

const Int32 Formatter::SECONDS_PER_MINUTE = 60;
const Int32 Formatter::SECONDS_PER_HOUR = 60 * 60;
const Int32 Formatter::SECONDS_PER_DAY = 24 * 60 * 60;

String Formatter::FormatFileSize(
    /* [in] */ IContext* context,
    /* [in] */ Int64 number)
{
    return FormatFileSize(context, number, FALSE);
}

String Formatter::FormatShortFileSize(
    /* [in] */ IContext* context,
    /* [in] */ Int64 number)
{
    return FormatFileSize(context, number, TRUE);
}

String Formatter::FormatFileSize(
    /* [in] */ IContext* context,
    /* [in] */ Int64 number,
    /* [in] */ Boolean shorter)
{
    if (context == NULL) {
        return String("");
    }

    Float result = number;
    Int32 suffix;
    suffix = R::string::byteShort;
    if (result > 900) {
        suffix = R::string::kilobyteShort;
        result = result / 1024;
    }
    if (result > 900) {
        suffix = R::string::megabyteShort;
        result = result / 1024;
    }
    if (result > 900) {
        suffix = R::string::gigabyteShort;
        result = result / 1024;
    }
    if (result > 900) {
        suffix = R::string::terabyteShort;
        result = result / 1024;
    }
    if (result > 900) {
        suffix = R::string::petabyteShort;
        result = result / 1024;
    }
    String value;
    if (result < 1) {
        value.AppendFormat("%.2f", result);
    }
    else if (result < 10) {
        if (shorter) {
            value.AppendFormat("%.1f", result);
        }
        else {
            value.AppendFormat("%.2f", result);
        }
    }
    else if (result < 100) {
        if (shorter) {
            value.AppendFormat("%.0f", result);
        }
        else {
            value.AppendFormat("%.2f", result);
        }
    }
    else {
        value.AppendFormat("%.0f", result);
    }
    String resStr;
    context->GetString(suffix, &resStr);
    AutoPtr< ArrayOf<IInterface*> > args = ArrayOf<IInterface*>::Alloc(2);
    AutoPtr<ICharSequence> cs;
    CString::New(value, (ICharSequence**)&cs);
    args->Set(0, cs);
    cs = NULL;
    CString::New(resStr, (ICharSequence**)&cs);
    args->Set(1, cs);
    AutoPtr<IResources> res;
    context->GetResources((IResources**)&res);
    String ret;
    res->GetString(R::string::fileSizeSuffix, args, &ret);
    return ret;
}

String Formatter::FormatIpAddress(
    /* [in] */ Int32 ipv4Address)
{
    String ret;
    AutoPtr<IInetAddress> ina;
    NetworkUtils::IntToInetAddress(ipv4Address, (IInetAddress**)&ina);
    ina->GetHostAddress(&ret);
    return ret;
}

String Formatter::FormatShortElapsedTime(
    /* [in] */ IContext* context,
    /* [in] */ Int64 millis)
{
    Int64 secondsLong = millis / 1000;

    Int32 days = 0, hours = 0, minutes = 0;
    if (secondsLong >= SECONDS_PER_DAY) {
        days = (Int32)(secondsLong / SECONDS_PER_DAY);
        secondsLong -= days * SECONDS_PER_DAY;
    }
    if (secondsLong >= SECONDS_PER_HOUR) {
        hours = (Int32)(secondsLong / SECONDS_PER_HOUR);
        secondsLong -= hours * SECONDS_PER_HOUR;
    }
    if (secondsLong >= SECONDS_PER_MINUTE) {
        minutes = (Int32)(secondsLong / SECONDS_PER_MINUTE);
        secondsLong -= minutes * SECONDS_PER_MINUTE;
    }
    Int32 seconds = (Int32)secondsLong;

    String ret;
    AutoPtr<IInteger32> daysPtr = CoreUtils::Convert(days);
    AutoPtr<IInteger32> hoursPtr = CoreUtils::Convert(hours);
    AutoPtr<IInteger32> minutesPtr = CoreUtils::Convert(minutes);
    AutoPtr<IInteger32> secondsPtr = CoreUtils::Convert(seconds);

    AutoPtr<ArrayOf<IInterface*> > daysFormat = ArrayOf<IInterface*>::Alloc(1);
    (*daysFormat)[0] = daysPtr;

    AutoPtr<ArrayOf<IInterface*> > hoursFormat = ArrayOf<IInterface*>::Alloc(1);
    (*hoursFormat)[0] = hoursPtr;

    AutoPtr<ArrayOf<IInterface*> > minutesFormat = ArrayOf<IInterface*>::Alloc(1);
    (*minutesFormat)[0] = minutesPtr;

    AutoPtr<ArrayOf<IInterface*> > secondsFormat = ArrayOf<IInterface*>::Alloc(1);
    (*secondsFormat)[0] = secondsPtr;

    AutoPtr<ArrayOf<IInterface*> > dayHourFormat = ArrayOf<IInterface*>::Alloc(2);
    (*dayHourFormat)[0] = daysPtr;
    (*dayHourFormat)[1] = hoursPtr;

    AutoPtr<ArrayOf<IInterface*> > hourminuteFormat = ArrayOf<IInterface*>::Alloc(2);
    (*hourminuteFormat)[0] = hoursPtr;
    (*hourminuteFormat)[1] = minutesPtr;

    AutoPtr<ArrayOf<IInterface*> > minutesecondFormat = ArrayOf<IInterface*>::Alloc(2);
    (*minutesecondFormat)[0] = minutesPtr;
    (*minutesecondFormat)[1] = secondsPtr;

    if (days >= 2) {
        days += (hours + 12) / 24;
        context->GetString(R::string::durationDays, daysFormat, &ret);
    } else if (days > 0) {
        if (hours == 1) {
            context->GetString(R::string::durationDayHour, dayHourFormat, &ret);
            return ret;
        }
        context->GetString(R::string::durationDayHours, dayHourFormat, &ret);
    } else if (hours >= 2) {
        hours += (minutes + 30) / 60;
        context->GetString(R::string::durationHours, hoursFormat, &ret);
    } else if (hours > 0) {
        if (minutes == 1) {
            context->GetString(R::string::durationHourMinute, hourminuteFormat, &ret);
            return ret;
        }
        context->GetString(R::string::durationHourMinutes, hourminuteFormat, &ret);
    } else if (minutes >= 2) {
        minutes += (seconds + 30) / 60;
        context->GetString(R::string::durationMinutes, minutesFormat, &ret);
    } else if (minutes > 0) {
        if (seconds == 1) {
            context->GetString(R::string::durationMinuteSecond, minutesecondFormat, &ret);
            return ret;
        }
        context->GetString(R::string::durationMinuteSeconds, minutesecondFormat, &ret);
    } else if (seconds == 1) {
        context->GetString(R::string::durationSecond, secondsFormat, &ret);
        return ret;
    } else {
        context->GetString(R::string::durationSeconds, secondsFormat, &ret);
    }
    return ret;
}

} // namespace Format
} // namespace Text
} // namepsace Droid
} // namespace Elastos
