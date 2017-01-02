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

#include "elastos/droid/media/WebVttParserHelper.h"
#include <elastos/core/StringUtils.h>

using Elastos::Core::StringUtils;

namespace Elastos {
namespace Droid {
namespace Media {

ECode WebVttParserHelper::ParseFloatPercentage(
    /* [in] */ const String& s,
    /* [out] */ Float* result)
{
    VALIDATE_NOT_NULL(result);

    if (!s.EndWith(String("%"))) {
        return E_NUMBER_FORMAT_EXCEPTION;
        // throw new NumberFormatException("does not end in %");
    }
    Int32 len = s.GetLength();
    String str = s;
    str = str.Substring(0, len - 1);
    // parseFloat allows an exponent or a sign

    Boolean flag = FALSE;
    StringUtils::Matches(str, String(".*[^0-9.].*"), &flag);
    if (flag) {
        // throw new NumberFormatException("contains an invalid character");
        return E_NUMBER_FORMAT_EXCEPTION;
    }

    // try {
    Float value = StringUtils::ParseFloat(str);
    if (value < 0.0f || value > 100.0f) {
        // throw new NumberFormatException("is out of range");
        return E_NUMBER_FORMAT_EXCEPTION;
    }

    *result = value;
    return NOERROR;
    // } catch (NumberFormatException e) {
    // throw new NumberFormatException("is not a number");
    // }

}

ECode WebVttParserHelper::ParseIntPercentage(
    /* [in] */ const String& s,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);

    if (!s.EndWith(String("%"))) {
        // throw new NumberFormatException("does not end in %");
        return E_NUMBER_FORMAT_EXCEPTION;
    }
    Int32 len = s.GetLength();
    String str = s;
    str = str.Substring(0, len - 1);
    // parseInt allows "-0" that returns 0, so check for non-digits
    Boolean flag = FALSE;
    StringUtils::Matches(str, String(".*[^0-9].*"), &flag);
    if (flag) {
        return E_NUMBER_FORMAT_EXCEPTION;
        // throw new NumberFormatException("contains an invalid character");
    }

    // try {
    Int32 value = StringUtils::ParseInt32(s);
    if (value < 0 || value > 100) {
        // throw new NumberFormatException("is out of range");
        return E_NUMBER_FORMAT_EXCEPTION;
    }
    *result = value;
    return NOERROR;
    // } catch (NumberFormatException e) {
        // throw new NumberFormatException("is not a number");
    // }

}

ECode WebVttParserHelper::ParseTimestampMs(
    /* [in] */ const String& s,
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result);

    Boolean flag = FALSE;
    if (!StringUtils::Matches(s, String("(\\d+:)?[0-5]\\d:[0-5]\\d\\.\\d{3}"), &flag)) {
        // throw new NumberFormatException("has invalid format");
        return E_NUMBER_FORMAT_EXCEPTION;
    }

    AutoPtr<ArrayOf<String> > parts;
    StringUtils::Split(s, String("\\."), 2, (ArrayOf<String>**)&parts);

    Int64 value = 0;
    AutoPtr<ArrayOf<String> > arrays;
    StringUtils::Split((*parts)[0], String(":"), (ArrayOf<String>**)&arrays);

    Int32 len = arrays->GetLength();
    for (Int32 i = 0; i < len; ++i) {
        value = value * 60 + StringUtils::ParseInt64((*arrays)[i]);
    }

    *result = value * 1000 + StringUtils::ParseInt64((*parts)[1]);
    return NOERROR;
}

ECode WebVttParserHelper::TimeToString(
    /* [in] */ Int64 timeMs,
    /* [out] */ String* result)
{

    VALIDATE_NOT_NULL(result);
    assert(0 && "TODO");
    // return String.format("%d:%02d:%02d.%03d",
    //         timeMs / 3600000, (timeMs / 60000) % 60,
    //         (timeMs / 1000) % 60, timeMs % 1000);
    return NOERROR;
}

} // namespace Media
} // namepsace Droid
} // namespace Elastos