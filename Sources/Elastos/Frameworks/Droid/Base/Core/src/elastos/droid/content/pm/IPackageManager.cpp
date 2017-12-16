
#include "elastos/droid/content/pm/IPackageManager.h"
#include "elastos/droid/os/AndroidParcelUtils.h"

using Elastos::Droid::Os::AndroidParcelUtils;

namespace Elastos {
namespace Droid {
namespace Content {
namespace Pm {

const String IPackageManagerProxy::DESCRIPTOR("android.content.pm.IPackageManager");

const Int32 IPackageManagerProxy::TRANSACTION_getPackageInfo = android::IBinder::FIRST_CALL_TRANSACTION + 2;
const Int32 IPackageManagerProxy::TRANSACTION_getPackagesForUid = android::IBinder::FIRST_CALL_TRANSACTION + 33;
const Int32 IPackageManagerProxy::TRANSACTION_hasSystemFeature = android::IBinder::FIRST_CALL_TRANSACTION + 104;
const Int32 IPackageManagerProxy::TRANSACTION_isSafeMode = android::IBinder::FIRST_CALL_TRANSACTION + 106;

CAR_INTERFACE_IMPL(IPackageManagerProxy, Object, IIPackageManager);

IPackageManagerProxy::IPackageManagerProxy(
    /* [in] */ android::sp<android::IBinder>& pm)
    : mPm(pm)
{}

ECode IPackageManagerProxy::IsPackageAvailable(
    /* [in] */ const String& packageName,
    /* [in] */ Int32 userId,
    /* [out] */ Boolean* result)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IPackageManagerProxy::GetPackageInfo(
    /* [in] */ const String& packageName,
    /* [in] */ Int32 flags,
    /* [in] */ Int32 userId,
    /* [out] */ IPackageInfo** info)
{
    VALIDATE_NOT_NULL(info);

    android::Parcel data, reply;

    data.writeInterfaceToken(android::String16(DESCRIPTOR.string()));
    AndroidParcelUtils::WriteString(data, packageName);
    AndroidParcelUtils::WriteInt32(data, flags);
    AndroidParcelUtils::WriteInt32(data, userId);
    mPm->transact(TRANSACTION_getPackageInfo, data, &reply, 0);
    ECode ec = reply.readExceptionCode() == 0 ? NOERROR : E_REMOTE_EXCEPTION;
    if (AndroidParcelUtils::ReadInt32(reply) != 0) {
        AutoPtr<IPackageInfo> pi = AndroidParcelUtils::ReadPackageInfo(reply);
        *info = pi;
        REFCOUNT_ADD(*info);
    }
    else {
        *info = NULL;
    }
    return ec;
}

ECode IPackageManagerProxy::GetPackageUid(
    /* [in] */ const String& packageName,
    /* [in] */ Int32 userId,
    /* [out] */ Int32* uid)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IPackageManagerProxy::GetPackageGids(
    /* [in] */ const String& packageName,
    /* [out, callee] */ ArrayOf<Int32>** gids)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IPackageManagerProxy::CurrentToCanonicalPackageNames(
    /* [in] */ const ArrayOf<String>& cuNames,
    /* [out, callee] */ ArrayOf<String>** caNames)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IPackageManagerProxy::CanonicalToCurrentPackageNames(
    /* [in] */ const ArrayOf<String>& caNames,
    /* [out, callee] */ ArrayOf<String>** cuNames)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IPackageManagerProxy::GetPermissionInfo(
    /* [in] */ const String& name,
    /* [in] */ Int32 flags,
    /* [out] */ IPermissionInfo** info)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IPackageManagerProxy::QueryPermissionsByGroup(
    /* [in] */ const String& group,
    /* [in] */ Int32 flags,
    /* [out] */ IList** infos)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IPackageManagerProxy::GetPermissionGroupInfo(
    /* [in] */ const String& name,
    /* [in] */ Int32 flags,
    /* [out] */ IPermissionGroupInfo** info)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IPackageManagerProxy::GetAllPermissionGroups(
    /* [in] */ Int32 flags,
    /* [out] */ IList** infos)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IPackageManagerProxy::GetApplicationInfo(
    /* [in] */ const String& packageName,
    /* [in] */ Int32 flags,
    /* [in] */ Int32 userId,
    /* [out] */ IApplicationInfo** info)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IPackageManagerProxy::GetActivityInfo(
    /* [in] */ IComponentName* className,
    /* [in] */ Int32 flags,
    /* [in] */ Int32 userId,
    /* [out] */ IActivityInfo** info)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IPackageManagerProxy::ActivitySupportsIntent(
    /* [in] */ IComponentName* className,
    /* [in] */ IIntent* intent,
    /* [in] */ const String& resolvedType,
    /* [out] */ Boolean* result)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IPackageManagerProxy::GetReceiverInfo(
    /* [in] */ IComponentName* className,
    /* [in] */ Int32 flags,
    /* [in] */ Int32 userId,
    /* [out] */ IActivityInfo** info)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IPackageManagerProxy::GetServiceInfo(
    /* [in] */ IComponentName* className,
    /* [in] */ Int32 flags,
    /* [in] */ Int32 userId,
    /* [out] */ IServiceInfo** info)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IPackageManagerProxy::GetProviderInfo(
    /* [in] */ IComponentName* className,
    /* [in] */ Int32 flags,
    /* [in] */ Int32 userId,
    /* [out] */ IProviderInfo** info)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IPackageManagerProxy::CheckPermission(
    /* [in] */ const String& permName,
    /* [in] */ const String& pkgName,
    /* [out] */ Int32* result)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IPackageManagerProxy::CheckUidPermission(
    /* [in] */ const String& permName,
    /* [in] */ Int32 uid,
    /* [out] */ Int32* result)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IPackageManagerProxy::AddPermission(
    /* [in] */ IPermissionInfo* info,
    /* [out] */ Boolean* result)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IPackageManagerProxy::RemovePermission(
    /* [in] */ const String& name)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IPackageManagerProxy::GrantPermission(
    /* [in] */ const String& packageName,
    /* [in] */ const String& permissionName)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IPackageManagerProxy::RevokePermission(
    /* [in] */ const String& packageName,
    /* [in] */ const String& permissionName)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IPackageManagerProxy::IsProtectedBroadcast(
    /* [in] */ const String& actionName,
    /* [out] */ Boolean* result)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IPackageManagerProxy::CheckSignatures(
    /* [in] */ const String& pkg1,
    /* [in] */ const String& pkg2,
    /* [out] */ Int32* result)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IPackageManagerProxy::CheckUidSignatures(
    /* [in] */ Int32 uid1,
    /* [in] */ Int32 uid2,
    /* [out] */ Int32* result)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IPackageManagerProxy::GetPackagesForUid(
    /* [in] */ Int32 uid,
    /* [out, callee] */ ArrayOf<String>** pkgs)
{
    VALIDATE_NOT_NULL(pkgs);

    android::Parcel data, reply;

    data.writeInterfaceToken(android::String16(DESCRIPTOR.string()));
    AndroidParcelUtils::WriteInt32(data, uid);
    mPm->transact(TRANSACTION_getPackagesForUid, data, &reply, 0);
    ECode ec = reply.readExceptionCode() == 0 ? NOERROR : E_REMOTE_EXCEPTION;
    AutoPtr< ArrayOf<String> > res = AndroidParcelUtils::CreateStringArray(reply);
    *pkgs = res;
    REFCOUNT_ADD(*pkgs);
    return ec;
}

ECode IPackageManagerProxy::GetNameForUid(
    /* [in] */ Int32 uid,
    /* [out] */ String* name)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IPackageManagerProxy::GetUidForSharedUser(
    /* [in] */ const String& sharedUserName,
    /* [out] */ Int32* uid)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IPackageManagerProxy::GetFlagsForUid(
    /* [in] */ Int32 uid,
    /* [out] */ Int32* result)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IPackageManagerProxy::IsUidPrivileged(
    /* [in] */ Int32 uid,
    /* [out] */ Boolean* result)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IPackageManagerProxy::GetAppOpPermissionPackages(
    /* [in] */ const String& permissionName,
    /* [out, callee] */ ArrayOf<String>** result)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IPackageManagerProxy::ResolveIntent(
    /* [in] */ IIntent* intent,
    /* [in] */ const String& resolvedType,
    /* [in] */ Int32 flags,
    /* [in] */ Int32 userId,
    /* [out] */ IResolveInfo** info)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IPackageManagerProxy::CanForwardTo(
    /* [in] */ IIntent* intent,
    /* [in] */ const String& resolvedType,
    /* [in] */ Int32 sourceUserId,
    /* [in] */ Int32 targetUserId,
    /* [out] */ Boolean* result)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IPackageManagerProxy::QueryIntentActivities(
    /* [in] */ IIntent* intent,
    /* [in] */ const String& resolvedType,
    /* [in] */ Int32 flags,
    /* [in] */ Int32 userId,
    /* [out] */ IList** infos)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IPackageManagerProxy::QueryIntentActivityOptions(
    /* [in] */ IComponentName* caller,
    /* [in] */ ArrayOf<IIntent*>* specifics,
    /* [in] */ ArrayOf<String>* specificTypes,
    /* [in] */ IIntent* intent,
    /* [in] */ const String& resolvedType,
    /* [in] */ Int32 flags,
    /* [in] */ Int32 userId,
    /* [out] */ IList** infos)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IPackageManagerProxy::QueryIntentReceivers(
    /* [in] */ IIntent* intent,
    /* [in] */ const String& resolvedType,
    /* [in] */ Int32 flags,
    /* [in] */ Int32 userId,
    /* [out] */ IList** receivers)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IPackageManagerProxy::ResolveService(
    /* [in] */ IIntent* intent,
    /* [in] */ const String& resolvedType,
    /* [in] */ Int32 flags,
    /* [in] */ Int32 userId,
    /* [out] */ IResolveInfo** resolveInfo)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IPackageManagerProxy::QueryIntentServices(
    /* [in] */ IIntent* intent,
    /* [in] */ const String& resolvedType,
    /* [in] */ Int32 flags,
    /* [in] */ Int32 userId,
    /* [out] */ IList** infos)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IPackageManagerProxy::QueryIntentContentProviders(
    /* [in] */ IIntent* intent,
    /* [in] */ const String& resolvedType,
    /* [in] */ Int32 flags,
    /* [in] */ Int32 userId,
    /* [out] */ IList** infos)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IPackageManagerProxy::GetInstalledPackages(
    /* [in] */ Int32 flags,
    /* [in] */ Int32 userId,
    /* [out] */ IParceledListSlice** slice)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IPackageManagerProxy::GetPackagesHoldingPermissions(
    /* [in] */ ArrayOf<String>* permissions,
    /* [in] */ Int32 flags,
    /* [in] */ Int32 userId,
    /* [out] */ IParceledListSlice** slice)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IPackageManagerProxy::GetInstalledApplications(
    /* [in] */ Int32 flags,
    /* [in] */ Int32 userId,
    /* [out] */ IParceledListSlice** slice)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IPackageManagerProxy::GetPersistentApplications(
    /* [in] */ Int32 flags,
    /* [out] */ IList** infos)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IPackageManagerProxy::ResolveContentProvider(
    /* [in] */ const String& name,
    /* [in] */ Int32 flags,
    /* [in] */ Int32 userId,
    /* [out] */ IProviderInfo** info)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IPackageManagerProxy::QuerySyncProviders(
    /* [in, out] */ IList* outNames,
    /* [in, out] */ IList* outInfo)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IPackageManagerProxy::QueryContentProviders(
    /* [in] */ const String& processName,
    /* [in] */ Int32 uid,
    /* [in] */ Int32 flags,
    /* [out] */ IList** providers)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IPackageManagerProxy::GetInstrumentationInfo(
    /* [in] */ IComponentName* className,
    /* [in] */ Int32 flags,
    /* [out] */ IInstrumentationInfo** info)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IPackageManagerProxy::QueryInstrumentation(
    /* [in] */ const String& targetPackage,
    /* [in] */ Int32 flags,
    /* [out] */ IList** infos)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IPackageManagerProxy::InstallPackage(
    /* [in] */ const String& originPath,
    /* [in] */ IIPackageInstallObserver2* observer,
    /* [in] */ Int32 flags,
    /* [in] */ const String& installerPackageName,
    /* [in] */ IVerificationParams* verificationParams,
    /* [in] */ const String& packageAbiOverride)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IPackageManagerProxy::InstallPackageAsUser(
    /* [in] */ const String& originPath,
    /* [in] */ IIPackageInstallObserver2* observer,
    /* [in] */ Int32 flags,
    /* [in] */ const String& installerPackageName,
    /* [in] */ IVerificationParams* verificationParams,
    /* [in] */ const String& packageAbiOverride,
    /* [in] */ Int32 userId)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IPackageManagerProxy::FinishPackageInstall(
    /* [in] */ Int32 token)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IPackageManagerProxy::SetInstallerPackageName(
    /* [in] */ const String& targetPackage,
    /* [in] */ const String& installerPackageName)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IPackageManagerProxy::DeletePackageAsUser(
    /* [in] */ const String& packageName,
    /* [in] */ IIPackageDeleteObserver* observer,
    /* [in] */ Int32 userId,
    /* [in] */ Int32 flags)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IPackageManagerProxy::DeletePackage(
    /* [in] */ const String& packageName,
    /* [in] */ IIPackageDeleteObserver2* observer,
    /* [in] */ Int32 userId,
    /* [in] */ Int32 flags)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IPackageManagerProxy::GetInstallerPackageName(
    /* [in] */ const String& packageName,
    /* [out] */ String* pkgName)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IPackageManagerProxy::AddPackageToPreferred(
    /* [in] */ const String& packageName)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IPackageManagerProxy::RemovePackageFromPreferred(
    /* [in] */ const String& packageName)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IPackageManagerProxy::GetPreferredPackages(
    /* [in] */ Int32 flags,
    /* [out] */ IList** infos)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IPackageManagerProxy::ResetPreferredActivities(
    /* [in] */ Int32 userId)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IPackageManagerProxy::GetLastChosenActivity(
    /* [in] */ IIntent* intent,
    /* [in] */ const String& resolvedType,
    /* [in] */ Int32 flags,
    /* [out] */ IResolveInfo** info)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IPackageManagerProxy::SetLastChosenActivity(
    /* [in] */ IIntent* intent,
    /* [in] */ const String& resolvedType,
    /* [in] */ Int32 flags,
    /* [in] */ IIntentFilter* filter,
    /* [in] */ Int32 match,
    /* [in] */ IComponentName* activity)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IPackageManagerProxy::AddPreferredActivity(
    /* [in] */ IIntentFilter* filter,
    /* [in] */ Int32 match,
    /* [in] */ ArrayOf<IComponentName*>* set,
    /* [in] */ IComponentName* activity,
    /* [in] */ Int32 userId)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IPackageManagerProxy::ReplacePreferredActivity(
    /* [in] */ IIntentFilter* filter,
    /* [in] */ Int32 match,
    /* [in] */ ArrayOf<IComponentName*>* set,
    /* [in] */ IComponentName* activity,
    /* [in] */ Int32 userId)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IPackageManagerProxy::ClearPackagePreferredActivities(
    /* [in] */ const String& packageName)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IPackageManagerProxy::GetPreferredActivities(
    /* [in, out] */ IList* outFilters,
    /* [in, out] */ IList* outActivities,
    /* [in] */ const String& packageName,
    /* [out] */ Int32* value)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IPackageManagerProxy::AddPersistentPreferredActivity(
    /* [in] */ IIntentFilter* filter,
    /* [in] */ IComponentName* activity,
    /* [in] */ Int32 userId)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IPackageManagerProxy::ClearPackagePersistentPreferredActivities(
    /* [in] */ const String& packageName,
    /* [in] */ Int32 userId)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IPackageManagerProxy::AddCrossProfileIntentFilter(
    /* [in] */ IIntentFilter* intentFilter,
    /* [in] */ const String& ownerPackage,
    /* [in] */ Int32 ownerUserId,
    /* [in] */ Int32 sourceUserId,
    /* [in] */ Int32 targetUserId,
    /* [in] */ Int32 flags)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IPackageManagerProxy::ClearCrossProfileIntentFilters(
    /* [in] */ Int32 sourceUserId,
    /* [in] */ const String& ownerPackage,
    /* [in] */ Int32 ownerUserId)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IPackageManagerProxy::GetHomeActivities(
    /* [in] */ IList* outHomeCandidates,
    /* [out] */ IComponentName** name)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IPackageManagerProxy::SetComponentEnabledSetting(
    /* [in] */ IComponentName* componentName,
    /* [in] */ Int32 newState,
    /* [in] */ Int32 flags,
    /* [in] */ Int32 userId)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IPackageManagerProxy::GetComponentEnabledSetting(
    /* [in] */ IComponentName* componentName,
    /* [in] */ Int32 userId,
    /* [out] */ Int32* value)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IPackageManagerProxy::SetApplicationEnabledSetting(
    /* [in] */ const String& packageName,
    /* [in] */ Int32 newState,
    /* [in] */ Int32 flags,
    /* [in] */ Int32 userId,
    /* [in] */ const String& callingPackage)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IPackageManagerProxy::GetApplicationEnabledSetting(
    /* [in] */ const String& packageName,
    /* [in] */ Int32 userId,
    /* [out] */ Int32* value)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IPackageManagerProxy::SetPackageStoppedState(
    /* [in] */ const String& packageName,
    /* [in] */ Boolean stopped,
    /* [in] */ Int32 userId)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IPackageManagerProxy::FreeStorageAndNotify(
    /* [in] */ Int64 freeStorageSize,
    /* [in] */ IIPackageDataObserver* observer)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IPackageManagerProxy::FreeStorage(
    /* [in] */ Int64 freeStorageSize,
    /* [in] */ IIntentSender* pi)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IPackageManagerProxy::DeleteApplicationCacheFiles(
    /* [in] */ const String& packageName,
    /* [in] */ IIPackageDataObserver* observer)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IPackageManagerProxy::ClearApplicationUserData(
    /* [in] */ const String& packageName,
    /* [in] */ IIPackageDataObserver* observer,
    /* [in] */ Int32 userId)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IPackageManagerProxy::GetPackageSizeInfo(
    /* [in] */ const String& packageName,
    /* [in] */ Int32 userHandle,
    /* [in] */ IIPackageStatsObserver* observer)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IPackageManagerProxy::GetSystemSharedLibraryNames(
    /* [out, callee] */ ArrayOf<String>** names)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IPackageManagerProxy::GetSystemAvailableFeatures(
    /* [out, callee] */ ArrayOf<IFeatureInfo*>** infos)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IPackageManagerProxy::HasSystemFeature(
    /* [in] */ const String& name,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    android::Parcel data, reply;

    data.writeInterfaceToken(android::String16(DESCRIPTOR.string()));
    AndroidParcelUtils::WriteString(data, name);
    Int32 version = 0;
    AndroidParcelUtils::WriteInt32(data, version);
    mPm->transact(TRANSACTION_hasSystemFeature, data, &reply, 0);
    ECode ec = reply.readExceptionCode() == 0 ? NOERROR : E_REMOTE_EXCEPTION;
    *result = AndroidParcelUtils::ReadInt32(reply) != 0;
    return ec;
}

ECode IPackageManagerProxy::EnterSafeMode()
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IPackageManagerProxy::IsSafeMode(
    /* [out] */ Boolean* isSafeMode)
{
    VALIDATE_NOT_NULL(isSafeMode);

    android::Parcel data, reply;

    data.writeInterfaceToken(android::String16(DESCRIPTOR.string()));
    mPm->transact(TRANSACTION_isSafeMode, data, &reply, 0);
    ECode ec = reply.readExceptionCode() == 0 ? NOERROR : E_REMOTE_EXCEPTION;
    *isSafeMode = AndroidParcelUtils::ReadInt32(reply) != 0;
    return ec;
}

ECode IPackageManagerProxy::SystemReady()
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IPackageManagerProxy::HasSystemUidErrors(
    /* [out] */ Boolean* result)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IPackageManagerProxy::PerformBootDexOpt()
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IPackageManagerProxy::PerformDexOptIfNeeded(
    /* [in] */ const String& packageName,
    /* [in] */ const String& instructionSet,
    /* [out] */ Boolean* result)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IPackageManagerProxy::ForceDexOpt(
    /* [in] */ const String& packageName)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IPackageManagerProxy::UpdateExternalMediaStatus(
    /* [in] */ Boolean mounted,
    /* [in] */ Boolean reportStatus)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IPackageManagerProxy::NextPackageToClean(
    /* [in] */ IPackageCleanItem* lastPackage,
    /* [out] */ IPackageCleanItem** nextPackage)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IPackageManagerProxy::MovePackage(
    /* [in] */ const String& packageName,
    /* [in] */ IIPackageMoveObserver* observer,
    /* [in] */ Int32 flags)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IPackageManagerProxy::AddPermissionAsync(
    /* [in] */ IPermissionInfo* info,
    /* [out] */ Boolean* result)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IPackageManagerProxy::SetInstallLocation(
    /* [in] */ Int32 loc,
    /* [out] */ Boolean* result)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IPackageManagerProxy::GetInstallLocation(
    /* [out] */ Int32* location)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IPackageManagerProxy::InstallExistingPackageAsUser(
    /* [in] */ const String& packageName,
    /* [in] */ Int32 userId,
    /* [out] */ Int32* result)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IPackageManagerProxy::VerifyPendingInstall(
    /* [in] */ Int32 id,
    /* [in] */ Int32 verificationCode)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IPackageManagerProxy::ExtendVerificationTimeout(
    /* [in] */ Int32 id,
    /* [in] */ Int32 verificationCodeAtTimeout,
    /* [in] */ Int64 millisecondsToDelay)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IPackageManagerProxy::GetVerifierDeviceIdentity(
    /* [out] */ IVerifierDeviceIdentity** identity)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IPackageManagerProxy::IsFirstBoot(
    /* [out] */ Boolean* result)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IPackageManagerProxy::IsOnlyCoreApps(
    /* [out] */ Boolean* result)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IPackageManagerProxy::SetPermissionEnforced(
    /* [in] */ const String& permission,
    /* [in] */ Boolean enforced)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IPackageManagerProxy::IsPermissionEnforced(
    /* [in] */ const String& permission,
    /* [out] */ Boolean* result)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IPackageManagerProxy::IsStorageLow(
    /* [out] */ Boolean* result)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IPackageManagerProxy::SetApplicationHiddenSettingAsUser(
    /* [in] */ const String& packageName,
    /* [in] */ Boolean hide,
    /* [in] */ Int32 userId,
    /* [out] */ Boolean* result)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IPackageManagerProxy::GetApplicationHiddenSettingAsUser(
    /* [in] */ const String& packageName,
    /* [in] */ Int32 userId,
    /* [out] */ Boolean* result)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IPackageManagerProxy::GetPackageInstaller(
    /* [out] */ IIPackageInstaller** installer)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IPackageManagerProxy::SetBlockUninstallForUser(
    /* [in] */ const String& packageName,
    /* [in] */ Boolean blockUninstall,
    /* [in] */ Int32 userId,
    /* [out] */ Boolean* result)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IPackageManagerProxy::GetBlockUninstallForUser(
    /* [in] */ const String& packageName,
    /* [in] */ Int32 userId,
    /* [out] */ Boolean* result)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IPackageManagerProxy::GetKeySetByAlias(
    /* [in] */ const String& packageName,
    /* [in] */ const String& alias,
    /* [out] */ IKeySet** set)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IPackageManagerProxy::GetSigningKeySet(
    /* [in] */ const String& packageName,
    /* [out] */ IKeySet** set)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IPackageManagerProxy::IsPackageSignedByKeySet(
    /* [in] */ const String& packageName,
    /* [in] */ IKeySet* ks,
    /* [out] */ Boolean* result)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IPackageManagerProxy::IsPackageSignedByKeySetExactly(
    /* [in] */ const String& packageName,
    /* [in] */ IKeySet* ks,
    /* [out] */ Boolean* result)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IPackageManagerProxy::SetComponentProtectedSetting(
    /* [in] */ IComponentName* componentName,
    /* [in] */ Boolean newState,
    /* [in] */ Int32 userId)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IPackageManagerProxy::UpdateIconMapping(
    /* [in] */ const String& pkgName)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IPackageManagerProxy::GetComposedIconInfo(
    /* [out] */ IComposedIconInfo** iconInfo)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IPackageManagerProxy::ProcessThemeResources(
    /* [in] */ const String& themePkgName,
    /* [out] */ Int32* res)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IPackageManagerProxy::SetPreLaunchCheckActivity(
    /* [in] */ IComponentName* componentName)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IPackageManagerProxy::AddPreLaunchCheckPackage(
    /* [in] */ const String& packageName)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IPackageManagerProxy::RemovePreLaunchCheckPackage(
    /* [in] */ const String& packageName)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IPackageManagerProxy::ClearPreLaunchCheckPackages()
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

} // namespace Pm
} // namespace Content
} // namespace Droid
} // namespace Elastos
