
#include "elastos/droid/app/ProfileManager.h"
#include "elastos/droid/os/ServiceManager.h"
#include "elastos/droid/os/CParcelUuid.h"
#include "elastos/droid/provider/CSettingsSystem.h"
#include <elastos/utility/logging/Logger.h>
#include <Elastos.Droid.Utility.h>

using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Os::IParcelUuid;
using Elastos::Droid::Os::CParcelUuid;
using Elastos::Droid::Os::IServiceManager;
using Elastos::Droid::Os::ServiceManager;
using Elastos::Droid::Provider::ISettingsSystem;
using Elastos::Droid::Provider::CSettingsSystem;
using Elastos::Utility::IUUIDHelper;
using Elastos::Utility::CUUIDHelper;

namespace Elastos {
namespace Droid {
namespace App {

AutoPtr<IUUID> InitNoProfile()
{
    AutoPtr<IUUIDHelper> helper;
    CUUIDHelper::AcquireSingleton((IUUIDHelper**)&helper);
    AutoPtr<IUUID> uuid;
    helper->FromString(String("00000000-0000-0000-0000-000000000000"), (IUUID**)&uuid);
    return uuid;
}

AutoPtr<IUUID> ProfileManager::NO_PROFILE = InitNoProfile();

AutoPtr<IIProfileManager> ProfileManager::sService;

const String ProfileManager::TAG("ProfileManager");

const String ProfileManager::SYSTEM_PROFILES_ENABLED("system_profiles_enabled");

AutoPtr<IProfile> ProfileManager::sEmptyProfile;

CAR_INTERFACE_IMPL(ProfileManager, Object, IProfileManager)

AutoPtr<IIProfileManager> ProfileManager::GetService()
{
    if (sService != NULL) {
        return sService;
    }
    AutoPtr<IInterface> b = ServiceManager::GetService(IContext::PROFILE_SERVICE);
    sService = IIProfileManager::Probe(b);
    return sService;
}

ECode ProfileManager::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)
    *str = String("ProfileManager");
    return NOERROR;
}

ECode ProfileManager::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IHandler* handler)
{
    mContext = context;
    // CProfile::New(String("EmptyProfile"), (IProfile**)&sEmptyProfile);
    return NOERROR;
}

ECode ProfileManager::SetActiveProfile(
    /* [in] */ const String& profileName)
{
    AutoPtr<ISettingsSystem> settingsSys;
    CSettingsSystem::AcquireSingleton((ISettingsSystem**)&settingsSys);
    AutoPtr<IContentResolver> resolver;
    mContext->GetContentResolver((IContentResolver**)&resolver);
    Int32 value;
    settingsSys->GetInt32(resolver, SYSTEM_PROFILES_ENABLED, 1, &value);
    if (value == 1) {
        // Profiles are enabled, return active profile
        // try {
        Boolean result;
        ECode ec = GetService()->SetActiveProfileByName(profileName, &result);
        if (ec == (ECode)E_REMOTE_EXCEPTION) {
            return ec;
        }
        // } catch (RemoteException e) {
        //     Log.e(TAG, e.getLocalizedMessage(), e);
        // }
    }
    return NOERROR;
}

ECode ProfileManager::SetActiveProfile(
    /* [in] */ IUUID* profileUuid)
{
    AutoPtr<ISettingsSystem> settingsSys;
    CSettingsSystem::AcquireSingleton((ISettingsSystem**)&settingsSys);
    AutoPtr<IContentResolver> resolver;
    mContext->GetContentResolver((IContentResolver**)&resolver);
    Int32 value;
    settingsSys->GetInt32(resolver, SYSTEM_PROFILES_ENABLED, 1, &value);
    if (value == 1) {
        // Profiles are enabled, return active profile
        // try {
        Boolean result;
        AutoPtr<IParcelUuid> uuid;
        CParcelUuid::New(profileUuid, (IParcelUuid**)&uuid);
        ECode ec = GetService()->SetActiveProfile(uuid, &result);
        if (ec == (ECode)E_REMOTE_EXCEPTION) {
            return ec;
        }
        // } catch (RemoteException e) {
        //     Log.e(TAG, e.getLocalizedMessage(), e);
        // }
    }
    return NOERROR;
}

ECode ProfileManager::GetActiveProfile(
    /* [out] */ IProfile** profile)
{
    VALIDATE_NOT_NULL(profile)

    AutoPtr<ISettingsSystem> settingsSys;
    CSettingsSystem::AcquireSingleton((ISettingsSystem**)&settingsSys);
    AutoPtr<IContentResolver> resolver;
    mContext->GetContentResolver((IContentResolver**)&resolver);
    Int32 value;
    settingsSys->GetInt32(resolver, SYSTEM_PROFILES_ENABLED, 1, &value);
    if (value == 1) {
        // Profiles are enabled, return active profile
        // try {
        ECode ec = GetService()->GetActiveProfile(profile);
        if (ec == (ECode)E_REMOTE_EXCEPTION) {
            *profile = NULL;
            return ec;
        }
        // } catch (RemoteException e) {
        //     Log.e(TAG, e.getLocalizedMessage(), e);
        // }
    } else {
        // Profiles are not enabled, return the empty profile
        *profile = sEmptyProfile;
        REFCOUNT_ADD(*profile)
    }

    return NOERROR;
}

