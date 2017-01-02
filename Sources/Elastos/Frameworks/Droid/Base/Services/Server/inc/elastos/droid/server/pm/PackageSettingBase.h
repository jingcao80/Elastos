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

#ifndef __ELASTOS_DROID_SERVER_PM_PACKAGESETTINGBASE_H__
#define __ELASTOS_DROID_SERVER_PM_PACKAGESETTINGBASE_H__

#include <Elastos.CoreLibrary.IO.h>
#include "Elastos.Droid.Content.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/server/pm/GrantedPermissions.h"
#include "elastos/droid/server/pm/PackageSignatures.h"
#include "elastos/droid/server/pm/PackageKeySetData.h"
#include "elastos/droid/content/pm/PackageUserState.h"
#include <elastos/utility/etl/HashMap.h>

using Elastos::Utility::Etl::HashMap;
using Elastos::IO::IFile;
using Elastos::Droid::Content::Pm::PackageUserState;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Pm {

class PackageSettingBase : public GrantedPermissions
{
public:
    PackageSettingBase(
        /* [in] */ const String& name,
        /* [in] */ const String& realName,
        /* [in] */ IFile* codePath,
        /* [in] */ IFile* resourcePath,
        /* [in] */ const String& legacyNativeLibraryPathString,
        /* [in] */ const String& primaryCpuAbiString,
        /* [in] */ const String& secondaryCpuAbiString,
        /* [in] */ const String& cpuAbiOverrideString,
        /* [in] */ Int32 pVersionCode,
        /* [in] */ Int32 pkgFlags);

    PackageSettingBase(
        /* [in] */ const PackageSettingBase* base);

    CARAPI_(void) Init(
        /* [in] */ IFile* codePath,
        /* [in] */ IFile* resourcePath,
        /* [in] */ const String& legacyNativeLibraryPathString,
        /* [in] */ const String& primaryCpuAbiString,
        /* [in] */ const String& secondaryCpuAbiString,
        /* [in] */ const String& cpuAbiOverrideString,
        /* [in] */ Int32 pVersionCode);

    CARAPI_(void) SetInstallerPackageName(
        /* [in] */ const String& packageName);

    CARAPI_(String) GetInstallerPackageName();

    CARAPI_(void) SetInstallStatus(
        /* [in] */ Int32 newStatus);

    CARAPI_(Int32) GetInstallStatus();

    CARAPI_(void) SetTimeStamp(
        /* [in] */ Int64 newStamp);

    CARAPI_(void) CopyFrom(
        /* [in] */ PackageSettingBase* base);

    CARAPI_(AutoPtr<PackageUserState>) ReadUserState(
        /* [in] */ Int32 userId);

    CARAPI_(void) SetEnabled(
        /* [in] */ Int32 state,
        /* [in] */ Int32 userId,
        /* [in] */ const String& callingPackage);

    CARAPI_(Int32) GetEnabled(
        /* [in] */ Int32 userId);

    CARAPI_(String) GetLastDisabledAppCaller(
        /* [in] */ Int32 userId);

    CARAPI_(void) SetInstalled(
        /* [in] */ Boolean inst,
        /* [in] */ Int32 userId);

    CARAPI_(Boolean) GetInstalled(
        /* [in] */ Int32 userId);

    CARAPI_(Boolean) IsAnyInstalled(
        /* [in] */ const ArrayOf<Int32>& users);

    CARAPI_(AutoPtr< ArrayOf<Int32> >) QueryInstalledUsers(
        /* [in] */ const ArrayOf<Int32>& users,
        /* [in] */ Boolean installed);

    CARAPI_(Boolean) GetStopped(
        /* [in] */ Int32 userId);

    CARAPI_(void) SetStopped(
        /* [in] */ Boolean stop,
        /* [in] */ Int32 userId);

    CARAPI_(Boolean) GetNotLaunched(
        /* [in] */ Int32 userId);

    CARAPI_(void) SetNotLaunched(
        /* [in] */ Boolean stop,
        /* [in] */ Int32 userId);

    CARAPI_(Boolean) GetHidden(
        /* [in] */ Int32 userId);

    CARAPI_(void) SetHidden(
        /* [in] */ Boolean hidden,
        /* [in] */ Int32 userId);

    CARAPI_(Boolean) GetBlockUninstall(
        /* [in] */ Int32 userId);

    CARAPI_(void) SetBlockUninstall(
        /* [in] */ Boolean blockUninstall,
        /* [in] */ Int32 userId);

    CARAPI_(void) SetUserState(
        /* [in] */ Int32 userId,
        /* [in] */ Int32 enabled,
        /* [in] */ Boolean installed,
        /* [in] */ Boolean stopped,
        /* [in] */ Boolean notLaunched,
        /* [in] */ Boolean hidden,
        /* [in] */ const String& lastDisableAppCaller,
        /* [in] */ HashSet<String>* enabledComponents,
        /* [in] */ HashSet<String>* disabledComponents,
        /* [in] */ Boolean blockUninstall,
        /* [in] */ HashSet<String>* protectedComponents,
        /* [in] */ HashSet<String>* visibleComponents);

    CARAPI_(AutoPtr<HashSet<Elastos::String> >) GetEnabledComponents(
        /* [in] */ Int32 userId);

    CARAPI_(AutoPtr<HashSet<Elastos::String> >) GetDisabledComponents(
        /* [in] */ Int32 userId);

    CARAPI_(void) SetEnabledComponents(
        /* [in] */ HashSet<String>* components,
        /* [in] */ Int32 userId);

    CARAPI_(void) SetDisabledComponents(
        /* [in] */ HashSet<String>* components,
        /* [in] */ Int32 userId);

    CARAPI_(void) SetEnabledComponentsCopy(
        /* [in] */ HashSet<String>* components,
        /* [in] */ Int32 userId);

    CARAPI_(void) SetDisabledComponentsCopy(
        /* [in] */ HashSet<String>* components,
        /* [in] */ Int32 userId);

    CARAPI_(AutoPtr<PackageUserState>) ModifyUserStateComponents(
        /* [in] */ Int32 userId,
        /* [in] */ Boolean disabled,
        /* [in] */ Boolean enabled);

    CARAPI_(AutoPtr<PackageUserState>) ModifyUserStateComponents(
        /* [in] */ Int32 userId);

    CARAPI_(void) AddDisabledComponent(
        /* [in] */ const String& componentClassName,
        /* [in] */ Int32 userId);

    CARAPI_(void) AddEnabledComponent(
        /* [in] */ const String& componentClassName,
        /* [in] */ Int32 userId);

    CARAPI_(Boolean) EnableComponentLPw(
        /* [in] */ const String& componentClassName,
        /* [in] */ Int32 userId);

    CARAPI_(Boolean) DisableComponentLPw(
        /* [in] */ const String& componentClassName,
        /* [in] */ Int32 userId);

    CARAPI_(Boolean) RestoreComponentLPw(
        /* [in] */ const String& componentClassName,
        /* [in] */ Int32 userId);

    CARAPI_(Int32) GetCurrentEnabledStateLPr(
        /* [in] */ const String& componentName,
        /* [in] */ Int32 userId);

    CARAPI_(Boolean) ProtectComponentLPw(
        /* [in] */ const String& componentClassName,
        /* [in] */ Boolean protect,
        /* [in] */ Int32 userId);

    CARAPI_(AutoPtr<HashSet<Elastos::String> >) GetProtectedComponents(
        /* [in] */ Int32 userId);

    CARAPI_(void) RemoveUser(
        /* [in] */ Int32 userId);

private:
    CARAPI_(AutoPtr<PackageUserState>) ModifyUserState(
        /* [in] */ Int32 userId);

public:
    /**
     * Indicates the state of installation. Used by PackageManager to figure out
     * incomplete installations. Say a package is being installed (the state is
     * set to PKG_INSTALL_INCOMPLETE) and remains so till the package
     * installation is successful or unsuccessful in which case the
     * PackageManager will no longer maintain state information associated with
     * the package. If some exception(like device freeze or battery being pulled
     * out) occurs during installation of a package, the PackageManager needs
     * this information to clean up the previously failed installation.
     */
    static const Int32 PKG_INSTALL_COMPLETE = 1;
    static const Int32 PKG_INSTALL_INCOMPLETE = 0;

    String mName;
    String mRealName;

    /**
     * Path where this package was found on disk. For monolithic packages
     * this is path to single base APK file; for cluster packages this is
     * path to the cluster directory.
     */
    AutoPtr<IFile> mCodePath;
    String mCodePathString;
    AutoPtr<IFile> mResourcePath;
    String mResourcePathString;

    /**
     * The path under which native libraries have been unpacked. This path is
     * always derived at runtime, and is only stored here for cleanup when a
     * package is uninstalled.
     */
    // @Deprecated
    String mLegacyNativeLibraryPathString;

    /**
     * The primary CPU abi for this package. This value is regenerated at every
     * boot scan.
     */
    String mPrimaryCpuAbiString;

    /**
     * The secondary CPU abi for this package. This value is regenerated at every
     * boot scan.
     */
    String mSecondaryCpuAbiString;

    /**
     * The install time CPU override, if any. This value is written at install time
     * and doesn't change during the life of an install. If non-null,
     * {@code primaryCpuAbiString} will contain the same value.
     */
    String mCpuAbiOverrideString;

    Int64 mTimeStamp;
    Int64 mFirstInstallTime;
    Int64 mLastUpdateTime;
    Int32 mVersionCode;

    Boolean mUidError;

    AutoPtr<PackageSignatures> mSignatures;

    Boolean mPermissionsFixed;
    Boolean mHaveGids;

    AutoPtr<PackageKeySetData> mKeySetData;

    Int32 mInstallStatus;

    AutoPtr<PackageSettingBase> mOrigPackage;

    /* package name of the app that installed this package */
    String mInstallerPackageName;

private:
    static const AutoPtr<PackageUserState> DEFAULT_USER_STATE;

    // Whether this package is currently stopped, thus can not be
    // started until explicitly launched by the user.
    HashMap<Int32, AutoPtr<PackageUserState> > mUserState;
};

} // namespace Pm
} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SERVER_PM_PACKAGESETTINGBASE_H__
