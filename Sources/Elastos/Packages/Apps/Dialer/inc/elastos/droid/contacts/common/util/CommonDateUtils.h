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

#ifndef __ELASTOS_DROID_CONTACTS_COMMON_UTIL_COMMONDATEUTILS_H__
#define __ELASTOS_DROID_CONTACTS_COMMON_UTIL_COMMONDATEUTILS_H__

#include "_Elastos.Droid.Dialer.h"

using Elastos::Text::ISimpleDateFormat;

namespace Elastos {
namespace Droid {
namespace Contacts {
namespace Common {
namespace Util {

/**
 * Common date utilities.
 */
class CommonDateUtils
{
public:
    // All the SimpleDateFormats in this class use the UTC timezone
    static const AutoPtr<ISimpleDateFormat> NO_YEAR_DATE_FORMAT;
    static const AutoPtr<ISimpleDateFormat> FULL_DATE_FORMAT;
    static const AutoPtr<ISimpleDateFormat> DATE_AND_TIME_FORMAT;
    static const AutoPtr<ISimpleDateFormat> NO_YEAR_DATE_AND_TIME_FORMAT;

    /**
     * Exchange requires 8:00 for birthdays
     */
    static const Int32 DEFAULT_HOUR = 8;
};

} // namespace Util
} // namespace Common
} // namespace Contacts
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_CONTACTS_COMMON_UTIL_COMMONDATEUTILS_H__
