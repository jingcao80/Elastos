#ifndef __ELASTOS_DROID_SERVER_PM_SETTINGS_H__
#define __ELASTOS_DROID_SERVER_PM_SETTINGS_H__

#include "_Elastos.Droid.Server.h"
#include "elastos/droid/server/pm/CUserManagerService.h"
#include "elastos/droid/server/pm/PackageSetting.h"
#include "elastos/droid/server/pm/SharedUserSetting.h"
#include "elastos/droid/server/pm/PreferredIntentResolver.h"
#include "elastos/droid/server/pm/BasePermission.h"
#include "elastos/droid/server/pm/Installer.h"
#include "elastos/droid/server/pm/DumpState.h"
#include "elastos/droid/server/pm/PendingPackage.h"
#include "elastos/droid/server/pm/PersistentPreferredIntentResolver.h"
#include "elastos/droid/server/pm/CrossProfileIntentResolver.h"
#include "elastos/droid/server/pm/KeySetManagerService.h"
#include "elastos/droid/server/pm/PackageKeySetData.h"
#include "elastos/droid/content/pm/PackageParser.h"
#include <elastos/core/StringBuilder.h>

using Elastos::Core::IBoolean;
using Elastos::Core::StringBuilder;
using Elastos::IO::IFile;
using Elastos::Utility::IArrayList;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntentFilterAuthorityEntry;
using Elastos::Droid::Content::Pm::PackageParser;
using Elastos::Droid::Content::Pm::IPackageCleanItem;
using Elastos::Droid::Content::Pm::IUserInfo;
using Elastos::Droid::Content::Pm::IVerifierDeviceIdentity;
using Elastos::Droid::Content::Pm::IComponentInfo;
using Elastos::Droid::Os::IUserHandle;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Pm {

class Settings : public Object
{
public:
    /**
     * This class contains constants that can be referred to from upgrade code.
     * Insert constant values here that describe the upgrade reason. The version
     * code must be monotonically increasing.
     */
    class DatabaseVersion
    {
    public:
        /**
         * The initial version of the database.
         */
        static const Int32 FIRST_VERSION = 1;

        /**
         * Migrating the Signature array from the entire certificate chain to
         * just the signing certificate.
         */
        static const Int32 SIGNATURE_END_ENTITY = 2;

        /**
         * There was a window of time in
         * {@link android.os.Build.VERSION_CODES#LOLLIPOP} where we persisted
         * certificates after potentially mutating them. To switch back to the
         * original untouched certificates, we need to force a collection pass.
         */
        static const Int32 SIGNATURE_MALFORMED_RECOVER = 3;
    };

public:
    Settings(
        /* [in] */ IContext* context);

    Settings(
        /* [in] */ IContext* context,
        /* [in] */ IFile* dataDir);

    CARAPI_(AutoPtr<PackageSetting>) GetPackageLPw(
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
        /* [in] */ Boolean add);

    CARAPI_(AutoPtr<PackageSetting>) PeekPackageLPr(
        /* [in] */ const String& name);

    CARAPI_(void) SetInstallStatus(
        /* [in] */ const String& pkgName,
        /* [in] */ Int32 status);

    CARAPI_(void) SetInstallerPackageName(
        /* [in] */ const String& pkgName,
        /* [in] */ const String& installerPkgName);

    CARAPI_(AutoPtr<SharedUserSetting>) GetSharedUserLPw(
        /* [in] */ const String& name,
        /* [in] */ Int32 pkgFlags,
        /* [in] */ Boolean create);

    HashMap<String, AutoPtr<SharedUserSetting> >& GetAllSharedUsersLPw();

    CARAPI_(Boolean) DisableSystemPackageLPw(
        /* [in] */ const String& name);

    CARAPI_(AutoPtr<PackageSetting>) EnableSystemPackageLPw(
        /* [in] */ const String& name);

    CARAPI_(Boolean) IsDisabledSystemPackageLPr(
        /* [in] */ const String& name);

    CARAPI_(void) RemoveDisabledSystemPackageLPw(
        /* [in] */ const String& name);

    CARAPI_(AutoPtr<PackageSetting>) AddPackageLPw(
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
        /* [in] */ Int32 pkgFlags);

    CARAPI_(AutoPtr<SharedUserSetting>) AddSharedUserLPw(
        /* [in] */ const String& name,
        /* [in] */ Int32 uid,
        /* [in] */ Int32 pkgFlags);

    CARAPI_(void) PruneSharedUsersLPw();

    // Transfer ownership of permissions from one package to another.
    CARAPI_(void) TransferPermissionsLPw(
        /* [in] */ const String& origPkg,
        /* [in] */ const String& newPkg);

    CARAPI_(Boolean) IsAdbInstallDisallowed(
        /* [in] */ CUserManagerService* userManager,
        /* [in] */ Int32 userId);

    CARAPI_(void) InsertPackageSettingLPw(
        /* [in] */ PackageSetting* p,
        /* [in] */ PackageParser::Package* pkg);

    /*
     * Update the shared user setting when a package using
     * specifying the shared user id is removed. The gids
     * associated with each permission of the deleted package
     * are removed from the shared user's gid list only if its
     * not in use by other permissions of packages in the
     * shared user setting.
     */
    CARAPI_(void) UpdateSharedUserPermsLPw(
        /* [in] */ PackageSetting* deletedPs,
        /* [in] */ ArrayOf<Int32>* globalGids);

    CARAPI_(Int32) RemovePackageLPw(
        /* [in] */ const String& name);

    CARAPI_(AutoPtr<IInterface>) GetUserIdLPr(
        /* [in] */ Int32 uid);

    CARAPI_(AutoPtr<PreferredIntentResolver>) EditPreferredActivitiesLPw(
        /* [in] */ Int32 userId);

    CARAPI_(AutoPtr<PersistentPreferredIntentResolver>) EditPersistentPreferredActivitiesLPw(
        /* [in] */ Int32 userId);

    CARAPI_(AutoPtr<CrossProfileIntentResolver>) EditCrossProfileIntentResolverLPw(
        /* [in] */ Int32 userId);

    CARAPI_(void) WriteAllUsersPackageRestrictionsLPr();

    CARAPI_(void) ReadAllUsersPackageRestrictionsLPr();

    /**
     * Returns whether the current database has is older than {@code version}
     * for apps on internal storage.
     */
    CARAPI_(Boolean) IsInternalDatabaseVersionOlderThan(
        /* [in] */ Int32 version);

    /**
     * Returns whether the current database has is older than {@code version}
     * for apps on external storage.
     */
    CARAPI_(Boolean) IsExternalDatabaseVersionOlderThan(
        /* [in] */ Int32 version);

    /**
     * Updates the database version for apps on internal storage. Called after
     * call the updates to the database format are done for apps on internal
     * storage after the initial start-up scan.
     */
    CARAPI_(void) UpdateInternalDatabaseVersion();

    /**
     * Updates the database version for apps on internal storage. Called after
     * call the updates to the database format are done for apps on internal
     * storage after the initial start-up scan.
     */
    CARAPI_(void) UpdateExternalDatabaseVersion();

    CARAPI_(void) ReadPackageRestrictionsLPr(
        /* [in] */ Int32 userId);

    CARAPI WritePreferredActivitiesLPr(
        /* [in] */ IXmlSerializer* serializer,
        /* [in] */ Int32 userId,
        /* [in] */ Boolean full);

    CARAPI WritePersistentPreferredActivitiesLPr(
        /* [in] */ IXmlSerializer* serializer,
        /* [in] */ Int32 userId);

    CARAPI WriteCrossProfileIntentFiltersLPr(
        /* [in] */ IXmlSerializer* serializer,
        /* [in] */ Int32 userId);

    CARAPI_(void) WritePackageRestrictionsLPr(
        /* [in] */ Int32 userId);

    // Note: assumed "stopped" field is already cleared in all packages.
    // Legacy reader, used to read in the old file format after an upgrade. Not used after that.
    CARAPI_(void) ReadStoppedLPw();

    CARAPI_(void) WriteLPr();

    CARAPI_(void) WritePrebundledPackagesLPr();

    CARAPI_(void) ReadPrebundledPackagesLPr();

    CARAPI_(void) MarkPrebundledPackageInstalledLPr(
        /* [in] */ const String& packageName);

    CARAPI_(Boolean) WasPrebundledPackageInstalledLPr(
        /* [in] */ const String& packageName);

    CARAPI WriteDisabledSysPackageLPr(
        /* [in] */ IXmlSerializer* serializer,
        /* [in] */ PackageSetting* pkg);

    CARAPI WritePackageLPr(
        /* [in] */ IXmlSerializer* serializer,
        /* [in] */ PackageSetting* pkg);

    CARAPI WriteSigningKeySetsLPr(
        /* [in] */ IXmlSerializer* serializer,
        /* [in] */ PackageKeySetData* data);

    CARAPI WriteUpgradeKeySetsLPr(
        /* [in] */ IXmlSerializer* serializer,
        /* [in] */ PackageKeySetData* data);

    CARAPI WriteKeySetAliasesLPr(
        /* [in] */ IXmlSerializer* serializer,
        /* [in] */ PackageKeySetData* data);

    CARAPI WritePermissionLPr(
        /* [in] */ IXmlSerializer* serializer,
        /* [in] */ BasePermission* bp);

    CARAPI_(AutoPtr< List< AutoPtr<PackageSetting> > >) GetListOfIncompleteInstallPackagesLPr();

    CARAPI_(void) AddPackageToCleanLPw(
        /* [in] */ IPackageCleanItem* pkg);

    CARAPI_(Boolean) ReadLPw(
        /* [in] */ CPackageManagerService* service,
        /* [in] */ IList* users,
        /* [in] */ Int32 sdkVersion,
        /* [in] */ Boolean onlyCore);

    CARAPI_(void) ReadDefaultPreferredAppsLPw(
        /* [in] */ CPackageManagerService* service,
        /* [in] */ Int32 userId);

    CARAPI_(void) CreateNewUserLILPw(
        /* [in] */ CPackageManagerService* service,
        /* [in] */ Installer* installer,
        /* [in] */ Int32 userHandle,
        /* [in] */ IFile* path);

    CARAPI_(void) RemoveUserLPw(
        /* [in] */ Int32 userId);

    CARAPI_(void) RemoveCrossProfileIntentFiltersLPw(
        /* [in] */ Int32 userId);

    CARAPI_(AutoPtr<IVerifierDeviceIdentity>) GetVerifierDeviceIdentityLPw();

    CARAPI_(AutoPtr<PackageSetting>) GetDisabledSystemPkgLPr(
        /* [in] */ const String& name);

    CARAPI_(Boolean) IsEnabledLPr(
        /* [in] */ IComponentInfo* componentInfo,
        /* [in] */ Int32 flags,
        /* [in] */ Int32 userId);

    CARAPI GetInstallerPackageNameLPr(
        /* [in] */ const String& packageName,
        /* [out] */ String* name);

    CARAPI GetApplicationEnabledSettingLPr(
        /* [in] */ const String& packageName,
        /* [in] */ Int32 userId,
        /* [out] */ Int32* result);

    CARAPI GetComponentEnabledSettingLPr(
        /* [in] */ IComponentName* componentName,
        /* [in] */ Int32 userId,
        /* [out] */ Int32* result);

    CARAPI SetPackageStoppedStateLPw(
        /* [in] */ const String& packageName,
        /* [in] */ Boolean stopped,
        /* [in] */ Boolean allowedByPermission,
        /* [in] */ Int32 uid,
        /* [in] */ Int32 userId,
        /* [out] */ Boolean* result);

    static CARAPI_(void) PrintFlags(
        /* [in] */ IPrintWriter* pw,
        /* [in] */ Int32 val,
        /* [in] */ ArrayOf<IInterface*>& spec);

private:
    CARAPI_(void) Init(
        /* [in] */ IContext* context,
        /* [in] */ IFile* dataDir);

    CARAPI_(AutoPtr<PackageSetting>) GetPackageLPw(
        /* [in] */ const String& name,
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
        /* [in] */ Boolean allowInstall);

    // Utility method that adds a PackageSetting to mPackages and
    // completes updating the shared user attributes
    CARAPI_(void) AddPackageSettingLPw(
        /* [in] */ PackageSetting* p,
        /* [in] */ const String& name,
        /* [in] */ SharedUserSetting* sharedUser);

    CARAPI_(void) ReplacePackageLPw(
        /* [in] */ const String& name,
        /* [in] */ PackageSetting* newp);

    CARAPI_(Boolean) AddUserIdLPw(
        /* [in] */ Int32 uid,
        /* [in] */ IInterface* obj,
        /* [in] */ IInterface* name);

    CARAPI_(void) RemoveUserIdLPw(
        /* [in] */ Int32 uid);

    CARAPI_(void) ReplaceUserIdLPw(
        /* [in] */ Int32 uid,
        /* [in] */ IInterface* obj);

    CARAPI_(AutoPtr<IFile>) GetUserPackagesStateFile(
        /* [in] */ Int32 userId);

    CARAPI_(AutoPtr<IFile>) GetUserPackagesStateBackupFile(
        /* [in] */ Int32 userId);

    CARAPI ReadPreferredActivitiesLPw(
        /* [in] */ IXmlPullParser* parser,
        /* [in] */ Int32 userId);

    CARAPI ReadPersistentPreferredActivitiesLPw(
        /* [in] */ IXmlPullParser* parser,
        /* [in] */ Int32 userId);

    CARAPI ReadCrossProfileIntentFiltersLPw(
        /* [in] */ IXmlPullParser* parser,
        /* [in] */ Int32 userId);

    CARAPI_(AutoPtr<HashSet<Elastos::String> >) ReadComponentsLPr(
        /* [in] */ IXmlPullParser* parser);

    CARAPI_(void) ApplyDefaultPreferredActivityLPw(
        /* [in] */ CPackageManagerService* service,
        /* [in] */ IIntentFilter* tmpPa,
        /* [in] */ IComponentName* cn,
        /* [in] */ Int32 userId);

    CARAPI_(void) ApplyDefaultPreferredActivityLPw(
        /* [in] */ CPackageManagerService* service,
        /* [in] */ IIntent* intent,
        /* [in] */ Int32 flags,
        /* [in] */ IComponentName* cn,
        /* [in] */ const String& scheme,
        /* [in] */ IPatternMatcher* ssp,
        /* [in] */ IIntentFilterAuthorityEntry* auth,
        /* [in] */ IPatternMatcher* path,
        /* [in] */ Int32 userId);

    CARAPI ReadDefaultPreferredActivitiesLPw(
        /* [in] */ CPackageManagerService* service,
        /* [in] */ IXmlPullParser* parser,
        /* [in] */ Int32 userId);

    CARAPI_(Int32) ReadInt32(
        /* [in] */ IXmlPullParser* parser,
        /* [in] */ const String& ns,
        /* [in] */ const String& name,
        /* [in] */ Int32 defValue);

    CARAPI ReadPermissionsLPw(
        /* [in] */ HashMap<String, AutoPtr<BasePermission> >& out,
        /* [in] */ IXmlPullParser* parser);

    CARAPI ReadDisabledSysPackageLPw(
        /* [in] */ IXmlPullParser* parser);

    CARAPI ReadPackageLPw(
        /* [in] */ IXmlPullParser* parser);

    CARAPI ReadDisabledComponentsLPw(
        /* [in] */ PackageSettingBase* packageSetting,
        /* [in] */ IXmlPullParser* parser,
        /* [in] */ Int32 userId);

    CARAPI ReadEnabledComponentsLPw(
        /* [in] */ PackageSettingBase* packageSetting,
        /* [in] */ IXmlPullParser* parser,
        /* [in] */ Int32 userId);

    CARAPI ReadSharedUserLPw(
        /* [in] */ IXmlPullParser* parser);

    CARAPI ReadGrantedPermissionsLPw(
        /* [in] */ IXmlPullParser* parser,
        /* [in] */ HashSet<String>& outPerms);

    // This should be called (at least) whenever an application is removed
    CARAPI_(void) SetFirstAvailableUid(
        /* [in] */ Int32 uid);

    // Returns -1 if we could not find an available UserId to assign
    CARAPI_(Int32) NewUserIdLPw(
        /* [in] */ IInterface* obj);

    CARAPI_(String) CompToString(
        /* [in] */ HashSet<String>* cmp);

    CARAPI_(AutoPtr<IList>) GetAllUsers();

public:
    static const String TAG_CROSS_PROFILE_INTENT_FILTERS;

    HashMap<String, AutoPtr<PackageSetting> > mPackages;

    // These are the last platform API version we were using for
    // the apps installed on internal and external storage.  It is
    // used to grant newer permissions one time during a system upgrade.
    Int32 mInternalSdkPlatform;
    Int32 mExternalSdkPlatform;

    /**
     * The current database version for apps on internal storage. This is
     * used to upgrade the format of the packages.xml database not necessarily
     * tied to an SDK version.
     */
    Int32 mInternalDatabaseVersion;
    Int32 mExternalDatabaseVersion;

    /**
     * Last known value of {@link Build#FINGERPRINT}. Used to determine when an
     * system update has occurred, meaning we need to clear code caches.
     */
    String mFingerprint;

    AutoPtr<IBoolean> mReadExternalStorageEnforced;

    // The user's preferred activities associated with particular intent
    // filters.
    HashMap<Int32, AutoPtr<PreferredIntentResolver> > mPreferredActivities;

    // The persistent preferred activities of the user's profile/device owner
    // associated with particular intent filters.
    HashMap<Int32, AutoPtr<PersistentPreferredIntentResolver> > mPersistentPreferredActivities;

    // For every user, it is used to find to which other users the intent can be forwarded.
    HashMap<Int32, AutoPtr<CrossProfileIntentResolver> > mCrossProfileIntentResolvers;
    Object mCrossProfileIntentResolversLock;

    HashMap<String, AutoPtr<SharedUserSetting> > mSharedUsers;

    // Mapping from permission names to info about them.
    HashMap<String, AutoPtr<BasePermission> > mPermissions;

    // Mapping from permission tree names to info about them.
    HashMap<String, AutoPtr<BasePermission> > mPermissionTrees;

    // Packages that have been uninstalled and still need their external
    // storage data deleted.
    AutoPtr<IArrayList> mPackagesToBeCleaned; // item is IPackageCleanItem

    // Packages that have been renamed since they were first installed.
    // Keys are the new names of the packages, values are the original
    // names.  The packages appear everwhere else under their original
    // names.
    HashMap<String, String> mRenamedPackages;

    AutoPtr<StringBuilder> mReadMessages;

    AutoPtr<KeySetManagerService> mKeySetManagerService;

private:
    static const String TAG;

    /**
     * Current version of the package database. Set it to the latest version in
     * the {@link DatabaseVersion} class below to ensure the database upgrade
     * doesn't happen repeatedly.
     * <p>
     * Note that care should be taken to make sure all database upgrades are
     * idempotent.
     */
    static const Int32 CURRENT_DATABASE_VERSION = DatabaseVersion::SIGNATURE_MALFORMED_RECOVER;

    static const Boolean DEBUG_STOPPED;
    static const Boolean DEBUG_MU;

    static const String TAG_READ_EXTERNAL_STORAGE;
    static const String ATTR_ENFORCEMENT;

    static const String TAG_ITEM;
    static const String TAG_DISABLED_COMPONENTS;
    static const String TAG_ENABLED_COMPONENTS;
    static const String TAG_PACKAGE_RESTRICTIONS;
    static const String TAG_PACKAGE;
    static const String TAG_PERSISTENT_PREFERRED_ACTIVITIES;

    static const String TAG_PROTECTED_COMPONENTS;
    static const String TAG_VISIBLE_COMPONENTS;

    static const String ATTR_NAME;
    static const String ATTR_USER;
    static const String ATTR_CODE;
    static const String ATTR_NOT_LAUNCHED;
    static const String ATTR_ENABLED;
    static const String ATTR_ENABLED_CALLER;
    static const String ATTR_STOPPED;
    // Legacy, here for reading older versions of the package-restrictions.
    static const String ATTR_BLOCKED;
    // New name for the above attribute.
    static const String ATTR_HIDDEN;
    static const String ATTR_INSTALLED;
    static const String ATTR_BLOCK_UNINSTALL;

    AutoPtr<IFile> mSettingsFilename;
    AutoPtr<IFile> mBackupSettingsFilename;
    AutoPtr<IFile> mPackageListFilename;
    AutoPtr<IFile> mStoppedPackagesFilename;
    AutoPtr<IFile> mBackupStoppedPackagesFilename;
    AutoPtr<IFile> mPrebundledPackagesFilename;

    // List of replaced system applications
    HashMap<String, AutoPtr<PackageSetting> > mDisabledSysPackages;
    HashSet<String> mPrebundledPackages;

    static Int32 mFirstAvailableUid;

    /** Device identity for the purpose of package verification. */
    AutoPtr<IVerifierDeviceIdentity> mVerifierDeviceIdentity;

    List< AutoPtr<IInterface> > mUserIds;
    HashMap<Int32, AutoPtr<IInterface> > mOtherUserIds;

    // For reading/writing settings file.
    List< AutoPtr<ISignature> > mPastSignatures;

    /**
     * Used to track packages that have a shared user ID that hasn't been read
     * in yet.
     * <p>
     * TODO: make this just a local variable that is passed in during package
     * scanning to make it less confusing.
     */
    List< AutoPtr<PendingPackage> > mPendingPackages;

    AutoPtr<IFile> mSystemDir;
};

} // namespace Pm
} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SERVER_PM_SETTINGS_H__
