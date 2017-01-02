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

#ifndef __ELASTOS_DROID_DIALER_CALLLOG_CCALLLOGNOTIFICATIONSSERVICE_H__
#define __ELASTOS_DROID_DIALER_CALLLOG_CCALLLOGNOTIFICATIONSSERVICE_H__

#include "_Elastos_Droid_Dialer_CallLog_CCallLogNotificationsService.h"
#include "elastos/droid/app/IntentService.h"
#include "elastos/droid/dialer/calllog/CallLogQueryHandler.h"

using Elastos::Droid::App::IntentService;

namespace Elastos{
namespace Droid{
namespace Dialer {
namespace CallLog {

/**
 * Provides operations for managing notifications.
 * <p>
 * It handles the following actions:
 * <ul>
 * <li>{@link #ACTION_MARK_NEW_VOICEMAILS_AS_OLD}: marks all the new voicemails in the call log as
 * old; this is called when a notification is dismissed.</li>
 * <li>{@link #ACTION_UPDATE_NOTIFICATIONS}: updates the content of the new items notification; it
 * may include an optional extra {@link #EXTRA_NEW_VOICEMAIL_URI}, containing the URI of the new
 * voicemail that has triggered this update (if any).</li>
 * </ul>
 */
CarClass(CCallLogNotificationsService)
    , public IntentService
    , public ICallLogNotificationsService
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor();

    // @Override
    CARAPI OnCreate();

protected:
    // @Override
    CARAPI OnHandleIntent(
        /* [in] */ IIntent* intent);

private:
    static const String TAG;

    AutoPtr<CallLogQueryHandler> mCallLogQueryHandler;
};

} // CallLog
} // Dialer
} // Droid
} // Elastos

#endif //__ELASTOS_DROID_DIALER_CALLLOG_CCALLLOGNOTIFICATIONSSERVICE_H__