ECode ProfileManager::AddProfile(
    /* [in] */ IProfile* profile)
{
    // try {
    Boolean result;
    return GetService()->AddProfile(profile, &result);
    // } catch (RemoteException e) {
    //     Log.e(TAG, e.getLocalizedMessage(), e);
    // }
}

ECode ProfileManager::RemoveProfile(
    /* [in] */ IProfile* profile)
{
    // try {
    Boolean result;
    return GetService()->RemoveProfile(profile, &result);
    // } catch (RemoteException e) {
    //     Log.e(TAG, e.getLocalizedMessage(), e);
    // }
}

ECode ProfileManager::UpdateProfile(
    /* [in] */ IProfile* profile)
{
    // try {
    return GetService()->UpdateProfile(profile);
    // } catch (RemoteException e) {
    //     Log.e(TAG, e.getLocalizedMessage(), e);
    // }
}

ECode ProfileManager::GetProfile(
    /* [in] */ const String& profileName,
    /* [out] */ IProfile** profile)
{
   VALIDATE_NOT_NULL(profile)

    // try {
    ECode ec = GetService()->GetProfileByName(profileName, profile);
    if (ec == (ECode)E_REMOTE_EXCEPTION) {
        *profile = NULL;
        return ec;
    }
    // } catch (RemoteException e) {
    //     Log.e(TAG, e.getLocalizedMessage(), e);
    // }
    return NOERROR;
}

ECode ProfileManager::GetProfile(
    /* [in] */ IUUID* profileUuid,
    /* [out] */ IProfile** profile)
{
    VALIDATE_NOT_NULL(profile)

    // try {
    AutoPtr<IParcelUuid> uuid;
    CParcelUuid::New(profileUuid, (IParcelUuid**)&uuid);
    ECode ec = GetService()->GetProfile(uuid, profile);
    if (ec == (ECode)E_REMOTE_EXCEPTION) {
        *profile = NULL;
        return ec;
    }
    // } catch (RemoteException e) {
    //     Log.e(TAG, e.getLocalizedMessage(), e);
    // }
    return NOERROR;
}

ECode ProfileManager::GetProfileNames(
    /* [out, callee] */ ArrayOf<String>** names)
{
    VALIDATE_NOT_NULL(names)
    // try {
    AutoPtr<ArrayOf<IProfile*> > profiles;
    ECode ec = GetService()->GetProfiles((ArrayOf<IProfile*>**)&profiles);
    if (ec == (ECode)E_REMOTE_EXCEPTION) {
        *names = NULL;
        return ec;
    }

    ArrayOf<String>* profileNames = ArrayOf<String>::Alloc(profiles->GetLength());
    for (Int32 i = 0; i < profiles->GetLength(); i++) {
        (*profiles)[i]->GetName(&(*profileNames)[i]);
    }
    *names = profileNames;
    // } catch (RemoteException e) {
    //     Log.e(TAG, e.getLocalizedMessage(), e);
    // }
    return NOERROR;
}

ECode ProfileManager::GetProfiles(
    /* [out, callee] */ ArrayOf<IProfile*>** profiles)
{
    VALIDATE_NOT_NULL(profiles)
    // try {
    ECode ec =  GetService()->GetProfiles(profiles);
    if (ec == (ECode)E_REMOTE_EXCEPTION) {
        *profiles = NULL;
        return ec;
    }
    // } catch (RemoteException e) {
    //     Log.e(TAG, e.getLocalizedMessage(), e);
    // }
    return NOERROR;
}

ECode ProfileManager::ProfileExists(
    /* [in] */ const String& profileName,
    /* [out] */ Boolean* isExisted)
{
    VALIDATE_NOT_NULL(isExisted)
    // try {
    ECode ec = GetService()->ProfileExistsByName(profileName, isExisted);
    if (ec == (ECode)E_REMOTE_EXCEPTION) {
        *isExisted = TRUE;
        return ec;
    }
    // } catch (RemoteException e) {
    //     Log.e(TAG, e.getLocalizedMessage(), e);
    //     // To be on the safe side, we'll return "true", to prevent duplicate profiles
    //     // from being created.
    //     return true;
    // }
    return NOERROR;
}

ECode ProfileManager::ProfileExists(
    /* [in] */ IUUID* profileUuid,
    /* [out] */ Boolean* isExisted)
{
    VALIDATE_NOT_NULL(isExisted)
    // try {
    AutoPtr<IParcelUuid> uuid;
    CParcelUuid::New(profileUuid, (IParcelUuid**)&uuid);
    ECode ec = GetService()->ProfileExists(uuid, isExisted);
    if (ec == (ECode)E_REMOTE_EXCEPTION) {
        *isExisted = TRUE;
        return ec;
    }
    // } catch (RemoteException e) {
    //     Log.e(TAG, e.getLocalizedMessage(), e);
    //     // To be on the safe side, we'll return "true", to prevent duplicate profiles
    //     // from being created.
    //     return true;
    // }
    return NOERROR;
}

