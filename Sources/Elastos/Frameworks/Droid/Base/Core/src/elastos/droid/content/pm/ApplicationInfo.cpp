#include "elastos/droid/content/pm/ApplicationInfo.h"
#include <elastos/core/StringBuilder.h>
#include <elastos/core/StringUtils.h>

using Elastos::Core::StringUtils;
using Elastos::Core::StringBuilder;

namespace Elastos {
namespace Droid {
namespace Content {
namespace Pm {

CAR_INTERFACE_IMPL_2(ApplicationInfo, PackageItemInfo, IApplicationInfo, IParcelable)

ApplicationInfo::ApplicationInfo()
    : mDescriptionRes(0)
    , mTheme(0)
    , mUiOptions(0)
    , mFlags(0)
    , mRequiresSmallestWidthDp(0)
    , mCompatibleWidthLimitDp(0)
    , mLargestWidthLimitDp(0)
    , mNativeLibraryRootRequiresIsa(FALSE)
    , mUid(0)
    , mTargetSdkVersion(0)
    , mVersionCode(0)
    , mEnabled(TRUE)
    , mEnabledSetting(IPackageManager::COMPONENT_ENABLED_STATE_DEFAULT)
    , mInstallLocation(IPackageInfo::INSTALL_LOCATION_UNSPECIFIED)
    , mIsThemeable(FALSE)
    , mProtect(FALSE)
{}

ApplicationInfo::~ApplicationInfo()
{}

ECode ApplicationInfo::constructor()
{
    return PackageItemInfo::constructor();
}

ECode ApplicationInfo::constructor(
    /* [in] */ IApplicationInfo* other)
{
    VALIDATE_NOT_NULL(other)

    FAIL_RETURN(PackageItemInfo::constructor(IPackageItemInfo::Probe(other)))

    ApplicationInfo* orig = (ApplicationInfo*)other;

    mTaskAffinity = orig->mTaskAffinity;
    mPermission = orig->mPermission;
    mProcessName = orig->mProcessName;
    mClassName = orig->mClassName;
    mTheme = orig->mTheme;
    mFlags = orig->mFlags;
    mRequiresSmallestWidthDp = orig->mRequiresSmallestWidthDp;
    mCompatibleWidthLimitDp = orig->mCompatibleWidthLimitDp;
    mLargestWidthLimitDp = orig->mLargestWidthLimitDp;
    mSourceDir = orig->mSourceDir;
    mPublicSourceDir = orig->mPublicSourceDir;
    mNativeLibraryDir = orig->mNativeLibraryDir;
    mResourceDirs = orig->mResourceDirs;
    mSharedLibraryFiles = orig->mSharedLibraryFiles;
    mDataDir = orig->mDataDir;
    mUid = orig->mUid;
    mTargetSdkVersion = orig->mTargetSdkVersion;
    mEnabled = orig->mEnabled;
    mEnabledSetting = orig->mEnabledSetting;
    mInstallLocation = orig->mInstallLocation;
    mManageSpaceActivityName = orig->mManageSpaceActivityName;
    mDescriptionRes = orig->mDescriptionRes;
    mUiOptions = orig->mUiOptions;
    mBackupAgentName = orig->mBackupAgentName;
    mProtect = orig->mProtect;
    mIsThemeable = orig->mIsThemeable;
    return NOERROR;
}

ECode ApplicationInfo::Dump(
    /* [in] */ IPrinter* pw,
    /* [in] */ const String& prefix)
{
    // super.dumpFront(pw, prefix);
    // if (className != null) {
    //     pw.println(prefix + "className=" + className);
    // }
    // if (permission != null) {
    //     pw.println(prefix + "permission=" + permission);
    // }
    // pw.println(prefix + "processName=" + processName);
    // pw.println(prefix + "taskAffinity=" + taskAffinity);
    // pw.println(prefix + "uid=" + uid + " flags=0x" + Integer.toHexString(flags)
    //         + " theme=0x" + Integer.toHexString(theme));
    // pw.println(prefix + "requiresSmallestWidthDp=" + requiresSmallestWidthDp
    //         + " compatibleWidthLimitDp=" + compatibleWidthLimitDp
    //         + " largestWidthLimitDp=" + largestWidthLimitDp);
    // pw.println(prefix + "sourceDir=" + sourceDir);
    // if (sourceDir == null) {
    //     if (publicSourceDir != null) {
    //         pw.println(prefix + "publicSourceDir=" + publicSourceDir);
    //     }
    // } else if (!sourceDir.equals(publicSourceDir)) {
    //     pw.println(prefix + "publicSourceDir=" + publicSourceDir);
    // }
    // if (resourceDirs != null) {
    //     pw.println(prefix + "resourceDirs=" + resourceDirs);
    // }
    // pw.println(prefix + "dataDir=" + dataDir);
    // if (sharedLibraryFiles != null) {
    //     pw.println(prefix + "sharedLibraryFiles=" + sharedLibraryFiles);
    // }
    // pw.println(prefix + "enabled=" + enabled + " targetSdkVersion=" + targetSdkVersion);
    // if (manageSpaceActivityName != null) {
    //     pw.println(prefix + "manageSpaceActivityName="+manageSpaceActivityName);
    // }
    // if (descriptionRes != 0) {
    //     pw.println(prefix + "description=0x"+Integer.toHexString(descriptionRes));
    // }
    // if (uiOptions != 0) {
    //     pw.println(prefix + "uiOptions=0x" + Integer.toHexString(uiOptions));
    // }
    // pw.println(prefix + "supportsRtl=" + (hasRtlSupport() ? "true" : "false"));
    // super.dumpBack(pw, prefix);
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode ApplicationInfo::HasRtlSupport(
    /* [out] */ Boolean* isSupport)
{
    VALIDATE_NOT_NULL(isSupport);

    *isSupport = ((mFlags & FLAG_SUPPORTS_RTL) == FLAG_SUPPORTS_RTL);
    return NOERROR;
}

ECode ApplicationInfo::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str);

