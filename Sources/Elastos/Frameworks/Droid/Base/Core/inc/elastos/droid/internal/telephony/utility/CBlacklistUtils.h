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

#ifndef __ELASTOS_DROID_INTERNAL_TELEPHONY_UTILITY_CBLACKLISTUTILS_H__
#define __ELASTOS_DROID_INTERNAL_TELEPHONY_UTILITY_CBLACKLISTUTILS_H__

#include "_Elastos_Droid_Internal_Telephony_Utility_CBlacklistUtils.h"
#include "elastos/core/Singleton.h"

using Elastos::Droid::Content::IContext;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace Utility {

CarClass(CBlacklistUtils)
    , public Singleton
    , public IBlacklistUtils
{
public:
    CAR_INTERFACE_DECL();

    CAR_SINGLETON_DECL();

    CARAPI AddOrUpdate(
        /* [in] */ IContext* context,
        /* [in] */ const String& number,
        /* [in] */ Int32 flags,
        /* [in] */ Int32 valid,
        /* [out] */ Boolean* result);

    CARAPI IsListed(
        /* [in] */ IContext* context,
        /* [in] */ const String& number,
        /* [in] */ Int32 mode,
        /* [out] */ Int32* result);

    CARAPI IsBlacklistEnabled(
        /* [in] */ IContext* context,
        /* [out] */ Boolean* result);

    CARAPI IsBlacklistNotifyEnabled(
        /* [in] */ IContext* context,
        /* [out] */ Boolean* result);

    CARAPI IsBlacklistPrivateNumberEnabled(
        /* [in] */ IContext* context,
        /* [in] */ Int32 mode,
        /* [out] */ Boolean* result);

    CARAPI IsBlacklistUnknownNumberEnabled(
        /* [in] */ IContext* context,
        /* [in] */ Int32 mode,
        /* [out] */ Boolean* result);

    CARAPI IsBlacklistRegexEnabled(
        /* [in] */ IContext* context,
        /* [out] */ Boolean* result);
};

} // namespace Utility
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_TELEPHONY_UTILITY_CBLACKLISTUTILS_H__
