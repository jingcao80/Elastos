
#include "elastos/droid/server/pm/CUserManagerService.h"
#include "elastos/droid/server/pm/CUserStopUserCallback.h"
#include "elastos/droid/server/pm/CPackageManagerService.h"
#include "elastos/droid/app/ActivityManagerNative.h"
#include "elastos/droid/os/Binder.h"
#include "elastos/droid/os/FileUtils.h"
#include "elastos/droid/os/UserHandle.h"
#include "elastos/droid/os/Environment.h"
#include "elastos/droid/os/ServiceManager.h"
#include "elastos/droid/R.h"
#include "elastos/droid/Manifest.h"
#include "elastos/droid/utility/Xml.h"
#include <Elastos.Droid.Graphics.h>
#include <Elastos.CoreLibrary.Security.h>
#include <elastos/core/AutoLock.h>
#include <elastos/core/Math.h>
#include <elastos/core/StringUtils.h>
#include <elastos/core/StringBuilder.h>
#include <elastos/utility/etl/List.h>
#include <elastos/utility/logging/Logger.h>
#include <elastos/utility/logging/Slogger.h>

using Elastos::Core::AutoLock;
using Elastos::Core::StringUtils;
using Elastos::Core::StringBuilder;
using Elastos::Core::IBoolean;
using Elastos::Core::CBoolean;
using Elastos::Core::EIID_IRunnable;
using Elastos::Core::ISystem;
using Elastos::Core::CSystem;
using Elastos::Core::IArrayOf;
using Elastos::IO::CFile;
using Elastos::IO::IFileInputStream;
using Elastos::IO::IFileOutputStream;
using Elastos::IO::CFileOutputStream;
using Elastos::IO::IBufferedOutputStream;
using Elastos::IO::CBufferedOutputStream;
using Elastos::IO::ICloseable;
using Elastos::Security::ISecureRandom;
using Elastos::Security::ISecureRandomHelper;
using Elastos::Security::IMessageDigest;
// using Elastos::Security::CSecureRandomHelper;
using Elastos::Security::IMessageDigestHelper;
using Elastos::Security::CMessageDigestHelper;
using Elastos::Utility::IArrayList;
using Elastos::Utility::CArrayList;
using Elastos::Utility::IRandom;
using Elastos::Utility::Etl::List;
using Elastos::Utility::Logging::Slogger;
using Elastos::Utility::Logging::Logger;
using Org::Xmlpull::V1::IXmlSerializer;
using Elastos::Droid::App::CActivityManagerHelper;
using Elastos::Droid::App::IActivityManagerHelper;
using Elastos::Droid::App::ActivityManagerNative;
using Elastos::Droid::App::IActivity;
using Elastos::Droid::App::IActivityManager;
using Elastos::Droid::R;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Content::Pm::CUserInfo;
using Elastos::Droid::Content::Pm::IPackageManager;
using Elastos::Droid::Content::Pm::IPackageItemInfo;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Internal::Utility::CFastXmlSerializer;
using Elastos::Droid::Internal::Utility::IFastXmlSerializer;
using Elastos::Droid::Os::Binder;
using Elastos::Droid::Os::Environment;
using Elastos::Droid::Os::FileUtils;
using Elastos::Droid::Os::CHandler;
using Elastos::Droid::Os::IProcess;
using Elastos::Droid::Os::UserHandle;
using Elastos::Droid::Os::IUserHandle;
using Elastos::Droid::Os::CUserHandle;
using Elastos::Droid::Os::IUserManagerHelper;
using Elastos::Droid::Os::CUserManagerHelper;
using Elastos::Droid::Os::CBundle;
using Elastos::Droid::Os::EIID_IBinder;
using Elastos::Droid::Os::EIID_IIUserManager;
using Elastos::Droid::Os::IUserManager;
using Elastos::Droid::Os::ServiceManager;
using Elastos::Droid::Text::Format::IDateUtils;
using Elastos::Droid::Utility::IAtomicFile;
using Elastos::Droid::Utility::CAtomicFile;
using Elastos::Droid::Utility::CParcelableList;
using Elastos::Droid::Utility::Xml;
using Elastos::Droid::Graphics::IBitmapFactory;
using Elastos::Droid::Graphics::CBitmapFactory;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Pm {

//==============================================================================
//                  CUserManagerService::FinishRemoveUserReceiver::FinishRemoveUserThread
//==============================================================================

CUserManagerService::FinishRemoveUserReceiver::FinishRemoveUserThread::FinishRemoveUserThread(
    /* [in] */ CUserManagerService* host,
    /* [in] */ Int32 userHandle)
    : mHost(host)
    , mUserHandle(userHandle)
{
    Thread::constructor(String("FinishRemoveUserThread"));
}

ECode CUserManagerService::FinishRemoveUserReceiver::FinishRemoveUserThread::Run()
{
    AutoPtr<IObject> lock = mHost->mInstallLock;
    {    AutoLock syncLock(lock);
        AutoPtr<IObject> pkgLock = mHost->mPackagesLock;
        {    AutoLock syncLock(pkgLock);
            mHost->RemoveUserStateLocked(mUserHandle);
        }
    }
    return NOERROR;
}


//==============================================================================
//                  CUserManagerService::FinishRemoveUserReceiver
//==============================================================================

ECode CUserManagerService::FinishRemoveUserReceiver::OnReceive(
    /* [in] */ IContext* context,
    /* [in] */ IIntent* intent)
{
    if (CUserManagerService::DBG) {
        Slogger::I(CUserManagerService::TAG,
                "USER_REMOVED broadcast sent, cleaning up user data %d", mUserHandle);
    }
    AutoPtr<FinishRemoveUserThread> thread = new FinishRemoveUserThread(mHost, mUserHandle);
    thread->Start();
    return NOERROR;
}


//==============================================================================
//                  CUserManagerService::RemoveUserStateRunnable
//==============================================================================

ECode CUserManagerService::RemoveUserStateRunnable::Run()
{
    AutoPtr<IObject> lock = mHost->mPackagesLock;
    {    AutoLock syncLock(lock);
        mHost->mRemovingUserIds.Erase(mUserHandle);
    }
    return NOERROR;
}


//==============================================================================
//                  CUserManagerService::UnhideAllInstalledAppsRunnable
//==============================================================================

ECode CUserManagerService::UnhideAllInstalledAppsRunnable::Run()
{
    AutoPtr<IParceledListSlice> slice;
    mHost->mPm->GetInstalledApplications(IPackageManager::GET_UNINSTALLED_PACKAGES,
            mUserHandle, (IParceledListSlice**)&slice);
    AutoPtr<IList> apps;
    slice->GetList((IList**)&apps);
    Int64 ident = Binder::ClearCallingIdentity();
    // try {
    AutoPtr<IIterator> it;
    apps->GetIterator((IIterator**)&it);
    Boolean hasNext;
    while (it->HasNext(&hasNext), hasNext) {
        AutoPtr<IInterface> value;
        it->GetNext((IInterface**)&value);
        AutoPtr<IApplicationInfo> appInfo = IApplicationInfo::Probe(value);
        Int32 flags;
        if ((appInfo->GetFlags(&flags), (flags & IApplicationInfo::FLAG_INSTALLED) != 0)
                && (flags & IApplicationInfo::FLAG_HIDDEN) != 0) {
            String packageName;
            IPackageItemInfo::Probe(appInfo)->GetPackageName(&packageName);
            Boolean result;
            mHost->mPm->SetApplicationHiddenSettingAsUser(packageName, FALSE, mUserHandle, &result);
        }
    }
    // } finally {
    //     Binder.restoreCallingIdentity(ident);
    // }
    Binder::RestoreCallingIdentity(ident);
    return NOERROR;
}


//==============================================================================
//                  CUserManagerService
//==============================================================================

const String CUserManagerService::TAG("CUserManagerService");
const Boolean CUserManagerService::DBG;
const String CUserManagerService::TAG_NAME("name");
const String CUserManagerService::ATTR_FLAGS("flags");
const String CUserManagerService::ATTR_ICON_PATH("icon");
const String CUserManagerService::ATTR_ID("id");
const String CUserManagerService::ATTR_CREATION_TIME("created");
const String CUserManagerService::ATTR_LAST_LOGGED_IN_TIME("lastLoggedIn");
const String CUserManagerService::ATTR_SALT("salt");
const String CUserManagerService::ATTR_PIN_HASH("pinHash");
const String CUserManagerService::ATTR_FAILED_ATTEMPTS("failedAttempts");
const String CUserManagerService::ATTR_LAST_RETRY_MS("lastAttemptMs");
const String CUserManagerService::ATTR_SERIAL_NO("serialNumber");
const String CUserManagerService::ATTR_NEXT_SERIAL_NO("nextSerialNumber");
const String CUserManagerService::ATTR_PARTIAL("partial");
const String CUserManagerService::ATTR_GUEST_TO_REMOVE("guestToRemove");
const String CUserManagerService::ATTR_USER_VERSION("version");
const String CUserManagerService::ATTR_PROFILE_GROUP_ID("profileGroupId");
const String CUserManagerService::TAG_GUEST_RESTRICTIONS("guestRestrictions");
const String CUserManagerService::TAG_USERS("users");
const String CUserManagerService::TAG_USER("user");
const String CUserManagerService::TAG_RESTRICTIONS("restrictions");
const String CUserManagerService::TAG_ENTRY("entry");
const String CUserManagerService::TAG_VALUE("value");
const String CUserManagerService::ATTR_KEY("key");
const String CUserManagerService::ATTR_VALUE_TYPE("type");
const String CUserManagerService::ATTR_MULTIPLE("m");
const String CUserManagerService::ATTR_TYPE_STRING_ARRAY("sa");
const String CUserManagerService::ATTR_TYPE_STRING("s");
const String CUserManagerService::ATTR_TYPE_BOOLEAN("b");
const String CUserManagerService::ATTR_TYPE_INTEGER("i");
const String CUserManagerService::USER_INFO_DIR("system/users"); //"system" + File.separator + "users";
const String CUserManagerService::USER_LIST_FILENAME("userlist.xml");
const String CUserManagerService::USER_PHOTO_FILENAME("photo.png");
const String CUserManagerService::RESTRICTIONS_FILE_PREFIX("res_");
const String CUserManagerService::XML_SUFFIX(".xml");
const Int32 CUserManagerService::MIN_USER_ID;
const Int32 CUserManagerService::USER_VERSION;
const Int64 CUserManagerService::EPOCH_PLUS_30_YEARS; // ms
const Int32 CUserManagerService::BACKOFF_INC_INTERVAL;
const Int32 CUserManagerService::MAX_MANAGED_PROFILES;

static AutoPtr<ArrayOf<Int32> > InitBackOffTimes()
{
    AutoPtr<ArrayOf<Int32> > arrays = ArrayOf<Int32>::Alloc(5);
    (*arrays)[0] = 0;
    (*arrays)[1] = 30*1000;
    (*arrays)[2] = 60*1000;
    (*arrays)[3] = 5*60*1000;
    (*arrays)[4] = 30*60*1000;
    return arrays;
}
const AutoPtr<ArrayOf<Int32> > CUserManagerService::BACKOFF_TIMES = InitBackOffTimes();
AutoPtr<CUserManagerService> CUserManagerService::sInstance;
Object CUserManagerService::sLock;

CUserManagerService::CUserManagerService()
    : mPm(NULL)
    , mInstallLock(NULL)
    , mPackagesLock(NULL)
    , mUsers(7)
    , mRemovingUserIds(7)
    , mNextSerialNumber(0)
    , mUserVersion(0)
{
    CBundle::New((IBundle**)&mGuestRestrictions);
}

CAR_INTERFACE_IMPL_2(CUserManagerService, Object, IIUserManager, IBinder)

CAR_OBJECT_IMPL(CUserManagerService)

ECode CUserManagerService::constructor(
    /* [in] */ IFile* dataDir,
    /* [in] */ IFile* baseUserPath)
{
    AutoPtr<IObject> installLock = (IObject*)new Object();
    AutoPtr<IObject> packagesLock = (IObject*)new Object();
    return Init(NULL, NULL, installLock, packagesLock, dataDir, baseUserPath);
}

ECode CUserManagerService::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IIPackageManager* pm,
    /* [in] */ IObject* installLock,
    /* [in] */ IObject* packagesLock)
{
    AutoPtr<IFile> dataDir = Environment::GetDataDirectory();
    AutoPtr<IFile> baseUserPath;
    CFile::New(dataDir, String("user"), (IFile**)&baseUserPath);
    return Init(context, pm, installLock, packagesLock, dataDir, baseUserPath);
}

