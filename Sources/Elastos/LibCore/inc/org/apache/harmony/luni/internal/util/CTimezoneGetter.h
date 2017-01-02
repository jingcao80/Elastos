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

#ifndef __ORG_APACHE_HARMONY_LUNI_INTERNAL_UTIL_CTIMEZONEGETTER_H__
#define __ORG_APACHE_HARMONY_LUNI_INTERNAL_UTIL_CTIMEZONEGETTER_H__

#include "_Org_Apache_Harmony_Luni_Internal_Util_CTimezoneGetter.h"
#include "org/apache/harmony/luni/internal/util/TimezoneGetter.h"

namespace Org {
namespace Apache {
namespace Harmony {
namespace Luni {
namespace Internal {
namespace Util {

CarClass(CTimezoneGetter)
    , public TimezoneGetter
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Util
} // namespace Internal
} // namespace Luni
} // namespace Harmony
} // namespace Apache
} // namespace Org

#endif // __ORG_APACHE_HARMONY_LUNI_INTERNAL_UTIL_CTIMEZONEGETTER_H__
