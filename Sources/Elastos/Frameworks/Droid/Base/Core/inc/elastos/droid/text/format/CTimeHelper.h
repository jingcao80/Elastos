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

#ifndef __ELASTOS_DROID_TEXT_FORMAT_CTIMEHELPER_H__
#define __ELASTOS_DROID_TEXT_FORMAT_CTIMEHELPER_H__

#include "_Elastos_Droid_Text_Format_CTimeHelper.h"
#include <elastos/core/Singleton.h>

using Elastos::Core::Singleton;

namespace Elastos {
namespace Droid {
namespace Text {
namespace Format {

CarClass(CTimeHelper)
    , public Singleton
    , public ITimeHelper
{
public:
    CAR_SINGLETON_DECL()

    CAR_INTERFACE_DECL()

    CARAPI Compare(
        /* [in] */ ITime* a,
        /* [in] */ ITime* b,
        /* [out] */ Int32* ret);

    CARAPI GetCurrentTimezone(
        /* [out] */ String* ret);

    CARAPI IsEpoch(
        /* [in] */ ITime* time,
        /* [out] */ Boolean* ret);

    CARAPI GetJulianDay(
        /* [in] */ Int64 millis,
        /* [in] */ Int64 gmtoff,
        /* [out] */ Int32* ret);

    CARAPI GetWeeksSinceEpochFromJulianDay(
        /* [in] */ Int32 julianDay,
        /* [in] */ Int32 firstDayOfWeek,
        /* [out] */ Int32* ret);

    CARAPI GetJulianMondayFromWeeksSinceEpoch(
        /* [in] */ Int32 week,
        /* [out] */ Int32* ret);
};

} // namespace Format
} // namespace Text
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_TEXT_FORMAT_CTIMEHELPER_H__