ECode CUserManagerService::Init(
    /* [in] */ IContext* context,
    /* [in] */ IIPackageManager* pm,
    /* [in] */ IObject* installLock,
    /* [in] */ IObject* packagesLock,
    /* [in] */ IFile* dataDir,
    /* [in] */ IFile* baseUserPath)
{
    mContext = context;
    mPm = (CPackageManagerService*)pm;
    mInstallLock = installLock;
    mPackagesLock = packagesLock;
    CHandler::New((IHandler**)&mHandler);
    {    AutoLock syncLock(mInstallLock);
        {    AutoLock syncLock(mPackagesLock);
            Boolean result;
            CFile::New(dataDir, USER_INFO_DIR, (IFile**)&mUsersDir);
            mUsersDir->Mkdirs(&result);
            // Make zeroth user directory, for services to migrate their files to that location
            AutoPtr<IFile> userZeroDir;
            CFile::New(mUsersDir, String("0"), (IFile**)&userZeroDir);
            userZeroDir->Mkdirs(&result);
            mBaseUserPath = baseUserPath;
            String usersDirStr;
            mUsersDir->ToString(&usersDirStr);

            FileUtils::SetPermissions(usersDirStr,
                FileUtils::sS_IRWXU | FileUtils::sS_IRWXG
                | FileUtils::sS_IROTH | FileUtils::sS_IXOTH,
                -1, -1);

            CFile::New(mUsersDir, USER_LIST_FILENAME, (IFile**)&mUserListFile);
            Logger::I(TAG, " read user list file %s", TO_CSTR(mUserListFile));
            ReadUserListLocked();
            // Prune out any partially created/partially removed users.
            Boolean partial, guestToRemove;
            List< AutoPtr<IUserInfo> > partials;
            HashMap<Int32, AutoPtr<IUserInfo> >::Iterator it;
            for (it = mUsers.Begin(); it != mUsers.End(); ++it) {
                AutoPtr<IUserInfo> ui = it->mSecond;
                ui->GetPartial(&partial);
                ui->GetGuestToRemove(&guestToRemove);
                if ((partial || guestToRemove) && it != mUsers.Begin()) {
                    partials.PushBack(ui);
                }
            }
            Int32 i =0;
            List< AutoPtr<IUserInfo> >::Iterator pit;
            for (pit = partials.Begin(); pit != partials.End(); ++pit) {
                AutoPtr<IUserInfo> ui = *pit;
                Int32 id;
                ui->GetId(&id);
                String name;
                ui->GetName(&name);
                Slogger::W(TAG, "Removing partially created user #%d (id=%d, name=%s).", i++, id, name.string());

                RemoveUserStateLocked(id);
            }
            sInstance = this;
        }
    }
    return NOERROR;
}

void CUserManagerService::SystemReady()
{
    UserForeground(IUserHandle::USER_OWNER);
    mAppOpsService = IIAppOpsService::Probe(ServiceManager::GetService(IContext::APP_OPS_SERVICE));
    for (Int32 i = 0; i < mUserIds->GetLength(); ++i) {
        // try {
        AutoPtr<IBundle> b;
        HashMap<Int32, AutoPtr<IBundle> >::Iterator it = mUserRestrictions.Find((*mUserIds)[i]);
        if (it != mUserRestrictions.End()) {
            b = it->mSecond;
        }
        if (FAILED(mAppOpsService->SetUserRestrictions(b, (*mUserIds)[i]))) {
            Logger::W(LOG_TAG, "Unable to notify AppOpsService of UserRestrictions");
        }
        // } catch (RemoteException e) {
        //     Log.w(LOG_TAG, "Unable to notify AppOpsService of UserRestrictions");
        // }
    }
}

AutoPtr<CUserManagerService> CUserManagerService::GetInstance()
{
    {    AutoLock syncLock(sLock);
        return sInstance;
    }
    return NULL;
}

ECode CUserManagerService::GetUsers(
    /* [in] */ Boolean excludeDying,
    /* [out] */ IList** _users)
{
    VALIDATE_NOT_NULL(_users)
    *_users = NULL;
    FAIL_RETURN(CheckManageUsersPermission(String("query users")))
    {    AutoLock syncLock(mPackagesLock);
        AutoPtr<IList> users;
        CArrayList::New(mUsers.GetSize(), (IList**)&users);
        HashMap<Int32, AutoPtr<IUserInfo> >::Iterator it;
        for (it = mUsers.Begin(); it != mUsers.End(); ++it) {
            AutoPtr<IUserInfo> ui = it->mSecond;
            Boolean partial;
            if (ui->GetPartial(&partial), partial) {
                continue;
            }
            Int32 id;
            ;
            if (!excludeDying ||
                    (ui->GetId(&id), !mRemovingUserIds[id])) {
                users->Add(ui);
            }
        }
        *_users = users;
        REFCOUNT_ADD(*_users)
    }
    return NOERROR;
}

ECode CUserManagerService::GetProfiles(
    /* [in] */ Int32 userId,
    /* [in] */ Boolean enabledOnly,
    /* [out] */ IList** result)
{
    VALIDATE_NOT_NULL(result)
    *result = NULL;

    if (userId != UserHandle::GetCallingUserId()) {
        FAIL_RETURN(CheckManageUsersPermission(
                String("getting profiles related to user ") + StringUtils::ToString(userId)))
    }
    Int64 ident = Binder::ClearCallingIdentity();
    // try {
    {
        AutoLock syncLock(mPackagesLock);
        AutoPtr<IList> list = GetProfilesLocked(userId, enabledOnly);
        *result = list;
        REFCOUNT_ADD(*result)
    }
    // } finally {
    //     Binder.restoreCallingIdentity(ident);
    // }
    Binder::RestoreCallingIdentity(ident);
    return NOERROR;
}

AutoPtr<IList> CUserManagerService::GetProfilesLocked(
    /* [in] */ Int32 userId,
    /* [in] */ Boolean enabledOnly)
{
    AutoPtr<IUserInfo> user = GetUserInfoLocked(userId);
    AutoPtr<IList> users;
    CParcelableList::New(mUsers.GetSize(), (IList**)&users);
    if (user == NULL) {
        // Probably a dying user
        return users;
    }
    HashMap<Int32, AutoPtr<IUserInfo> >::Iterator it;
    for (it = mUsers.Begin(); it != mUsers.End(); ++it) {
        AutoPtr<IUserInfo> profile = it->mSecond;
        if (!IsProfileOf(user, profile)) {
            continue;
        }
        Boolean isEnabled;
        if (enabledOnly && (profile->IsEnabled(&isEnabled), !isEnabled)) {
            continue;
        }
        Int32 id;
        if (profile->GetId(&id), mRemovingUserIds[id]) {
            continue;
        }
        users->Add(profile);
    }
    return users;
}

ECode CUserManagerService::GetProfileParent(
    /* [in] */ Int32 userHandle,
    /* [out] */ IUserInfo** info)
{
    VALIDATE_NOT_NULL(info)
    *info = NULL;

    FAIL_RETURN(CheckManageUsersPermission(String("get the profile parent")))
    {
        AutoLock syncLock(mPackagesLock);
        AutoPtr<IUserInfo> profile = GetUserInfoLocked(userHandle);
        Int32 parentUserId;
        profile->GetProfileGroupId(&parentUserId);
        if (parentUserId == IUserInfo::NO_PROFILE_GROUP_ID) {
            return NOERROR;
        }
        else {
            AutoPtr<IUserInfo> temp = GetUserInfoLocked(parentUserId);
            *info = temp;
            REFCOUNT_ADD(*info)
        }
    }
    return NOERROR;
}

Boolean CUserManagerService::IsProfileOf(
    /* [in] */ IUserInfo* user,
    /* [in] */ IUserInfo* profile)
{
    Int32 userId, profileId;
    user->GetId(&userId);
    profile->GetId(&profileId);
    if (userId != profileId) {
        Int32 userProfileGroupId, profileGroupId;
        return (user->GetProfileGroupId(&userProfileGroupId), userProfileGroupId != IUserInfo::NO_PROFILE_GROUP_ID
                && (profile->GetProfileGroupId(&profileGroupId), userProfileGroupId == profileGroupId));

    }
    return TRUE;
}

ECode CUserManagerService::SetUserEnabled(
    /* [in] */ Int32 userHandle)
{
    FAIL_RETURN(CheckManageUsersPermission(String("enable user")))
    {
        AutoLock syncLock(mPackagesLock);
        AutoPtr<IUserInfo> info = GetUserInfoLocked(userHandle);
        Boolean isEnabled;
        if (info != NULL && (info->IsEnabled(&isEnabled), !isEnabled)) {
            Int32 flags;
            info->GetFlags(&flags);
            flags ^= IUserInfo::FLAG_DISABLED;
            info->SetFlags(flags);
            WriteUserLocked(info);
        }
    }
    return NOERROR;
}

ECode CUserManagerService::GetUserInfo(
    /* [in] */ Int32 userId,
    /* [out] */ IUserInfo** userInfo)
{
    VALIDATE_NOT_NULL(userInfo)
    *userInfo = NULL;

    FAIL_RETURN(CheckManageUsersPermission(String("query user")))
    {
        AutoLock syncLock(mPackagesLock);
        AutoPtr<IUserInfo> user = GetUserInfoLocked(userId);
        *userInfo = user;
        REFCOUNT_ADD(*userInfo)
    }
    return NOERROR;
}

ECode CUserManagerService::IsRestricted(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    {    AutoLock syncLock(mPackagesLock);
        GetUserInfoLocked(UserHandle::GetCallingUserId())->IsRestricted(result);
    }
    return NOERROR;
}

AutoPtr<IUserInfo> CUserManagerService::GetUserInfoLocked(
    /* [in] */ Int32 userId)
{
    AutoPtr<IUserInfo> ui;
    HashMap<Int32, AutoPtr<IUserInfo> >::Iterator it = mUsers.Find(userId);
    if (it != mUsers.End()) {
        ui = it->mSecond;
    }
    // If it is partial and not in the process of being removed, return as unknown user.
    Boolean partial;
    if (ui != NULL && (ui->GetPartial(&partial), partial) && !mRemovingUserIds[userId]) {
        Slogger::W(TAG, "getUserInfo: unknown user #%d", userId);
        return NULL;
    }
    return ui;
}

Boolean CUserManagerService::Exists(
    /* [in] */ Int32 userId)
{
    {
        AutoLock syncLock(mPackagesLock);
        for (Int32 i = 0; i < mUserIds->GetLength(); ++i) {
            if ((*mUserIds)[i] == userId) return TRUE;
        }
    }
    return FALSE;
}

ECode CUserManagerService::SetUserName(
    /* [in] */ Int32 userId,
    /* [in] */ const String& name)
{
    FAIL_RETURN(CheckManageUsersPermission(String("rename users")))
    Boolean changed = FALSE;
    {
        AutoLock syncLock(mPackagesLock);
        AutoPtr<IUserInfo> info;
        HashMap<Int32, AutoPtr<IUserInfo> >::Iterator it = mUsers.Find(userId);
        if (it != mUsers.End()) {
            info = it->mSecond;
        }
        Boolean partial;
        if (info == NULL || (info->GetPartial(&partial), partial)) {
            Slogger::W(TAG, "setUserName: unknown user #%d", userId);
            return NOERROR;
        }
        String infoName;
        if (!name.IsNull() && (info->GetName(&infoName), !name.Equals(infoName))) {
            info->SetName(name);
            WriteUserLocked(info);
            changed = TRUE;
        }
    }
    if (changed) {
        SendUserInfoChangedBroadcast(userId);
    }
    return NOERROR;
}

ECode CUserManagerService::SetUserIcon(
    /* [in] */ Int32 userId,
    /* [in] */ IBitmap* bitmap)
{
    FAIL_RETURN(CheckManageUsersPermission(String("update users")))
    Int64 ident = Binder::ClearCallingIdentity();
    // try {
    {
        AutoLock syncLock(mPackagesLock);
        AutoPtr<IUserInfo> info;
        HashMap<Int32, AutoPtr<IUserInfo> >::Iterator it = mUsers.Find(userId);
        if (it != mUsers.End()) {
            info = it->mSecond;
        }
        Boolean partial;
        if (info == NULL || (info->GetPartial(&partial), partial)) {
            Slogger::W(LOG_TAG, "setUserIcon: unknown user #%d", userId);
            return NOERROR;
        }
        WriteBitmapLocked(info, bitmap);
        WriteUserLocked(info);
    }
    SendUserInfoChangedBroadcast(userId);
    // } finally {
    //     Binder.restoreCallingIdentity(ident);
    // }
    Binder::RestoreCallingIdentity(ident);
    return NOERROR;
}

void CUserManagerService::SendUserInfoChangedBroadcast(
    /* [in] */ Int32 userId)
{
    AutoPtr<IIntent> changedIntent;
    CIntent::New(IIntent::ACTION_USER_INFO_CHANGED, (IIntent**)&changedIntent);
    changedIntent->PutExtra(IIntent::EXTRA_USER_HANDLE, userId);
    changedIntent->AddFlags(IIntent::FLAG_RECEIVER_REGISTERED_ONLY);
    mContext->SendBroadcastAsUser(changedIntent, UserHandle::ALL);
}

ECode CUserManagerService::GetUserIcon(
    /* [in] */ Int32 userId,
    /* [out] */ IBitmap** userIcon)
{
    VALIDATE_NOT_NULL(userIcon)
    *userIcon = NULL;
    {
        AutoLock syncLock(mPackagesLock);
        AutoPtr<IUserInfo> info;
        HashMap<Int32, AutoPtr<IUserInfo> >::Iterator it = mUsers.Find(userId);
        if (it != mUsers.End()) {
            info = it->mSecond;
        }
        Boolean partial;
        if (info == NULL || (info->GetPartial(&partial), partial)) {
            Slogger::W(TAG, "getUserIcon: unknown user #%d", userId);
            return NOERROR;
        }
        AutoPtr<IUserInfo> userInfo;
        it = mUsers.Find(UserHandle::GetCallingUserId());
        if (it != mUsers.End()) {
            userInfo = it->mSecond;
        }
        Int32 callingGroupId;
        userInfo->GetProfileGroupId(&callingGroupId);
        Int32 profileGroupId;
        if (callingGroupId == IUserInfo::NO_PROFILE_GROUP_ID
                || (info->GetProfileGroupId(&profileGroupId), callingGroupId != profileGroupId)) {
            FAIL_RETURN(CheckManageUsersPermission(String("get the icon of a user who is not related")))
        }
        String iconPath;
        info->GetIconPath(&iconPath);
        if (iconPath.IsNull()) {
            return NOERROR;
        }
        AutoPtr<IBitmapFactory> factory;
        CBitmapFactory::AcquireSingleton((IBitmapFactory**)&factory);
        factory->DecodeFile(iconPath, userIcon);
    }
    return NOERROR;
}

