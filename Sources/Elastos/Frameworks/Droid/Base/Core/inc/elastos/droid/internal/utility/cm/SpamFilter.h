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

#ifndef __ELASTOS_DROID_INTERNAL_UTILITY_CM_SPAMFILTER_H__
#define __ELASTOS_DROID_INTERNAL_UTILITY_CM_SPAMFILTER_H__

#include "Elastos.Droid.App.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Internal.h"
#include "Elastos.Droid.Os.h"
#include "elastos/droid/ext/frameworkdef.h"

using Elastos::Droid::App::INotification;
using Elastos::Droid::Net::IUri;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Utility {
namespace Cm {

class SpamFilter
{
public:
    static CARAPI GetUri(
        /* [out] */ IUri** uri);

    static CARAPI GetNormalizedContent(
        /* [in] */ const String& msg,
        /* [out] */ String* result);

    static CARAPI GetNotificationContent(
        /* [in] */ INotification* notification,
        /* [out] */ String* result);
};

} // namespace Cm
} // namespace Utility
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_INTERNAL_UTILITY_CM_SPAMFILTER_H__
