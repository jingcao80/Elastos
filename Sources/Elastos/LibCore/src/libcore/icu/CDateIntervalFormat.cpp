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

#include "CDateIntervalFormat.h"
#include "DateIntervalFormat.h"

namespace Libcore {
namespace ICU {

CAR_SINGLETON_IMPL(CDateIntervalFormat)

CAR_INTERFACE_IMPL(CDateIntervalFormat, Singleton, IDateIntervalFormat)

ECode CDateIntervalFormat::FormatDateRange(
    /* [in] */ Int64 startMs,
    /* [in] */ Int64 endMs,
    /* [in] */ Int32 flags,
    /* [in] */ const String& olsonId,
    /* [out] */ String* rst)
{
    VALIDATE_NOT_NULL(rst);

    *rst = DateIntervalFormat::FormatDateRange(startMs, endMs, flags, olsonId);
    return NOERROR;
}

ECode CDateIntervalFormat::FormatDateRange(
    /* [in] */ ILocale* locale,
    /* [in] */ ITimeZone* tz,
    /* [in] */ Int64 startMs,
    /* [in] */ Int64 endMs,
    /* [in] */ Int32 flags,
    /* [out] */ String* rst)
{
    VALIDATE_NOT_NULL(rst);

    *rst = DateIntervalFormat::FormatDateRange(locale, tz, startMs, endMs, flags);
    return NOERROR;
}

} // ICU
} // Libcore
