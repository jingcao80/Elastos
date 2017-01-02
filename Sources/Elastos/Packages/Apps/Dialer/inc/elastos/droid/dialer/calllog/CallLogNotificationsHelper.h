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

#ifndef __ELASTOS_DROID_DIALER_CALLLOG_CALLLOGNOTIFICATIONSHELPER_H__
#define __ELASTOS_DROID_DIALER_CALLLOG_CALLLOGNOTIFICATIONSHELPER_H__

#include "_Elastos.Droid.Dialer.h"
#include "elastos/core/Object.h"

using Elastos::Droid::Content::IContext;

namespace Elastos {
namespace Droid {
namespace Dialer {
namespace CallLog {

/**
 * Helper class operating on call log notifications.
 */
class CallLogNotificationsHelper
    : public Object
{
public:
    /** Removes the missed call notifications. */
    static CARAPI_(void) RemoveMissedCallNotifications(
        /* [in] */ IContext* context);

    /** Update the voice mail notifications. */
    static CARAPI_(void) UpdateVoicemailNotifications(
        /* [in] */ IContext* context);

};

} // CallLog
} // Dialer
} // Droid
} // Elastos

#endif //__ELASTOS_DROID_DIALER_CALLLOG_CALLLOGNOTIFICATIONSHELPER_H__