    StringBuilder sb("ApplicationInfo:0x");
    sb += StringUtils::ToHexString((Int32)this);
    sb += ":";
    sb += mPackageName;
    sb += ", ";
    sb += mProcessName;
    sb += ", ";
    sb += mClassName;
    sb += ", ";
    sb += mSourceDir;
    sb.ToString(str);
    return NOERROR;
}

ECode ApplicationInfo::ReadFromParcel(
    /* [in] */ IParcel* source)
{
    assert(source != NULL);

    PackageItemInfo::ReadFromParcel(source);
    source->ReadString(&mTaskAffinity);
    source->ReadString(&mPermission);
    source->ReadString(&mProcessName);
    source->ReadString(&mClassName);
    source->ReadInt32(&mTheme);
    source->ReadInt32(&mFlags);
    source->ReadInt32(&mRequiresSmallestWidthDp);
    source->ReadInt32(&mCompatibleWidthLimitDp);
    source->ReadInt32(&mLargestWidthLimitDp);
    source->ReadString(&mScanSourceDir);
    source->ReadString(&mScanPublicSourceDir);
    source->ReadString(&mSourceDir);
    source->ReadString(&mPublicSourceDir);
    source->ReadArrayOfString((ArrayOf<String>**)&mSplitSourceDirs);
    source->ReadArrayOfString((ArrayOf<String>**)&mSplitPublicSourceDirs);
    source->ReadString(&mNativeLibraryDir);
    source->ReadString(&mSecondaryNativeLibraryDir);
    source->ReadString(&mNativeLibraryRootDir);
    source->ReadBoolean(&mNativeLibraryRootRequiresIsa);
    source->ReadString(&mPrimaryCpuAbi);
    source->ReadString(&mSecondaryCpuAbi);
    source->ReadArrayOfString((ArrayOf<String>**)&mResourceDirs);
    source->ReadString(&mSeinfo);
    source->ReadArrayOfString((ArrayOf<String>**)&mSharedLibraryFiles);
    source->ReadString(&mDataDir);
    source->ReadInt32(&mUid);
    source->ReadInt32(&mTargetSdkVersion);
    source->ReadInt32(&mVersionCode);
    source->ReadBoolean(&mEnabled);
    source->ReadInt32(&mEnabledSetting);
    source->ReadInt32(&mInstallLocation);
    source->ReadString(&mManageSpaceActivityName);
    source->ReadString(&mBackupAgentName);
    source->ReadInt32(&mDescriptionRes);
    source->ReadInt32(&mUiOptions);
    source->ReadBoolean(&mProtect);
    source->ReadBoolean(&mIsThemeable);
    return NOERROR;
}

ECode ApplicationInfo::WriteToParcel(
    /* [in] */ IParcel* dest)
{
    assert(dest != NULL);

    PackageItemInfo::WriteToParcel(dest);
    dest->WriteString(mTaskAffinity);
    dest->WriteString(mPermission);
    dest->WriteString(mProcessName);
    dest->WriteString(mClassName);
    dest->WriteInt32(mTheme);
    dest->WriteInt32(mFlags);
    dest->WriteInt32(mRequiresSmallestWidthDp);
    dest->WriteInt32(mCompatibleWidthLimitDp);
    dest->WriteInt32(mLargestWidthLimitDp);
    dest->WriteString(mScanSourceDir);
    dest->WriteString(mScanPublicSourceDir);
    dest->WriteString(mSourceDir);
    dest->WriteString(mPublicSourceDir);
    dest->WriteArrayOfString(mSplitSourceDirs);
    dest->WriteArrayOfString(mSplitPublicSourceDirs);
    dest->WriteString(mNativeLibraryDir);
    dest->WriteString(mSecondaryNativeLibraryDir);
    dest->WriteString(mNativeLibraryRootDir);
    dest->WriteBoolean(mNativeLibraryRootRequiresIsa);
    dest->WriteString(mPrimaryCpuAbi);
    dest->WriteString(mSecondaryCpuAbi);
    dest->WriteArrayOfString(mResourceDirs);
    dest->WriteString(mSeinfo);
    dest->WriteArrayOfString(mSharedLibraryFiles);
    dest->WriteString(mDataDir);
    dest->WriteInt32(mUid);
    dest->WriteInt32(mTargetSdkVersion);
    dest->WriteInt32(mVersionCode);
    dest->WriteBoolean(mEnabled);
    dest->WriteInt32(mEnabledSetting);
    dest->WriteInt32(mInstallLocation);
    dest->WriteString(mManageSpaceActivityName);
    dest->WriteString(mBackupAgentName);
    dest->WriteInt32(mDescriptionRes);
    dest->WriteInt32(mUiOptions);
    dest->WriteBoolean(mProtect);
    dest->WriteBoolean(mIsThemeable);
    return NOERROR;
}

ECode ApplicationInfo::LoadDescription(
    /* [in] */ IPackageManager* pm,
    /* [out] */ ICharSequence** label)
{
    VALIDATE_NOT_NULL(label);
    *label = NULL;

    if (mDescriptionRes != 0) {
        pm->GetText(mPackageName, mDescriptionRes, this, label);
    }

    return NOERROR;
}

ECode ApplicationInfo::DisableCompatibilityMode()
{
    mFlags |= (FLAG_SUPPORTS_LARGE_SCREENS | FLAG_SUPPORTS_NORMAL_SCREENS |
            FLAG_SUPPORTS_SMALL_SCREENS | FLAG_RESIZEABLE_FOR_SCREENS |
            FLAG_SUPPORTS_SCREEN_DENSITIES | FLAG_SUPPORTS_XLARGE_SCREENS);
    return NOERROR;
}

ECode ApplicationInfo::LoadDefaultIcon(
    /* [in] */ IPackageManager* pm,
    /* [out] */ IDrawable** icon)
{
    VALIDATE_NOT_NULL(icon);

    if ((mFlags & FLAG_EXTERNAL_STORAGE) != 0
            && IsPackageUnavailable(pm)) {
        // return Resources.getSystem().getDrawable(
        //         com.android.internal.R.drawable.sym_app_on_sd_unavailable_icon);
        assert(0);
        return E_NOT_IMPLEMENTED;
    }
    return pm->GetDefaultActivityIcon(icon);
}

Boolean ApplicationInfo::IsPackageUnavailable(
    /* [in] */ IPackageManager* pm)
{
    // try {
        AutoPtr<IPackageInfo> pkgInfo;
        ECode ec = pm->GetPackageInfo(mPackageName, 0, (IPackageInfo**)&pkgInfo);
        return SUCCEEDED(ec) ? pkgInfo == NULL : TRUE;
    // } catch (NameNotFoundException ex) {
    //     return true;
    // }
}

AutoPtr<IApplicationInfo> ApplicationInfo::GetApplicationInfo()
{
    return this;
}

ECode ApplicationInfo::GetTaskAffinity(
    /* [out] */ String* task)
{
    VALIDATE_NOT_NULL(task);
    *task = mTaskAffinity;
    return NOERROR;
}

ECode ApplicationInfo::SetTaskAffinity(
    /* [in] */ const String& task)
{
    mTaskAffinity = task;
    return NOERROR;
}

ECode ApplicationInfo::GetPermission(
    /* [out] */ String* permission)
{
    VALIDATE_NOT_NULL(permission);
    *permission = mPermission;
    return NOERROR;
}

ECode ApplicationInfo::SetPermission(
    /* [in] */ const String& permission)
{
    mPermission = permission;
    return NOERROR;
}

ECode ApplicationInfo::GetProcessName(
    /* [out] */ String* name)
{
    VALIDATE_NOT_NULL(name);
    *name = mProcessName;
    return NOERROR;
}

ECode ApplicationInfo::SetProcessName(
    /* [in] */ const String& name)
{
    mProcessName = name;
    return NOERROR;
}

ECode ApplicationInfo::GetClassName(
    /* [out] */ String* name)
{
    VALIDATE_NOT_NULL(name);
    *name = mClassName;
    return NOERROR;
}

ECode ApplicationInfo::SetClassName(
    /* [in] */ const String& name)
{
    mClassName = name;
    return NOERROR;
}

ECode ApplicationInfo::GetDescriptionRes(
    /* [out] */ Int32* desRes)
{
    VALIDATE_NOT_NULL(desRes);
    *desRes = mDescriptionRes;
    return NOERROR;
}

ECode ApplicationInfo::SetDescriptionRes(
    /* [in] */ Int32 desRes)
{
    mDescriptionRes = desRes;
    return NOERROR;
}

ECode ApplicationInfo::GetTheme(
    /* [out] */ Int32* theme)
{
    VALIDATE_NOT_NULL(theme);
    *theme = mTheme;
    return NOERROR;
}

ECode ApplicationInfo::SetTheme(
    /* [in] */ Int32 theme)
{
    mTheme = theme;
    return NOERROR;
}

ECode ApplicationInfo::GetManageSpaceActivityName(
    /* [out] */ String* name)
{
    VALIDATE_NOT_NULL(name);
    *name = mManageSpaceActivityName;
    return NOERROR;
}

ECode ApplicationInfo::SetManageSpaceActivityName(
    /* [in] */ const String& name)
{
    mManageSpaceActivityName = name;
    return NOERROR;
}

ECode ApplicationInfo::GetBackupAgentName(
    /* [out] */ String* name)
{
    VALIDATE_NOT_NULL(name);
    *name = mBackupAgentName;
    return NOERROR;
}

ECode ApplicationInfo::SetBackupAgentName(
    /* [in] */ const String& name)
{
    mBackupAgentName = name;
    return NOERROR;
}

ECode ApplicationInfo::GetUiOptions(
    /* [out] */ Int32* uiOptions)
{
    VALIDATE_NOT_NULL(uiOptions);
    *uiOptions = mUiOptions;
    return NOERROR;
}

ECode ApplicationInfo::SetUiOptions(
    /* [in] */ Int32 uiOptions)
{
    mUiOptions = uiOptions;
    return NOERROR;
}

ECode ApplicationInfo::GetFlags(
    /* [out] */ Int32* flags)
{
    VALIDATE_NOT_NULL(flags);
    *flags = mFlags;
    return NOERROR;
}

ECode ApplicationInfo::SetFlags(
    /* [in] */ Int32 flags)
{
    mFlags = flags;
    return NOERROR;
}

ECode ApplicationInfo::GetRequiresSmallestWidthDp(
    /* [out] */ Int32* requiresSmallestWidthDp)
{
    VALIDATE_NOT_NULL(requiresSmallestWidthDp);
    *requiresSmallestWidthDp = mRequiresSmallestWidthDp;
    return NOERROR;
}

ECode ApplicationInfo::SetRequiresSmallestWidthDp(
    /* [in] */ Int32 requiresSmallestWidthDp)
{
    mRequiresSmallestWidthDp = requiresSmallestWidthDp;
    return NOERROR;
}

ECode ApplicationInfo::GetCompatibleWidthLimitDp(
    /* [out] */ Int32* compatibleWidthLimitDp)
{
    VALIDATE_NOT_NULL(compatibleWidthLimitDp);
    *compatibleWidthLimitDp = mCompatibleWidthLimitDp;
    return NOERROR;
}

ECode ApplicationInfo::SetCompatibleWidthLimitDp(
    /* [in] */ Int32 compatibleWidthLimitDp)
{
    mCompatibleWidthLimitDp = compatibleWidthLimitDp;
    return NOERROR;
}

ECode ApplicationInfo::GetLargestWidthLimitDp(
    /* [out] */ Int32* largestWidthLimitDp)
{
    VALIDATE_NOT_NULL(largestWidthLimitDp);
    *largestWidthLimitDp = mLargestWidthLimitDp;
    return NOERROR;
}

ECode ApplicationInfo::SetLargestWidthLimitDp(
    /* [in] */ Int32 largestWidthLimitDp)
{
    mLargestWidthLimitDp = largestWidthLimitDp;
    return NOERROR;
}

ECode ApplicationInfo::GetSourceDir(
    /* [out] */ String* sourceDir)
{
    VALIDATE_NOT_NULL(sourceDir);
    *sourceDir = mSourceDir;
    return NOERROR;
}

ECode ApplicationInfo::SetSourceDir(
    /* [in] */ const String& sourceDir)
{
    mSourceDir = sourceDir;
    return NOERROR;
}

ECode ApplicationInfo::GetPublicSourceDir(
    /* [out] */ String* publicSourceDir)
{
    VALIDATE_NOT_NULL(publicSourceDir);
    *publicSourceDir = mPublicSourceDir;
    return NOERROR;
}

ECode ApplicationInfo::SetPublicSourceDir(
    /* [in] */ const String& pubicSourceDir)
{
    mPublicSourceDir = pubicSourceDir;
    return NOERROR;
}

ECode ApplicationInfo::GetResourceDirs(
    /* [out, callee] */ ArrayOf<String>** resourceDirs)
{
    VALIDATE_NOT_NULL(resourceDirs);
    *resourceDirs = mResourceDirs;
    REFCOUNT_ADD(*resourceDirs);
    return NOERROR;
}

ECode ApplicationInfo::SetResourceDirs(
    /* [in] */ ArrayOf<String>* resourceDirs)
{
    mResourceDirs = resourceDirs;
    return NOERROR;
}

ECode ApplicationInfo::GetSharedLibraryFiles(
    /* [out, callee] */ ArrayOf<String>** sharedLibraryFiles)
{
    VALIDATE_NOT_NULL(sharedLibraryFiles);
    *sharedLibraryFiles = mSharedLibraryFiles;
    REFCOUNT_ADD(*sharedLibraryFiles);
    return NOERROR;
}

ECode ApplicationInfo::SetSharedLibraryFiles(
    /* [in] */ ArrayOf<String>* sharedLibraryFiles)
{
    mSharedLibraryFiles = sharedLibraryFiles;
    return NOERROR;
}

ECode ApplicationInfo::GetDataDir(
    /* [out] */ String* dataDir)
{
    VALIDATE_NOT_NULL(dataDir);
    *dataDir = mDataDir;
    return NOERROR;
}

ECode ApplicationInfo::SetDataDir(
    /* [in] */ const String& dataDir)
{
    mDataDir = dataDir;
    return NOERROR;
}

ECode ApplicationInfo::GetNativeLibraryDir(
    /* [out] */ String* libraryDir)
{
    VALIDATE_NOT_NULL(libraryDir);
    *libraryDir = mNativeLibraryDir;
    return NOERROR;
}

ECode ApplicationInfo::SetNativeLibraryDir(
    /* [in] */ const String& libraryDir)
{
    mNativeLibraryDir = libraryDir;
    return NOERROR;
}

ECode ApplicationInfo::GetUid(
    /* [out] */ Int32* uid)
{
    VALIDATE_NOT_NULL(uid);
    *uid = mUid;
    return NOERROR;
}

ECode ApplicationInfo::SetUid(
    /* [in] */ Int32 uid)
{
    mUid = uid;
    return NOERROR;
}

ECode ApplicationInfo::GetTargetSdkVersion(
    /* [out] */ Int32* sdkVersion)
{
    VALIDATE_NOT_NULL(sdkVersion);
    *sdkVersion = mTargetSdkVersion;
    return NOERROR;
}

ECode ApplicationInfo::SetTargetSdkVersion(
    /* [in] */ Int32 sdkVersion)
{
    mTargetSdkVersion = sdkVersion;
    return NOERROR;
}

ECode ApplicationInfo::GetEnabled(
    /* [out] */ Boolean* enabled)
{
    VALIDATE_NOT_NULL(enabled);
    *enabled = mEnabled;
    return NOERROR;
}

ECode ApplicationInfo::SetEnabled(
    /* [in] */ Boolean enabled)
{
    mEnabled = enabled;
    return NOERROR;
}

ECode ApplicationInfo::GetEnabledSetting(
    /* [out] */ Int32* enabledSetting)
{
    VALIDATE_NOT_NULL(enabledSetting);
    *enabledSetting = mEnabledSetting;
    return NOERROR;
}

ECode ApplicationInfo::SetEnabledSetting(
    /* [in] */ Int32 enabledSetting)
{
    mEnabledSetting = enabledSetting;
    return NOERROR;
}

ECode ApplicationInfo::GetInstallLocation(
    /* [out] */ Int32* location)
{
    VALIDATE_NOT_NULL(location);
    *location = mInstallLocation;
    return NOERROR;
}

ECode ApplicationInfo::SetInstallLocation(
    /* [in] */ Int32 location)
{
    mInstallLocation = location;
    return NOERROR;
}


ECode ApplicationInfo::SetCodePath(
    /* [in] */ const String& codePath)
{
    mScanSourceDir = codePath;
    return NOERROR;
}

ECode ApplicationInfo::SetBaseCodePath(
    /* [in] */ const String& baseCodePath)
{
    mSourceDir = baseCodePath;
    return NOERROR;
}

ECode ApplicationInfo::SetSplitCodePaths(
    /* [in] */ ArrayOf<String>* splitCodePaths)
{
    mSplitSourceDirs = splitCodePaths;
    return NOERROR;
}

ECode ApplicationInfo::SetResourcePath(
    /* [in] */ const String& resourcePath)
{
    mScanPublicSourceDir = resourcePath;
    return NOERROR;
}

ECode ApplicationInfo::SetBaseResourcePath(
    /* [in] */ const String& baseResourcePath)
{
    mPublicSourceDir = baseResourcePath;
    return NOERROR;
}

ECode ApplicationInfo::SetSplitResourcePaths(
    /* [in] */ ArrayOf<String>* splitResourcePaths)
{
    mSplitPublicSourceDirs = splitResourcePaths;
    return NOERROR;
}

ECode ApplicationInfo::GetCodePath(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mScanSourceDir;
    return NOERROR;
}

ECode ApplicationInfo::GetBaseCodePath(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mSourceDir;
    return NOERROR;
}

ECode ApplicationInfo::GetSplitCodePaths(
    /* [out, callee] */ ArrayOf<String>** result)
{
    VALIDATE_NOT_NULL(result)
    *result = mSplitSourceDirs;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode ApplicationInfo::GetResourcePath(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mScanPublicSourceDir;
    return NOERROR;
}

ECode ApplicationInfo::GetBaseResourcePath(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mPublicSourceDir;
    return NOERROR;
}

ECode ApplicationInfo::GetSplitResourcePaths(
    /* [out, callee] */ ArrayOf<String>** result)
{
    VALIDATE_NOT_NULL(result)
    *result = mSplitPublicSourceDirs;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode ApplicationInfo::GetSplitSourceDirs(
    /* [out, callee] */ ArrayOf<String>** resourceDirs)
{
    VALIDATE_NOT_NULL(resourceDirs)
    *resourceDirs = mSplitSourceDirs;
    REFCOUNT_ADD(*resourceDirs)
    return NOERROR;
}

ECode ApplicationInfo::SetSplitSourceDirs(
    /* [in] */ ArrayOf<String>* resourceDirs)
{
    mResourceDirs = resourceDirs;
    return NOERROR;
}

ECode ApplicationInfo::GetSplitPublicSourceDirs(
    /* [out, callee] */ ArrayOf<String>** resourceDirs)
{
    VALIDATE_NOT_NULL(resourceDirs)
    *resourceDirs = mResourceDirs;
    REFCOUNT_ADD(*resourceDirs)
    return NOERROR;
}

ECode ApplicationInfo::SetSplitPublicSourceDirs(
    /* [in] */ ArrayOf<String>* resourceDirs)
{
    mResourceDirs = resourceDirs;
    return NOERROR;
}

ECode ApplicationInfo::GetPrimaryCpuAbi(
    /* [out] */ String* libraryDir)
{
    VALIDATE_NOT_NULL(libraryDir)
    *libraryDir = mPrimaryCpuAbi;
    return NOERROR;
}

ECode ApplicationInfo::SetPrimaryCpuAbi(
    /* [in] */ const String& libraryDir)
{
    mPrimaryCpuAbi = libraryDir;
    return NOERROR;
}

ECode ApplicationInfo::GetSecondaryCpuAbi(
    /* [out] */ String* libraryDir)
{
    VALIDATE_NOT_NULL(libraryDir)
    *libraryDir = mSecondaryCpuAbi;
    return NOERROR;
}

ECode ApplicationInfo::SetSecondaryCpuAbi(
    /* [in] */ const String& libraryDir)
{
    mSecondaryCpuAbi = libraryDir;
    return NOERROR;
}

ECode ApplicationInfo::GetVersionCode(
    /* [out] */ Int32* code)
{
    VALIDATE_NOT_NULL(code)
    *code = mVersionCode;
    return NOERROR;
}

ECode ApplicationInfo::SetVersionCode(
    /* [in] */ Int32 code)
{
    mVersionCode = code;
    return NOERROR;
}

ECode ApplicationInfo::SetScanSourceDir(
    /* [in] */ const String& value)
{
    mScanSourceDir = value;
    return NOERROR;
}

ECode ApplicationInfo::SetScanPublicSourceDir(
    /* [in] */ const String& value)
{
    mScanPublicSourceDir = value;
    return NOERROR;
}

ECode ApplicationInfo::SetSeinfo(
    /* [in] */ const String& value)
{
    mSeinfo = value;
    return NOERROR;
}

ECode ApplicationInfo::SetSecondaryNativeLibraryDir(
    /* [in] */ const String& value)
{
    mSecondaryNativeLibraryDir = value;
    return NOERROR;
}

ECode ApplicationInfo::SetNativeLibraryRootDir(
    /* [in] */ const String& value)
{
    mNativeLibraryRootDir = value;
    return NOERROR;
}

ECode ApplicationInfo::SetNativeLibraryRootRequiresIsa(
    /* [in] */ Boolean value)
{
    mNativeLibraryRootRequiresIsa = value;
    return NOERROR;
}

ECode ApplicationInfo::GetScanSourceDir(
    /* [out */ String* value)
{
    VALIDATE_NOT_NULL(value)
    *value = mScanSourceDir;
    return NOERROR;
}

ECode ApplicationInfo::GetScanPublicSourceDir(
    /* [out */ String* value)
{
    VALIDATE_NOT_NULL(value)
    *value = mScanPublicSourceDir;
    return NOERROR;
}

ECode ApplicationInfo::GetSeinfo(
    /* [out */ String* value)
{
    VALIDATE_NOT_NULL(value)
    *value = mSeinfo;
    return NOERROR;
}

ECode ApplicationInfo::GetSecondaryNativeLibraryDir(
    /* [out */ String* value)
{
    VALIDATE_NOT_NULL(value)
    *value = mSecondaryNativeLibraryDir;
    return NOERROR;
}

ECode ApplicationInfo::GetNativeLibraryRootDir(
    /* [out */ String* value)
{
    VALIDATE_NOT_NULL(value)
    *value = mNativeLibraryRootDir;
    return NOERROR;
}

ECode ApplicationInfo::GetNativeLibraryRootRequiresIsa(
    /* [out */ Boolean* value)
{
    VALIDATE_NOT_NULL(value)
    *value = mNativeLibraryRootRequiresIsa;
    return NOERROR;
}

ECode ApplicationInfo::GetIsThemeable(
    /* [out] */ Boolean* isThemeable)
{
    VALIDATE_NOT_NULL(isThemeable)
    *isThemeable = mIsThemeable;
    return NOERROR;
}

ECode ApplicationInfo::SetIsThemeable(
    /* [in] */ Boolean isThemeable)
{
    mIsThemeable = isThemeable;
    return NOERROR;
}

ECode ApplicationInfo::GetProtect(
    /* [out] */ Boolean* protect)
{
    VALIDATE_NOT_NULL(protect)
    *protect = mProtect;
    return NOERROR;
}

ECode ApplicationInfo::SetProtect(
    /* [in] */ Boolean protect)
{
    mProtect = protect;
    return NOERROR;
}


} // namespace Pm
} // namespace Content
} // namespace Droid
} // namespace Elastos
