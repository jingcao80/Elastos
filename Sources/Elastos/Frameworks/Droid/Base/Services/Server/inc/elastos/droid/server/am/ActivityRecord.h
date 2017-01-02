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

#ifndef __ELASTOS_DROID_SERVER_AM_ACTIVITYRECORD_H__
#define __ELASTOS_DROID_SERVER_AM_ACTIVITYRECORD_H__

#include "elastos/droid/ext/frameworkhash.h"
#include "elastos/droid/server/am/ActivityResult.h"
#include "elastos/droid/server/am/ActivityState.h"
#include "elastos/droid/server/am/ConnectionRecord.h"
#include "elastos/droid/server/am/CPendingIntentRecord.h"
#include "elastos/droid/server/am/TaskRecord.h"
#include "elastos/droid/server/am/UriPermissionOwner.h"
#include <elastos/core/StringBuilder.h>
#include <elastos/utility/etl/List.h>
#include <elastos/utility/etl/HashSet.h>

using Elastos::Droid::App::IActivityOptions;
using Elastos::Droid::Graphics::IBitmap;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::IComponentName;
using Elastos::Droid::Content::Res::IConfiguration;
using Elastos::Droid::Content::Res::ICompatibilityInfo;
using Elastos::Droid::Content::Pm::IActivityInfo;
using Elastos::Droid::Content::Pm::IApplicationInfo;
using Elastos::Droid::Os::IBinder;
using Elastos::Droid::Os::IPersistableBundle;
using Elastos::Droid::View::IApplicationToken;
using Elastos::Core::ICharSequence;
using Elastos::IO::IPrintWriter;
using Elastos::Utility::Etl::List;
using Elastos::Utility::Etl::HashSet;


