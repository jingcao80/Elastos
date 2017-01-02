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

#ifndef __ELASTOS_DROID_SERVER_TELECOM_SPAMBLOCKER_H__
#define __ELASTOS_DROID_SERVER_TELECOM_SPAMBLOCKER_H__

#include "_Elastos.Droid.Server.Telecom.h"
#include "elastos/droid/server/telecom/CallsManagerListenerBase.h"
#include <elastos/droid/ext/frameworkext.h>
#include <elastos/core/Object.h>

using Elastos::Droid::Content::IContext;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Telecom {

/**
 * Listens for call updates and records whether calls should be blocked based on
 * caller info results.  Can also hang up calls while they are ringing, in case
 * they could not be blocked in time.
 */
class SpamBlocker
    : public CallsManagerListenerBase
{
public:
    CARAPI constructor(
        /* [in] */ IContext* context);

    CARAPI ShouldBlock(
        /* [in] */ const String& number,
        /* [out] */ Boolean* result);
};

} // namespace Telecom
} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SERVER_TELECOM_SPAMBLOCKER_H__
