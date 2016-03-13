
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/content/pm/CPackageInfo.h"
#include <elastos/utility/logging/Slogger.h>

using Elastos::Utility::Logging::Slogger;

namespace Elastos {
namespace Droid {
namespace Content {
namespace Pm {

CAR_INTERFACE_IMPL_2(CPackageInfo, Object, IPackageInfo, IParcelable)

CAR_OBJECT_IMPL(CPackageInfo)

CPackageInfo::CPackageInfo()
    : mVersionCode(0)
    , mSharedUserLabel(0)
    , mFirstInstallTime(0)
    , mLastUpdateTime(0)
    , mInstallLocation(INSTALL_LOCATION_INTERNAL_ONLY)
    , mCoreApp(FALSE)
    , mIsThemeApk(FALSE)
    , mHasIconPack(FALSE)
    , mIsLegacyIconPackApk(FALSE)
{}

CPackageInfo::~CPackageInfo()
{}

ECode CPackageInfo::constructor()
{
    return NOERROR;
}

ECode CPackageInfo::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str);
    // return "PackageInfo{"
    //         + Integer.toHexString(System.identityHashCode(this))
    //         + " " + packageName + "}";
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode CPackageInfo::ReadFromParcel(
    /* [in] */ IParcel* source)
{
    assert(source != NULL);
    source->ReadString(&mPackageName);
    source->ReadArrayOfString((ArrayOf<String>**)&mSplitNames);
    source->ReadInt32(&mVersionCode);
    source->ReadString(&mVersionName);
    source->ReadString(&mSharedUserId);
    source->ReadInt32(&mSharedUserLabel);
    AutoPtr<IInterface> obj;
    source->ReadInterfacePtr((Handle32*)&obj);
    mApplicationInfo = obj != NULL ? IApplicationInfo::Probe(obj) : NULL;
    source->ReadInt64(&mFirstInstallTime);
    source->ReadInt64(&mLastUpdateTime);
    source->ReadArrayOf((Handle32*)&mGids);
    source->ReadArrayOf((Handle32*)&mActivities);
    source->ReadArrayOf((Handle32*)&mReceivers);
    source->ReadArrayOf((Handle32*)&mServices);
    source->ReadArrayOf((Handle32*)&mProviders);
    source->ReadArrayOf((Handle32*)&mInstrumentation);
    source->ReadArrayOf((Handle32*)&mPermissions);
    source->ReadArrayOfString((ArrayOf<String>**)&mRequestedPermissions);
    source->ReadArrayOf((Handle32*)&mRequestedPermissionsFlags);
    source->ReadArrayOf((Handle32*)&mSignatures);
    source->ReadArrayOf((Handle32*)&mConfigPreferences);
    source->ReadArrayOf((Handle32*)&mReqFeatures);
    source->ReadArrayOf((Handle32*)&mFeatureGroups);
    source->ReadInt32(&mInstallLocation);
    Int32 ival;
    source->ReadInt32(&ival);
    mCoreApp = ival != 0;
    source->ReadInt32(&ival);
    mRequiredForAllUsers = ival != 0;
    source->ReadString(&mRestrictedAccountType);
    source->ReadString(&mRequiredAccountType);
    source->ReadString(&mOverlayTarget);

    /* Theme-specific. */
    source->ReadBoolean(&mIsThemeApk);
    source->ReadArrayOfString((ArrayOf<String>**)&mOverlayTargets);
    obj = NULL;
    source->ReadInterfacePtr((Handle32*)&obj);
    mThemeInfo = IThemeInfo::Probe(obj);
    source->ReadBoolean(&mHasIconPack);
    source->ReadBoolean(&mIsLegacyIconPackApk);
    return NOERROR;
}

ECode CPackageInfo::WriteToParcel(
    /* [in] */ IParcel* dest)
{
    assert(dest != NULL);
    dest->WriteString(mPackageName);
    dest->WriteArrayOfString(mSplitNames.Get());
    dest->WriteInt32(mVersionCode);
    dest->WriteString(mVersionName);
    dest->WriteString(mSharedUserId);
    dest->WriteInt32(mSharedUserLabel);
    dest->WriteInterfacePtr(mApplicationInfo);
    dest->WriteInt64(mFirstInstallTime);
    dest->WriteInt64(mLastUpdateTime);
    dest->WriteArrayOf((Handle32)mGids.Get());
    dest->WriteArrayOf((Handle32)mActivities.Get());
    dest->WriteArrayOf((Handle32)mReceivers.Get());
    dest->WriteArrayOf((Handle32)mServices.Get());
    dest->WriteArrayOf((Handle32)mProviders.Get());
    dest->WriteArrayOf((Handle32)mInstrumentation.Get());
    dest->WriteArrayOf((Handle32)mPermissions.Get());
    dest->WriteArrayOfString(mRequestedPermissions.Get());
    dest->WriteArrayOf((Handle32)mRequestedPermissionsFlags.Get());
    dest->WriteArrayOf((Handle32)mSignatures.Get());
    dest->WriteArrayOf((Handle32)mConfigPreferences.Get());
    dest->WriteArrayOf((Handle32)mReqFeatures.Get());
    dest->WriteArrayOf((Handle32)mFeatureGroups.Get());
    dest->WriteInt32(mInstallLocation);
    dest->WriteInt32(mCoreApp ? 1 : 0);
    dest->WriteInt32(mRequiredForAllUsers ? 1 : 0);
    dest->WriteString(mRestrictedAccountType);
    dest->WriteString(mRequiredAccountType);
    dest->WriteString(mOverlayTarget);

    /* Theme-specific. */
    dest->WriteBoolean(mIsThemeApk);
    dest->WriteArrayOfString(mOverlayTargets);
    dest->WriteInterfacePtr(mThemeInfo);
    dest->WriteBoolean(mHasIconPack);
    dest->WriteBoolean(mIsLegacyIconPackApk);
    return NOERROR;
}

ECode CPackageInfo::GetPackageName(
    /* [out] */ String* packageName)
{
    VALIDATE_NOT_NULL(packageName);
    *packageName = mPackageName;
    return NOERROR;
}

ECode CPackageInfo::SetPackageName(
    /* [in] */ const String& packageName)
{
    mPackageName = packageName;
    return NOERROR;
}

ECode CPackageInfo::SetSplitNames(
    /* [in] */ ArrayOf<String>* names)
{
    mSplitNames = names;
    return NOERROR;
}

ECode CPackageInfo::GetSplitNames(
    /* [out, callee] */ ArrayOf<String>** names)
{
    VALIDATE_NOT_NULL(names)
    *names = mSplitNames;
    REFCOUNT_ADD(*names)
    return NOERROR;
}

ECode CPackageInfo::GetVersionCode(
    /* [out] */ Int32* versionCode)
{
    VALIDATE_NOT_NULL(versionCode);
    *versionCode = mVersionCode;
    return NOERROR;
}

ECode CPackageInfo::SetVersionCode(
    /* [in] */ Int32 versionCode)
{
    mVersionCode = versionCode;
    return NOERROR;
}

ECode CPackageInfo::GetVersionName(
    /* [out] */ String* versionName)
{
    VALIDATE_NOT_NULL(versionName);
    *versionName = mVersionName;
    return NOERROR;
}

ECode CPackageInfo::SetVersionName(
    /* [in] */ const String& versionName)
{
    mVersionName = versionName;
    return NOERROR;
}

ECode CPackageInfo::GetSharedUserId(
    /* [out] */ String* id)
{
    VALIDATE_NOT_NULL(id);
    *id = mSharedUserId;
    return NOERROR;
}

ECode CPackageInfo::SetSharedUserId(
    /* [in] */ const String& id)
{
    mSharedUserId = id;
    return NOERROR;
}

ECode CPackageInfo::GetSharedUserLabel(
    /* [out] */ Int32* label)
{
    VALIDATE_NOT_NULL(label);
    *label = mSharedUserLabel;
    return NOERROR;
}

ECode CPackageInfo::SetSharedUserLabel(
    /* [in] */ Int32 label)
{
    mSharedUserLabel = label;
    return NOERROR;
}

ECode CPackageInfo::GetApplicationInfo(
    /* [out] */ IApplicationInfo** info)
{
    VALIDATE_NOT_NULL(info);
    *info = mApplicationInfo;
    REFCOUNT_ADD(*info);
    return NOERROR;
}

ECode CPackageInfo::SetApplicationInfo(
    /* [in] */ IApplicationInfo* info)
{
    mApplicationInfo = info;
    return NOERROR;
}

ECode CPackageInfo::GetFirstInstallTime(
    /* [out] */ Int64* time)
{
    VALIDATE_NOT_NULL(time);
    *time = mFirstInstallTime;
    return NOERROR;
}

ECode CPackageInfo::SetFirstInstallTime(
    /* [in] */ Int64 time)
{
    mFirstInstallTime = time;
    return NOERROR;
}

ECode CPackageInfo::GetLastUpdateTime(
    /* [out] */ Int64* time)
{
    VALIDATE_NOT_NULL(time);
    *time = mLastUpdateTime;
    return NOERROR;
}

ECode CPackageInfo::SetLastUpdateTime(
    /* [in] */ Int64 time)
{
    mLastUpdateTime = time;
    return NOERROR;
}

ECode CPackageInfo::GetGids(
    /* [out, callee] */ ArrayOf<Int32>** gids)
{
    VALIDATE_NOT_NULL(gids);
    *gids = mGids;
    REFCOUNT_ADD(*gids);
    return NOERROR;
}

ECode CPackageInfo::SetGids(
    /* [in] */ ArrayOf<Int32>* gids)
{
    mGids = gids;
    return NOERROR;
}

ECode CPackageInfo::GetActivities(
    /* [out, callee] */ ArrayOf<IActivityInfo*>** activities)
{
    VALIDATE_NOT_NULL(activities);
    *activities = mActivities;
    REFCOUNT_ADD(*activities);
    return NOERROR;
}

ECode CPackageInfo::SetActivities(
    /* [in] */ ArrayOf<IActivityInfo*>* activities)
{
    mActivities = activities;
    return NOERROR;
}

ECode CPackageInfo::GetReceivers(
    /* [out, callee] */ ArrayOf<IActivityInfo*>** receivers)
{
    VALIDATE_NOT_NULL(receivers);
    *receivers = mReceivers;
    REFCOUNT_ADD(*receivers);
    return NOERROR;
}

ECode CPackageInfo::SetReceivers(
    /* [in] */ ArrayOf<IActivityInfo*>* receivers)
{
    mReceivers = receivers;
    return NOERROR;
}

ECode CPackageInfo::GetServices(
    /* [out, callee] */ ArrayOf<IServiceInfo*>** services)
{
    VALIDATE_NOT_NULL(services);
    *services = mServices;
    REFCOUNT_ADD(*services);
    return NOERROR;
}

ECode CPackageInfo::SetServices(
    /* [in] */ ArrayOf<IServiceInfo*>* services)
{
    mServices = services;
    return NOERROR;
}

ECode CPackageInfo::GetProviders(
    /* [out, callee] */ ArrayOf<IProviderInfo*>** providers)
{
    VALIDATE_NOT_NULL(providers);
    *providers = mProviders;
    REFCOUNT_ADD(*providers);
    return NOERROR;
}

ECode CPackageInfo::SetProviders(
    /* [in] */ ArrayOf<IProviderInfo*>* providers)
{
    mProviders = providers;
    return NOERROR;
}

ECode CPackageInfo::GetInstrumentation(
    /* [out, callee] */ ArrayOf<IInstrumentationInfo*>** instrumentation)
{
    VALIDATE_NOT_NULL(instrumentation);
    *instrumentation = mInstrumentation;
    REFCOUNT_ADD(*instrumentation);
    return NOERROR;
}

ECode CPackageInfo::SetInstrumentation(
    /* [in] */ ArrayOf<IInstrumentationInfo*>* instrumentation)
{
    mInstrumentation = instrumentation;
    return NOERROR;
}

ECode CPackageInfo::GetPermissions(
    /* [out, callee] */ ArrayOf<IPermissionInfo*>** permissions)
{
    VALIDATE_NOT_NULL(permissions);
    *permissions = mPermissions;
    REFCOUNT_ADD(*permissions);
    return NOERROR;
}

ECode CPackageInfo::SetPermissions(
    /* [in] */ ArrayOf<IPermissionInfo*>* permissions)
{
    mPermissions = permissions;
    return NOERROR;
}

ECode CPackageInfo::GetRequestedPermissions(
    /* [out, callee] */ ArrayOf<String>** requestedPermissions)
{
    VALIDATE_NOT_NULL(requestedPermissions);
    *requestedPermissions = mRequestedPermissions;
    REFCOUNT_ADD(*requestedPermissions);
    return NOERROR;
}

ECode CPackageInfo::SetRequestedPermissions(
    /* [in] */ ArrayOf<String>* requestedPermissions)
{
    mRequestedPermissions = requestedPermissions;
    return NOERROR;
}

ECode CPackageInfo::GetRequestedPermissionsFlags(
    /* [out, callee] */ ArrayOf<Int32>** requestedPermissionsFlags)
{
    VALIDATE_NOT_NULL(requestedPermissionsFlags);
    *requestedPermissionsFlags = mRequestedPermissionsFlags;
    REFCOUNT_ADD(*requestedPermissionsFlags);
    return NOERROR;
}

ECode CPackageInfo::SetRequestedPermissionsFlags(
    /* [in] */ ArrayOf<Int32>* requestedPermissionsFlags)
{
    mRequestedPermissionsFlags = requestedPermissionsFlags;
    return NOERROR;
}

ECode CPackageInfo::GetSignatures(
    /* [out, callee] */ ArrayOf<ISignature*>** signatures)
{
    VALIDATE_NOT_NULL(signatures);
    *signatures = mSignatures;
    REFCOUNT_ADD(*signatures);
    return NOERROR;
}

ECode CPackageInfo::SetSignatures(
    /* [in] */ ArrayOf<ISignature*>* signatures)
{
    mSignatures = signatures;
    return NOERROR;
}

ECode CPackageInfo::GetConfigPreferences(
    /* [out, callee] */ ArrayOf<IConfigurationInfo*>** configPreferences)
{
    VALIDATE_NOT_NULL(configPreferences);
    *configPreferences = mConfigPreferences;
    REFCOUNT_ADD(*configPreferences);
    return NOERROR;
}

ECode CPackageInfo::SetConfigPreferences(
    /* [in] */ ArrayOf<IConfigurationInfo*>* configPreferences)
{
    mConfigPreferences = configPreferences;
    return NOERROR;
}

ECode CPackageInfo::GetReqFeatures(
    /* [out, callee] */ ArrayOf<IFeatureInfo*>** reqFeatures)
{
    VALIDATE_NOT_NULL(reqFeatures);
    *reqFeatures = mReqFeatures;
    REFCOUNT_ADD(*reqFeatures);
    return NOERROR;
}

ECode CPackageInfo::SetReqFeatures(
    /* [in] */ ArrayOf<IFeatureInfo*>* reqFeatures)
{
    mReqFeatures = reqFeatures;
    return NOERROR;
}

ECode CPackageInfo::GetFeatureGroups(
    /* [out, callee] */ ArrayOf<IFeatureGroupInfo*>** featureGroups)
{
    VALIDATE_NOT_NULL(featureGroups)
    *featureGroups = mFeatureGroups;
    REFCOUNT_ADD(*featureGroups)
    return NOERROR;
}

ECode CPackageInfo::SetFeatureGroups(
    /* [in] */ ArrayOf<IFeatureGroupInfo*>* featureGroups)
{
    mFeatureGroups = featureGroups;
    return NOERROR;
}

ECode CPackageInfo::GetInstallLocation(
    /* [out] */ Int32* installLocation)
{
    VALIDATE_NOT_NULL(installLocation);
    *installLocation = mInstallLocation;
    return NOERROR;
}

ECode CPackageInfo::SetInstallLocation(
    /* [in] */ Int32 installLocation)
{
    mInstallLocation = installLocation;
    return NOERROR;
}

ECode CPackageInfo::GetIsLegacyIconPackPkg(
    /* [out] */ Boolean* result)
{
    assert(0);
    return NOERROR;
}

ECode CPackageInfo::SetIsLegacyIconPackPkg(
    /* [in] */ Boolean isLegacyIconPackPkg)
{
    assert(0);
    return NOERROR;
}

/** @hide */
ECode CPackageInfo::GetRequiredAccountType(
    /* [out] */ String* requiredAccountType)
{
    VALIDATE_NOT_NULL(requiredAccountType);
    *requiredAccountType = mRequiredAccountType;
    return NOERROR;
}

/** @hide */
ECode CPackageInfo::SetRequiredAccountType(
    /* [in] */ const String& requiredAccountType)
{
    mRequiredAccountType = requiredAccountType;
    return NOERROR;
}

/** @hide */
ECode CPackageInfo::GetRestrictedAccountType(
    /* [out] */ String* type)
{
    VALIDATE_NOT_NULL(type);
    *type = mRestrictedAccountType;
    return NOERROR;
}

/** @hide */
ECode CPackageInfo::SetRestrictedAccountType(
    /* [in] */ const String& type)
{
    mRestrictedAccountType = type;
    return NOERROR;
}

ECode CPackageInfo::GetIsThemeApk(
    /* [out] */ Boolean* isThemeApk)
{
    VALIDATE_NOT_NULL(isThemeApk)
    *isThemeApk = mIsThemeApk;
    return NOERROR;
}

ECode CPackageInfo::SetIsThemeApk(
    /* [in] */ Boolean isThemeApk)
{
    mIsThemeApk = isThemeApk;
    return NOERROR;
}

ECode CPackageInfo::GetHasIconPack(
    /* [out] */ Boolean* hasIconPack)
{
    VALIDATE_NOT_NULL(hasIconPack)
    *hasIconPack = mHasIconPack;
    return NOERROR;
}

ECode CPackageInfo::SetHasIconPack(
    /* [in] */ Boolean hasIconPack)
{
    mHasIconPack = hasIconPack;
    return NOERROR;
}

ECode CPackageInfo::GetOverlayTargets(
    /* [out, callee] */ ArrayOf<String>** targets)
{
    VALIDATE_NOT_NULL(targets)
    *targets = mOverlayTargets;
    REFCOUNT_ADD(*targets)
    return NOERROR;
}

ECode CPackageInfo::SetOverlayTargets(
    /* [in] */ ArrayOf<String>* targets)
{
    mOverlayTargets = targets;
    return NOERROR;
}

ECode CPackageInfo::GetIsLegacyIconPackApk(
    /* [out] */ Boolean* isLegacyIconPackApk)
{
    VALIDATE_NOT_NULL(isLegacyIconPackApk)
    *isLegacyIconPackApk = mIsLegacyIconPackApk;
    return NOERROR;
}

ECode CPackageInfo::SetIsLegacyIconPackApk(
    /* [in] */ Boolean isLegacyIconPackApk)
{
    mIsLegacyIconPackApk = isLegacyIconPackApk;
    return NOERROR;
}

ECode CPackageInfo::GetThemeInfo(
    /* [out] */ IThemeInfo** themeInfo)
{
    VALIDATE_NOT_NULL(themeInfo)
    *themeInfo = mThemeInfo;
    REFCOUNT_ADD(*themeInfo)
    return NOERROR;
}

ECode CPackageInfo::SetThemeInfo(
    /* [in] */ IThemeInfo* themeInfo)
{
    mThemeInfo = themeInfo;
    return NOERROR;
}

} // namespace Pm
} // namespace Content
} // namespace Droid
} // namespace Elastos
