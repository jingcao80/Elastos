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

#include "elastos/droid/contacts/common/util/TelephonyManagerUtils.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Contacts {
namespace Common {
namespace Util {

const String TelephonyManagerUtils::TAG("TelephonyManagerUtils");

ECode TelephonyManagerUtils::GetVoiceMailAlphaTag(
    /* [in] */ IContext* context,
    /* [out] */ String* tag)
{
    assert(0);
    return NOERROR;
}

ECode TelephonyManagerUtils::GetCurrentCountryIso(
    /* [in] */ IContext* context,
    /* [in] */ ILocale* locale,
    /* [out] */ String* iso)
{
    VALIDATE_NOT_NULL(iso);
    Logger::E("TelephonyManagerUtils", "TODO GetCurrentCountryIso no impl, default US");
    *iso = String("US");
    //assert(0);
    return NOERROR;
}

Boolean TelephonyManagerUtils::HasVideoCallSubscription(
    /* [in] */ IContext* context)
{
    assert(0);
    return FALSE;
}

} // namespace Util
} // namespace Common
} // namespace Contacts
} // namespace Droid
} // namespace Elastos
