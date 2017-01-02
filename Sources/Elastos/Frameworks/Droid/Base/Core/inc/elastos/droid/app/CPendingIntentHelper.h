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

#ifndef __ELASTOS_DROID_APP_CPENDINGINTENTHELPER_H__
#define __ELASTOS_DROID_APP_CPENDINGINTENTHELPER_H__

#include "_Elastos_Droid_App_CPendingIntentHelper.h"
#include <elastos/core/Singleton.h>

using Elastos::Droid::Os::IUserHandle;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;

namespace Elastos {
namespace Droid {
namespace App {

CarClass(CPendingIntentHelper)
   , public Singleton
   , public IPendingIntentHelper
{
public:
   CAR_INTERFACE_DECL()

   CAR_SINGLETON_DECL()

     /**
      * Retrieve a PendingIntent that will start a new activity, like calling
      * {@link Context#startActivity(Intent) Context.startActivity(Intent)}.
      * Note that the activity will be started outside of the context of an
      * existing activity, so you must use the {@link Intent#FLAG_ACTIVITY_NEW_TASK
      * Intent.FLAG_ACTIVITY_NEW_TASK} launch flag in the Intent.
      *
      * @param context The Context in which this PendingIntent should start
      * the activity.
      * @param requestCode Private request code for the sender (currently
      * not used).
      * @param intent Intent of the activity to be launched.
      * @param flags May be {@link #FLAG_ONE_SHOT}, {@link #FLAG_NO_CREATE},
      * {@link #FLAG_CANCEL_CURRENT}, {@link #FLAG_UPDATE_CURRENT},
      * or any of the flags as supported by
      * {@link Intent#fillIn Intent.fillIn()} to control which unspecified parts
      * of the intent that can be supplied when the actual send happens.
      *
      * @return Returns an existing or new PendingIntent matching the given
      * parameters.  May return null only if {@link #FLAG_NO_CREATE} has been
      * supplied.
      */
      CARAPI GetActivity(
         /* [in] */ IContext* context,
         /* [in] */ Int32 requestCode,
         /* [in] */ IIntent* intent,
         /* [in] */ Int32 flags,
         /* [out] */ IPendingIntent** pendingIntent);

     /**
      * Retrieve a PendingIntent that will start a new activity, like calling
      * {@link Context#startActivity(Intent) Context.startActivity(Intent)}.
      * Note that the activity will be started outside of the context of an
      * existing activity, so you must use the {@link Intent#FLAG_ACTIVITY_NEW_TASK
      * Intent.FLAG_ACTIVITY_NEW_TASK} launch flag in the Intent.
      *
      * @param context The Context in which this PendingIntent should start
      * the activity.
      * @param requestCode Private request code for the sender (currently
      * not used).
      * @param intent Intent of the activity to be launched.
      * @param flags May be {@link #FLAG_ONE_SHOT}, {@link #FLAG_NO_CREATE},
      * {@link #FLAG_CANCEL_CURRENT}, {@link #FLAG_UPDATE_CURRENT},
      * or any of the flags as supported by
      * {@link Intent#fillIn Intent.fillIn()} to control which unspecified parts
      * of the intent that can be supplied when the actual send happens.
      * @param options Additional options for how the Activity should be started.
      * May be null if there are no options.
      *
      * @return Returns an existing or new PendingIntent matching the given
      * parameters.  May return null only if {@link #FLAG_NO_CREATE} has been
      * supplied.
      */
     CARAPI GetActivity(
         /* [in] */ IContext* context,
         /* [in] */ Int32 requestCode,
         /* [in] */ IIntent* intent,
         /* [in] */ Int32 flags,
         /* [in] */ IBundle* options,
         /* [out] */ IPendingIntent** pendingIntent);

     /**
      * @hide
      * Note that UserHandle.CURRENT will be interpreted at the time the
      * activity is started, not when the pending intent is created.
      */
     CARAPI GetActivityAsUser(
         /* [in] */ IContext* context,
         /* [in] */ Int32 requestCode,
         /* [in] */ IIntent* intent,
         /* [in] */ Int32 flags,
         /* [in] */ IBundle* options,
         /* [in] */ IUserHandle* user,
         /* [out] */ IPendingIntent** pendingIntent);

     /**
      * Like {@link #getActivity(Context, int, Intent, int)}, but allows an
      * array of Intents to be supplied.  The first Intent in the array is
      * taken as the primary key for the PendingIntent, like the single Intent
      * given to {@link #getActivity(Context, int, Intent, int)}.  Upon sending
      * the resulting PendingIntent, all of the Intents are started in the same
      * way as they would be by passing them to {@link Context#startActivities(Intent[])}.
      *
      * <p class="note">
      * The <em>first</em> intent in the array will be started outside of the context of an
      * existing activity, so you must use the {@link Intent#FLAG_ACTIVITY_NEW_TASK
      * Intent.FLAG_ACTIVITY_NEW_TASK} launch flag in the Intent.  (Activities after
      * the first in the array are started in the context of the previous activity
      * in the array, so FLAG_ACTIVITY_NEW_TASK is not needed nor desired for them.)
      * </p>
      *
      * <p class="note">
      * The <em>last</em> intent in the array represents the key for the
      * PendingIntent.  In other words, it is the significant element for matching
      * (as done with the single intent given to {@link #getActivity(Context, int, Intent, int)},
      * its content will be the subject of replacement by
      * {@link #send(Context, int, Intent)} and {@link #FLAG_UPDATE_CURRENT}, etc.
      * This is because it is the most specific of the supplied intents, and the
      * UI the user actually sees when the intents are started.
      * </p>
      *
      * @param context The Context in which this PendingIntent should start
      * the activity.
      * @param requestCode Private request code for the sender (currently
      * not used).
      * @param intents Array of Intents of the activities to be launched.
      * @param flags May be {@link #FLAG_ONE_SHOT}, {@link #FLAG_NO_CREATE},
      * {@link #FLAG_CANCEL_CURRENT}, {@link #FLAG_UPDATE_CURRENT},
      * or any of the flags as supported by
      * {@link Intent#fillIn Intent.fillIn()} to control which unspecified parts
      * of the intent that can be supplied when the actual send happens.
      *
      * @return Returns an existing or new PendingIntent matching the given
      * parameters.  May return null only if {@link #FLAG_NO_CREATE} has been
      * supplied.
      */
     CARAPI GetActivities(
         /* [in] */ IContext* context,
         /* [in] */ Int32 requestCode,
         /* [in] */ ArrayOf<IIntent*>* intents,
         /* [in] */ Int32 flags,
         /* [out] */ IPendingIntent** pendingIntent);

     /**
      * Like {@link #getActivity(Context, int, Intent, int)}, but allows an
      * array of Intents to be supplied.  The first Intent in the array is
      * taken as the primary key for the PendingIntent, like the single Intent
      * given to {@link #getActivity(Context, int, Intent, int)}.  Upon sending
      * the resulting PendingIntent, all of the Intents are started in the same
      * way as they would be by passing them to {@link Context#startActivities(Intent[])}.
      *
      * <p class="note">
      * The <em>first</em> intent in the array will be started outside of the context of an
      * existing activity, so you must use the {@link Intent#FLAG_ACTIVITY_NEW_TASK
      * Intent.FLAG_ACTIVITY_NEW_TASK} launch flag in the Intent.  (Activities after
      * the first in the array are started in the context of the previous activity
      * in the array, so FLAG_ACTIVITY_NEW_TASK is not needed nor desired for them.)
      * </p>
      *
      * <p class="note">
      * The <em>last</em> intent in the array represents the key for the
      * PendingIntent.  In other words, it is the significant element for matching
      * (as done with the single intent given to {@link #getActivity(Context, int, Intent, int)},
      * its content will be the subject of replacement by
      * {@link #send(Context, int, Intent)} and {@link #FLAG_UPDATE_CURRENT}, etc.
      * This is because it is the most specific of the supplied intents, and the
      * UI the user actually sees when the intents are started.
      * </p>
      *
      * @param context The Context in which this PendingIntent should start
      * the activity.
      * @param requestCode Private request code for the sender (currently
      * not used).
      * @param intents Array of Intents of the activities to be launched.
      * @param flags May be {@link #FLAG_ONE_SHOT}, {@link #FLAG_NO_CREATE},
      * {@link #FLAG_CANCEL_CURRENT}, {@link #FLAG_UPDATE_CURRENT},
      * or any of the flags as supported by
      * {@link Intent#fillIn Intent.fillIn()} to control which unspecified parts
      * of the intent that can be supplied when the actual send happens.
      *
      * @return Returns an existing or new PendingIntent matching the given
      * parameters.  May return null only if {@link #FLAG_NO_CREATE} has been
      * supplied.
      */
     CARAPI GetActivities(
         /* [in] */ IContext* context,
         /* [in] */ Int32 requestCode,
         /* [in] */ ArrayOf<IIntent*>* intents,
         /* [in] */ Int32 flags,
         /* [in] */ IBundle* options,
         /* [out] */ IPendingIntent** pendingIntent);

      /**
      * @hide
      * Note that UserHandle.CURRENT will be interpreted at the time the
      * activity is started, not when the pending intent is created.
      */
     CARAPI GetActivitiesAsUser(
         /* [in] */ IContext* context,
         /* [in] */ Int32 requestCode,
         /* [in] */ ArrayOf<IIntent*>* intents,
         /* [in] */ Int32 flags,
         /* [in] */ IBundle* options,
         /* [in] */ IUserHandle* user,
         /* [out] */ IPendingIntent** pendingIntent);

     /**
      * Retrieve a PendingIntent that will perform a broadcast, like calling
      * {@link Context#sendBroadcast(Intent) Context.sendBroadcast()}.
      *
      * @param context The Context in which this PendingIntent should perform
      * the broadcast.
      * @param requestCode Private request code for the sender (currently
      * not used).
      * @param intent The Intent to be broadcast.
      * @param flags May be {@link #FLAG_ONE_SHOT}, {@link #FLAG_NO_CREATE},
      * {@link #FLAG_CANCEL_CURRENT}, {@link #FLAG_UPDATE_CURRENT},
      * or any of the flags as supported by
      * {@link Intent#fillIn Intent.fillIn()} to control which unspecified parts
      * of the intent that can be supplied when the actual send happens.
      *
      * @return Returns an existing or new PendingIntent matching the given
      * parameters.  May return null only if {@link #FLAG_NO_CREATE} has been
      * supplied.
      */
     CARAPI GetBroadcast(
         /* [in] */ IContext* context,
         /* [in] */ Int32 requestCode,
         /* [in] */ IIntent* intent,
         /* [in] */ Int32 flags,
         /* [out] */ IPendingIntent** pendingIntent);

     /**
      * @hide
      * Note that UserHandle.CURRENT will be interpreted at the time the
      * broadcast is sent, not when the pending intent is created.
      */
     CARAPI GetBroadcastAsUser(
         /* [in] */ IContext* context,
         /* [in] */ Int32 requestCode,
         /* [in] */ IIntent* intent,
         /* [in] */ Int32 flags,
         /* [in] */ IUserHandle* userHandle,
         /* [out] */ IPendingIntent** pendingIntent);

     /**
      * Retrieve a PendingIntent that will start a service, like calling
      * {@link Context#startService Context.startService()}.  The start
      * arguments given to the service will come from the extras of the Intent.
      *
      * @param context The Context in which this PendingIntent should start
      * the service.
      * @param requestCode Private request code for the sender (currently
      * not used).
      * @param intent An Intent describing the service to be started.
      * @param flags May be {@link #FLAG_ONE_SHOT}, {@link #FLAG_NO_CREATE},
      * {@link #FLAG_CANCEL_CURRENT}, {@link #FLAG_UPDATE_CURRENT},
      * or any of the flags as supported by
      * {@link Intent#fillIn Intent.fillIn()} to control which unspecified parts
      * of the intent that can be supplied when the actual send happens.
      *
      * @return Returns an existing or new PendingIntent matching the given
      * parameters.  May return null only if {@link #FLAG_NO_CREATE} has been
      * supplied.
      */
     CARAPI GetService(
         /* [in] */ IContext* context,
         /* [in] */ Int32 requestCode,
         /* [in] */ IIntent* intent,
         /* [in] */ Int32 flags,
         /* [out] */ IPendingIntent** pendingIntent);

     /**
      * Convenience function for writing either a PendingIntent or null pointer to
      * a Parcel.  You must use this with {@link #readPendingIntentOrNullFromParcel}
      * for later reading it.
      *
      * @param sender The PendingIntent to write, or null.
      * @param out Where to write the PendingIntent.
      */
     CARAPI WritePendingIntentOrNullToParcel(
         /* [in] */ IPendingIntent* sender,
         /* [in] */ IParcel* out);

     /**
      * Convenience function for reading either a Messenger or null pointer from
      * a Parcel.  You must have previously written the Messenger with
      * {@link #writePendingIntentOrNullToParcel}.
      *
      * @param in The Parcel containing the written Messenger.
      *
      * @return Returns the Messenger read from the Parcel, or null if null had
      * been written.
      */
     CARAPI ReadPendingIntentOrNullFromParcel(
         /* [in] */ IParcel* in,
         /* [out] */ IPendingIntent** messenger);

private:
};

} // namespace App
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_APP_CPENDINGINTENTHELPER_H__
