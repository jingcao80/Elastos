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

#ifndef __LIBCORE_ICU_CDATEINTERVALFORMAT_H__
#define __LIBCORE_ICU_CDATEINTERVALFORMAT_H__

#include "Singleton.h"
#include "_Libcore_ICU_CDateIntervalFormat.h"

using Elastos::Core::Singleton;
using Elastos::Utility::ILocale;
using Elastos::Utility::ITimeZone;

namespace Libcore {
namespace ICU {

CarClass(CDateIntervalFormat)
    , public Singleton
    , public IDateIntervalFormat
{
    CAR_SINGLETON_DECL()

public:
    CAR_INTERFACE_DECL()

    CARAPI FormatDateRange(
        /* [in] */ Int64 startMs,
        /* [in] */ Int64 endMs,
        /* [in] */ Int32 flags,
        /* [in] */ const String& olsonId,
        /* [out] */ String* rst);

    CARAPI FormatDateRange(
        /* [in] */ ILocale* locale,
        /* [in] */ ITimeZone* tz,
        /* [in] */ Int64 startMs,
        /* [in] */ Int64 endMs,
        /* [in] */ Int32 flags,
        /* [out] */ String* rst);

};

} // ICU
} // Libcore

#endif // __LIBCORE_ICU_CDATEINTERVALFORMAT_H__