ECode CUserManagerService::MakeInitialized(
    /* [in] */ Int32 userId)
{
    FAIL_RETURN(CheckManageUsersPermission(String("makeInitialized")))
    {
        AutoLock syncLock(mPackagesLock);
        AutoPtr<IUserInfo> info;
        HashMap<Int32, AutoPtr<IUserInfo> >::Iterator it = mUsers.Find(userId);
        if (it != mUsers.End()) {
            info = it->mSecond;
        }
        Boolean partial;
        if (info == NULL || (info->GetPartial(&partial), partial)) {
            Slogger::W(TAG, "makeInitialized: unknown user #%d", userId);
        }
        Int32 flags;
        if (info->GetFlags(&flags), (flags & IUserInfo::FLAG_INITIALIZED) == 0) {
            flags |= IUserInfo::FLAG_INITIALIZED;
            info->SetFlags(flags);
            WriteUserLocked(info);
        }
    }
    return NOERROR;
}

void CUserManagerService::InitDefaultGuestRestrictions()
{
    Boolean isEmpty;
    if (mGuestRestrictions->IsEmpty(&isEmpty), isEmpty) {
        mGuestRestrictions->PutBoolean(IUserManager::DISALLOW_OUTGOING_CALLS, TRUE);
        WriteUserListLocked();
    }
}

ECode CUserManagerService::GetDefaultGuestRestrictions(
    /* [out] */ IBundle** result)
{
    VALIDATE_NOT_NULL(result)
    *result = NULL;
    FAIL_RETURN(CheckManageUsersPermission(String("getDefaultGuestRestrictions")))
    {    AutoLock syncLock(mPackagesLock);
        CBundle::New(mGuestRestrictions, result);
    }
    return NOERROR;
}

ECode CUserManagerService::SetDefaultGuestRestrictions(
    /* [in] */ IBundle* restrictions)
{
    FAIL_RETURN(CheckManageUsersPermission(String("setDefaultGuestRestrictions")))
    {    AutoLock syncLock(mPackagesLock);
        mGuestRestrictions->Clear();
        mGuestRestrictions->PutAll(restrictions);
        WriteUserListLocked();
    }
    return NOERROR;
}

ECode CUserManagerService::HasUserRestriction(
    /* [in] */ const String& restrictionKey,
    /* [in] */ Int32 userId,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;
    {    AutoLock syncLock(mPackagesLock);
        AutoPtr<IBundle> restrictions;
        HashMap<Int32, AutoPtr<IBundle> >::Iterator it = mUserRestrictions.Find(userId);
        if (it != mUserRestrictions.End()) {
            restrictions = it->mSecond;
        }
        if (restrictions != NULL) {
            restrictions->GetBoolean(restrictionKey, result);
        }
    }
    return NOERROR;
}

ECode CUserManagerService::GetUserRestrictions(
    /* [in] */ Int32 userHandle,
    /* [out] */ IBundle** bundle)
{
    VALIDATE_NOT_NULL(bundle)
    // checkManageUsersPermission("getUserRestrictions");

    {    AutoLock syncLock(mPackagesLock);
        AutoPtr<IBundle> restrictions;
        HashMap<Int32, AutoPtr<IBundle> >::Iterator it = mUserRestrictions.Find(userHandle);
        if (it != mUserRestrictions.End()) {
            restrictions = it->mSecond;
        }
        if (restrictions != NULL) {
            CBundle::New(restrictions, bundle);
        }
        else {
            CBundle::New(bundle);
        }
    }
    return NOERROR;
}

ECode CUserManagerService::SetUserRestrictions(
    /* [in] */ IBundle* restrictions,
    /* [in] */ Int32 userId)
{
    FAIL_RETURN(CheckManageUsersPermission(String("setUserRestrictions")))
    if (restrictions == NULL) return NOERROR;

    {
        AutoLock syncLock(mPackagesLock);
        AutoPtr<IBundle> bundle;
        HashMap<Int32, AutoPtr<IBundle> >::Iterator it = mUserRestrictions.Find(userId);
        if (it != mUserRestrictions.End()) {
            bundle = it->mSecond;
        }
        assert(bundle);
        bundle->Clear();
        bundle->PutAll(restrictions);
        Int64 token = Binder::ClearCallingIdentity();
        // try {
        if (FAILED(mAppOpsService->SetUserRestrictions(bundle, userId))) {
            Logger::W(LOG_TAG, "Unable to notify AppOpsService of UserRestrictions");
        }
        // } catch (RemoteException e) {
        //     Log.w(LOG_TAG, "Unable to notify AppOpsService of UserRestrictions");
        // } finally {
        //     Binder.restoreCallingIdentity(token);
        // }
        Binder::RestoreCallingIdentity(token);
        AutoPtr<IUserInfo> info;
        HashMap<Int32, AutoPtr<IUserInfo> >::Iterator infoIt = mUsers.Find(userId);
        if (infoIt != mUsers.End()) {
            info = infoIt->mSecond;
        }
        WriteUserLocked(info);
    }
    return NOERROR;
}

Boolean CUserManagerService::IsUserLimitReachedLocked()
{
    Int32 aliveUserCount = 0;
    // Skip over users being removed
    HashMap<Int32, AutoPtr<IUserInfo> >::Iterator it = mUsers.Begin();
    for (; it != mUsers.End(); ++it) {
        AutoPtr<IUserInfo> user = it->mSecond;
        Int32 id;
        user->GetId(&id);
        Boolean isGuest, partial;
        if (!mRemovingUserIds[id]
                && (user->IsGuest(&isGuest), !isGuest) && (user->GetPartial(&partial), !partial)) {
            aliveUserCount++;
        }
    }
    AutoPtr<IUserManagerHelper> helper;
    CUserManagerHelper::AcquireSingleton((IUserManagerHelper**)&helper);
    Int32 mUsers;
    helper->GetMaxSupportedUsers(&mUsers);
    return aliveUserCount >= mUsers;
}

ECode CUserManagerService::CheckManageUsersPermission(
    /* [in] */ const String& message)
{
    const Int32 uid = Binder::GetCallingUid();
    AutoPtr<IActivityManagerHelper> amHelper;
    CActivityManagerHelper::AcquireSingleton((IActivityManagerHelper**)&amHelper);
    Int32 val;
    amHelper->CheckComponentPermission(
            Elastos::Droid::Manifest::permission::MANAGE_USERS,
            uid, -1, TRUE, &val);
    if (uid != IProcess::SYSTEM_UID && uid != 0
            && val != IPackageManager::PERMISSION_GRANTED) {
        Slogger::D(TAG, "You need MANAGE_USERS permission to: ");
        return E_SECURITY_EXCEPTION;
    }
    return NOERROR;
}

void CUserManagerService::WriteBitmapLocked(
    /* [in] */ IUserInfo* info,
    /* [in] */ IBitmap* bitmap)
{
    // try {
    Int32 id;
    info->GetId(&id);
    AutoPtr<IFile> dir;
    CFile::New(mUsersDir, StringUtils::ToString(id), (IFile**)&dir);
    AutoPtr<IFile> file;
    CFile::New(dir, USER_PHOTO_FILENAME, (IFile**)&file);
    Boolean exists = FALSE;
    dir->Exists(&exists);
    if (!exists) {
        Boolean mked = FALSE;
        dir->Mkdir(&mked);
        String dirPath;
        dir->GetPath(&dirPath);
        FileUtils::SetPermissions(
                dirPath,
                FileUtils::sS_IRWXU | FileUtils::sS_IRWXG | FileUtils::sS_IXOTH,
                -1, -1);
    }
    AutoPtr<IFileOutputStream> os;
    CFileOutputStream::New(file, (IFileOutputStream**)&os);
    Boolean result = FALSE;
    if (bitmap->Compress(Elastos::Droid::Graphics::BitmapCompressFormat_PNG, 100,
            IOutputStream::Probe(os), &result), result) {
        String aPath;
        file->GetAbsolutePath(&aPath);
        info->SetIconPath(aPath);
    }
    // try {
    ICloseable::Probe(os)->Close();
    // } catch (IOException ioe) {
    //     // What the ... !
    // }
    // } catch (FileNotFoundException e) {
    //     Slog.w(TAG, "Error setting photo for user ", e);
    // }
}

AutoPtr< ArrayOf<Int32> > CUserManagerService::GetUserIds()
{
    {    AutoLock syncLock(mPackagesLock);
        return mUserIds;
    }
    return NULL;
}

AutoPtr< ArrayOf<Int32> > CUserManagerService::GetUserIdsLPr()
{
    return mUserIds;
}

void CUserManagerService::ReadUserListLocked()
{
    String tag;
    Boolean isExist;
    if (mUserListFile->Exists(&isExist), !isExist) {
        Slogger::E(TAG, "user list file %s does not exist.");
        FallbackToSingleUserLocked();
        return;
    }
    AutoPtr<IAtomicFile> userListFile;
    CAtomicFile::New(mUserListFile, (IAtomicFile**)&userListFile);

//     try {
    Int32 type;
    ECode ec = NOERROR;
    AutoPtr<IXmlPullParser> parser;
    String nullStr;
    AutoPtr<IFileInputStream> fis;
    ec = userListFile->OpenRead((IFileInputStream**)&fis);
    FAIL_GOTO(ec, _EXIT_);

    Xml::NewPullParser((IXmlPullParser**)&parser);
    ec = parser->SetInput(IInputStream::Probe(fis), nullStr);
    FAIL_GOTO(ec, _EXIT_);

    ec = parser->Next(&type);
    FAIL_GOTO(ec, _EXIT_);
    while (type != IXmlPullParser::START_TAG && type != IXmlPullParser::END_DOCUMENT) {
        ec = parser->Next(&type);
        FAIL_GOTO(ec, _EXIT_);
    }

    if (type != IXmlPullParser::START_TAG) {
        Slogger::E(TAG, "Unable to read user list");
        FallbackToSingleUserLocked();
        goto _EXIT_;
    }

    mNextSerialNumber = -1;
    if (parser->GetName(&tag), tag.Equals(TAG_USERS)) {
        String lastSerialNumber;
        parser->GetAttributeValue(nullStr, ATTR_NEXT_SERIAL_NO, &lastSerialNumber);
        if (!lastSerialNumber.IsNull()) {
            mNextSerialNumber = StringUtils::ParseInt32(lastSerialNumber);
        }
        String versionNumber;
        parser->GetAttributeValue(nullStr, ATTR_USER_VERSION, &versionNumber);
        if (!versionNumber.IsNull()) {
            mUserVersion = StringUtils::ParseInt32(versionNumber);
        }
    }

    while (parser->Next(&type), type != IXmlPullParser::END_DOCUMENT) {
        if (type == IXmlPullParser::START_TAG) {
            String name;
            parser->GetName(&name);
            if (name.Equals(TAG_USER)) {
                String id;
                parser->GetAttributeValue(nullStr, ATTR_ID, &id);
                AutoPtr<IUserInfo> user = ReadUserLocked(StringUtils::ParseInt32(id));

                if (user != NULL) {
                    Int32 userId;
                    user->GetId(&userId);
                    mUsers[userId] = user;
                    if (mNextSerialNumber < 0 || mNextSerialNumber <= userId) {
                        mNextSerialNumber = userId + 1;
                    }
                }
            }
            else if (name.Equals(TAG_GUEST_RESTRICTIONS)) {
                mGuestRestrictions->Clear();
                ReadRestrictionsLocked(parser, mGuestRestrictions);
            }
        }
    }
    UpdateUserIdsLocked();
    UpgradeIfNecessaryLock();

_EXIT_:
    if (ec == (ECode)E_IO_EXCEPTION || ec == (ECode)E_XML_PULL_PARSER_EXCEPTION) {
        Slogger::E(LOG_TAG, "Error readUserListLocked , restore default. ec=%08x\n", ec);
        FallbackToSingleUserLocked();
    }
    else if (FAILED(ec)) {
        Slogger::E(LOG_TAG, "Error readUserListLocked , restore default. ec=%08x\n", ec);
        FallbackToSingleUserLocked();
    }

    if (fis != NULL) {
        // try {
        ICloseable::Probe(fis)->Close();
        // } catch (IOException e) {
        // }
    }
//     }
}

void CUserManagerService::UpgradeIfNecessaryLock()
{
    Int32 userVersion = mUserVersion;
    if (userVersion < 1) {
        // Assign a proper name for the owner, if not initialized correctly before
        AutoPtr<IUserInfo> user;
        HashMap<Int32, AutoPtr<IUserInfo> >::Iterator it = mUsers.Find(IUserHandle::USER_OWNER);
        if (it != mUsers.End()) {
            user = it->mSecond;
        }
        assert(user != NULL);
        String temp;
        user->GetName(&temp);
        if (String("Primary").Equals(temp)) {
            AutoPtr<IResources> resources;
            mContext->GetResources((IResources**)&resources);
            resources->GetString(R::string::owner_name, &temp);
            user->SetName(temp);
            WriteUserLocked(user);
        }
        userVersion = 1;
    }

    if (userVersion < 2) {
        // Owner should be marked as initialized
        HashMap<Int32, AutoPtr<IUserInfo> >::Iterator it = mUsers.Find(IUserHandle::USER_OWNER);
        AutoPtr<IUserInfo> user;
        if (it != mUsers.End()) {
            user = it->mSecond;
            Int32 uFlags;
            user->GetFlags(&uFlags);
            if ((uFlags & IUserInfo::FLAG_INITIALIZED) == 0) {
                uFlags |= IUserInfo::FLAG_INITIALIZED;
                user->SetFlags(uFlags);
                WriteUserLocked(user);
            }
            userVersion = 2;
        }
    }

    if (userVersion < 4) {
        userVersion = 4;
    }

    if (userVersion < 5) {
        InitDefaultGuestRestrictions();
        userVersion = 5;
    }

    if (userVersion < USER_VERSION) {
        Slogger::W(TAG, "User version %d didn't upgrade as expected to %d", mUserVersion, USER_VERSION);
    }
    else {
        mUserVersion = userVersion;
        WriteUserListLocked();
    }
}

