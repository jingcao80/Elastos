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

#include "_Elastos.Droid.App.h"
#include "elastos/droid/server/am/ActivityRecord.h"
#include "elastos/droid/server/am/CActivityManagerService.h"
#include "elastos/droid/server/am/CActivityRecordToken.h"
#include "elastos/droid/server/am/ActivityStack.h"
#include "elastos/droid/server/am/ActivityStackSupervisor.h"
#include "elastos/droid/server/am/ProcessRecord.h"
#include "elastos/droid/server/am/TaskPersister.h"
#include "elastos/droid/server/AttributeCache.h"
#include "elastos/droid/R.h"
#include <elastos/droid/internal/utility/XmlUtils.h>
#include <elastos/droid/os/Build.h>
#include <elastos/droid/os/Process.h>
#include <elastos/droid/os/SystemClock.h>
#include <elastos/droid/utility/TimeUtils.h>
#include <elastos/core/AutoLock.h>
#include <elastos/core/StringBuilder.h>
#include <elastos/core/StringUtils.h>
#include <elastos/utility/logging/Logger.h>
#include <elastos/utility/logging/Slogger.h>
#include <utils/CallStack.h>

using Elastos::Droid::App::CActivityManagerTaskDescription;
using Elastos::Droid::App::CActivityOptions;
using Elastos::Droid::Content::CComponentName;
using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Content::CIntentHelper;
using Elastos::Droid::Content::EIID_IIIntentSender;
using Elastos::Droid::Content::IIntentHelper;
using Elastos::Droid::Content::Pm::IComponentInfo;
using Elastos::Droid::Content::Pm::IPackageItemInfo;
using Elastos::Droid::Graphics::CRect;
using Elastos::Droid::Graphics::IRect;
using Elastos::Droid::Internal::Utility::XmlUtils;
using Elastos::Droid::Os::Build;
using Elastos::Droid::Os::CBundle;
using Elastos::Droid::Os::CPersistableBundleHelper;
using Elastos::Droid::Os::CUserHandleHelper;
using Elastos::Droid::Os::IBinder;
using Elastos::Droid::Os::IMessage;
using Elastos::Droid::Os::IPersistableBundleHelper;
using Elastos::Droid::Os::IProcess;
using Elastos::Droid::Os::IRemoteCallback;
using Elastos::Droid::Os::IUserHandleHelper;
using Elastos::Droid::Os::Process;
using Elastos::Droid::Os::SystemClock;
using Elastos::Droid::Utility::TimeUtils;
using Elastos::Droid::View::IWindowManagerLayoutParams;
using Elastos::Core::CSystem;
using Elastos::Core::ISystem;
using Elastos::Core::StringBuilder;
using Elastos::Core::StringUtils;
using Elastos::Utility::CArrayList;
using Elastos::Utility::IList;
using Elastos::Utility::Logging::Logger;
using Elastos::Utility::Logging::Slogger;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Am {

const String ActivityRecord::TAG("ActivityManager");
const String ActivityRecord::TAG_TIMELINE("Timeline");
const Boolean ActivityRecord::DEBUG_SAVED_STATE = FALSE;// ActivityStackSupervisor::DEBUG_SAVED_STATE;
const String ActivityRecord::RECENTS_PACKAGE_NAME("Elastos.Droid.SystemUI.Recent");
const String ActivityRecord::ACTIVITY_ICON_SUFFIX("_activity_icon_");
const Int32 ActivityRecord::APPLICATION_ACTIVITY_TYPE;
const Int32 ActivityRecord::HOME_ACTIVITY_TYPE;
const Int32 ActivityRecord::RECENTS_ACTIVITY_TYPE;
const String ActivityRecord::TAG_ACTIVITY("activity");
const String ActivityRecord::ATTR_ID("id");
const String ActivityRecord::TAG_INTENT("intent");
const String ActivityRecord::ATTR_USERID("user_id");
const String ActivityRecord::TAG_PERSISTABLEBUNDLE("persistable_bundle");
const String ActivityRecord::ATTR_LAUNCHEDFROMUID("launched_from_uid");
const String ActivityRecord::ATTR_LAUNCHEDFROMPACKAGE("launched_from_package");
const String ActivityRecord::ATTR_RESOLVEDTYPE("resolved_type");
const String ActivityRecord::ATTR_COMPONENTSPECIFIED("component_specified");

CAR_INTERFACE_IMPL(ActivityRecord, Object, IActivityRecord)

ActivityRecord::ActivityRecord()
    : mUserId(0)
    , mStateNotNeeded(FALSE)
    , mFullscreen(FALSE)
    , mNoDisplay(FALSE)
    , mActivityType(0)
    , mLabelRes(0)
    , mIcon(0)
    , mTheme(0)
    , mRealTheme(0)
    , mWindowFlags(0)
    , mCreateTime(0)
    , mDisplayStartTime(0)
    , mFullyDrawnStartTime(0)
    , mStartTime(0)
    , mLastVisibleTime(0)
    , mCpuTimeAtResume(0)
    , mPauseTime(0)
    , mLaunchTickTime(0)
    , mState(ActivityState_INITIALIZING)
    , mFrontOfTask(FALSE)
    , mLaunchFailed(FALSE)
    , mHaveState(FALSE)
    , mStopped(FALSE)
    , mDelayedResume(FALSE)
    , mFinishing(FALSE)
    , mConfigDestroy(FALSE)
    , mConfigChangeFlags(0)
    , mKeysPaused(FALSE)
    , mLaunchMode(0)
    , mVisible(TRUE)
    , mSleeping(FALSE)
    , mWaitingVisible(FALSE)
    , mNowVisible(FALSE)
    , mIdle(FALSE)
    , mHasBeenLaunched(FALSE)
    , mFrozenBeforeDestroy(FALSE)
    , mImmersive(FALSE)
    , mForceNewConfig(FALSE)
    , mLaunchCount(0)
    , mLastLaunchTime(0)
    , mStartingWindowShown(FALSE)
    , mLaunchTaskBehind(FALSE)
    , mInHistory(FALSE)
{
}

ActivityRecord::~ActivityRecord()
{
    Slogger::D("ActivityRecord", " =========== ~ActivityRecord() %s", ToString().string());
    // android::CallStack stack;
    // stack.update();
    // Logger::I(TAG, "-------------------------------------------------------");
    // Logger::I(TAG, "callstack:\n%s", stack.toString("").string());
    // Logger::I(TAG, "-------------------------------------------------------");
}

ECode ActivityRecord::constructor(
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
    /* [in] */ IBundle* options)
{
    mService = service;
    mInfo = aInfo;
    mLaunchedFromUid = launchedFromUid;
    mLaunchedFromPackage = launchedFromPackage;
    mIntent = intent;
    mResolvedType = resolvedType;
    mComponentSpecified = componentSpecified;
    mConfiguration = configuration;
    mResultTo = resultTo;
    mResultWho = resultWho;
    mRequestCode = reqCode;
    mStackSupervisor = supervisor;
    mInitialActivityContainer = container;

    ASSERT_SUCCEEDED(CActivityRecordToken::New(this, (IApplicationToken**)&mAppToken));

    AutoPtr<IComponentName> component;
    intent->GetComponent((IComponentName**)&component);
    component->FlattenToShortString(&mShortComponentName);

    AutoPtr<ISystem> system;
    CSystem::AcquireSingleton((ISystem**)&system);
    system->GetCurrentTimeMillis(&mCreateTime);

    if (options != NULL) {
        CActivityOptions::New(options, (IActivityOptions**)&mPendingOptions);
        mPendingOptions->GetLaunchTaskBehind(&mLaunchTaskBehind);
    }

    // This starts out TRUE, since the initial state of an activity
    // is that we have everything, and we shouldn't never consider it
    // lacking in state to be removed if it dies.
    mHaveState = TRUE;

    if (mInfo != NULL) {
        IPackageItemInfo* piInfo = IPackageItemInfo::Probe(mInfo);
        IComponentInfo* ciInfo = IComponentInfo::Probe(mInfo);

        AutoPtr<IApplicationInfo> appInfo;
        ciInfo->GetApplicationInfo((IApplicationInfo**)&appInfo);
        IPackageItemInfo* piAppInfo = IPackageItemInfo::Probe(appInfo);
        Int32 uid;
        appInfo->GetUid(&uid);

        AutoPtr<IUserHandleHelper> helper;
        CUserHandleHelper::AcquireSingleton((IUserHandleHelper**)&helper);
        helper->GetUserId(uid, &mUserId);

        String target;
        mInfo->GetTargetActivity(&target);
        Int32 mode;
        mInfo->GetLaunchMode(&mode);
        if (target.IsNull() || mode == IActivityInfo::LAUNCH_MULTIPLE
            || mode == IActivityInfo::LAUNCH_SINGLE_TOP) {
            mIntent->GetComponent((IComponentName**)&mRealActivity);
        }
        else {
            String pname;
            piInfo->GetPackageName(&pname);
            CComponentName::New(pname, target, (IComponentName**)&mRealActivity);
        }
        mInfo->GetTaskAffinity(&mTaskAffinity);
        Int32 flags;
        mInfo->GetFlags(&flags);
        mStateNotNeeded = (flags & IActivityInfo::FLAG_STATE_NOT_NEEDED) != 0;
        mAppInfo = appInfo;
        piInfo->GetNonLocalizedLabel((ICharSequence**)&mNonLocalizedLabel);
        piInfo->GetLabelRes(&mLabelRes);
        if (mNonLocalizedLabel == NULL && mLabelRes == 0) {
            piAppInfo->GetNonLocalizedLabel((ICharSequence**)&mNonLocalizedLabel);
            piAppInfo->GetLabelRes(&mLabelRes);
        }
        ciInfo->GetIconResource(&mIcon);
        ciInfo->GetLogoResource(&mLogo);
        mInfo->GetThemeResource(&mTheme);
        mRealTheme = mTheme;
        if (mRealTheme == 0) {
            Int32 version;
            appInfo->GetTargetSdkVersion(&version);
            mRealTheme = version
                < Build::VERSION_CODES::HONEYCOMB
                ? R::style::Theme : R::style::Theme_Holo;
        }
        if ((flags & IActivityInfo::FLAG_HARDWARE_ACCELERATED) != 0) {
            mWindowFlags |= IWindowManagerLayoutParams::FLAG_HARDWARE_ACCELERATED;
        }
        Int32 appUid, aUid;
        if ((flags & IActivityInfo::FLAG_MULTIPROCESS) != 0
                && caller != NULL
                && (appInfo->GetUid(&appUid), caller->mInfo->GetUid(&aUid),
                    appUid == IProcess::SYSTEM_UID || appUid == aUid)) {
            mProcessName = caller->mProcessName;
        }
        else {
            ciInfo->GetProcessName(&mProcessName);
        }

        if (intent != NULL && (flags & IActivityInfo::FLAG_EXCLUDE_FROM_RECENTS) != 0) {
            intent->AddFlags(IIntent::FLAG_ACTIVITY_EXCLUDE_FROM_RECENTS);
        }

        piAppInfo->GetPackageName(&mPackageName);
        mInfo->GetLaunchMode(&mLaunchMode);

        AutoPtr<ArrayOf<Int32> > attrIds = TO_ATTRS_ARRAYOF(R::styleable::Window);

        AutoPtr<Entry> ent = AttributeCache::GetInstance()->Get(
            mPackageName, mRealTheme, attrIds, mUserId);
        if (ent != NULL) {
            Boolean isFloating, isTranslucent;
            ent->mArray->GetBoolean(
               R::styleable::Window_windowIsFloating, FALSE, &isFloating);
            ent->mArray->GetBoolean(
               R::styleable::Window_windowIsTranslucent, FALSE, &isTranslucent);
            mFullscreen = !isFloating && !isTranslucent;

            ent->mArray->GetBoolean(
               R::styleable::Window_windowNoDisplay, FALSE, &mNoDisplay);
        }

        String action;
        Boolean hasHomeCategory;
        AutoPtr< ArrayOf<String> > categories;
        AutoPtr<IUri> data;
        String type;
        Int32 iflags;
        String className;
        if ((!mComponentSpecified || mLaunchedFromUid == Process::MyUid()
            || mLaunchedFromUid == 0) &&
            (mIntent->GetAction(&action), IIntent::ACTION_MAIN.Equals(action)) &&
            (mIntent->HasCategory(IIntent::CATEGORY_HOME, &hasHomeCategory), hasHomeCategory) &&
            (mIntent->GetCategories((ArrayOf<String>**)&categories), categories != NULL && categories->GetLength() == 1) &&
            (mIntent->GetData((IUri**)&data), data == NULL) &&
            (mIntent->GetType(&type), type.IsNull()) &&
            (mIntent->GetFlags(&iflags), iflags & IIntent::FLAG_ACTIVITY_NEW_TASK) != 0 &&
            IsNotResolverActivity()) {
            // This sure looks like a home activity!
            mActivityType = HOME_ACTIVITY_TYPE;
        }
        else if (mRealActivity->GetClassName(&className), className.Contains(RECENTS_PACKAGE_NAME)) {
            mActivityType = RECENTS_ACTIVITY_TYPE;
        }
        else {
            mActivityType = APPLICATION_ACTIVITY_TYPE;
        }

        mImmersive = (flags & IActivityInfo::FLAG_IMMERSIVE) != 0;
    }
    else {
        mStateNotNeeded = FALSE;
        mFullscreen = TRUE;
        mNoDisplay = FALSE;
        mActivityType = APPLICATION_ACTIVITY_TYPE;
        mImmersive = FALSE;
    }
    return NOERROR;
}

void ActivityRecord::Dump(
    /* [in] */ IPrintWriter* pw,
    /* [in] */ const String& prefix)
{
    Int64 now = SystemClock::GetUptimeMillis();
    pw->Print(prefix);
    pw->Print(String("packageName="));
    pw->Print(mPackageName);
    pw->Print(String(" processName="));
    pw->Println(mProcessName);
    pw->Print(prefix);
    pw->Print(String("launchedFromUid="));
    pw->Print(mLaunchedFromUid);
    pw->Print(String(" launchedFromPackage="));
    pw->Print(mLaunchedFromPackage);
    pw->Print(String(" userId="));
    pw->Println(mUserId);
    pw->Print(prefix);
    pw->Print(String("app="));
    pw->Println(mApp->ToString());
    pw->Print(prefix);
    String str;
    mIntent->ToInsecureStringWithClip(&str);
    pw->Println(str);
    pw->Print(prefix);
    pw->Print(String("frontOfTask="));
    pw->Print(mFrontOfTask);
    pw->Print(String(" task="));
    pw->Println(mTask->ToString());
    pw->Print(prefix);
    pw->Print(String("taskAffinity="));
    pw->Println(mTaskAffinity);
    pw->Print(prefix);
    pw->Print(String("realActivity="));
    mRealActivity->FlattenToShortString(&str);
    pw->Println(str);
    if (mAppInfo != NULL) {
        pw->Print(prefix);
        pw->Print(String("baseDir="));
        String sourceDir, publicSourceDir;
        mAppInfo->GetSourceDir(&sourceDir);
        mAppInfo->GetPublicSourceDir(&publicSourceDir);
        pw->Println(sourceDir);
        if (!sourceDir.Equals(publicSourceDir)) {
            pw->Print(prefix);
            pw->Print(String("resDir="));
            pw->Println(publicSourceDir);
        }
        pw->Print(prefix);
        pw->Print(String("dataDir="));
        String dataDir;
        mAppInfo->GetDataDir(&dataDir);
        pw->Println(dataDir);
    }
    pw->Print(prefix);
    pw->Print(String("stateNotNeeded="));
    pw->Print(mStateNotNeeded);
    pw->Print(String(" componentSpecified="));
    pw->Print(mComponentSpecified);
    pw->Print(String(" mActivityType="));
    pw->Println(mActivityType);
    pw->Print(prefix);
    pw->Print(String("compat="));
    pw->Print(mCompat);
    pw->Print(String(" labelRes=0x"));
    pw->Print(StringUtils::ToString(mLabelRes, 16));
    pw->Print(String(" icon=0x"));
    pw->Print(StringUtils::ToString(mIcon, 16));
    pw->Print(String(" theme=0x"));
    pw->Println(StringUtils::ToString(mTheme, 16));
    pw->Print(prefix);
    pw->Println(mConfiguration);
    if (mResultTo != NULL || mResultWho != NULL) {
        pw->Print(prefix);
        pw->Print(String("resultTo="));
        pw->Print(mResultTo->ToString());
        pw->Print(String(" resultWho="));
        pw->Print(mResultWho);
        pw->Print(String(" resultCode="));
        pw->Println(mRequestCode);
    }
    if (mTaskDescription != NULL) {
        String iconFilename;
        mTaskDescription->GetIconFilename(&iconFilename);
        String label;
        Int32 primaryColor;
        if (iconFilename != NULL || (mTaskDescription->GetLabel(&label), label != NULL)
            || (mTaskDescription->GetPrimaryColor(&primaryColor), primaryColor != 0)) {
            pw->Print(prefix);
            pw->Print(String("taskDescription:"));
            pw->Print(String(" iconFilename="));
            pw->Print(iconFilename);
            pw->Print(String(" label=\""));
            pw->Print(label);
            pw->Print(String("\""));
            pw->Print(String(" color="));
            mTaskDescription->GetPrimaryColor(&primaryColor);
            pw->Println(StringUtils::ToString(primaryColor, 16));
        }
        AutoPtr<IBitmap> icon;
        if (iconFilename == NULL && (mTaskDescription->GetIcon((IBitmap**)&icon), icon != NULL)) {
            pw->Print(prefix);
            pw->Println(String("taskDescription contains Bitmap"));
        }
    }
    if (mResults != NULL) {
        pw->Print(prefix);
        pw->Print(String("results="));
        // pw->Println(results);
    }
    if (mPendingResults != NULL && mPendingResults->GetSize() > 0) {
        pw->Print(prefix);
        pw->Println(String("Pending Results:"));
        HashSet<AutoPtr<IWeakReference> >::Iterator iter = mPendingResults->Begin();
        for (; iter != mPendingResults->End(); ++iter) {
            AutoPtr<IWeakReference> wpir = *iter;
            AutoPtr<IIIntentSender> pir;
            if (wpir != NULL) {
                wpir->Resolve(EIID_IIIntentSender, (IInterface**)&pir);
            }
            pw->Print(prefix);
            pw->Print(String("  - "));
            if (pir == NULL) {
                pw->Println(String("NULL"));
            }
            else {
                pw->Println(pir);
                ((CPendingIntentRecord*)pir.Get())->Dump(pw, prefix + "    ");
            }
        }
    }
    if (mNewIntents != NULL && mNewIntents->GetSize() > 0) {
        pw->Print(prefix);
        pw->Println(String("Pending New Intents:"));
        List< AutoPtr<IIntent> >::Iterator iter = mNewIntents->Begin();
        for (; iter != mNewIntents->End(); ++iter) {
            AutoPtr<IIntent> intent = *iter;
            pw->Print(prefix);
            pw->Print(String("  - "));
            if (intent == NULL) {
                pw->Println(String("NULL"));
            }
            else {
                intent->ToShortString(FALSE, TRUE, FALSE, TRUE, &str);
                pw->Println(str);
            }
        }
    }
    if (mPendingOptions != NULL) {
        pw->Print(prefix);
        pw->Print(String("pendingOptions="));
        pw->Println(mPendingOptions);
    }
    if (mUriPermissions != NULL) {
        mUriPermissions->Dump(pw, prefix);
    }
    pw->Print(prefix);
    pw->Print(String("launchFailed="));
    pw->Print(mLaunchFailed);
    pw->Print(String(" launchCount="));
    pw->Print(mLaunchCount);
    pw->Print(String(" lastLaunchTime="));
    if (mLastLaunchTime == 0)
        pw->Print(String("0"));
    else
        TimeUtils::FormatDuration(mLastLaunchTime, now, pw);
    pw->Println();
    pw->Print(prefix);
    pw->Print(String("haveState="));
    pw->Print(mHaveState);
    pw->Print(String(" icicle="));
    pw->Println(mIcicle);
    pw->Print(prefix);
    pw->Print(String("state="));
    pw->Print((Int32)mState);
    pw->Print(String(" stopped="));
    pw->Print(mStopped);
    pw->Print(String(" delayedResume="));
    pw->Print(mDelayedResume);
    pw->Print(String(" finishing="));
    pw->Println(mFinishing);
    pw->Print(prefix);
    pw->Print(String("keysPaused="));
    pw->Print(mKeysPaused);
    pw->Print(String(" inHistory="));
    pw->Print(mInHistory);
    pw->Print(String(" visible="));
    pw->Print(mVisible);
    pw->Print(String(" sleeping="));
    pw->Print(mSleeping);
    pw->Print(String(" idle="));
    pw->Println(mIdle);
    pw->Print(prefix);
    pw->Print(String("fullscreen="));
    pw->Print(mFullscreen);
    pw->Print(String(" noDisplay="));
    pw->Print(mNoDisplay);
    pw->Print(String(" immersive="));
    pw->Print(mImmersive);
    pw->Print(String(" launchMode="));
    pw->Println(mLaunchMode);
    pw->Print(prefix);
    pw->Print(String("frozenBeforeDestroy="));
    pw->Print(mFrozenBeforeDestroy);
    pw->Print(String(" forceNewConfig="));
    pw->Println(mForceNewConfig);
    pw->Print(prefix);
    pw->Print(String("mActivityType="));
    pw->Println(ActivityTypeToString(mActivityType));
    if (mDisplayStartTime != 0 || mStartTime != 0) {
        pw->Print(prefix);
        pw->Print(String("displayStartTime="));
        if (mDisplayStartTime == 0)
            pw->Print(String("0"));
        else
            TimeUtils::FormatDuration(mDisplayStartTime, now, pw);
        pw->Print(String(" startTime="));
        if (mStartTime == 0)
            pw->Print(String("0"));
        else
            TimeUtils::FormatDuration(mStartTime, now, pw);
        pw->Println();
    }
    if (mLastVisibleTime != 0 || mWaitingVisible || mNowVisible) {
        pw->Print(prefix);
        pw->Print(String("waitingVisible="));
        pw->Print(mWaitingVisible);
        pw->Print(String(" nowVisible="));
        pw->Print(mNowVisible);
        pw->Print(String(" lastVisibleTime="));
        if (mLastVisibleTime == 0)
            pw->Print(String("0"));
        else
            TimeUtils::FormatDuration(mLastVisibleTime, now, pw);
        pw->Println();
    }
    if (mConfigDestroy || mConfigChangeFlags != 0) {
        pw->Print(prefix);
        pw->Print(String("configDestroy="));
        pw->Print(mConfigDestroy);
        pw->Print(String(" configChangeFlags="));
        pw->Println(StringUtils::ToString(mConfigChangeFlags, 16));
    }
    if (mConnections != NULL) {
        pw->Print(prefix);
        pw->Print(String("connections="));
        // pw->Println(mConnections);
    }
}

AutoPtr<ActivityRecord> ActivityRecord::ForToken(
    /* [in] */ IBinder* token)
{
    IApplicationToken* at = IApplicationToken::Probe(token);
    if (at != NULL) {
        return ((CActivityRecordToken*)at)->GetActivityRecord();
    }
    else if (token != NULL) {
        Slogger::W(TAG, "Bad activity token: 0x%08x, %s", token, TO_CSTR(token));
    }
    return NULL;
}

Boolean ActivityRecord::IsNotResolverActivity()
{
    String className;
    mRealActivity->GetClassName(&className);
    // TODO: delete log
    Slogger::D(TAG, "IsNotResolverActivity className = %s", className.string());
    return !className.Equals("ResolverActivity");
}

void ActivityRecord::SetTask(
    /* [in] */ TaskRecord* newTask,
    /* [in] */ TaskRecord* taskToAffiliateWith)
{
    if (mTask != NULL && mTask->RemoveActivity(this)) {
        if (mTask.Get() != newTask) {
            mTask->mStack->RemoveTask(mTask);
        }
        else {
            Slogger::D(TAG, "!!! REMOVE THIS LOG !!! setTask: nearly removed stack=%s",
                (newTask == NULL ? "NULL" : newTask->mStack->ToString().string()));
        }
    }
    mTask = newTask;
    SetTaskToAffiliateWith(taskToAffiliateWith);
}

void ActivityRecord::SetTaskToAffiliateWith(
    /* [in] */ TaskRecord* taskToAffiliateWith)
{
    if (taskToAffiliateWith != NULL &&
        mLaunchMode != IActivityInfo::LAUNCH_SINGLE_INSTANCE &&
        mLaunchMode != IActivityInfo::LAUNCH_SINGLE_TASK) {
        mTask->SetTaskToAffiliateWith(taskToAffiliateWith);
    }
}

Boolean ActivityRecord::ChangeWindowTranslucency(
    /* [in] */ Boolean toOpaque)
{
    if (mFullscreen == toOpaque) {
        return FALSE;
    }

    // Keep track of the number of mFullscreen activities in this task.
    mTask->mNumFullscreen += toOpaque ? +1 : -1;

    mFullscreen = toOpaque;
    return TRUE;
}

void ActivityRecord::PutInHistory()
{
    if (!mInHistory) {
        mInHistory = TRUE;
    }
}

void ActivityRecord::TakeFromHistory()
{
    if (mInHistory) {
        mInHistory = FALSE;
        if (mTask != NULL && !mFinishing) {
            mTask = NULL;
        }
        ClearOptionsLocked();
    }
}

Boolean ActivityRecord::IsInHistory()
{
    return mInHistory;
}

Boolean ActivityRecord::IsHomeActivity()
{
    return mActivityType == HOME_ACTIVITY_TYPE;
}

Boolean ActivityRecord::IsRecentsActivity()
{
    return mActivityType == RECENTS_ACTIVITY_TYPE;
}

Boolean ActivityRecord::IsApplicationActivity()
{
    return mActivityType == APPLICATION_ACTIVITY_TYPE;
}

Boolean ActivityRecord::IsPersistable()
{
    Int32 persistableMode, flags;
    mInfo->GetPersistableMode(&persistableMode);
    return (persistableMode == IActivityInfo::PERSIST_ROOT_ONLY ||
        persistableMode == IActivityInfo::PERSIST_ACROSS_REBOOTS) &&
        (mIntent == NULL || (mIntent->GetFlags(&flags),
            flags & IIntent::FLAG_ACTIVITY_EXCLUDE_FROM_RECENTS) == 0);
}

void ActivityRecord::MakeFinishing()
{
    if (!mFinishing) {
        if (this == mTask->mStack->GetVisibleBehindActivity()) {
            // A finishing activity should not remain as visible in the background
            mStackSupervisor->RequestVisibleBehindLocked(this, FALSE);
        }
        mFinishing = TRUE;
        if (mStopped) {
            ClearOptionsLocked();
        }
    }
}

AutoPtr<UriPermissionOwner> ActivityRecord::GetUriPermissionsLocked()
{
    if (mUriPermissions == NULL) {
        mUriPermissions = new UriPermissionOwner(mService, this);
    }
    return mUriPermissions;
}

void ActivityRecord::AddResultLocked(
    /* [in] */ ActivityRecord* from,
    /* [in] */ const String& resultWho,
    /* [in] */ Int32 requestCode,
    /* [in] */ Int32 resultCode,
    /* [in] */ IIntent* resultData)
{
    AutoPtr<ActivityResult> r = new ActivityResult(from, resultWho,
           requestCode, resultCode, resultData);
    if (mResults == NULL) {
        mResults = new List< AutoPtr<ActivityResult> >();
    }
    mResults->PushBack(r);
}

void ActivityRecord::RemoveResultsLocked(
    /* [in] */ ActivityRecord* from,
    /* [in] */ const String& resultWho,
    /* [in] */ Int32 requestCode)
{
    if (mResults != NULL) {
        String who;
        Int32 code = 0;
        List< AutoPtr<ActivityResult> >::ReverseIterator rit = mResults->RBegin();
        while (rit != mResults->REnd()) {
            AutoPtr<ActivityResult> r = *rit;
            r->mResultInfo->GetResultWho(&who);
            r->mResultInfo->GetRequestCode(&code);

            assert(r != NULL);
            if (r->mFrom != from) {
                ++rit;
                continue;
            }
            if (who.IsNull()) {
               if (!resultWho.IsNull()) {
                    ++rit;
                    continue;
                }
            }
            else {
               if (!who.Equals(resultWho)) {
                    ++rit;
                    continue;
                }
            }
            if (code != requestCode) {
                ++rit;
                continue;
            }

            rit = List< AutoPtr<ActivityResult> >::ReverseIterator(mResults->Erase(--(rit.GetBase())));
        }
    }
}

void ActivityRecord::AddNewIntentLocked(
    /* [in] */ IIntent* intent)
{
    if (mNewIntents == NULL) {
        mNewIntents = new List< AutoPtr<IIntent> >();
    }
    mNewIntents->PushBack(intent);
}

ECode ActivityRecord::DeliverNewIntentLocked(
    /* [in] */ Int32 callingUid,
    /* [in] */ IIntent* _intent)
{
    // The activity now gets access to the data associated with this Intent.
    FAIL_RETURN(mService->GrantUriPermissionFromIntentLocked(callingUid, mPackageName,
            _intent, GetUriPermissionsLocked(), mUserId));
    // We want to immediately deliver the intent to the activity if
    // it is currently the top resumed activity...  however, if the
    // device is sleeping, then all activities are stopped, so in that
    // case we will deliver it if this is the current top activity on its
    // stack.
    Boolean unsent = TRUE;
    if ((mState == ActivityState_RESUMED || (mService->IsSleeping()
            && mTask->mStack->TopRunningActivityLocked(NULL).Get() == this))
            && mApp != NULL && mApp->mThread != NULL) {
        // try {
        AutoPtr<IList> ar;
        CArrayList::New((IList**)&ar);
        AutoPtr<IIntent> intent;
        CIntent::New(_intent, (IIntent**)&intent);
        ar->Add(intent);
        ECode ec = mApp->mThread->ScheduleNewIntent(ar, IBinder::Probe(mAppToken));
        if (SUCCEEDED(ec)) {
            unsent = FALSE;
        }
        else {
            Slogger::W(TAG, "Exception thrown sending new intent to %s, error code: %08x",
                ToString().string(), ec);
        }
    }
    if (unsent) {
        AutoPtr<IIntent> intent;
        CIntent::New(_intent, (IIntent**)&intent);
        AddNewIntentLocked(intent);
    }
    return NOERROR;
}

void ActivityRecord::UpdateOptionsLocked(
    /* [in] */ IBundle* options)
{
    if (options != NULL) {
        if (mPendingOptions != NULL) {
            mPendingOptions->Abort();
            mPendingOptions = NULL;
        }
        CActivityOptions::New(options, (IActivityOptions**)&mPendingOptions);
    }
}

void ActivityRecord::UpdateOptionsLocked(
    /* [in] */ IActivityOptions* options)
{
    if (options != NULL) {
        if (mPendingOptions != NULL) {
            mPendingOptions->Abort();
        }
        mPendingOptions = options;
    }
}

void ActivityRecord::ApplyOptionsLocked()
{
    Int32 animationType;
    if (mPendingOptions != NULL && (mPendingOptions->GetAnimationType(&animationType),
        animationType != IActivityOptions::ANIM_SCENE_TRANSITION)) {
        switch (animationType) {
            case IActivityOptions::ANIM_CUSTOM:
            {
                String packageName;
                Int32 enterResId, exitResId;
                AutoPtr<IIRemoteCallback> listener;
                mPendingOptions->GetPackageName(&packageName);
                mPendingOptions->GetCustomEnterResId(&enterResId);
                mPendingOptions->GetCustomExitResId(&exitResId);
                mPendingOptions->GetOnAnimationStartListener((IIRemoteCallback**)&listener);
                mService->mWindowManager->OverridePendingAppTransition(
                    packageName, enterResId, exitResId, listener);
                break;
            }
            case IActivityOptions::ANIM_SCALE_UP:
            {
                Int32 x, y, w, h;
                mPendingOptions->GetStartX(&x);
                mPendingOptions->GetStartY(&y);
                mPendingOptions->GetWidth(&w);
                mPendingOptions->GetHeight(&h);
                mService->mWindowManager->OverridePendingAppTransitionScaleUp(x, y, w, h);
                AutoPtr<IRect> rect;
                mIntent->GetSourceBounds((IRect**)&rect);
                if (rect == NULL) {
                    CRect::New(x, y, x + w, y + h, (IRect**)&rect);
                    mIntent->SetSourceBounds(rect);
                }
                break;
            }
            case IActivityOptions::ANIM_THUMBNAIL_SCALE_UP:
            case IActivityOptions::ANIM_THUMBNAIL_SCALE_DOWN:
            {
                Boolean UNUSED(scaleUp) = (animationType == IActivityOptions::ANIM_THUMBNAIL_SCALE_UP);
                AutoPtr<IBitmap> thumb;
                mPendingOptions->GetThumbnail((IBitmap**)&thumb);
                Int32 x, y;
                mPendingOptions->GetStartX(&x);
                mPendingOptions->GetStartY(&y);
                AutoPtr<IIRemoteCallback> listener;
                mPendingOptions->GetOnAnimationStartListener((IIRemoteCallback**)&listener);
                mService->mWindowManager->OverridePendingAppTransitionThumb(
                    thumb, x, y, listener, scaleUp);
                AutoPtr<IRect> rect;
                mIntent->GetSourceBounds((IRect**)&rect);
                if (rect == NULL) {
                    Int32 w, h;
                    thumb->GetWidth(&w);
                    thumb->GetHeight(&h);
                    CRect::New(x, y, x + w, y + h, (IRect**)&rect);
                    mIntent->SetSourceBounds(rect);
                }
                break;
            }
            case IActivityOptions::ANIM_THUMBNAIL_ASPECT_SCALE_UP:
            case IActivityOptions::ANIM_THUMBNAIL_ASPECT_SCALE_DOWN:
            {
                AutoPtr<IBitmap> thumbnail;
                mPendingOptions->GetThumbnail((IBitmap**)&thumbnail);
                Int32 x, y, w, h;
                mPendingOptions->GetStartX(&x);
                mPendingOptions->GetStartY(&y);
                mPendingOptions->GetWidth(&w);
                mPendingOptions->GetHeight(&h);
                AutoPtr<IIRemoteCallback> cb;
                mPendingOptions->GetOnAnimationStartListener((IIRemoteCallback**)&cb);
                mService->mWindowManager->OverridePendingAppTransitionAspectScaledThumb(
                    thumbnail, x, y, w, h, cb,
                    (animationType == IActivityOptions::ANIM_THUMBNAIL_ASPECT_SCALE_UP));
                AutoPtr<IRect> rect;
                mIntent->GetSourceBounds((IRect**)&rect);
                if (rect == NULL) {
                    CRect::New(x, y, x + w, y + h, (IRect**)&rect);
                    mIntent->SetSourceBounds(rect);
                }
                break;
            }
            default:
                Slogger::E(TAG, "applyOptionsLocked: Unknown animationType=%d", animationType);
                break;
        }
        mPendingOptions = NULL;
    }
}

AutoPtr<IActivityOptions> ActivityRecord::GetOptionsForTargetActivityLocked()
{
    AutoPtr<IActivityOptions> options;
    if (mPendingOptions != NULL)
        mPendingOptions->ForTargetActivity((IActivityOptions**)&options);
    return options;
}

void ActivityRecord::ClearOptionsLocked()
{
    if (mPendingOptions != NULL) {
        mPendingOptions->Abort();
        mPendingOptions = NULL;
    }
}

AutoPtr<IActivityOptions> ActivityRecord::TakeOptionsLocked()
{
    AutoPtr<IActivityOptions> opts = mPendingOptions;
    mPendingOptions = NULL;
    return opts;
}

void ActivityRecord::RemoveUriPermissionsLocked()
{
    if (mUriPermissions != NULL) {
        mUriPermissions->RemoveUriPermissionsLocked();
        mUriPermissions = NULL;
    }
}

void ActivityRecord::PauseKeyDispatchingLocked()
{
    if (!mKeysPaused) {
        mKeysPaused = TRUE;
        mService->mWindowManager->PauseKeyDispatching(IBinder::Probe(mAppToken));
    }
}

void ActivityRecord::ResumeKeyDispatchingLocked()
{
    if (mKeysPaused) {
        mKeysPaused = FALSE;
        mService->mWindowManager->ResumeKeyDispatching(IBinder::Probe(mAppToken));
    }
}

void ActivityRecord::UpdateThumbnail(
    /* [in] */ IBitmap* newThumbnail,
    /* [in] */ ICharSequence* description)
{
    if (newThumbnail != NULL) {
        if (CActivityManagerService::DEBUG_THUMBNAILS) {
            Slogger::I(TAG, "Setting thumbnail of %s to %p",
                ToString().string(), newThumbnail);
        }
        Boolean thumbnailUpdated = mTask->SetLastThumbnail(newThumbnail);
        if (thumbnailUpdated && IsPersistable()) {
            mStackSupervisor->mService->NotifyTaskPersisterLocked(mTask, FALSE);
        }
    }
    mTask->mLastDescription = description;
}

void ActivityRecord::StartLaunchTickingLocked()
{
    if (CActivityManagerService::IS_USER_BUILD) {
        return;
    }
    if (mLaunchTickTime == 0) {
        mLaunchTickTime = SystemClock::GetUptimeMillis();
        ContinueLaunchTickingLocked();
    }
}

Boolean ActivityRecord::ContinueLaunchTickingLocked()
{
    if (mLaunchTickTime != 0) {
        ActivityStack* stack = mTask->mStack;
        AutoPtr<IMessage> msg;
        stack->mHandler->ObtainMessage(ActivityStack::LAUNCH_TICK_MSG, (IMessage**)&msg);
        msg->SetObj(TO_IINTERFACE(this));

        stack->mHandler->RemoveMessages(ActivityStack::LAUNCH_TICK_MSG);
        Boolean result;
        stack->mHandler->SendMessageDelayed(msg, ActivityStack::LAUNCH_TICK, &result);
        return TRUE;
    }
    return FALSE;
}

ECode ActivityRecord::FinishLaunchTickingLocked()
{
    mLaunchTickTime = 0;
    return mTask->mStack->mHandler->RemoveMessages(ActivityStack::LAUNCH_TICK_MSG);
}

Boolean ActivityRecord::MayFreezeScreenLocked(
    /* [in] */ ProcessRecord* app)
{
    // Only freeze the screen if this activity is currently attached to
    // an application, and that application is not blocked or unresponding.
    // In any other case, we can't count on getting the screen unfrozen,
    // so it is best to leave as-is.
    return app != NULL && (!app->mCrashing && !app->mNotResponding);
}

ECode ActivityRecord::StartFreezingScreenLocked(
    /* [in] */ ProcessRecord* app,
    /* [in] */ Int32 configChanges)
{
    Boolean b = MayFreezeScreenLocked(app);
    if (b) {
        return mService->mWindowManager->StartAppFreezingScreen(IBinder::Probe(mAppToken), configChanges);
    }
    return NOERROR;
}

ECode ActivityRecord::StopFreezingScreenLocked(
    /* [in] */ Boolean force)
{
    if (force || mFrozenBeforeDestroy) {
        mFrozenBeforeDestroy = FALSE;
        FAIL_RETURN(mService->mWindowManager->StopAppFreezingScreen(IBinder::Probe(mAppToken), force));
    }
    return NOERROR;
}

ECode ActivityRecord::ReportFullyDrawnLocked()
{
    Int64 curTime = SystemClock::GetUptimeMillis();
    if (mDisplayStartTime != 0) {
        ReportLaunchTimeLocked(curTime);
    }
    if (mFullyDrawnStartTime != 0) {
        ActivityStack* stack = mTask->mStack;
        Int64 thisTime = curTime - mFullyDrawnStartTime;
        Int64 totalTime = stack->mFullyDrawnStartTime != 0
                ? (curTime - stack->mFullyDrawnStartTime) : thisTime;
        if (CActivityManagerService::SHOW_ACTIVITY_START_TIME) {
            // Trace.asyncTraceEnd(Trace.TRACE_TAG_ACTIVITY_MANAGER, "drawing", 0);
            // EventLog.writeEvent(EventLogTags.AM_ACTIVITY_FULLY_DRAWN_TIME,
            //         userId, System.identityHashCode(this), shortComponentName,
            //         thisTime, totalTime);
            StringBuilder* sb = mService->mStringBuilder;
            sb->SetLength(0);
            sb->Append("Fully drawn ");
            sb->Append(mShortComponentName);
            sb->Append(": ");
            TimeUtils::FormatDuration(thisTime, *sb);
            if (thisTime != totalTime) {
                sb->Append(" (total ");
                TimeUtils::FormatDuration(totalTime, *sb);
                sb->Append(")");
            }
            Logger::I(TAG, sb->ToString().string());
        }
        if (totalTime > 0) {
            //service.mUsageStatsService.noteFullyDrawnTime(realActivity, (Int32) totalTime);
        }
        mFullyDrawnStartTime = 0;
        stack->mFullyDrawnStartTime = 0;
    }
    return NOERROR;
}

void ActivityRecord::ReportLaunchTimeLocked(
    /* [in] */ Int64 curTime)
{
    ActivityStack* stack = mTask->mStack;
    Int64 thisTime = curTime - mDisplayStartTime;
    Int64 totalTime = stack->mLaunchStartTime != 0
            ? (curTime - stack->mLaunchStartTime) : thisTime;
    if (CActivityManagerService::SHOW_ACTIVITY_START_TIME) {
        // Trace.asyncTraceEnd(Trace.TRACE_TAG_ACTIVITY_MANAGER, "launching", 0);
        // EventLog.writeEvent(EventLogTags.AM_ACTIVITY_LAUNCH_TIME,
        //         userId, System.identityHashCode(this), shortComponentName,
        //         thisTime, totalTime);
        StringBuilder* sb = mService->mStringBuilder;
        sb->SetLength(0);
        sb->Append("Displayed ");
        sb->Append(mShortComponentName);
        sb->Append(": ");
        TimeUtils::FormatDuration(thisTime, *sb);
        if (thisTime != totalTime) {
            sb->Append(" (total ");
            TimeUtils::FormatDuration(totalTime, *sb);
            sb->Append(")");
        }
        Logger::I(TAG, sb->ToString().string());
    }
    mStackSupervisor->ReportActivityLaunchedLocked(FALSE, this, thisTime, totalTime);
    if (totalTime > 0) {
        //service.mUsageStatsService.noteLaunchTime(realActivity, (Int32)totalTime);
    }
    mDisplayStartTime = 0;
    stack->mLaunchStartTime = 0;
}

ECode ActivityRecord::WindowsDrawn()
{
    AutoLock lock(mService);

    if (mDisplayStartTime != 0) {
        ReportLaunchTimeLocked(SystemClock::GetUptimeMillis());
    }
    mStackSupervisor->SendWaitingVisibleReportLocked(this);
    mStartTime = 0;
    FinishLaunchTickingLocked();
    if (mTask != NULL) {
        mTask->mHasBeenVisible = TRUE;
    }
    return NOERROR;
}

ECode ActivityRecord::WindowsVisible()
{
    AutoLock lock(mService);

    mStackSupervisor->ReportActivityVisibleLocked(this);
    if (CActivityManagerService::DEBUG_SWITCH) {
        Logger::V(TAG, "windowsVisible(): %s", ToString().string());
    }
    if (!mNowVisible) {
        mNowVisible = TRUE;
        mLastVisibleTime = SystemClock::GetUptimeMillis();
        if (!mIdle) {
            // Instead of doing the full stop routine here, let's just
            // hide any activities we now can, and let them stop when
            // the normal idle happens.
            mStackSupervisor->ProcessStoppingActivitiesLocked(FALSE);
        }
        else {
            // If this activity was already idle, then we now need to
            // make sure we perform the full stop of any activities
            // that are waiting to do so.    This is because we won't
            // do that while they are still waiting for this one to
            // become visible.
            Int32 N;
            mStackSupervisor->mWaitingVisibleActivities->GetSize(&N);
            if (N > 0) {
                for (Int32 i = 0; i < N; i++) {
                    AutoPtr<IInterface> item;
                    mStackSupervisor->mWaitingVisibleActivities->Get(i, (IInterface**)&item);
                    AutoPtr<ActivityRecord> r = (ActivityRecord*)IActivityRecord::Probe(item);
                    r->mWaitingVisible = FALSE;
                    if (CActivityManagerService::DEBUG_SWITCH) Logger::V(
                        CActivityManagerService::TAG, "Was waiting for visible: %s", r->ToString().string());
                }
                mStackSupervisor->mWaitingVisibleActivities->Clear();
                mStackSupervisor->ScheduleIdleLocked();
            }
        }
        mService->ScheduleAppGcsLocked();
    }
    Logger::I(TAG_TIMELINE, "Timeline: Activity_windows_visible id: %s time:%lld",
        ToString().string(), SystemClock::GetUptimeMillis());
    return NOERROR;
}

ECode ActivityRecord::WindowsGone()
{
    if (CActivityManagerService::DEBUG_SWITCH) {
        String info;
        ToString(&info);
        Slogger::V(TAG, "windowsGone(): %s", info.string());
    }
    mNowVisible = FALSE;
    return NOERROR;
}

AutoPtr<ActivityRecord> ActivityRecord::GetWaitingHistoryRecordLocked()
{
    // First find the real culprit...  if we are waiting
    // for another app to start, then we have paused dispatching
    // for this activity.
    AutoPtr<ActivityRecord> r = this;
    if (r->mWaitingVisible || r->mStopped) {
        AutoPtr<ActivityStack> stack = mStackSupervisor->GetFocusedStack();
        // Hmmm, who might we be waiting for?
        r = stack->mResumedActivity;
        if (r == NULL) {
            r = stack->mPausingActivity;
        }
        // Both of those NULL?  Fall back to 'this' again
        if (r == NULL) {
            r = this;
        }
    }

    return r;
}

ECode ActivityRecord::KeyDispatchingTimedOut(
    /* [in] */ const String& reason,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    AutoPtr<ActivityRecord> r;
    AutoPtr<ProcessRecord> anrApp;
    {
        AutoLock lock(mService);

        r = GetWaitingHistoryRecordLocked();
        anrApp = r != NULL ? r->mApp : NULL;
    }
    return mService->InputDispatchingTimedOut(anrApp, r, this, FALSE, reason, result);
}

ECode ActivityRecord::GetKeyDispatchingTimeout(
    /* [out] */ Int64* timeout)
{
    VALIDATE_NOT_NULL(timeout);

    AutoLock lock(mService);

    AutoPtr<ActivityRecord> r = GetWaitingHistoryRecordLocked();
    *timeout = CActivityManagerService::GetInputDispatchingTimeoutLocked(r);
    return NOERROR;
}

Boolean ActivityRecord::IsInterestingToUserLocked()
{
    return mVisible || mNowVisible || mState == ActivityState_PAUSING ||
            mState == ActivityState_RESUMED;
}

ECode ActivityRecord::SetSleeping(
    /* [in] */ Boolean sleeping)
{
    if (mSleeping == sleeping) {
        return NOERROR;
    }
    if (mApp != NULL && mApp->mThread != NULL) {
        if (FAILED(mApp->mThread->ScheduleSleeping(IBinder::Probe(mAppToken), sleeping))) {
            AutoPtr<IComponentName> component;
            mIntent->GetComponent((IComponentName**)&component);
            Slogger::W(TAG, "Exception thrown when sleeping: %s", TO_CSTR(component));
        }
        else {
            Boolean contains;
            if (sleeping && !(mStackSupervisor->mGoingToSleepActivities->Contains(
                TO_IINTERFACE(this), &contains), contains)) {
                mStackSupervisor->mGoingToSleepActivities->Add(TO_IINTERFACE(this));
            }
            mSleeping = sleeping;
        }
    }
    return NOERROR;
}

void ActivityRecord::ActivityResumedLocked(
    /* [in] */ IBinder* token)
{
    AutoPtr<ActivityRecord> r = ForToken(token);
    if (DEBUG_SAVED_STATE)
        Slogger::I(TAG, "Resumed activity; dropping state of: %s",
            r->ToString().string());
    r->mIcicle = NULL;
    r->mHaveState = FALSE;
}

Int32 ActivityRecord::GetTaskForActivityLocked(
    /* [in] */ IBinder* token,
    /* [in] */ Boolean onlyRoot)
{
    AutoPtr<ActivityRecord> r = ForToken(token);
    if (r == NULL) {
        return -1;
    }
    TaskRecord* task = r->mTask;
    Int32 activityNdx;
    task->mActivities->IndexOf((IActivityRecord*)r, &activityNdx);
    if (activityNdx < 0 || (onlyRoot && activityNdx > task->FindEffectiveRootIndex())) {
        return -1;
    }
    return task->mTaskId;
}

AutoPtr<ActivityRecord> ActivityRecord::IsInStackLocked(
    /* [in] */ IBinder* token)
{
    AutoPtr<ActivityRecord> r = ForToken(token);
    if (r != NULL) {
        return r->mTask->mStack->IsInStackLocked(token);
    }
    return NULL;
}

AutoPtr<ActivityStack> ActivityRecord::GetStackLocked(
    /* [in] */ IBinder* token)
{
    AutoPtr<ActivityRecord> r = IsInStackLocked(token);
    if (r != NULL) {
        return r->mTask->mStack;
    }
    return NULL;
}

Boolean ActivityRecord::IsDestroyable()
{
    if (mFinishing || mApp == NULL || mState == ActivityState_DESTROYING
            || mState == ActivityState_DESTROYED) {
        // This would be redundant.
        return FALSE;
    }
    if (mTask == NULL || mTask->mStack == NULL || this == mTask->mStack->mResumedActivity
            || this == mTask->mStack->mPausingActivity || !mHaveState || !mStopped) {
        // We're not ready for this kind of thing.
        return FALSE;
    }
    if (mVisible) {
        // The user would notice this!
        return FALSE;
    }
    return true;
}

String ActivityRecord::CreateImageFilename(
    /* [in] */ Int64 createTime,
    /* [in] */ Int32 taskId)
{
    StringBuilder sb;
    sb += taskId;
    sb += ACTIVITY_ICON_SUFFIX;
    sb += createTime;
    sb += TaskPersister::IMAGE_EXTENSION;
    return sb.ToString();
}

void ActivityRecord::SetTaskDescription(
    /* [in] */ IActivityManagerTaskDescription* taskDescription)
{
    String name;
    AutoPtr<IBitmap> icon;
    if ((taskDescription->GetIconFilename(&name), name == NULL) &&
        (taskDescription->GetIcon((IBitmap**)&icon), icon) != NULL) {
        String iconFilename = CreateImageFilename(mCreateTime, mTask->mTaskId);
        mStackSupervisor->mService->mTaskPersister->SaveImage(icon, iconFilename);
        taskDescription->SetIconFilename(iconFilename);
    }
    mTaskDescription = taskDescription;
}

ECode ActivityRecord::SaveToXml(
    /* [in] */ IXmlSerializer* out)
{
    FAIL_RETURN(out->WriteAttribute(String(NULL), ATTR_ID,
        StringUtils::ToString(mCreateTime)));

    FAIL_RETURN(out->WriteAttribute(String(NULL), ATTR_LAUNCHEDFROMUID,
        StringUtils::ToString(mLaunchedFromUid)));

    if (mLaunchedFromPackage != NULL) {
        FAIL_RETURN(out->WriteAttribute(String(NULL), ATTR_LAUNCHEDFROMPACKAGE, mLaunchedFromPackage));
    }
    if (mResolvedType != NULL) {
        FAIL_RETURN(out->WriteAttribute(String(NULL), ATTR_RESOLVEDTYPE, mResolvedType));
    }
    FAIL_RETURN(out->WriteAttribute(String(NULL), ATTR_COMPONENTSPECIFIED,
        StringUtils::ToString(mComponentSpecified)));

    FAIL_RETURN(out->WriteAttribute(String(NULL), ATTR_USERID,
        StringUtils::ToString(mUserId)));

    if (mTaskDescription != NULL) {
        FAIL_RETURN(mTaskDescription->SaveToXml(out));
    }

    FAIL_RETURN(out->WriteStartTag(String(NULL), TAG_INTENT));
    FAIL_RETURN(mIntent->SaveToXml(out));
    FAIL_RETURN(out->WriteEndTag(String(NULL), TAG_INTENT));

    if (IsPersistable() && mPersistentState != NULL) {
        FAIL_RETURN(out->WriteStartTag(String(NULL), TAG_PERSISTABLEBUNDLE));
        FAIL_RETURN(mPersistentState->SaveToXml(out));
        FAIL_RETURN(out->WriteEndTag(String(NULL), TAG_PERSISTABLEBUNDLE));
    }
    return NOERROR;
}

ECode ActivityRecord::RestoreFromXml(
    /* [in] */ IXmlPullParser* in,
    /* [in] */ Int32 taskId,
    /* [in] */ ActivityStackSupervisor* stackSupervisor,
    /* [out] */ ActivityRecord** ar)
{
    VALIDATE_NOT_NULL(ar)
    AutoPtr<IIntent> intent;
    AutoPtr<IPersistableBundle> persistentState;
    Int32 launchedFromUid = 0;
    String launchedFromPackage;
    String resolvedType;
    Boolean componentSpecified = FALSE;
    Int32 userId = 0;
    Int64 createTime = -1;
    Int32 outerDepth;
    FAIL_RETURN(in->GetDepth(&outerDepth));
    AutoPtr<IActivityManagerTaskDescription> taskDescription;
    CActivityManagerTaskDescription::New((IActivityManagerTaskDescription**)&taskDescription);

    Int32 count;
    FAIL_RETURN(in->GetAttributeCount(&count));
    for (Int32 attrNdx = count - 1; attrNdx >= 0; --attrNdx) {
        String attrName;
        FAIL_RETURN(in->GetAttributeName(attrNdx, &attrName));
        String attrValue;
        FAIL_RETURN(in->GetAttributeValue(attrNdx, &attrValue));
        if (TaskPersister::DEBUG) Slogger::D(TaskPersister::TAG,
            "ActivityRecord: attribute name=%s value=%s", attrName.string(), attrValue.string());
        if (ATTR_ID.Equals(attrName)) {
            createTime = StringUtils::ParseInt64(attrValue);
        }
        else if (ATTR_LAUNCHEDFROMUID.Equals(attrName)) {
            launchedFromUid = StringUtils::ParseInt32(attrValue);
        }
        else if (ATTR_LAUNCHEDFROMPACKAGE.Equals(attrName)) {
            launchedFromPackage = attrValue;
        }
        else if (ATTR_RESOLVEDTYPE.Equals(attrName)) {
            resolvedType = attrValue;
        }
        else if (ATTR_COMPONENTSPECIFIED.Equals(attrName)) {
            componentSpecified = StringUtils::ParseBoolean(attrValue);
        }
        else if (ATTR_USERID.Equals(attrName)) {
            userId = StringUtils::ParseInt32(attrValue);
        }
        else if (attrName.StartWith(IActivityManagerTaskDescription::ATTR_TASKDESCRIPTION_PREFIX)) {
            taskDescription->RestoreFromXml(attrName, attrValue);
        }
        else {
            Logger::D(TAG, "Unknown ActivityRecord attribute=%s", attrName.string());
        }
    }

    Int32 event, depth;
    while (((in->Next(&event)) != IXmlPullParser::END_DOCUMENT) &&
            (event != IXmlPullParser::END_TAG || (in->GetDepth(&depth), depth) < outerDepth)) {
        if (event == IXmlPullParser::START_TAG) {
            String name;
            FAIL_RETURN(in->GetName(&name));
            if (TaskPersister::DEBUG) Slogger::D(TaskPersister::TAG,
                    "ActivityRecord: START_TAG name=%s", name.string());
            if (TAG_INTENT.Equals(name)) {
                AutoPtr<IIntentHelper> helper;
                CIntentHelper::AcquireSingleton((IIntentHelper**)&helper);
                helper->RestoreFromXml(in, (IIntent**)&intent);
                if (TaskPersister::DEBUG) {
                    String str;
                    IObject::Probe(intent)->ToString(&str);
                    Slogger::D(TaskPersister::TAG, "ActivityRecord: intent=%s", str.string());
                }
            }
            else if (TAG_PERSISTABLEBUNDLE.Equals(name)) {
                AutoPtr<IPersistableBundleHelper> helper;
                CPersistableBundleHelper::AcquireSingleton((IPersistableBundleHelper**)&helper);
                helper->RestoreFromXml(in, (IPersistableBundle**)&persistentState);
                if (TaskPersister::DEBUG) {
                    String str;
                    IObject::Probe(persistentState)->ToString(&str);
                    Slogger::D(TaskPersister::TAG, "ActivityRecord: persistentState=%s", str.string());
                }
            }
            else {
                Slogger::W(TAG, "restoreActivity: unexpected name=%s", name.string());
                XmlUtils::SkipCurrentTag(in);
            }
        }
    }

    if (intent == NULL) {
        Slogger::E(TAG, "restoreActivity error intent=NULL");
        return E_XML_PULL_PARSER_EXCEPTION;
    }

    CActivityManagerService* service = stackSupervisor->mService;
    AutoPtr<IActivityInfo> aInfo = stackSupervisor->ResolveActivity(
        intent, resolvedType, 0, NULL, userId);
    if (aInfo == NULL) {
        Slogger::E(TAG, "restoreActivity resolver error. Intent=%s resolvedType=%s",
            TO_CSTR(intent), resolvedType.string());
        return E_XML_PULL_PARSER_EXCEPTION;
    }
    AutoPtr<IConfiguration> config;
    service->GetConfiguration((IConfiguration**)&config);
    AutoPtr<ActivityRecord> r = new ActivityRecord();
    r->constructor(service, /*caller*/NULL, launchedFromUid,
            launchedFromPackage, intent, resolvedType, aInfo, config,
            NULL, String(NULL), 0, componentSpecified, stackSupervisor, NULL, NULL);

    r->mPersistentState = persistentState;
    r->mTaskDescription = taskDescription;
    r->mCreateTime = createTime;

    *ar = r;
    REFCOUNT_ADD(*ar)
    return NOERROR;
}

String ActivityRecord::ActivityTypeToString(
    /* [in] */ Int32 type)
{
    switch (type) {
        case APPLICATION_ACTIVITY_TYPE:
            return String("APPLICATION_ACTIVITY_TYPE");
        case HOME_ACTIVITY_TYPE:
            return String("HOME_ACTIVITY_TYPE");
        case RECENTS_ACTIVITY_TYPE:
            return String("RECENTS_ACTIVITY_TYPE");
        default:
            return StringUtils::ToString(type);
    }
}

ECode ActivityRecord::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str);
    *str = ToString();
    return NOERROR;
}

String ActivityRecord::ToString()
{
    if (mStringName != NULL) {
        StringBuilder sb(128);
        sb += mStringName;
        sb += " t";
        sb += (mTask == NULL ? -1 : mTask->mTaskId);
        sb += (mFinishing ? " f}" : "}");
        return sb.ToString();
    }
    StringBuilder sb(128);
    sb += "ActivityRecord{0X";
    sb += StringUtils::ToHexString((Int32)this);
    sb += " u";
    sb += mUserId;
    sb += ' ';
    AutoPtr<IComponentName> component;
    mIntent->GetComponent((IComponentName**)&component);
    String shortComponentName;
    component->FlattenToShortString(&shortComponentName);
    sb += shortComponentName;
    mStringName = sb.ToString();
    return ToString();
}

} // namespace Am
} // namespace Server
} // namespace Droid
} // namespace Elastos
