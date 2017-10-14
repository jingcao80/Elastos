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

#ifndef __ELASTOS_DROID_APP_APPLICATIONPACKAGEMANAGER_H__
#define __ELASTOS_DROID_APP_APPLICATIONPACKAGEMANAGER_H__

#include "elastos/droid/content/pm/PackageManager.h"
#include <elastos/utility/etl/HashMap.h>

using Elastos::Droid::Os::IUserManager;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::IComponentName;
using Elastos::Droid::Content::IIntentSender;
using Elastos::Droid::Content::Pm::IIPackageManager;
using Elastos::Droid::Content::Pm::IPackageManager;
using Elastos::Droid::Content::Pm::PackageManager;
using Elastos::Droid::Content::Pm::IPackageInfo;
using Elastos::Droid::Content::Pm::IPackageItemInfo;
using Elastos::Droid::Content::Pm::IPermissionInfo;
using Elastos::Droid::Content::Pm::IPermissionGroupInfo;
using Elastos::Droid::Content::Pm::IApplicationInfo;
using Elastos::Droid::Content::Pm::IActivityInfo;
using Elastos::Droid::Content::Pm::IUserInfo;
using Elastos::Droid::Content::Pm::IServiceInfo;
using Elastos::Droid::Content::Pm::IProviderInfo;
using Elastos::Droid::Content::Pm::IFeatureInfo;
using Elastos::Droid::Content::Pm::IResolveInfo;
using Elastos::Droid::Content::Pm::IComponentInfo;
using Elastos::Droid::Content::Pm::IInstrumentationInfo;
using Elastos::Droid::Content::Pm::IIPackageInstallObserver;
using Elastos::Droid::Content::Pm::IIPackageMoveObserver;
using Elastos::Droid::Content::Pm::IIPackageDeleteObserver;
using Elastos::Droid::Content::Pm::IIPackageDataObserver;
using Elastos::Droid::Content::Pm::IIPackageStatsObserver;
using Elastos::Droid::Content::Pm::IPackageInstaller;
using Elastos::Droid::Content::Pm::IManifestDigest;
using Elastos::Droid::Content::Pm::IContainerEncryptionParams;
using Elastos::Droid::Content::Pm::IVerificationParams;
using Elastos::Droid::Content::Pm::IVerifierDeviceIdentity;
using Elastos::Droid::Content::Pm::IKeySet;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Content::Res::IXmlResourceParser;
using Elastos::Droid::Graphics::IRect;
using Elastos::Droid::Graphics::Drawable::IDrawable;
using Elastos::Droid::Net::IUri;

using Elastos::Core::ICharSequence;
using Elastos::Utility::IList;
using Elastos::Utility::Etl::HashMap;

