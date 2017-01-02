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

#include "elastos/droid/systemui/statusbar/phone/SecureCameraLaunchManager.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Os.h"
#include "Elastos.Droid.Provider.h"
#include <elastos/droid/os/AsyncTask.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::Pm::IActivityInfo;
using Elastos::Droid::Content::Pm::IPackageManager;
using Elastos::Droid::Content::Pm::IPackageItemInfo;
using Elastos::Droid::Internal::Widget::CLockPatternUtils;
using Elastos::Droid::Os::AsyncTask;
using Elastos::Droid::Os::CHandler;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Provider::IMediaStore;
using Elastos::Core::CBoolean;
using Elastos::Core::CString;
using Elastos::Core::IBoolean;
using Elastos::Core::ICharSequence;
using Elastos::Utility::CHashMap;
using Elastos::Utility::ICollection;
using Elastos::Utility::IIterator;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {
namespace Phone {

const Boolean SecureCameraLaunchManager::DEBUG = FALSE;
const String SecureCameraLaunchManager::TAG("SecureCameraLaunchManager");
const String SecureCameraLaunchManager::CLOSE_CAMERA_ACTION_NAME("com.android.systemui.statusbar.phone.CLOSE_CAMERA");
const String SecureCameraLaunchManager::META_DATA_WILL_WAIT_FOR_CAMERA_AVAILABLE("com.android.systemui.statusbar.phone.will_wait_for_camera_available");
const Int32 SecureCameraLaunchManager::CAMERA_AVAILABILITY_TIMEOUT_MS = 1000;

SecureCameraLaunchManager::CameraAvailabilityCallback::CameraAvailabilityCallback(
    /* [in] */ SecureCameraLaunchManager* host)
    : mHost(host)
{}

ECode SecureCameraLaunchManager::CameraAvailabilityCallback::OnCameraUnavailable(
    /* [in] */ const String& cameraId)
{
    if (DEBUG) Logger::D(TAG, "onCameraUnavailble(%s)", cameraId.string());
    AutoPtr<ICharSequence> key;
    CString::New(cameraId, (ICharSequence**)&key);
    AutoPtr<IBoolean> value;
    CBoolean::New(FALSE, (IBoolean**)&value);
    mHost->mCameraAvailabilityMap->Put(key, value);
    return NOERROR;
}

ECode SecureCameraLaunchManager::CameraAvailabilityCallback::OnCameraAvailable(
    /* [in] */ const String& cameraId)
{
    if (DEBUG) Logger::D(TAG, "onCameraAvailable(%s)", cameraId.string());

    AutoPtr<ICharSequence> key;
    CString::New(cameraId, (ICharSequence**)&key);
    AutoPtr<IBoolean> value;
    CBoolean::New(TRUE, (IBoolean**)&value);
    mHost->mCameraAvailabilityMap->Put(key, value);

    // If we were waiting for the camera hardware to become available to launch the
    // secure camera, we can launch it now if all cameras are available.  If one or more
    // cameras are still not available, we will get this callback again for those
    // cameras.
    if (mHost->mWaitingToLaunchSecureCamera && mHost->AreAllCamerasAvailable()) {
        mHost->mKeyguardBottomArea->LaunchCamera();
        mHost->mWaitingToLaunchSecureCamera = FALSE;

        // We no longer need to launch the camera after the timeout hits.
        mHost->mHandler->RemoveCallbacks(mHost->mLaunchCameraRunnable);
    }
    return NOERROR;
}

SecureCameraLaunchManager::Runnable1::Runnable1(
    /* [in] */ SecureCameraLaunchManager* host)
    : mHost(host)
{}

ECode SecureCameraLaunchManager::Runnable1::Run()
{
    if (mHost->mWaitingToLaunchSecureCamera) {
        Logger::W(TAG, "Timeout waiting for camera availability");
        mHost->mKeyguardBottomArea->LaunchCamera();
        mHost->mWaitingToLaunchSecureCamera = FALSE;
    }
    return NOERROR;
}

SecureCameraLaunchManager::Runnable2::Runnable2(
    /* [in] */ SecureCameraLaunchManager* host)
    : mHost(host)
{}

ECode SecureCameraLaunchManager::Runnable2::Run()
{
    AutoPtr<IIntent> intent;
    CIntent::New((IIntent**)&intent);
    intent->SetAction(CLOSE_CAMERA_ACTION_NAME);
    mHost->mContext->SendBroadcast(intent);
    return NOERROR;
}

SecureCameraLaunchManager::SecureCameraLaunchManager(
    /* [in] */ IContext* context,
    /* [in] */ IKeyguardBottomAreaView* keyguardBottomArea)
    : mWaitingToLaunchSecureCamera(FALSE)
{
    mContext = context;
    CHandler::New((IHandler**)&mHandler);
    CLockPatternUtils::New(context, (ILockPatternUtils**)&mLockPatternUtils);
    mKeyguardBottomArea = keyguardBottomArea;

    AutoPtr<IInterface> obj;
    context->GetSystemService(IContext::CAMERA_SERVICE, (IInterface**)&obj);
    mCameraManager = ICameraManager::Probe(obj);
    mCameraAvailabilityCallback = new CameraAvailabilityCallback(this);

    // An onCameraAvailable() or onCameraUnavailable() callback will be received for each camera
    // when the availability callback is registered, thus initializing the map.
    //
    // Keeping track of the state of all cameras using the onCameraAvailable() and
    // onCameraUnavailable() callbacks can get messy when dealing with hot-pluggable cameras.
    // However, we have a timeout in place such that we will never hang waiting for cameras.
    CHashMap::New((IMap**)&mCameraAvailabilityMap);

    mWaitingToLaunchSecureCamera = FALSE;
    mLaunchCameraRunnable  = new Runnable1(this);
}

void SecureCameraLaunchManager::Create()
{
    mCameraManager->RegisterAvailabilityCallback(mCameraAvailabilityCallback, mHandler);
}

void SecureCameraLaunchManager::Destroy()
{
    mCameraManager->UnregisterAvailabilityCallback(mCameraAvailabilityCallback);
}

void SecureCameraLaunchManager::OnSwipingStarted()
{
    if (DEBUG) Logger::D(TAG, "onSwipingStarted");
    AutoPtr<Runnable2> run = new Runnable2(this);
    AsyncTask::Execute(run);
}

void SecureCameraLaunchManager::StartSecureCameraLaunch()
{
    if (DEBUG) Logger::D(TAG, "startSecureCameraLunch");
    if (AreAllCamerasAvailable() || TargetWillWaitForCameraAvailable()) {
        mKeyguardBottomArea->LaunchCamera();
    }
    else {
        mWaitingToLaunchSecureCamera = TRUE;
        Boolean tmp = FALSE;
        mHandler->PostDelayed(mLaunchCameraRunnable, CAMERA_AVAILABILITY_TIMEOUT_MS, &tmp);
    }
}

Boolean SecureCameraLaunchManager::AreAllCamerasAvailable()
{
    AutoPtr<ICollection> values;
    mCameraAvailabilityMap->GetValues((ICollection**)&values);
    AutoPtr<IIterator> ator;
    values->GetIterator((IIterator**)&ator);
    Boolean hasNext;
    while (ator->HasNext(&hasNext), hasNext) {
        AutoPtr<IInterface> obj;
        ator->GetNext((IInterface**)&obj);
        Boolean cameraAvailable = FALSE;
        IBoolean::Probe(obj)->GetValue(&cameraAvailable);
        if (!cameraAvailable) {
            return FALSE;
        }
    }

    return TRUE;
}

Boolean SecureCameraLaunchManager::TargetWillWaitForCameraAvailable()
{
    // Create intent that would launch the secure camera.
    AutoPtr<IIntent> intent;
    CIntent::New(IMediaStore::INTENT_ACTION_STILL_IMAGE_CAMERA_SECURE, (IIntent**)&intent);
    intent->AddFlags(IIntent::FLAG_ACTIVITY_EXCLUDE_FROM_RECENTS);
    AutoPtr<IPackageManager> packageManager;
    mContext->GetPackageManager((IPackageManager**)&packageManager);

    // Get the list of applications that can handle the intent.
    AutoPtr<IList> appList;  /*<ResolveInfo*/
    Int32 user = 0;
    mLockPatternUtils->GetCurrentUser(&user);
    packageManager->QueryIntentActivitiesAsUser(
            intent, IPackageManager::MATCH_DEFAULT_ONLY, user, (IList**)&appList);

    Int32 size = 0;
    appList->GetSize(&size);
    if (size == 0) {
        if (DEBUG) Logger::D(TAG, "No targets found for secure camera intent");
        return FALSE;
    }

    // Get the application that the intent resolves to.
    AutoPtr<IResolveInfo> resolved;
    packageManager->ResolveActivityAsUser(intent,
            IPackageManager::MATCH_DEFAULT_ONLY | IPackageManager::GET_META_DATA,
            user, (IResolveInfo**)&resolved);

    AutoPtr<IActivityInfo> ai;
    if (resolved == NULL || (resolved->GetActivityInfo((IActivityInfo**)&ai), ai.Get()) == NULL) {
        return FALSE;
    }

    // If we would need to launch the resolver activity, then we can't assume that the target
    // is one that would wait for the camera.
    if (WouldLaunchResolverActivity(resolved, appList)) {
        if (DEBUG) Logger::D(TAG, "Secure camera intent would launch resolver");
        return FALSE;
    }

    resolved->GetActivityInfo((IActivityInfo**)&ai);
    // If the target doesn't have meta-data we must assume it won't wait for the camera.
    AutoPtr<IBundle> metaData;
    Boolean empty = FALSE;
    if ((IPackageItemInfo::Probe(ai)->GetMetaData((IBundle**)&metaData), metaData.Get()) == NULL
        || (metaData->IsEmpty(&empty), empty)) {
        if (DEBUG) Logger::D(TAG, "No meta-data found for secure camera application");
        return FALSE;
    }

    // Check the secure camera app meta-data to see if it indicates that it will wait for the
    // camera to become available.
    Boolean willWaitForCameraAvailability = FALSE;
    metaData->GetBoolean(META_DATA_WILL_WAIT_FOR_CAMERA_AVAILABLE, &willWaitForCameraAvailability);

    if (DEBUG) Logger::D(TAG, "Target will wait for camera: %d", willWaitForCameraAvailability);

    return willWaitForCameraAvailability;
}

Boolean SecureCameraLaunchManager::WouldLaunchResolverActivity(
    /* [in] */ IResolveInfo* resolved,
    /* [in] */ IList/*<ResolveInfo>*/* appList)
{
    // If the list contains the resolved activity, then it can't be the ResolverActivity itself.
    Int32 size = 0;
    appList->GetSize(&size);
    for (Int32 i = 0; i < size; i++) {
        AutoPtr<IInterface> obj;
        appList->Get(i, (IInterface**)&obj);
        AutoPtr<IResolveInfo> tmp = IResolveInfo::Probe(obj);

        AutoPtr<IActivityInfo> activityInfo;
        tmp->GetActivityInfo((IActivityInfo**)&activityInfo);
        AutoPtr<IActivityInfo> activityInfo2;
        resolved->GetActivityInfo((IActivityInfo**)&activityInfo2);
        String name1, name2;
        IPackageItemInfo::Probe(activityInfo)->GetName(&name1);
        IPackageItemInfo::Probe(activityInfo2)->GetName(&name2);

        String pkn1, pkn2;
        IPackageItemInfo::Probe(activityInfo)->GetPackageName(&pkn1);
        IPackageItemInfo::Probe(activityInfo2)->GetPackageName(&pkn2);
        if (name1.Equals(name2) && pkn1.Equals(pkn2)) {
            return FALSE;
        }
    }
    return TRUE;
}

} // namespace Phone
} // namespace StatusBar
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos
