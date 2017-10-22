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

#ifndef __ELASTOS_DROID_TEXT_FORMAT_CTIMEFORMATTER_H__
#define __ELASTOS_DROID_TEXT_FORMAT_CTIMEFORMATTER_H__

#include "_Elastos_Droid_Text_Format_CTimeFormatter.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/StringBuilder.h>

using Elastos::Core::StringBuilder;
using Elastos::Core::ICharSequence;
using Elastos::Utility::ILocale;
using Elastos::IO::ICharBuffer;
using Libcore::ICU::ILocaleData;
using Libcore::Utility::IZoneInfoWallTime;
using Libcore::Utility::IZoneInfo;

namespace Elastos {
namespace Droid {
namespace Text {
namespace Format {

CarClass(CTimeFormatter)
    , public Object
    , public ITimeFormatter
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CTimeFormatter();

    virtual ~CTimeFormatter();

    CARAPI constructor();

    CARAPI Format(
        /* [in] */ const String& pattern,
        /* [in] */ IZoneInfoWallTime* wallTime,
        /* [in] */ IZoneInfo* zoneInfo,
        /* [out] */ String* ret);

private:
    CARAPI_(String) LocalizeDigits(
        /* [in] */ const String& s);

    CARAPI FormatInternal(
        /* [in] */ const String& pattern,
        /* [in] */ IZoneInfoWallTime* wallTime,
        /* [in] */ IZoneInfo* zoneInfo);

    CARAPI_(Boolean) HandleToken(
        /* [in] */ ICharBuffer* formatBuffer,
        /* [in] */ IZoneInfoWallTime* wallTime,
        /* [in] */ IZoneInfo* zoneInfo);

    CARAPI ModifyAndAppend(
        /* [in] */ ICharSequence* str,
        /* [in] */ Int32 modifier);

    CARAPI OutputYear(
        /* [in] */ Int32 value,
        /* [in] */ Boolean outputTop,
        /* [in] */ Boolean outputBottom,
        /* [in] */ Int32 modifier);

    static CARAPI_(String) GetFormat(
        /* [in] */ Int32 modifier,
        /* [in] */ const String& normal,
        /* [in] */ const String& underscore,
        /* [in] */ const String& dash,
        /* [in] */ const String& zero);

    static CARAPI_(Boolean) IsLeap(
        /* [in] */ Int32 year);

    static CARAPI_(Boolean) BrokenIsUpper(
        /* [in] */ Char32 toCheck);

    static CARAPI_(Boolean) BrokenIsLower(
        /* [in] */ Char32 toCheck);

    static CARAPI_(Char32) BrokenToLower(
        /* [in] */ Char32 input);

    static CARAPI_(Char32) BrokenToUpper(
        /* [in] */ Char32 input);

private:
    // An arbitrary value outside the range representable by a char.
    static const Int32 FORCE_LOWER_CASE;
    static const Int32 SECSPERMIN;
    static const Int32 MINSPERHOUR;
    static const Int32 DAYSPERWEEK;
    static const Int32 MONSPERYEAR;
    static const Int32 HOURSPERDAY;
    static const Int32 DAYSPERLYEAR;
    static const Int32 DAYSPERNYEAR;

    /**
     * The Locale for which the cached LocaleData and formats have been loaded.
     */
    static AutoPtr<ILocale> sLocale;
    static AutoPtr<ILocaleData> sLocaleData;
    static String sTimeOnlyFormat;
    static String sDateOnlyFormat;
    static String sDateTimeFormat;

    /*const*/ AutoPtr<ILocaleData> mLocaleData;
    /*const*/ String mDateTimeFormat;
    /*const*/ String mTimeOnlyFormat;
    /*const*/ String mDateOnlyFormat;

    StringBuilder mOutputBuilder;
    AutoPtr<Elastos::Utility::IFormatter> mNumberFormatter;
    Object mLock;
};

} // namespace Format
} // namespace Text
} // namespace Droid
} // namespace Elastos
#endif //__ELASTOS_DROID_TEXT_FORMAT_TIMEFORMATTER_H__
