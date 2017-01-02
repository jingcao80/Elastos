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

#ifndef __ELASTOS_DROID_APP_CTASKSTACKBUILDER_H__
#define __ELASTOS_DROID_APP_CTASKSTACKBUILDER_H__

#include "elastos/droid/ext/frameworkext.h"
#include "Elastos.Droid.Content.h"
#include "_Elastos_Droid_App_CTaskStackBuilder.h"
#include <elastos/core/Object.h>
#include <elastos/utility/etl/List.h>

using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Os::IUserHandle;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::IComponentName;
using Elastos::Utility::Etl::List;


namespace Elastos {
namespace Droid {
namespace App {

/**
 * Utility class for constructing synthetic back stacks for cross-task navigation
 * on Android 3.0 and newer.
 *
 * <p>In API level 11 (Android 3.0/Honeycomb) the recommended conventions for
 * app navigation using the back key changed. The back key's behavior is local
 * to the current task and does not capture navigation across different tasks.
 * Navigating across tasks and easily reaching the previous task is accomplished
 * through the "recents" UI, accessible through the software-provided Recents key
 * on the navigation or system bar. On devices with the older hardware button configuration
 * the recents UI can be accessed with a long press on the Home key.</p>
 *
 * <p>When crossing from one task stack to another post-Android 3.0,
 * the application should synthesize a back stack/history for the new task so that
 * the user may navigate out of the new task and back to the Launcher by repeated
 * presses of the back key. Back key presses should not navigate across task stacks.</p>
 *
 * <p>TaskStackBuilder provides a way to obey the correct conventions
 * around cross-task navigation.</p>
 *
 * <div class="special reference">
 * <h3>About Navigation</h3>
 * For more detailed information about tasks, the back stack, and navigation design guidelines,
 * please read
 * <a href="{@docRoot}guide/topics/fundamentals/tasks-and-back-stack.html">Tasks and Back Stack</a>
 * from the developer guide and <a href="{@docRoot}design/patterns/navigation.html">Navigation</a>
 * from the design guide.
 * </div>
 */
CarClass(CTaskStackBuilder)
    , public Object
    , public ITaskStackBuilder
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CTaskStackBuilder();

    ~CTaskStackBuilder();

    CARAPI constructor(
        /* [in] */ IContext * context);

    /**
     * Return a new TaskStackBuilder for launching a fresh task stack consisting
     * of a series of activities.
     *
     * @param context The context that will launch the new task stack or generate a PendingIntent
     * @return A new TaskStackBuilder
     */
    static CARAPI_(AutoPtr<ITaskStackBuilder>) Create(
        /* [in] */ IContext * context);

    /**
     * Add a new Intent to the task stack. The most recently added Intent will invoke
     * the Activity at the top of the task stack.
     *
     * @param nextIntent Intent for the next Activity in the synthesized task stack
     * @return This TaskStackBuilder for method chaining
     */
    CARAPI AddNextIntent(
        /* [in] */ IIntent * nextIntent);

    /**
     * Add a new Intent with the resolved chain of parents for the target activity to
     * the task stack.
     *
     * <p>This is equivalent to calling {@link #addParentStack(ComponentName) addParentStack}
     * with the resolved ComponentName of nextIntent (if it can be resolved), followed by
     * {@link #addNextIntent(Intent) addNextIntent} with nextIntent.</p>
     *
     * @param nextIntent Intent for the topmost Activity in the synthesized task stack.
     *                   Its chain of parents as specified in the manifest will be added.
     * @return This TaskStackBuilder for method chaining.
     */
    CARAPI AddNextIntentWithParentStack(
        /* [in] */ IIntent * nextIntent);

    /**
     * Add the activity parent chain as specified by the
     * {@link Activity#getParentActivityIntent() getParentActivityIntent()} method of the activity
     * specified and the {@link android.R.attr#parentActivityName parentActivityName} attributes
     * of each successive activity (or activity-alias) element in the application's manifest
     * to the task stack builder.
     *
     * @param sourceActivity All parents of this activity will be added
     * @return This TaskStackBuilder for method chaining
     */
    CARAPI AddParentStack(
        /* [in] */ IActivity * sourceActivity);

    /**
     * Add the activity parent chain as specified by the
     * {@link android.R.attr#parentActivityName parentActivityName} attribute of the activity
     * (or activity-alias) element in the application's manifest to the task stack builder.
     *
     * @param sourceActivityClass All parents of this activity will be added
     * @return This TaskStackBuilder for method chaining
     */
    // public TaskStackBuilder addParentStack(Class<?> sourceActivityClass)

