
#include <Elastos.CoreLibrary.Utility.h>
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Provider.h"
#include "elastos/droid/R.h"
#include "elastos/droid/ext/frameworkdef.h"
#include "elastos/droid/os/CUserManager.h"
#include "elastos/droid/os/CUserHandle.h"
#include "elastos/droid/os/SystemProperties.h"
#include "elastos/droid/os/Process.h"
#include "elastos/droid/os/UserHandle.h"
#include "elastos/droid/os/Build.h"
#include "elastos/droid/os/CBundle.h"
#include "elastos/droid/app/CActivityManager.h"
#include "elastos/droid/app/ActivityManagerNative.h"
#include "elastos/droid/content/res/CResourcesHelper.h"
#include "elastos/droid/provider/Settings.h"

using Elastos::Droid::R;
using Elastos::Droid::App::ActivityManagerNative;
using Elastos::Droid::App::CActivityManager;
using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Content::Res::IResourcesHelper;
using Elastos::Droid::Provider::Settings;
using Elastos::Droid::Provider::ISettingsGlobal;
using Elastos::Droid::Provider::ISettingsSecure;
using Elastos::Droid::Content::Res::CResourcesHelper;
using Elastos::Droid::Content::Pm::IPackageManager;
using Elastos::Utility::IArrayList;
using Elastos::Utility::CArrayList;
using Elastos::Utility::IIterator;