namespace Elastos {
namespace Droid {
namespace Server {
namespace Am {

class ActivityStackSupervisor;
class CActivityManagerService;
class ProcessRecord;

class ActivityRecord
    : public Object
    , public IActivityRecord
{
public:
    CAR_INTERFACE_DECL()

    ActivityRecord();

    ~ActivityRecord();

    CARAPI constructor(
        /* [in] */ CActivityManagerService* service,
        /* [in] */ ProcessRecord* caller,
        /* [in] */ Int32 launchedFromUid,
        /* [in] */ const String& launchedFromPackage,
        /* [in] */ IIntent* intent,
        /* [in] */ const String& resolvedType,
        /* [in] */ IActivityInfo* aInfo,
        /* [in] */ IConfiguration* configuration,
        /* [in] */ ActivityRecord* resultTo,
        /* [in] */ const String& resultWho,
        /* [in] */ Int32 reqCode,
        /* [in] */ Boolean componentSpecified,
        /* [in] */ ActivityStackSupervisor* supervisor,
        /* [in] */ IIActivityContainer* container,
        /* [in] */ IBundle* options);


    CARAPI_(void) Dump(
        /* [in] */ IPrintWriter* pw,
        /* [in] */ const String& prefix);

    static CARAPI_(AutoPtr<ActivityRecord>) ForToken(
        /* [in] */ IBinder* token);

    CARAPI_(Boolean) IsNotResolverActivity();

    CARAPI_(void) SetTask(
        /* [in] */ TaskRecord* newTask,
        /* [in] */ TaskRecord* taskToAffiliateWith);

    CARAPI_(void) SetTaskToAffiliateWith(
        /* [in] */ TaskRecord* taskToAffiliateWith);

    CARAPI_(Boolean) ChangeWindowTranslucency(
        /* [in] */ Boolean toOpaque);

    CARAPI_(void) PutInHistory();

    CARAPI_(void) TakeFromHistory();

    CARAPI_(Boolean) IsInHistory();

    CARAPI_(Boolean) IsHomeActivity();

    CARAPI_(Boolean) IsRecentsActivity();

    CARAPI_(Boolean) IsApplicationActivity();

    CARAPI_(Boolean) IsPersistable();

    CARAPI_(void) MakeFinishing();

    CARAPI_(AutoPtr<UriPermissionOwner>) GetUriPermissionsLocked();

    CARAPI_(void) AddResultLocked(
        /* [in] */ ActivityRecord* from,
        /* [in] */ const String& resultWho,
        /* [in] */ Int32 requestCode,
        /* [in] */ Int32 resultCode,
        /* [in] */ IIntent* resultData);

    CARAPI_(void) RemoveResultsLocked(
        /* [in] */ ActivityRecord* from,
        /* [in] */ const String& resultWho,
        /* [in] */ Int32 requestCode);

    CARAPI_(void) AddNewIntentLocked(
        /* [in] */ IIntent* intent);

    /**
     * Deliver a new Intent to an existing activity, so that its onNewIntent()
     * method will be called at the proper time.
     */
    CARAPI DeliverNewIntentLocked(
        /* [in] */ Int32 callingUid,
        /* [in] */ IIntent* intent);

    CARAPI_(void) UpdateOptionsLocked(
        /* [in] */ IBundle* options);

    CARAPI_(void) UpdateOptionsLocked(
        /* [in] */ IActivityOptions* options);

    CARAPI_(void) ApplyOptionsLocked();

    CARAPI_(AutoPtr<IActivityOptions>) GetOptionsForTargetActivityLocked();

    CARAPI_(void) ClearOptionsLocked();

    CARAPI_(AutoPtr<IActivityOptions>) TakeOptionsLocked();

    CARAPI_(void) RemoveUriPermissionsLocked();

    CARAPI_(void) PauseKeyDispatchingLocked();

    CARAPI_(void) ResumeKeyDispatchingLocked();

    CARAPI_(void) UpdateThumbnail(
        /* [in] */ IBitmap* newThumbnail,
        /* [in] */ ICharSequence* description);

    CARAPI_(void) StartLaunchTickingLocked();

    CARAPI_(Boolean) ContinueLaunchTickingLocked();

    CARAPI FinishLaunchTickingLocked();

    // IApplicationToken

    CARAPI_(Boolean) MayFreezeScreenLocked(
        /* [in] */ ProcessRecord* app);

    CARAPI StartFreezingScreenLocked(
        /* [in] */ ProcessRecord* app,
        /* [in] */ Int32 configChanges);

    CARAPI StopFreezingScreenLocked(
        /* [in] */ Boolean force);

    CARAPI ReportFullyDrawnLocked();

    CARAPI WindowsDrawn();

    CARAPI WindowsVisible();

    CARAPI WindowsGone();

    CARAPI KeyDispatchingTimedOut(
        /* [in] */ const String& reason,
        /* [out] */ Boolean* result);

    /** Returns the key dispatching timeout for this application token. */
    CARAPI GetKeyDispatchingTimeout(
        /* [out] */ Int64* timeout);

    /**
     * This method will return true if the activity is either visible, is becoming visible, is
     * currently pausing, or is resumed.
     */
    CARAPI_(Boolean) IsInterestingToUserLocked();

    CARAPI SetSleeping(
        /* [in] */ Boolean sleeping);

    CARAPI ToString(
        /* [out] */ String* str);

    CARAPI_(String) ToString();

    static CARAPI_(void) ActivityResumedLocked(
        /* [in] */ IBinder* token);

    static CARAPI_(Int32) GetTaskForActivityLocked(
        /* [in] */ IBinder* token,
        /* [in] */ Boolean onlyRoot);

    static CARAPI_(AutoPtr<ActivityRecord>) IsInStackLocked(
        /* [in] */ IBinder* token);

    static CARAPI_(AutoPtr<ActivityStack>) GetStackLocked(
        /* [in] */ IBinder* token);

    CARAPI_(Boolean) IsDestroyable();

    CARAPI_(void) SetTaskDescription(
        /* [in] */ IActivityManagerTaskDescription* taskDescription);

    CARAPI SaveToXml(
        /* [in] */ IXmlSerializer* out);

    static CARAPI RestoreFromXml(
        /* [in] */ IXmlPullParser* in,
        /* [in] */ Int32 taskId,
        /* [in] */ ActivityStackSupervisor* stackSupervisor,
        /* [out] */ ActivityRecord** ar);

private:
    CARAPI_(void) ReportLaunchTimeLocked(
        /* [in] */ Int64 curTime);

    CARAPI_(AutoPtr<ActivityRecord>) GetWaitingHistoryRecordLocked();

    static String CreateImageFilename(
        /* [in] */ Int64 createTime,
        /* [in] */ Int32 taskId);

    static String ActivityTypeToString(
        /* [in] */ Int32 type);

public:
    static const String TAG;
    static const String TAG_TIMELINE;
    static const Boolean DEBUG_SAVED_STATE;
    static const String RECENTS_PACKAGE_NAME;
    static const String ACTIVITY_ICON_SUFFIX;
    static const Int32 APPLICATION_ACTIVITY_TYPE = 0;
    static const Int32 HOME_ACTIVITY_TYPE = 1;
    static const Int32 RECENTS_ACTIVITY_TYPE = 2;

private:
    static const String TAG_ACTIVITY;
    static const String ATTR_ID;
    static const String TAG_INTENT;
    static const String ATTR_USERID;
    static const String TAG_PERSISTABLEBUNDLE;
    static const String ATTR_LAUNCHEDFROMUID;
    static const String ATTR_LAUNCHEDFROMPACKAGE;
    static const String ATTR_RESOLVEDTYPE;
    static const String ATTR_COMPONENTSPECIFIED;

public:
    CActivityManagerService* mService; // owner - weak-ref
    AutoPtr<IApplicationToken> mAppToken; // window manager token
    AutoPtr<IActivityInfo> mInfo; // all about me
    AutoPtr<IApplicationInfo> mAppInfo; // information about activity's app
    Int32 mLaunchedFromUid; // always the uid who started the activity.
    String mLaunchedFromPackage; // always the package who started the activity.
    Int32 mUserId;          // Which user is this running for?
    AutoPtr<IIntent> mIntent;    // the original intent that generated us
    AutoPtr<IComponentName> mRealActivity;  // the intent component, or target of an alias.
    String mShortComponentName; // the short component name of the intent
    String mResolvedType; // as per original caller;
    String mPackageName; // the package implementing intent's component
    String mProcessName; // process where this component wants to run
    String mTaskAffinity; // as per ActivityInfo.taskAffinity
    Boolean mStateNotNeeded; // As per ActivityInfo.flags
    Boolean mFullscreen; // covers the full screen?
    Boolean mNoDisplay;  // activity is not displayed?
    Boolean mComponentSpecified;  // did caller specifiy an explicit component?

    Int32 mActivityType;

    AutoPtr<ICharSequence> mNonLocalizedLabel;  // the label information from the package mgr.
    Int32 mLabelRes;           // the label information from the package mgr.
    Int32 mIcon;               // resource identifier of activity's icon.
    Int32 mLogo;               // resource identifier of activity's logo.
    Int32 mTheme;              // resource identifier of activity's theme.
    Int32 mRealTheme;          // actual theme resource we will use, never 0.
    Int32 mWindowFlags;        // custom window flags for preview window.
    AutoPtr<TaskRecord> mTask;        // the task this is in.
    Int64 mCreateTime;
    Int64 mDisplayStartTime;  // when we started launching this activity
    Int64 mFullyDrawnStartTime; // when we started launching this activity
    Int64 mStartTime;         // last time this activity was started
    Int64 mLastVisibleTime;   // last time this activity became visible
    Int64 mCpuTimeAtResume;   // the cpu time of host process at the time of resuming activity
    Int64 mPauseTime;         // last time we started pausing the activity
    Int64 mLaunchTickTime;    // base time for launch tick messages
    AutoPtr<IConfiguration> mConfiguration; // configuration activity was last running in
    AutoPtr<ICompatibilityInfo> mCompat;// last used compatibility mode
    AutoPtr<ActivityRecord> mResultTo; // who started this entry, so will get our reply
    String mResultWho; // additional identifier for use by resultTo.
    Int32 mRequestCode;  // code given by requester (resultTo)
    AutoPtr< List< AutoPtr<ActivityResult> > > mResults;      // pending ActivityResult objs we have received
    AutoPtr< HashSet<AutoPtr<IWeakReference> > > mPendingResults; // all pending intents for this act,PendingIntentRecord
    AutoPtr< List< AutoPtr<IIntent> > > mNewIntents;   // any pending new intents for single-top mode
    AutoPtr<IActivityOptions> mPendingOptions; // most recently given options
    AutoPtr<IActivityOptions> mReturningOptions; // options that are coming back via convertToTranslucent
    AutoPtr< HashSet<AutoPtr<ConnectionRecord> > > mConnections; // All ConnectionRecord we hold
    AutoPtr<UriPermissionOwner> mUriPermissions; // current special URI access perms.
    AutoPtr<ProcessRecord> mApp;      // if non-null, hosting application
    ActivityState mState;    // current state we are in
    AutoPtr<IBundle> mIcicle;         // last saved activity state
    AutoPtr<IPersistableBundle> mPersistentState; // last persistently saved activity state
    Boolean mFrontOfTask;    // is this the root activity of its task?
    Boolean mLaunchFailed;   // set if a launched failed, to abort on 2nd try
    Boolean mHaveState;      // have we gotten the last activity state?
    Boolean mStopped;        // is activity pause finished?
    Boolean mDelayedResume;  // not yet resumed because of stopped app switches?
    Boolean mFinishing;      // activity in pending finish list?
    Boolean mConfigDestroy;  // need to destroy due to config change?
    Int32 mConfigChangeFlags;  // which config values have changed
    Boolean mKeysPaused;     // has key dispatching been paused for it?
    Int32 mLaunchMode;         // the launch mode activity attribute.
    Boolean mVisible;        // does this activity's window need to be shown?
    Boolean mSleeping;       // have we told the activity to sleep?
    Boolean mWaitingVisible; // true if waiting for a new act to become vis
    Boolean mNowVisible;     // is this activity's window visible?
    Boolean mIdle;           // has the activity gone idle?
    Boolean mHasBeenLaunched;// has this activity ever been launched?
    Boolean mFrozenBeforeDestroy;// has been frozen but not yet destroyed.
    Boolean mImmersive;      // immersive mode (don't interrupt if possible)
    Boolean mForceNewConfig; // force re-create with new config next time
    Int32 mLaunchCount;        // count of launches since last state
    Int64 mLastLaunchTime;    // time of last lauch of this activity
    List<AutoPtr<IIActivityContainer> > mChildContainers;

    String mStringName;      // for caching of toString().

    ActivityStackSupervisor* mStackSupervisor;
    Boolean mStartingWindowShown;
    AutoPtr<IIActivityContainer> mInitialActivityContainer;

    AutoPtr<IActivityManagerTaskDescription> mTaskDescription; // the recents information for this activity
    Boolean mLaunchTaskBehind; // this activity is actively being launched with
        // ActivityOptions.setLaunchTaskBehind, will be cleared once launch is completed.

private:
    Boolean mInHistory;  // are we in the history stack?
};

} // namespace Am
} // namespace Server
} // namespace Droid
} // namespace Elastos

DEFINE_OBJECT_HASH_FUNC_FOR(Elastos::Droid::Server::Am::ActivityRecord)
DEFINE_CONVERSION_FOR(Elastos::Droid::Server::Am::ActivityRecord, IInterface)

#endif // __ELASTOS_DROID_SERVER_AM_ACTIVITYRECORD_H__