    /**
     * Add the activity parent chain as specified by the
     * {@link android.R.attr#parentActivityName parentActivityName} attribute of the activity
     * (or activity-alias) element in the application's manifest to the task stack builder.
     *
     * @param sourceActivityName Must specify an Activity component. All parents of
     *                           this activity will be added
     * @return This TaskStackBuilder for method chaining
     */
    CARAPI AddParentStack(
        /* [in] */ IComponentName * sourceActivityName);

    /**
     * @return the number of intents added so far.
     */
    CARAPI GetIntentCount(
        /* [out] */ Int32 * num);

    /**
     * Return the intent at the specified index for modification.
     * Useful if you need to modify the flags or extras of an intent that was previously added,
     * for example with {@link #addParentStack(Activity)}.
     *
     * @param index Index from 0-getIntentCount()
     * @return the intent at position index
     */
    CARAPI EditIntentAt(
        /* [in] */ Int32 index,
        /* [out] */ IIntent **intent);

    /**
     * Start the task stack constructed by this builder.
     */
    CARAPI StartActivities();

    /**
     * Start the task stack constructed by this builder.
     * @hide
     */
    CARAPI StartActivities(
        /* [in] */ IBundle * options,
        /* [in] */ IUserHandle * userHandle);

    /**
     * Start the task stack constructed by this builder.
     *
     * @param options Additional options for how the Activity should be started.
     * See {@link android.content.Context#startActivity(Intent, Bundle)
     * Context.startActivity(Intent, Bundle)} for more details.
     */
    CARAPI StartActivities(
        /* [in] */ IBundle * options);

    /**
     * Obtain a {@link PendingIntent} for launching the task constructed by this builder so far.
     *
     * @param requestCode request code for the sender
     * @param flags May be {@link PendingIntent#FLAG_ONE_SHOT},
     *              {@link PendingIntent#FLAG_NO_CREATE}, {@link PendingIntent#FLAG_CANCEL_CURRENT},
     *              {@link PendingIntent#FLAG_UPDATE_CURRENT}, or any of the flags supported by
     *              {@link Intent#fillIn(Intent, int)} to control which unspecified parts of the
     *              intent that can be supplied when the actual send happens.
     *
     * @return The obtained PendingIntent
     */
    CARAPI GetPendingIntent(
        /* [in] */ Int32 requestCode,
        /* [in] */ Int32 flags,
        /* [out] */ IPendingIntent **pendingIntent);

    /**
     * Obtain a {@link PendingIntent} for launching the task constructed by this builder so far.
     *
     * @param requestCode request code for the sender
     * @param flags May be {@link PendingIntent#FLAG_ONE_SHOT},
     *              {@link PendingIntent#FLAG_NO_CREATE}, {@link PendingIntent#FLAG_CANCEL_CURRENT},
     *              {@link PendingIntent#FLAG_UPDATE_CURRENT}, or any of the flags supported by
     *              {@link Intent#fillIn(Intent, int)} to control which unspecified parts of the
     *              intent that can be supplied when the actual send happens.
     * @param options Additional options for how the Activity should be started.
     * See {@link android.content.Context#startActivity(Intent, Bundle)
     * Context.startActivity(Intent, Bundle)} for more details.
     *
     * @return The obtained PendingIntent
     */
    CARAPI GetPendingIntent(
        /* [in] */ Int32 requestCode,
        /* [in] */ Int32 flags,
        /* [in] */ IBundle * options,
        /* [out] */ IPendingIntent **pendingIntent);

    /**
     * @hide
     */
    CARAPI GetPendingIntent(
        /* [in] */ Int32 requestCode,
        /* [in] */ Int32 flags,
        /* [in] */ IBundle * options,
        /* [in] */ IUserHandle * user,
        /* [out] */ IPendingIntent **pendingIntent);

    /**
     * Return an array containing the intents added to this builder. The intent at the
     * root of the task stack will appear as the first item in the array and the
     * intent at the top of the stack will appear as the last item.
     *
     * @return An array containing the intents added to this builder.
     */
    CARAPI GetIntents(
        /* [out, callee] */ ArrayOf<IIntent *> **intents);

public:
    static const String TAG;

    List<AutoPtr<IIntent> > mIntents;
    AutoPtr<IContext> mSourceContext;
};

} // namespace App
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_APP_CTASKSTACKBUILDER_H__