namespace Elastos {
namespace Droid {
namespace Os {

String CUserManager::TAG("CUserManager");
AutoPtr<IUserManager> CUserManager::sInstance;

CAR_INTERFACE_IMPL(CUserManager, Object, IUserManager)

CAR_OBJECT_IMPL(CUserManager)

AutoPtr<IUserManager> CUserManager::Get(
    /* [in] */ IContext* context)
{
    if (sInstance == NULL) {
        AutoPtr<IInterface> obj;
        context->GetSystemService(IContext::USER_SERVICE, (IInterface**)&obj);
        sInstance = IUserManager::Probe(obj);
    }
    return sInstance;
}

ECode CUserManager::CUserManager::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IIUserManager* service)
{
    mService = service;
    mContext = context;
    return NOERROR;
}

Boolean CUserManager::SupportsMultipleUsers()
{
    if (GetMaxSupportedUsers() <= 1) {
        return FALSE;
    }

    AutoPtr<IResourcesHelper> helper;
    CResourcesHelper::AcquireSingleton((IResourcesHelper**)&helper);
    AutoPtr<IResources> system;
    helper->GetSystem((IResources**)&system);
    Boolean bval, result;
    system->GetBoolean(R::bool_::config_enableMultiUserUI, &bval);
    SystemProperties::GetBoolean(String("fw.show_multiuserui"), bval, &result);
    return result;
}

ECode CUserManager::GetUserHandle(
    /* [out] */ Int32* userHandle)
{
    VALIDATE_NOT_NULL(userHandle);
    *userHandle = CUserHandle::GetMyUserId();
    return NOERROR;
}

ECode CUserManager::GetUserName(
    /* [out] */ String* userName)
{
    VALIDATE_NOT_NULL(userName);
    // try {
    Int32 user;
    GetUserHandle(&user);
    AutoPtr<IUserInfo> userInfo;
    ECode ec = mService->GetUserInfo(user, (IUserInfo**)&userInfo);
    if (FAILED(ec)) {
        *userName = "";
        return NOERROR;
    }
    else {
        return userInfo->GetName(userName);
    }
    // } catch (RemoteException re) {
    //     Log.w(TAG, "Could not get user name", re);
    //     return "";
    // }
}

ECode CUserManager::IsUserAGoat(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;
    AutoPtr<IPackageManager> pkgMgr;
    mContext->GetPackageManager((IPackageManager**)&pkgMgr);
    return pkgMgr->IsPackageAvailable(String("com.coffeestainstudios.goatsimulator"), result);
}

ECode CUserManager::IsLinkedUser(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    // try {
    return mService->IsRestricted(result);
    // } catch (RemoteException re) {
    //     Log.w(TAG, "Could not check if user is limited ", re);
    //     return false;
    // }
}

ECode CUserManager::IsGuestUser(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;

    AutoPtr<IUserInfo> user;
    GetUserInfo(UserHandle::GetMyUserId(), (IUserInfo**)&user);
    if (user != NULL) {
        return user->IsGuest(result);
    }
    return NOERROR;
}

ECode CUserManager::IsManagedProfile(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;

    AutoPtr<IUserInfo> user;
    GetUserInfo(UserHandle::GetMyUserId(), (IUserInfo**)&user);
    if (user != NULL) {
        return user->IsManagedProfile(result);
    }
    return NOERROR;
}

ECode CUserManager::IsUserRunning(
    /* [in] */ IUserHandle* user,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    // try {
    Int32 id;
    user->GetIdentifier(&id);
    ECode ec = ActivityManagerNative::GetDefault()->IsUserRunning(
            id, FALSE, result);
    if (FAILED(ec)) *result = FALSE;
    return NOERROR;
    // } catch (RemoteException e) {
    //     return false;
    // }
}

ECode CUserManager::IsUserRunningOrStopping(
    /* [in] */ IUserHandle* user,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    // try {
    Int32 id;
    user->GetIdentifier(&id);
    ECode ec = ActivityManagerNative::GetDefault()->IsUserRunning(
            id, TRUE, result);
    if (FAILED(ec)) *result = FALSE;
    return NOERROR;
    // } catch (RemoteException e) {
    //     return false;
    // }
}

ECode CUserManager::GetUserInfo(
    /* [in] */ Int32 userHandle,
    /* [out] */ IUserInfo** userInfo)
{
    VALIDATE_NOT_NULL(userInfo);
    // try {
    ECode ec = mService->GetUserInfo(userHandle, userInfo);
    if (FAILED(ec)) *userInfo = NULL;
    return NOERROR;
    // } catch (RemoteException re) {
    //     Log.w(TAG, "Could not get user info", re);
    //     return null;
    // }
}

ECode CUserManager::GetUserRestrictions(
    /* [out] */ IBundle** bundle)
{
    AutoPtr<IUserHandle> userHandle;
    Process::MyUserHandle((IUserHandle**)&userHandle);
    return GetUserRestrictions(userHandle, bundle);
}

ECode CUserManager::GetUserRestrictions(
    /* [in] */ IUserHandle* userHandle,
    /* [out] */ IBundle** bundle)
{
    VALIDATE_NOT_NULL(bundle)
    *bundle = NULL;
    // try {
    Int32 id;
    userHandle->GetIdentifier(&id);
    return mService->GetUserRestrictions(id, bundle);
    // } catch (RemoteException re) {
    //     Log.w(TAG, "Could not get user restrictions", re);
    //     return Bundle.EMPTY;
    // }
}

ECode CUserManager::SetUserRestrictions(
    /* [in] */ IBundle* restrictions)
{
    AutoPtr<IUserHandle> userHandle;
    Process::MyUserHandle((IUserHandle**)&userHandle);
    return SetUserRestrictions(restrictions, userHandle);
}

ECode CUserManager::SetUserRestrictions(
    /* [in] */ IBundle* restrictions,
    /* [in] */ IUserHandle* userHandle)
{
    // try {
    Int32 id;
    userHandle->GetIdentifier(&id);
    return mService->SetUserRestrictions(restrictions, id);
    // } catch (RemoteException re) {
    //     Log.w(TAG, "Could not set user restrictions", re);
    // }
}

ECode CUserManager::SetUserRestriction(
    /* [in] */ const String& key,
    /* [in] */ Boolean value)
{
    AutoPtr<IBundle> bundle;
    GetUserRestrictions((IBundle**)&bundle);
    IBaseBundle::Probe(bundle)->PutBoolean(key, value);
    return SetUserRestrictions(bundle);
}

ECode CUserManager::SetUserRestriction(
    /* [in] */ const String& key,
    /* [in] */ Boolean value,
    /* [in] */ IUserHandle* userHandle)
{
    AutoPtr<IBundle> bundle;
    GetUserRestrictions(userHandle, (IBundle**)&bundle);
    IBaseBundle::Probe(bundle)->PutBoolean(key, value);
    return SetUserRestrictions(bundle, userHandle);
}

ECode CUserManager::HasUserRestriction(
    /* [in] */ const String& restrictionKey,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<IUserHandle> userHandle;
    Process::MyUserHandle((IUserHandle**)&userHandle);
    return HasUserRestriction(restrictionKey, userHandle, result);
}

ECode CUserManager::HasUserRestriction(
    /* [in] */ const String& restrictionKey,
    /* [in] */ IUserHandle* userHandle,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    // try {
    Int32 id;
    userHandle->GetIdentifier(&id);
    return mService->HasUserRestriction(restrictionKey, id, result);
    // } catch (RemoteException re) {
    //     Log.w(TAG, "Could not check user restrictions", re);
    //     return false;
    // }
}

ECode CUserManager::GetSerialNumberForUser(
    /* [in] */ IUserHandle* user,
    /* [out] */ Int64* number)
{
    VALIDATE_NOT_NULL(number);
    Int32 id;
    user->GetIdentifier(&id);
    Int32 n;
    FAIL_RETURN(GetUserSerialNumber(id, &n));
    *number = n;
    return NOERROR;
}

ECode CUserManager::GetUserForSerialNumber(
    /* [in] */ Int64 serialNumber,
    /* [out] */ IUserHandle** user)
{
    VALIDATE_NOT_NULL(user);
    *user = NULL;

    Int32 ident;
    FAIL_RETURN(GetUserHandle((Int32)serialNumber, &ident));
    if (ident >= 0) {
        return CUserHandle::New(ident, user);
    }

    return NOERROR;
}

ECode CUserManager::CreateUser(
    /* [in] */ const String& name,
    /* [in] */ Int32 flags,
    /* [out] */ IUserInfo** userInfo)
{
    VALIDATE_NOT_NULL(userInfo);
    // try {
    ECode ec = mService->CreateUser(name, flags, userInfo);
    if (FAILED(ec)) *userInfo = NULL;
    return NOERROR;
    // } catch (RemoteException re) {
    //     Log.w(TAG, "Could not create a user", re);
    //     return null;
    // }
}

ECode CUserManager::CreateGuest(
    /* [in] */ IContext* context,
    /* [in] */ const String& name,
    /* [out] */ IUserInfo** userInfo)
{
    VALIDATE_NOT_NULL(userInfo)
    *userInfo = NULL;

    AutoPtr<IUserInfo> guest;
    CreateUser(name, IUserInfo::FLAG_GUEST, (IUserInfo**)&guest);
    if (guest != NULL) {
        Int32 id;
        guest->GetId(&id);
        AutoPtr<IContentResolver> resolver;
        context->GetContentResolver((IContentResolver**)&resolver);
        Boolean bval;
        Settings::Secure::PutStringForUser(resolver,
            ISettingsSecure::SKIP_FIRST_USE_HINTS, String("1"), id, &bval);
        // try {
            AutoPtr<IBundle> guestRestrictions;
            mService->GetDefaultGuestRestrictions((IBundle**)&guestRestrictions);
            IBaseBundle::Probe(guestRestrictions)->PutBoolean(DISALLOW_SMS, TRUE);
            IBaseBundle::Probe(guestRestrictions)->PutBoolean(DISALLOW_INSTALL_UNKNOWN_SOURCES, TRUE);
            mService->SetUserRestrictions(guestRestrictions, id);
        // } catch (RemoteException re) {
        //     Log.w(TAG, "Could not update guest restrictions");
        // }
    }

    *userInfo = guest;
    REFCOUNT_ADD(*userInfo)
    return NOERROR;
}

ECode CUserManager::CreateSecondaryUser(
    /* [in] */ const String& name,
    /* [in] */ Int32 flags,
    /* [out] */ IUserInfo** userInfo)
{
    VALIDATE_NOT_NULL(userInfo)
    *userInfo = NULL;

    // try {
    AutoPtr<IUserInfo> user;
    mService->CreateUser(name, flags, (IUserInfo**)&user);
    if (user == NULL) {
        return NOERROR;
    }

    Int32 id;
    user->GetId(&id);
    AutoPtr<IBundle> userRestrictions;
    mService->GetUserRestrictions(id, (IBundle**)&userRestrictions);
    AddDefaultUserRestrictions(userRestrictions);
    mService->SetUserRestrictions(userRestrictions, id);
    *userInfo = user;
    REFCOUNT_ADD(*userInfo)
    return NOERROR;
    // } catch (RemoteException re) {
    //     Log.w(TAG, "Could not create a user", re);
    //     return null;
    // }
}

ECode CUserManager::AddDefaultUserRestrictions(
    /* [in] */ IBundle* restrictions)
{
    IBaseBundle::Probe(restrictions)->PutBoolean(DISALLOW_OUTGOING_CALLS, TRUE);
    IBaseBundle::Probe(restrictions)->PutBoolean(DISALLOW_SMS, TRUE);
    return NOERROR;
}

ECode CUserManager::CreateProfileForUser(
    /* [in] */ const String& name,
    /* [in] */ Int32 flags,
    /* [in] */ Int32 userHandle,
    /* [out] */ IUserInfo** userInfo)
{
    // try {
    return mService->CreateProfileForUser(name, flags, userHandle, userInfo);
    // } catch (RemoteException re) {
    //     Log.w(TAG, "Could not create a user", re);
    //     return null;
    // }
}

ECode CUserManager::MarkGuestForDeletion(
    /* [in] */ Int32 userHandle,
    /* [out] */ Boolean* result)
{
    // try {
    return mService->MarkGuestForDeletion(userHandle, result);
    // } catch (RemoteException re) {
    //     Log.w(TAG, "Could not mark guest for deletion", re);
    //     return false;
    // }
}

ECode CUserManager::SetUserEnabled(
    /* [in] */ Int32 userHandle)
{
    // try {
    return mService->SetUserEnabled(userHandle);
    // } catch (RemoteException e) {
    //     Log.w(TAG, "Could not enable the profile", e);
    // }
}

ECode CUserManager::GetUserCount(
    /* [out] */ Int32* count)
{
    VALIDATE_NOT_NULL(count);
    AutoPtr<IList> users;
    GetUsers((IList**)&users);
    if (users != NULL) {
        users->GetSize(count);
    }
    else *count = 1;
    return NOERROR;
}

ECode CUserManager::GetUsers(
    /* [out] */ IList** users)
{
    VALIDATE_NOT_NULL(users);
    // try {
    ECode ec = mService->GetUsers(FALSE, users);
    if (FAILED(ec)) *users = NULL;
    return NOERROR;
    // } catch (RemoteException re) {
    //     Log.w(TAG, "Could not get user list", re);
    //     return null;
    // }
}

ECode CUserManager::CanAddMoreUsers(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    AutoPtr<IList> users;
    GetUsers(TRUE, (IList**)&users);
    Int32 totalUserCount;
    users->GetSize(&totalUserCount);
    Int32 aliveUserCount = 0;
    IUserInfo* user;
    Boolean isGuest;
    for (Int32 i = 0; i < totalUserCount; i++) {
        AutoPtr<IInterface> obj;
        users->Get(i, (IInterface**)&obj);
        user = IUserInfo::Probe(obj);
        if (user->IsGuest(&isGuest), !isGuest) {
            aliveUserCount++;
        }
    }
    *result = aliveUserCount < GetMaxSupportedUsers();
    return NOERROR;
}

ECode CUserManager::GetProfiles(
    /* [in] */ Int32 userHandle,
    /* [out] */ IList** list)
{
    // try {
        return mService->GetProfiles(userHandle, FALSE /* enabledOnly */, list);
    // } catch (RemoteException re) {
    //     Log.w(TAG, "Could not get user list", re);
    //     return null;
    // }
}

ECode CUserManager::GetUserProfiles(
    /* [out] */ IList** list)
{
    VALIDATE_NOT_NULL(list)
    *list = NULL;

    AutoPtr<IArrayList> profiles;
    CArrayList::New((IArrayList**)&profiles);
    AutoPtr<IList> users;
    CArrayList::New((IList**)&users);

    // try {
        FAIL_RETURN(mService->GetProfiles(
            UserHandle::GetMyUserId(), TRUE /* enabledOnly */, (IList**)&users))
    // } catch (RemoteException re) {
    //     Log.w(TAG, "Could not get user list", re);
    //     return null;
    // }
    AutoPtr<IIterator> it;
    users->GetIterator((IIterator**)&it);
    Boolean hasNext;
    IUserInfo* info;
    Int32 id;
    while (it->HasNext(&hasNext), hasNext) {
        AutoPtr<IInterface> obj;
        it->GetNext((IInterface**)&obj);
        info = IUserInfo::Probe(obj);
        info->GetId(&id);
        AutoPtr<IUserHandle> userHandle;
        CUserHandle::New(id, (IUserHandle**)&userHandle);
        profiles->Add(userHandle.Get());
    }

    *list = IList::Probe(profiles);
    REFCOUNT_ADD(*list)
    return NOERROR;
}

ECode CUserManager::GetProfileParent(
    /* [in] */ Int32 userHandle,
    /* [out] */ IUserInfo** userInfo)
{
    VALIDATE_NOT_NULL(userInfo)
    //try {
    return mService->GetProfileParent(userHandle, userInfo);
    // } catch (RemoteException re) {
    //     Log.w(TAG, "Could not get profile parent", re);
    //     return null;
    // }
}

ECode CUserManager::GetBadgedIconForUser(
    /* [in] */ IDrawable* icon,
    /* [in] */ IUserHandle* user,
    /* [out] */ IDrawable** drawable)
{
    VALIDATE_NOT_NULL(drawable)
    AutoPtr<IPackageManager> pkgMgr;
    mContext->GetPackageManager((IPackageManager**)&pkgMgr);
    return pkgMgr->GetUserBadgedIcon(icon, user, drawable);
}

ECode CUserManager::GetBadgedDrawableForUser(
    /* [in] */ IDrawable* badgedDrawable,
    /* [in] */ IUserHandle* user,
    /* [in] */ IRect* badgeLocation,
    /* [in] */ Int32 badgeDensity,
    /* [out] */ IDrawable** drawable)
{
    VALIDATE_NOT_NULL(drawable)
    AutoPtr<IPackageManager> pkgMgr;
    mContext->GetPackageManager((IPackageManager**)&pkgMgr);
    return pkgMgr->GetUserBadgedDrawableForDensity(badgedDrawable, user,
            badgeLocation, badgeDensity, drawable);
}

ECode CUserManager::GetBadgedLabelForUser(
    /* [in] */ ICharSequence* label,
    /* [in] */ IUserHandle* user,
    /* [out] */ ICharSequence** csq)
{
    VALIDATE_NOT_NULL(csq)
    AutoPtr<IPackageManager> pkgMgr;
    mContext->GetPackageManager((IPackageManager**)&pkgMgr);
    return pkgMgr->GetUserBadgedLabel(label, user, csq);
}

ECode CUserManager::GetUsers(
    /* [in] */ Boolean excludeDying,
    /* [out] */ IList** users)
{
    VALIDATE_NOT_NULL(users);
    // try {
    return mService->GetUsers(excludeDying, users);
    // } catch (RemoteException re) {
    //     Log.w(TAG, "Could not get user list", re);
    //     return null;
    // }
}

ECode CUserManager::RemoveUser(
    /* [in] */ Int32 userHandle,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    // try {
    return mService->RemoveUser(userHandle, result);
    // } catch (RemoteException re) {
    //     Log.w(TAG, "Could not remove user ", re);
    //     return false;
    // }
}

ECode CUserManager::SetUserName(
    /* [in] */ Int32 userHandle,
    /* [in] */ const String& name)
{
    // try {
    return mService->SetUserName(userHandle, name);
    // } catch (RemoteException re) {
    //     Log.w(TAG, "Could not set the user name ", re);
    // }
}

ECode CUserManager::SetUserIcon(
    /* [in] */ Int32 userHandle,
    /* [in] */ IBitmap* icon)
{
    // try {
    return mService->SetUserIcon(userHandle, icon);
    // } catch (RemoteException re) {
    //     Log.w(TAG, "Could not set the user icon ", re);
    // }
}

ECode CUserManager::GetUserIcon(
    /* [in] */ Int32 userHandle,
    /* [out] */ IBitmap** icon)
{
    VALIDATE_NOT_NULL(icon);
    // try {
    ECode ec = mService->GetUserIcon(userHandle, icon);
    if (FAILED(ec)) *icon = NULL;
    return NOERROR;
    // } catch (RemoteException re) {
    //     Log.w(TAG, "Could not get the user icon ", re);
    //     return null;
    // }
}

/**
 * Returns the maximum number of users that can be created on this device. A return value
 * of 1 means that it is a single user device.
 * @hide
 * @return a value greater than or equal to 1
 */
Int32 CUserManager::GetMaxSupportedUsers()
{
    // Don't allow multiple users on certain builds
    if (Build::ID.StartWith(String("JVP"))) return 1;
    // Svelte devices don't get multi-user.
    if (CActivityManager::IsLowRamDeviceStatic()) return 1;

    AutoPtr<IResourcesHelper> helper;
    CResourcesHelper::AcquireSingleton((IResourcesHelper**)&helper);
    AutoPtr<IResources> system;
    helper->GetSystem((IResources**)&system);
    Int32 ival;
    system->GetInteger(R::integer::config_multiuserMaximumUsers, &ival);
    Int32 result;
    SystemProperties::GetInt32(String("fw.max_users"), ival, &result);
    return result;
}

ECode CUserManager::IsUserSwitcherEnabled(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;

    AutoPtr<IList> users;
    GetUsers(TRUE, (IList**)&users);
    if (users == NULL) {
       return NOERROR;
    }

    Int32 switchableUserCount = 0;
    AutoPtr<IIterator> it;
    users->GetIterator((IIterator**)&it);
    Boolean hasNext, support;
    IUserInfo* info;
    while (it->HasNext(&hasNext), hasNext) {
        AutoPtr<IInterface> obj;
        it->GetNext((IInterface**)&obj);
        info = IUserInfo::Probe(obj);
        info->SupportsSwitchTo(&support);
        if (support) {
            ++switchableUserCount;
        }
    }

    AutoPtr<IContentResolver> resolver;
    mContext->GetContentResolver((IContentResolver**)&resolver);

    Int32 ival;
    Settings::Global::GetInt32(resolver, ISettingsGlobal::GUEST_USER_ENABLED, 0, &ival);
    Boolean guestEnabled = ival == 1;
    *result = switchableUserCount > 1 || guestEnabled;
    return NOERROR;
}

ECode CUserManager::GetUserSerialNumber(
    /* [in] */ Int32 userHandle,
    /* [out] */ Int32* number)
{
    VALIDATE_NOT_NULL(number);
    // try {
    ECode ec = mService->GetUserSerialNumber(userHandle, number);
    if (FAILED(ec)) *number = -1;
    return NOERROR;
    // } catch (RemoteException re) {
    //     Log.w(TAG, "Could not get serial number for user " + userHandle);
    // }
    // return -1;
}

ECode CUserManager::GetUserHandle(
    /* [in] */ Int32 userSerialNumber,
    /* [out] */ Int32* userHandle)
{
    VALIDATE_NOT_NULL(userHandle);
    // try {
    ECode ec = mService->GetUserHandle(userSerialNumber, userHandle);
    if (FAILED(ec)) *userHandle = -1;
    return NOERROR;
    // } catch (RemoteException re) {
    //     Log.w(TAG, "Could not get userHandle for user " + userSerialNumber);
    // }
    // return -1;
}

ECode CUserManager::GetApplicationRestrictions(
    /* [in] */ const String& packageName,
    /* [out] */ IBundle** bundle)
{
    // try {
    return mService->GetApplicationRestrictions(packageName, bundle);
    // } catch (RemoteException re) {
    //     Log.w(TAG, "Could not get application restrictions for package " + packageName);
    // }
    // return null;
}

ECode CUserManager::GetApplicationRestrictions(
    /* [in] */ const String& packageName,
    /* [in] */ IUserHandle* user,
    /* [out] */ IBundle** bundle)
{
    // try {
    Int32 id;
    user->GetIdentifier(&id);
    return mService->GetApplicationRestrictionsForUser(packageName, id, bundle);
    // } catch (RemoteException re) {
    //     Log.w(TAG, "Could not get application restrictions for user " + user.getIdentifier());
    // }
    // return null;
}

ECode CUserManager::SetApplicationRestrictions(
    /* [in] */ const String& packageName,
    /* [in] */ IBundle* restrictions,
    /* [in] */ IUserHandle* user)
{
    // try {
    Int32 id;
    user->GetIdentifier(&id);
    return mService->SetApplicationRestrictions(packageName, restrictions, id);
    // } catch (RemoteException re) {
    //     Log.w(TAG, "Could not set application restrictions for user " + user.getIdentifier());
    // }
}

ECode CUserManager::SetRestrictionsChallenge(
    /* [in] */ const String& newPin,
    /* [out] */ Boolean* result)
{
    // try {
    return mService->SetRestrictionsChallenge(newPin, result);
    // } catch (RemoteException re) {
    //     Log.w(TAG, "Could not change restrictions pin");
    // }
    // return false;
}

ECode CUserManager::CheckRestrictionsChallenge(
    /* [in] */ const String& pin,
    /* [out] */ Int32* result)
{
    // try {
    return mService->CheckRestrictionsChallenge(pin, result);
    // } catch (RemoteException re) {
    //     Log.w(TAG, "Could not check restrictions pin");
    // }
    // return PIN_VERIFICATION_FAILED_INCORRECT;
}

ECode CUserManager::HasRestrictionsChallenge(
    /* [out] */ Boolean* result)
{
    // try {
    return mService->HasRestrictionsChallenge(result);
    // } catch (RemoteException re) {
    //     Log.w(TAG, "Could not change restrictions pin");
    // }
    // return false;
}

ECode CUserManager::RemoveRestrictions()
{
    // try {
    return mService->RemoveRestrictions();
    // } catch (RemoteException re) {
    //     Log.w(TAG, "Could not change restrictions pin");
    // }
}

ECode CUserManager::SetDefaultGuestRestrictions(
    /* [in] */ IBundle* restrictions)
{
    // try {
    return mService->SetDefaultGuestRestrictions(restrictions);
    // } catch (RemoteException re) {
    //     Log.w(TAG, "Could not set guest restrictions");
    // }
}

ECode CUserManager::GetDefaultGuestRestrictions(
    /* [out] */ IBundle** bundle)
{
    VALIDATE_NOT_NULL(bundle)

    // try {
    AutoPtr<IBundle> b;
    ECode ec = mService->GetDefaultGuestRestrictions((IBundle**)&b);
    if (FAILED(ec)) {
        b = NULL;
        CBundle::New((IBundle**)&b);
    }
    // } catch (RemoteException re) {
    //     Log.w(TAG, "Could not set guest restrictions");
    // }
    *bundle = b;
    REFCOUNT_ADD(*bundle)
    return NOERROR;
}

} // namespace Os
} // namespace Droid
} // namespace Elastos
