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

#ifndef __ELASTOS_DROID_INTERNAL_TELEPHONY_UTILITY_BLACKLISTUTILS_H
#define __ELASTOS_DROID_INTERNAL_TELEPHONY_UTILITY_BLACKLISTUTILS_H

#include "elastos/droid/ext/frameworkext.h"
#include "Elastos.Droid.Provider.h"

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Provider::ISettingsSystem;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace Utility {

/**
 * Blacklist Utility Class
 * @hide
 */
class BlacklistUtils
{
public:
    static Boolean AddOrUpdate(
        /* [in] */ IContext* context,
        /* [in] */ const String& number,
        /* [in] */ Int32 flags,
        /* [in] */ Int32 valid);

    /**
     * Check if the number is in the blacklist
     * @param number: Number to check
     * @return one of: MATCH_NONE, MATCH_PRIVATE, MATCH_UNKNOWN, MATCH_LIST or MATCH_REGEX
     */
    static Int32 IsListed(
        /* [in] */ IContext* context,
        /* [in] */ const String& number,
        /* [in] */ Int32 mode);

    static Boolean IsBlacklistEnabled(
        /* [in] */ IContext* context);

    static Boolean IsBlacklistNotifyEnabled(
        /* [in] */ IContext* context);

    static Boolean IsBlacklistPrivateNumberEnabled(
        /* [in] */ IContext* context,
        /* [in] */ Int32 mode);

    static Boolean IsBlacklistUnknownNumberEnabled(
        /* [in] */ IContext* context,
        /* [in] */ Int32 mode);

    static Boolean IsBlacklistRegexEnabled(
        /* [in] */ IContext* context);

private:
    static const String TAG;// = "BlacklistUtils";
    static const Boolean DEBUG;// = FALSE;

};

} // namespace Utility
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_TELEPHONY_UTILITY_BLACKLISTUTILS_H
