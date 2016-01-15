
#include "pm/PackageSettingBase.h"

using Elastos::Droid::Content::Pm::IPackageManager;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Pm {

const Int32 PackageSettingBase::PKG_INSTALL_COMPLETE;
const Int32 PackageSettingBase::PKG_INSTALL_INCOMPLETE;
const AutoPtr<PackageUserState> PackageSettingBase::DEFAULT_USER_STATE = new PackageUserState();

PackageSettingBase::PackageSettingBase(
    /* [in] */ const String& name,
    /* [in] */ const String& realName,
    /* [in] */ IFile* codePath,
    /* [in] */ IFile* resourcePath,
    /* [in] */ const String& legacyNativeLibraryPathString,
    /* [in] */ const String& primaryCpuAbiString,
    /* [in] */ const String& secondaryCpuAbiString,
    /* [in] */ const String& cpuAbiOverrideString,
    /* [in] */ Int32 pVersionCode,
    /* [in] */ Int32 pkgFlags)
    : GrantedPermissions(pkgFlags)
    , mName(name)
    , mRealName(realName)
    , mTimeStamp(0)
    , mFirstInstallTime(0)
    , mLastUpdateTime(0)
    , mUidError(FALSE)
    , mPermissionsFixed(FALSE)
    , mHaveGids(FALSE)
    , mInstallStatus(PKG_INSTALL_COMPLETE)
    , mUserState(20)
{
    mSignatures = new PackageSignatures();
    mKeySetData = new PackageKeySetData();
    Init(codePath, resourcePath, legacyNativeLibraryPathString, primaryCpuAbiString,
            secondaryCpuAbiString, cpuAbiOverrideString, pVersionCode);
}

PackageSettingBase::PackageSettingBase(
    /* [in] */ const PackageSettingBase* base)
    : GrantedPermissions(base)
    , mName(base->mName)
    , mRealName(base->mRealName)
    , mCodePath(base->mCodePath)
    , mCodePathString(base->mCodePathString)
    , mResourcePath(base->mResourcePath)
    , mResourcePathString(base->mResourcePathString)
    , mLegacyNativeLibraryPathString(base->mLegacyNativeLibraryPathString;
    , mPrimaryCpuAbiString(base->mPrimaryCpuAbiString;
    , mSecondaryCpuAbiString(base->mSecondaryCpuAbiString;
    , mCpuAbiOverrideString(base->mCpuAbiOverrideString;
    , mNativeLibraryPathString(base->mNativeLibraryPathString)
    , mTimeStamp(base->mTimeStamp)
    , mFirstInstallTime(base->mFirstInstallTime)
    , mLastUpdateTime(base->mLastUpdateTime)
    , mVersionCode(base->mVersionCode)
    , mUidError(base->mUidError)
    , mPermissionsFixed(base->mPermissionsFixed)
    , mHaveGids(base->mHaveGids)
    , mInstallStatus(base->mInstallStatus)
    , mOrigPackage(base->mOrigPackage)
    , mInstallerPackageName(base->mInstallerPackageName)
    , mUserState(20)
{
    mSignatures = new PackageSignatures(base->mSignatures);
    mUserState.Clear();
    HashMap<Int32, AutoPtr<PackageUserState> >::ConstIterator it;
    for (it = base->mUserState.Begin(); it != base->mUserState.End(); ++it) {
        mUserState[it->mFirst] = new PackageUserState(it->mSecond);
    }

    mKeySetData = new PackageKeySetData(base->mKeySetData);
}

void PackageSettingBase::Init(
    /* [in] */ IFile* codePath,
    /* [in] */ IFile* resourcePath,
    /* [in] */ const String& legacyNativeLibraryPathString,
    /* [in] */ const String& primaryCpuAbiString,
    /* [in] */ const String& secondaryCpuAbiString,
    /* [in] */ const String& cpuAbiOverrideString,
    /* [in] */ Int32 pVersionCode)
{
    mCodePath = codePath;
    codePath->ToString(&mCodePathString);
    mResourcePath = resourcePath;
    resourcePath->ToString(&mResourcePathString);
    mLegacyNativeLibraryPathString = legacyNativeLibraryPathString;
    mPrimaryCpuAbiString = primaryCpuAbiString;
    mSecondaryCpuAbiString = secondaryCpuAbiString;
    mCpuAbiOverrideString = cpuAbiOverrideString;
    mVersionCode = pVersionCode;
}

void PackageSettingBase::SetInstallerPackageName(
    /* [in] */ const String& packageName)
{
    mInstallerPackageName = packageName;
}

String PackageSettingBase::GetInstallerPackageName()
{
    return mInstallerPackageName;
}

void PackageSettingBase::SetInstallStatus(
    /* [in] */ Int32 newStatus)
{
    mInstallStatus = newStatus;
}

Int32 PackageSettingBase::GetInstallStatus()
{
    return mInstallStatus;
}

void PackageSettingBase::SetTimeStamp(
    /* [in] */ Int64 newStamp)
{
    mTimeStamp = newStamp;
}

void PackageSettingBase::CopyFrom(
    /* [in] */ PackageSettingBase* base)
{
    mGrantedPermissions.Insert(base->mGrantedPermissions.Begin(), base->mGrantedPermissions.End());
    mGids = base->mGids;

    mPrimaryCpuAbiString = base->mPrimaryCpuAbiString;
    mSecondaryCpuAbiString = base->mSecondaryCpuAbiString;
    mCpuAbiOverrideString = base->mCpuAbiOverrideString;
    mTimeStamp = base->mTimeStamp;
    mFirstInstallTime = base->mFirstInstallTime;
    mLastUpdateTime = base->mLastUpdateTime;
    mSignatures = base->mSignatures;
    mPermissionsFixed = base->mPermissionsFixed;
    mHaveGids = base->mHaveGids;
    mUserState.Clear();
    HashMap<Int32, AutoPtr<PackageUserState> >::Iterator it;
    for (it = base->mUserState.Begin(); it != base->mUserState.End(); ++it) {
        mUserState[it->mFirst] = new PackageUserState(it->mSecond);
    }
    mInstallStatus = base->mInstallStatus;
    mKeySetData = base->mKeySetData;
}

AutoPtr<PackageUserState> PackageSettingBase::ModifyUserState(
    /* [in] */ Int32 userId)
{
    AutoPtr<PackageUserState> state = mUserState[userId];
    if (state == NULL) {
        state = new PackageUserState();
        mUserState[userId] = state;
    }
    return state;
}

AutoPtr<PackageUserState> PackageSettingBase::ReadUserState(
    /* [in] */ Int32 userId)
{
    HashMap<Int32, AutoPtr<PackageUserState> >::Iterator it = mUserState.Find(userId);
    if (it != mUserState.End()) {
        return it->mSecond;
    }
    return DEFAULT_USER_STATE;
}

void PackageSettingBase::SetEnabled(
    /* [in] */ Int32 state,
    /* [in] */ Int32 userId,
    /* [in] */ const String& callingPackage)
{
    AutoPtr<PackageUserState> st = ModifyUserState(userId);
    st->mEnabled = state;
    st->mLastDisableAppCaller = callingPackage;
}

Int32 PackageSettingBase::GetEnabled(
    /* [in] */ Int32 userId)
{
    return ReadUserState(userId)->mEnabled;
}

String PackageSettingBase::GetLastDisabledAppCaller(
    /* [in] */ Int32 userId)
{
    return ReadUserState(userId)->mLastDisableAppCaller;
}

void PackageSettingBase::SetInstalled(
    /* [in] */ Boolean inst,
    /* [in] */ Int32 userId)
{
    ModifyUserState(userId)->mInstalled = inst;
}

Boolean PackageSettingBase::GetInstalled(
    /* [in] */ Int32 userId)
{
    return ReadUserState(userId)->mInstalled;
}

Boolean PackageSettingBase::IsAnyInstalled(
    /* [in] */ const ArrayOf<Int32>& users)
{
    for (Int32 i = 0; i < users.GetLength(); ++i) {
        if (ReadUserState(users[i])->mInstalled) {
            return TRUE;
        }
    }
    return FALSE;
}

AutoPtr< ArrayOf<Int32> > PackageSettingBase::QueryInstalledUsers(
    /* [in] */ const ArrayOf<Int32>& users,
    /* [in] */ Boolean installed)
{
    Int32 num = 0;
    for (Int32 i = 0; i < users.GetLength(); ++i) {
        if (GetInstalled(users[i]) == installed) {
            num++;
        }
    }
    AutoPtr< ArrayOf<Int32> > res = ArrayOf<Int32>::Alloc(num);
    num = 0;
    for (Int32 i = 0; i < users.GetLength(); ++i) {
        if (GetInstalled(users[i]) == installed) {
            (*res)[num] = users[i];
            num++;
        }
    }
    return res;
}

Boolean PackageSettingBase::GetStopped(
    /* [in] */ Int32 userId)
{
    return ReadUserState(userId)->mStopped;
}

void PackageSettingBase::SetStopped(
    /* [in] */ Boolean stop,
    /* [in] */ Int32 userId)
{
    ModifyUserState(userId)->mStopped = stop;
}

Boolean PackageSettingBase::GetNotLaunched(
    /* [in] */ Int32 userId)
{
    return ReadUserState(userId)->mNotLaunched;
}

void PackageSettingBase::SetNotLaunched(
    /* [in] */ Boolean stop,
    /* [in] */ Int32 userId)
{
    ModifyUserState(userId)->mNotLaunched = stop;
}

Boolean PackageSettingBase::GetHidden(
    /* [in] */ Int32 userId)
{
    return ReadUserState(userId)->mHidden;
}

void PackageSettingBase::SetHidden(
    /* [in] */ Boolean hidden,
    /* [in] */ Int32 userId)
{
    ModifyUserState(userId)->mHidden = hidden;
}

Boolean PackageSettingBase::GetBlockUninstall(
    /* [in] */ Int32 userId)
{
    return ReadUserState(userId)->mBlockUninstall;
}

void PackageSettingBase::SetBlockUninstall(
    /* [in] */ Boolean blockUninstall,
    /* [in] */ Int32 userId)
{
    ModifyUserState(userId)->mBlockUninstall = blockUninstall;
}

void PackageSettingBase::SetUserState(
    /* [in] */ Int32 userId,
    /* [in] */ Int32 enabled,
    /* [in] */ Boolean installed,
    /* [in] */ Boolean stopped,
    /* [in] */ Boolean notLaunched,
    /* [in] */ Boolean hidden,
    /* [in] */ const String& lastDisableAppCaller,
    /* [in] */ HashSet<String>* enabledComponents,
    /* [in] */ HashSet<String>* disabledComponents,
    /* [in] */ Boolean blockUninstall)
{
    AutoPtr<PackageUserState> state = ModifyUserState(userId);
    state->mEnabled = enabled;
    state->mInstalled = installed;
    state->mStopped = stopped;
    state->mNotLaunched = notLaunched;
    state->mHidden = hidden;
    state->mLastDisableAppCaller = lastDisableAppCaller;
    state->mEnabledComponents = enabledComponents;
    state->mDisabledComponents = disabledComponents;
    state->mBlockUninstall = blockUninstall;
}

AutoPtr<HashSet<String> > PackageSettingBase::GetEnabledComponents(
    /* [in] */ Int32 userId)
{
    return ReadUserState(userId)->mEnabledComponents;
}

AutoPtr<HashSet<String> > PackageSettingBase::GetDisabledComponents(
    /* [in] */ Int32 userId)
{
    return ReadUserState(userId)->mDisabledComponents;
}

void PackageSettingBase::SetEnabledComponents(
    /* [in] */ HashSet<String>* components,
    /* [in] */ Int32 userId)
{
    AutoPtr<PackageUserState> state = ModifyUserState(userId);
    state->mEnabledComponents = components;
}

void PackageSettingBase::SetDisabledComponents(
    /* [in] */ HashSet<String>* components,
    /* [in] */ Int32 userId)
{
    AutoPtr<PackageUserState> state = ModifyUserState(userId);
    state->mDisabledComponents = components;
}

void PackageSettingBase::SetEnabledComponentsCopy(
    /* [in] */ HashSet<String>* components,
    /* [in] */ Int32 userId)
{
    AutoPtr<PackageUserState> state = ModifyUserState(userId);
    state->mEnabledComponents = components != NULL
        ? new HashSet<String>(components->Begin(), components->End()) : NULL;
}

void PackageSettingBase::SetDisabledComponentsCopy(
    /* [in] */ HashSet<String>* components,
    /* [in] */ Int32 userId)
{
    AutoPtr<PackageUserState> state = ModifyUserState(userId);
    state->mDisabledComponents = components != NULL
        ? new HashSet<String>(components->Begin(), components->End()) : NULL;
}

AutoPtr<PackageUserState> PackageSettingBase::ModifyUserStateComponents(
    /* [in] */ Int32 userId,
    /* [in] */ Boolean disabled,
    /* [in] */ Boolean enabled)
{
    AutoPtr<PackageUserState> state = ModifyUserState(userId);
    if (disabled && state->mDisabledComponents == NULL) {
        state->mDisabledComponents = new HashSet<String>(1);
    }
    if (enabled && state->mEnabledComponents == NULL) {
        state->mEnabledComponents = new HashSet<String>(1);
    }
    return state;
}

void PackageSettingBase::AddDisabledComponent(
    /* [in] */ const String& componentClassName,
    /* [in] */ Int32 userId)
{
    ModifyUserStateComponents(userId, TRUE, FALSE)->mDisabledComponents->Insert(componentClassName);
}

void PackageSettingBase::AddEnabledComponent(
    /* [in] */ const String& componentClassName,
    /* [in] */ Int32 userId)
{
    ModifyUserStateComponents(userId, FALSE, TRUE)->mEnabledComponents->Insert(componentClassName);
}

Boolean PackageSettingBase::EnableComponentLPw(
    /* [in] */ const String& componentClassName,
    /* [in] */ Int32 userId)
{
    AutoPtr<PackageUserState> state = ModifyUserStateComponents(userId, FALSE, TRUE);
    Boolean changed = FALSE;
    if (state->mDisabledComponents != NULL) {
        HashSet<String>::Iterator it = state->mDisabledComponents->Find(componentClassName);
        if (it != state->mDisabledComponents->End()) {
            state->mDisabledComponents->Erase(it);
            changed = TRUE;
        }
    }
    HashSet<String>::Iterator it = state->mEnabledComponents->Find(componentClassName);
    if (it == state->mEnabledComponents->End()) {
        state->mEnabledComponents->Insert(componentClassName);
        changed |= TRUE;
    }
    return changed;
}

Boolean PackageSettingBase::DisableComponentLPw(
    /* [in] */ const String& componentClassName,
    /* [in] */ Int32 userId)
{
    AutoPtr<PackageUserState> state = ModifyUserStateComponents(userId, TRUE, FALSE);
    Boolean changed = FALSE;
    if (state->mEnabledComponents != NULL) {
        HashSet<String>::Iterator it = state->mEnabledComponents->Find(componentClassName);
        if (it != state->mEnabledComponents->End()) {
            state->mEnabledComponents->Erase(it);
            changed = TRUE;
        }
    }
    HashSet<String>::Iterator it = state->mDisabledComponents->Find(componentClassName);
    if (it == state->mDisabledComponents->End()) {
        state->mDisabledComponents->Insert(componentClassName);
        changed |= TRUE;
    }
    return changed;
}

Boolean PackageSettingBase::RestoreComponentLPw(
    /* [in] */ const String& componentClassName,
    /* [in] */ Int32 userId)
{
    AutoPtr<PackageUserState> state = ModifyUserStateComponents(userId, TRUE, TRUE);
    Boolean changed = FALSE;
    if (state->mDisabledComponents != NULL) {
        HashSet<String>::Iterator it = state->mDisabledComponents->Find(componentClassName);
        if (it != state->mDisabledComponents->End()) {
            state->mDisabledComponents->Erase(it);
            changed = TRUE;
        }
    }
    if (state->mEnabledComponents != NULL) {
        HashSet<String>::Iterator it = state->mEnabledComponents->Find(componentClassName);
        if (it != state->mEnabledComponents->End()) {
            state->mEnabledComponents->Erase(it);
            changed |= TRUE;
        }
    }
    return changed;
}

Int32 PackageSettingBase::GetCurrentEnabledStateLPr(
    /* [in] */ const String& componentName,
    /* [in] */ Int32 userId)
{
    AutoPtr<PackageUserState> state = ReadUserState(userId);
    if (state->mEnabledComponents != NULL &&
        state->mEnabledComponents->Find(componentName) != state->mEnabledComponents->End()) {
        return IPackageManager::COMPONENT_ENABLED_STATE_ENABLED;
    }
    else if (state->mDisabledComponents != NULL &&
        state->mDisabledComponents->Find(componentName) != state->mDisabledComponents->End()) {
        return IPackageManager::COMPONENT_ENABLED_STATE_DISABLED;
    }
    else {
        return IPackageManager::COMPONENT_ENABLED_STATE_DEFAULT;
    }
}

void PackageSettingBase::RemoveUser(
    /* [in] */ Int32 userId)
{
    mUserState.Erase(userId);
}

} // namespace Pm
} // namespace Server
} // namespace Droid
} // namespace Elastos