ECode ProfileManager::NotificationGroupExists(
    /* [in] */ const String& notificationGroupName,
    /* [out] */ Boolean* isExisted)
{
    VALIDATE_NOT_NULL(isExisted)
    // try {
    ECode ec = GetService()->NotificationGroupExistsByName(notificationGroupName, isExisted);
    if (ec == (ECode)E_REMOTE_EXCEPTION) {
        *isExisted = TRUE;
        return ec;
    }
    // } catch (RemoteException e) {
    //     Log.e(TAG, e.getLocalizedMessage(), e);
    //     // To be on the safe side, we'll return "true", to prevent duplicate notification
    //     // groups from being created.
    //     return true;
    // }
    return NOERROR;
}

ECode ProfileManager::GetNotificationGroups(
    /* [out, callee] */ ArrayOf<INotificationGroup*>** groups)
{
    VALIDATE_NOT_NULL(groups)
    // try {
    ECode ec = GetService()->GetNotificationGroups(groups);
    if (ec == (ECode)E_REMOTE_EXCEPTION) {
        *groups = NULL;
        return ec;
    }
    // } catch (RemoteException e) {
    //     Log.e(TAG, e.getLocalizedMessage(), e);
    // }
    return NOERROR;
}

ECode ProfileManager::AddNotificationGroup(
    /* [in] */ INotificationGroup* group)
{
    // try {
    return GetService()->AddNotificationGroup(group);
    // } catch (RemoteException e) {
    //     Log.e(TAG, e.getLocalizedMessage(), e);
    // }
}

ECode ProfileManager::RemoveNotificationGroup(
    /* [in] */ INotificationGroup* group)
{
    // try {
    return GetService()->RemoveNotificationGroup(group);
    // } catch (RemoteException e) {
    //     Log.e(TAG, e.getLocalizedMessage(), e);
    // }
}


ECode ProfileManager::UpdateNotificationGroup(
    /* [in] */ INotificationGroup* group)
{
    // try {
    return GetService()->UpdateNotificationGroup(group);
    // } catch (RemoteException e) {
    //     Log.e(TAG, e.getLocalizedMessage(), e);
    // }
}

ECode ProfileManager::GetNotificationGroupForPackage(
    /* [in] */ const String& pkg,
    /* [out] */ INotificationGroup** group)
{
    VALIDATE_NOT_NULL(group)
    // try {
    ECode ec = GetService()->GetNotificationGroupForPackage(pkg, group);
    if (ec == (ECode)E_REMOTE_EXCEPTION) {
        *group = NULL;
        return ec;
    }
    // } catch (RemoteException e) {
    //     Log.e(TAG, e.getLocalizedMessage(), e);
    // }
    return NOERROR;
}

ECode ProfileManager::GetNotificationGroup(
    /* [in] */ IUUID* groupParcelUuid,
    /* [out] */ INotificationGroup** group)
{
    VALIDATE_NOT_NULL(group)
    // try {
    AutoPtr<IParcelUuid> uuid;
    CParcelUuid::New(groupParcelUuid, (IParcelUuid**)&uuid);
    ECode ec = GetService()->GetNotificationGroup(uuid, group);
    if (ec == (ECode)E_REMOTE_EXCEPTION) {
        *group = NULL;
        return ec;
    }
    // } catch (RemoteException e) {
    //     Log.e(TAG, e.getLocalizedMessage(), e);
    // }
    return NOERROR;
}

ECode ProfileManager::GetActiveProfileGroup(
    /* [in] */ const String& packageName,
    /* [out] */ IProfileGroup** group)
{
    VALIDATE_NOT_NULL(group)
    AutoPtr<INotificationGroup> notificationGroup;
    GetNotificationGroupForPackage(packageName, (INotificationGroup**)&notificationGroup);
    AutoPtr<IProfile> profile;
    GetActiveProfile((IProfile**)&profile);
    if (notificationGroup == NULL) {
        AutoPtr<IProfileGroup> defaultGroup;
        profile->GetDefaultGroup((IProfileGroup**)&defaultGroup);
        *group = defaultGroup;
        REFCOUNT_ADD(*group);
        return NOERROR;
    }

    AutoPtr<IUUID> uuid;
    notificationGroup->GetUuid((IUUID**)&uuid);
    return profile->GetProfileGroup(uuid, group);
}


ECode ProfileManager::ResetAll()
{
    // try {
    ECode ec = GetService()->ResetAll();
    if (ec == (ECode)E_REMOTE_EXCEPTION
            || ec == (ECode)E_SECURITY_EXCEPTION) {
        return ec;
    }
    return NOERROR;
    // } catch (RemoteException e) {
    //     Log.e(TAG, e.getLocalizedMessage(), e);
    // } catch (SecurityException e) {
    //     Log.e(TAG, e.getLocalizedMessage(), e);
    // }
}

} // namespace App
} // namespace Droid
} // namespace Elastos
