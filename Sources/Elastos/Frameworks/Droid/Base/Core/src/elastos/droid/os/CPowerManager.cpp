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

#include "Elastos.Droid.Content.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/os/CPowerManager.h"
#include "elastos/droid/os/SystemProperties.h"
#include "elastos/droid/text/TextUtils.h"
#include "elastos/droid/R.h"
#include <elastos/utility/logging/Slogger.h>

using Elastos::Droid::Content::EIID_IContext;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Content::IComponentName;
using Elastos::Droid::Text::TextUtils;
using Elastos::Utility::Logging::Slogger;

namespace Elastos {
namespace Droid {
namespace Os {

const String CPowerManager::TAG("PowerManager");

CAR_INTERFACE_IMPL(CPowerManager, Object, IPowerManager)

CAR_OBJECT_IMPL(CPowerManager)

ECode CPowerManager::GetMinimumScreenBrightnessSetting(
    /* [out] */ Int32* screenBrightness)
{
    VALIDATE_NOT_NULL(screenBrightness)
    *screenBrightness = 0;

    AutoPtr<IContext> context = GetContext();
    if (context == NULL) {
        return NOERROR;
    }
    AutoPtr<IResources> res;
    FAIL_RETURN(context->GetResources((IResources**)&res));
    return res->GetInteger(R::integer::config_screenBrightnessSettingMinimum, screenBrightness);
}

ECode CPowerManager::GetMaximumScreenBrightnessSetting(
    /* [out] */ Int32* screenBrightness)
{
    VALIDATE_NOT_NULL(screenBrightness)
    *screenBrightness = 0;

    AutoPtr<IContext> context = GetContext();
    if (context == NULL) {
        return NOERROR;
    }
    AutoPtr<IResources> res;
    FAIL_RETURN(context->GetResources((IResources**)&res));
    return res->GetInteger(R::integer::config_screenBrightnessSettingMaximum, screenBrightness);
}

ECode CPowerManager::GetDefaultScreenBrightnessSetting(
    /* [out] */ Int32* screenBrightness)
{
    VALIDATE_NOT_NULL(screenBrightness)
    *screenBrightness = 0;

    AutoPtr<IContext> context = GetContext();
    if (context == NULL) {
        return NOERROR;
    }
    AutoPtr<IResources> res;
    FAIL_RETURN(context->GetResources((IResources**)&res));
    return res->GetInteger(R::integer::config_screenBrightnessSettingDefault, screenBrightness);
}

Boolean CPowerManager::UseTwilightAdjustmentFeature()
{
    Boolean bval;
    SystemProperties::GetBoolean(String("persist.power.usetwilightadj"), FALSE, &bval);
    return bval;
}

ECode CPowerManager::NewWakeLock(
    /* [in] */ Int32 levelAndFlags,
    /* [in] */ const String& tag,
    /* [out] */ IPowerManagerWakeLock** wakeLock)
{
    VALIDATE_NOT_NULL(wakeLock)
    *wakeLock = NULL;

    AutoPtr<IContext> context = GetContext();
    if (context == NULL) {
        return NOERROR;
    }
    FAIL_RETURN(ValidateWakeLockParameters(levelAndFlags, tag));
    String name;
    context->GetOpPackageName(&name);
    return CPowerManagerWakeLock::New(levelAndFlags, tag, name, this, wakeLock);
}

ECode CPowerManager::ValidateWakeLockParameters(
    /* [in] */ Int32 levelAndFlags,
    /* [in] */ const String& tag)
{
    switch (levelAndFlags & WAKE_LOCK_LEVEL_MASK) {
        case PARTIAL_WAKE_LOCK:
        case SCREEN_DIM_WAKE_LOCK:
        case SCREEN_BRIGHT_WAKE_LOCK:
        case FULL_WAKE_LOCK:
        case PROXIMITY_SCREEN_OFF_WAKE_LOCK:
        case DOZE_WAKE_LOCK:
            break;
        default:
            Slogger::E(TAG, "Must specify a valid wake lock level.");
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
            // throw new IllegalArgumentException("Must specify a valid wake lock level.");
    }
    if (tag.IsNull()) {
        Slogger::E(TAG, "The tag must not be null.");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
        // throw new IllegalArgumentException("The tag must not be null.");
    }
    return NOERROR;
}

ECode CPowerManager::UserActivity(
    /* [in] */ Int64 when,
    /* [in] */ Boolean noChangeLights)
{
    return UserActivity(when, USER_ACTIVITY_EVENT_OTHER,
            noChangeLights ? USER_ACTIVITY_FLAG_NO_CHANGE_LIGHTS : 0);
}

ECode CPowerManager::UserActivity(
    /* [in] */ Int64 when,
    /* [in] */ Int32 event,
    /* [in] */ Int32 flags)
{
    // try {
    return mService->UserActivity(when, event, flags);
    // } catch (RemoteException e) {
    // }
}

ECode CPowerManager::GoToSleep(
    /* [in] */ Int64 eventTime)
{
    return GoToSleep(eventTime, GO_TO_SLEEP_REASON_APPLICATION, 0);
}

ECode CPowerManager::GoToSleep(
    /* [in] */ Int64 eventTime,
    /* [in] */ Int32 reason,
    /* [in] */ Int32 flags)
{
    // try {
    return mService->GoToSleep(eventTime, reason, flags);
    // } catch (RemoteException e) {
    // }
}

ECode CPowerManager::WakeUp(
    /* [in] */ Int64 eventTime)
{
    // try {
    return mService->WakeUp(eventTime);
    // } catch (RemoteException e) {
    // }
}

ECode CPowerManager::Nap(
    /* [in] */ Int64 time)
{
    // try {
    return mService->Nap(time);
    // } catch (RemoteException e) {
    // }
}

ECode CPowerManager::SetBacklightBrightness(
    /* [in] */ Int32 brightness)
{
    // try {
    return mService->SetTemporaryScreenBrightnessSettingOverride(brightness);
    // } catch (RemoteException e) {
    // }
}

ECode CPowerManager::IsWakeLockLevelSupported(
    /* [in] */ Int32 level,
    /* [out] */ Boolean* isSupported)
{
    VALIDATE_NOT_NULL(isSupported);
    // try {
    ECode ec = mService->IsWakeLockLevelSupported(level, isSupported);
    if (ec == (ECode)E_REMOTE_EXCEPTION) {
        *isSupported = FALSE;
        return NOERROR;
    }
    return NOERROR;
}

ECode CPowerManager::IsScreenOn(
    /* [out] */ Boolean* isScreenOn)
{
    return IsInteractive(isScreenOn);
}

ECode CPowerManager::IsInteractive(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    // try {
    ECode ec = mService->IsInteractive(result);
    if (ec == (ECode)E_REMOTE_EXCEPTION) {
        *result = FALSE;
        return NOERROR;
    }
    return NOERROR;
}

ECode CPowerManager::Reboot(
    /* [in] */ const String& reason)
{
    // try {
    return mService->Reboot(FALSE, reason, TRUE);
    // } catch (RemoteException e) {
    // }
}

ECode CPowerManager::IsPowerSaveMode(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    ECode ec = mService->IsPowerSaveMode(result);
    if (ec == (ECode)E_REMOTE_EXCEPTION) {
        *result = FALSE;
        return NOERROR;
    }
    return NOERROR;
}

ECode CPowerManager::SetPowerSaveMode(
    /* [in] */ Boolean mode,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    ECode ec = mService->SetPowerSaveMode(mode, result);
    if (ec == (ECode)E_REMOTE_EXCEPTION) {
        *result = FALSE;
        return NOERROR;
    }
    return NOERROR;
}

ECode CPowerManager::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IIPowerManager* service,
    /* [in] */ IHandler* handler)
{
    IWeakReferenceSource::Probe(context)->GetWeakReference((IWeakReference**)&mWeakContext);
    mService = service;
    mHandler = handler;
    return NOERROR;
}

AutoPtr<IContext> CPowerManager::GetContext()
{
    AutoPtr<IContext> ctx;
    mWeakContext->Resolve(EIID_IContext, (IInterface**)&ctx);
    return ctx;
}

ECode CPowerManager::CpuBoost(
    /* [in] */ Int32 duration)
{
    // try {
    if (mService != NULL) {
        return mService->CpuBoost(duration);
    }
    // } catch (RemoteException e) {
    // }
    return NOERROR;
}

ECode CPowerManager::HasPowerProfiles(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;
    AutoPtr<IContext> context = GetContext();
    if (context == NULL) {
        return NOERROR;
    }

    String defaultProfile, profile;
    GetDefaultPowerProfile(&defaultProfile);
    AutoPtr<IResources> res;
    context->GetResources((IResources**)&res);
    res->GetString(R::string::config_perf_profile_prop, &profile);
    *result = !TextUtils::IsEmpty(defaultProfile) &&
           !TextUtils::IsEmpty(profile);
    return NOERROR;
}

ECode CPowerManager::GetDefaultPowerProfile(
    /* [out] */ String* profile)
{
    VALIDATE_NOT_NULL(profile)
    *profile = String(NULL);

    AutoPtr<IContext> context = GetContext();
    if (context == NULL) {
        return NOERROR;
    }
    AutoPtr<IResources> res;
    context->GetResources((IResources**)&res);
    return res->GetString(R::string::config_perf_profile_default_entry, profile);
}

ECode CPowerManager::SetPowerProfile(
    /* [in] */ const String&  profile,
    /* [out] */ Boolean* result)
{
    *result = FALSE;
    Boolean hasProfiles;
    if (HasPowerProfiles(&hasProfiles), !hasProfiles) {
        SLOGGERE("CPowerManager", "Power profiles not enabled on this system!")
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    // try {
    if (mService != NULL) {
        if (FAILED(mService->SetPowerProfile(profile, result))) {
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
        }
    }
    // } catch (RemoteException e) {
        // throw new IllegalArgumentException(e);
    // }
    return NOERROR;
}

ECode CPowerManager::GetPowerProfile(
    /* [out] */ String* result)
{
    *result = NULL;
    Boolean hasProfiles;
    if (HasPowerProfiles(&hasProfiles), hasProfiles) {
        // try {
        if (mService != NULL) {
            return mService->GetPowerProfile(result);
        }
        // } catch (RemoteException e) {
            // nothing
        // }
    }
    return NOERROR;
}

ECode CPowerManager::ActivityResumed(
    /* [in] */ IIntent* intent)
{
    Boolean hasProfiles;
    if (HasPowerProfiles(&hasProfiles), hasProfiles) {
        // try {
        if (intent != NULL && mService != NULL) {
            AutoPtr<IComponentName> cn;
            intent->GetComponent((IComponentName**)&cn);
            if (cn != NULL) {
                String cnStr;
                cn->FlattenToString(&cnStr);
                return mService->ActivityResumed(cnStr);
            }
        }
        // } catch (RemoteException e) {
            // nothing
        // }
    }
    return NOERROR;
}

ECode CPowerManager::SetKeyboardVisibility(
    /* [in] */ Boolean visible)
{
    // try {
    if (mService != NULL) {
        return mService->SetKeyboardVisibility(visible);
    }
    // } catch (RemoteException e) {
    // }
    return NOERROR;
}

ECode CPowerManager::SetKeyboardLight(
    /* [in] */ Boolean on,
    /* [in] */ Int32 key)
{
    // try {
    return mService->SetKeyboardLight(on, key);
    // } catch (RemoteException e) {
    // }
}

ECode CPowerManager::GetDefaultButtonBrightness(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = 0;

    AutoPtr<IContext> context = GetContext();
    if (context == NULL) {
        return NOERROR;
    }
    AutoPtr<IResources> res;
    context->GetResources((IResources**)&res);
    return res->GetInteger(R::integer::config_buttonBrightnessSettingDefault, result);
}

ECode CPowerManager::GetDefaultKeyboardBrightness(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = 0;

    AutoPtr<IContext> context = GetContext();
    if (context == NULL) {
        return NOERROR;
    }
    AutoPtr<IResources> res;
    context->GetResources((IResources**)&res);
    return res->GetInteger(R::integer::config_keyboardBrightnessSettingDefault, result);
}

ECode CPowerManager::WakeUpWithProximityCheck(
    /* [in] */ Int64 time)
{
    // try {
    return mService->WakeUpWithProximityCheck(time);
    // } catch (RemoteException e) {
    // }
}

} // namespace Os
} // namespace Droid
} // namespace Elastos
