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

#include "elastos/droid/server/am/ActivityContainer.h"
#include "elastos/droid/server/am/ActivityStackSupervisor.h"
#include "elastos/droid/os/Binder.h"

#include <Elastos.Droid.Os.h>
#include <Elastos.Droid.Content.h>
#include <Elastos.Droid.Graphics.h>
#include <Elastos.Droid.View.h>
#include <Elastos.CoreLibrary.Utility.h>

using Elastos::Droid::Os::Binder;
using Elastos::Droid::Os::EIID_IBinder;
using Elastos::Droid::App::EIID_IIActivityContainer;
using Elastos::Droid::Hardware::Input::IInputManager;
using Elastos::Droid::Hardware::Display::IDisplayManagerGlobal;
using Elastos::Droid::Hardware::Display::CDisplayManagerGlobalHelper;
using Elastos::Droid::Hardware::Display::IDisplayManagerGlobalHelper;
using Elastos::Droid::View::CDisplayInfo;
using Elastos::Utility::CArrayList;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Am {

static const String TAG("ActivityContainer");
static const String VIRTUAL_DISPLAY_BASE_NAME("ActivityViewVirtualDisplay");

//=====================================================================
//               ActivityDisplay
//=====================================================================
ActivityDisplay::ActivityDisplay(
    /* [in] */ ActivityStackSupervisor* owner)
    : mOwner(owner)
{
    CDisplayInfo::New((IDisplayInfo**)&mDisplayInfo);
    CArrayList::New((IArrayList**)&mStacks);
}

ActivityDisplay::ActivityDisplay(
    /* [in] */ Int32 displayId,
    /* [in] */ ActivityStackSupervisor* owner)
    : mOwner(owner)
{
    CDisplayInfo::New((IDisplayInfo**)&mDisplayInfo);
    CArrayList::New((IArrayList**)&mStacks);
    AutoPtr<IDisplay> display;
    mOwner->mDisplayManager->GetDisplay(displayId, (IDisplay**)&display);
    assert(display != NULL);
    Init(display);
}

ECode ActivityDisplay::Init(
    /* [in] */ IDisplay* display)
{
    mDisplay = display;
    display->GetDisplayId(&mDisplayId);
    Boolean res;
    mDisplay->GetDisplayInfo(mDisplayInfo, &res);
    return NOERROR;
}

ECode ActivityDisplay::AttachActivities(
    /* [in] */ ActivityStack* stack)
{
    if (CActivityManagerService::DEBUG_STACK) {
        Slogger::V(TAG, "attachActivities: attaching %s to displayId=%d", TO_CSTR(stack), mDisplayId);
    }
    mStacks->Add(TO_IINTERFACE(stack));
    return NOERROR;
}

ECode ActivityDisplay::DetachActivitiesLocked(
    /* [in] */ ActivityStack* stack)
{
    if (CActivityManagerService::DEBUG_STACK) {
        Slogger::V(TAG, "attachActivities: detaching %s from displayId=%d", TO_CSTR(stack), mDisplayId);
    }
    mStacks->Remove(TO_IINTERFACE(stack));
    return NOERROR;
}

ECode ActivityDisplay::GetBounds(
    /* [in] */ IPoint* bounds)
{
    Boolean res;
    mDisplay->GetDisplayInfo(mDisplayInfo, &res);
    Int32 appWidth, appHeight;
    mDisplayInfo->GetAppWidth(&appWidth);
    mDisplayInfo->GetAppHeight(&appHeight);
    bounds->Set(appWidth, appHeight);
    return NOERROR;
}

ECode ActivityDisplay::SetVisibleBehindActivity(
    /* [in] */ IActivityRecord* r)
{
    mVisibleBehindActivity = r;
    return NOERROR;
}

Boolean ActivityDisplay::HasVisibleBehindActivity()
{
    return mVisibleBehindActivity != NULL;
}

ECode ActivityDisplay::ToString(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    Int32 size;
    mStacks->GetSize(&size);

    StringBuilder sb("ActivityDisplay={");
    sb += "displayId=";
    sb += mDisplayId;
    sb += " numStacks=";
    sb += size;;
    sb += "}";
    *result = sb.ToString();
    return NOERROR;
}

//=====================================================================
//           VirtualActivityDisplay
//=====================================================================
VirtualActivityDisplay::VirtualActivityDisplay(
    /* [in] */ Int32 width,
    /* [in] */ Int32 height,
    /* [in] */ Int32 density,
    /* [in] */ ActivityStackSupervisor* owner)
    : ActivityDisplay(owner)
{
    AutoPtr<IDisplayManagerGlobalHelper> helper;
    CDisplayManagerGlobalHelper::AcquireSingleton((IDisplayManagerGlobalHelper**)&helper);
    AutoPtr<IDisplayManagerGlobal> dm;
    helper->GetInstance((IDisplayManagerGlobal**)&dm);
    dm->CreateVirtualDisplay(mOwner->mService->mContext, NULL,
        VIRTUAL_DISPLAY_BASE_NAME, width, height, density, NULL,
        IDisplayManager::VIRTUAL_DISPLAY_FLAG_PUBLIC |
        IDisplayManager::VIRTUAL_DISPLAY_FLAG_OWN_CONTENT_ONLY,
        NULL, NULL, (IVirtualDisplay**)&mVirtualDisplay);

    AutoPtr<IDisplay> display;
    mVirtualDisplay->GetDisplay((IDisplay**)&display);
    assert(display != NULL);
    Init(display);

    mOwner->mWindowManager->HandleDisplayAdded(mDisplayId);
}

ECode VirtualActivityDisplay::SetSurface(
    /* [in] */ ISurface* surface)
{
    if (mVirtualDisplay != NULL) {
        mVirtualDisplay->SetSurface(surface);
    }
    return NOERROR;
}

ECode VirtualActivityDisplay::DetachActivitiesLocked(
    /* [in] */ ActivityStack* stack)
{
    ActivityDisplay::DetachActivitiesLocked(stack);
    if (mVirtualDisplay != NULL) {
        assert(0 && "TODO");
        // mVirtualDisplay->ReleaseResources();
        mVirtualDisplay = NULL;
    }
    return NOERROR;
}

ECode VirtualActivityDisplay::ToString(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    StringBuilder sb("VirtualActivityDisplay={");
    sb += "displayId=";
    sb += mDisplayId;
    sb += "}";
    *result = sb.ToString();
    return NOERROR;
}

//=====================================================================
// ActivityContainer
//=====================================================================
CAR_INTERFACE_IMPL_2(ActivityContainer, Object, IIActivityContainer, IBinder)

const Int32 ActivityContainer::FORCE_NEW_TASK_FLAGS =
    IIntent::FLAG_ACTIVITY_NEW_TASK | IIntent::FLAG_ACTIVITY_MULTIPLE_TASK | IIntent::FLAG_ACTIVITY_NO_ANIMATION;
const Int32 ActivityContainer::CONTAINER_STATE_HAS_SURFACE = 0;
const Int32 ActivityContainer::CONTAINER_STATE_NO_SURFACE = 1;
const Int32 ActivityContainer::CONTAINER_STATE_FINISHING = 2;

ActivityContainer::ActivityContainer()
    : mVisible(FALSE)
    , mContainerState(0)
    , mOwner(NULL)
{
}

ECode ActivityContainer::constructor(
    /* [in] */ Int32 stackId,
    /* [in] */ IDisplayListener* owner)
{
    mVisible = TRUE;
    mContainerState = CONTAINER_STATE_HAS_SURFACE;
    mOwner = (ActivityStackSupervisor*)owner;

    {
        AutoLock lock(mOwner->mService);
        mStackId = stackId;
        mStack = new ActivityStack(this);

        StringBuilder sb("ActivtyContainer{");
        sb += " stackId=";
        sb += mStackId;
        sb += "}";
        mIdString = sb.ToString();
        if (CActivityManagerService::DEBUG_STACK) Slogger::D(TAG, "Creating ActivityContainer %s", TO_CSTR(this));
    }
    return NOERROR;
}

ECode ActivityContainer::AttachToDisplayLocked(
    /* [in] */ ActivityDisplay* activityDisplay)
{
    if (CActivityManagerService::DEBUG_STACK) {
        Slogger::D(TAG, "attachToDisplayLocked: %s to display=%s",
            TO_CSTR(this), TO_CSTR(activityDisplay));
    }

    mActivityDisplay = activityDisplay;
    mStack->mDisplayId = activityDisplay->mDisplayId;
    mStack->mStacks = activityDisplay->mStacks;

    activityDisplay->AttachActivities(mStack);
    mOwner->mWindowManager->AttachStack(mStackId, activityDisplay->mDisplayId);
    return NOERROR;
}

ECode ActivityContainer::AttachToDisplay(
    /* [in] */ Int32 displayId)
{
    AutoLock lock(mOwner->mService);
    AutoPtr<IInterface> adobj;
    mOwner->mActivityDisplays->Get(displayId, (IInterface**)&adobj);
    ActivityDisplay* activityDisplay = (ActivityDisplay*)(IObject::Probe(adobj));
    if (activityDisplay == NULL) {
        return NOERROR;
    }
    AttachToDisplayLocked(activityDisplay);
    return NOERROR;
}

ECode ActivityContainer::GetDisplayId(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    {
        AutoLock lock(mOwner->mService);
        if (mActivityDisplay != NULL) {
            *result = mActivityDisplay->mDisplayId;
            return NOERROR;
        }
    }
    *result = -1;
    return NOERROR;
}

ECode ActivityContainer::InjectEvent(
    /* [in] */ IInputEvent* event,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;

    Int64 origId = Binder::ClearCallingIdentity();
    {
        AutoLock lock(mOwner->mService);
        if (mActivityDisplay != NULL) {
            Binder::RestoreCallingIdentity(origId);
            return mOwner->mInputManagerInternal->InjectInputEvent(event,
                    mActivityDisplay->mDisplayId,
                    IInputManager::INJECT_INPUT_EVENT_MODE_ASYNC, result);
        }
    }

    Binder::RestoreCallingIdentity(origId);
    return NOERROR;
}

ECode ActivityContainer::ReleaseResources()
{
    AutoLock lock(mOwner->mService);
    if (mContainerState == CONTAINER_STATE_FINISHING) {
        return NOERROR;
    }
    mContainerState = CONTAINER_STATE_FINISHING;

    AutoPtr<IMessage> msg;
    mOwner->mHandler->ObtainMessage(
        ActivityStackSupervisor::CONTAINER_TASK_LIST_EMPTY_TIMEOUT,
        TO_IINTERFACE(this), (IMessage**)&msg);
    Boolean res;
    mOwner->mHandler->SendMessageDelayed(msg, 2000, &res);

    Int64 origId = Binder::ClearCallingIdentity();
    //try {
        mStack->FinishAllActivitiesLocked(FALSE);
        mOwner->RemovePendingActivityLaunchesLocked(mStack);
    //} finally {
        Binder::RestoreCallingIdentity(origId);
    //}

    return NOERROR;
}

ECode ActivityContainer::StartActivity(
    /* [in] */ IIntent* intent,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = 0;

    FAIL_RETURN(mOwner->mService->EnforceNotIsolatedCaller(String("ActivityContainer.startActivity")))

    Int32 callingPid =  Binder::GetCallingPid();
    Int32 callingUid = Binder::GetCallingUid();
    Int32 userId;
    mOwner->mService->HandleIncomingUser(callingPid,
        callingUid, mOwner->mCurrentUser, FALSE,
        CActivityManagerService::ALLOW_FULL_ONLY,
        String("ActivityContainer"), String(NULL), &userId);
    // TODO: Switch to user app stacks here.
    intent->AddFlags(FORCE_NEW_TASK_FLAGS);
    String mimeType;
    intent->GetType(&mimeType);
    if (mimeType.IsNullOrEmpty()) {
        AutoPtr<IUri> iData;
        intent->GetData((IUri**)&iData);
        if (iData != NULL) {
            String scheme;
            iData->GetScheme(&scheme);
            if (scheme.Equals("content")) {
                mOwner->mService->GetProviderMimeType(iData, userId, &mimeType);
            }
        }
    }
    return mOwner->StartActivityMayWait(NULL, -1, String(NULL), intent, mimeType,
        NULL, NULL, NULL, String(NULL), 0,
        0, NULL, NULL, NULL, NULL, userId, this, NULL, result);
}

ECode ActivityContainer::StartActivityIntentSender(
    /* [in] */ IIIntentSender* intentSender,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = 0;

    FAIL_RETURN(mOwner->mService->EnforceNotIsolatedCaller(String("ActivityContainer.startActivityIntentSender")))

    IPendingIntentRecord* pir = IPendingIntentRecord::Probe(intentSender);
    if (pir == NULL) {
        Slogger::E("ActivityContainer::StartActivityIntentSender", "Bad PendingIntent object");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    String nullStr;
    *result = ((CPendingIntentRecord*)pir)->SendInner(
        0, NULL, nullStr, NULL, nullStr, NULL,
        nullStr, 0, FORCE_NEW_TASK_FLAGS, FORCE_NEW_TASK_FLAGS, NULL, this);
    return NOERROR;
}

ECode ActivityContainer::CheckEmbeddedAllowed(
    /* [in] */ IIntent* intent)
{
    return CheckEmbeddedAllowedInner(intent, String(NULL));
}

ECode ActivityContainer::CheckEmbeddedAllowedIntentSender(
    /* [in] */ IIIntentSender* intentSender)
{
    IPendingIntentRecord* pir = IPendingIntentRecord::Probe(intentSender);
    if (pir == NULL) {
        Slogger::E("ActivityContainer::CheckEmbeddedAllowedIntentSender", "Bad PendingIntent object");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    CPendingIntentRecord* pendingIntent = (CPendingIntentRecord*)pir;
    return CheckEmbeddedAllowedInner(pendingIntent->mKey->mRequestIntent,
            pendingIntent->mKey->mRequestResolvedType);
}

ECode ActivityContainer::SetSurface(
    /* [in] */ ISurface* surface,
    /* [in] */ Int32 width,
    /* [in] */ Int32 height,
    /* [in] */ Int32 density)
{
    return mOwner->mService->EnforceNotIsolatedCaller(String("ActivityContainer.attachToSurface"));
}

AutoPtr<ActivityStackSupervisor> ActivityContainer::GetOuter()
{
    return mOwner;
}

Boolean ActivityContainer::IsAttachedLocked()
{
    return mActivityDisplay != NULL;
}

ECode ActivityContainer::GetBounds(
    /* [in] */ IPoint* outBounds)
{
    VALIDATE_NOT_NULL(outBounds);

    AutoLock lock(mOwner->mService);
    if (mActivityDisplay != NULL) {
        mActivityDisplay->GetBounds(outBounds);
    } else {
        outBounds->Set(0, 0);
    }

    return NOERROR;
}

ECode ActivityContainer::SetVisible(
    /* [in] */ Boolean visible)
{
    if (mVisible != visible) {
        mVisible = visible;
        if (mCallback != NULL) {
            AutoPtr<IMessage> msg;
            mOwner->mHandler->ObtainMessage(
                ActivityStackSupervisor::CONTAINER_CALLBACK_VISIBILITY, visible ? 1 : 0,
                0 /* unused */, TO_IINTERFACE(this), (IMessage**)&msg);
            msg->SendToTarget();
        }
    }
    return NOERROR;
}

ECode ActivityContainer::SetDrawn()
{
    return NOERROR;
}

Boolean ActivityContainer::IsEligibleForNewTasks()
{
    return TRUE;
}

ECode ActivityContainer::OnTaskListEmptyLocked()
{
    return NOERROR;
}

ECode ActivityContainer::ToString(
    /* [out] */ String* info)
{
    VALIDATE_NOT_NULL(info);

    StringBuilder sb(mIdString);
    if (mActivityDisplay == NULL) {
        sb += "N";
    }
    else {
        sb += "A";
    }
    *info = sb.ToString();
    return NOERROR;
}

void ActivityContainer::DetachLocked()
{
    //if (CActivityManagerService::DEBUG_STACK) Slogger::D(TAG, "detachLocked: " + this + " from display="
    //        + mActivityDisplay + " Callers=" + Debug.getCallers(2));
    if (mActivityDisplay != NULL) {
        mActivityDisplay->DetachActivitiesLocked(mStack);
        mActivityDisplay = NULL;
        mStack->mDisplayId = -1;
        mStack->mStacks = NULL;
        mOwner->mWindowManager->DetachStack(mStackId);
    }
}

ECode ActivityContainer::CheckEmbeddedAllowedInner(
    /* [in] */ IIntent* intent,
    /* [in] */ const String& _resolvedType)
{
    Int32 callingPid = Binder::GetCallingPid();
    Int32 callingUid = Binder::GetCallingUid();
    Int32 userId;
    mOwner->mService->HandleIncomingUser(callingPid,
        callingUid, mOwner->mCurrentUser, FALSE,
        CActivityManagerService::ALLOW_FULL_ONLY, String("ActivityContainer"), String(NULL), &userId);

    String resolvedType = _resolvedType;
    if (resolvedType.IsNullOrEmpty()) {
        intent->GetType(&resolvedType);
        if (resolvedType.IsNullOrEmpty()) {
            AutoPtr<IUri> iData;
            intent->GetData((IUri**)&iData);
            if (iData != NULL) {
                String scheme;
                iData->GetScheme(&scheme);
                if (scheme.Equals("content")) {
                    mOwner->mService->GetProviderMimeType(iData, userId, &resolvedType);
                }
            }
        }
    }

    AutoPtr<IActivityInfo> aInfo = mOwner->ResolveActivity(intent, resolvedType, 0, NULL, userId);
    Int32 flags;
    if (aInfo != NULL && ((aInfo->GetFlags(&flags), flags) & IActivityInfo::FLAG_ALLOW_EMBEDDED) == 0) {
        Slogger::E(TAG, "Attempt to embed activity that has not set allowEmbedded=\"TRUE\"");
        return E_SECURITY_EXCEPTION;
    }
    return NOERROR;
}

//=====================================================================
// VirtualActivityContainer
//=====================================================================
VirtualActivityContainer::VirtualActivityContainer()
    : mDrawn(FALSE)
{}

ECode VirtualActivityContainer::constructor(
    /* [in] */ IActivityRecord* parent,
    /* [in] */ IActivityContainerCallback* callback,
    /* [in] */ IDisplayListener* owner)
{
    ActivityStackSupervisor* ass = (ActivityStackSupervisor*)owner;
    FAIL_RETURN(ActivityContainer::constructor(ass->GetNextStackId(), owner))
    mParentActivity = parent;
    mCallback = callback;
    mContainerState = CONTAINER_STATE_NO_SURFACE;
    StringBuilder sb("VirtualActivityContainer{");
    sb += "mStackId";
    sb += ", parent=";
    sb += Object::ToString(mParentActivity);
    sb += "}";
    mIdString = sb.ToString();
    return NOERROR;
}

ECode VirtualActivityContainer::SetSurface(
    /* [in] */ ISurface* surface,
    /* [in] */ Int32 width,
    /* [in] */ Int32 height,
    /* [in] */ Int32 density)
{
    ActivityContainer::SetSurface(surface, width, height, density);

    {
        AutoLock lock(mOwner->mService);
        Int64 origId = Binder::ClearCallingIdentity();
        //try {
            SetSurfaceLocked(surface, width, height, density);
        //} finally {
            Binder::RestoreCallingIdentity(origId);
        //}
    }
    return NOERROR;
}

Boolean VirtualActivityContainer::IsAttachedLocked()
{
    return mSurface != NULL && ActivityContainer::IsAttachedLocked();
}

ECode VirtualActivityContainer::SetDrawn()
{
    {
        AutoLock lock(mOwner->mService);
        mDrawn = TRUE;
        SetSurfaceIfReadyLocked();
    }
    return NOERROR;
}

Boolean VirtualActivityContainer::IsEligibleForNewTasks()
{
    return FALSE;
}

ECode VirtualActivityContainer::OnTaskListEmptyLocked()
{
    mOwner->mHandler->RemoveMessages(
        ActivityStackSupervisor::CONTAINER_TASK_LIST_EMPTY_TIMEOUT, TO_IINTERFACE(this));
    DetachLocked();
    mOwner->DeleteActivityContainer(this);
    AutoPtr<IMessage> msg;
    mOwner->mHandler->ObtainMessage(
        ActivityStackSupervisor::CONTAINER_CALLBACK_TASK_LIST_EMPTY,
        TO_IINTERFACE(this), (IMessage**)&msg);
    msg->SendToTarget();
    return NOERROR;
}

void VirtualActivityContainer::SetSurfaceLocked(
    /* [in] */ ISurface* surface,
    /* [in] */ Int32 width,
    /* [in] */ Int32 height,
    /* [in] */ Int32 density)
{
    if (mContainerState == CONTAINER_STATE_FINISHING) {
        return;
    }
    AutoPtr<VirtualActivityDisplay> virtualActivityDisplay =
            (VirtualActivityDisplay*)(mActivityDisplay.Get());
    if (virtualActivityDisplay == NULL) {
        virtualActivityDisplay =
                new VirtualActivityDisplay(width, height, density, mOwner);
        mActivityDisplay = virtualActivityDisplay;
        mOwner->mActivityDisplays->Put(virtualActivityDisplay->mDisplayId, TO_IINTERFACE(virtualActivityDisplay));
        AttachToDisplayLocked(virtualActivityDisplay);
    }

    if (mSurface != NULL) {
        mSurface->ReleaseResources();
    }

    mSurface = surface;
    if (surface != NULL) {
        mStack->ResumeTopActivityLocked(NULL);
    } else {
        mContainerState = CONTAINER_STATE_NO_SURFACE;
        ((VirtualActivityDisplay*)mActivityDisplay.Get())->SetSurface(NULL);
        if (mStack->mPausingActivity == NULL && mStack->mResumedActivity != NULL) {
            mStack->StartPausingLocked(FALSE, TRUE, FALSE, FALSE);
        }
    }

    SetSurfaceIfReadyLocked();

    if (CActivityManagerService::DEBUG_STACK) {
        Slogger::D(TAG, "setSurface: %s to display %s", TO_CSTR(this), TO_CSTR(virtualActivityDisplay));
    }
}

void VirtualActivityContainer::SetSurfaceIfReadyLocked()
{
    if (CActivityManagerService::DEBUG_STACK) {
        Slogger::V(TAG, "setSurfaceIfReadyLocked: mDrawn=%d mContainerState=%d mSurface=%s",
            mDrawn, mContainerState, TO_CSTR(mSurface));
    }
    if (mDrawn && mSurface != NULL && mContainerState == CONTAINER_STATE_NO_SURFACE) {
        ((VirtualActivityDisplay*)mActivityDisplay.Get())->SetSurface(mSurface);
        mContainerState = CONTAINER_STATE_HAS_SURFACE;
    }
}

} // namespace Am
} // namespace Server
} // namespace Droid
} // namespace Elastos