void CUserManagerService::FallbackToSingleUserLocked()
{
    // Create the primary user
    AutoPtr<IResources> resources;
    mContext->GetResources((IResources**)&resources);
    String valueString;
    resources->GetString(R::string::owner_name, &valueString);
    AutoPtr<IUserInfo> primary;
    CUserInfo::New(IUserHandle::USER_OWNER, valueString, String(NULL),
            IUserInfo::FLAG_ADMIN | IUserInfo::FLAG_PRIMARY | IUserInfo::FLAG_INITIALIZED, (IUserInfo**)&primary);
    mUsers[0] = primary;
    mNextSerialNumber = MIN_USER_ID;
    mUserVersion = USER_VERSION;

    AutoPtr<IBundle> restrictions;
    CBundle::New((IBundle**)&restrictions);
    mUserRestrictions[IUserHandle::USER_OWNER] = restrictions;

    UpdateUserIdsLocked();
    InitDefaultGuestRestrictions();

    WriteUserListLocked();
    WriteUserLocked(primary);
}

void CUserManagerService::WriteUserLocked(
    /* [in] */ IUserInfo* userInfo)
{
    AutoPtr<IFileOutputStream> fos;
    Int32 id;
    userInfo->GetId(&id);
    AutoPtr<IFile> baseFile;
    CFile::New(mUsersDir, StringUtils::ToString(id) + XML_SUFFIX, (IFile**)&baseFile);
    AutoPtr<IAtomicFile> userFile;
    CAtomicFile::New(baseFile, (IAtomicFile**)&userFile);
//     try {
    userFile->StartWrite((IFileOutputStream**)&fos);
    AutoPtr<IBufferedOutputStream> bos;
    CBufferedOutputStream::New(IOutputStream::Probe(fos), (IBufferedOutputStream**)&bos);

    // XmlSerializer serializer = XmlUtils.serializerInstance();
    AutoPtr<IXmlSerializer> serializer;
    CFastXmlSerializer::New((IXmlSerializer**)&serializer);
    serializer->SetOutput(IOutputStream::Probe(bos), String("utf-8"));
    serializer->StartDocument(String(NULL), TRUE);
    serializer->SetFeature(String("http://xmlpull.org/v1/doc/features.html#indent-output"), TRUE);

    serializer->WriteStartTag(String(NULL), TAG_USER);
    userInfo->GetId(&id);
    serializer->WriteAttribute(String(NULL), ATTR_ID, StringUtils::ToString(id));
    Int32 serialNumber;
    userInfo->GetSerialNumber(&serialNumber);
    serializer->WriteAttribute(String(NULL), ATTR_SERIAL_NO, StringUtils::ToString(serialNumber));
    Int32 flags;
    userInfo->GetFlags(&flags);
    serializer->WriteAttribute(String(NULL), ATTR_FLAGS, StringUtils::ToString(flags));
    Int64 creationTime;
    userInfo->GetCreationTime(&creationTime);
    serializer->WriteAttribute(String(NULL), ATTR_CREATION_TIME, StringUtils::ToString(creationTime));
    Int64 lastLoggedInTime;
    userInfo->GetLastLoggedInTime(&lastLoggedInTime);
    serializer->WriteAttribute(String(NULL), ATTR_LAST_LOGGED_IN_TIME,
            StringUtils::ToString(lastLoggedInTime));

    AutoPtr<RestrictionsPinState> pinState;
    HashMap<Int32, AutoPtr<RestrictionsPinState> >::Iterator it = mRestrictionsPinStates.Find(id);
    if (it != mRestrictionsPinStates.End()) {
        pinState = it->mSecond;
    }
    if (pinState != NULL) {
        if (pinState->mSalt != 0) {
            serializer->WriteAttribute(String(NULL), ATTR_SALT, StringUtils::ToString(pinState->mSalt));
        }
        if (pinState->mPinHash != NULL) {
            serializer->WriteAttribute(String(NULL), ATTR_PIN_HASH, pinState->mPinHash);
        }
        if (pinState->mFailedAttempts != 0) {
            serializer->WriteAttribute(String(NULL), ATTR_FAILED_ATTEMPTS,
                    StringUtils::ToString(pinState->mFailedAttempts));
            serializer->WriteAttribute(String(NULL), ATTR_LAST_RETRY_MS,
                    StringUtils::ToString(pinState->mLastAttemptTime));
        }
    }

    String iconPath;
    userInfo->GetIconPath(&iconPath);
    if (!iconPath.IsNull()) {
        serializer->WriteAttribute(String(NULL),  ATTR_ICON_PATH, iconPath);
    }
    Boolean partial;
    userInfo->GetPartial(&partial);
    if (partial) {
        serializer->WriteAttribute(String(NULL), ATTR_PARTIAL, String("true"));
    }
    Boolean guestToRemove;
    if (userInfo->GetGuestToRemove(&guestToRemove), guestToRemove) {
        serializer->WriteAttribute(String(NULL), ATTR_GUEST_TO_REMOVE, String("true"));
    }
    Int32 profileGroupId;
    if (userInfo->GetProfileGroupId(&profileGroupId), profileGroupId != IUserInfo::NO_PROFILE_GROUP_ID) {
        serializer->WriteAttribute(String(NULL), ATTR_PROFILE_GROUP_ID,
                StringUtils::ToString(profileGroupId));
    }

    serializer->WriteStartTag(String(NULL), TAG_NAME);
    String name;
    userInfo->GetName(&name);
    serializer->WriteText(name);
    serializer->WriteEndTag(String(NULL), TAG_NAME);
    AutoPtr<IBundle> restrictions;
    HashMap<Int32, AutoPtr<IBundle> >::Iterator restrictionsIt = mUserRestrictions.Find(id);
    if (restrictionsIt != mUserRestrictions.End()) {
        restrictions = restrictionsIt->mSecond;
    }
    if (restrictions != NULL) {
        WriteRestrictionsLocked(serializer, restrictions);
    }
    serializer->WriteEndTag(String(NULL), TAG_USER);

    serializer->EndDocument();
    userFile->FinishWrite(fos);
//     } catch (Exception ioe) {
//         Slog.e(TAG, "Error writing user info " + userInfo.id + "\n" + ioe);
//         userFile.failWrite(fos);
//     }
}

void CUserManagerService::WriteUserListLocked()
{
    String nullStr;
    AutoPtr<IFileOutputStream> fos;
    AutoPtr<IAtomicFile> userListFile;
    CAtomicFile::New(mUserListFile, (IAtomicFile**)&userListFile);
    // try {
    userListFile->StartWrite((IFileOutputStream**)&fos);
    AutoPtr<IBufferedOutputStream> bos;
    CBufferedOutputStream::New(IOutputStream::Probe(fos), (IBufferedOutputStream**)&bos);

    // XmlSerializer serializer = XmlUtils.serializerInstance();
    AutoPtr<IXmlSerializer> serializer;
    CFastXmlSerializer::New((IXmlSerializer**)&serializer);
    serializer->SetOutput(IOutputStream::Probe(bos), String("utf-8"));
    serializer->StartDocument(nullStr, TRUE);
    serializer->SetFeature(String("http://xmlpull.org/v1/doc/features.html#indent-output"), TRUE);

    serializer->WriteStartTag(nullStr, TAG_USERS);
    serializer->WriteAttribute(nullStr, ATTR_NEXT_SERIAL_NO, StringUtils::ToString(mNextSerialNumber));
    serializer->WriteAttribute(nullStr, ATTR_USER_VERSION, StringUtils::ToString(mUserVersion));

    serializer->WriteStartTag(nullStr, TAG_GUEST_RESTRICTIONS);
    WriteRestrictionsLocked(serializer, mGuestRestrictions);
    serializer->WriteEndTag(nullStr, TAG_GUEST_RESTRICTIONS);

    HashMap<Int32, AutoPtr<IUserInfo> >::Iterator it;
    for (it = mUsers.Begin(); it != mUsers.End(); ++it) {
        AutoPtr<IUserInfo> user = it->mSecond;
        Int32 id;
        user->GetId(&id);
        serializer->WriteStartTag(nullStr, TAG_USER);
        serializer->WriteAttribute(nullStr, ATTR_ID, StringUtils::ToString(id));
        serializer->WriteEndTag(nullStr, TAG_USER);
    }

    serializer->WriteEndTag(nullStr, TAG_USERS);

    serializer->EndDocument();
    userListFile->FinishWrite(fos);
    // } catch (Exception e) {
    //     userListFile.failWrite(fos);
    //     Slog.e(TAG, "Error writing user list");
    // }
}

void CUserManagerService::WriteRestrictionsLocked(
    /* [in] */ IXmlSerializer* serializer,
    /* [in] */ IBundle* restrictions)
{
    serializer->WriteStartTag(String(NULL), TAG_RESTRICTIONS);
    WriteBoolean(serializer, restrictions, IUserManager::DISALLOW_CONFIG_WIFI);
    WriteBoolean(serializer, restrictions, IUserManager::DISALLOW_MODIFY_ACCOUNTS);
    WriteBoolean(serializer, restrictions, IUserManager::DISALLOW_INSTALL_APPS);
    WriteBoolean(serializer, restrictions, IUserManager::DISALLOW_UNINSTALL_APPS);
    WriteBoolean(serializer, restrictions, IUserManager::DISALLOW_SHARE_LOCATION);
    WriteBoolean(serializer, restrictions, IUserManager::DISALLOW_INSTALL_UNKNOWN_SOURCES);
    WriteBoolean(serializer, restrictions, IUserManager::DISALLOW_CONFIG_BLUETOOTH);
    WriteBoolean(serializer, restrictions, IUserManager::DISALLOW_USB_FILE_TRANSFER);
    WriteBoolean(serializer, restrictions, IUserManager::DISALLOW_CONFIG_CREDENTIALS);
    WriteBoolean(serializer, restrictions, IUserManager::DISALLOW_REMOVE_USER);
    WriteBoolean(serializer, restrictions, IUserManager::DISALLOW_DEBUGGING_FEATURES);
    WriteBoolean(serializer, restrictions, IUserManager::DISALLOW_CONFIG_VPN);
    WriteBoolean(serializer, restrictions, IUserManager::DISALLOW_CONFIG_TETHERING);
    WriteBoolean(serializer, restrictions, IUserManager::DISALLOW_FACTORY_RESET);
    WriteBoolean(serializer, restrictions, IUserManager::DISALLOW_ADD_USER);
    WriteBoolean(serializer, restrictions, IUserManager::ENSURE_VERIFY_APPS);
    WriteBoolean(serializer, restrictions, IUserManager::DISALLOW_CONFIG_CELL_BROADCASTS);
    WriteBoolean(serializer, restrictions, IUserManager::DISALLOW_CONFIG_MOBILE_NETWORKS);
    WriteBoolean(serializer, restrictions, IUserManager::DISALLOW_APPS_CONTROL);
    WriteBoolean(serializer, restrictions, IUserManager::DISALLOW_MOUNT_PHYSICAL_MEDIA);
    WriteBoolean(serializer, restrictions, IUserManager::DISALLOW_UNMUTE_MICROPHONE);
    WriteBoolean(serializer, restrictions, IUserManager::DISALLOW_ADJUST_VOLUME);
    WriteBoolean(serializer, restrictions, IUserManager::DISALLOW_OUTGOING_CALLS);
    WriteBoolean(serializer, restrictions, IUserManager::DISALLOW_SMS);
    WriteBoolean(serializer, restrictions, IUserManager::DISALLOW_CREATE_WINDOWS);
    WriteBoolean(serializer, restrictions, IUserManager::DISALLOW_CROSS_PROFILE_COPY_PASTE);
    WriteBoolean(serializer, restrictions, IUserManager::DISALLOW_OUTGOING_BEAM);
    serializer->WriteEndTag(String(NULL), TAG_RESTRICTIONS);
}

