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

#ifndef __ELASTOS_DROID_APP_CNOTIFICATIONACTIONWEARABLEEXTENDER_H__
#define __ELASTOS_DROID_APP_CNOTIFICATIONACTIONWEARABLEEXTENDER_H__

#include "_Elastos_Droid_App_CNotificationActionWearableExtender.h"
#include "elastos/droid/app/NotificationActionWearableExtender.h"

namespace Elastos {
namespace Droid {
namespace App {

/**
 * Wearable extender for notification actions. To add extensions to an action,
 * create a new {@link android.app.Notification.Action.WearableExtender} object using
 * the {@code WearableExtender()} constructor and apply it to a
 * {@link android.app.Notification.Action.Builder} using
 * {@link android.app.Notification.Action.Builder#extend}.
 *
 * <pre class="prettyprint">
 * Notification.Action action = new Notification.Action.Builder(
 *         R.drawable.archive_all, "Archive all", actionIntent)
 *         .extend(new Notification.Action.WearableExtender()
 *                 .setAvailableOffline(false))
 *         .build();</pre>
 */
CarClass(CNotificationActionWearableExtender)
    , public NotificationActionWearableExtender
{
public:
    CAR_OBJECT_DECL()
};

} // namespace App
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_APP_CNOTIFICATIONACTIONWEARABLEEXTENDER_H__
