
#include <Elastos.Droid.Provider.h>
#include "elastos/droid/server/power/PerformanceManager.h"
#include "elastos/droid/server/power/PowerManagerService.h"
#include <elastos/droid/os/Binder.h>
#include "elastos/droid/text/TextUtils.h"
#include "elastos/droid/R.h"
#include <elastos/core/StringUtils.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Os::ISystemProperties;
using Elastos::Droid::Os::CSystemProperties;
using Elastos::Droid::Os::Binder;
using Elastos::Droid::Provider::ISettingsSecure;
using Elastos::Droid::Provider::CSettingsSecure;
using Elastos::Droid::Text::TextUtils;
using Elastos::Core::StringUtils;
using Elastos::Utility::Logging::Logger;
using Elastos::Utility::Regex::IPatternHelper;
using Elastos::Utility::Regex::CPatternHelper;
using Elastos::Utility::Regex::IMatcher;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Power {

const String PerformanceManager::TAG("PerformanceManager");

PerformanceManager::PerformanceManager(
    /* [in] */ IContext* context)
    : mContext(context)
    , mProfileSetByUser(FALSE)
    , mProfilesEnabled(FALSE)
    , mUsePowerHAL(FALSE)
{
    AutoPtr<IResources> res;
    context->GetResources((IResources**)&res);
    AutoPtr<ArrayOf<String> > activities;
    res->GetStringArray(Elastos::Droid::R::array::config_auto_perf_activities, (ArrayOf<String>**)&activities);
    if (activities != NULL && activities->GetLength() > 0) {
        Int32 len = activities->GetLength();
        mPatterns = ArrayOf<IPattern*>::Alloc(len);
        mProfiles = ArrayOf<String>::Alloc(len);
        for (Int32 i = 0; i < len; i++) {
            AutoPtr<ArrayOf<String> > info;
            StringUtils::Split((*activities)[i], String(","), (ArrayOf<String>**)&info);
            if (info->GetLength() == 2) {
                AutoPtr<IPatternHelper> helper;
                CPatternHelper::AcquireSingleton((IPatternHelper**)&helper);
                helper->Compile((*info)[0], (IPattern**)&((*mPatterns)[i]));
                (*mProfiles)[i] = (*info)[1];
            }
        }
    }

    res->GetString(Elastos::Droid::R::string::config_perf_profile_default_entry, &mPerfProfileDefault);
    res->GetString(Elastos::Droid::R::string::config_perf_profile_prop, &mPerfProfileProperty);
    mProfilesEnabled = !TextUtils::IsEmpty(mPerfProfileDefault) &&
            !TextUtils::IsEmpty(mPerfProfileProperty);

    if (mProfilesEnabled) {
        // Magic value- this will use a power hint instead of relying on
        // a property trigger via init. This will become the default
        // after devices have been updated.
        mUsePowerHAL = mPerfProfileProperty.Equals("powerhal");
    }
}

void PerformanceManager::Reset()
{
    if (mProfilesEnabled) {
        // Reset the stored value to the default profile when we are initialized.
        // We currently do not want to support booting in perf or powersave modes.
        SetPowerProfileLocked(mPerfProfileDefault);
    }
}

void PerformanceManager::SelectProfile(
    /* [in] */ const String& profile)
{
    mCurrentProfile = profile;

    if (mUsePowerHAL) {
        PowerManagerService::NativeSetPowerProfile(StringUtils::ParseInt32(profile));
    }
    else {
        AutoPtr<ISystemProperties> sysProp;
        CSystemProperties::AcquireSingleton((ISystemProperties**)&sysProp);
        sysProp->Set(mPerfProfileProperty, profile);
    }
}

Boolean PerformanceManager::HasAppProfiles()
{
    AutoPtr<IContentResolver> cr;
    mContext->GetContentResolver((IContentResolver**)&cr);
    AutoPtr<ISettingsSecure> secure;
    CSettingsSecure::AcquireSingleton((ISettingsSecure**)&secure);
    Int32 value;
    secure->GetInt32(cr, ISettingsSecure::APP_PERFORMANCE_PROFILES_ENABLED, 1, &value);
    return mProfilesEnabled && mPatterns != NULL && (value == 1);
}

Boolean PerformanceManager::SetPowerProfile(
    /* [in] */ const String& profile)
{
    if (!mProfilesEnabled) {
        Logger::E(TAG, "Power profiles not enabled on this device");
        return FALSE;
    }
    if (profile.IsNull() || profile.Equals(GetPowerProfile())) {
        return FALSE;
    }
    synchronized (this) {
        mProfileSetByUser = !profile.Equals(mPerfProfileDefault);
        SetPowerProfileLocked(profile);
    }
    return TRUE;
}

void PerformanceManager::SetPowerProfileLocked(
    /* [in] */ const String& profile)
{
    Int64 token = Binder::ClearCallingIdentity();
    SelectProfile(profile);
    AutoPtr<IContentResolver> cr;
    mContext->GetContentResolver((IContentResolver**)&cr);
    AutoPtr<ISettingsSecure> secure;
    CSettingsSecure::AcquireSingleton((ISettingsSecure**)&secure);
    Boolean result;
    secure->PutString(cr, ISettingsSecure::PERFORMANCE_PROFILE, profile, &result);
    Binder::RestoreCallingIdentity(token);
}

String PerformanceManager::GetPowerProfile()
{
    return mCurrentProfile;
}

String PerformanceManager::GetProfileForActivity(
    /* [in] */ const String& componentName)
{
    if (!componentName.IsNull()) {
        for (Int32 i = 0; i < mPatterns->GetLength(); i++) {
            AutoPtr<IMatcher> matcher;
            (*mPatterns)[i]->Matcher(componentName, (IMatcher**)&matcher);
            Boolean matches;
            if (matcher->Matches(&matches), matches) {
                return (*mProfiles)[i];
            }
        }
    }
    return mPerfProfileDefault;
}

void PerformanceManager::ActivityResumed(
    /* [in] */ const String& componentName)
{
    if (!HasAppProfiles()) {
        return;
    }

    synchronized (this) {
        // Don't mess with it if the user has manually set a profile
        if (mProfileSetByUser) {
            return;
        }

        String forApp = GetProfileForActivity(componentName);
        if (forApp.Equals(mCurrentProfile)) {
            return;
        }

        SetPowerProfileLocked(forApp);
    }
}

} // Power
} // Server
} // Droid
} // Elastos
