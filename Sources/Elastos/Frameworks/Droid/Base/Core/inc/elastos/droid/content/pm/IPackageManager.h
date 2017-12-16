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

#ifndef __ELASTOS_DROID_CONTENT_PM_IPACKAGEMANAGER_H__
#define __ELASTOS_DROID_CONTENT_PM_IPACKAGEMANAGER_H__

#include "Elastos.Droid.App.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.CoreLibrary.Utility.h"
#include <elastos/core/Object.h>
#include <binder/Binder.h>

using Elastos::Droid::App::IComposedIconInfo;
using Elastos::Core::Object;
using Elastos::Utility::IList;

namespace Elastos {
namespace Droid {
namespace Content {
namespace Pm {

class IPackageManagerProxy
    : public Object
    , public IIPackageManager
{
public:
    IPackageManagerProxy(
        /* [in] */ android::sp<android::IBinder>& pm);

    CAR_INTERFACE_DECL();

    CARAPI IsPackageAvailable(
        /* [in] */ const String& packageName,
        /* [in] */ Int32 userId,
        /* [out] */ Boolean* result);

    CARAPI GetPackageInfo(
        /* [in] */ const String& packageName,
        /* [in] */ Int32 flags,
        /* [in] */ Int32 userId,
        /* [out] */ IPackageInfo** info);

    CARAPI GetPackageUid(
        /* [in] */ const String& packageName,
        /* [in] */ Int32 userId,
        /* [out] */ Int32* uid);

    CARAPI GetPackageGids(
        /* [in] */ const String& packageName,
        /* [out, callee] */ ArrayOf<Int32>** gids);

    CARAPI CurrentToCanonicalPackageNames(
        /* [in] */ const ArrayOf<String>& cuNames,
        /* [out, callee] */ ArrayOf<String>** caNames);

    CARAPI CanonicalToCurrentPackageNames(
        /* [in] */ const ArrayOf<String>& caNames,
        /* [out, callee] */ ArrayOf<String>** cuNames);

    CARAPI GetPermissionInfo(
        /* [in] */ const String& name,
        /* [in] */ Int32 flags,
        /* [out] */ IPermissionInfo** info);

    CARAPI QueryPermissionsByGroup(
        /* [in] */ const String& group,
        /* [in] */ Int32 flags,
        /* [out] */ IList** infos);

    CARAPI GetPermissionGroupInfo(
        /* [in] */ const String& name,
        /* [in] */ Int32 flags,
        /* [out] */ IPermissionGroupInfo** info);

    CARAPI GetAllPermissionGroups(
        /* [in] */ Int32 flags,
        /* [out] */ IList** infos);

    CARAPI GetApplicationInfo(
        /* [in] */ const String& packageName,
        /* [in] */ Int32 flags,
        /* [in] */ Int32 userId,
        /* [out] */ IApplicationInfo** info);

    CARAPI GetActivityInfo(
        /* [in] */ IComponentName* className,
        /* [in] */ Int32 flags,
        /* [in] */ Int32 userId,
        /* [out] */ IActivityInfo** info);

    CARAPI ActivitySupportsIntent(
        /* [in] */ IComponentName* className,
        /* [in] */ IIntent* intent,
        /* [in] */ const String& resolvedType,
        /* [out] */ Boolean* result);

    CARAPI GetReceiverInfo(
        /* [in] */ IComponentName* className,
        /* [in] */ Int32 flags,
        /* [in] */ Int32 userId,
        /* [out] */ IActivityInfo** info);

    CARAPI GetServiceInfo(
        /* [in] */ IComponentName* className,
        /* [in] */ Int32 flags,
        /* [in] */ Int32 userId,
        /* [out] */ IServiceInfo** info);

    CARAPI GetProviderInfo(
        /* [in] */ IComponentName* className,
        /* [in] */ Int32 flags,
        /* [in] */ Int32 userId,
        /* [out] */ IProviderInfo** info);

    CARAPI CheckPermission(
        /* [in] */ const String& permName,
        /* [in] */ const String& pkgName,
        /* [out] */ Int32* result);

    CARAPI CheckUidPermission(
        /* [in] */ const String& permName,
        /* [in] */ Int32 uid,
        /* [out] */ Int32* result);

    CARAPI AddPermission(
        /* [in] */ IPermissionInfo* info,
        /* [out] */ Boolean* result);

    CARAPI RemovePermission(
        /* [in] */ const String& name);

    CARAPI GrantPermission(
        /* [in] */ const String& packageName,
        /* [in] */ const String& permissionName);

    CARAPI RevokePermission(
        /* [in] */ const String& packageName,
        /* [in] */ const String& permissionName);

    CARAPI IsProtectedBroadcast(
        /* [in] */ const String& actionName,
        /* [out] */ Boolean* result);

    CARAPI CheckSignatures(
        /* [in] */ const String& pkg1,
        /* [in] */ const String& pkg2,
        /* [out] */ Int32* result);

    CARAPI CheckUidSignatures(
        /* [in] */ Int32 uid1,
        /* [in] */ Int32 uid2,
        /* [out] */ Int32* result);

    CARAPI GetPackagesForUid(
        /* [in] */ Int32 uid,
        /* [out, callee] */ ArrayOf<String>** pkgs);

    CARAPI GetNameForUid(
        /* [in] */ Int32 uid,
        /* [out] */ String* name);

    CARAPI GetUidForSharedUser(
        /* [in] */ const String& sharedUserName,
        /* [out] */ Int32* uid);

    CARAPI GetFlagsForUid(
        /* [in] */ Int32 uid,
        /* [out] */ Int32* result);

    CARAPI IsUidPrivileged(
        /* [in] */ Int32 uid,
        /* [out] */ Boolean* result);

    CARAPI GetAppOpPermissionPackages(
        /* [in] */ const String& permissionName,
        /* [out, callee] */ ArrayOf<String>** result);

    CARAPI ResolveIntent(
        /* [in] */ IIntent* intent,
        /* [in] */ const String& resolvedType,
        /* [in] */ Int32 flags,
        /* [in] */ Int32 userId,
        /* [out] */ IResolveInfo** info);

    CARAPI CanForwardTo(
        /* [in] */ IIntent* intent,
        /* [in] */ const String& resolvedType,
        /* [in] */ Int32 sourceUserId,
        /* [in] */ Int32 targetUserId,
        /* [out] */ Boolean* result);

    CARAPI QueryIntentActivities(
        /* [in] */ IIntent* intent,
        /* [in] */ const String& resolvedType,
        /* [in] */ Int32 flags,
        /* [in] */ Int32 userId,
        /* [out] */ IList** infos);

    CARAPI QueryIntentActivityOptions(
        /* [in] */ IComponentName* caller,
        /* [in] */ ArrayOf<IIntent*>* specifics,
        /* [in] */ ArrayOf<String>* specificTypes,
        /* [in] */ IIntent* intent,
        /* [in] */ const String& resolvedType,
        /* [in] */ Int32 flags,
        /* [in] */ Int32 userId,
        /* [out] */ IList** infos);

    CARAPI QueryIntentReceivers(
        /* [in] */ IIntent* intent,
        /* [in] */ const String& resolvedType,
        /* [in] */ Int32 flags,
        /* [in] */ Int32 userId,
        /* [out] */ IList** receivers);

    CARAPI ResolveService(
        /* [in] */ IIntent* intent,
        /* [in] */ const String& resolvedType,
        /* [in] */ Int32 flags,
        /* [in] */ Int32 userId,
        /* [out] */ IResolveInfo** resolveInfo);

    CARAPI QueryIntentServices(
        /* [in] */ IIntent* intent,
        /* [in] */ const String& resolvedType,
        /* [in] */ Int32 flags,
        /* [in] */ Int32 userId,
        /* [out] */ IList** infos);

    CARAPI QueryIntentContentProviders(
        /* [in] */ IIntent* intent,
        /* [in] */ const String& resolvedType,
        /* [in] */ Int32 flags,
        /* [in] */ Int32 userId,
        /* [out] */ IList** infos);

    CARAPI GetInstalledPackages(
        /* [in] */ Int32 flags,
        /* [in] */ Int32 userId,
        /* [out] */ IParceledListSlice** slice);

    CARAPI GetPackagesHoldingPermissions(
        /* [in] */ ArrayOf<String>* permissions,
        /* [in] */ Int32 flags,
        /* [in] */ Int32 userId,
        /* [out] */ IParceledListSlice** slice);

    CARAPI GetInstalledApplications(
        /* [in] */ Int32 flags,
        /* [in] */ Int32 userId,
        /* [out] */ IParceledListSlice** slice);

    CARAPI GetPersistentApplications(
        /* [in] */ Int32 flags,
        /* [out] */ IList** infos);

    CARAPI ResolveContentProvider(
        /* [in] */ const String& name,
        /* [in] */ Int32 flags,
        /* [in] */ Int32 userId,
        /* [out] */ IProviderInfo** info);

    CARAPI QuerySyncProviders(
        /* [in, out] */ IList* outNames,
        /* [in, out] */ IList* outInfo);

    CARAPI QueryContentProviders(
        /* [in] */ const String& processName,
        /* [in] */ Int32 uid,
        /* [in] */ Int32 flags,
        /* [out] */ IList** providers);

    CARAPI GetInstrumentationInfo(
        /* [in] */ IComponentName* className,
        /* [in] */ Int32 flags,
        /* [out] */ IInstrumentationInfo** info);

    CARAPI QueryInstrumentation(
        /* [in] */ const String& targetPackage,
        /* [in] */ Int32 flags,
        /* [out] */ IList** infos);

    CARAPI InstallPackage(
        /* [in] */ const String& originPath,
        /* [in] */ IIPackageInstallObserver2* observer,
        /* [in] */ Int32 flags,
        /* [in] */ const String& installerPackageName,
        /* [in] */ IVerificationParams* verificationParams,
        /* [in] */ const String& packageAbiOverride);

    CARAPI InstallPackageAsUser(
        /* [in] */ const String& originPath,
        /* [in] */ IIPackageInstallObserver2* observer,
        /* [in] */ Int32 flags,
        /* [in] */ const String& installerPackageName,
        /* [in] */ IVerificationParams* verificationParams,
        /* [in] */ const String& packageAbiOverride,
        /* [in] */ Int32 userId);

    CARAPI FinishPackageInstall(
        /* [in] */ Int32 token);

    CARAPI SetInstallerPackageName(
        /* [in] */ const String& targetPackage,
        /* [in] */ const String& installerPackageName);

    CARAPI DeletePackageAsUser(
        /* [in] */ const String& packageName,
        /* [in] */ IIPackageDeleteObserver* observer,
        /* [in] */ Int32 userId,
        /* [in] */ Int32 flags);

    CARAPI DeletePackage(
        /* [in] */ const String& packageName,
        /* [in] */ IIPackageDeleteObserver2* observer,
        /* [in] */ Int32 userId,
        /* [in] */ Int32 flags);

    CARAPI GetInstallerPackageName(
        /* [in] */ const String& packageName,
        /* [out] */ String* pkgName);

    CARAPI AddPackageToPreferred(
        /* [in] */ const String& packageName);

    CARAPI RemovePackageFromPreferred(
        /* [in] */ const String& packageName);

    CARAPI GetPreferredPackages(
        /* [in] */ Int32 flags,
        /* [out] */ IList** infos);

    CARAPI ResetPreferredActivities(
        /* [in] */ Int32 userId);

    CARAPI GetLastChosenActivity(
        /* [in] */ IIntent* intent,
        /* [in] */ const String& resolvedType,
        /* [in] */ Int32 flags,
        /* [out] */ IResolveInfo** info);

    CARAPI SetLastChosenActivity(
        /* [in] */ IIntent* intent,
        /* [in] */ const String& resolvedType,
        /* [in] */ Int32 flags,
        /* [in] */ IIntentFilter* filter,
        /* [in] */ Int32 match,
        /* [in] */ IComponentName* activity);

    CARAPI AddPreferredActivity(
        /* [in] */ IIntentFilter* filter,
        /* [in] */ Int32 match,
        /* [in] */ ArrayOf<IComponentName*>* set,
        /* [in] */ IComponentName* activity,
        /* [in] */ Int32 userId);

    CARAPI ReplacePreferredActivity(
        /* [in] */ IIntentFilter* filter,
        /* [in] */ Int32 match,
        /* [in] */ ArrayOf<IComponentName*>* set,
        /* [in] */ IComponentName* activity,
        /* [in] */ Int32 userId);

    CARAPI ClearPackagePreferredActivities(
        /* [in] */ const String& packageName);

    CARAPI GetPreferredActivities(
        /* [in, out] */ IList* outFilters,
        /* [in, out] */ IList* outActivities,
        /* [in] */ const String& packageName,
        /* [out] */ Int32* value);

    CARAPI AddPersistentPreferredActivity(
        /* [in] */ IIntentFilter* filter,
        /* [in] */ IComponentName* activity,
        /* [in] */ Int32 userId);

    CARAPI ClearPackagePersistentPreferredActivities(
        /* [in] */ const String& packageName,
        /* [in] */ Int32 userId);

    CARAPI AddCrossProfileIntentFilter(
        /* [in] */ IIntentFilter* intentFilter,
        /* [in] */ const String& ownerPackage,
        /* [in] */ Int32 ownerUserId,
        /* [in] */ Int32 sourceUserId,
        /* [in] */ Int32 targetUserId,
        /* [in] */ Int32 flags);

    CARAPI ClearCrossProfileIntentFilters(
        /* [in] */ Int32 sourceUserId,
        /* [in] */ const String& ownerPackage,
        /* [in] */ Int32 ownerUserId);

    CARAPI GetHomeActivities(
        /* [in] */ IList* outHomeCandidates,
        /* [out] */ IComponentName** name);

    CARAPI SetComponentEnabledSetting(
        /* [in] */ IComponentName* componentName,
        /* [in] */ Int32 newState,
        /* [in] */ Int32 flags,
        /* [in] */ Int32 userId);

    CARAPI GetComponentEnabledSetting(
        /* [in] */ IComponentName* componentName,
        /* [in] */ Int32 userId,
        /* [out] */ Int32* value);

    CARAPI SetApplicationEnabledSetting(
        /* [in] */ const String& packageName,
        /* [in] */ Int32 newState,
        /* [in] */ Int32 flags,
        /* [in] */ Int32 userId,
        /* [in] */ const String& callingPackage);

    CARAPI GetApplicationEnabledSetting(
        /* [in] */ const String& packageName,
        /* [in] */ Int32 userId,
        /* [out] */ Int32* value);

    CARAPI SetPackageStoppedState(
        /* [in] */ const String& packageName,
        /* [in] */ Boolean stopped,
        /* [in] */ Int32 userId);

    CARAPI FreeStorageAndNotify(
        /* [in] */ Int64 freeStorageSize,
        /* [in] */ IIPackageDataObserver* observer);

    CARAPI FreeStorage(
        /* [in] */ Int64 freeStorageSize,
        /* [in] */ IIntentSender* pi);

    CARAPI DeleteApplicationCacheFiles(
        /* [in] */ const String& packageName,
        /* [in] */ IIPackageDataObserver* observer);

    CARAPI ClearApplicationUserData(
        /* [in] */ const String& packageName,
        /* [in] */ IIPackageDataObserver* observer,
        /* [in] */ Int32 userId);

    CARAPI GetPackageSizeInfo(
        /* [in] */ const String& packageName,
        /* [in] */ Int32 userHandle,
        /* [in] */ IIPackageStatsObserver* observer);

    CARAPI GetSystemSharedLibraryNames(
        /* [out, callee] */ ArrayOf<String>** names);

    CARAPI GetSystemAvailableFeatures(
        /* [out, callee] */ ArrayOf<IFeatureInfo*>** infos);

    CARAPI HasSystemFeature(
        /* [in] */ const String& name,
        /* [out] */ Boolean* result);

    CARAPI EnterSafeMode();

    CARAPI IsSafeMode(
        /* [out] */ Boolean* isSafeMode);

    CARAPI SystemReady();

    CARAPI HasSystemUidErrors(
        /* [out] */ Boolean* result);

    CARAPI PerformBootDexOpt();

    CARAPI PerformDexOptIfNeeded(
        /* [in] */ const String& packageName,
        /* [in] */ const String& instructionSet,
        /* [out] */ Boolean* result);

    CARAPI ForceDexOpt(
        /* [in] */ const String& packageName);

    CARAPI UpdateExternalMediaStatus(
        /* [in] */ Boolean mounted,
        /* [in] */ Boolean reportStatus);

    CARAPI NextPackageToClean(
        /* [in] */ IPackageCleanItem* lastPackage,
        /* [out] */ IPackageCleanItem** nextPackage);

    CARAPI MovePackage(
        /* [in] */ const String& packageName,
        /* [in] */ IIPackageMoveObserver* observer,
        /* [in] */ Int32 flags);

    CARAPI AddPermissionAsync(
        /* [in] */ IPermissionInfo* info,
        /* [out] */ Boolean* result);

    CARAPI SetInstallLocation(
        /* [in] */ Int32 loc,
        /* [out] */ Boolean* result);

    CARAPI GetInstallLocation(
        /* [out] */ Int32* location);

    CARAPI InstallExistingPackageAsUser(
        /* [in] */ const String& packageName,
        /* [in] */ Int32 userId,
        /* [out] */ Int32* result);

    CARAPI VerifyPendingInstall(
        /* [in] */ Int32 id,
        /* [in] */ Int32 verificationCode);

    CARAPI ExtendVerificationTimeout(
        /* [in] */ Int32 id,
        /* [in] */ Int32 verificationCodeAtTimeout,
        /* [in] */ Int64 millisecondsToDelay);

    CARAPI GetVerifierDeviceIdentity(
        /* [out] */ IVerifierDeviceIdentity** identity);

    CARAPI IsFirstBoot(
        /* [out] */ Boolean* result);

    CARAPI IsOnlyCoreApps(
        /* [out] */ Boolean* result);

    CARAPI SetPermissionEnforced(
        /* [in] */ const String& permission,
        /* [in] */ Boolean enforced);

    CARAPI IsPermissionEnforced(
        /* [in] */ const String& permission,
        /* [out] */ Boolean* result);

    CARAPI IsStorageLow(
        /* [out] */ Boolean* result);

    CARAPI SetApplicationHiddenSettingAsUser(
        /* [in] */ const String& packageName,
        /* [in] */ Boolean hide,
        /* [in] */ Int32 userId,
        /* [out] */ Boolean* result);

    CARAPI GetApplicationHiddenSettingAsUser(
        /* [in] */ const String& packageName,
        /* [in] */ Int32 userId,
        /* [out] */ Boolean* result);

    CARAPI GetPackageInstaller(
        /* [out] */ IIPackageInstaller** installer);

    CARAPI SetBlockUninstallForUser(
        /* [in] */ const String& packageName,
        /* [in] */ Boolean blockUninstall,
        /* [in] */ Int32 userId,
        /* [out] */ Boolean* result);

    CARAPI GetBlockUninstallForUser(
        /* [in] */ const String& packageName,
        /* [in] */ Int32 userId,
        /* [out] */ Boolean* result);

    CARAPI GetKeySetByAlias(
        /* [in] */ const String& packageName,
        /* [in] */ const String& alias,
        /* [out] */ IKeySet** set);

    CARAPI GetSigningKeySet(
        /* [in] */ const String& packageName,
        /* [out] */ IKeySet** set);

    CARAPI IsPackageSignedByKeySet(
        /* [in] */ const String& packageName,
        /* [in] */ IKeySet* ks,
        /* [out] */ Boolean* result);

    CARAPI IsPackageSignedByKeySetExactly(
        /* [in] */ const String& packageName,
        /* [in] */ IKeySet* ks,
        /* [out] */ Boolean* result);

    CARAPI SetComponentProtectedSetting(
        /* [in] */ IComponentName* componentName,
        /* [in] */ Boolean newState,
        /* [in] */ Int32 userId);

    CARAPI UpdateIconMapping(
        /* [in] */ const String& pkgName);

    CARAPI GetComposedIconInfo(
        /* [out] */ IComposedIconInfo** iconInfo);

    CARAPI ProcessThemeResources(
        /* [in] */ const String& themePkgName,
        /* [out] */ Int32* res);

    CARAPI SetPreLaunchCheckActivity(
        /* [in] */ IComponentName* componentName);

    CARAPI AddPreLaunchCheckPackage(
        /* [in] */ const String& packageName);

    CARAPI RemovePreLaunchCheckPackage(
        /* [in] */ const String& packageName);

    CARAPI ClearPreLaunchCheckPackages();

private:
    static const String DESCRIPTOR;
    static const Int32 TRANSACTION_getPackageInfo;
    static const Int32 TRANSACTION_getPackagesForUid;
    static const Int32 TRANSACTION_hasSystemFeature;
    static const Int32 TRANSACTION_isSafeMode;

    android::sp<android::IBinder> mPm;
};

} // namespace Pm
} // namespace Content
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_CONTENT_PM_IPACKAGEMANAGER_H__