AutoPtr<IUserInfo> CUserManagerService::ReadUserLocked(
    /* [in] */ Int32 id)
{
    Int32 flags = 0;
    Int32 serialNumber = id;
    String tag;
    String name;
    String iconPath;
    Int64 creationTime = 0ll;
    Int64 lastLoggedInTime = 0ll;
    Int64 salt = 0L;
    String pinHash(NULL);
    Int32 failedAttempts = 0;
    Int32 profileGroupId = IUserInfo::NO_PROFILE_GROUP_ID;
    Int64 lastAttemptTime = 0L;
    Boolean partial = FALSE;
    Boolean guestToRemove = FALSE;
    AutoPtr<IBundle> restrictions;
    CBundle::New((IBundle**)&restrictions);

    AutoPtr<IUserInfo> userInfo;
    AutoPtr<IFileInputStream> fis;
//     try {
    AutoPtr<IFile> baseFile;
    CFile::New(mUsersDir, StringUtils::ToString(id) + XML_SUFFIX, (IFile**)&baseFile);
    AutoPtr<IAtomicFile> userFile;
    CAtomicFile::New(baseFile, (IAtomicFile**)&userFile);
    userFile->OpenRead((IFileInputStream**)&fis);
    AutoPtr<IXmlPullParser> parser;
    Xml::NewPullParser((IXmlPullParser**)&parser);
    parser->SetInput(IInputStream::Probe(fis), String(NULL));
    Int32 type;
    while ((parser->Next(&type), type) != IXmlPullParser::START_TAG
            && type != IXmlPullParser::END_DOCUMENT) {
        ;
    }

    if (type != IXmlPullParser::START_TAG) {
        Slogger::E(TAG, "Unable to read user %d", id);
        userInfo = NULL;
        goto exit;
    }

    if (type == IXmlPullParser::START_TAG && (parser->GetName(&tag), tag.Equals(TAG_USER))) {
        Int32 storedId = ReadInt32Attribute(parser, ATTR_ID, -1);
        if (storedId != id) {
            Slogger::E(TAG, "User id does not match the file name");
            userInfo = NULL;
            goto exit;
        }
        serialNumber = ReadInt32Attribute(parser, ATTR_SERIAL_NO, id);
        flags = ReadInt32Attribute(parser, ATTR_FLAGS, 0);
        parser->GetAttributeValue(String(NULL), ATTR_ICON_PATH, &iconPath);
        creationTime = ReadInt64Attribute(parser, ATTR_CREATION_TIME, 0);
        lastLoggedInTime = ReadInt64Attribute(parser, ATTR_LAST_LOGGED_IN_TIME, 0);
        salt = ReadInt64Attribute(parser, ATTR_SALT, 0L);
        parser->GetAttributeValue(String(NULL), ATTR_PIN_HASH, &pinHash);
        failedAttempts = ReadInt32Attribute(parser, ATTR_FAILED_ATTEMPTS, 0);
        lastAttemptTime = ReadInt64Attribute(parser, ATTR_LAST_RETRY_MS, 0L);
        profileGroupId = ReadInt32Attribute(parser, ATTR_PROFILE_GROUP_ID, IUserInfo::NO_PROFILE_GROUP_ID);
        if (profileGroupId == IUserInfo::NO_PROFILE_GROUP_ID) {
            // This attribute was added and renamed during development of L.
            // TODO Remove upgrade path by 1st May 2014
            profileGroupId = ReadInt32Attribute(parser, String("relatedGroupId"),
                    IUserInfo::NO_PROFILE_GROUP_ID);
        }
        String valueString;
        parser->GetAttributeValue(String(NULL), ATTR_PARTIAL, &valueString);
        if (valueString.Equals("true")) {
            partial = TRUE;
        }
        parser->GetAttributeValue(String(NULL), ATTR_GUEST_TO_REMOVE, &valueString);
        if (valueString.Equals("true")) {
            guestToRemove = TRUE;
        }

        Int32 outerDepth;
        parser->GetDepth(&outerDepth);
        Int32 type, depth;
        while ((parser->Next(&type), type != IXmlPullParser::END_DOCUMENT)
               && (type != IXmlPullParser::END_TAG || (parser->GetDepth(&depth), depth > outerDepth))) {
            if (type == IXmlPullParser::END_TAG || type == IXmlPullParser::TEXT) {
                continue;
            }
            String tag;
            parser->GetName(&tag);
            if (TAG_NAME.Equals(tag)) {
                parser->Next(&type);
                if (type == IXmlPullParser::TEXT) {
                    parser->GetText(&name);
                }
            }
            else if (TAG_RESTRICTIONS.Equals(tag)) {
                ReadRestrictionsLocked(parser, restrictions);
            }
        }
    }

    CUserInfo::New(id, name, iconPath, flags, (IUserInfo**)&userInfo);
    userInfo->SetSerialNumber(serialNumber);
    userInfo->SetCreationTime(creationTime);
    userInfo->SetLastLoggedInTime(lastLoggedInTime);
    userInfo->SetPartial(partial);
    userInfo->SetGuestToRemove(guestToRemove);
    userInfo->SetProfileGroupId(profileGroupId);
    mUserRestrictions[id] = restrictions;
    if (salt != 0L) {
        AutoPtr<RestrictionsPinState> pinState;
        HashMap<Int32, AutoPtr<RestrictionsPinState> >::Iterator pinStateIt = mRestrictionsPinStates.Find(id);
        if (pinStateIt != mRestrictionsPinStates.End()) {
            pinState = pinStateIt->mSecond;
        }
        if (pinState == NULL) {
            pinState = new RestrictionsPinState();
            mRestrictionsPinStates[id] = pinState;
        }
        pinState->mSalt = salt;
        pinState->mPinHash = pinHash;
        pinState->mFailedAttempts = failedAttempts;
        pinState->mLastAttemptTime = lastAttemptTime;
    }

//     } catch (IOException ioe) {
//     } catch (XmlPullParserException pe) {
//     } finally {
exit:
    if (fis != NULL) {
        // try {
        ICloseable::Probe(fis)->Close();
        // } catch (IOException e) {
        // }
    }
//     }
    return userInfo;
}

void CUserManagerService::ReadRestrictionsLocked(
    /* [in] */ IXmlPullParser* parser,
    /* [in] */ IBundle* restrictions)
{
    ReadBoolean(parser, restrictions, IUserManager::DISALLOW_CONFIG_WIFI);
    ReadBoolean(parser, restrictions, IUserManager::DISALLOW_MODIFY_ACCOUNTS);
    ReadBoolean(parser, restrictions, IUserManager::DISALLOW_INSTALL_APPS);
    ReadBoolean(parser, restrictions, IUserManager::DISALLOW_UNINSTALL_APPS);
    ReadBoolean(parser, restrictions, IUserManager::DISALLOW_SHARE_LOCATION);
    ReadBoolean(parser, restrictions, IUserManager::DISALLOW_INSTALL_UNKNOWN_SOURCES);
    ReadBoolean(parser, restrictions, IUserManager::DISALLOW_CONFIG_BLUETOOTH);
    ReadBoolean(parser, restrictions, IUserManager::DISALLOW_USB_FILE_TRANSFER);
    ReadBoolean(parser, restrictions, IUserManager::DISALLOW_CONFIG_CREDENTIALS);
    ReadBoolean(parser, restrictions, IUserManager::DISALLOW_REMOVE_USER);
    ReadBoolean(parser, restrictions, IUserManager::DISALLOW_DEBUGGING_FEATURES);
    ReadBoolean(parser, restrictions, IUserManager::DISALLOW_CONFIG_VPN);
    ReadBoolean(parser, restrictions, IUserManager::DISALLOW_CONFIG_TETHERING);
    ReadBoolean(parser, restrictions, IUserManager::DISALLOW_FACTORY_RESET);
    ReadBoolean(parser, restrictions, IUserManager::DISALLOW_ADD_USER);
    ReadBoolean(parser, restrictions, IUserManager::ENSURE_VERIFY_APPS);
    ReadBoolean(parser, restrictions, IUserManager::DISALLOW_CONFIG_CELL_BROADCASTS);
    ReadBoolean(parser, restrictions, IUserManager::DISALLOW_CONFIG_MOBILE_NETWORKS);
    ReadBoolean(parser, restrictions, IUserManager::DISALLOW_APPS_CONTROL);
    ReadBoolean(parser, restrictions, IUserManager::DISALLOW_MOUNT_PHYSICAL_MEDIA);
    ReadBoolean(parser, restrictions, IUserManager::DISALLOW_UNMUTE_MICROPHONE);
    ReadBoolean(parser, restrictions, IUserManager::DISALLOW_ADJUST_VOLUME);
    ReadBoolean(parser, restrictions, IUserManager::DISALLOW_OUTGOING_CALLS);
    ReadBoolean(parser, restrictions, IUserManager::DISALLOW_SMS);
    ReadBoolean(parser, restrictions, IUserManager::DISALLOW_CREATE_WINDOWS);
    ReadBoolean(parser, restrictions, IUserManager::DISALLOW_CROSS_PROFILE_COPY_PASTE);
    ReadBoolean(parser, restrictions, IUserManager::DISALLOW_OUTGOING_BEAM);
}

void CUserManagerService::ReadBoolean(
    /* [in] */ IXmlPullParser* parser,
    /* [in] */ IBundle* restrictions,
    /* [in] */ const String& restrictionKey)
{
    String value;
    parser->GetAttributeValue(String(NULL), restrictionKey, &value);
    if (!value.IsNull()) {
        restrictions->PutBoolean(restrictionKey, StringUtils::ParseBoolean(value));
    }
}

void CUserManagerService::WriteBoolean(
    /* [in] */ IXmlSerializer* xml,
    /* [in] */ IBundle* restrictions,
    /* [in] */ const String& restrictionKey)
{
    Boolean contains;
    if (restrictions->ContainsKey(restrictionKey, &contains), contains) {
        Boolean value;
        restrictions->GetBoolean(restrictionKey, &value);
        xml->WriteAttribute(String(NULL), restrictionKey, StringUtils::ToString(value));
    }
}

Int32 CUserManagerService::ReadInt32Attribute(
    /* [in] */ IXmlPullParser* parser,
    /* [in] */ const String& attr,
    /* [in] */ Int32 defaultValue)
{
    String valueString;
    parser->GetAttributeValue(String(NULL), attr, &valueString);
    if (valueString.IsNull()) return defaultValue;
    // try {
    return StringUtils::ParseInt32(valueString);
    // } catch (NumberFormatException nfe) {
    //     return defaultValue;
    // }
}

Int64 CUserManagerService::ReadInt64Attribute(
    /* [in] */ IXmlPullParser* parser,
    /* [in] */ const String& attr,
    /* [in] */ Int64 defaultValue)
{
    String valueString;
    parser->GetAttributeValue(String(NULL), attr, &valueString);
    if (valueString.IsNull()) return defaultValue;
    // try {
    return StringUtils::ParseInt64(valueString);
    // } catch (NumberFormatException nfe) {
    //     return defaultValue;
    // }
}

Boolean CUserManagerService::IsPackageInstalled(
    /* [in] */ const String& pkg,
    /* [in] */ Int32 userId)
{
    AutoPtr<IApplicationInfo> info;
    mPm->GetApplicationInfo(pkg, IPackageManager::GET_UNINSTALLED_PACKAGES,
            userId, (IApplicationInfo**)&info);
    Int32 flags;
    if (info == NULL || (info->GetFlags(&flags), (flags & IApplicationInfo::FLAG_INSTALLED) == 0)) {
        return FALSE;
    }
    return TRUE;
}

void CUserManagerService::CleanAppRestrictions(
    /* [in] */ Int32 userId)
{
    {    AutoLock syncLock(mPackagesLock);
        AutoPtr<IFile> dir = Environment::GetUserSystemDirectory(userId);
        AutoPtr<ArrayOf<String> > files;
        dir->List((ArrayOf<String>**)&files);
        if (files == NULL) return;
        for (Int32 i = 0; i < files->GetLength(); ++i) {
            String fileName = (*files)[i];
            if (fileName.StartWith(RESTRICTIONS_FILE_PREFIX)) {
                AutoPtr<IFile> resFile;
                CFile::New(dir, fileName, (IFile**)&resFile);
                Boolean exists;
                if (resFile->Exists(&exists), exists) {
                    resFile->Delete();
                }
            }
        }
    }
}

void CUserManagerService::CleanAppRestrictionsForPackage(
    /* [in] */ const String& pkg,
    /* [in] */ Int32 userId)
{
    {    AutoLock syncLock(mPackagesLock);
        AutoPtr<IFile> dir = Environment::GetUserSystemDirectory(userId);
        AutoPtr<IFile> resFile;
        CFile::New(dir, PackageToRestrictionsFileName(pkg), (IFile**)&resFile);
        Boolean exists;
        if (resFile->Exists(&exists), exists) {
            resFile->Delete();
        }
    }
}

ECode CUserManagerService::CreateProfileForUser(
    /* [in] */ const String& name,
    /* [in] */ Int32 flags,
    /* [in] */ Int32 userId,
    /* [out] */ IUserInfo** userInfo)
{
    VALIDATE_NOT_NULL(userInfo)
    *userInfo = NULL;

    FAIL_RETURN(CheckManageUsersPermission(String("Only the system can create users")))
    if (userId != IUserHandle::USER_OWNER) {
        Slogger::W(LOG_TAG, "Only user owner can have profiles");
        return NOERROR;
    }
    AutoPtr<IUserInfo> temp = CreateUserInternal(name, flags, userId);
    *userInfo = temp;
    REFCOUNT_ADD(*userInfo)
    return NOERROR;
}

ECode CUserManagerService::CreateUser(
    /* [in] */ const String& name,
    /* [in] */ Int32 flags,
    /* [out] */ IUserInfo** uInfo)
{
    VALIDATE_NOT_NULL(uInfo)
    *uInfo = NULL;
    FAIL_RETURN(CheckManageUsersPermission(String("Only the system can create users")))
    AutoPtr<IUserInfo> temp = CreateUserInternal(name, flags, IUserHandle::USER_NULL);
    *uInfo = temp;
    REFCOUNT_ADD(*uInfo)
    return NOERROR;
}