namespace Elastos {
namespace Droid {
namespace App {

/*package*/
class ApplicationPackageManager
    : public PackageManager
{
public:
    class ResourceName
        : public Object
    {
    public:
        ResourceName(
            /* [in] */ const String& packageName,
            /* [in] */ Int32 iconId);

        ResourceName(
            /* [in] */ IApplicationInfo* aInfo,
            /* [in] */ Int32 iconId);

        ResourceName(
            /* [in] */ IComponentInfo* cInfo,
            /* [in] */ Int32 iconId);

        ResourceName(
            /* [in] */ IResolveInfo* rInfo,
            /* [in] */ Int32 iconId);

        using Object::Equals;

        CARAPI_(Boolean) Equals(
            /* [in] */ ResourceName* o);

        CARAPI GetHashCode(
            /* [out] */ Int32* hash);

        CARAPI ToString(
            /* [out] */ String* str);

    public:
        String mPackageName;
        Int32 mIconId;
    };

public:
    ApplicationPackageManager(
        /* [in] */ IContextImpl* context,
        /* [in] */ IIPackageManager* pm);

    AutoPtr<IUserManager> GetUserManager();

    CARAPI GetPackageInfo(
        /* [in] */ const String& packageName,
        /* [in] */ Int32 flags,
        /* [out] */ IPackageInfo** info);

    CARAPI CurrentToCanonicalPackageNames(
        /* [in] */ const ArrayOf<String>& cuNames,
        /* [out, callee] */ ArrayOf<String>** caNames);

    CARAPI CanonicalToCurrentPackageNames(
        /* [in] */ const ArrayOf<String>& caNames,
        /* [out, callee] */ ArrayOf<String>** cuNames);

    CARAPI GetLaunchIntentForPackage(
        /* [in] */ const String& packageName,
        /* [out] */ IIntent** intent);

    CARAPI GetLeanbackLaunchIntentForPackage(
        /* [in] */ const String& packageName,
        /* [out] */ IIntent** intent);

    CARAPI GetPackageGids(
        /* [in] */ const String& packageName,
        /* [out, callee] */ ArrayOf<Int32>** pgids);

    CARAPI GetPermissionInfo(
        /* [in] */ const String& name,
        /* [in] */ Int32 flags,
        /* [out] */ IPermissionInfo** info);

    CARAPI GetPackageUid(
        /* [in] */ const String& packageName,
        /* [in] */ Int32 userHandle,
        /* [out] */ Int32* uid);

    CARAPI QueryPermissionsByGroup(
        /* [in] */ const String& group,
        /* [in] */ Int32 flags,
        /* [out] */ IList** permissions);

    CARAPI GetPermissionGroupInfo(
        /* [in] */ const String& name,
        /* [in] */ Int32 flags,
        /* [out] */ IPermissionGroupInfo** info);

    CARAPI GetAllPermissionGroups(
        /* [in] */ Int32 flags,
        /* [out] */ IList** groups);

    CARAPI GetApplicationInfo(
        /* [in] */ const String& packageName,
        /* [in] */ Int32 flags,
        /* [out] */ IApplicationInfo** info);

    CARAPI GetActivityInfo(
        /* [in] */ IComponentName* component,
        /* [in] */ Int32 flags,
        /* [out] */ IActivityInfo** info);

    CARAPI GetReceiverInfo(
        /* [in] */ IComponentName* component,
        /* [in] */ Int32 flags,
        /* [out] */ IActivityInfo** info);

    CARAPI GetServiceInfo(
        /* [in] */ IComponentName* component,
        /* [in] */ Int32 flags,
        /* [out] */ IServiceInfo** info);

    CARAPI GetProviderInfo(
        /* [in] */ IComponentName* component,
        /* [in] */ Int32 flags,
        /* [out] */ IProviderInfo** info);

    CARAPI GetSystemSharedLibraryNames(
        /* [out, callee] */ ArrayOf<String>** names);

    CARAPI GetSystemAvailableFeatures(
        /* [out, callee] */ ArrayOf<IFeatureInfo*>** features);

    CARAPI HasSystemFeature(
        /* [in] */ const String& name,
        /* [out] */ Boolean* result);

    CARAPI CheckPermission(
        /* [in] */ const String& permName,
        /* [in] */ const String& pkgName,
        /* [out] */ Int32* result);

    CARAPI AddPermission(
        /* [in] */ IPermissionInfo* info,
        /* [out] */ Boolean* result);

    CARAPI AddPermissionAsync(
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

    CARAPI CheckSignatures(
        /* [in] */ const String& pkg1,
        /* [in] */ const String& pkg2,
        /* [out] */ Int32* result);

    CARAPI CheckSignatures(
        /* [in] */ Int32 uid1,
        /* [in] */ Int32 uid2,
        /* [out] */ Int32* result);

    CARAPI GetPackagesForUid(
        /* [in] */ Int32 uid,
        /* [out, callee] */ ArrayOf<String>** packages);

    CARAPI GetNameForUid(
        /* [in] */ Int32 uid,
        /* [out] */ String* name);

    CARAPI GetUidForSharedUser(
        /* [in] */ const String& sharedUserName,
        /* [out] */ Int32* useruid);

    CARAPI GetInstalledPackages(
        /* [in] */ Int32 flags,
        /* [out] */ IList** infos);

    CARAPI GetInstalledPackages(
        /* [in] */ Int32 flags,
        /* [in] */ Int32 userId,
        /* [out] */ IList** infos);

    CARAPI GetPackagesHoldingPermissions(
        /* [in] */ ArrayOf<String>* permissions,
        /* [in] */ Int32 flags,
        /* [out] */ IList** result);

    CARAPI GetInstalledApplications(
        /* [in] */ Int32 flags,
        /* [out] */ IList** apps);

    CARAPI ResolveActivity(
        /* [in] */ IIntent* intent,
        /* [in] */ Int32 flags,
        /* [out] */ IResolveInfo** resolve);

    CARAPI ResolveActivityAsUser(
        /* [in] */ IIntent* intent,
        /* [in] */ Int32 flags,
        /* [in] */ Int32 userId,
        /* [out] */ IResolveInfo** resolve);

    CARAPI QueryIntentActivities(
        /* [in] */ IIntent* intent,
        /* [in] */ Int32 flags,
        /* [out] */ IList** resolves);

    CARAPI QueryIntentActivitiesAsUser(
        /* [in] */ IIntent* intent,
        /* [in] */ Int32 flags,
        /* [in] */ Int32 userId,
        /* [out] */ IList** resolves);

    CARAPI QueryIntentActivityOptions(
        /* [in] */ IComponentName* caller,
        /* [in] */ ArrayOf<IIntent*>* specifics,
        /* [in] */ IIntent* intent,
        /* [in] */ Int32 flags,
        /* [out] */ IList** resolves);

    CARAPI QueryBroadcastReceivers(
        /* [in] */ IIntent* intent,
        /* [in] */ Int32 flags,
        /* [in] */ Int32 userId,
        /* [out] */ IList** resolves);

    CARAPI QueryBroadcastReceivers(
        /* [in] */ IIntent* intent,
        /* [in] */ Int32 flags,
        /* [out] */ IList** resolves);

    CARAPI ResolveService(
        /* [in] */ IIntent* intent,
        /* [in] */ Int32 flags,
        /* [out] */ IResolveInfo** resolve);

    CARAPI QueryIntentServicesAsUser(
        /* [in] */ IIntent* intent,
        /* [in] */ Int32 flags,
        /* [in] */ Int32 userId,
        /* [out] */ IList** resolves);

    CARAPI QueryIntentServices(
        /* [in] */ IIntent* intent,
        /* [in] */ Int32 flags,
        /* [out] */ IList** resolves);

    CARAPI QueryIntentContentProvidersAsUser(
        /* [in] */ IIntent* intent,
        /* [in] */ Int32 flags,
        /* [in] */ Int32 userId,
        /* [out] */ IList** resolveInfos); //List<ResolveInfo>

    CARAPI QueryIntentContentProviders(
        /* [in] */ IIntent* intent,
        /* [in] */ Int32 flags,
        /* [out] */ IList** resolveInfos); //List<ResolveInfo>

    CARAPI ResolveContentProvider(
        /* [in] */ const String& name,
        /* [in] */ Int32 flags,
        /* [out] */ IProviderInfo** provider);

    CARAPI ResolveContentProvider(
        /* [in] */ const String& name,
        /* [in] */ Int32 flags,
        /* [in] */ Int32 userId,
        /* [out] */ IProviderInfo** provider);

    CARAPI QueryContentProviders(
        /* [in] */ const String& processName,
        /* [in] */ Int32 uid,
        /* [in] */ Int32 flags,
        /* [out] */ IList** providers);

    CARAPI GetInstrumentationInfo(
        /* [in] */ IComponentName* className,
        /* [in] */ Int32 flags,
        /* [out] */ IInstrumentationInfo** instrumentation);

    CARAPI QueryInstrumentation(
        /* [in] */ const String& targetPackage,
        /* [in] */ Int32 flags,
        /* [out] */ IList** instrumentations);

    CARAPI GetDrawable(
        /* [in] */ const String& packageName,
        /* [in] */ Int32 resid,
        /* [in] */ IApplicationInfo* appInfo,
        /* [out] */ IDrawable** drawable);

    CARAPI GetActivityIcon(
        /* [in] */ IComponentName* activityName,
        /* [out] */ IDrawable** icon);

    CARAPI GetActivityIcon(
        /* [in] */ IIntent* intent,
        /* [out] */ IDrawable** icon);

    CARAPI GetDefaultActivityIcon(
        /* [out] */ IDrawable** icon);

    CARAPI GetApplicationIcon(
        /* [in] */ IApplicationInfo* info,
        /* [out] */ IDrawable** icon);

    CARAPI GetApplicationIcon(
        /* [in] */ const String& packageName,
        /* [out] */ IDrawable** icon);

    CARAPI GetActivityBanner(
        /* [in] */ IComponentName* activityName,
        /* [out] */ IDrawable** icon);

    CARAPI GetActivityBanner(
        /* [in] */ IIntent* intent,
        /* [out] */ IDrawable** icon);

    CARAPI GetApplicationBanner(
        /* [in] */ IApplicationInfo* info,
        /* [out] */ IDrawable** icon);

    CARAPI GetApplicationBanner(
        /* [in] */ const String& packageName,
        /* [out] */ IDrawable** icon);

    CARAPI GetActivityLogo(
        /* [in] */ IComponentName* activityName,
        /* [out] */ IDrawable** logo);

    CARAPI GetActivityLogo(
        /* [in] */ IIntent* intent,
        /* [out] */ IDrawable** logo);

    CARAPI GetApplicationLogo(
        /* [in] */ IApplicationInfo* info,
        /* [out] */ IDrawable** logo);

    CARAPI GetApplicationLogo(
        /* [in] */ const String& packageName,
        /* [out] */ IDrawable** logo);

    CARAPI GetUserBadgedIcon(
        /* [in] */ IDrawable* icon,
        /* [in] */ IUserHandle* user,
        /* [out] */ IDrawable** drawable);

    CARAPI GetUserBadgedDrawableForDensity(
        /* [in] */ IDrawable* drawable,
        /* [in] */ IUserHandle* user,
        /* [in] */ IRect* badgeLocation,
        /* [in] */ Int32 badgeDensity,
        /* [out] */ IDrawable** result);

    CARAPI GetUserBadgeForDensity(
        /* [in] */ IUserHandle* user,
        /* [in] */ Int32 density,
        /* [out] */ IDrawable** drawable);

    CARAPI GetUserBadgedLabel(
        /* [in] */ ICharSequence* label,
        /* [in] */ IUserHandle* user,
        /* [out] */ ICharSequence** csq);

    CARAPI GetResourcesForActivity(
        /* [in] */ IComponentName* activityName,
        /* [out] */ IResources** res);

    CARAPI GetResourcesForApplication(
        /* [in] */ IApplicationInfo* app,
        /* [out] */ IResources** res);

    CARAPI GetResourcesForApplication(
        /* [in] */ const String& appPackageName,
        /* [out] */ IResources** res);

    CARAPI GetResourcesForApplicationAsUser(
        /* [in] */ const String& appPackageName,
        /* [in] */ Int32 userId,
        /* [out] */ IResources** res);

    CARAPI GetThemedResourcesForApplication(
        /* [in] */ IApplicationInfo* app,
        /* [in] */ const String& themePkgName,
        /* [out] */ IResources** res);

    CARAPI GetThemedResourcesForApplication(
        /* [in] */ const String& appPackageName,
        /* [in] */ const String& themePkgName,
        /* [out] */ IResources** res);

    CARAPI GetThemedResourcesForApplicationAsUser(
        /* [in] */ const String& appPackageName,
        /* [in] */ const String& themePkgName,
        /* [in] */ Int32 userId,
        /* [out] */ IResources** res);

    CARAPI IsSafeMode(
        /* [out] */ Boolean* result);

    static CARAPI_(void) ConfigurationChanged();

    static CARAPI_(void) HandlePackageBroadcast(
        /* [in] */ Int32 cmd,
        /* [in] */ ArrayOf<String>* pkgList,
        /* [in] */ Boolean hasPkgInfo);

    CARAPI GetText(
        /* [in] */ const String& packageName,
        /* [in] */ Int32 resid,
        /* [in] */ IApplicationInfo* appInfo,
        /* [out] */ ICharSequence** text);

    CARAPI GetXml(
        /* [in] */ const String& packageName,
        /* [in] */ Int32 resid,
        /* [in] */ IApplicationInfo* appInfo,
        /* [out] */ IXmlResourceParser** parser);

    CARAPI GetApplicationLabel(
        /* [in] */ IApplicationInfo* info,
        /* [out] */ ICharSequence** label);

    CARAPI InstallPackage(
        /* [in] */ IUri* packageURI,
        /* [in] */ IIPackageInstallObserver* observer,
        /* [in] */ Int32 flags,
        /* [in] */ const String& installerPackageName);

    CARAPI InstallPackageWithVerification(
        /* [in] */ IUri* packageURI,
        /* [in] */ IIPackageInstallObserver* observer,
        /* [in] */ Int32 flags,
        /* [in] */ const String& installerPackageName,
        /* [in] */ IUri* verificationURI,
        /* [in] */ IManifestDigest* manifestDigest,
        /* [in] */ IContainerEncryptionParams* encryptionParams);

    CARAPI InstallPackageWithVerificationAndEncryption(
        /* [in] */ IUri* packageURI,
        /* [in] */ IIPackageInstallObserver* observer,
        /* [in] */ Int32 flags,
        /* [in] */ const String& installerPackageName,
        /* [in] */ IVerificationParams* verificationParams,
        /* [in] */ IContainerEncryptionParams* encryptionParams);

    CARAPI InstallExistingPackage(
        /* [in] */ const String& packageName,
        /* [out] */ Int32* result);

    CARAPI VerifyPendingInstall(
        /* [in] */ Int32 id,
        /* [in] */ Int32 verificationCode);

    CARAPI ExtendVerificationTimeout(
        /* [in] */ Int32 id,
        /* [in] */ Int32 verificationCodeAtTimeout,
        /* [in] */ Int64 millisecondsToDelay);

    CARAPI SetInstallerPackageName(
        /* [in] */ const String& targetPackage,
        /* [in] */ const String& installerPackageName);

    CARAPI MovePackage(
        /* [in] */ const String& packageName,
        /* [in] */ IIPackageMoveObserver* observer,
        /* [in] */ Int32 flags);

    CARAPI GetInstallerPackageName(
        /* [in] */ const String& packageName,
        /* [out] */ String* name);

    CARAPI DeletePackage(
        /* [in] */ const String& packageName,
        /* [in] */ IIPackageDeleteObserver* observer,
        /* [in] */ Int32 flags);

    CARAPI ClearApplicationUserData(
        /* [in] */ const String& packageName,
        /* [in] */ IIPackageDataObserver* observer);

    CARAPI DeleteApplicationCacheFiles(
        /* [in] */ const String& packageName,
        /* [in] */ IIPackageDataObserver* observer);

    CARAPI FreeStorageAndNotify(
        /* [in] */ Int64 freeStorageSize,
        /* [in] */ IIPackageDataObserver* observer);

    CARAPI FreeStorage(
        /* [in] */ Int64 freeStorageSize,
        /* [in] */ IIntentSender* pi);

    CARAPI GetPackageSizeInfo(
        /* [in] */ const String& packageName,
        /* [in] */ Int32 userHandle,
        /* [in] */ IIPackageStatsObserver* observer);

    CARAPI AddPackageToPreferred(
        /* [in] */ const String& packageName);

    CARAPI RemovePackageFromPreferred(
        /* [in] */ const String& packageName);

    CARAPI GetPreferredPackages(
        /* [in] */ Int32 flags,
        /* [out] */ IList** packages);

    CARAPI AddPreferredActivity(
        /* [in] */ IIntentFilter* filter,
        /* [in] */ Int32 match,
        /* [in] */ ArrayOf<IComponentName*>* set,
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
        /* [in] */ IComponentName* activity);

    CARAPI ReplacePreferredActivityAsUser(
        /* [in] */ IIntentFilter* filter,
        /* [in] */ Int32 match,
        /* [in] */ ArrayOf<IComponentName*>* set,
        /* [in] */ IComponentName* activity,
        /* [in] */ Int32 userId);

    CARAPI ClearPackagePreferredActivities(
        /* [in] */ const String& packageName);

    CARAPI GetPreferredActivities(
        /* [in] */ IList* outFilters,
        /* [in] */ IList* outActivities,
        /* [in] */ const String& packageName,
        /* [out] */ Int32* num);

    CARAPI GetHomeActivities(
        /* [in] */ IList* /*<ResolveInfo>*/ outActivities,
        /* [out] */ IComponentName** cn);

    CARAPI SetComponentEnabledSetting(
        /* [in] */ IComponentName* componentName,
        /* [in] */ Int32 newState,
        /* [in] */ Int32 flags);

    CARAPI GetComponentEnabledSetting(
        /* [in] */ IComponentName* componentName,
        /* [out] */ Int32* value);

    CARAPI SetApplicationEnabledSetting(
        /* [in] */ const String& packageName,
        /* [in] */ Int32 newState,
        /* [in] */ Int32 flags);

    CARAPI GetApplicationEnabledSetting(
        /* [in] */ const String& packageName,
        /* [out] */ Int32* value);

    //@Override
    CARAPI SetApplicationHiddenSettingAsUser(
        /* [in] */ const String& packageName,
        /* [in] */ Boolean hidden,
        /* [in] */ IUserHandle* user,
        /* [out] */ Boolean* result);

    //@Override
    CARAPI GetApplicationHiddenSettingAsUser(
        /* [in] */ const String& packageName,
        /* [in] */ IUserHandle* user,
        /* [out] */ Boolean* result);

    /** @hide */
    //@Override
    CARAPI GetKeySetByAlias(
        /* [in] */ const String& packageName,
        /* [in] */ const String& alias,
        /* [out] */ IKeySet** keySet);

    /** @hide */
    //@Override
    CARAPI GetSigningKeySet(
        /* [in] */ const String& packageName,
        /* [out] */ IKeySet** keySet);

    /** @hide */
    //@Override
    CARAPI IsSignedBy(
        /* [in] */ const String& packageName,
        /* [in] */ IKeySet* ks,
        /* [out] */ Boolean* result);

    /** @hide */
    //@Override
    CARAPI IsSignedByExactly(
        /* [in] */ const String& packageName,
        /* [in] */ IKeySet* ks,
        /* [out] */ Boolean* result);

    CARAPI GetVerifierDeviceIdentity(
        /* [out] */ IVerifierDeviceIdentity** identity);

    CARAPI SetComponentProtectedSetting(
        /* [in] */ IComponentName* componentName,
        /* [in] */ Boolean newState);

    //@Override
    CARAPI GetPackageInstaller(
        /* [out] */ IPackageInstaller** installer);

    //@Override
    CARAPI IsPackageAvailable(
        /* [in] */ const String& packageName,
        /* [out] */ Boolean* result);

    /**
     * @hide
     */
    CARAPI AddCrossProfileIntentFilter(
        /* [in] */ IIntentFilter* filter,
        /* [in] */ Int32 sourceUserId,
        /* [in] */ Int32 targetUserId,
        /* [in] */ Int32 flags);

    /**
     * @hide
     */
    CARAPI ClearCrossProfileIntentFilters(
        /* [in] */ Int32 sourceUserId);

    CARAPI LoadItemIcon(
        /* [in] */ IPackageItemInfo* itemInfo,
        /* [in] */ IApplicationInfo* appInfo,
        /* [out] */ IDrawable** drawable);

    CARAPI UpdateIconMaps(
        /* [in] */ const String& pkgName);

    CARAPI ProcessThemeResources(
        /* [in] */ const String& themePkgName,
        /* [out] */ Int32* result);

private:
    CARAPI_(AutoPtr<IDrawable>) GetCachedIcon(
        /* [in] */ ResourceName* name);

    static CARAPI_(void) MaybeAdjustApplicationInfo(
        /* [in] */ IApplicationInfo* info);

    CARAPI_(void) PutCachedIcon(
        /* [in] */ ResourceName* name,
        /* [in] */ IDrawable* dr);

    CARAPI_(AutoPtr<ICharSequence>) GetCachedString(
        /* [in] */ ResourceName* name);

    CARAPI_(void) PutCachedString(
        /* [in] */ ResourceName* name,
        /* [in] */ ICharSequence* cs);

    AutoPtr<IDrawable> GetBadgedDrawable(
        /* [in] */ IDrawable* drawable,
        /* [in] */ IDrawable* badgeDrawable,
        /* [in] */ IRect* badgeLocation,
        /* [in] */ Boolean tryBadgeInPlace);

    Int32 GetBadgeResIdForUser(
        /* [in] */ Int32 userHandle);

    AutoPtr<IUserInfo> GetUserIfProfile(
    /* [in] */ Int32 userHandle);

    CARAPI InstallCommon(
        /* [in] */ IUri* packageURI,
        /* [in] */ IPackageInstallObserver* observer,
        /* [in] */ Int32 flags,
        /* [in] */ const String& installerPackageName,
        /* [in] */ IVerificationParams* verificationParams,
        /* [in] */ IContainerEncryptionParams* encryptionParams);

private:
    static const String TAG;
    static const Boolean DEBUG;
    static const Boolean DEBUG_ICONS;

    // Default flags to use with PackageManager when no flags are given.
    static Int32 sDefaultFlags;

    Object mLock;

    // @GuardedBy("mLock")
    AutoPtr<IUserManager> mUserManager;
    // @GuardedBy("mLock")
    AutoPtr<IPackageInstaller> mInstaller;

    IContext* mContext;
    AutoPtr<IIPackageManager> mPM;

    static Object sSync;
    /*WeakReference<Drawable.ConstantState>*/
    static HashMap<AutoPtr<ResourceName>, AutoPtr<IWeakReference> > sIconCache;
    /*WeakReference<CharSequence>*/
    static HashMap<AutoPtr<ResourceName>, AutoPtr<IWeakReference> > sStringCache;

    Int32 mCachedSafeMode;
};

} // namespace App
} // namespace Droid
} // namespace Elastos


DEFINE_OBJECT_HASH_FUNC_FOR(Elastos::Droid::App::ApplicationPackageManager::ResourceName)

#endif //__ELASTOS_DROID_APP_APPLICATIONPACKAGEMANAGER_H__
