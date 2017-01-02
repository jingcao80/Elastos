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
    args->Set(0, CoreUtils::Convert(value));
    args->Set(1, CoreUtils::Convert(resStr));
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

    AutoPtr<ArrayOf<IInterface*> > args;
    String ret;

    if (days >= 2) {
        days += (hours + 12) / 24;
        args = ArrayOf<IInterface*>::Alloc(1);
        args->Set(0, CoreUtils::Convert(days));
        context->GetString(R::string::durationDays, args, &ret);
        return ret;
    }
    else if (days > 0) {
        args = ArrayOf<IInterface*>::Alloc(2);
        args->Set(0, CoreUtils::Convert(days));
        args->Set(1, CoreUtils::Convert(hours));
        if (hours == 1) {
            context->GetString(R::string::durationDayHour, args, &ret);
            return ret;
        }
        context->GetString(R::string::durationDayHours, args, &ret);
        return ret;
    }
    else if (hours >= 2) {
        hours += (minutes + 30) / 60;
        args = ArrayOf<IInterface*>::Alloc(1);
        args->Set(0, CoreUtils::Convert(hours));
        context->GetString(R::string::durationHours, args, &ret);
        return ret;
    }
    else if (hours > 0) {
        args = ArrayOf<IInterface*>::Alloc(2);
        args->Set(0, CoreUtils::Convert(hours));
        args->Set(1, CoreUtils::Convert(minutes));
        if (minutes == 1) {
            context->GetString(R::string::durationHourMinute, args, &ret);
            return ret;
        }
        context->GetString(R::string::durationHourMinutes, args, &ret);
        return ret;
    }
    else if (minutes >= 2) {
        minutes += (seconds + 30) / 60;
        args = ArrayOf<IInterface*>::Alloc(1);
        args->Set(0, CoreUtils::Convert(minutes));
        context->GetString(R::string::durationMinutes, args, &ret);
        return ret;
    }
    else if (minutes > 0) {
        args = ArrayOf<IInterface*>::Alloc(2);
        args->Set(0, CoreUtils::Convert(minutes));
        args->Set(1, CoreUtils::Convert(seconds));
        if (seconds == 1) {
            context->GetString(R::string::durationMinuteSecond, args, &ret);
            return ret;
        }
        context->GetString(R::string::durationMinuteSeconds, args, &ret);
        return ret;
    }
    else if (seconds == 1) {
        args = ArrayOf<IInterface*>::Alloc(1);
        args->Set(0, CoreUtils::Convert(seconds));
        context->GetString(R::string::durationSecond, args, &ret);
        return ret;
    }
    else {
        args = ArrayOf<IInterface*>::Alloc(1);
        args->Set(0, CoreUtils::Convert(seconds));
        context->GetString(R::string::durationSeconds, args, &ret);
        return ret;
    }
}

} // namespace Format
} // namespace Text
} // namepsace Droid
} // namespace Elastos