AutoPtr<IUserInfo> CUserManagerService::CreateUserInternal(
    /* [in] */ const String& name,
    /* [in] */ Int32 flags,
    /* [in] */ Int32 parentId)
{
    AutoPtr<IBundle> b;
    GetUserRestrictions(UserHandle::GetCallingUserId(), (IBundle**)&b);
    Boolean value;
    if (b->GetBoolean(IUserManager::DISALLOW_ADD_USER, FALSE, &value), value) {
        Logger::W(LOG_TAG, "Cannot add user. DISALLOW_ADD_USER is enabled.");
        return NULL;
    }
    Boolean isGuest = (flags & IUserInfo::FLAG_GUEST) != 0;
    Int64 ident = Binder::ClearCallingIdentity();
    AutoPtr<IUserInfo> userInfo;
    // try {
    {    AutoLock syncLock(mInstallLock);
        {    AutoLock syncLock(mPackagesLock);
            AutoPtr<IUserInfo> parent;
            if (parentId != IUserHandle::USER_NULL) {
                parent = GetUserInfoLocked(parentId);
                if (parent == NULL) return NULL;
            }
            // If we're not adding a guest user and the limit has been reached,
            // cannot add a user.
            if (!isGuest && IsUserLimitReachedLocked()) {
                return NULL;
            }
            // If we're adding a guest and there already exists one, bail.
            if (isGuest && FindCurrentGuestUserLocked() != NULL) {
                return NULL;
            }
            // Limit number of managed profiles that can be created
            if ((flags & IUserInfo::FLAG_MANAGED_PROFILE) != 0
                    && NumberOfUsersOfTypeLocked(IUserInfo::FLAG_MANAGED_PROFILE, TRUE) >= MAX_MANAGED_PROFILES) {
                return NULL;
            }
            Int32 userId = GetNextAvailableIdLocked();
            CUserInfo::New(userId, name, String(NULL), flags, (IUserInfo**)&userInfo);
            AutoPtr<IFile> userPath;
            CFile::New(mBaseUserPath, StringUtils::ToString(userId), (IFile**)&userPath);
            userInfo->SetSerialNumber(mNextSerialNumber++);
            AutoPtr<ISystem> system;
            CSystem::AcquireSingleton((ISystem**)&system);
            Int64 now;
            system->GetCurrentTimeMillis(&now);
            userInfo->SetCreationTime((now > EPOCH_PLUS_30_YEARS) ? now : 0);
            userInfo->SetPartial(TRUE);
            Int32 id;
            userInfo->GetId(&id);
            AutoPtr<IFile> usDir = Environment::GetUserSystemDirectory(id);
            Boolean mked;
            usDir->Mkdirs(&mked);
            mUsers[userId] = userInfo;
            WriteUserListLocked();
            if (parent != NULL) {
                Int32 profileGroupId;
                parent->GetProfileGroupId(&profileGroupId);
                if (profileGroupId == IUserInfo::NO_PROFILE_GROUP_ID) {
                    Int32 id;
                    parent->GetId(&id);
                    parent->SetProfileGroupId(id);
                    WriteUserLocked(parent);
                }
                userInfo->SetProfileGroupId(profileGroupId);
            }
            WriteUserLocked(userInfo);
            mPm->CreateNewUserLILPw(userId, userPath);
            userInfo->SetPartial(FALSE);
            WriteUserLocked(userInfo);
            UpdateUserIdsLocked();
            AutoPtr<IBundle> restrictions;
            CBundle::New((IBundle**)&restrictions);
            mUserRestrictions[userId] = restrictions;
        }
    }
    if (userInfo != NULL) {
        AutoPtr<IIntent> addedIntent;
        CIntent::New(IIntent::ACTION_USER_ADDED, (IIntent**)&addedIntent);
        Int32 id;
        userInfo->GetId(&id);
        addedIntent->PutExtra(IIntent::EXTRA_USER_HANDLE, id);
        mContext->SendBroadcastAsUser(addedIntent, UserHandle::ALL, Elastos::Droid::Manifest::permission::MANAGE_USERS);
    }
    // } finally {
    Binder::RestoreCallingIdentity(ident);
    // }
    return userInfo;
}

Int32 CUserManagerService::NumberOfUsersOfTypeLocked(
    /* [in] */ Int32 flags,
    /* [in] */ Boolean excludeDying)
{
    Int32 count = 0;
    HashMap<Int32, AutoPtr<IUserInfo> >::Iterator it = mUsers.Begin();
    for (; it != mUsers.End(); ++it) {
        AutoPtr<IUserInfo> user = it->mSecond;
        Int32 id;
        if (!excludeDying || (user->GetId(&id), !mRemovingUserIds[id])) {
            Int32 userFlags;
            if ((user->GetFlags(&userFlags) & flags) != 0) {
                count++;
            }
        }
    }
    return count;
}

AutoPtr<IUserInfo> CUserManagerService::FindCurrentGuestUserLocked()
{
    HashMap<Int32, AutoPtr<IUserInfo> >::Iterator it = mUsers.Begin();
    for (; it != mUsers.End(); ++it) {
        AutoPtr<IUserInfo> user = it->mSecond;
        Boolean isGuest, guestToRemove;
        Int32 id;
        if ((user->IsGuest(&isGuest), isGuest) &&
                (user->GetGuestToRemove(&guestToRemove), !guestToRemove) &&
                        (user->GetId(&id), !mRemovingUserIds[id])) {
            return user;
        }
    }
    return NULL;
}

ECode CUserManagerService::MarkGuestForDeletion(
    /* [in] */ Int32 userHandle,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;
    FAIL_RETURN(CheckManageUsersPermission(String("Only the system can remove users")))
    AutoPtr<IBundle> b;
    GetUserRestrictions(UserHandle::GetCallingUserId(), (IBundle**)&b);
    Boolean value;
    if (b->GetBoolean(IUserManager::DISALLOW_REMOVE_USER, FALSE, &value), value) {
        Logger::W(LOG_TAG, "Cannot remove user. DISALLOW_REMOVE_USER is enabled.");
        return NOERROR;
    }

    Int64 ident = Binder::ClearCallingIdentity();
    // try {
    AutoPtr<IUserInfo> user;
    {    AutoLock syncLock(mPackagesLock);
        HashMap<Int32, AutoPtr<IUserInfo> >::Iterator it = mUsers.Find(userHandle);
        if (it != mUsers.End()) {
            user = it->mSecond;
        }
        if (userHandle == 0 || user == NULL || mRemovingUserIds[userHandle]) {
            return FALSE;
        }
        Boolean isGuest;
        if (user->IsGuest(&isGuest), !isGuest) {
            return FALSE;
        }
        // We set this to a guest user that is to be removed. This is a temporary state
        // where we are allowed to add new Guest users, even if this one is still not
        // removed. This user will still show up in getUserInfo() calls.
        // If we don't get around to removing this Guest user, it will be purged on next
        // startup.
        user->SetGuestToRemove(TRUE);
        // Mark it as disabled, so that it isn't returned any more when
        // profiles are queried.
        Int32 flags;
        user->GetFlags(&flags);
        flags |= IUserInfo::FLAG_DISABLED;
        user->SetFlags(flags);
        WriteUserLocked(user);
    }
    // } finally {
    //     Binder.restoreCallingIdentity(ident);
    // }
    Binder::RestoreCallingIdentity(ident);
    *result = TRUE;
    return NOERROR;
}

ECode CUserManagerService::RemoveUser(
    /* [in] */ Int32 userHandle,
    /* [out] */ Boolean* succeeded)
{
    VALIDATE_NOT_NULL(succeeded)
    *succeeded = FALSE;
    FAIL_RETURN(CheckManageUsersPermission(String("Only the system can remove users")))
    AutoPtr<IBundle> b;
    GetUserRestrictions(UserHandle::GetCallingUserId(), (IBundle**)&b);
    Boolean value;
    if (b->GetBoolean(IUserManager::DISALLOW_REMOVE_USER, FALSE, &value), value) {
        Logger::W(LOG_TAG, "Cannot remove user. DISALLOW_REMOVE_USER is enabled.");
        return NOERROR;
    }

    Int64 ident = Binder::ClearCallingIdentity();
    // try {
    AutoPtr<IUserInfo> user;
    {    AutoLock syncLock(mPackagesLock);
        HashMap<Int32, AutoPtr<IUserInfo> >::Iterator it = mUsers.Find(userHandle);
        if (it != mUsers.End()) {
            user = it->mSecond;
        }
        if (userHandle == 0 || user == NULL || mRemovingUserIds[userHandle]) {
            return NOERROR;
        }
        mRemovingUserIds[userHandle] = TRUE;
        // try {
        if (FAILED(mAppOpsService->RemoveUser(userHandle))) {
            Logger::W(LOG_TAG, "Unable to notify AppOpsService of removing user");
        }
        // } catch (RemoteException e) {
        //     Log.w(LOG_TAG, "Unable to notify AppOpsService of removing user", e);
        // }
        // Set this to a partially created user, so that the user will be purged
        // on next startup, in case the runtime stops now before stopping and
        // removing the user completely.
        user->SetPartial(TRUE);
        // Mark it as disabled, so that it isn't returned any more when
        // profiles are queried.
        Int32 flags;
        user->GetFlags(&flags);
        flags |= IUserInfo::FLAG_DISABLED;
        user->SetFlags(flags);
        WriteUserLocked(user);
    }

    Int32 profileGroupId;
    Boolean isManagedProfile;
    if (user->GetProfileGroupId(&profileGroupId), profileGroupId != IUserInfo::NO_PROFILE_GROUP_ID
            && (user->IsManagedProfile(&isManagedProfile), isManagedProfile)) {
        // Send broadcast to notify system that the user removed was a
        // managed user.
        Int32 id;
        user->GetId(&id);
        SendProfileRemovedBroadcast(profileGroupId, id);
    }

    if (DBG) Slogger::I(LOG_TAG, "Stopping user %d", userHandle);
    Int32 res;
    // try {
    AutoPtr<IStopUserCallback> callback;
    CUserStopUserCallback::New(this, (IStopUserCallback**)&callback);
    if (FAILED(ActivityManagerNative::GetDefault()->StopUser(userHandle, callback, &res))) {
        *succeeded = FALSE;
        Binder::RestoreCallingIdentity(ident);
        return NOERROR;
    }
    // } catch (RemoteException e) {
    //     return false;
    // }
    *succeeded = res == IActivityManager::USER_OP_SUCCESS;
    // } finally {
    //     Binder.restoreCallingIdentity(ident);
    // }
    Binder::RestoreCallingIdentity(ident);
    return NOERROR;
}

void CUserManagerService::FinishRemoveUser(
    /* [in] */ Int32 userHandle)
{
    if (DBG) Slogger::I(TAG, "finishRemoveUser %d", userHandle);
    // Let other services shutdown any activity and clean up their state before completely
    // wiping the user's system directory and removing from the user list
    Int64 ident = Binder::ClearCallingIdentity();
    // try {
    AutoPtr<IIntent> addedIntent;
    CIntent::New(IIntent::ACTION_USER_REMOVED, (IIntent**)&addedIntent);
    addedIntent->PutExtra(IIntent::EXTRA_USER_HANDLE, userHandle);
    AutoPtr<IBroadcastReceiver> receiver = new FinishRemoveUserReceiver(this, userHandle);
    mContext->SendOrderedBroadcastAsUser(addedIntent, UserHandle::ALL,
            Elastos::Droid::Manifest::permission::MANAGE_USERS, receiver,
            NULL, IActivity::RESULT_OK, String(NULL), NULL);
    // } finally {
    Binder::RestoreCallingIdentity(ident);
    // }
}

void CUserManagerService::RemoveUserStateLocked(
    /* [in] */ Int32 userHandle)
{
    // Cleanup package manager settings
    mPm->CleanUpUserLILPw(this, userHandle);

    // Remove this user from the list
    mUsers.Erase(userHandle);

    // Have user ID linger for several seconds to let external storage VFS
    // cache entries expire. This must be greater than the 'entry_valid'
    // timeout used by the FUSE daemon.
    AutoPtr<IRunnable> runnable = new RemoveUserStateRunnable(this, userHandle);
    Boolean result;
    mHandler->PostDelayed(runnable, IDateUtils::MINUTE_IN_MILLIS, &result);

    mRestrictionsPinStates.Erase(userHandle);
    mRemovingUserIds.Erase(userHandle);
    // Remove user file
    AutoPtr<IFile> file;
    CFile::New(mUsersDir, StringUtils::ToString(userHandle) + XML_SUFFIX, (IFile**)&file);
    AutoPtr<IAtomicFile> userFile;
    CAtomicFile::New(file, (IAtomicFile**)&userFile);
    userFile->Delete();
    // Update the user list
    WriteUserListLocked();
    UpdateUserIdsLocked();
    AutoPtr<IFile> usDir = Environment::GetUserSystemDirectory(userHandle);
    RemoveDirectoryRecursive(usDir);
}

void CUserManagerService::RemoveDirectoryRecursive(
    /* [in] */ IFile* parent)
{
    Boolean result;
    if (parent->IsDirectory(&result), result) {
        AutoPtr< ArrayOf<String> > files;
        parent->List((ArrayOf<String>**)&files);
        for (Int32 i = 0; i < files->GetLength(); ++i) {
            String filename = (*files)[i];
            AutoPtr<IFile> child;
            CFile::New(parent, filename, (IFile**)&child);
            RemoveDirectoryRecursive(child);
        }
    }
    parent->Delete(&result);
}

