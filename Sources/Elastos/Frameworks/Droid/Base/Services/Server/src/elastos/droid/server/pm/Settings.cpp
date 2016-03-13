
#include "elastos/droid/ext/frameworkdef.h"
#include "elastos/droid/server/pm/Settings.h"
#include "elastos/droid/server/pm/CPackageManagerService.h"
#include "elastos/droid/server/pm/PackageSettingBase.h"
#include "elastos/droid/os/Process.h"
#include "elastos/droid/os/FileUtils.h"
#include "elastos/droid/os/UserHandle.h"
#include "elastos/droid/os/Binder.h"
#include "elastos/droid/os/Environment.h"
#include "elastos/droid/utility/Xml.h"
#include "elastos/droid/internal/utility/XmlUtils.h"
#include <Elastos.CoreLibrary.Libcore.h>
#include <Elastos.CoreLibrary.Text.h>
#include <elastos/utility/logging/Logger.h>
#include <elastos/utility/logging/Slogger.h>
#include <elastos/core/StringUtils.h>

using Elastos::Core::StringUtils;
using Elastos::Core::ICharSequence;
using Elastos::Core::CString;
using Elastos::Core::EIID_IInteger32;
using Elastos::Core::IBoolean;
using Elastos::Core::CBoolean;
using Elastos::IO::CFile;
using Elastos::IO::IFileInputStream;
using Elastos::IO::IFileOutputStream;
using Elastos::IO::CFileInputStream;
using Elastos::IO::CFileOutputStream;
using Elastos::IO::IBufferedOutputStream;
using Elastos::IO::CBufferedOutputStream;
using Elastos::IO::IFlushable;
using Elastos::IO::ICloseable;
using Elastos::Text::ISimpleDateFormat;
using Elastos::Text::CSimpleDateFormat;
using Elastos::Utility::IDate;
using Elastos::Utility::CDate;
using Elastos::Utility::Logging::Slogger;
using Elastos::Utility::Logging::Logger;
using Libcore::IO::IIoUtils;
using Libcore::IO::CIoUtils;
using Elastos::Droid::Internal::Utility::IFastXmlSerializer;
using Elastos::Droid::Internal::Utility::CFastXmlSerializer;
using Elastos::Droid::Utility::Xml;
using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Content::CIntentFilter;
using Elastos::Droid::Content::CComponentName;
using Elastos::Droid::Content::Pm::CPackageCleanItem;
using Elastos::Droid::Content::Pm::CUserInfo;
using Elastos::Droid::Content::Pm::CVerifierDeviceIdentityHelper;
using Elastos::Droid::Content::Pm::IApplicationInfo;
using Elastos::Droid::Content::Pm::IPackageManager;
using Elastos::Droid::Content::Pm::IVerifierDeviceIdentityHelper;
using Elastos::Droid::Content::Pm::CPermissionInfo;
using Elastos::Droid::Content::Pm::IPermissionInfoHelper;
using Elastos::Droid::Content::Pm::CPermissionInfoHelper;
using Elastos::Droid::Content::Pm::IPackageItemInfo;
using Elastos::Droid::Content::Pm::IComponentInfo;
using Elastos::Droid::Net::IUriBuilder;
using Elastos::Droid::Net::CUriBuilder;
using Elastos::Droid::Os::Environment;
using Elastos::Droid::Os::FileUtils;
using Elastos::Droid::Os::IProcess;
using Elastos::Droid::Os::IUserHandleHelper;
using Elastos::Droid::Os::CUserHandleHelper;
using Elastos::Droid::Os::UserHandle;
using Elastos::Droid::Os::Binder;
using Elastos::Droid::Os::IUserManager;
using Elastos::Droid::Server::Pm::CPackageManagerService;
using Elastos::Droid::Server::Pm::PackageSettingBase;
using Elastos::Droid::Internal::Utility::XmlUtils;
using Elastos::Droid::Internal::Utility::IJournaledFile;
using Elastos::Droid::Internal::Utility::CJournaledFile;
using Elastos::Droid::Utility::ILogHelper;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Pm {

//==============================================================================
//                  Settings::DatabaseVersion
//==============================================================================

const Int32 Settings::DatabaseVersion::FIRST_VERSION;
const Int32 Settings::DatabaseVersion::SIGNATURE_END_ENTITY;
const Int32 Settings::DatabaseVersion::SIGNATURE_MALFORMED_RECOVER;


//==============================================================================
//                  Settings
//==============================================================================

const String Settings::TAG("Pm::Settings");
const Int32 Settings::CURRENT_DATABASE_VERSION;
const Boolean Settings::DEBUG_STOPPED = FALSE;
const Boolean Settings::DEBUG_MU = FALSE;
const String Settings::TAG_READ_EXTERNAL_STORAGE("read-external-storage");
const String Settings::ATTR_ENFORCEMENT("enforcement");
const String Settings::TAG_ITEM("item");
const String Settings::TAG_DISABLED_COMPONENTS("disabled-components");
const String Settings::TAG_ENABLED_COMPONENTS("enabled-components");
const String Settings::TAG_PACKAGE_RESTRICTIONS("package-restrictions");
const String Settings::TAG_PACKAGE("pkg");
const String Settings::TAG_PERSISTENT_PREFERRED_ACTIVITIES("persistent-preferred-activities");
const String Settings::TAG_CROSS_PROFILE_INTENT_FILTERS("crossProfile-intent-filters");
const String Settings::ATTR_NAME("name");
const String Settings::ATTR_USER("user");
const String Settings::ATTR_CODE("code");
const String Settings::ATTR_NOT_LAUNCHED("nl");
const String Settings::ATTR_ENABLED("enabled");
const String Settings::ATTR_ENABLED_CALLER("enabledCaller");
const String Settings::ATTR_STOPPED("stopped");
const String Settings::ATTR_BLOCKED("blocked");
const String Settings::ATTR_HIDDEN("hidden");
const String Settings::ATTR_INSTALLED("inst");
const String Settings::ATTR_BLOCK_UNINSTALL("blockUninstall");
Int32 Settings::mFirstAvailableUid = 0;

Settings::Settings(
    /* [in] */ IContext* context)
    : mPackages(50)
    , mInternalSdkPlatform(0)
    , mExternalSdkPlatform(0)
    , mInternalDatabaseVersion(0)
    , mExternalDatabaseVersion(0)
    , mPreferredActivities(20)
    , mSharedUsers(20)
    , mPermissions(50)
    , mPermissionTrees(50)
    , mRenamedPackages(50)
    , mDisabledSysPackages(20)
    , mOtherUserIds(20)
{
    mKeySetManagerService = new KeySetManagerService(&mPackages);
    AutoPtr<IFile> dataDir = Environment::GetDataDirectory();
    Init(context, dataDir);
}

Settings::Settings(
    /* [in] */ IContext* context,
    /* [in] */ IFile* dataDir)
    : mPackages(50)
    , mInternalSdkPlatform(0)
    , mExternalSdkPlatform(0)
    , mInternalDatabaseVersion(0)
    , mExternalDatabaseVersion(0)
    , mPreferredActivities(20)
    , mSharedUsers(20)
    , mPermissions(50)
    , mPermissionTrees(50)
    , mRenamedPackages(50)
    , mDisabledSysPackages(20)
    , mOtherUserIds(20)
{
    mKeySetManagerService = new KeySetManagerService(&mPackages);
    Init(context, dataDir);
}

void Settings::Init(
    /* [in] */ IContext* context,
    /* [in] */ IFile* dataDir)
{
    CFile::New(dataDir, String("system"), (IFile**)&mSystemDir);
    Boolean result;
    String temp;
    mSystemDir->Mkdirs(&result);
    FileUtils::SetPermissions((mSystemDir->ToString(&temp), temp),
            FileUtils::sS_IRWXU|FileUtils::sS_IRWXG
            |FileUtils::sS_IROTH|FileUtils::sS_IXOTH,
            -1, -1);
    CFile::New(mSystemDir, String("packages.xml"), (IFile**)&mSettingsFilename);
    CFile::New(mSystemDir, String("packages-backup.xml"), (IFile**)&mBackupSettingsFilename);
    CFile::New(mSystemDir, String("packages.list"), (IFile**)&mPackageListFilename);
    FileUtils::SetPermissions(mPackageListFilename, 0660, IProcess::SYSTEM_UID, IProcess::PACKAGE_INFO_GID);

    // Deprecated: Needed for migration
    CFile::New(mSystemDir, String("packages-stopped.xml"), (IFile**)&mStoppedPackagesFilename);
    CFile::New(mSystemDir, String("packages-stopped-backup.xml"), (IFile**)&mBackupStoppedPackagesFilename);
    mReadMessages = new StringBuilder();
}

AutoPtr<PackageSetting> Settings::GetPackageLPw(
    /* [in] */ PackageParser::Package* pkg,
    /* [in] */ PackageSetting* origPackage,
    /* [in] */ const String& realName,
    /* [in] */ SharedUserSetting* sharedUser,
    /* [in] */ IFile* codePath,
    /* [in] */ IFile* resourcePath,
    /* [in] */ const String& legacyNativeLibraryPathString,
    /* [in] */ const String& primaryCpuAbi,
    /* [in] */ const String& secondaryCpuAbi,
    /* [in] */ Int32 pkgFlags,
    /* [in] */ IUserHandle* user,
    /* [in] */ Boolean add)
{
    const String& name = pkg->mPackageName;
    AutoPtr<PackageSetting> p = GetPackageLPw(name, origPackage, realName, sharedUser, codePath,
            resourcePath, legacyNativeLibraryPathString, primaryCpuAbi, secondaryCpuAbi,
            pkg->mVersionCode, pkgFlags, user, add, TRUE /* allowInstall */);
    return p;
}

AutoPtr<PackageSetting> Settings::PeekPackageLPr(
    /* [in] */ const String& name)
{
    AutoPtr<PackageSetting> p;
    HashMap<String, AutoPtr<PackageSetting> >::Iterator it = mPackages.Find(name);
    if (it != mPackages.End()) {
        p = it->mSecond;
    }
    return p;
}

void Settings::SetInstallStatus(
    /* [in] */ const String& pkgName,
    /* [in] */ Int32 status)
{
    AutoPtr<PackageSetting> p;
    HashMap<String, AutoPtr<PackageSetting> >::Iterator it = mPackages.Find(pkgName);
    if (it != mPackages.End()) {
        p = it->mSecond;
    }
    if(p != NULL) {
        if(p->GetInstallStatus() != status) {
            p->SetInstallStatus(status);
        }
    }
}

void Settings::SetInstallerPackageName(
    /* [in] */ const String& pkgName,
    /* [in] */ const String& installerPkgName)
{
    AutoPtr<PackageSetting> p;
    HashMap<String, AutoPtr<PackageSetting> >::Iterator it = mPackages.Find(pkgName);
    if (it != mPackages.End()) {
        p = it->mSecond;
    }
    if(p != NULL) {
        p->SetInstallerPackageName(installerPkgName);
    }
}

AutoPtr<SharedUserSetting> Settings::GetSharedUserLPw(
    /* [in] */ const String& name,
    /* [in] */ Int32 pkgFlags,
    /* [in] */ Boolean create)
{
    AutoPtr<SharedUserSetting> s;
    HashMap<String, AutoPtr<SharedUserSetting> >::Iterator it = mSharedUsers.Find(name);
    if (it != mSharedUsers.End()) {
        s = it->mSecond;
    }
    if (s == NULL) {
        if (!create) {
            return NULL;
        }
        s = new SharedUserSetting(name, pkgFlags);
        s->mUserId = NewUserIdLPw((IObject*)s);
        Slogger::I(TAG, "New shared user %s: id=%d", name.string(), s->mUserId);
        // < 0 means we couldn't assign a userid; fall out and return
        // s, which is currently null
        if (s->mUserId >= 0) {
            mSharedUsers[name] = s;
        }
    }

    return s;
}

HashMap<String, AutoPtr<SharedUserSetting> >& Settings::GetAllSharedUsersLPw()
{
    return mSharedUsers;
}

Boolean Settings::DisableSystemPackageLPw(
    /* [in] */ const String& name)
{
    AutoPtr<PackageSetting> p;
    HashMap<String, AutoPtr<PackageSetting> >::Iterator it = mPackages.Find(name);
    if (it != mPackages.End()) {
        p = it->mSecond;
    }
    if(p == NULL) {
        Slogger::W(TAG, "Package:%s is not an installed package", name.string());
        return FALSE;
    }
    AutoPtr<PackageSetting> dp;
    HashMap<String, AutoPtr<PackageSetting> >::Iterator dpIt = mDisabledSysPackages.Find(name);
    if (dpIt != mDisabledSysPackages.End()) {
        dp = dpIt->mSecond;
    }
    // always make sure the system package code and resource paths dont change
    if (dp == NULL) {
        if((p->mPkg != NULL) && (p->mPkg->mApplicationInfo != NULL)) {
            Int32 flags;
            p->mPkg->mApplicationInfo->GetFlags(&flags);
            flags |= IApplicationInfo::FLAG_UPDATED_SYSTEM_APP;
            p->mPkg->mApplicationInfo->SetFlags(flags);
        }
        mDisabledSysPackages[name] = p;

        // a little trick...  when we install the new package, we don't
        // want to modify the existing PackageSetting for the built-in
        // version.  so at this point we need a new PackageSetting that
        // is okay to muck with.
        AutoPtr<PackageSetting> newp = new PackageSetting(p);
        ReplacePackageLPw(name, newp);
        return TRUE;
    }
    return FALSE;
}

AutoPtr<PackageSetting> Settings::EnableSystemPackageLPw(
    /* [in] */ const String& name)
{
    AutoPtr<PackageSetting> p;
    HashMap<String, AutoPtr<PackageSetting> >::Iterator it = mDisabledSysPackages.Find(name);
    if (it != mDisabledSysPackages.End()) {
        p = it->mSecond;
    }
    if(p == NULL) {
        Slogger::W(TAG, "Package:%s  is not disabled", name.string());
        return NULL;
    }
    // Reset flag in ApplicationInfo object
    if((p->mPkg != NULL) && (p->mPkg->mApplicationInfo != NULL)) {
        Int32 flags;
        p->mPkg->mApplicationInfo->GetFlags(&flags);
        flags &= ~IApplicationInfo::FLAG_UPDATED_SYSTEM_APP;
        p->mPkg->mApplicationInfo->SetFlags(flags);
    }
    AutoPtr<PackageSetting> ret = AddPackageLPw(name, p->mRealName, p->mCodePath, p->mResourcePath,
            p->mLegacyNativeLibraryPathString, p->mPrimaryCpuAbiString,
            p->mSecondaryCpuAbiString, p->mSecondaryCpuAbiString,
            p->mAppId, p->mVersionCode, p->mPkgFlags);
    mDisabledSysPackages.Erase(name);
    return ret;
}

Boolean Settings::IsDisabledSystemPackageLPr(
    /* [in] */ const String& name)
{
    return mDisabledSysPackages.Find(name) != mDisabledSysPackages.End();
}

void Settings::RemoveDisabledSystemPackageLPw(
    /* [in] */ const String& name)
{
    mDisabledSysPackages.Erase(name);
}

AutoPtr<PackageSetting> Settings::AddPackageLPw(
    /* [in] */ const String& name,
    /* [in] */ const String& realName,
    /* [in] */ IFile* codePath,
    /* [in] */ IFile* resourcePath,
    /* [in] */ const String& legacyNativeLibraryPathString,
    /* [in] */ const String& primaryCpuAbiString,
    /* [in] */ const String& secondaryCpuAbiString,
    /* [in] */ const String& cpuAbiOverrideString,
    /* [in] */ Int32 uid,
    /* [in] */ Int32 vc,
    /* [in] */ Int32 pkgFlags)
{
    AutoPtr<PackageSetting> p;
    HashMap<String, AutoPtr<PackageSetting> >::Iterator it = mPackages.Find(name);
    if (it != mPackages.End()) {
        p = it->mSecond;
    }
    if (p != NULL) {
        if (p->mAppId == uid) {
            return p;
        }
        CPackageManagerService::ReportSettingsProblem(ILogHelper::ERROR,
               String("Adding duplicate package, keeping first: ") + name);
        return NULL;
    }
    p = new PackageSetting(name, realName, codePath, resourcePath,
            legacyNativeLibraryPathString, primaryCpuAbiString, secondaryCpuAbiString,
            cpuAbiOverrideString, vc, pkgFlags);
    p->mAppId = uid;
    AutoPtr<ICharSequence> csName;
    CString::New(name, (ICharSequence**)&csName);
    if (AddUserIdLPw(uid, p->Probe(EIID_IInterface), csName)) {
        mPackages[name] = p;
        return p;
    }
    return NULL;
}

AutoPtr<SharedUserSetting> Settings::AddSharedUserLPw(
    /* [in] */ const String& name,
    /* [in] */ Int32 uid,
    /* [in] */ Int32 pkgFlags)
{
    AutoPtr<SharedUserSetting> s;
    HashMap<String, AutoPtr<SharedUserSetting> >::Iterator it = mSharedUsers.Find(name);
    if (it != mSharedUsers.End()) {
        s = it->mSecond;
    }
    if (s != NULL) {
        if (s->mUserId == uid) {
            return s;
        }
        CPackageManagerService::ReportSettingsProblem(ILogHelper::ERROR,
               String("Adding duplicate shared user, keeping first: ") + name);
        return NULL;
    }
    s = new SharedUserSetting(name, pkgFlags);
    s->mUserId = uid;
    AutoPtr<ICharSequence> csName;
    CString::New(name, (ICharSequence**)&csName);
    if (AddUserIdLPw(uid, s->Probe(EIID_IInterface), csName)) {
        mSharedUsers[name] = s;
        return s;
    }
    return NULL;
}

void Settings::PruneSharedUsersLPw()
{
    List<String> removeStage;
    HashMap<String, AutoPtr<SharedUserSetting> >::Iterator userit = mSharedUsers.Begin();
    for (; userit != mSharedUsers.End(); ++userit) {
        AutoPtr<SharedUserSetting> sus = userit->mSecond;
        if (sus == NULL || sus->mPackages.Begin() == sus->mPackages.End()) {
            removeStage.PushBack(userit->mFirst);
        }
    }
    List<String>::Iterator it = removeStage.Begin();
    for (; it != removeStage.End(); ++it) {
        mSharedUsers.Erase(*it);
    }
}

void Settings::TransferPermissionsLPw(
    /* [in] */ const String& origPkg,
    /* [in] */ const String& newPkg)
{
    // Transfer ownership of permissions to the new package.
    for (Int32 i = 0; i < 2; i++) {
        HashMap<String, AutoPtr<BasePermission> >& permissions =
                i == 0 ? mPermissionTrees : mPermissions;
        HashMap<String, AutoPtr<BasePermission> >::Iterator it;
        for (it = permissions.Begin(); it != permissions.End(); ++it) {
            AutoPtr<BasePermission> bp = it->mSecond;
            if (origPkg.Equals(bp->mSourcePackage)) {
                if (CPackageManagerService::DEBUG_UPGRADE) Slogger::V(TAG,
                       "Moving permission %s  from pkg %s  to ", bp->mName.string(), bp->mSourcePackage.string(), newPkg.string());
                bp->mSourcePackage = newPkg;
                bp->mPackageSetting = NULL;
                bp->mPerm = NULL;
                if (bp->mPendingInfo != NULL) {
                    IPackageItemInfo::Probe(bp->mPendingInfo)->SetPackageName(newPkg);
                }
                bp->mUid = 0;
                bp->mGids = NULL;
            }
        }
    }
}

AutoPtr<PackageSetting> Settings::GetPackageLPw(
    /* [in] */ const String& name_,
    /* [in] */ PackageSetting* origPackage,
    /* [in] */ const String& realName,
    /* [in] */ SharedUserSetting* sharedUser,
    /* [in] */ IFile* codePath,
    /* [in] */ IFile* resourcePath,
    /* [in] */ const String& legacyNativeLibraryPathString,
    /* [in] */ const String& primaryCpuAbiString,
    /* [in] */ const String& secondaryCpuAbiString,
    /* [in] */ Int32 vc,
    /* [in] */ Int32 pkgFlags,
    /* [in] */ IUserHandle* installUser,
    /* [in] */ Boolean add,
    /* [in] */ Boolean allowInstall)
{
    String name(name_);
    AutoPtr<PackageSetting> p;
    HashMap<String, AutoPtr<PackageSetting> >::Iterator it = mPackages.Find(name);
    if (it != mPackages.End()) {
        p = it->mSecond;
    }
    AutoPtr<CUserManagerService> userManager = CUserManagerService::GetInstance();
    if (p != NULL) {
        p->mPrimaryCpuAbiString = primaryCpuAbiString;
        p->mSecondaryCpuAbiString = secondaryCpuAbiString;

        Boolean isEqual;
        if (p->mCodePath->Equals(codePath, &isEqual), !isEqual) {
            String codePathStr;
            codePath->ToString(&codePathStr);
            // Check to see if its a disabled system app
            if ((p->mPkgFlags & IApplicationInfo::FLAG_SYSTEM) != 0) {
                // This is an updated system app with versions in both system
                // and data partition. Just let the most recent version
                // take precedence.
                Slogger::W(TAG, "Trying to update system app code path from %s to %s",
                    p->mCodePathString.string(), codePathStr.string());
            }
            else {
                // Just a change in the code path is not an issue, but
                // let's log a message about it.
                Slogger::I(TAG, "Package %s  codePath changed from %s to %s; Retaining data and using new",
                    name.string(), p->mCodePathString.string(), codePathStr.string());
                /*
                 * Since we've changed paths, we need to prefer the new
                 * native library path over the one stored in the
                 * package settings since we might have moved from
                 * internal to external storage or vice versa.
                 */
                p->mLegacyNativeLibraryPathString = legacyNativeLibraryPathString;
            }
        }
        if (p->mSharedUser.Get() != sharedUser) {
            // If what we are scanning is a system (and possibly privileged) package,
            // then make it so, regardless of whether it was previously installed only
            // in the data partition.
            Int32 sysPrivFlags = pkgFlags
                    & (IApplicationInfo::FLAG_SYSTEM | IApplicationInfo::FLAG_PRIVILEGED);
            p->mPkgFlags |= sysPrivFlags;
            p = NULL;
        }
        else {
            // If what we are scanning is a system (and possibly privileged) package,
            // then make it so, regardless of whether it was previously installed only
            // in the data partition.
            Int32 sysPrivFlags = pkgFlags
                    & (IApplicationInfo::FLAG_SYSTEM | IApplicationInfo::FLAG_PRIVILEGED);
            p->mPkgFlags |= sysPrivFlags;
        }
    }
    if (p == NULL) {
        if (origPackage != NULL) {
            // We are consuming the data from an existing package.
            p = new PackageSetting(origPackage->mName, name, codePath, resourcePath,
                    legacyNativeLibraryPathString, primaryCpuAbiString, secondaryCpuAbiString,
                    String(NULL) /* cpuAbiOverrideString */, vc, pkgFlags);
//            if (PackageManagerService.DEBUG_UPGRADE) Log.v(PackageManagerService.TAG, "Package "
//                    + name + " is adopting original package " + origPackage.name);
            // Note that we will retain the new package's signature so
            // that we can keep its data.
            AutoPtr<PackageSignatures> s = p->mSignatures;
            p->CopyFrom(origPackage);
            p->mSignatures = s;
            p->mSharedUser = origPackage->mSharedUser;
            p->mAppId = origPackage->mAppId;
            p->mOrigPackage = origPackage;
            mRenamedPackages[name] = origPackage->mName;
            name = origPackage->mName;
            // Update new package state.
            Int64 time;
            codePath->GetLastModified(&time);
            p->SetTimeStamp(time);
        }
        else {
            p = new PackageSetting(name, realName, codePath, resourcePath,
                    legacyNativeLibraryPathString, primaryCpuAbiString, secondaryCpuAbiString,
                    String(NULL) /* cpuAbiOverrideString */, vc, pkgFlags);
            Int64 time;
            codePath->GetLastModified(&time);
            p->SetTimeStamp(time);
            p->mSharedUser = sharedUser;
            // If this is not a system app, it starts out stopped.
            if ((pkgFlags & IApplicationInfo::FLAG_SYSTEM) == 0) {
                if (DEBUG_STOPPED) {
//                    RuntimeException e = new RuntimeException("here");
//                    e.fillInStackTrace();
//                    Slog.i(PackageManagerService.TAG, "Stopping package " + name, e);
                }
                AutoPtr<IList> users = GetAllUsers();
                Int32 installUserId = 0;
                if (installUser != NULL) {
                    installUser->GetIdentifier(&installUserId);
                }
                if (users != NULL && allowInstall) {
                    AutoPtr<IIterator> it;
                    users->GetIterator((IIterator**)&it);
                    Boolean hasNext;
                    while (it->HasNext(&hasNext), hasNext) {
                        AutoPtr<IInterface> value;
                        it->GetNext((IInterface**)&value);
                        AutoPtr<IUserInfo> user = IUserInfo::Probe(value);
                        assert(user != NULL);
                        // By default we consider this app to be installed
                        // for the user if no user has been specified (which
                        // means to leave it at its original value, and the
                        // original default value is true), or we are being
                        // asked to install for all users, or this is the
                        // user we are installing for.
                        Int32 userId;
                        user->GetId(&userId);
                        Boolean installed = installUser == NULL
                                || ((installUserId == IUserHandle::USER_ALL)
                                        && !IsAdbInstallDisallowed(userManager, userId))
                                || installUserId == userId;
                        p->SetUserState(userId, IPackageManager::COMPONENT_ENABLED_STATE_DEFAULT,
                                installed,
                                TRUE, // stopped,
                                TRUE, // notLaunched
                                FALSE, // hidden
                                String(NULL), NULL, NULL,
                                FALSE // blockUninstall
                                );
                        WritePackageRestrictionsLPr(userId);
                    }
                }
            }
            if (sharedUser != NULL) {
                p->mAppId = sharedUser->mUserId;
            }
            else {
                // Clone the setting here for disabled system packages
                AutoPtr<PackageSetting> dis;
                HashMap<String, AutoPtr<PackageSetting> >::Iterator disIt = mDisabledSysPackages.Find(name);
                if (disIt != mDisabledSysPackages.End()) {
                    dis = disIt->mSecond;
                }
                if (dis != NULL) {
                    // For disabled packages a new setting is created
                    // from the existing user id. This still has to be
                    // added to list of user id's
                    // Copy signatures from previous setting
                    if (dis->mSignatures->mSignatures != NULL) {
                        p->mSignatures->mSignatures = dis->mSignatures->mSignatures->Clone();
                    }
                    p->mAppId = dis->mAppId;
                    // Clone permissions
                    p->mGrantedPermissions.Clear();
                    p->mGrantedPermissions.Insert(dis->mGrantedPermissions.Begin(), dis->mGrantedPermissions.End());
                    // Clone component info
                    AutoPtr<IList> users = GetAllUsers();
                    if (users != NULL) {
                        AutoPtr<IIterator> it;
                        users->GetIterator((IIterator**)&it);
                        Boolean hasNext;
                        while (it->HasNext(&hasNext), hasNext) {
                            AutoPtr<IInterface> value;
                            it->GetNext((IInterface**)&value);
                            AutoPtr<IUserInfo> userInfo = IUserInfo::Probe(value);
                            Int32 userId;
                            userInfo->GetId(&userId);
                            p->SetDisabledComponentsCopy(
                                    dis->GetDisabledComponents(userId), userId);
                            p->SetEnabledComponentsCopy(
                                    dis->GetEnabledComponents(userId), userId);
                        }
                    }
                    // Add new setting to list of user ids
                    AutoPtr<ICharSequence> csName;
                    CString::New(name, (ICharSequence**)&csName);
                    AddUserIdLPw(p->mAppId, p->Probe(EIID_IInterface), csName);
                }
                else {
                    // Assign new user id
                    p->mAppId = NewUserIdLPw((IObject*)p);
                }
            }
        }
        if (p->mAppId < 0) {
            CPackageManagerService::ReportSettingsProblem(ILogHelper::WARN,
                   String("Package ") + name + String(" could not be assigned a valid uid"));
            return NULL;
        }
        if (add) {
            // Finish adding new package by adding it and updating shared
            // user preferences
            AddPackageSettingLPw(p, name, sharedUser);
        }
    }
    else {
        if (installUser != NULL && allowInstall) {
            // The caller has explicitly specified the user they want this
            // package installed for, and the package already exists.
            // Make sure it conforms to the new request.
            AutoPtr<IList> users = GetAllUsers();
            if (users != NULL) {
                Int32 id;
                installUser->GetIdentifier(&id);
                AutoPtr<IIterator> it;
                users->GetIterator((IIterator**)&it);
                Boolean hasNext;
                while (it->HasNext(&hasNext), hasNext) {
                    AutoPtr<IInterface> value;
                    it->GetNext((IInterface**)&value);
                    AutoPtr<IUserInfo> userInfo = IUserInfo::Probe(value);
                    Int32 userId;
                    userInfo->GetId(&userId);
                    if ((id == IUserHandle::USER_ALL && !IsAdbInstallDisallowed(userManager, userId))
                            || id == userId) {
                        Boolean installed = p->GetInstalled(userId);
                        if (!installed) {
                            p->SetInstalled(TRUE, userId);
                            WritePackageRestrictionsLPr(userId);
                        }
                    }
                }
            }
        }
    }
    return p;
}

Boolean Settings::IsAdbInstallDisallowed(
    /* [in] */ CUserManagerService* userManager,
    /* [in] */ Int32 userId)
{
    Boolean result;
    userManager->HasUserRestriction(IUserManager::DISALLOW_DEBUGGING_FEATURES,
            userId, &result);
    return result;
}

void Settings::InsertPackageSettingLPw(
    /* [in] */ PackageSetting* p,
    /* [in] */ PackageParser::Package* pkg)
{
    p->mPkg = pkg;
    String codePath, resourcePath, legacyNativeLibraryPath;
    pkg->mApplicationInfo->GetCodePath(&codePath);
    pkg->mApplicationInfo->GetResourcePath(&resourcePath);
    pkg->mApplicationInfo->GetNativeLibraryRootDir(&legacyNativeLibraryPath);
    // Update code path if needed
    if (!codePath.Equals(p->mCodePathString)) {
        Slogger::W(TAG, "Code path for pkg : %s  changing from %s  to ", p->mPkg->mPackageName.string(),
               p->mCodePathString.string(), codePath.string());
        p->mCodePath = NULL;
        CFile::New(codePath, (IFile**)&p->mCodePath);
        p->mCodePathString = codePath;
    }
    //Update resource path if needed
    if (!resourcePath.EqualsIgnoreCase(p->mResourcePathString)) {
        Slogger::W(TAG, "Resource path for pkg : %s  changing from %s  to %s", p->mPkg->mPackageName.string(),
               p->mResourcePathString.string(), resourcePath.string());
        p->mResourcePath = NULL;
        CFile::New(resourcePath, (IFile**)&p->mResourcePath);
        p->mResourcePathString = resourcePath;
    }
    // Update the native library paths if needed
    if (!legacyNativeLibraryPath.Equals(p->mLegacyNativeLibraryPathString)) {
        p->mLegacyNativeLibraryPathString = legacyNativeLibraryPath;
    }

    // Update the required Cpu Abi
    pkg->mApplicationInfo->GetPrimaryCpuAbi(&p->mPrimaryCpuAbiString);
    pkg->mApplicationInfo->GetSecondaryCpuAbi(&p->mSecondaryCpuAbiString);
    p->mCpuAbiOverrideString = pkg->mCpuAbiOverride;
    // Update version code if needed
    if (pkg->mVersionCode != p->mVersionCode) {
        p->mVersionCode = pkg->mVersionCode;
    }
    // Update signatures if needed.
    if (p->mSignatures->mSignatures == NULL) {
        p->mSignatures->AssignSignatures(pkg->mSignatures);
    }
    // Update flags if needed.
    Int32 flags;
    pkg->mApplicationInfo->GetFlags(&flags);
    if (flags != p->mPkgFlags) {
        p->mPkgFlags = flags;
    }
    // If this app defines a shared user id initialize
    // the shared user signatures as well.
    if (p->mSharedUser != NULL && p->mSharedUser->mSignatures->mSignatures == NULL) {
        p->mSharedUser->mSignatures->AssignSignatures(pkg->mSignatures);
    }
    AddPackageSettingLPw(p, pkg->mPackageName, p->mSharedUser);
}

void Settings::AddPackageSettingLPw(
    /* [in] */ PackageSetting* p,
    /* [in] */ const String& name,
    /* [in] */ SharedUserSetting* sharedUser)
{
    // begin from this
    mPackages[name] = p;
    if (sharedUser != NULL) {
        if (p->mSharedUser != NULL && p->mSharedUser.Get() != sharedUser) {
            String str1, str2;
            p->mSharedUser->ToString(&str1);
            sharedUser->ToString(&str2);
            CPackageManagerService::ReportSettingsProblem(ILogHelper::ERROR,
                    String("Package ") + p->mName + " was user "
                    + str1 + " but is now " + str2
                    + "; I am not changing its files so it will probably fail!");
            p->mSharedUser->RemovePackage(p);
        }
        else if (p->mAppId != sharedUser->mUserId) {
            String str;
            sharedUser->ToString(&str);
            CPackageManagerService::ReportSettingsProblem(ILogHelper::ERROR,
                    String("Package ") + p->mName + " was user id " + StringUtils::ToString(p->mAppId)
                    + " but is now user " + str
                    + " with id " + StringUtils::ToString(sharedUser->mUserId)
                    + "; I am not changing its files so it will probably fail!");
        }

        sharedUser->AddPackage(p);
        p->mSharedUser = sharedUser;
        p->mAppId = sharedUser->mUserId;
    }
}

void Settings::UpdateSharedUserPermsLPw(
    /* [in] */ PackageSetting* deletedPs,
    /* [in] */ ArrayOf<Int32>* globalGids)
{
    if ((deletedPs == NULL) || (deletedPs->mPkg == NULL)) {
        Slogger::I(TAG,
               "Trying to update info for null package. Just ignoring");
        return;
    }
    // No sharedUserId
    if (deletedPs->mSharedUser == NULL) {
        return;
    }
    AutoPtr<SharedUserSetting> sus = deletedPs->mSharedUser;
    // Update permissions
    List<String>::Iterator it;
    for (it = deletedPs->mPkg->mRequestedPermissions.Begin();
            it != deletedPs->mPkg->mRequestedPermissions.End(); ++it) {
        String eachPerm = *it;
        Boolean used = FALSE;
        if (sus->mGrantedPermissions.Find(eachPerm) == sus->mGrantedPermissions.End()) {
            continue;
        }
        HashSet<AutoPtr<PackageSetting> >::Iterator pit;
        for (pit = sus->mPackages.Begin(); pit != sus->mPackages.End(); ++pit) {
            PackageSetting* pkg = *pit;
            if (pkg->mPkg != NULL &&
                    !pkg->mPkg->mPackageName.Equals(deletedPs->mPkg->mPackageName) &&
                    (Find(pkg->mPkg->mRequestedPermissions.Begin(),
                        pkg->mPkg->mRequestedPermissions.End(), eachPerm) !=
                        pkg->mPkg->mRequestedPermissions.End())) {
                used = TRUE;
                break;
            }
        }
        if (!used) {
            // can safely delete this permission from list
            sus->mGrantedPermissions.Erase(eachPerm);
        }
    }
    // Update gids
    AutoPtr< ArrayOf<Int32> > newGids = globalGids;
    HashSet<String>::Iterator sit;
    for (sit = sus->mGrantedPermissions.Begin(); sit != sus->mGrantedPermissions.End(); ++sit) {
        String eachPerm = *sit;
        AutoPtr<BasePermission> bp;
        HashMap<String, AutoPtr<BasePermission> >::Iterator bit = mPermissions.Find(eachPerm);
        if (bit != mPermissions.End()) {
            bp = bit->mSecond;
        }
        if (bp != NULL) {
            newGids = CPackageManagerService::AppendInts(newGids, bp->mGids);
        }
    }
    sus->mGids = newGids;
}

Int32 Settings::RemovePackageLPw(
    /* [in] */ const String& name)
{
    AutoPtr<PackageSetting> p;
    HashMap<String, AutoPtr<PackageSetting> >::Iterator it = mPackages.Find(name);
    if (it != mPackages.End()) {
        p = it->mSecond;
    }
    if (p != NULL) {
        mPackages.Erase(it);
        if (p->mSharedUser != NULL) {
            p->mSharedUser->RemovePackage(p);
            if (p->mSharedUser->mPackages.Begin() == p->mSharedUser->mPackages.End()) {
                mSharedUsers.Erase(p->mSharedUser->mName);
                RemoveUserIdLPw(p->mSharedUser->mUserId);
                p->mPkg = NULL;
                return p->mSharedUser->mUserId;
            }
        }
        else {
            RemoveUserIdLPw(p->mAppId);
            p->mPkg = NULL;
            return p->mAppId;
        }
    }
    return -1;
}

void Settings::ReplacePackageLPw(
    /* [in] */ const String& name,
    /* [in] */ PackageSetting* newp)
{
    AutoPtr<PackageSetting> p;
    HashMap<String, AutoPtr<PackageSetting> >::Iterator it = mPackages.Find(name);
    if (it != mPackages.End()) {
        p = it->mSecond;
    }
    if (p != NULL) {
        if (p->mSharedUser != NULL) {
            p->mSharedUser->RemovePackage(p);
            p->mSharedUser->AddPackage(newp);
        }
        else {
            ReplaceUserIdLPw(p->mAppId, (IObject*)newp);
        }
    }
    mPackages[name] = newp;
}

Boolean Settings::AddUserIdLPw(
    /* [in] */ Int32 uid,
    /* [in] */ IInterface* obj,
    /* [in] */ IInterface* name)
{
    if (uid > IProcess::LAST_APPLICATION_UID) {
        return FALSE;
    }

    if (uid >= IProcess::FIRST_APPLICATION_UID) {
        Int32 N = mUserIds.GetSize();
        Int32 index = uid - IProcess::FIRST_APPLICATION_UID;
        while (index >= N) {
            mUserIds.PushBack(NULL);
            N++;
        }
        if (mUserIds[index] != NULL) {
            StringBuilder sb;
            sb += "Adding duplicate user id: ";
            sb += uid;
            sb += " name=";
            sb += "name";
            String msg;
            CPackageManagerService::ReportSettingsProblem(ILogHelper::ERROR, (sb.ToString(&msg), msg));
            return FALSE;
        }
        mUserIds[index] = obj;
    }
    else {
        if (mOtherUserIds[uid] != NULL) {
            StringBuilder sb;
            sb += "Adding duplicate shared id: ";
            sb += uid;
            sb += " name=";
            sb += "name";
            String msg;
            CPackageManagerService::ReportSettingsProblem(ILogHelper::ERROR, (sb.ToString(&msg), msg));
            return FALSE;
        }
        mOtherUserIds[uid] = obj;
    }
    return TRUE;
}

AutoPtr<IInterface> Settings::GetUserIdLPr(
    /* [in] */ Int32 uid)
{
    if (uid >= IProcess::FIRST_APPLICATION_UID) {
        Int32 N = mUserIds.GetSize();
        Int32 index = uid - IProcess::FIRST_APPLICATION_UID;
        return index < N ? mUserIds[index] : NULL;
    }
    else {
        AutoPtr<IInterface> obj;
        HashMap<Int32, AutoPtr<IInterface> >::Iterator it = mOtherUserIds.Find(uid);
        if (it != mOtherUserIds.End()) {
            obj = it->mSecond;
        }
        return obj;
    }
}

void Settings::RemoveUserIdLPw(
    /* [in] */ Int32 uid)
{
    if (uid >= IProcess::FIRST_APPLICATION_UID) {
        Int32 N = mUserIds.GetSize();
        Int32 index = uid - IProcess::FIRST_APPLICATION_UID;
        if (index < N) mUserIds[index] = NULL;
    }
    else {
        mOtherUserIds.Erase(uid);
    }
    SetFirstAvailableUid(uid + 1);
}

void Settings::ReplaceUserIdLPw(
    /* [in] */ Int32 uid,
    /* [in] */ IInterface* obj)
{
    if (uid >= IProcess::FIRST_APPLICATION_UID) {
        Int32 N = mUserIds.GetSize();
        Int32 index = uid - IProcess::FIRST_APPLICATION_UID;
        if (index < N) mUserIds[index] = obj;
    }
    else {
        mOtherUserIds[uid] = obj;
    }
}

AutoPtr<PreferredIntentResolver> Settings::EditPreferredActivitiesLPw(
    /* [in] */ Int32 userId)
{
    AutoPtr<PreferredIntentResolver> pir;
    HashMap<Int32, AutoPtr<PreferredIntentResolver> >::Iterator it = mPreferredActivities.Find(userId);
    if (it != mPreferredActivities.End()) {
        pir = it->mSecond;
    }
    if (pir == NULL) {
        pir = new PreferredIntentResolver();
        mPreferredActivities[userId] = pir;
    }
    return pir;
}

AutoPtr<PersistentPreferredIntentResolver> Settings::EditPersistentPreferredActivitiesLPw(
    /* [in] */ Int32 userId)
{
    AutoPtr<PersistentPreferredIntentResolver> ppir;
    HashMap<Int32, AutoPtr<PersistentPreferredIntentResolver> >::Iterator it = mPersistentPreferredActivities.Find(userId);
    if (it != mPersistentPreferredActivities.End()) {
        ppir = it->mSecond;
    }
    if (ppir == NULL) {
        ppir = new PersistentPreferredIntentResolver();
        mPersistentPreferredActivities[userId] = ppir;
    }
    return ppir;
}

AutoPtr<CrossProfileIntentResolver> Settings::EditCrossProfileIntentResolverLPw(
    /* [in] */ Int32 userId)
{
    AutoPtr<CrossProfileIntentResolver> cpir;
    HashMap<Int32, AutoPtr<CrossProfileIntentResolver> >::Iterator it = mCrossProfileIntentResolvers.Find(userId);
    if (it != mCrossProfileIntentResolvers.End()) {
        cpir = it->mSecond;
    }
    if (cpir == NULL) {
        cpir = new CrossProfileIntentResolver();
        mCrossProfileIntentResolvers[userId] = cpir;
    }
    return cpir;
}

AutoPtr<IFile> Settings::GetUserPackagesStateFile(
    /* [in] */ Int32 userId)
{
    AutoPtr<IFile> stateFile;
    AutoPtr<IFile> dir = Environment::GetUserSystemDirectory(userId);
    CFile::New(dir, String("package-restrictions.xml"), (IFile**)&stateFile);
    return stateFile;
}

AutoPtr<IFile> Settings::GetUserPackagesStateBackupFile(
    /* [in] */ Int32 userId)
{
    AutoPtr<IFile> backupFile;
    AutoPtr<IFile> dir = Environment::GetUserSystemDirectory(userId);
    CFile::New(dir, String("package-restrictions-backup.xml"), (IFile**)&backupFile);
    return backupFile;
}

void Settings::WriteAllUsersPackageRestrictionsLPr()
{
    AutoPtr<IList> users = GetAllUsers();
    if (users == NULL) return;

    AutoPtr<IIterator> it;
    users->GetIterator((IIterator**)&it);
    Boolean hasNext;
    while (it->HasNext(&hasNext), hasNext) {
        AutoPtr<IInterface> value;
        it->GetNext((IInterface**)&value);
        AutoPtr<IUserInfo> userInfo = IUserInfo::Probe(value);
        Int32 userId;
        userInfo->GetId(&userId);
        WritePackageRestrictionsLPr(userId);
    }
}

void Settings::ReadAllUsersPackageRestrictionsLPr()
{
    AutoPtr<IList> users = GetAllUsers();
    if (users == NULL) {
        ReadPackageRestrictionsLPr(0);
        return;
    }

    AutoPtr<IIterator> it;
    users->GetIterator((IIterator**)&it);
    Boolean hasNext;
    while (it->HasNext(&hasNext), hasNext) {
        AutoPtr<IInterface> value;
        it->GetNext((IInterface**)&value);
        AutoPtr<IUserInfo> userInfo = IUserInfo::Probe(value);
        Int32 userId;
        userInfo->GetId(&userId);
        ReadPackageRestrictionsLPr(userId);
    }
}

Boolean Settings::IsInternalDatabaseVersionOlderThan(
    /* [in] */ Int32 version)
{
    return mInternalDatabaseVersion < version;
}

Boolean Settings::IsExternalDatabaseVersionOlderThan(
    /* [in] */ Int32 version)
{
    return mExternalDatabaseVersion < version;
}

void Settings::UpdateInternalDatabaseVersion()
{
    mInternalDatabaseVersion = CURRENT_DATABASE_VERSION;
}

void Settings::UpdateExternalDatabaseVersion()
{
    mExternalDatabaseVersion = CURRENT_DATABASE_VERSION;
}

ECode Settings::ReadPreferredActivitiesLPw(
    /* [in] */ IXmlPullParser* parser,
    /* [in] */ Int32 userId)
{
    Int32 outerDepth, type, depth;
    parser->GetDepth(&outerDepth);
    while ((parser->Next(&type), type != IXmlPullParser::END_DOCUMENT)
            && (type != IXmlPullParser::END_TAG || (parser->GetDepth(&depth), depth > outerDepth))) {
        if (type == IXmlPullParser::END_TAG || type == IXmlPullParser::TEXT) {
            continue;
        }

        if (type != IXmlPullParser::START_TAG) {
            continue;
        }

        String tagName;
        parser->GetName(&tagName);
        if (tagName.Equals(TAG_ITEM)) {
            AutoPtr<PreferredActivity> pa = new PreferredActivity(parser);
            if (pa->mPref->GetParseError().IsNull()) {
                EditPreferredActivitiesLPw(userId)->AddFilter(pa);
            }
            else {
                String parserPositionDesc;
                parser->GetPositionDescription(&parserPositionDesc);
                CPackageManagerService::ReportSettingsProblem(ILogHelper::WARN,
                       String("Error in package manager settings: <preferred-activity> ")
                               + pa->mPref->GetParseError() + String(" at ")
                               + parserPositionDesc);
            }
        }
        else {
            String nameStr;
            CPackageManagerService::ReportSettingsProblem(ILogHelper::WARN,
                   String("Unknown element under <preferred-activities>: ") + (parser->GetName(&nameStr), nameStr));
            XmlUtils::SkipCurrentTag(parser);
        }
    }
    return NOERROR;
}

ECode Settings::ReadPersistentPreferredActivitiesLPw(
    /* [in] */ IXmlPullParser* parser,
    /* [in] */ Int32 userId)
{
    Int32 outerDepth;
    parser->GetDepth(&outerDepth);
    Int32 type, depth;
    while ((parser->Next(&type), type != IXmlPullParser::END_DOCUMENT)
            && (type != IXmlPullParser::END_TAG || (parser->GetDepth(&depth), depth > outerDepth))) {
        if (type == IXmlPullParser::END_TAG || type == IXmlPullParser::TEXT) {
            continue;
        }
        String tagName;
        parser->GetName(&tagName);
        if (tagName.Equals(TAG_ITEM)) {
            AutoPtr<PersistentPreferredActivity> ppa = new PersistentPreferredActivity(parser);
            EditPersistentPreferredActivitiesLPw(userId)->AddFilter(ppa);
        }
        else {
            CPackageManagerService::ReportSettingsProblem(ILogHelper::WARN,
                    String("Unknown element under <") + TAG_PERSISTENT_PREFERRED_ACTIVITIES + ">: "
                    + tagName);
            XmlUtils::SkipCurrentTag(parser);
        }
    }
    return NOERROR;
}

ECode Settings::ReadCrossProfileIntentFiltersLPw(
    /* [in] */ IXmlPullParser* parser,
    /* [in] */ Int32 userId)
{
    Int32 outerDepth;
    parser->GetDepth(&outerDepth);
    Int32 type, depth;
    while ((parser->Next(&type), type != IXmlPullParser::END_DOCUMENT)
            && (type != IXmlPullParser::END_TAG || (parser->GetDepth(&depth), depth > outerDepth))) {
        if (type == IXmlPullParser::END_TAG || type == IXmlPullParser::TEXT) {
            continue;
        }
        String tagName;
        parser->GetName(&tagName);
        if (tagName.Equals(TAG_ITEM)) {
            AutoPtr<CrossProfileIntentFilter> cpif = new CrossProfileIntentFilter(parser);
            EditCrossProfileIntentResolverLPw(userId)->AddFilter(cpif);
        }
        else {
            String msg = String("Unknown element under ") +  TAG_CROSS_PROFILE_INTENT_FILTERS + ": " +
                    tagName;
            CPackageManagerService::ReportSettingsProblem(ILogHelper::WARN, msg);
            XmlUtils::SkipCurrentTag(parser);
        }
    }
    return NOERROR;
}

void Settings::ReadPackageRestrictionsLPr(
    /* [in] */ Int32 userId)
{
    if (DEBUG_MU) {
        Slogger::I(TAG, "Reading package restrictions for user= %d", userId);
    }
    AutoPtr<IFileInputStream> str;
    AutoPtr<IFile> userPackagesStateFile = GetUserPackagesStateFile(userId);
    AutoPtr<IFile> backupFile = GetUserPackagesStateBackupFile(userId);
    Boolean result;
    String temp;
    ECode ecode = NOERROR;
    if (backupFile->Exists(&result), result) {
        //try {
        CFileInputStream::New(backupFile, (IFileInputStream**)&str);
        mReadMessages->Append(String("Reading from backup stopped packages file\n"));
        CPackageManagerService::ReportSettingsProblem(ILogHelper::INFO,
                String("Need to read from backup stopped packages file"));
        if (userPackagesStateFile->Exists(&result), result) {
            // If both the backup and normal file exist, we
            // ignore the normal one since it might have been
            // corrupted.
            Slogger::W(TAG, String("Cleaning up stopped packages file ")
                    + (userPackagesStateFile->ToString(&temp), temp));
            userPackagesStateFile->Delete(&result);
        }
        // } catch (java.io.IOException e) {
        //     // We'll try for the normal settings file.
        // }
    }

    AutoPtr<PackageSetting> ps;
    AutoPtr<IXmlPullParser> parser;

    //try
    if (str == NULL) {
        if (userPackagesStateFile->Exists(&result), !result) {
            mReadMessages->Append(String("No stopped packages file found\n"));
            CPackageManagerService::ReportSettingsProblem(ILogHelper::INFO,
                     String("No stopped packages file; assuming all started"));
            // At first boot, make sure no packages are stopped.
            // We usually want to have third party apps initialize
            // in the stopped state, but not at first boot.  Also
            // consider all applications to be installed.
            HashMap<String, AutoPtr<PackageSetting> >::Iterator it = mPackages.Begin();
            for (; it != mPackages.End(); it++) {
                AutoPtr<PackageSetting> pkg = it->mSecond;
                pkg->SetUserState(userId, IPackageManager::COMPONENT_ENABLED_STATE_DEFAULT,
                        TRUE,   // installed
                        FALSE,  // stopped
                        FALSE,  // notLaunched
                        FALSE,  // hidden
                        String(NULL), NULL, NULL,
                        FALSE // blockUninstall
                        );
            }
            return;
        }
        FAIL_GOTO(ecode = CFileInputStream::New(userPackagesStateFile, (IFileInputStream**)&str), EXIT);
    }

    Xml::NewPullParser((IXmlPullParser**)&parser);
    FAIL_GOTO(ecode = parser->SetInput(IInputStream::Probe(str), String(NULL)), EXIT);

    Int32 type;
    while ((parser->Next(&type), type) != IXmlPullParser::START_TAG
               && type != IXmlPullParser::END_DOCUMENT) {
        ;
    }

    if (type != IXmlPullParser::START_TAG) {
        mReadMessages->Append(String("No start tag found in package restrictions file\n"));
        CPackageManagerService::ReportSettingsProblem(ILogHelper::WARN,
                String("No start tag found in package manager stopped packages"));
        return;
    }

    Int32 outerDepth, outerDepth1;
    FAIL_GOTO(ecode = parser->GetDepth(&outerDepth), EXIT)
    while ((parser->Next(&type), type) != IXmlPullParser::END_DOCUMENT
           && (type != IXmlPullParser::END_TAG
                   || (parser->GetDepth(&outerDepth1), outerDepth1) > outerDepth)) {
        if (type == IXmlPullParser::END_TAG || type == IXmlPullParser::TEXT) {
            continue;
        }
        if (type != IXmlPullParser::START_TAG) {
            continue;
        }

        String tagName;
        FAIL_GOTO(ecode = parser->GetName(&tagName), EXIT);
        if (tagName.Equals(TAG_PACKAGE)) {
            String name;
            FAIL_GOTO(ecode = parser->GetAttributeValue(String(NULL), ATTR_NAME, &name), EXIT)
            HashMap<String, AutoPtr<PackageSetting> >::Iterator it2 = mPackages.Find(name);
            ps = (it2 != mPackages.End() ? it2->mSecond : NULL);
            if (ps == NULL) {
                Slogger::W(TAG, String("No package known for stopped package: ")
                        + name);
                XmlUtils::SkipCurrentTag(parser);
                continue;
            }
            String enabledStr;
            FAIL_GOTO(ecode = parser->GetAttributeValue(String(NULL), ATTR_ENABLED, &enabledStr), EXIT);
            Int32 enabled = enabledStr.IsNull()
                    ? IPackageManager::COMPONENT_ENABLED_STATE_DEFAULT : StringUtils::ParseInt32(enabledStr);
            String enabledCaller;
            FAIL_GOTO(ecode = parser->GetAttributeValue(String(NULL), ATTR_ENABLED_CALLER, &enabledCaller), EXIT)
            String installedStr;
            FAIL_GOTO(ecode = parser->GetAttributeValue(String(NULL), ATTR_INSTALLED, &installedStr), EXIT)
            Boolean installed = installedStr.IsNull() ? TRUE : StringUtils::ParseBoolean(installedStr);
            String stoppedStr;
            FAIL_GOTO(ecode = parser->GetAttributeValue(String(NULL), ATTR_STOPPED, &stoppedStr), EXIT)
            Boolean stopped = stoppedStr.IsNull() ? FALSE : StringUtils::ParseBoolean(stoppedStr);
            // For backwards compatibility with the previous name of "blocked", which
            // now means hidden, read the old attribute as well.
            String blockedStr;
            FAIL_GOTO(ecode = parser->GetAttributeValue(String(NULL), ATTR_BLOCKED, &blockedStr), EXIT)
            Boolean hidden = blockedStr.IsNull() ? FALSE : StringUtils::ParseBoolean(blockedStr);
            String hiddenStr;
            FAIL_GOTO(ecode = parser->GetAttributeValue(String(NULL), ATTR_HIDDEN, &hiddenStr), EXIT)
            hidden = hiddenStr.IsNull() ? hidden : StringUtils::ParseBoolean(hiddenStr);
            String notLaunchedStr;
            FAIL_GOTO(ecode = parser->GetAttributeValue(String(NULL), ATTR_NOT_LAUNCHED, &notLaunchedStr), EXIT)
            Boolean notLaunched = stoppedStr.IsNull() ? FALSE : StringUtils::ParseBoolean(notLaunchedStr);
            String blockUninstallStr;
            FAIL_GOTO(ecode = parser->GetAttributeValue(String(NULL), ATTR_BLOCK_UNINSTALL, &blockUninstallStr), EXIT)
            Boolean blockUninstall = blockUninstallStr.IsNull() ? FALSE : StringUtils::ParseBoolean(blockUninstallStr);

            AutoPtr<HashSet<String> > enabledComponents;
            AutoPtr<HashSet<String> > disabledComponents;

            Int32 packageDepth, packageDepth1;
            FAIL_GOTO(ecode = parser->GetDepth(&packageDepth), EXIT);
            while ((parser->Next(&type), type) != IXmlPullParser::END_DOCUMENT
                    && (type != IXmlPullParser::END_TAG
                    || (parser->GetDepth(&packageDepth1), packageDepth1) > packageDepth)) {

                if (type == IXmlPullParser::END_TAG || type == IXmlPullParser::TEXT) {
                    continue;
                }

                if (type != IXmlPullParser::START_TAG) {
                    continue;
                }

                FAIL_GOTO(ecode = parser->GetName(&tagName), EXIT);
                if (tagName.Equals(TAG_ENABLED_COMPONENTS)) {
                    assert(enabledComponents == NULL);
                    enabledComponents = ReadComponentsLPr(parser);
                }
                else if (tagName.Equals(TAG_DISABLED_COMPONENTS)) {
                    assert(disabledComponents == NULL);
                    disabledComponents = ReadComponentsLPr(parser);
                }
            }

            ps->SetUserState(userId, enabled, installed, stopped, notLaunched, hidden,
                    enabledCaller, enabledComponents, disabledComponents, blockUninstall);
        }
        else if (tagName.Equals("preferred-activities")) {
            ReadPreferredActivitiesLPw(parser, userId);
        }
        else if (tagName.Equals(TAG_PERSISTENT_PREFERRED_ACTIVITIES)) {
            ReadPersistentPreferredActivitiesLPw(parser, userId);
        }
        else if (tagName.Equals(TAG_CROSS_PROFILE_INTENT_FILTERS)) {
            ReadCrossProfileIntentFiltersLPw(parser, userId);
        }
        else {
            Slogger::W(TAG, String("Unknown element under <stopped-packages>: ")
                    + (parser->GetName(&temp), temp.IsNull() ? String("NULL") : temp));
            XmlUtils::SkipCurrentTag(parser);
        }
    }

    FAIL_GOTO(ecode = ICloseable::Probe(str)->Close(), EXIT);

    // } catch (XmlPullParserException e) {
    //     mReadMessages.append("Error reading: " + e.toString());
    //     CPackageManagerService::ReportSettingsProblem(ILogHelper::ERROR,
    //             "Error reading stopped packages: " + e);
    //     Logger::wtf(TAG, "Error reading package manager stopped packages", e);

//     } catch (java.io.IOException e) {
//         mReadMessages.append("Error reading: " + e.toString());
//         PackageManagerService.reportSettingsProblem(Log.ERROR, "Error reading settings: " + e);
//         Log.wtf(PackageManagerService.TAG, "Error reading package manager stopped packages", e);
//     } catch (Exception e) {
//         //add by aw lisidong@allwinnertech.com
//         //For some reason,Parse xml will fail for all other situations,remove some files so late will recreate them
//         mReadMessages.append("Error reading: " + e.toString());
//         PackageManagerService.reportSettingsProblem(Log.ERROR, "Error reading settings: " + e);
//         Log.wtf(PackageManagerService.TAG, "Error reading package manager stopped packages", e);
//         userPackagesStateFile.delete();
//         backupFile.delete();
//         throw new RuntimeException("Error readPackageRestrictionsLPr userId=" + userId + ", remove that bad file");
//     }

EXIT:
    if ((ECode)E_XML_PULL_PARSER_EXCEPTION == ecode) {
        mReadMessages->Append(String("Error reading:  + e.toString()"));
        CPackageManagerService::ReportSettingsProblem(ILogHelper::ERROR,
                String("Error reading stopped packages: "));
        Slogger::E(TAG, "Error reading package manager stopped packages, e");
    }
    else if ((ECode)E_IO_EXCEPTION == ecode) {
        mReadMessages->Append(String("Error reading:  + e.toString()"));
        CPackageManagerService::ReportSettingsProblem(ILogHelper::ERROR, String("Error reading settings:  + e"));
        Slogger::E(TAG, "Error reading package manager stopped packages, e");
    }
}

AutoPtr< HashSet<String> > Settings::ReadComponentsLPr(
    /* [in] */ IXmlPullParser* parser)
{
    AutoPtr< HashSet<String> > components;
    Int32 outerDepth, type, depth;
    parser->GetDepth(&outerDepth);
    String tagName;
    while (((parser->Next(&type), type) != IXmlPullParser::END_DOCUMENT)
            && (type != IXmlPullParser::END_TAG
            || (parser->GetDepth(&depth), depth) > outerDepth)) {
        if (type == IXmlPullParser::END_TAG
                || type == IXmlPullParser::TEXT) {
            continue;
        }
        if (type != IXmlPullParser::START_TAG) {
            continue;
        }
        parser->GetName(&tagName);
        if (tagName.Equals(TAG_ITEM)) {
            String componentName;
            parser->GetAttributeValue(String(NULL), ATTR_NAME, &componentName);
            if (!componentName.IsNull()) {
                if (components == NULL) {
                    components = new HashSet<String>();
                }
                components->Insert(componentName);
            }
        }
    }
    return components;
}

ECode Settings::WritePreferredActivitiesLPr(
    /* [in] */ IXmlSerializer* serializer,
    /* [in] */ Int32 userId,
    /* [in] */ Boolean full)
{
    serializer->WriteStartTag(String(NULL), String("preferred-activities"));
    AutoPtr<PreferredIntentResolver> pir;
    HashMap<Int32, AutoPtr<PreferredIntentResolver> >::Iterator it = mPreferredActivities.Find(userId);
    if (it != mPreferredActivities.End()) {
        pir = it->mSecond;
    }
    if (pir != NULL) {
        AutoPtr<HashSet<AutoPtr<PreferredActivity> > > set = pir->FilterSet();
        HashSet<AutoPtr<PreferredActivity> >::Iterator sit;
        for (sit = set->Begin(); sit != set->End(); ++sit) {
            serializer->WriteStartTag(String(NULL), TAG_ITEM);
            (*sit)->WriteToXml(serializer, full);
            serializer->WriteEndTag(String(NULL), TAG_ITEM);
        }
    }
    serializer->WriteEndTag(String(NULL), String("preferred-activities"));
    return NOERROR;
}

ECode Settings::WritePersistentPreferredActivitiesLPr(
    /* [in] */ IXmlSerializer* serializer,
    /* [in] */ Int32 userId)
{
    serializer->WriteStartTag(String(NULL), TAG_PERSISTENT_PREFERRED_ACTIVITIES);
    AutoPtr<PersistentPreferredIntentResolver> ppir;
    HashMap<Int32, AutoPtr<PersistentPreferredIntentResolver> >::Iterator it = mPersistentPreferredActivities.Find(userId);
    if (it != mPersistentPreferredActivities.End()) {
        ppir = it->mSecond;
    }
    if (ppir != NULL) {
        AutoPtr<HashSet< AutoPtr<PersistentPreferredActivity> > > ppas = ppir->FilterSet();
        HashSet<AutoPtr<PersistentPreferredActivity> >::Iterator ppaIt = ppas->Begin();
        for (; ppaIt != ppas->End(); ++ppaIt) {
            serializer->WriteStartTag(String(NULL), TAG_ITEM);
            (*ppaIt)->WriteToXml(serializer);
            serializer->WriteEndTag(String(NULL), TAG_ITEM);
        }
    }
    serializer->WriteEndTag(String(NULL), TAG_PERSISTENT_PREFERRED_ACTIVITIES);
    return NOERROR;
}

ECode Settings::WriteCrossProfileIntentFiltersLPr(
    /* [in] */ IXmlSerializer* serializer,
    /* [in] */ Int32 userId)
{
    serializer->WriteStartTag(String(NULL), TAG_CROSS_PROFILE_INTENT_FILTERS);
    AutoPtr<CrossProfileIntentResolver> cpir;
    HashMap<Int32, AutoPtr<CrossProfileIntentResolver> >::Iterator it = mCrossProfileIntentResolvers.Find(userId);
    if (it != mCrossProfileIntentResolvers.End()) {
        cpir = it->mSecond;
    }
    if (cpir != NULL) {
        AutoPtr<HashSet< AutoPtr<CrossProfileIntentFilter> > > cpifs = cpir->FilterSet();
        HashSet<AutoPtr<CrossProfileIntentFilter> >::Iterator cpifIt = cpifs->Begin();
        for (; cpifIt != cpifs->End(); ++cpifIt) {
            serializer->WriteStartTag(String(NULL), TAG_ITEM);
            (*cpifIt)->WriteToXml(serializer);
            serializer->WriteEndTag(String(NULL), TAG_ITEM);
        }
    }
    serializer->WriteEndTag(String(NULL), TAG_CROSS_PROFILE_INTENT_FILTERS);
    return NOERROR;
}

void Settings::WritePackageRestrictionsLPr(
    /* [in] */ Int32 userId)
{
    if (DEBUG_MU) {
        Slogger::I(TAG, "Writing package restrictions for user= %d", userId);
    }
    // Keep the old stopped packages around until we know the new ones have
    // been successfully written.
    AutoPtr<IFile> userPackagesStateFile = GetUserPackagesStateFile(userId);
    AutoPtr<IFile> backupFile = GetUserPackagesStateBackupFile(userId);
    String temp;
    Boolean result;
    userPackagesStateFile->GetParent(&temp);
    AutoPtr<IFile> stateFileParent;
    CFile::New(temp, (IFile**)&stateFileParent);
    stateFileParent->Mkdirs(&result);

    if (userPackagesStateFile->Exists(&result), result) {
        // Presence of backup settings file indicates that we failed
        // to persist packages earlier. So preserve the older
        // backup for future reference since the current packages
        // might have been corrupted.
        if (backupFile->Exists(&result), !result) {
            if (userPackagesStateFile->RenameTo(backupFile, &result), !result) {
                Slogger::E(TAG, String("Unable to backup user packages state file, ")
                        + String("current changes will be lost at reboot"));
                return;
            }
        }
        else {
            userPackagesStateFile->Delete(&result);
            Slogger::W(TAG, "Preserving older stopped packages backup");
        }
    }
    //try
    AutoPtr<IFileOutputStream> fstr;
    CFileOutputStream::New(userPackagesStateFile, (IFileOutputStream**)&fstr);
    AutoPtr<IBufferedOutputStream> str;
    CBufferedOutputStream::New(IOutputStream::Probe(fstr), (IBufferedOutputStream**)&str);

    AutoPtr<IXmlSerializer> serializer;
    CFastXmlSerializer::New((IXmlSerializer**)&serializer);
    serializer->SetOutput(IOutputStream::Probe(str), String("utf-8"));
    serializer->StartDocument(String(NULL), TRUE);
    serializer->SetFeature(String("http://xmlpull.org/v1/doc/features.html#indent-output"), TRUE);

    serializer->WriteStartTag(String(NULL), TAG_PACKAGE_RESTRICTIONS);
    HashMap<String, AutoPtr<PackageSetting> >::Iterator it = mPackages.Begin();
    for (; it  != mPackages.End(); it++) {
        AutoPtr<PackageSetting> pkg = it->mSecond;
        AutoPtr<PackageUserState> ustate = pkg->ReadUserState(userId);
        if (ustate->mStopped || ustate->mNotLaunched || !ustate->mInstalled
                || ustate->mEnabled != IPackageManager::COMPONENT_ENABLED_STATE_DEFAULT
                || ustate->mHidden
                || (ustate->mEnabledComponents != NULL
                        && !ustate->mEnabledComponents->IsEmpty())
                || (ustate->mDisabledComponents != NULL
                        && !ustate->mDisabledComponents->IsEmpty())
                || ustate->mBlockUninstall) {
            serializer->WriteStartTag(String(NULL), TAG_PACKAGE);
            serializer->WriteAttribute(String(NULL), ATTR_NAME, pkg->mName);
            if (DEBUG_MU) {
                Slogger::I(TAG, "  pkg=%s, state=%d", pkg->mName.string(), ustate->mEnabled);
            }
            if (!ustate->mInstalled) {
                serializer->WriteAttribute(String(NULL), ATTR_INSTALLED, String("false"));
            }
            if (ustate->mStopped) {
                serializer->WriteAttribute(String(NULL), ATTR_STOPPED, String("true"));
            }
            if (ustate->mNotLaunched) {
                serializer->WriteAttribute(String(NULL), ATTR_NOT_LAUNCHED, String("true"));
            }
            if (ustate->mHidden) {
                serializer->WriteAttribute(String(NULL), ATTR_HIDDEN, String("true"));
            }
            if (ustate->mBlockUninstall) {
                serializer->WriteAttribute(String(NULL), ATTR_BLOCK_UNINSTALL, String("true"));
            }
            if (ustate->mEnabled != IPackageManager::COMPONENT_ENABLED_STATE_DEFAULT) {
                serializer->WriteAttribute(String(NULL), ATTR_ENABLED,
                        StringUtils::ToString(ustate->mEnabled));
                if (!ustate->mLastDisableAppCaller.IsNull()) {
                    serializer->WriteAttribute(String(NULL), ATTR_ENABLED_CALLER, ustate->mLastDisableAppCaller);
                }
            }
            if (ustate->mEnabledComponents != NULL
                    && ustate->mEnabledComponents->IsEmpty() == FALSE) {
                serializer->WriteStartTag(String(NULL), TAG_ENABLED_COMPONENTS);
                HashSet<String>::Iterator it2 = ustate->mEnabledComponents->Begin();
                for (; it2 != ustate->mEnabledComponents->End(); it2++) {
                    String name = (*it2);
                    serializer->WriteStartTag(String(NULL), TAG_ITEM);
                    serializer->WriteAttribute(String(NULL), ATTR_NAME, name);
                    serializer->WriteEndTag(String(NULL), TAG_ITEM);
                }
                serializer->WriteEndTag(String(NULL), TAG_ENABLED_COMPONENTS);
            }
            if (ustate->mDisabledComponents != NULL
                    && ustate->mDisabledComponents->IsEmpty() == FALSE) {
                serializer->WriteStartTag(String(NULL), TAG_DISABLED_COMPONENTS);
                HashSet<String>::Iterator it3 = ustate->mDisabledComponents->Begin();
                for (; it3 != ustate->mDisabledComponents->End(); it3++) {
                    String name = (*it3);
                    serializer->WriteStartTag(String(NULL), TAG_ITEM);
                    serializer->WriteAttribute(String(NULL), ATTR_NAME, name);
                    serializer->WriteEndTag(String(NULL), TAG_ITEM);
                }
                serializer->WriteEndTag(String(NULL), TAG_DISABLED_COMPONENTS);
            }
            serializer->WriteEndTag(String(NULL), TAG_PACKAGE);
        }
    }

    WritePreferredActivitiesLPr(serializer, userId, TRUE);

    WritePersistentPreferredActivitiesLPr(serializer, userId);

    WriteCrossProfileIntentFiltersLPr(serializer, userId);

    serializer->WriteEndTag(String(NULL), TAG_PACKAGE_RESTRICTIONS);
    serializer->EndDocument();

    IFlushable::Probe(str)->Flush();
    result = FileUtils::Sync(fstr);
    ICloseable::Probe(str)->Close();

    // New settings successfully written, old ones are no longer
    // needed.
    backupFile->Delete(&result);
    FileUtils::SetPermissions((userPackagesStateFile->ToString(&temp), temp),
            FileUtils::sS_IRUSR|FileUtils::sS_IWUSR
            |FileUtils::sS_IRGRP|FileUtils::sS_IWGRP, -1, -1);

    // Done, all is good!
    return;
    // } catch(java.io.IOException e) {
    //     Log.wtf(TAG,
    //             "Unable to write package manager user packages state, "
    //             + " current changes will be lost at reboot", e);
    // }

    // Clean up partially written files
    // if (userPackagesStateFile->Exists(&result), result) {
    //     if (userPackagesStateFile->Delete(&result), !result) {
    //         Slogger::I(TAG, String("Failed to clean up mangled file: ")
    //                 + (mStoppedPackagesFilename->ToString(&temp), temp));
    //     }
    // }
}

void Settings::ReadStoppedLPw()
{
    AutoPtr<IFileInputStream> str;
    Boolean result;
    String temp;
    if (mBackupStoppedPackagesFilename->Exists(&result), result) {
        //try {
        CFileInputStream::New(mBackupStoppedPackagesFilename, (IFileInputStream**)&str);
        mReadMessages->Append(String("Reading from backup stopped packages file\n"));
        CPackageManagerService::ReportSettingsProblem(ILogHelper::INFO,
                String("Need to read from backup stopped packages file"));
        if (mSettingsFilename->Exists(&result), result) {
            // If both the backup and normal file exist, we
            // ignore the normal one since it might have been
            // corrupted.
            Slogger::W(TAG, String("Cleaning up stopped packages file ")
                    + (mStoppedPackagesFilename->ToString(&temp), temp));
            mStoppedPackagesFilename->Delete(&result);
        }
        // } catch (java.io.IOException e) {
        //     // We'll try for the normal settings file.
        // }
    }
    //try
    if (str == NULL) {
        if (mStoppedPackagesFilename->Exists(&result), !result) {
            mReadMessages->Append(String("No stopped packages file found\n"));
            CPackageManagerService::ReportSettingsProblem(ILogHelper::INFO,
                    String("No stopped packages file file; assuming all started"));
            // At first boot, make sure no packages are stopped.
            // We usually want to have third party apps initialize
            // in the stopped state, but not at first boot.
            HashMap<String, AutoPtr<PackageSetting> >::Iterator it = mPackages.Begin();
            for (; it != mPackages.End(); it++) {
                AutoPtr<PackageSetting> pkg = it->mSecond;
                pkg->SetStopped(FALSE, 0);
                pkg->SetNotLaunched(FALSE, 0);
            }
            return;
        }
        CFileInputStream::New(mStoppedPackagesFilename, (IFileInputStream**)&str);
    }
    AutoPtr<IXmlPullParser> parser;
    Xml::NewPullParser((IXmlPullParser**)&parser);
    parser->SetInput(IInputStream::Probe(str), String(NULL));

    Int32 type;
    while ((parser->Next(&type), type) != IXmlPullParser::START_TAG
               && type != IXmlPullParser::END_DOCUMENT) {
        ;
    }

    if (type != IXmlPullParser::START_TAG) {
        mReadMessages->Append(String("No start tag found in stopped packages file\n"));
        CPackageManagerService::ReportSettingsProblem(ILogHelper::WARN,
                String("No start tag found in package manager stopped packages"));
        return;
    }

    Int32 outerDepth, outerDepth1;
    parser->GetDepth(&outerDepth);
    while ((parser->Next(&type), type) != IXmlPullParser::END_DOCUMENT
           && (type != IXmlPullParser::END_TAG
                   || (parser->GetDepth(&outerDepth1), outerDepth1) > outerDepth)) {
        if (type == IXmlPullParser::END_TAG
                || type == IXmlPullParser::TEXT) {
            continue;
        }

        if (type != IXmlPullParser::START_TAG) {
            continue;
        }

        String tagName;
        parser->GetName(&tagName);
        if (tagName.Equals(TAG_PACKAGE)) {
            String name;
            parser->GetAttributeValue(String(NULL), ATTR_NAME, &name);
            HashMap<String, AutoPtr<PackageSetting> >::Iterator it2 = mPackages.Find(name);
            AutoPtr<PackageSetting> ps = (it2 != mPackages.End() ? it2->mSecond : NULL);
            if (ps != NULL) {
                ps->SetStopped(TRUE, 0);
                parser->GetAttributeValue(String(NULL), ATTR_NOT_LAUNCHED, &temp);
                if (temp.Equals("1")) {
                    ps->SetNotLaunched(TRUE, 0);
                }
            }
            else {
                Slogger::W(TAG,
                        String("No package known for stopped package: ") + name);
            }
            XmlUtils::SkipCurrentTag(parser);
        }
        else {
            Slogger::W(TAG, String("Unknown element under <stopped-packages>: ")
                  + (parser->GetName(&temp), temp.IsNull() ? String("NULL") : temp));
            XmlUtils::SkipCurrentTag(parser);
        }
    }

    ICloseable::Probe(str)->Close();

    // } catch (XmlPullParserException e) {
    //     mReadMessages->append("Error reading: " + e.toString());
    //     CPackageManagerService::ReportSettingsProblem(ILogHelper::ERROR,
    //             "Error reading stopped packages: " + e);
    //     Log.wtf(TAG, "Error reading package manager stopped packages", e);

    // } catch (java.io.IOException e) {
    //     mReadMessages->append("Error reading: " + e.toString());
    //     CPackageManagerService::ReportSettingsProblem(Log.ERROR, "Error reading settings: "ger::+ e);
    //     Log.wtf(TAG, "Error reading package manager stopped packages", e);

    // }
// EXIT:
//     if ((ECode)E_IO_EXCEPTION == ecode) {
//         mReadMessages->Append(String("Error reading:  + e.toString()"));
//         CPackageManagerService::ReportSettingsProblem(ILogHelper::ERROR,
//                 String("Error reading stopped packages:  + e"));
//         Slogger::E(TAG, "Error reading package manager stopped packages, e");
//     }
//     else if ((ECode)E_XML_PULL_PARSER_EXCEPTION == ecode) {
//         mReadMessages->Append(String("Error reading:  + e.toString()"));
//         CPackageManagerService::ReportSettingsProblem(ILogHelper::ERROR, String("Error reading settings: +e"));
//         Slogger::E(TAG, "Error reading package manager stopped packages, e");
//     }
}

void Settings::WriteLPr()
{
    //Debug.startMethodTracing("/data/system/packageprof", 8 * 1024 * 1024);

    // Keep the old settings around until we know the new ones have
    // been successfully written.
    Boolean result;
    if (mSettingsFilename->Exists(&result), result) {
        // Presence of backup settings file indicates that we failed
        // to persist settings earlier. So preserve the older
        // backup for future reference since the current settings
        // might have been corrupted.
        if (mBackupSettingsFilename->Exists(&result), !result) {
            if (mSettingsFilename->RenameTo(mBackupSettingsFilename, &result), !result) {
                // Log.wtf(TAG, "Unable to backup package manager settings, "
                //         + " current changes will be lost at reboot");
                Slogger::E(TAG, "Unable to backup package manager settings, "
                    " current changes will be lost at reboot");
                return;
            }
        }
        else {
            mSettingsFilename->Delete(&result);
            Slogger::W(TAG, "Preserving older settings backup");
        }
    }

    mPastSignatures.Clear();

    //try
    ECode ec = NOERROR;
    String temp, nullStr;
    Int32 val;
    do {
        AutoPtr<IFileOutputStream> fstr;
        ec = CFileOutputStream::New(mSettingsFilename, (IFileOutputStream**)&fstr);
        if (FAILED(ec)) break;
        AutoPtr<IBufferedOutputStream> str;
        CBufferedOutputStream::New(IOutputStream::Probe(fstr), (IBufferedOutputStream**)&str);

        AutoPtr<IXmlSerializer> serializer;
        CFastXmlSerializer::New((IXmlSerializer**)&serializer);
        ec = serializer->SetOutput(IOutputStream::Probe(str), String("utf-8"));
        if (FAILED(ec)) break;

        ec = serializer->StartDocument(nullStr, TRUE);
        if (FAILED(ec)) break;
        ec = serializer->SetFeature(String("http://xmlpull.org/v1/doc/features.html#indent-output"), TRUE);
        if (FAILED(ec)) break;

        ec = serializer->WriteStartTag(nullStr, String("packages"));
        if (FAILED(ec)) break;

        {
            ec = serializer->WriteStartTag(nullStr, String("last-platform-version"));
            if (FAILED(ec)) break;
            ec = serializer->WriteAttribute(nullStr, String("internal"), StringUtils::ToString(mInternalSdkPlatform));
            if (FAILED(ec)) break;
            ec = serializer->WriteAttribute(nullStr, String("external"), StringUtils::ToString(mExternalSdkPlatform));
            if (FAILED(ec)) break;
            Logger::D(TAG, " fingerprint: [%s]", mFingerprint.string());
            ec = serializer->WriteAttribute(nullStr, String("fingerprint"), mFingerprint);
            if (FAILED(ec)) break;
            ec = serializer->WriteEndTag(nullStr, String("last-platform-version"));
            if (FAILED(ec)) break;
        }

        {
            ec = serializer->WriteStartTag(nullStr, String("database-version"));
            if (FAILED(ec)) break;
            ec = serializer->WriteAttribute(nullStr, String("internal"), StringUtils::ToString(mInternalDatabaseVersion));
            if (FAILED(ec)) break;
            ec = serializer->WriteAttribute(nullStr, String("external"), StringUtils::ToString(mExternalDatabaseVersion));
            if (FAILED(ec)) break;
            ec = serializer->WriteEndTag(nullStr, String("database-version"));
            if (FAILED(ec)) break;
        }

        if (mVerifierDeviceIdentity != NULL) {
            ec = serializer->WriteStartTag(nullStr, String("verifier"));
            if (FAILED(ec)) break;

            ec = serializer->WriteAttribute(nullStr, String("device"), Object::ToString(mVerifierDeviceIdentity));
            if (FAILED(ec)) break;
            ec = serializer->WriteEndTag(nullStr, String("verifier"));
            if (FAILED(ec)) break;
        }

        if (mReadExternalStorageEnforced != NULL) {
            Boolean value;
            mReadExternalStorageEnforced->GetValue(&value);
            ec = serializer->WriteStartTag(nullStr, TAG_READ_EXTERNAL_STORAGE);
            if (FAILED(ec)) break;
            ec = serializer->WriteAttribute(nullStr, ATTR_ENFORCEMENT, value ? String("1") : String("0"));
            if (FAILED(ec)) break;
            ec = serializer->WriteEndTag(nullStr, TAG_READ_EXTERNAL_STORAGE);
            if (FAILED(ec)) break;
        }

        {
            ec = serializer->WriteStartTag(nullStr, String("permission-trees"));
            if (FAILED(ec)) break;
            HashMap<String, AutoPtr<BasePermission> >::Iterator it = mPermissionTrees.Begin();
            for (; it != mPermissionTrees.End(); it++) {
                AutoPtr<BasePermission> bp = it->mSecond;
                ec = WritePermissionLPr(serializer, bp);
                if (FAILED(ec)) break;
            }

            ec = serializer->WriteEndTag(nullStr, String("permission-trees"));
            if (FAILED(ec)) break;
        }

        {
            ec = serializer->WriteStartTag(nullStr, String("permissions"));
            if (FAILED(ec)) break;

            HashMap<String, AutoPtr<BasePermission> >::Iterator it2 = mPermissions.Begin();
            for (; it2 != mPermissions.End(); it2++) {
                AutoPtr<BasePermission> bp = it2->mSecond;
                WritePermissionLPr(serializer, bp);
            }

            ec = serializer->WriteEndTag(nullStr, String("permissions"));
            if (FAILED(ec)) break;
        }

        HashMap<String, AutoPtr<PackageSetting> >::Iterator it3 = mPackages.Begin();
        for (; it3 != mPackages.End(); it3++) {
            AutoPtr<PackageSetting> pkg = it3->mSecond;
            WritePackageLPr(serializer, pkg);
        }

        HashMap<String, AutoPtr<PackageSetting> >::Iterator it4 = mDisabledSysPackages.Begin();
        for (; it4 != mDisabledSysPackages.End(); it4++) {
            AutoPtr<PackageSetting> pkg = it4->mSecond;
            WriteDisabledSysPackageLPr(serializer, pkg);
        }

        HashMap<String, AutoPtr<SharedUserSetting> >::Iterator it5 = mSharedUsers.Begin();
        for (; it5 != mSharedUsers.End(); it5++) {
            AutoPtr<SharedUserSetting> user = it5->mSecond;
            ec = serializer->WriteStartTag(nullStr, String("shared-user"));
            if (FAILED(ec)) break;
            ec = serializer->WriteAttribute(nullStr, ATTR_NAME, user->mName);
            if (FAILED(ec)) break;
            ec = serializer->WriteAttribute(nullStr, String("userId"),
                    StringUtils::ToString(user->mUserId));
            if (FAILED(ec)) break;
            user->mSignatures->WriteXml(serializer, String("sigs"), mPastSignatures);
            ec = serializer->WriteStartTag(nullStr, String("perms"));
            if (FAILED(ec)) break;

            HashSet<String>::Iterator it6 = user->mGrantedPermissions.Begin();
            for (; it6 != user->mGrantedPermissions.End(); it6++) {
                String name = (*it6);
                ec = serializer->WriteStartTag(nullStr, TAG_ITEM);
                if (FAILED(ec)) break;
                ec = serializer->WriteAttribute(nullStr, ATTR_NAME, name);
                if (FAILED(ec)) break;
                ec = serializer->WriteEndTag(nullStr, TAG_ITEM);
                if (FAILED(ec)) break;
            }
            ec = serializer->WriteEndTag(nullStr, String("perms"));
            if (FAILED(ec)) break;
            ec = serializer->WriteEndTag(nullStr, String("shared-user"));
            if (FAILED(ec)) break;
        }

        if (!mPackagesToBeCleaned.IsEmpty()) {
            List< AutoPtr<IPackageCleanItem> >::Iterator it7 = mPackagesToBeCleaned.Begin();
            for (; it7 != mPackagesToBeCleaned.End(); it7++) {
                AutoPtr<IPackageCleanItem> item = it7->Get();
                String userStr = StringUtils::ToString(item->GetUserId(&val), val);
                ec = serializer->WriteStartTag(nullStr, String("cleaning-package"));
                if (FAILED(ec)) break;
                ec = serializer->WriteAttribute(nullStr, ATTR_NAME, (item->GetPackageName(&temp), temp));
                if (FAILED(ec)) break;
                item->GetAndCode(&result);
                ec = serializer->WriteAttribute(nullStr, ATTR_CODE, result ? String("true") : String("FALSE"));
                if (FAILED(ec)) break;
                ec = serializer->WriteAttribute(nullStr, ATTR_USER, userStr);
                if (FAILED(ec)) break;
                ec = serializer->WriteEndTag(nullStr, String("cleaning-package"));
                if (FAILED(ec)) break;
            }
        }

        if (!mRenamedPackages.IsEmpty()) {
            HashMap<String, String>::Iterator it8 = mRenamedPackages.Begin();
            for (; it8 != mRenamedPackages.End(); it8++) {
                ec = serializer->WriteStartTag(nullStr, String("renamed-package"));
                ec = serializer->WriteAttribute(nullStr, String("new"), it8->mFirst);
                ec = serializer->WriteAttribute(nullStr, String("old"), it8->mSecond);
                ec = serializer->WriteEndTag(nullStr, String("renamed-package"));
            }
        }

        mKeySetManagerService->WriteKeySetManagerServiceLPr(serializer);

        ec = serializer->WriteEndTag(nullStr, String("packages"));
        if (FAILED(ec)) break;

        ec = serializer->EndDocument();
        if (FAILED(ec)) break;

        IFlushable::Probe(str)->Flush();
        FileUtils::Sync(fstr);
        ICloseable::Probe(str)->Close();

        // New settings successfully written, old ones are no longer
        // needed.
        mBackupSettingsFilename->Delete(&result);
        mSettingsFilename->ToString(&temp);
        FileUtils::SetPermissions(temp,
            FileUtils::sS_IRUSR | FileUtils::sS_IWUSR | FileUtils::sS_IRGRP | FileUtils::sS_IWGRP,
            -1, -1);

        // Write package list file now, use a JournaledFile.
        AutoPtr<IFile> tempFile;
        String absolutePath;
        mPackageListFilename->GetAbsolutePath(&absolutePath);
        CFile::New(absolutePath + ".tmp", (IFile**)&tempFile);
        AutoPtr<IJournaledFile> journal;
        CJournaledFile::New(mPackageListFilename, tempFile, (IJournaledFile**)&journal);

        AutoPtr<IFile> writeTarget;
        journal->ChooseForWrite((IFile**)&writeTarget);
        fstr = NULL;
        CFileOutputStream::New(writeTarget, (IFileOutputStream**)&fstr);
        str = NULL;
        CBufferedOutputStream::New(IOutputStream::Probe(fstr), (IBufferedOutputStream**)&str);
        //try
        AutoPtr<IFileDescriptor> fd;
        fstr->GetFD((IFileDescriptor**)&fd);
        FileUtils::SetPermissions(fd, 0660, IProcess::SYSTEM_UID, IProcess::PACKAGE_INFO_GID);

        StringBuilder sb;
        HashMap<String, AutoPtr<PackageSetting> >::Iterator it9 = mPackages.Begin();
        for (; it9 != mPackages.End(); it9++) {
            AutoPtr<PackageSetting> pkg = it9->mSecond;
            if (pkg->mPkg == NULL || pkg->mPkg->mApplicationInfo == NULL) {
                Slogger::W(TAG, "Skipping %p due to missing metadata", pkg.Get());
                continue;
            }

            AutoPtr<IApplicationInfo> ai = pkg->mPkg->mApplicationInfo;
            String dataPath;
            ai->GetDataDir(&dataPath);
            Int32 flags;
            Boolean isDebug = (ai->GetFlags(&flags), (flags & IApplicationInfo::FLAG_DEBUGGABLE) != 0);
            AutoPtr< ArrayOf<Int32> > gids = pkg->GetGids();

            // Avoid any application that has a space in its path.
            if (dataPath.IndexOf(" ") >= 0 || val < IProcess::FIRST_APPLICATION_UID)
                continue;

            // we store on each line the following information for now:
            //
            // pkgName    - package name
            // userId     - application-specific user id
            // debugFlag  - 0 or 1 if the package is debuggable.
            // dataPath   - path to package's data path
            // seinfo     - seinfo label for the app (assigned at install time)
            // gids       - supplementary gids this app launches with
            //
            // NOTE: We prefer not to expose all ApplicationInfo flags for now.
            //
            // DO NOT MODIFY THIS FORMAT UNLESS YOU CAN ALSO MODIFY ITS USERS
            // FROM NATIVE CODE. AT THE MOMENT, LOOK AT THE FOLLOWING SOURCES:
            //   system/core/run-as/run-as.c
            //   system/core/sdcard/sdcard.c
            //   external/libselinux/src/android.c:package_info_init()

            sb.SetLength(0);

            sb.Append((IPackageItemInfo::Probe(ai)->GetPackageName(&temp), temp));
            sb.Append(" ");
            ai->GetUid(&val);
            sb.Append(val);
            sb.Append(isDebug ? " 1 " : " 0 ");
            dataPath.IsNull() ? sb.Append("NULL") : sb.Append(dataPath);
            sb.Append(" ");
            sb.Append((ai->GetSeinfo(&temp), temp));
            sb.Append(" ");
            if (gids != NULL && gids->GetLength() > 0) {
                sb.Append((*gids)[0]);
                for (Int32 i = 1; i < gids->GetLength(); i++) {
                    sb.Append(",");
                    sb.Append((*gids)[i]);
                }
            }
            else {
                sb.Append("none");
            }
            sb.Append("\n");
            AutoPtr<ArrayOf<Byte> > bytes = sb.ToString().GetBytes();
            IOutputStream::Probe(str)->Write(bytes);
        }

        IFlushable::Probe(str)->Flush();
        FileUtils::Sync(fstr);
        ICloseable::Probe(str)->Close();
        journal->Commit();
        // } catch (Exception e) {
        //     IoUtils.closeQuietly(str);
        //     journal.rollback();
        // }

        mPackageListFilename->ToString(&temp);
        FileUtils::SetPermissions(temp,
                FileUtils::sS_IRUSR|FileUtils::sS_IWUSR
                |FileUtils::sS_IRGRP|FileUtils::sS_IWGRP,
                -1, -1);

        WriteAllUsersPackageRestrictionsLPr();
        return;
    } while (0);
    // } catch(XmlPullParserException e) {
    //     Log.wtf(TAG, "Unable to write package manager settings, "
    //             + "current changes will be lost at reboot", e);
    // } catch(java.io.IOException e) {
    //     Log.wtf(TAG, "Unable to write package manager settings, "
    //             + "current changes will be lost at reboot", e);
    // }
    //Debug.stopMethodTracing();

    if ((ECode)E_XML_PULL_PARSER_EXCEPTION == ec) {
        Slogger::E(TAG, "Unable to write package manager settings, current changes will be lost at reboot exception E_XML_PULL_PARSER_EXCEPTION");
    }
    else if ((ECode)E_IO_EXCEPTION == ec) {
        Slogger::E(TAG, "Unable to write package manager settings, current changes will be lost at reboot E_IO_EXCEPTION");
    }

    // Clean up partially written files
    if (mSettingsFilename->Exists(&result), result) {
        if (mSettingsFilename->Delete(&result), !result) {
            Slogger::E(TAG, "Failed to clean up mangled file: %s"
                    , (mSettingsFilename->ToString(&temp), temp.string()));
        }
    }
}

ECode Settings::WriteDisabledSysPackageLPr(
    /* [in] */ IXmlSerializer* serializer,
    /* [in] */ PackageSetting* pkg)
{

    serializer->WriteStartTag(String(NULL), String("updated-package"));
    serializer->WriteAttribute(String(NULL), ATTR_NAME, pkg->mName);
    if (pkg->mRealName != NULL) {
        serializer->WriteAttribute(String(NULL), String("realName"), pkg->mRealName);
    }
    serializer->WriteAttribute(String(NULL), String("codePath"), pkg->mCodePathString);
    serializer->WriteAttribute(String(NULL), String("ft"), StringUtils::ToHexString(pkg->mTimeStamp));
    serializer->WriteAttribute(String(NULL), String("it"), StringUtils::ToHexString(pkg->mFirstInstallTime));
    serializer->WriteAttribute(String(NULL), String("ut"), StringUtils::ToHexString(pkg->mLastUpdateTime));
    serializer->WriteAttribute(String(NULL), String("version"), StringUtils::ToString(pkg->mVersionCode));
    if (!pkg->mResourcePathString.Equals(pkg->mCodePathString)) {
        serializer->WriteAttribute(String(NULL), String("resourcePath"), pkg->mResourcePathString);
    }
    if (!pkg->mLegacyNativeLibraryPathString.IsNull()) {
        serializer->WriteAttribute(String(NULL), String("nativeLibraryPath"), pkg->mLegacyNativeLibraryPathString);
    }
    if (!pkg->mPrimaryCpuAbiString.IsNull()) {
       serializer->WriteAttribute(String(NULL), String("primaryCpuAbi"), pkg->mPrimaryCpuAbiString);
    }
    if (!pkg->mSecondaryCpuAbiString.IsNull()) {
        serializer->WriteAttribute(String(NULL), String("secondaryCpuAbi"), pkg->mSecondaryCpuAbiString);
    }
    if (!pkg->mCpuAbiOverrideString.IsNull()) {
        serializer->WriteAttribute(String(NULL), String("cpuAbiOverride"), pkg->mCpuAbiOverrideString);
    }

    if (pkg->mSharedUser == NULL) {
        serializer->WriteAttribute(String(NULL), String("userId"), StringUtils::ToString(pkg->mAppId));
    }
    else {
        serializer->WriteAttribute(String(NULL), String("sharedUserId"), StringUtils::ToString(pkg->mAppId));
    }
    serializer->WriteStartTag(String(NULL), String("perms"));
    if (pkg->mSharedUser == NULL) {
        // If this is a shared user, the permissions will
        // be written there. We still need to write an
        // empty permissions list so permissionsFixed will
        // be set.
        HashSet<String>::Iterator it = pkg->mGrantedPermissions.Begin();
        for (; it != pkg->mGrantedPermissions.End(); it++) {
            String name = (*it);
            HashMap<String, AutoPtr<BasePermission> >::Iterator it2 = mPermissions.Find(name);
            AutoPtr<BasePermission> bp = it2->mSecond;
            if (bp != NULL) {
                // We only need to write signature or system permissions but
                // this wont
                // match the semantics of grantedPermissions. So write all
                // permissions.
                serializer->WriteStartTag(String(NULL), TAG_ITEM);
                serializer->WriteAttribute(String(NULL), ATTR_NAME, name);
                serializer->WriteEndTag(String(NULL), TAG_ITEM);
            }
        }
    }
    serializer->WriteEndTag(String(NULL), String("perms"));
    serializer->WriteEndTag(String(NULL), String("updated-package"));
    return NOERROR;
}

ECode Settings::WritePackageLPr(
    /* [in] */ IXmlSerializer* serializer,
    /* [in] */ PackageSetting* pkg)
{
    serializer->WriteStartTag(String(NULL), String("package"));
    serializer->WriteAttribute(String(NULL), ATTR_NAME, pkg->mName);
    if (!pkg->mRealName.IsNull()) {
        serializer->WriteAttribute(String(NULL), String("realName"), pkg->mRealName);
    }
    serializer->WriteAttribute(String(NULL), String("codePath"), pkg->mCodePathString);
    if (!pkg->mResourcePathString.Equals(pkg->mCodePathString)) {
        serializer->WriteAttribute(String(NULL), String("resourcePath"), pkg->mResourcePathString);
    }
    if (!pkg->mLegacyNativeLibraryPathString.IsNull()) {
        serializer->WriteAttribute(String(NULL), String("nativeLibraryPath"), pkg->mLegacyNativeLibraryPathString);
    }
    if (!pkg->mPrimaryCpuAbiString.IsNull()) {
        serializer->WriteAttribute(String(NULL), String("primaryCpuAbi"), pkg->mPrimaryCpuAbiString);
    }
    if (!pkg->mSecondaryCpuAbiString.IsNull()) {
        serializer->WriteAttribute(String(NULL), String("secondaryCpuAbi"), pkg->mSecondaryCpuAbiString);
    }
    if (!pkg->mCpuAbiOverrideString.IsNull()) {
        serializer->WriteAttribute(String(NULL), String("cpuAbiOverride"), pkg->mCpuAbiOverrideString);
    }

    serializer->WriteAttribute(String(NULL), String("flags"), StringUtils::ToString(pkg->mPkgFlags));
    serializer->WriteAttribute(String(NULL), String("ft"), StringUtils::ToHexString(pkg->mTimeStamp));
    serializer->WriteAttribute(String(NULL), String("it"), StringUtils::ToHexString(pkg->mFirstInstallTime));
    serializer->WriteAttribute(String(NULL), String("ut"), StringUtils::ToHexString(pkg->mLastUpdateTime));
    serializer->WriteAttribute(String(NULL), String("version"), StringUtils::ToString(pkg->mVersionCode));
    if (pkg->mSharedUser == NULL) {
        serializer->WriteAttribute(String(NULL), String("userId"), StringUtils::ToString(pkg->mAppId));
    }
    else {
        serializer->WriteAttribute(String(NULL), String("sharedUserId"), StringUtils::ToString(pkg->mAppId));
    }
    if (pkg->mUidError) {
        serializer->WriteAttribute(String(NULL), String("uidError"), String("true"));
    }
    if (pkg->mInstallStatus == PackageSettingBase::PKG_INSTALL_INCOMPLETE) {
        serializer->WriteAttribute(String(NULL), String("installStatus"), String("false"));
    }
    if (!pkg->mInstallerPackageName.IsNull()) {
        serializer->WriteAttribute(String(NULL), String("installer"), pkg->mInstallerPackageName);
    }
    pkg->mSignatures->WriteXml(serializer, String("sigs"), mPastSignatures);
    if ((pkg->mPkgFlags & IApplicationInfo::FLAG_SYSTEM) == 0) {
        serializer->WriteStartTag(String(NULL), String("perms"));
        if (pkg->mSharedUser == NULL) {
            // If this is a shared user, the permissions will
            // be written there. We still need to write an
            // empty permissions list so permissionsFixed will
            // be set.
            HashSet<String>::Iterator it = pkg->mGrantedPermissions.Begin();
            for (; it != pkg->mGrantedPermissions.End(); it++) {
                String name = *it;
                serializer->WriteStartTag(String(NULL), TAG_ITEM);
                serializer->WriteAttribute(String(NULL), ATTR_NAME, name);
                serializer->WriteEndTag(String(NULL), TAG_ITEM);
            }
        }
        serializer->WriteEndTag(String(NULL), String("perms"));
    }

    WriteSigningKeySetsLPr(serializer, pkg->mKeySetData);
    WriteUpgradeKeySetsLPr(serializer, pkg->mKeySetData);
    WriteKeySetAliasesLPr(serializer, pkg->mKeySetData);

    serializer->WriteEndTag(String(NULL), String("package"));
    return NOERROR;
}

ECode Settings::WriteSigningKeySetsLPr(
    /* [in] */ IXmlSerializer* serializer,
    /* [in] */ PackageKeySetData* data)
{
    if (data->GetSigningKeySets() != NULL) {
        // Keep track of the original signing-keyset.
        // Must be recorded first, since it will be read first and wipe the
        // current signing-keysets for the package when set.
        Int64 properSigningKeySet = data->GetProperSigningKeySet();
        serializer->WriteStartTag(String(NULL), String("proper-signing-keyset"));
        serializer->WriteAttribute(String(NULL), String("identifier"), StringUtils::ToString(properSigningKeySet));
        serializer->WriteEndTag(String(NULL), String("proper-signing-keyset"));
        AutoPtr<ArrayOf<Int64> > sets = data->GetSigningKeySets();
        for (Int32 i = 0; i < sets->GetLength(); ++i) {
            serializer->WriteStartTag(String(NULL), String("signing-keyset"));
            serializer->WriteAttribute(String(NULL), String("identifier"), StringUtils::ToString((*sets)[i]));
            serializer->WriteEndTag(String(NULL), String("signing-keyset"));
        }
    }
    return NOERROR;
}

ECode Settings::WriteUpgradeKeySetsLPr(
    /* [in] */ IXmlSerializer* serializer,
    /* [in] */ PackageKeySetData* data)
{
    if (data->IsUsingUpgradeKeySets()) {
        AutoPtr<ArrayOf<Int64> > sets = data->GetUpgradeKeySets();
        for (Int32 i = 0; i < sets->GetLength(); ++i) {
            serializer->WriteStartTag(String(NULL), String("upgrade-keyset"));
            serializer->WriteAttribute(String(NULL), String("identifier"), StringUtils::ToString((*sets)[i]));
            serializer->WriteEndTag(String(NULL), String("upgrade-keyset"));
        }
    }
    return NOERROR;
}

ECode Settings::WriteKeySetAliasesLPr(
    /* [in] */ IXmlSerializer* serializer,
    /* [in] */ PackageKeySetData* data)
{
    HashMap<String, Int64>& aliases =  data->GetAliases();
    HashMap<String, Int64>::Iterator it = aliases.Begin();
    for (; it != aliases.End(); ++it) {
        serializer->WriteStartTag(String(NULL), String("defined-keyset"));
        serializer->WriteAttribute(String(NULL), String("alias"), it->mFirst);
        serializer->WriteAttribute(String(NULL), String("identifier"), StringUtils::ToString(it->mSecond));
        serializer->WriteEndTag(String(NULL), String("defined-keyset"));
    }
    return NOERROR;
}

ECode Settings::WritePermissionLPr(
    /* [in] */ IXmlSerializer* serializer,
    /* [in] */ BasePermission* bp)
{
    String nullStr;
    if (bp->mType != BasePermission::TYPE_BUILTIN && !bp->mSourcePackage.IsNull()) {
        FAIL_RETURN(serializer->WriteStartTag(nullStr, TAG_ITEM));
        FAIL_RETURN(serializer->WriteAttribute(nullStr, ATTR_NAME, bp->mName));
        FAIL_RETURN(serializer->WriteAttribute(nullStr, String("package"), bp->mSourcePackage));
        if (bp->mProtectionLevel != IPermissionInfo::PROTECTION_NORMAL) {
            FAIL_RETURN(serializer->WriteAttribute(nullStr, String("protection"), StringUtils::ToString(bp->mProtectionLevel)));
        }

        if (bp->mType == BasePermission::TYPE_DYNAMIC) {
            AutoPtr<IPermissionInfo> pi = bp->mPerm != NULL ? bp->mPerm->mInfo : bp->mPendingInfo;
            Int32 val;

            if (pi != NULL) {
                FAIL_RETURN(serializer->WriteAttribute(nullStr, String("type"), String("dynamic")));
                AutoPtr<IPackageItemInfo> pii = IPackageItemInfo::Probe(pi);
                pii->GetIcon(&val);
                if (val != 0) {
                    FAIL_RETURN(serializer->WriteAttribute(nullStr, String("icon"), StringUtils::ToString(val)));
                }
                AutoPtr<ICharSequence> label;
                pii->GetNonLocalizedLabel((ICharSequence**)&label);
                if (label != NULL) {
                    FAIL_RETURN(serializer->WriteAttribute( nullStr, String("label"), Object::ToString(label)));
                }
            }
        }
        FAIL_RETURN(serializer->WriteEndTag(nullStr, TAG_ITEM));
    }
    return NOERROR;
}

AutoPtr<List< AutoPtr<PackageSetting> > > Settings::GetListOfIncompleteInstallPackagesLPr()
{
    AutoPtr<List< AutoPtr<PackageSetting> > > ret = new List< AutoPtr<PackageSetting> >();
    HashMap<String, AutoPtr<PackageSetting> >::Iterator it;
    for (it = mPackages.Begin(); it != mPackages.End(); ++it) {
        AutoPtr<PackageSetting> ps = it->mSecond;
        if (ps->GetInstallStatus() == PackageSettingBase::PKG_INSTALL_INCOMPLETE) {
            ret->PushBack(ps);
        }
    }
    return ret;
}

void Settings::AddPackageToCleanLPw(
    /* [in] */ IPackageCleanItem* pkg)
{
    if (Find(mPackagesToBeCleaned.Begin(), mPackagesToBeCleaned.End(),
            AutoPtr<IPackageCleanItem>(pkg)) == mPackagesToBeCleaned.End()) {
        mPackagesToBeCleaned.PushBack(pkg);
    }
}

Boolean Settings::ReadLPw(
    /* [in] */ CPackageManagerService* service,
    /* [in] */ IList* users,
    /* [in] */ Int32 sdkVersion,
    /* [in] */ Boolean onlyCore)
{
    AutoPtr<IFileInputStream> str;
    Boolean result;
    String temp;
    if (mBackupSettingsFilename->Exists(&result), result) {
        //try {
        CFileInputStream::New(mBackupSettingsFilename, (IFileInputStream**)&str);
        mReadMessages->Append(String("Reading from backup settings file\n"));
        CPackageManagerService::ReportSettingsProblem(ILogHelper::INFO,
                String("Need to read from backup settings file"));
        if (mSettingsFilename->Exists(&result), result) {
            // If both the backup and settings file exist, we
            // ignore the settings since it might have been
            // corrupted.
            mSettingsFilename->ToString(&temp);
            Slogger::W(TAG, "Cleaning up settings file %s", temp.string());
            mSettingsFilename->Delete(&result);
        }
        //} catch (java.io.IOException e) {
        //    // We'll try for the normal settings file.
        //}
    }

    mPendingPackages.Clear();
    mPastSignatures.Clear();

    //try {
    ECode ec = NOERROR;
    do {
        if (str == NULL) {
            if (mSettingsFilename->Exists(&result), !result) {
                mReadMessages->Append(String("No settings file found\n"));
                CPackageManagerService::ReportSettingsProblem(ILogHelper::INFO,
                        String("No settings file; creating initial state"));
                mInternalSdkPlatform = mExternalSdkPlatform = sdkVersion;
                mFingerprint = Build::FINGERPRINT;
                return FALSE;
            }
            ec = CFileInputStream::New(mSettingsFilename, (IFileInputStream**)&str);
            if (FAILED(ec)) break;
        }

        AutoPtr<IXmlPullParser> parser;
        Xml::NewPullParser((IXmlPullParser**)&parser);
        parser->SetInput(IInputStream::Probe(str), String(NULL));
        Int32 type;
        while ((parser->Next(&type), type) != IXmlPullParser::START_TAG
                && type != IXmlPullParser::END_DOCUMENT) {
            ;
        }

        if (type != IXmlPullParser::START_TAG) {
            mReadMessages->Append(String("No start tag found in settings file\n"));
            CPackageManagerService::ReportSettingsProblem(ILogHelper::WARN,
                   String("No start tag found in package manager settings"));
            // Logger::Wtf(CPackageManagerService.TAG,
            //         "No start tag found in package manager settings");
            return FALSE;
        }

        Int32 outerDepth, val;
        parser->GetDepth(&outerDepth);
        while ((parser->Next(&type), type != IXmlPullParser::END_DOCUMENT)
                && (type != IXmlPullParser::END_TAG || (parser->GetDepth(&val), val > outerDepth))) {
            if (type == IXmlPullParser::END_TAG || type == IXmlPullParser::TEXT) {
                continue;
            }

            if (type != IXmlPullParser::START_TAG) {
                continue;
            }

            String tagName;
            parser->GetName(&tagName);
            if (tagName.Equals("package")) {
                ReadPackageLPw(parser);
            }
            else if (tagName.Equals("permissions")) {
                ReadPermissionsLPw(mPermissions, parser);
            }
            else if (tagName.Equals("permission-trees")) {
                ReadPermissionsLPw(mPermissionTrees, parser);
            }
            else if (tagName.Equals("shared-user")) {
                ReadSharedUserLPw(parser);
            }
            else if (tagName.Equals("preferred-packages")) {
                // no longer used.
            }
            else if (tagName.Equals("preferred-activities")) {
                // Upgrading from old single-user implementation;
                // these are the preferred activities for user 0.
                ReadPreferredActivitiesLPw(parser, 0);
            }
            else if (tagName.Equals(TAG_PERSISTENT_PREFERRED_ACTIVITIES)) {
                // TODO: check whether this is okay! as it is very
                // similar to how preferred-activities are treated
                ReadPersistentPreferredActivitiesLPw(parser, 0);
            }
            else if (tagName.Equals(TAG_CROSS_PROFILE_INTENT_FILTERS)) {
                // TODO: check whether this is okay! as it is very
                // similar to how preferred-activities are treated
                ReadCrossProfileIntentFiltersLPw(parser, 0);
            }
            else if (tagName.Equals("updated-package")) {
                ReadDisabledSysPackageLPw(parser);
            }
            else if (tagName.Equals("cleaning-package")) {
                String name, userStr, codeStr;
                ec = parser->GetAttributeValue(String(NULL), ATTR_NAME, &name);
                if (FAILED(ec)) break;
                ec = parser->GetAttributeValue(String(NULL), ATTR_USER, &userStr);
                if (FAILED(ec)) break;
                ec = parser->GetAttributeValue(String(NULL), ATTR_CODE, &codeStr);
                if (FAILED(ec)) break;
                if (!name.IsNull()) {
                    Int32 userId = 0;
                    Boolean andCode = TRUE;
                    //try {
                    if (!userStr.IsNull()) {
                        userId = StringUtils::ParseInt32(userStr);
                    }
                    // } catch (NumberFormatException e) {
                    // }
                    if (!codeStr.IsNull()) {
                        andCode = StringUtils::ParseInt32(codeStr);
                    }
                    AutoPtr<IPackageCleanItem> pkg;
                    CPackageCleanItem::New(userId, name, andCode, (IPackageCleanItem**)&pkg);
                    AddPackageToCleanLPw(pkg);
                }

            }
            else if (tagName.Equals("renamed-package")) {
                String nname, oname;
                ec = parser->GetAttributeValue(String(NULL), String("new"), &nname);
                if (FAILED(ec)) break;
                ec = parser->GetAttributeValue(String(NULL), String("old"), &oname);
                if (FAILED(ec)) break;
                if (!nname.IsNull() && !oname.IsNull()) {
                    mRenamedPackages[nname] = oname;
                }
            }
            else if (tagName.Equals("last-platform-version")) {
                mInternalSdkPlatform = mExternalSdkPlatform = 0;
                //try {
                String internal;
                ec = parser->GetAttributeValue(String(NULL), String("internal"), &internal);
                if (FAILED(ec)) break;
                if (!internal.IsNull()) {
                    mInternalSdkPlatform = StringUtils::ParseInt32(internal);
                }
                String external;
                ec = parser->GetAttributeValue(String(NULL), String("external"), &external);
                if (FAILED(ec)) break;
                if (!external.IsNull()) {
                    mExternalSdkPlatform = StringUtils::ParseInt32(external);
                }
                // } catch (NumberFormatException e) {
                // }
                ec = parser->GetAttributeValue(String(NULL), String("fingerprint"), &mFingerprint);
                if (FAILED(ec)) break;
            }
            else if (tagName.Equals("database-version")) {
                mInternalDatabaseVersion = mExternalDatabaseVersion = 0;
                // try {
                String internalDbVersionString;
                ec = parser->GetAttributeValue(String(NULL), String("internal"), &internalDbVersionString);
                if (FAILED(ec)) break;
                if (!internalDbVersionString.IsNull()) {
                    mInternalDatabaseVersion = StringUtils::ParseInt32(internalDbVersionString);
                }
                String externalDbVersionString;
                ec = parser->GetAttributeValue(String(NULL), String("external"), &externalDbVersionString);
                if (FAILED(ec)) break;
                if (!externalDbVersionString.IsNull()) {
                    mExternalDatabaseVersion = StringUtils::ParseInt32(externalDbVersionString);
                }
                // } catch (NumberFormatException ignored) {
                // }
            }
            else if (tagName.Equals("verifier")) {
                String deviceIdentity;
                ec = parser->GetAttributeValue(String(NULL), String("device"), &deviceIdentity);
                if (FAILED(ec)) break;
                //try {
                AutoPtr<IVerifierDeviceIdentityHelper> vdiHelper;
                CVerifierDeviceIdentityHelper::AcquireSingleton((IVerifierDeviceIdentityHelper**)&vdiHelper);
                mVerifierDeviceIdentity = NULL;
                vdiHelper->Parse(deviceIdentity, (IVerifierDeviceIdentity**)&mVerifierDeviceIdentity);
                //} catch (IllegalArgumentException e) {
                    // Slogger::W(TAG, "Discard invalid verifier device id: "
                    //         + e.getMessage());
                //}
            }
            else if (TAG_READ_EXTERNAL_STORAGE.Equals(tagName)) {
                String enforcement;
                ec = parser->GetAttributeValue(String(NULL), ATTR_ENFORCEMENT, &enforcement);
                if (FAILED(ec)) break;
                mReadExternalStorageEnforced = NULL;
                CBoolean::New(enforcement.Equals("1"), (IBoolean**)&mReadExternalStorageEnforced);
            }
            else if (tagName.Equals("keyset-settings")) {
                mKeySetManagerService->ReadKeySetsLPw(parser);
            }
            else {
                // Slogger::W(TAG, String("Unknown element under <packages>: ")
                //         + (parser->GetName(&temp), temp.IsNull() ? String("NULL") : temp));
                XmlUtils::SkipCurrentTag(parser);
            }
        }
        ICloseable::Probe(str)->Close();
        // } catch (XmlPullParserException e) {
        // mReadMessages.append("Error reading: " + e.toString());
        // PackageManagerService.reportSettingsProblem(Log.ERROR, "Error reading settings: " + e);
        // Log.wtf(PackageManagerService.TAG, "Error reading package manager settings", e);

        // } catch (java.io.IOException e) {
        //     mReadMessages.append("Error reading: " + e.toString());
        //     PackageManagerService.reportSettingsProblem(Log.ERROR, "Error reading settings: " + e);
        //     Log.wtf(PackageManagerService.TAG, "Error reading package manager settings", e);
        // }
    } while (0);


    if ((ECode)E_XML_PULL_PARSER_EXCEPTION == ec) {
        mReadMessages->Append("Error reading: ");
        CPackageManagerService::ReportSettingsProblem(ILogHelper::ERROR, String("Error reading settings: "));
        Slogger::W(TAG, "Error reading accounts ecode = XmlPullParserException");
    }
    else if ((ECode)E_IO_EXCEPTION == ec) {
        mReadMessages->Append("Error reading: ");
        CPackageManagerService::ReportSettingsProblem(ILogHelper::ERROR, String("Error reading settings: "));
    }

    List<AutoPtr<PendingPackage> >::Iterator it = mPendingPackages.Begin();
    for (; it != mPendingPackages.End(); it++) {
        AutoPtr<PendingPackage> pp = (*it).Get();
        AutoPtr<IInterface> idObj = GetUserIdLPr(pp->mSharedId);
        if (idObj != NULL && idObj->Probe(EIID_SharedUserSetting)) {
            SharedUserSetting* sus = reinterpret_cast<SharedUserSetting*>(idObj->Probe(EIID_SharedUserSetting));
            AutoPtr<PackageSetting> p = GetPackageLPw(pp->mName, NULL, pp->mRealName,
                    sus, pp->mCodePath, pp->mResourcePath,
                    pp->mLegacyNativeLibraryPathString, pp->mPrimaryCpuAbiString,
                    pp->mSecondaryCpuAbiString, pp->mVersionCode, pp->mPkgFlags, NULL,
                    TRUE /* add */, FALSE /* allowInstall */);
            if (p == NULL) {
                CPackageManagerService::ReportSettingsProblem(ILogHelper::WARN,
                        String("Unable to create application package for ") + pp->mName);
                continue;
            }
            p->CopyFrom((PackageSettingBase*)pp);
        }
        else if (idObj != NULL) {
            String msg = String("Bad package setting: package ") + pp->mName + " has shared uid "
                    + StringUtils::ToString(pp->mSharedId) + " that is not a shared uid\n";
            mReadMessages->Append(msg);
            CPackageManagerService::ReportSettingsProblem(ILogHelper::ERROR, msg);
        }
        else {
            String msg = String("Bad package setting: package ") + pp->mName + " has shared uid "
                    + StringUtils::ToString(pp->mSharedId) + " that is not defined\n";
            mReadMessages->Append(msg);
            CPackageManagerService::ReportSettingsProblem(ILogHelper::ERROR, msg);
        }
    }
    mPendingPackages.Clear();

    if ((mBackupStoppedPackagesFilename->Exists(&result), result)
                || (mStoppedPackagesFilename->Exists(&result), result)) {
            // Read old file
            ReadStoppedLPw();
            mBackupStoppedPackagesFilename->Delete(&result);
            mStoppedPackagesFilename->Delete(&result);
            // Migrate to new file format
            WritePackageRestrictionsLPr(0);
        }
        else {
            Int32 size;
            if (users->GetSize(&size), size == 0) {
                ReadPackageRestrictionsLPr(0);
            }
            else {
                AutoPtr<IIterator> it;
                users->GetIterator((IIterator**)&it);
                Boolean hasNext;
                while (it->HasNext(&hasNext), hasNext) {
                    AutoPtr<IInterface> value;
                    it->GetNext((IInterface**)&value);
                    AutoPtr<IUserInfo> user = IUserInfo::Probe(value);
                    Int32 userId;
                    user->GetId(&userId);
                    ReadPackageRestrictionsLPr(userId);
                }
            }
        }

    /*
     * Make sure all the updated system packages have their shared users
     * associated with them.
     */
    HashMap<String, AutoPtr<PackageSetting> >::Iterator disabledIt = mDisabledSysPackages.Begin();
    //Iterator<PackageSetting> disabledIt = mDisabledSysPackages.values().iterator();
    for(; disabledIt != mDisabledSysPackages.End(); disabledIt++) {
        AutoPtr<PackageSetting> disabledPs = disabledIt->mSecond;
        AutoPtr<IInterface> id = GetUserIdLPr(disabledPs->mAppId);
        if (id != NULL && id->Probe(EIID_SharedUserSetting)) {
            disabledPs->mSharedUser = reinterpret_cast<SharedUserSetting*>(id->Probe(EIID_SharedUserSetting));
        }
    }

    mReadMessages->Append(String("Read completed successfully: ") + StringUtils::ToString((Int32)mPackages.GetSize()) +
            " packages, " + StringUtils::ToString((Int32)mSharedUsers.GetSize()) + " shared uids\n");
    return TRUE;
}

void Settings::ReadDefaultPreferredAppsLPw(
    /* [in] */ CPackageManagerService* service,
    /* [in] */ Int32 userId)
{
    // First pull data from any pre-installed apps.
    HashMap<String, AutoPtr<PackageSetting> >::Iterator it = mPackages.Begin();
    for (; it != mPackages.End(); ++it) {
        AutoPtr<PackageSetting> ps = it->mSecond;
        if ((ps->mPkgFlags & IApplicationInfo::FLAG_SYSTEM) != 0 && ps->mPkg != NULL
                && ps->mPkg->mPreferredActivityFilters != NULL) {
            AutoPtr<List< AutoPtr<PackageParser::ActivityIntentInfo> > > intents = ps->mPkg->mPreferredActivityFilters;
            List< AutoPtr<PackageParser::ActivityIntentInfo> >::Iterator it = intents->Begin();
            for (; it != intents->End(); ++it) {
                AutoPtr<PackageParser::ActivityIntentInfo> aii = *it;
                AutoPtr<IComponentName> cn;
                CComponentName::New(ps->mName, aii->mActivity->mClassName, (IComponentName**)&cn);
                ApplyDefaultPreferredActivityLPw(service, aii, cn, userId);
            }
        }
    }

    // Read preferred apps from .../etc/preferred-apps directory.
    AutoPtr<IFile> preferredDir;
    AutoPtr<IFile> dir = Environment::GetRootDirectory();
    CFile::New(dir, String("etc/preferred-apps"), (IFile**)&preferredDir);
    Boolean result;
    String temp;
    if ((preferredDir->Exists(&result), !result) || (preferredDir->IsDirectory(&result), !result)) {
        return;
    }
    if ((preferredDir->CanRead(&result), !result)) {
        Slogger::W(TAG, "Directory %s cannot be read", TO_CSTR(preferredDir));
        return;
    }

    AutoPtr<ArrayOf<IFile*> > preferredFiles;
    preferredDir->ListFiles((ArrayOf<IFile*>**)&preferredFiles);
    // Iterate over the files in the directory and scan .xml files
    for (Int32 i = 0; preferredFiles != NULL && i < preferredFiles->GetLength(); i++) {
        AutoPtr<IFile> f = (*preferredFiles)[i];
        f->GetPath(&temp);
        if (!temp.EndWith(".xml")) {
            Slogger::I(TAG, "Non-xml file %s in  directory, ignoring", TO_CSTR(f), TO_CSTR(preferredDir));
            continue;
        }
        if (f->CanRead(&result), result) {
            Slogger::W(TAG, "Preferred apps file %s cannot be read", TO_CSTR(f));
            continue;
        }

        if (CPackageManagerService::DEBUG_PREFERRED) Logger::D(TAG, "Reading default preferred %s", TO_CSTR(f));

        AutoPtr<IFileInputStream> str;
        //try
        CFileInputStream::New(f, (IFileInputStream**)&str);
        AutoPtr<IXmlPullParser> parser;
        Xml::NewPullParser((IXmlPullParser**)&parser);
        parser->SetInput(IInputStream::Probe(str), String(NULL));

        Int32 type;
        while ((parser->Next(&type), type) != IXmlPullParser::START_TAG
                && type != IXmlPullParser::END_DOCUMENT) {
            ;
        }

        if (type != IXmlPullParser::START_TAG) {
            Slogger::W(TAG, "Preferred apps file %s does not have start tag", TO_CSTR(f));
            continue;
        }
        parser->GetName(&temp);
        if (!temp.Equals("preferred-activities")) {
            Slogger::W(TAG, "Preferred apps file %s does not start with 'preferred-activities'", TO_CSTR(f));
            continue;
        }
        ReadDefaultPreferredActivitiesLPw(service, parser, userId);
        // } catch (XmlPullParserException e) {
        //     Slog.w(TAG, "Error reading apps file " + f, e);
        // } catch (IOException e) {
        //     Slog.w(TAG, "Error reading apps file " + f, e);
        // } finally {
        //     if (str != NULL) {
        //         try {
        //             str.close();
        //         } catch (IOException e) {
        //         }
        //     }
        // }
        if (str != NULL) {
            //try {
            ICloseable::Probe(str)->Close();
            // } catch (IOException e) {
            // }
        }
    }
}

void Settings::ApplyDefaultPreferredActivityLPw(
    /* [in] */ CPackageManagerService* service,
    /* [in] */ IIntentFilter* tmpPa,
    /* [in] */ IComponentName* cn,
    /* [in] */ Int32 userId)
{
    // The initial preferences only specify the target activity
    // component and intent-filter, not the set of matches.  So we
    // now need to query for the matches to build the correct
    // preferred activity entry.
    if (CPackageManagerService::DEBUG_PREFERRED) {
        Logger::D(TAG, "Processing preferred:");
        // tmpPa.dump(new LogPrinter(Log.DEBUG, TAG), "  ");
    }
    AutoPtr<IIntent> intent;
    CIntent::New((IIntent**)&intent);
    Int32 flags = 0;
    String action;
    tmpPa->GetAction(0, &action);
    intent->SetAction(action);
    Int32 count;
    tmpPa->CountCategories(&count);
    for (Int32 i = 0; i < count; i++) {
        String cat;
        tmpPa->GetCategory(i, &cat);
        if (cat.Equals(IIntent::CATEGORY_DEFAULT)) {
            flags |= IPackageManager::MATCH_DEFAULT_ONLY;
        }
        else {
            intent->AddCategory(cat);
        }
    }

    Boolean doNonData = TRUE;
    Boolean hasSchemes = FALSE;

    tmpPa->CountDataSchemes(&count);
    for (Int32 ischeme = 0; ischeme < count; ischeme++) {
        Boolean doScheme = TRUE;
        String scheme;
        tmpPa->GetDataScheme(ischeme, &scheme);
        if (!scheme.IsNull() && !scheme.IsEmpty()) {
            hasSchemes = TRUE;
        }
        Int32 isspCount;
        tmpPa->CountDataSchemeSpecificParts(&isspCount);
        for (Int32 issp = 0; issp < isspCount; issp++) {
            AutoPtr<IUriBuilder> builder;
            CUriBuilder::New((IUriBuilder**)&builder);
            builder->Scheme(scheme);
            AutoPtr<IPatternMatcher> ssp;
            tmpPa->GetDataSchemeSpecificPart(issp, (IPatternMatcher**)&ssp);
            String path;
            ssp->GetPath(&path);
            builder->OpaquePart(path);
            AutoPtr<IIntent> finalIntent;
            CIntent::New(intent, (IIntent**)&finalIntent);
            AutoPtr<IUri> uri;
            builder->Build((IUri**)&uri);
            finalIntent->SetData(uri);
            ApplyDefaultPreferredActivityLPw(service, finalIntent, flags, cn,
                    scheme, ssp, NULL, NULL, userId);
            doScheme = FALSE;
        }
        Int32 iauthCount;
        tmpPa->CountDataAuthorities(&iauthCount);
        for (Int32 iauth = 0; iauth < iauthCount; iauth++) {
            Boolean doAuth = TRUE;
            AutoPtr<IIntentFilterAuthorityEntry> auth;
            tmpPa->GetDataAuthority(iauth, (IIntentFilterAuthorityEntry**)&auth);
            Int32 ipathCount;
            tmpPa->CountDataPaths(&ipathCount);
            for (Int32 ipath = 0; ipath < ipathCount; ipath++) {
                AutoPtr<IUriBuilder> builder;
                CUriBuilder::New((IUriBuilder**)&builder);
                builder->Scheme(scheme);
                String host;
                if (auth->GetHost(&host), !host.IsNull()) {
                    builder->Authority(host);
                }
                AutoPtr<IPatternMatcher> path;
                tmpPa->GetDataPath(ipath, (IPatternMatcher**)&path);
                String p;
                path->GetPath(&p);
                builder->Path(p);
                AutoPtr<IIntent> finalIntent;
                CIntent::New(intent, (IIntent**)&finalIntent);
                AutoPtr<IUri> uri;
                builder->Build((IUri**)&uri);
                finalIntent->SetData(uri);
                ApplyDefaultPreferredActivityLPw(service, finalIntent, flags, cn,
                        scheme, NULL, auth, path, userId);
                doAuth = doScheme = FALSE;
            }
            if (doAuth) {
                AutoPtr<IUriBuilder> builder;
                CUriBuilder::New((IUriBuilder**)&builder);
                builder->Scheme(scheme);
                String host;
                if (auth->GetHost(&host), !host.IsNull()) {
                    builder->Authority(host);
                }
                AutoPtr<IIntent> finalIntent;
                CIntent::New(intent, (IIntent**)&finalIntent);
                AutoPtr<IUri> uri;
                builder->Build((IUri**)&uri);
                finalIntent->SetData(uri);
                ApplyDefaultPreferredActivityLPw(service, finalIntent, flags, cn,
                        scheme, NULL, auth, NULL, userId);
                doScheme = FALSE;
            }
        }
        if (doScheme) {
            AutoPtr<IUriBuilder> builder;
            CUriBuilder::New((IUriBuilder**)&builder);
            builder->Scheme(scheme);
            AutoPtr<IIntent> finalIntent;
            CIntent::New(intent, (IIntent**)&finalIntent);
            AutoPtr<IUri> uri;
            builder->Build((IUri**)&uri);
            finalIntent->SetData(uri);
            ApplyDefaultPreferredActivityLPw(service, finalIntent, flags, cn,
                    scheme, NULL, NULL, NULL, userId);
        }
        doNonData = FALSE;
    }

    tmpPa->CountDataTypes(&count);
    for (Int32 idata = 0; idata < count; idata++) {
        String mimeType;
        tmpPa->GetDataType(idata, &mimeType);
        if (hasSchemes) {
            AutoPtr<IUriBuilder> builder;
            CUriBuilder::New((IUriBuilder**)&builder);
            Int32 ischemeCount;
            tmpPa->CountDataSchemes(&ischemeCount);
            for (Int32 ischeme = 0; ischeme < ischemeCount; ischeme++) {
                String scheme;
                tmpPa->GetDataScheme(ischeme, &scheme);
                if (!scheme.IsNull() && !scheme.IsEmpty()) {
                    AutoPtr<IIntent> finalIntent;
                    CIntent::New(intent, (IIntent**)&finalIntent);
                    builder->Scheme(scheme);
                    AutoPtr<IUri> uri;
                    builder->Build((IUri**)&uri);
                    finalIntent->SetDataAndType(uri, mimeType);
                    ApplyDefaultPreferredActivityLPw(service, finalIntent, flags, cn,
                            scheme, NULL, NULL, NULL, userId);
                }
            }
        }
        else {
            AutoPtr<IIntent> finalIntent;
            CIntent::New(intent, (IIntent**)&finalIntent);
            finalIntent->SetType(mimeType);
            ApplyDefaultPreferredActivityLPw(service, finalIntent, flags, cn,
                    String(NULL), NULL, NULL, NULL, userId);
        }
        doNonData = false;
    }

    if (doNonData) {
        ApplyDefaultPreferredActivityLPw(service, intent, flags, cn,
                String(NULL), NULL, NULL, NULL, userId);
    }
}

void Settings::ApplyDefaultPreferredActivityLPw(
    /* [in] */ CPackageManagerService* service,
    /* [in] */ IIntent* intent,
    /* [in] */ Int32 flags,
    /* [in] */ IComponentName* cn,
    /* [in] */ const String& scheme,
    /* [in] */ IPatternMatcher* ssp,
    /* [in] */ IIntentFilterAuthorityEntry* auth,
    /* [in] */ IPatternMatcher* path,
    /* [in] */ Int32 userId)
{
    String type;
    intent->GetType(&type);
    AutoPtr<List<AutoPtr<IResolveInfo> > > ri = service->mActivities->QueryIntent(intent,
            type, flags, 0);
    if (CPackageManagerService::DEBUG_PREFERRED) Logger::D(TAG, "Queried %p result: %p", intent, ri.Get());
    Int32 systemMatch = 0;
    Int32 thirdPartyMatch = 0;
    Int32 size = ri->GetSize();
    if (ri != NULL && size > 1) {
        Boolean haveAct = FALSE;
        AutoPtr<IComponentName> haveNonSys;
        AutoPtr< ArrayOf<IComponentName*> > set = ArrayOf<IComponentName*>::Alloc(size);
        List<AutoPtr<IResolveInfo> >::Iterator it = ri->Begin();
        for (Int32 i = 0; it != ri->End(); ++it, ++i) {
            AutoPtr<IResolveInfo> resolveInfo = *it;
            AutoPtr<IActivityInfo> ai;
            resolveInfo->GetActivityInfo((IActivityInfo**)&ai);
            AutoPtr<IPackageItemInfo> pii = IPackageItemInfo::Probe(ai);
            String pkgName, aiName;
            pii->GetPackageName(&pkgName);
            pii->GetName(&aiName);
            AutoPtr<IComponentName> cn;
            CComponentName::New(pkgName, aiName, (IComponentName**)&cn);
            set->Set(i, cn);
            AutoPtr<IApplicationInfo> appInfo;
            IComponentInfo::Probe(ai)->GetApplicationInfo((IApplicationInfo**)&appInfo);
            Int32 flags;
            String cnPkgName, className;
            if (appInfo->GetFlags(&flags), (flags & IApplicationInfo::FLAG_SYSTEM) == 0) {
                Int32 match;
                if (resolveInfo->GetMatch(&match), match >= thirdPartyMatch) {
                    // Keep track of the best match we find of all third
                    // party apps, for use later to determine if we actually
                    // want to set a preferred app for this intent.

                    if (CPackageManagerService::DEBUG_PREFERRED) {
                        Logger::D(TAG, "Result %s/%s: non-system!", pkgName.string(), aiName.string());
                    }
                    haveNonSys = (*set)[i];
                    break;
                }
            }
            else if ((cn->GetPackageName(&cnPkgName), cnPkgName.Equals(pkgName))
                    && (cn->GetClassName(&className), className.Equals(aiName))) {
                if (CPackageManagerService::DEBUG_PREFERRED) {
                    Logger::D(TAG, "Result %s/%s: default!", pkgName.string(), aiName.string());
                }
                haveAct = TRUE;
                resolveInfo->GetMatch(&systemMatch);
            }
            else {
                if (CPackageManagerService::DEBUG_PREFERRED) {
                    Logger::D(TAG, "Result %s/%s: skipped!", pkgName.string(), aiName.string());
                }
            }
        }
        if (haveNonSys != NULL && thirdPartyMatch < systemMatch) {
            // If we have a matching third party app, but its match is not as
            // good as the built-in system app, then we don't want to actually
            // consider it a match because presumably the built-in app is still
            // the thing we want users to see by default.
            haveNonSys = NULL;
        }
        if (haveAct && haveNonSys == NULL) {
            AutoPtr<IIntentFilter> filter;
            CIntentFilter::New((IIntentFilter**)&filter);
            String action;
            if (intent->GetAction(&action), !action.IsNull()) {
                filter->AddAction(action);
            }
            AutoPtr<ArrayOf<String> > cats;
            if (intent->GetCategories((ArrayOf<String>**)&cats), cats != NULL) {
                for (Int32 i = 0; i < cats->GetLength(); ++i) {
                    filter->AddCategory((*cats)[i]);
                }
            }
            if ((flags & IPackageManager::MATCH_DEFAULT_ONLY) != 0) {
                filter->AddCategory(IIntent::CATEGORY_DEFAULT);
            }
            if (!scheme.IsNull()) {
                filter->AddDataScheme(scheme);
            }
            if (ssp != NULL) {
                String p;
                Int32 type;
                ssp->GetPath(&p);
                ssp->GetType(&type);
                filter->AddDataSchemeSpecificPart(p, type);
            }
            if (auth != NULL) {
                filter->AddDataAuthority(auth);
            }
            if (path != NULL) {
                filter->AddDataPath(path);
            }
            String intentT;
            if (intent->GetType(&intentT), !intentT.IsNull()) {
                // try {
                if (FAILED(filter->AddDataType(intentT))) {
                    Slogger::W(TAG, "Malformed mimetype %s for %p", intentT.string(), cn);
                }
                // } catch (IntentFilter.MalformedMimeTypeException ex) {
                //     Slog.w(TAG, "Malformed mimetype " + intent.getType() + " for " + cn);
                // }
            }
            AutoPtr<PreferredActivity> pa = new PreferredActivity(filter, systemMatch, set, cn, TRUE);
            EditPreferredActivitiesLPw(userId)->AddFilter(pa);
        }
        else if (haveNonSys == NULL) {
            StringBuilder sb;
            sb.Append("No component ");
            String str;
            cn->FlattenToShortString(&str);
            sb.Append(str);
            sb.Append(" found setting preferred ");
            IObject::Probe(intent)->ToString(&str);
            sb.Append(str);
            sb.Append("; possible matches are ");
            for (Int32 i = 0; i < set->GetLength(); i++) {
                if (i > 0) sb.Append(", ");
                (*set)[i]->FlattenToShortString(&str);
                sb.Append(str);
            }
            Slogger::W(TAG, sb.ToString());
        }
        else {
            String intentStr, str;
            IObject::Probe(intent)->ToString(&str);
            haveNonSys->FlattenToShortString(&str);
            Slogger::I(TAG, "Not setting preferred %s; found third party match %s", intentStr.string()
                    , str.string());
        }
    }
    else {
        String intentStr, str;
        IObject::Probe(intent)->ToString(&str);
        cn->FlattenToShortString(&str);
        Slogger::I(TAG, "No potential matches found for %s; while setting preferred %s", intentStr.string()
                , str.string());
    }
}

ECode Settings::ReadDefaultPreferredActivitiesLPw(
    /* [in] */ CPackageManagerService* service,
    /* [in] */ IXmlPullParser* parser,
    /* [in] */ Int32 userId)
{
    Int32 outerDepth;
    parser->GetDepth(&outerDepth);
    Int32 type, depth;
    while ((type = parser->Next(&type), type != IXmlPullParser::END_DOCUMENT)
            && (type != IXmlPullParser::END_TAG || (parser->GetDepth(&depth), depth > outerDepth))) {
        if (type == IXmlPullParser::END_TAG || type == IXmlPullParser::TEXT) {
            continue;
        }

        String tagName;
        parser->GetName(&tagName);
        if (tagName.Equals(TAG_ITEM)) {
            AutoPtr<PreferredActivity> tmpPa = new PreferredActivity(parser);
            if (tmpPa->mPref->GetParseError().IsNull()) {
                ApplyDefaultPreferredActivityLPw(service, tmpPa, tmpPa->mPref->mComponent, userId);
            }
            else {
                String des;
                parser->GetPositionDescription(&des);
                CPackageManagerService::ReportSettingsProblem(ILogHelper::WARN,
                        String("Error in package manager settings: <preferred-activity> ")
                                + tmpPa->mPref->GetParseError() + " at " + des);
            }
        }
        else {
            CPackageManagerService::ReportSettingsProblem(ILogHelper::WARN,
                    String("Unknown element under <preferred-activities>: ") + tagName);
            XmlUtils::SkipCurrentTag(parser);
        }
    }
    return NOERROR;
}

Int32 Settings::ReadInt32(
    /* [in] */ IXmlPullParser* parser,
    /* [in] */ const String& ns,
    /* [in] */ const String& name,
    /* [in] */ Int32 defValue)
{
    String v;
    parser->GetAttributeValue(ns, name, &v);
    // try {
    if (v.IsNull()) {
        return defValue;
    }
    return StringUtils::ParseInt32(v, 10, defValue);
    // } catch (NumberFormatException e) {
    //     PackageManagerService.reportSettingsProblem(Log.WARN,
    //             "Error in package manager settings: attribute " + name
    //                     + " has bad integer value " + v + " at "
    //                     + parser.getPositionDescription());
    // }
    // return defValue;
}

ECode Settings::ReadPermissionsLPw(
    /* [in] */ HashMap<String, AutoPtr<BasePermission> >& out,
    /* [in] */ IXmlPullParser* parser)
{
    Int32 outerDepth, outerDepth1;
    parser->GetDepth(&outerDepth);
    Int32 type;
    String temp;
    while ((parser->Next(&type), type) != IXmlPullParser::END_DOCUMENT
            && (type != IXmlPullParser::END_TAG || (parser->GetDepth(&outerDepth1), outerDepth1) > outerDepth)) {
        if (type == IXmlPullParser::END_TAG || type == IXmlPullParser::TEXT) {
            continue;
        }

        if (type != IXmlPullParser::START_TAG) {
            continue;
        }

        String tagName;
        parser->GetName(&tagName);
        if (tagName.Equals(TAG_ITEM)) {
            String name, sourcePackage, ptype;
            parser->GetAttributeValue(String(NULL), ATTR_NAME, &name);
            parser->GetAttributeValue(String(NULL), String("package"), &sourcePackage);
            parser->GetAttributeValue(String(NULL), String("type"), &ptype);
            if (!name.IsNull() && !sourcePackage.IsNull()) {
                Boolean dynamic = ptype.Equals("dynamic");
                AutoPtr<BasePermission> bp = new BasePermission(name, sourcePackage,
                        dynamic ? BasePermission::TYPE_DYNAMIC : BasePermission::TYPE_NORMAL);
                bp->mProtectionLevel = ReadInt32(parser, String(NULL), String("protection"),
                        IPermissionInfo::PROTECTION_NORMAL);
                AutoPtr<IPermissionInfoHelper> helper;
                CPermissionInfoHelper::AcquireSingleton((IPermissionInfoHelper**)&helper);
                helper->FixProtectionLevel(bp->mProtectionLevel, &bp->mProtectionLevel);

                if (dynamic) {
                    AutoPtr<IPermissionInfo> pi;
                    CPermissionInfo::New((IPermissionInfo**)&pi);
                    AutoPtr<IPackageItemInfo> pii = IPackageItemInfo::Probe(pi);
                    pii->SetPackageName(sourcePackage);
                    pii->SetName(name);
                    pii->SetIcon(ReadInt32(parser, String(NULL), String("icon"), 0));

                    parser->GetAttributeValue(String(NULL), String("label"), &temp);
                    AutoPtr<ICharSequence> csq;
                    CString::New(temp, (ICharSequence**)&csq);

                    pii->SetNonLocalizedLabel(csq);
                    pi->SetProtectionLevel(bp->mProtectionLevel);
                    bp->mPendingInfo = pi;
                }

                out[bp->mName] = bp;
            }
            else {
                StringBuilder sb("Error in package manager settings: permissions ");
                sb += tagName;
                sb += " has no name at ";
                parser->GetPositionDescription(&temp);
                sb += temp;

                CPackageManagerService::ReportSettingsProblem(ILogHelper::WARN, sb.ToString());
            }
        }
        else {
            StringBuilder sb("Unknown element reading permissions: [");
            sb += tagName;
            sb += "] at ";
            parser->GetPositionDescription(&temp);
            sb += temp;

            CPackageManagerService::ReportSettingsProblem(ILogHelper::WARN, sb.ToString());
            assert(0);
        }
        XmlUtils::SkipCurrentTag(parser);
    }
    return NOERROR;
}

ECode Settings::ReadDisabledSysPackageLPw(
    /* [in] */ IXmlPullParser* parser)
{
    String temp;
    String name;
    parser->GetAttributeValue(String(NULL), ATTR_NAME, &name);
    String realName;
    parser->GetAttributeValue(String(NULL), String("realName"), &realName);
    String codePathStr;
    parser->GetAttributeValue(String(NULL), String("codePath"), &codePathStr);
    String resourcePathStr;
    parser->GetAttributeValue(String(NULL), String("resourcePath"), &resourcePathStr);

    String legacyCpuAbiStr;
    parser->GetAttributeValue(String(NULL), String("requiredCpuAbi"), &legacyCpuAbiStr);
    String legacyNativeLibraryPathStr;
    parser->GetAttributeValue(String(NULL), String("nativeLibraryPath"), &legacyNativeLibraryPathStr);

    String primaryCpuAbiStr;
    parser->GetAttributeValue(String(NULL), String("primaryCpuAbi"), &primaryCpuAbiStr);
    String secondaryCpuAbiStr;
    parser->GetAttributeValue(String(NULL), String("secondaryCpuAbi"), &secondaryCpuAbiStr);
    String cpuAbiOverrideStr;
    parser->GetAttributeValue(String(NULL), String("cpuAbiOverride"), &cpuAbiOverrideStr);

    if (primaryCpuAbiStr.IsNull() && !legacyCpuAbiStr.IsNull()) {
        primaryCpuAbiStr = legacyCpuAbiStr;
    }

    if (resourcePathStr.IsNull()) {
        resourcePathStr = codePathStr;
    }
    String version;
    parser->GetAttributeValue(String(NULL), String("version"), &version);
    Int32 versionCode = 0;
    if (!version.IsNull()) {
        //try {
        versionCode = StringUtils::ParseInt32(version);
        // } catch (NumberFormatException e) {
        // }
    }

    Int32 pkgFlags = 0;
    pkgFlags |= IApplicationInfo::FLAG_SYSTEM;
    AutoPtr<IFile> codePathFile;
    CFile::New(codePathStr, (IFile**)&codePathFile);
    if (CPackageManagerService::LocationIsPrivileged(codePathFile)) {
        pkgFlags |= IApplicationInfo::FLAG_PRIVILEGED;
    }
    AutoPtr<IFile> resourcePathFile;
    CFile::New(resourcePathStr, (IFile**)&resourcePathFile);
    AutoPtr<PackageSetting> ps = new PackageSetting(name, realName, codePathFile,
            resourcePathFile, legacyNativeLibraryPathStr, primaryCpuAbiStr,
            secondaryCpuAbiStr, cpuAbiOverrideStr, versionCode, pkgFlags);
    String timeStampStr;
    parser->GetAttributeValue(String(NULL), String("ft"), &timeStampStr);
    if (!timeStampStr.IsNull()) {
        //try {
        Int64 timeStamp = StringUtils::ParseInt64(timeStampStr, 16);
        ps->SetTimeStamp(timeStamp);
        // } catch (NumberFormatException e) {
        // }
    }
    else {
        parser->GetAttributeValue(String(NULL), String("ts"), &timeStampStr);
        if (!timeStampStr.IsNull()) {
            //try {
            Int64 timeStamp = StringUtils::ParseInt64(timeStampStr);
            ps->SetTimeStamp(timeStamp);
            // } catch (NumberFormatException e) {
            // }
        }
    }

    parser->GetAttributeValue(String(NULL), String("it"), &timeStampStr);
    if (!timeStampStr.IsNull()) {
        //try {
            ps->mFirstInstallTime = StringUtils::ParseInt64(timeStampStr, 16);
        // } catch (NumberFormatException e) {
        // }
    }
    parser->GetAttributeValue(String(NULL), String("ut"), &timeStampStr);
    if (!timeStampStr.IsNull()) {
        //try {
            ps->mLastUpdateTime = StringUtils::ParseInt64(timeStampStr, 16);
        // } catch (NumberFormatException e) {
        // }
    }

    String idStr;
    parser->GetAttributeValue(String(NULL), String("userId"), &idStr);
    ps->mAppId = !idStr.IsNull() ? StringUtils::ParseInt32(idStr) : 0;
    if (ps->mAppId <= 0) {
        String sharedIdStr;
        parser->GetAttributeValue(String(NULL), String("sharedUserId"), &sharedIdStr);
        ps->mAppId = !sharedIdStr.IsNull() ? StringUtils::ParseInt32(sharedIdStr) : 0;
    }
    Int32 outerDepth, outerDepth1;
    parser->GetDepth(&outerDepth);
    Int32 type;
    while ((parser->Next(&type), type) != IXmlPullParser::END_DOCUMENT
            && (type != IXmlPullParser::END_TAG || (parser->GetDepth(&outerDepth1), outerDepth1) > outerDepth)) {
        if (type != IXmlPullParser::START_TAG) {
            continue;
        }

        String tagName;
        parser->GetName(&tagName);
        if (tagName.Equals("perms")) {
            ReadGrantedPermissionsLPw(parser, ps->mGrantedPermissions);
        }
        else {
            CPackageManagerService::ReportSettingsProblem(ILogHelper::WARN,
                    String("Unknown element under <updated-package>: ") + (parser->GetName(&temp), temp.IsNull() ? String("NULL") : temp));
            XmlUtils::SkipCurrentTag(parser);
        }
    }
    mDisabledSysPackages[name] = ps;
    return NOERROR;
}

ECode Settings::ReadPackageLPw(
    /* [in] */ IXmlPullParser* parser)
{
    String name;
    String realName;
    String idStr;
    String sharedIdStr;
    String codePathStr;
    String resourcePathStr;
    String legacyCpuAbiString;
    String legacyNativeLibraryPathStr;
    String primaryCpuAbiString;
    String secondaryCpuAbiString;
    String cpuAbiOverrideString;
    String systemStr;
    String installerPackageName;
    String uidError;
    Int32 pkgFlags = 0;
    Int64 timeStamp = 0;
    Int64 firstInstallTime = 0;
    Int64 lastUpdateTime = 0;
    AutoPtr<PackageSettingBase> packageSetting;
    String version;
    Int32 versionCode = 0;
//     try {
    parser->GetAttributeValue(String(NULL), ATTR_NAME, &name);
    parser->GetAttributeValue(String(NULL), String("realName"), &realName);
    parser->GetAttributeValue(String(NULL), String("userId"), &idStr);
    parser->GetAttributeValue(String(NULL), String("uidError"), &uidError);
    parser->GetAttributeValue(String(NULL), String("sharedUserId"), &sharedIdStr);
    parser->GetAttributeValue(String(NULL), String("codePath"), &codePathStr);
    parser->GetAttributeValue(String(NULL), String("resourcePath"), &resourcePathStr);

    parser->GetAttributeValue(String(NULL), String("requiredCpuAbi"), &legacyCpuAbiString);

    parser->GetAttributeValue(String(NULL), String("nativeLibraryPath"), &legacyNativeLibraryPathStr);
    parser->GetAttributeValue(String(NULL), String("primaryCpuAbi"), &primaryCpuAbiString);
    parser->GetAttributeValue(String(NULL), String("secondaryCpuAbi"), &secondaryCpuAbiString);
    parser->GetAttributeValue(String(NULL), String("cpuAbiOverride"), &cpuAbiOverrideString);

    if (primaryCpuAbiString.IsNull() && !legacyCpuAbiString.IsNull()) {
        primaryCpuAbiString = legacyCpuAbiString;
    }

    parser->GetAttributeValue(String(NULL), String("version"), &version);
    if (!version.IsNull()) {
        // try {
        versionCode = StringUtils::ParseInt32(version);
        // } catch (NumberFormatException e) {
        // }
    }
    parser->GetAttributeValue(String(NULL), String("installer"), &installerPackageName);

    parser->GetAttributeValue(String(NULL), String("flags"), &systemStr);
    if (!systemStr.IsNull()) {
        // try {
        pkgFlags = StringUtils::ParseInt32(systemStr);
        // } catch (NumberFormatException e) {
        // }
    }
    else {
        // For backward compatibility
        parser->GetAttributeValue(String(NULL), String("system"), &systemStr);
        if (!systemStr.IsNull()) {
            pkgFlags |= (systemStr.EqualsIgnoreCase("true")) ? IApplicationInfo::FLAG_SYSTEM
                    : 0;
        }
        else {
            // Old settings that don't specify system... just treat
            // them as system, good enough.
            pkgFlags |= IApplicationInfo::FLAG_SYSTEM;
        }
    }
    String timeStampStr;
    parser->GetAttributeValue(String(NULL), String("ft"), &timeStampStr);
    if (!timeStampStr.IsNull()) {
        // try {
        timeStamp = StringUtils::ParseInt64(timeStampStr, 16);
        // } catch (NumberFormatException e) {
        // }
    }
    else {
        parser->GetAttributeValue(String(NULL), String("ts"), &timeStampStr);
        if (!timeStampStr.IsNull()) {
            // try {
            timeStamp = StringUtils::ParseInt64(timeStampStr);
            // } catch (NumberFormatException e) {
            // }
        }
    }
    parser->GetAttributeValue(String(NULL), String("it"), &timeStampStr);
    if (!timeStampStr.IsNull()) {
        // try {
        firstInstallTime = StringUtils::ParseInt64(timeStampStr, 16);
        // } catch (NumberFormatException e) {
        // }
    }
    parser->GetAttributeValue(String(NULL), String("ut"), &timeStampStr);
    if (!timeStampStr.IsNull()) {
        // try {
        lastUpdateTime = StringUtils::ParseInt64(timeStampStr, 16);
        // } catch (NumberFormatException e) {
        // }
    }
    if (CPackageManagerService::DEBUG_SETTINGS)
        Slogger::I(TAG, String("Reading package: ") + name + String(" userId=") + idStr
               + String(" sharedUserId=") + sharedIdStr);
    Int32 userId = !idStr.IsNull() ? StringUtils::ParseInt32(idStr) : 0;
    if (resourcePathStr.IsNull()) {
        resourcePathStr = codePathStr;
    }
    // if (realName != null) {
    //     realName = realName.intern();
    // }
    String parserPositionDesc;
    parser->GetPositionDescription(&parserPositionDesc);
    StringBuilder sb;
    if (name.IsNull()) {
        sb += "Error in package manager settings: <package> has no name at ";
        sb += parserPositionDesc;
        CPackageManagerService::ReportSettingsProblem(ILogHelper::WARN, sb.ToString());
    }
    else if (codePathStr.IsNull()) {
        sb += "Error in package manager settings: <package> has no codePath at ";
        sb += parserPositionDesc;
        CPackageManagerService::ReportSettingsProblem(ILogHelper::WARN, sb.ToString());
    }
    else if (userId > 0) {
        AutoPtr<IFile> codePath, resourcePath;
        CFile::New(codePathStr, (IFile**)&codePath);
        CFile::New(resourcePathStr, (IFile**)&resourcePath);
        packageSetting = AddPackageLPw(name, realName, codePath,
                resourcePath, legacyNativeLibraryPathStr, primaryCpuAbiString,
                secondaryCpuAbiString, cpuAbiOverrideString, userId, versionCode, pkgFlags);
       if (CPackageManagerService::DEBUG_SETTINGS) {
            Slogger::I(TAG, "Reading package %s: userId=%d", name.string(), userId);
        }
        if (packageSetting == NULL) {
            sb += "Failure adding uid ";
            sb += userId;
            sb += " while parsing settings at ";
            sb += parserPositionDesc;
            CPackageManagerService::ReportSettingsProblem(ILogHelper::ERROR, sb.ToString());
        }
        else {
            packageSetting->SetTimeStamp(timeStamp);
            packageSetting->mFirstInstallTime = firstInstallTime;
            packageSetting->mLastUpdateTime = lastUpdateTime;
        }
    }
    else if (!sharedIdStr.IsNull()) {
        userId = StringUtils::ParseInt32(sharedIdStr);
        if (userId > 0) {
            AutoPtr<IFile> codePath, resourcePath;
            CFile::New(codePathStr, (IFile**)&codePath);
            CFile::New(resourcePathStr, (IFile**)&resourcePath);
            packageSetting = new PendingPackage(name, realName, codePath, resourcePath,
                    legacyNativeLibraryPathStr, primaryCpuAbiString, secondaryCpuAbiString,
                    cpuAbiOverrideString, userId, versionCode, pkgFlags);
            packageSetting->SetTimeStamp(timeStamp);
            packageSetting->mFirstInstallTime = firstInstallTime;
            packageSetting->mLastUpdateTime = lastUpdateTime;
            mPendingPackages.PushBack((PendingPackage*)packageSetting.Get());
           if (CPackageManagerService::DEBUG_SETTINGS) {
                Slogger::I(TAG, "Reading package %s: sharedUserId=%d", name.string(), userId);
            }
        }
        else {
            sb += "Error in package manager settings: package ";
            sb += name;
            sb += " has bad sharedId ";
            sb += sharedIdStr;
            sb += " at ";
            sb += parserPositionDesc;
            CPackageManagerService::ReportSettingsProblem(ILogHelper::WARN, sb.ToString());
        }
    }
    else {
        sb += "Error in package manager settings: package ";
        sb += name;
        sb += " has bad userId ";
        sb += idStr;
        sb += " at ";
        sb += parserPositionDesc;
        CPackageManagerService::ReportSettingsProblem(ILogHelper::WARN, sb.ToString());
    }
//     } catch (NumberFormatException e) {
//         PackageManagerService.reportSettingsProblem(Log.WARN,
//                 "Error in package manager settings: package " + name + " has bad userId "
//                         + idStr + " at " + parser.getPositionDescription());
//     }
    if (packageSetting != NULL) {
        packageSetting->mUidError = uidError.Equals("true");
        packageSetting->mInstallerPackageName = installerPackageName;
        packageSetting->mLegacyNativeLibraryPathString = legacyNativeLibraryPathStr;
        packageSetting->mPrimaryCpuAbiString = primaryCpuAbiString;
        packageSetting->mSecondaryCpuAbiString = secondaryCpuAbiString;
        // Handle legacy string here for single-user mode
        String enabledStr;
        parser->GetAttributeValue(String(NULL), ATTR_ENABLED, &enabledStr);
        if (!enabledStr.IsNull()) {
            // try {
            packageSetting->SetEnabled(StringUtils::ParseInt32(enabledStr), 0 /* userId */, String(NULL));
            // } catch (NumberFormatException e) {
            //     if (enabledStr.equalsIgnoreCase("true")) {
            //         packageSetting.setEnabled(COMPONENT_ENABLED_STATE_ENABLED, 0, null);
            //     } else if (enabledStr.equalsIgnoreCase("false")) {
            //         packageSetting.setEnabled(COMPONENT_ENABLED_STATE_DISABLED, 0, null);
            //     } else if (enabledStr.equalsIgnoreCase("default")) {
            //         packageSetting.setEnabled(COMPONENT_ENABLED_STATE_DEFAULT, 0, null);
            //     } else {
            //         PackageManagerService.reportSettingsProblem(Log.WARN,
            //                 "Error in package manager settings: package " + name
            //                         + " has bad enabled value: " + idStr + " at "
            //                         + parser.getPositionDescription());
            //     }
            // }
        }
        else {
            packageSetting->SetEnabled(IPackageManager::COMPONENT_ENABLED_STATE_DEFAULT, 0, String(NULL));
        }

        String installStatusStr;
        parser->GetAttributeValue(String(NULL), String("installStatus"), &installStatusStr);
        if (!installStatusStr.IsNull()) {
            if (installStatusStr.EqualsIgnoreCase("false")) {
                packageSetting->mInstallStatus = PackageSettingBase::PKG_INSTALL_INCOMPLETE;
            }
            else {
                packageSetting->mInstallStatus = PackageSettingBase::PKG_INSTALL_COMPLETE;
            }
        }

        Int32 outerDepth, depth, type;
        parser->GetDepth(&outerDepth);
        while ((parser->Next(&type), type) != IXmlPullParser::END_DOCUMENT
                && (type != IXmlPullParser::END_TAG || (parser->GetDepth(&depth), depth) > outerDepth)) {
            if (type == IXmlPullParser::END_TAG || type == IXmlPullParser::TEXT) {
                continue;
            }

            if (type != IXmlPullParser::START_TAG) {
                continue;
            }

            String tagName;
            parser->GetName(&tagName);
            // Legacy
            if (tagName.Equals(TAG_DISABLED_COMPONENTS)) {
                ReadDisabledComponentsLPw(packageSetting, parser, 0);
            }
            else if (tagName.Equals(TAG_ENABLED_COMPONENTS)) {
                ReadEnabledComponentsLPw(packageSetting, parser, 0);
            }
            else if (tagName.Equals("sigs")) {
                packageSetting->mSignatures->ReadXml(parser, mPastSignatures);
            }
            else if (tagName.Equals("perms")) {
                ReadGrantedPermissionsLPw(parser, packageSetting->mGrantedPermissions);
                packageSetting->mPermissionsFixed = TRUE;
            }
            else if (tagName.Equals("proper-signing-keyset")) {
                String value;
                parser->GetAttributeValue(String(NULL), String("identifier"), &value);
                Int64 id = StringUtils::ParseInt64(value);
                packageSetting->mKeySetData->SetProperSigningKeySet(id);
            }
            else if (tagName.Equals("signing-keyset")) {
                String value;
                parser->GetAttributeValue(String(NULL), String("identifier"), &value);
                Int64 id = StringUtils::ParseInt64(value);
                packageSetting->mKeySetData->AddSigningKeySet(id);
            }
            else if (tagName.Equals("upgrade-keyset")) {
                String value;
                parser->GetAttributeValue(String(NULL), String("identifier"), &value);
                Int64 id = StringUtils::ParseInt64(value);
                packageSetting->mKeySetData->AddUpgradeKeySetById(id);
            }
            else if (tagName.Equals("defined-keyset")) {
                String value;
                parser->GetAttributeValue(String(NULL), String("identifier"), &value);
                Int64 id = StringUtils::ParseInt64(value);
                String alias;
                parser->GetAttributeValue(String(NULL), String("alias"), &alias);
                packageSetting->mKeySetData->AddDefinedKeySet(id, alias);
            }
            else {
                String nameStr;
                parser->GetName(&nameStr);
                CPackageManagerService::ReportSettingsProblem(ILogHelper::WARN,
                       String("Unknown element under <package>: ") + nameStr);
                XmlUtils::SkipCurrentTag(parser);
            }
        }
    }
    else {
        XmlUtils::SkipCurrentTag(parser);
    }
    return NOERROR;
}

ECode Settings::ReadDisabledComponentsLPw(
    /* [in] */ PackageSettingBase* packageSetting,
    /* [in] */ IXmlPullParser* parser,
    /* [in] */ Int32 userId)
{
    Int32 outerDepth, outerDepth1;
    String positionDes, name;
    parser->GetDepth(&outerDepth);
    Int32 type;
    while ((parser->Next(&type), type) != IXmlPullParser::END_DOCUMENT
            && (type != IXmlPullParser::END_TAG || (parser->GetDepth(&outerDepth1), outerDepth1) > outerDepth)) {
        if (type == IXmlPullParser::END_TAG || type == IXmlPullParser::TEXT) {
            continue;
        }

        if (type != IXmlPullParser::START_TAG) {
            continue;
        }

        String tagName;
        parser->GetName(&tagName);
        if (tagName.Equals(TAG_ITEM)) {
            String name;
            parser->GetAttributeValue(String(NULL), ATTR_NAME, &name);
            if (!name.IsNull()) {
                packageSetting->AddDisabledComponent(name, userId);
            }
            else {
                CPackageManagerService::ReportSettingsProblem(ILogHelper::WARN,
                        String("Error in package manager settings: <disabled-components> has")
                                + String(" no name at ") + (parser->GetPositionDescription(&positionDes), positionDes));
            }
        }
        else {
            CPackageManagerService::ReportSettingsProblem(ILogHelper::WARN,
                    String("Unknown element under <disabled-components>: ") + (parser->GetName(&name), name.IsNull() ? String("NULL") : name));
        }
        XmlUtils::SkipCurrentTag(parser);
    }
    return NOERROR;
}

ECode Settings::ReadEnabledComponentsLPw(
    /* [in] */ PackageSettingBase* packageSetting,
    /* [in] */ IXmlPullParser* parser,
    /* [in] */ Int32 userId)
{
    Int32 outerDepth, outerDepth1;
    parser->GetDepth(&outerDepth);
    Int32 type;
    String positionDes, name;
    while ((parser->Next(&type), type) != IXmlPullParser::END_DOCUMENT
            && (type != IXmlPullParser::END_TAG || (parser->GetDepth(&outerDepth1), outerDepth1) > outerDepth)) {
        if (type == IXmlPullParser::END_TAG || type == IXmlPullParser::TEXT) {
            continue;
        }

        if (type != IXmlPullParser::START_TAG) {
            continue;
        }

        String tagName;
        parser->GetName(&tagName);
        if (tagName.Equals(TAG_ITEM)) {
            String name;
            parser->GetAttributeValue(String(NULL), ATTR_NAME, &name);
            if (!name.IsNull()) {
                packageSetting->AddEnabledComponent(name, userId);
            }
            else {
                CPackageManagerService::ReportSettingsProblem(ILogHelper::WARN,
                        String("Error in package manager settings: <enabled-components> has")
                                + String(" no name at ") + (parser->GetPositionDescription(&positionDes), positionDes));
            }
        }
        else {
            CPackageManagerService::ReportSettingsProblem(ILogHelper::WARN,
                    String("Unknown element under <enabled-components>: ") + (parser->GetName(&name), name.IsNull() ? String("NULL") : name));
        }
        XmlUtils::SkipCurrentTag(parser);
    }
    return NOERROR;
}

ECode Settings::ReadSharedUserLPw(
    /* [in] */ IXmlPullParser* parser)
{
    String name;
    String idStr;
    String temp;
    Int32 pkgFlags = 0;
    AutoPtr<SharedUserSetting> su;
    //try
    parser->GetAttributeValue(String(NULL), ATTR_NAME, &name);
    parser->GetAttributeValue(String(NULL), String("userId"), &idStr);
    Int32 userId = !idStr.IsNull() ? StringUtils::ParseInt32(idStr) : 0;
    parser->GetAttributeValue(String(NULL), String("system"), &temp);
    if (temp.Equals("true")) {
        pkgFlags |= IApplicationInfo::FLAG_SYSTEM;
    }
    if (name.IsNull()) {
        CPackageManagerService::ReportSettingsProblem(ILogHelper::WARN,
                String("Error in package manager settings: <shared-user> has no name at ")
                        + (parser->GetPositionDescription(&temp), temp));
    }
    else if (userId == 0) {
        CPackageManagerService::ReportSettingsProblem(ILogHelper::WARN,
                String("Error in package manager settings: shared-user ") + name
                        + String(" has bad userId ") + idStr + String(" at ")
                        + (parser->GetPositionDescription(&temp), temp));
    }
    else {
        if ((su = AddSharedUserLPw(name, userId, pkgFlags)) == NULL) {
            CPackageManagerService::ReportSettingsProblem(ILogHelper::ERROR, String("Occurred while parsing settings at ")
                    + (parser->GetPositionDescription(&temp), temp));
        }
    }
    // } catch (NumberFormatException e) {
    //     CPackageManagerService::ReportSettingsProblem(ILogHelper::WARN,
    //             "Error in package manager settings: package " + name + " has bad userId "
    //                     + idStr + " at " + parser->GetPositionDescription());
    // }

    if (su != NULL) {
        Int32 outerDepth, outerDepth1;
        parser->GetDepth(&outerDepth);
        Int32 type;
        while ((parser->Next(&type), type) != IXmlPullParser::END_DOCUMENT
                && (type != IXmlPullParser::END_TAG || (parser->GetDepth(&outerDepth1), outerDepth1) > outerDepth)) {
            if (type == IXmlPullParser::END_TAG || type == IXmlPullParser::TEXT) {
                continue;
            }

            if (type != IXmlPullParser::START_TAG) {
                continue;
            }

            String tagName;
            parser->GetName(&tagName);
            if (tagName.Equals("sigs")) {
                su->mSignatures->ReadXml(parser, mPastSignatures);
            }
            else if (tagName.Equals("perms")) {
                ReadGrantedPermissionsLPw(parser, su->mGrantedPermissions);
            }
            else {
                CPackageManagerService::ReportSettingsProblem(ILogHelper::WARN,
                        String("Unknown element under <shared-user>: ") + (parser->GetName(&temp), temp.IsNull() ? String("NULL") : temp));
                XmlUtils::SkipCurrentTag(parser);
            }
        }

    } else {
        XmlUtils::SkipCurrentTag(parser);
    }
    return NOERROR;
}

ECode Settings::ReadGrantedPermissionsLPw(
    /* [in] */ IXmlPullParser* parser,
    /* [in] */ HashSet<String>& outPerms)
{
    Int32 outerDepth, outerDepth1;
    parser->GetDepth(&outerDepth);
    Int32 type;
    String temp;
    while ((parser->Next(&type), type) != IXmlPullParser::END_DOCUMENT
            && (type != IXmlPullParser::END_TAG || (parser->GetDepth(&outerDepth1), outerDepth1) > outerDepth)) {
        if (type == IXmlPullParser::END_TAG || type == IXmlPullParser::TEXT) {
            continue;
        }

        if (type != IXmlPullParser::START_TAG) {
            continue;
        }

        String tagName;
        parser->GetName(&tagName);
        if (tagName.Equals(TAG_ITEM)) {
            String name;
            parser->GetAttributeValue(String(NULL), ATTR_NAME, &name);
            if (!name.IsNull()) {
                outPerms.Insert(name);
            }
            else {
                CPackageManagerService::ReportSettingsProblem(ILogHelper::WARN,
                        String("Error in package manager settings: <perms> has") + String(" no name at ")
                                + (parser->GetPositionDescription(&temp), temp));
            }
        }
        else {
            CPackageManagerService::ReportSettingsProblem(ILogHelper::WARN,
                    String("Unknown element under <perms>: ") + (parser->GetName(&temp), temp.IsNull() ? String("NULL") : temp));
        }
        XmlUtils::SkipCurrentTag(parser);
    }
    return NOERROR;
}

void Settings::CreateNewUserLILPw(
    /* [in] */ CPackageManagerService* service,
    /* [in] */ Installer* installer,
    /* [in] */ Int32 userHandle,
    /* [in] */ IFile* path)
{
    String temp;
    Boolean result;
    path->Mkdir(&result);
    path->ToString(&temp);
    FileUtils::SetPermissions(temp, FileUtils::sS_IRWXU | FileUtils::sS_IRWXG
            | FileUtils::sS_IXOTH, -1, -1);

    HashMap<String, AutoPtr<PackageSetting> >::Iterator it = mPackages.Begin();
    for (; it != mPackages.End(); it++) {
        AutoPtr<PackageSetting> ps = it->mSecond;
        if (ps->mPkg == NULL || ps->mPkg->mApplicationInfo == NULL) {
            continue;
        }
        // Only system apps are initially installed.
        ps->SetInstalled((ps->mPkgFlags & IApplicationInfo::FLAG_SYSTEM) != 0, userHandle);
        // Need to create a data directory for all apps under this user.
        String seinfo;
        ps->mPkg->mApplicationInfo->GetSeinfo(&seinfo);
        installer->CreateUserData(ps->mName,
                UserHandle::GetUid(userHandle, ps->mAppId), userHandle, seinfo);
    }
    ReadDefaultPreferredAppsLPw(service, userHandle);
    WritePackageRestrictionsLPr(userHandle);
}

void Settings::RemoveUserLPw(
    /* [in] */ Int32 userId)
{
    HashMap<String, AutoPtr<PackageSetting> >::Iterator it;
    for (it = mPackages.Begin(); it != mPackages.End(); ++it) {
        it->mSecond->RemoveUser(userId);
    }
    mPreferredActivities.Erase(userId);
    AutoPtr<IFile> file = GetUserPackagesStateFile(userId);
    Boolean result;
    file->Delete(&result);
    file = GetUserPackagesStateBackupFile(userId);
    file->Delete(&result);
    RemoveCrossProfileIntentFiltersLPw(userId);
}

void Settings::RemoveCrossProfileIntentFiltersLPw(
    /* [in] */ Int32 userId)
{
    synchronized (mCrossProfileIntentResolversLock) {
        // userId is the source user
        if (mCrossProfileIntentResolvers.Find(userId) != mCrossProfileIntentResolvers.End()) {
            mCrossProfileIntentResolvers.Erase(userId);
            WritePackageRestrictionsLPr(userId);
        }
        // userId is the target user
        HashMap<Int32, AutoPtr<CrossProfileIntentResolver> >::Iterator it = mCrossProfileIntentResolvers.Begin();
        for (; it != mCrossProfileIntentResolvers.End(); ++it) {
            Int32 sourceUserId = it->mFirst;
            AutoPtr<CrossProfileIntentResolver> cpir = it->mSecond;
            Boolean needsWriting = false;
            AutoPtr<HashSet<AutoPtr<CrossProfileIntentFilter> > > cpifs = cpir->FilterSet();
            HashSet<AutoPtr<CrossProfileIntentFilter> >::Iterator cpifIt = cpifs->Begin();
            for (; cpifIt != cpifs->End(); ++cpifIt) {
                AutoPtr<CrossProfileIntentFilter> cpif = *cpifIt;
                if (cpif->GetTargetUserId() == userId) {
                    needsWriting = TRUE;
                    cpir->RemoveFilter(cpif);
                }
            }
            if (needsWriting) {
                WritePackageRestrictionsLPr(sourceUserId);
            }
        }
    }
}

void Settings::SetFirstAvailableUid(
    /* [in] */ Int32 uid)
{
    if (uid > mFirstAvailableUid) {
        mFirstAvailableUid = uid;
    }
}

Int32 Settings::NewUserIdLPw(
    /* [in] */ IInterface* obj)
{
    // Let's be stupidly inefficient for now...
    List<AutoPtr<IInterface> >::Iterator it = mUserIds.Begin();
    for (Int32 i = 0; it != mUserIds.End(); ++it, ++i) {
        if (i == mFirstAvailableUid) break;
    }
    for (Int32 i = mFirstAvailableUid; it != mUserIds.End(); ++it, ++i) {
        if (*it == NULL) {
            *it = obj;
            return IProcess::FIRST_APPLICATION_UID + i;
        }
    }

    // None left?
    Int32 N = mUserIds.GetSize();
    if (N > (IProcess::LAST_APPLICATION_UID - IProcess::FIRST_APPLICATION_UID)) {
        return -1;
    }

    mUserIds.PushBack(obj);
    return IProcess::FIRST_APPLICATION_UID + N;
}

AutoPtr<IVerifierDeviceIdentity> Settings::GetVerifierDeviceIdentityLPw()
{
    if (mVerifierDeviceIdentity == NULL) {
        AutoPtr<IVerifierDeviceIdentityHelper> helper;
        CVerifierDeviceIdentityHelper::AcquireSingleton((IVerifierDeviceIdentityHelper**)&helper);
        helper->Generate((IVerifierDeviceIdentity**)&mVerifierDeviceIdentity);

        WriteLPr();
    }

    return mVerifierDeviceIdentity;
}

AutoPtr<PackageSetting> Settings::GetDisabledSystemPkgLPr(
    /* [in] */ const String& name)
{
    AutoPtr<PackageSetting> ps;
    HashMap<String, AutoPtr<PackageSetting> >::Iterator it = mDisabledSysPackages.Find(name);
    if (it != mDisabledSysPackages.End()) {
        ps = it->mSecond;
    }
    return ps;
}

String Settings::CompToString(
    /* [in] */ HashSet<String>* cmp)
{
    if (cmp != NULL) {
        Int32 size = cmp->GetSize();
        StringBuilder sb(size * 7);
        sb.AppendChar('[');
        HashSet<String>::Iterator it;
        for (it = cmp->Begin(); it != cmp->End(); ++it) {
            sb.Append(*it);
        }
        sb.AppendChar(']');
        return sb.ToString();
    }
    else {
        return String("[]");
    }
}

Boolean Settings::IsEnabledLPr(
    /* [in] */ IComponentInfo* componentInfo,
    /* [in] */ Int32 flags,
    /* [in] */ Int32 userId)
{
    if ((flags & IPackageManager::GET_DISABLED_COMPONENTS) != 0) {
        return TRUE;
    }
    AutoPtr<IPackageItemInfo> pii = IPackageItemInfo::Probe(componentInfo);
    String pkgName;
    pii->GetPackageName(&pkgName);
    String name;
    pii->GetName(&name);
    AutoPtr<PackageSetting> packageSettings;
    HashMap<String, AutoPtr<PackageSetting> >::Iterator it = mPackages.Find(pkgName);
    if (it != mPackages.End()) {
        packageSettings = it->mSecond;
    }
    if (CPackageManagerService::DEBUG_SETTINGS) {
        Slogger::V(TAG, "isEnabledLock - packageName = %s  componentName = %s"
                ,pkgName.string(), name.string());
        /*Slogger::V(TAG, "enabledComponents: "
                + compToString(packageSettings.getEnabledComponents(userId)));
        Slogger::V(TAG, "disabledComponents: "
                + compToString(packageSettings.getDisabledComponents(userId)));*/
    }
    if (packageSettings == NULL) {
        return FALSE;
    }
    AutoPtr<PackageUserState> ustate = packageSettings->ReadUserState(userId);
    if ((flags & IPackageManager::GET_DISABLED_UNTIL_USED_COMPONENTS) != 0) {
        if (ustate->mEnabled == IPackageManager::COMPONENT_ENABLED_STATE_DISABLED_UNTIL_USED) {
            return TRUE;
        }
    }
    Boolean enabled;
    if (ustate->mEnabled == IPackageManager::COMPONENT_ENABLED_STATE_DISABLED
            || ustate->mEnabled == IPackageManager::COMPONENT_ENABLED_STATE_DISABLED_USER
            || ustate->mEnabled == IPackageManager::COMPONENT_ENABLED_STATE_DISABLED_UNTIL_USED
            || (packageSettings->mPkg != NULL &&
                    (packageSettings->mPkg->mApplicationInfo->GetEnabled(&enabled), !enabled)
                    && ustate->mEnabled == IPackageManager::COMPONENT_ENABLED_STATE_DEFAULT)) {
        return FALSE;
    }

    if (ustate->mEnabledComponents != NULL
            && ustate->mEnabledComponents->Find(name) != ustate->mEnabledComponents->End()) {
        return TRUE;
    }
    if (ustate->mDisabledComponents != NULL
            && ustate->mDisabledComponents->Find(name) != ustate->mDisabledComponents->End()) {
        return FALSE;
    }
    componentInfo->GetEnabled(&enabled);
    return enabled;
}

ECode Settings::GetInstallerPackageNameLPr(
    /* [in] */ const String& packageName,
    /* [out] */ String* name)
{
    VALIDATE_NOT_NULL(name)
    *name = String(NULL);

    AutoPtr<PackageSetting> pkg;
    HashMap<String, AutoPtr<PackageSetting> >::Iterator it = mPackages.Find(packageName);
    if (it != mPackages.End()) {
        pkg = it->mSecond;
    }
    if (pkg == NULL) {
        // throw new IllegalArgumentException("Unknown package: " + packageName);
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    *name = pkg->mInstallerPackageName;
    return NOERROR;
}

ECode Settings::GetApplicationEnabledSettingLPr(
    /* [in] */ const String& packageName,
    /* [in] */ Int32 userId,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = 0;

    AutoPtr<PackageSetting> pkg;
    HashMap<String, AutoPtr<PackageSetting> >::Iterator it = mPackages.Find(packageName);
    if (it != mPackages.End()) {
        pkg = it->mSecond;
    }
    if (pkg == NULL) {
        // throw new IllegalArgumentException("Unknown package: " + packageName);
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    *result = pkg->GetEnabled(userId);
    return NOERROR;
}

ECode Settings::GetComponentEnabledSettingLPr(
    /* [in] */ IComponentName* componentName,
    /* [in] */ Int32 userId,
    /* [out] */ Int32* result)
{
    String packageName;
    componentName->GetPackageName(&packageName);
    AutoPtr<PackageSetting> pkg;
    HashMap<String, AutoPtr<PackageSetting> >::Iterator it = mPackages.Find(packageName);
    if (it != mPackages.End()) {
        pkg = it->mSecond;
    }
    if (pkg == NULL) {
        // throw new IllegalArgumentException("Unknown component: " + componentName);
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    String classNameStr;
    componentName->GetClassName(&classNameStr);
    *result = pkg->GetCurrentEnabledStateLPr(classNameStr, userId);
    return NOERROR;
}

ECode Settings::SetPackageStoppedStateLPw(
    /* [in] */ const String& packageName,
    /* [in] */ Boolean stopped,
    /* [in] */ Boolean allowedByPermission,
    /* [in] */ Int32 uid,
    /* [in] */ Int32 userId,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;
    AutoPtr<IUserHandleHelper> helper;
    CUserHandleHelper::AcquireSingleton((IUserHandleHelper**)&helper);
    Int32 appId;
    helper->GetAppId(uid, &appId);
    AutoPtr<PackageSetting> pkgSetting;
    HashMap<String, AutoPtr<PackageSetting> >::Iterator it = mPackages.Find(packageName);
    if (it != mPackages.End()) {
        pkgSetting = it->mSecond;
    }
    if (pkgSetting == NULL) {
        // throw new IllegalArgumentException("Unknown package: " + packageName);
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (!allowedByPermission && (appId != pkgSetting->mAppId)) {
        // throw new SecurityException(
        //         "Permission Denial: attempt to change stopped state from pid="
        //         + Binder.getCallingPid()
        //         + ", uid=" + uid + ", package uid=" + pkgSetting.appId);
        return E_SECURITY_EXCEPTION;
    }
//     if (DEBUG_STOPPED) {
//         if (stopped) {
//             RuntimeException e = new RuntimeException("here");
//             e.fillInStackTrace();
//             Slog.i(TAG, "Stopping package " + packageName, e);
//         }
//     }
    if (pkgSetting->GetStopped(userId) != stopped) {
        pkgSetting->SetStopped(stopped, userId);
        if (pkgSetting->GetNotLaunched(userId)) {
            if (!pkgSetting->mInstallerPackageName.IsNull()) {
                AutoPtr<ArrayOf<Int32> > iArray = ArrayOf<Int32>::Alloc(1);
                iArray->Set(0, userId);
                CPackageManagerService::SendPackageBroadcast(IIntent::ACTION_PACKAGE_FIRST_LAUNCH,
                       pkgSetting->mName, NULL,
                       pkgSetting->mInstallerPackageName, NULL, iArray);
            }
            pkgSetting->SetNotLaunched(FALSE, userId);
        }
        *result = TRUE;
        return NOERROR;
    }
    *result = FALSE;
    return NOERROR;
}

AutoPtr<IList> Settings::GetAllUsers()
{
    Int64 id = Binder::ClearCallingIdentity();
    //try {
    AutoPtr<IList> users;
    if (FAILED(CUserManagerService::GetInstance()->GetUsers(FALSE, (IList**)&users))) {
        users = NULL;
    }
    // } catch (NullPointerException npe) {
    //     // packagemanager not yet initialized
    // } finally {
    //     Binder.restoreCallingIdentity(id);
    // }
    Binder::RestoreCallingIdentity(id);
    return users;
}

void Settings::PrintFlags(
    /* [in] */ IPrintWriter* pw,
    /* [in] */ Int32 val,
    /* [in] */ ArrayOf<IInterface*>& spec)
{
    pw->Print(String("[ "));
    for (Int32 i = 0; i < spec.GetLength(); i += 2) {
        AutoPtr<IInteger32> mask = (IInteger32*)spec[i]->Probe(EIID_IInteger32);
        Int32 maskValue;
        mask->GetValue(&maskValue);
        if ((val & maskValue) != 0) {
            pw->Print(spec[i+1]);
            pw->Print(String(" "));
        }
    }
    pw->Print(String("]"));
}

} // namespace Pm
} // namespace Server
} // namespace Droid
} // namespace Elastos