void CUserManagerService::SendProfileRemovedBroadcast(
    /* [in] */ Int32 parentUserId,
    /* [in] */ Int32 removedUserId)
{
    AutoPtr<IIntent> managedProfileIntent;
    CIntent::New(IIntent::ACTION_MANAGED_PROFILE_REMOVED, (IIntent**)&managedProfileIntent);
    managedProfileIntent->AddFlags(IIntent::FLAG_RECEIVER_REGISTERED_ONLY |
            IIntent::FLAG_RECEIVER_FOREGROUND);
    AutoPtr<IUserHandle> removedHandle;
    CUserHandle::New(removedUserId, (IUserHandle**)&removedHandle);
    managedProfileIntent->PutExtra(IIntent::EXTRA_USER, IParcelable::Probe(removedHandle));
    AutoPtr<IUserHandle> parentHandle;
    CUserHandle::New(parentUserId, (IUserHandle**)&parentHandle);
    mContext->SendBroadcastAsUser(managedProfileIntent, parentHandle, String(NULL));
}

ECode CUserManagerService::GetApplicationRestrictions(
    /* [in] */ const String& packageName,
    /* [out] */ IBundle** bundle)
{
    VALIDATE_NOT_NULL(bundle)
    return GetApplicationRestrictionsForUser(packageName,
            UserHandle::GetCallingUserId(), bundle);
}

ECode CUserManagerService::GetApplicationRestrictionsForUser(
    /* [in] */ const String& packageName,
    /* [in] */ Int32 userId,
    /* [out] */ IBundle** bundle)
{
    VALIDATE_NOT_NULL(bundle)
    *bundle = NULL;
    if (UserHandle::GetCallingUserId() != userId
            || !UserHandle::IsSameApp(Binder::GetCallingUid(), GetUidForPackage(packageName))) {
        FAIL_RETURN(CheckManageUsersPermission(String("Only system can get restrictions for other users/apps")))
    }
    {    AutoLock syncLock(mPackagesLock);
        // Read the restrictions from XML
        AutoPtr<IBundle> temp = ReadApplicationRestrictionsLocked(packageName, userId);
        *bundle = temp;
        REFCOUNT_ADD(*bundle)
    }
    return NOERROR;
}

ECode CUserManagerService::SetApplicationRestrictions(
    /* [in] */ const String& packageName,
    /* [in] */ IBundle* restrictions,
    /* [in] */ Int32 userId)
{
    if (UserHandle::GetCallingUserId() != userId
            || !UserHandle::IsSameApp(Binder::GetCallingUid(), GetUidForPackage(packageName))) {
        FAIL_RETURN(CheckManageUsersPermission(String("Only system can set restrictions for other users/apps")))
    }
    {    AutoLock syncLock(mPackagesLock);
        Boolean isEmpty;
        if (restrictions == NULL || (restrictions->IsEmpty(&isEmpty), isEmpty)) {
            CleanAppRestrictionsForPackage(packageName, userId);
        }
        else {
            // Write the restrictions to XML
            WriteApplicationRestrictionsLocked(packageName, restrictions, userId);
        }
    }

    if (IsPackageInstalled(packageName, userId)) {
        // Notify package of changes via an intent - only sent to explicitly registered receivers.
        AutoPtr<IIntent> changeIntent;
        CIntent::New(IIntent::ACTION_APPLICATION_RESTRICTIONS_CHANGED, (IIntent**)&changeIntent);
        changeIntent->SetPackage(packageName);
        changeIntent->AddFlags(IIntent::FLAG_RECEIVER_REGISTERED_ONLY);
        AutoPtr<IUserHandle> handle;
        CUserHandle::New(userId, (IUserHandle**)&handle);
        mContext->SendBroadcastAsUser(changeIntent, handle);
    }
    return NOERROR;
}

ECode CUserManagerService::SetRestrictionsChallenge(
    /* [in] */ const String& newPin,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;

    FAIL_RETURN(CheckManageUsersPermission(String("Only system can modify restrictions pin")))
    Int32 userId = UserHandle::GetCallingUserId();
    {    AutoLock syncLock(mPackagesLock);
        AutoPtr<RestrictionsPinState> pinState;
        HashMap<Int32, AutoPtr<RestrictionsPinState> >::Iterator it = mRestrictionsPinStates.Find(userId);
        if (it != mRestrictionsPinStates.End()) {
            pinState = it->mSecond;
        }
        if (pinState == NULL) {
            pinState = new RestrictionsPinState();
        }
        if (newPin.IsNull()) {
            pinState->mSalt = 0;
            pinState->mPinHash = NULL;
        }
        else {
            // try {
            AutoPtr<ISecureRandomHelper> helper;
            assert(0);
            // CSecureRandomHelper::AcquireSingleton((ISecureRandomHelper**)&helper);
            AutoPtr<ISecureRandom> random;
            helper->GetInstance(String("SHA1PRNG"), (ISecureRandom**)&random);
            if (FAILED(IRandom::Probe(random)->NextInt64(&pinState->mSalt))) {
                pinState->mSalt = (Int64)(Elastos::Core::Math::Random() * Elastos::Core::Math::INT64_MAX_VALUE);
            }
            // } catch (NoSuchAlgorithmException e) {
            //     pinState.salt = (long) (Math.random() * Long.MAX_VALUE);
            // }
            pinState->mPinHash = PasswordToHash(newPin, pinState->mSalt);
            pinState->mFailedAttempts = 0;
        }
        mRestrictionsPinStates[userId] = pinState;
        AutoPtr<IUserInfo> info;
        HashMap<Int32, AutoPtr<IUserInfo> >::Iterator userIt = mUsers.Find(userId);
        if (userIt != mUsers.End()) {
            info = userIt->mSecond;
        }
        WriteUserLocked(info);
    }
    *result = TRUE;
    return NOERROR;
}

ECode CUserManagerService::CheckRestrictionsChallenge(
    /* [in] */ const String& pin,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = -1;

    FAIL_RETURN(CheckManageUsersPermission(String("Only system can verify the restrictions pin")))
    Int32 userId = UserHandle::GetCallingUserId();
    {    AutoLock syncLock(mPackagesLock);
        AutoPtr<RestrictionsPinState> pinState;
        HashMap<Int32, AutoPtr<RestrictionsPinState> >::Iterator it = mRestrictionsPinStates.Find(userId);
        if (it != mRestrictionsPinStates.End()) {
            pinState = it->mSecond;
        }
        // If there's no pin set, return error code
        if (pinState == NULL || pinState->mSalt == 0 || pinState->mPinHash.IsNull()) {
            *result = IUserManager::PIN_VERIFICATION_FAILED_NOT_SET;
        }
        else if (pin == NULL) {
            // If just checking if user can be prompted, return remaining time
            Int32 waitTime = GetRemainingTimeForPinAttempt(pinState);
            Slogger::D(LOG_TAG, "Remaining waittime peek=%d", waitTime);
            *result = waitTime;
        }
        else {
            Int32 waitTime = GetRemainingTimeForPinAttempt(pinState);
            Slogger::D(LOG_TAG, "Remaining waittime=%d", waitTime);
            if (waitTime > 0) {
                *result = waitTime;
                return NOERROR;
            }
            if (PasswordToHash(pin, pinState->mSalt).Equals(pinState->mPinHash)) {
                pinState->mFailedAttempts = 0;
                AutoPtr<IUserInfo> info;
                HashMap<Int32, AutoPtr<IUserInfo> >::Iterator userIt = mUsers.Find(userId);
                if (userIt != mUsers.End()) {
                    info = userIt->mSecond;
                }
                WriteUserLocked(info);
                *result = IUserManager::PIN_VERIFICATION_SUCCESS;
            }
            else {
                pinState->mFailedAttempts++;
                AutoPtr<ISystem> sys;
                CSystem::AcquireSingleton((ISystem**)&sys);
                sys->GetCurrentTimeMillis(&pinState->mLastAttemptTime);
                AutoPtr<IUserInfo> info;
                HashMap<Int32, AutoPtr<IUserInfo> >::Iterator userIt = mUsers.Find(userId);
                if (userIt != mUsers.End()) {
                    info = userIt->mSecond;
                }
                WriteUserLocked(info);
                *result = waitTime;
            }
        }
    }
    return NOERROR;
}

Int32 CUserManagerService::GetRemainingTimeForPinAttempt(
    /* [in] */ RestrictionsPinState* pinState)
{
    Int32 backoffIndex = Elastos::Core::Math::Min(pinState->mFailedAttempts / BACKOFF_INC_INTERVAL,
            BACKOFF_TIMES->GetLength() - 1);
    Int32 backoffTime = (pinState->mFailedAttempts % BACKOFF_INC_INTERVAL) == 0 ?
            (*BACKOFF_TIMES)[backoffIndex] : 0;
    Int64 millis;
    AutoPtr<ISystem> sys;
    CSystem::AcquireSingleton((ISystem**)&sys);
    sys->GetCurrentTimeMillis(&millis);
    return (Int32)Elastos::Core::Math::Max(backoffTime + pinState->mLastAttemptTime - millis, (Int64)0);
}

ECode CUserManagerService::HasRestrictionsChallenge(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    Int32 userId = UserHandle::GetCallingUserId();
    {    AutoLock syncLock(mPackagesLock);
        *result = HasRestrictionsPinLocked(userId);
    }
    return NOERROR;
}

Boolean CUserManagerService::HasRestrictionsPinLocked(
    /* [in] */ Int32 userId)
{
    AutoPtr<RestrictionsPinState> pinState;
    HashMap<Int32, AutoPtr<RestrictionsPinState> >::Iterator it = mRestrictionsPinStates.Find(userId);
    if (it != mRestrictionsPinStates.End()) {
        pinState = it->mSecond;
    }
    if (pinState == NULL || pinState->mSalt == 0 || pinState->mPinHash.IsNull()) {
        return FALSE;
    }
    return TRUE;
}

ECode CUserManagerService::RemoveRestrictions()
{
    FAIL_RETURN(CheckManageUsersPermission(String("Only system can remove restrictions")))
    Int32 userHandle = UserHandle::GetCallingUserId();
    RemoveRestrictionsForUser(userHandle, TRUE);
    return NOERROR;
}

void CUserManagerService::RemoveRestrictionsForUser(
    /* [in] */ Int32 userHandle,
    /* [in] */ Boolean unhideApps)
{
    {    AutoLock syncLock(mPackagesLock);
        // Remove all user restrictions
        AutoPtr<IBundle> b;
        CBundle::New((IBundle**)&b);
        SetUserRestrictions(b, userHandle);
        // Remove restrictions pin
        Boolean result;
        SetRestrictionsChallenge(String(NULL), &result);
        // Remove any app restrictions
        CleanAppRestrictions(userHandle);
    }
    if (unhideApps) {
        UnhideAllInstalledAppsForUser(userHandle);
    }
}

void CUserManagerService::UnhideAllInstalledAppsForUser(
    /* [in] */ Int32 userHandle)
{
    AutoPtr<IRunnable> runnable = new UnhideAllInstalledAppsRunnable(this, userHandle);
    Boolean result;
    mHandler->Post(runnable, &result);
}

String CUserManagerService::PasswordToHash(
    /* [in] */ const String& password,
    /* [in] */ Int64 salt)
{
    if (password.IsNull()) {
        return String(NULL);
    }
    String algo(NULL);
    StringBuilder sb(salt); sb += password;
    String hashed = sb.ToString();
    // try {
    sb.Reset(); sb += password; sb += salt;
    AutoPtr<ArrayOf<Byte> > saltedPassword = sb.ToString().GetBytes();
    AutoPtr<IMessageDigestHelper> helper;
    CMessageDigestHelper::AcquireSingleton((IMessageDigestHelper**)&helper);
    AutoPtr<IMessageDigest> md;
    helper->GetInstance(String("SHA-1"), (IMessageDigest**)&md);
    AutoPtr<ArrayOf<Byte> > sha1;
    md->Digest(saltedPassword, (ArrayOf<Byte>**)&sha1);
    md = NULL;
    helper->GetInstance(String("MD5"), (IMessageDigest**)&md);
    AutoPtr<ArrayOf<Byte> > md5;
    md->Digest(saltedPassword, (ArrayOf<Byte>**)&md5);
    hashed = ToHex(sha1) + ToHex(md5);
    // } catch (NoSuchAlgorithmException e) {
    //     Log.w(LOG_TAG, "Failed to encode string because of missing algorithm: " + algo);
    // }
    return hashed;
}

String CUserManagerService::ToHex(
    /* [in] */ ArrayOf<Byte>* ary)
{
    String hex("0123456789ABCDEF");
    StringBuilder sb("");
    for (Int32 i = 0; i < ary->GetLength(); i++) {
        sb += hex.GetChar(((*ary)[i] >> 4) & 0xf);
        sb += hex.GetChar((*ary)[i] & 0xf);
    }
    return sb.ToString();
}

Int32 CUserManagerService::GetUidForPackage(
    /* [in] */ const String& packageName)
{
    Int64 ident = Binder::ClearCallingIdentity();
    // try {
    AutoPtr<IPackageManager> pm;
    mContext->GetPackageManager((IPackageManager**)&pm);
    AutoPtr<IApplicationInfo> appInfo;
    pm->GetApplicationInfo(packageName, IPackageManager::GET_UNINSTALLED_PACKAGES, (IApplicationInfo**)&appInfo);
    Int32 uid;
    appInfo->GetUid(&uid);
    Binder::RestoreCallingIdentity(ident);
    return uid;
    // } catch (NameNotFoundException nnfe) {
    //     return -1;
    // } finally {
    //     Binder.restoreCallingIdentity(ident);
    // }
}

AutoPtr<IBundle> CUserManagerService::ReadApplicationRestrictionsLocked(
    /* [in] */ const String& packageName,
    /* [in] */ Int32 userId)
{
    AutoPtr<IBundle> restrictions;
    CBundle::New((IBundle**)&restrictions);
    List<String> values;

    AutoPtr<IFileInputStream> fis;
    // try {
    AutoPtr<IFile> dir = Environment::GetUserSystemDirectory(userId);
    AutoPtr<IFile> file;
    CFile::New(dir, PackageToRestrictionsFileName(packageName), (IFile**)&file);
    AutoPtr<IAtomicFile> restrictionsFile;
    CAtomicFile::New(file, (IAtomicFile**)&restrictionsFile);
    restrictionsFile->OpenRead((IFileInputStream**)&fis);
    AutoPtr<IXmlPullParser> parser;
    Xml::NewPullParser((IXmlPullParser**)&parser);
    parser->SetInput(IInputStream::Probe(fis), String(NULL));
    Int32 type;
    while ((parser->Next(&type), type != IXmlPullParser::START_TAG)
            && type != IXmlPullParser::END_DOCUMENT) {
        ;
    }

    if (type != IXmlPullParser::START_TAG) {
        AutoPtr<IFile> baseFile;
        restrictionsFile->GetBaseFile((IFile**)&baseFile);
        Slogger::E(LOG_TAG, "Unable to read restrictions file %p", baseFile.Get());
        return restrictions;
    }

    while (parser->Next(&type), type != IXmlPullParser::END_DOCUMENT) {
        String name;
        if (type == IXmlPullParser::START_TAG &&
                (parser->GetName(&name), name.Equals(TAG_ENTRY))) {
            String key;
            parser->GetAttributeValue(String(NULL), ATTR_KEY, &key);
            String valType;
            parser->GetAttributeValue(String(NULL), ATTR_VALUE_TYPE, &valType);
            String multiple;
            parser->GetAttributeValue(String(NULL), ATTR_MULTIPLE, &multiple);
            if (!multiple.IsNull()) {
                values.Clear();
                Int32 count = StringUtils::ParseInt32(multiple);
                while (count > 0 && (parser->Next(&type), type != IXmlPullParser::END_DOCUMENT)) {
                    String n;
                    if (type == IXmlPullParser::START_TAG
                            && (parser->GetName(&n), n.Equals(TAG_VALUE))) {
                        String text;
                        parser->NextText(&text);
                        values.PushBack(text.Trim());
                        count--;
                    }
                }
                AutoPtr<ArrayOf<String> > valueStrings = ArrayOf<String>::Alloc(values.GetSize());
                List<String>::Iterator it = values.Begin();
                for (Int32 i = 0; it != values.End(); ++it, ++i) {
                    (*valueStrings)[i] = *it;
                }
                restrictions->PutStringArray(key, valueStrings);
            }
            else {
                String text;
                parser->NextText(&text);
                String value = text.Trim();
                if (ATTR_TYPE_BOOLEAN.Equals(valType)) {
                    restrictions->PutBoolean(key, StringUtils::ParseBoolean(value));
                }
                else if (ATTR_TYPE_INTEGER.Equals(valType)) {
                    restrictions->PutInt32(key, StringUtils::ParseInt32(value));
                }
                else {
                    restrictions->PutString(key, value);
                }
            }
        }
    }
    // } catch (IOException ioe) {
    // } catch (XmlPullParserException pe) {
    // } finally {
    //     if (fis != null) {
    //         try {
    //             fis.close();
    //         } catch (IOException e) {
    //         }
    //     }
    // }
    if (fis != NULL) {
        // try {
        ICloseable::Probe(fis)->Close();
        // } catch (IOException e) {
        // }
    }
    return restrictions;
}

void CUserManagerService::WriteApplicationRestrictionsLocked(
    /* [in] */ const String& packageName,
    /* [in] */ IBundle* restrictions,
    /* [in] */ Int32 userId)
{
    AutoPtr<IFileOutputStream> fos;
    AutoPtr<IFile> dir = Environment::GetUserSystemDirectory(userId);
    AutoPtr<IFile> file;
    CFile::New(dir, PackageToRestrictionsFileName(packageName), (IFile**)&file);
    AutoPtr<IAtomicFile> restrictionsFile;
    CAtomicFile::New(file, (IAtomicFile**)&restrictionsFile);
    // try {
    restrictionsFile->StartWrite((IFileOutputStream**)&fos);
    AutoPtr<IBufferedOutputStream> bos;
    CBufferedOutputStream::New(IOutputStream::Probe(fos), (IBufferedOutputStream**)&bos);

    // XmlSerializer serializer = XmlUtils.serializerInstance();
AutoPtr<IXmlSerializer> serializer;
    CFastXmlSerializer::New((IXmlSerializer**)&serializer);
    serializer->SetOutput(IOutputStream::Probe(bos), String("utf-8"));
    serializer->StartDocument(String(NULL), TRUE);
    serializer->SetFeature(String("http://xmlpull.org/v1/doc/features.html#indent-output"), TRUE);

    serializer->WriteStartTag(String(NULL), TAG_RESTRICTIONS);

    AutoPtr<ISet> set;
    restrictions->GetKeySet((ISet**)&set);
    AutoPtr<IIterator> it;
    set->GetIterator((IIterator**)&it);
    Boolean hasNext;
    while(it->HasNext(&hasNext), hasNext) {
        AutoPtr<IInterface> next;
        it->GetNext((IInterface**)&next);
        String key;
        ICharSequence::Probe(next)->ToString(&key);
        AutoPtr<IInterface> value;
        restrictions->Get(key, (IInterface**)&value);
        serializer->WriteStartTag(String(NULL), TAG_ENTRY);
        serializer->WriteAttribute(String(NULL), ATTR_KEY, key);

        if (IBoolean::Probe(value) != NULL) {
            serializer->WriteAttribute(String(NULL), ATTR_VALUE_TYPE, ATTR_TYPE_BOOLEAN);
            String str;
            IObject::Probe(value)->ToString(&str);
            serializer->WriteText(str);
        }
        else if (IInteger32::Probe(value) != NULL) {
            serializer->WriteAttribute(String(NULL), ATTR_VALUE_TYPE, ATTR_TYPE_INTEGER);
            String str;
            IObject::Probe(value)->ToString(&str);
            serializer->WriteText(str);
        }
        else if (value == NULL || ICharSequence::Probe(value) != NULL) {
            serializer->WriteAttribute(String(NULL), ATTR_VALUE_TYPE, ATTR_TYPE_STRING);
            String str("");
            if (value != NULL) {
                ICharSequence::Probe(value)->ToString(&str);
            }
            serializer->WriteText(str);
        }
        else {
            serializer->WriteAttribute(String(NULL), ATTR_VALUE_TYPE, ATTR_TYPE_STRING_ARRAY);
            AutoPtr<IArrayOf> values = IArrayOf::Probe(value);
            Int32 length;
            values->GetLength(&length);
            serializer->WriteAttribute(String(NULL), ATTR_MULTIPLE, StringUtils::ToString(length));
            for (Int32 i = 0; i < length; ++i) {
                AutoPtr<IInterface> value;
                values->Get(i, (IInterface**)&value);
                String choice;
                ICharSequence::Probe(value)->ToString(&choice);
                serializer->WriteStartTag(String(NULL), TAG_VALUE);
                serializer->WriteText(!choice.IsNull() ? choice : String(""));
                serializer->WriteEndTag(String(NULL), TAG_VALUE);
            }
        }
        serializer->WriteEndTag(String(NULL), TAG_ENTRY);
    }

    serializer->WriteEndTag(String(NULL), TAG_RESTRICTIONS);

    serializer->EndDocument();
    restrictionsFile->FinishWrite(fos);
    // } catch (Exception e) {
    //     restrictionsFile.failWrite(fos);
    //     Slog.e(LOG_TAG, "Error writing application restrictions list");
    // }
}

ECode CUserManagerService::GetUserSerialNumber(
    /* [in] */ Int32 userHandle,
    /* [out] */ Int32* serialNo)
{
    VALIDATE_NOT_NULL(serialNo)
    {    AutoLock syncLock(mPackagesLock);
        if (!Exists(userHandle)) {
            *serialNo = -1;
            return NOERROR;
        }
        GetUserInfoLocked(userHandle)->GetSerialNumber(serialNo);
    }
    return NOERROR;
}

ECode CUserManagerService::GetUserHandle(
    /* [in] */ Int32 userSerialNumber,
    /* [out] */ Int32* userHandle)
{
    VALIDATE_NOT_NULL(userHandle)
    {    AutoLock syncLock(mPackagesLock);
        for (Int32 i = 0; i < mUserIds->GetLength(); ++i) {
            Int32 userId = (*mUserIds)[i];
            AutoPtr<IUserInfo> user = GetUserInfoLocked(userId);
            Int32 serialNumber;
            user->GetSerialNumber(&serialNumber);
            if (serialNumber == userSerialNumber) {
                *userHandle = userId;
                return NOERROR;
            }
        }
        // Not found
        *userHandle = -1;
    }
    return NOERROR;
}

void CUserManagerService::UpdateUserIdsLocked()
{
    List<Int32> list;
    Boolean partial;
    HashMap<Int32, AutoPtr<IUserInfo> >::Iterator it;
    for (it = mUsers.Begin(); it != mUsers.End(); ++it) {
        it->mSecond->GetPartial(&partial);
        if (!partial) {
            list.PushBack(it->mFirst);
        }
    }
    AutoPtr< ArrayOf<Int32> > newUsers = ArrayOf<Int32>::Alloc(list.GetSize());
    Int32 i = 0;
    List<Int32>::Iterator lit;
    for (lit = list.Begin(); lit != list.End(); ++lit) {
        (*newUsers)[i++] = *lit;
    }
    mUserIds = newUsers;
}

void CUserManagerService::UserForeground(
    /* [in] */ Int32 userId)
{
    {    AutoLock syncLock(mPackagesLock);
        AutoPtr<IUserInfo> user;
        HashMap<Int32, AutoPtr<IUserInfo> >::Iterator it = mUsers.Find(userId);
        if (it != mUsers.End()) {
            user = it->mSecond;
        }

        AutoPtr<ISystem> system;
        CSystem::AcquireSingleton((ISystem**)&system);
        Int64 now;
        system->GetCurrentTimeMillis(&now);
        Boolean partial;
        if (user == NULL || (user->GetPartial(&partial), partial)) {
            Slogger::W(TAG, "userForeground: unknown user #%d", userId);
            return;
        }
        if (now > EPOCH_PLUS_30_YEARS) {
            user->SetLastLoggedInTime(now);
            WriteUserLocked(user);
        }
    }
}

Int32 CUserManagerService::GetNextAvailableIdLocked()
{
    Int32 i = MIN_USER_ID;
    {    AutoLock syncLock(mPackagesLock);
        while (i < Elastos::Core::Math::INT32_MAX_VALUE) {
            if (mUsers.Find(i) == mUsers.End() && !mRemovingUserIds[i]) {
                break;
            }
            i++;
        }
    }
    return i;
}

String CUserManagerService::PackageToRestrictionsFileName(
    /* [in] */ const String& packageName)
{
    return RESTRICTIONS_FILE_PREFIX + packageName + XML_SUFFIX;
}

String CUserManagerService::RestrictionsFileNameToPackage(
    /* [in] */ const String& fileName)
{
    return fileName.Substring(RESTRICTIONS_FILE_PREFIX.GetLength(),
            (Int32)(fileName.GetLength() - XML_SUFFIX.GetLength()));
}

ECode CUserManagerService::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)
    return Object::ToString(str);
}

// @Override
// protected void dump(FileDescriptor fd, PrintWriter pw, String[] args) {
//     if (mContext.checkCallingOrSelfPermission(Elastos::Droid::Manifest::permission::DUMP)
//             != PackageManager.PERMISSION_GRANTED) {
//         pw.println("Permission Denial: can't dump UserManager from from pid="
//                 + Binder.getCallingPid()
//                 + ", uid=" + Binder.getCallingUid()
//                 + " without permission "
//                 + Elastos::Droid::Manifest::permission::DUMP);
//         return;
//     }

//     long now = System.currentTimeMillis();
//     StringBuilder sb = new StringBuilder();
//     {    AutoLock syncLock(mPackagesLock);
//         pw.println("Users:");
//         for (int i = 0; i < mUsers.size(); i++) {
//             UserInfo user = mUsers.valueAt(i);
//             if (user == null) continue;
//             pw.print("  "); pw.print(user); pw.print(" serialNo="); pw.print(user.serialNumber);
//             if (mRemovingUserIds.contains(mUsers.keyAt(i))) pw.print(" <removing> ");
//             if (user.partial) pw.print(" <partial>");
//             pw.println();
//             pw.print("    Created: ");
//             if (user.creationTime == 0) {
//                 pw.println("<unknown>");
//             } else {
//                 sb.setLength(0);
//                 TimeUtils.formatDuration(now - user.creationTime, sb);
//                 sb.append(" ago");
//                 pw.println(sb);
//             }
//             pw.print("    Last logged in: ");
//             if (user.lastLoggedInTime == 0) {
//                 pw.println("<unknown>");
//             } else {
//                 sb.setLength(0);
//                 TimeUtils.formatDuration(now - user.lastLoggedInTime, sb);
//                 sb.append(" ago");
//                 pw.println(sb);
//             }
//         }
//     }
// }

} // namespace Pm
} // namespace Server
} // namespace Droid
} // namespace Elastos
