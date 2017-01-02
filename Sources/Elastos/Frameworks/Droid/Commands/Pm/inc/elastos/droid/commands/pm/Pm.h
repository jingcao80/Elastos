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

#ifndef __ELASTOS_DROID_COMMANDS_PM_PM_H__
#define __ELASTOS_DROID_COMMANDS_PM_PM_H__

#include <elastos/droid/app/PackageInstallObserver.h>
#include "_Elastos.Droid.Commands.Pm.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Os.h"
#include "Elastos.CoreLibrary.IO.h"
#include "Elastos.CoreLibrary.Utility.h"
#include <elastos/core/Singleton.h>
#include <elastos/utility/etl/HashMap.h>

using Elastos::Droid::App::PackageInstallObserver;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::IIIntentSender;
using Elastos::Droid::Content::IIntentSender;
using Elastos::Droid::Content::Pm::IIPackageDataObserver;
using Elastos::Droid::Content::Pm::IPackageItemInfo;
using Elastos::Droid::Content::Pm::IIPackageManager;
using Elastos::Droid::Content::Pm::IIPackageInstaller;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Os::IBinder;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Os::IIUserManager;
using Elastos::Core::ICharSequence;
using Elastos::Core::IComparator;
using Elastos::Core::Singleton;
using Elastos::IO::IPrintStream;
using Elastos::Utility::IList;
using Elastos::Utility::Etl::HashMap;
using Elastos::Utility::Concurrent::ISynchronousQueue;

namespace Elastos {
namespace Droid {
namespace Commands {
namespace Pm {

class Pm
    : public Singleton
    , public IPm
{
public:
    class ClearDataObserver
        : public Object
        , public IIPackageDataObserver
        , public IBinder
    {
    public:
        ClearDataObserver()
            : mFinished(FALSE)
            , mResult(FALSE)
        {}

        CAR_INTERFACE_DECL()

        // @Override
        CARAPI OnRemoveCompleted(
            /* [in] */ const String& packageName,
            /* [in] */ Boolean succeeded);

        CARAPI ToString(
            /* [in] */ String* str);

    public:
        Boolean mFinished;
        Boolean mResult;
    };

    class ClearCacheObserver
        : public Object
        , public IIPackageDataObserver
        , public IBinder
    {
    public:
        ClearCacheObserver()
            : mFinished(FALSE)
            , mResult(FALSE)
        {}

        CAR_INTERFACE_DECL()

        // @Override
        CARAPI OnRemoveCompleted(
            /* [in] */ const String& packageName,
            /* [in] */ Boolean succeeded);

        CARAPI ToString(
            /* [in] */ String* str);
    public:
        Boolean mFinished;
        Boolean mResult;
    };

private:
    class System
    {
    public:
        static AutoPtr<IPrintStream> Out;
        static AutoPtr<IPrintStream> Err;
    };

    class LocalPackageInstallObserver : public PackageInstallObserver
    {
    public:
        LocalPackageInstallObserver()
            : mFinished(FALSE)
            , mResult(0)
        {
            PackageInstallObserver::constructor();
        }

        CARAPI OnPackageInstalled(
            /* [in] */ const String& name,
            /* [in] */ Int32 status,
            /* [in] */ const String& msg,
            /* [in] */ IBundle* extras);

    public:
        Boolean mFinished;
        Int32 mResult;
        String mExtraPermission;
        String mExtraPackage;
    };

    class LocalIntentReceiver : public Object
    {
    public:
        LocalIntentReceiver();

        AutoPtr<IIntentSender> GetIntentSender();

        AutoPtr<IIntent> GetResult();

    private:
        AutoPtr<ISynchronousQueue> mResult;
        AutoPtr<IIIntentSender> mLocalSender;
    };

public:
    Pm();

    CAR_INTERFACE_DECL()

    /**
     * Command-line entry point.
     *
     * @param args The command-line arguments
     */
    CARAPI Main(
        /* [in] */ const ArrayOf<String>& args);

    CARAPI Run(
        /* [in]*/ ArrayOf<String>* args);

private:
    /**
     * Execute the list sub-command.
     *
     * pm list [package | packages]
     * pm list permission-groups
     * pm list permissions
     * pm list features
     * pm list libraries
     * pm list instrumentation
     */
    CARAPI_(void) RunList();

    /**
     * Lists all the installed packages.
     */
    CARAPI_(void) RunListPackages(
        /* [in] */ Boolean showApplicationPackage);

    // @SuppressWarnings("unchecked")
    CARAPI GetInstalledPackages(
        /* [in] */ IIPackageManager* pm,
        /* [in] */ Int32 flags,
        /* [in] */ Int32 userId,
        /* [out] */ IList** packages);

    /**
     * Lists all of the features supported by the current device.
     *
     * pm list features
     */
    CARAPI_(void) RunListFeatures();

    /**
     * Lists all of the libraries supported by the current device.
     *
     * pm list libraries
     */
    CARAPI_(void) RunListLibraries();

    /**
     * Lists all of the installed instrumentation, or all for a given package
     *
     * pm list instrumentation [package] [-f]
     */
    CARAPI_(void) RunListInstrumentation();

    /**
     * Lists all the known permission groups.
     */
    CARAPI_(void) RunListPermissionGroups();

    CARAPI_(String) LoadText(
        /* [in] */ IPackageItemInfo* pii,
        /* [in] */ Int32 res,
        /* [in] */ ICharSequence* nonLocalized);

    /**
     * Lists all the permissions in a group.
     */
    CARAPI_(void) RunListPermissions();

    CARAPI DoListPermissions(
        /* [in] */ ArrayOf<String>* groupList,
        /* [in] */ Boolean groups,
        /* [in] */ Boolean labels,
        /* [in] */ Boolean summary,
        /* [in] */ Int32 startProtectionLevel,
        /* [in] */ Int32 endProtectionLevel);

    CARAPI_(void) RunPath();

    CARAPI_(void) RunDump();

    /**
     * Converts a failure code into a string by using reflection to find a matching constant
     * in IPackageManager::
     */
    CARAPI_(String) InstallFailureToString(
        /* [in] */ LocalPackageInstallObserver* obs);

    CARAPI_(void) RunSetInstallLocation();

    CARAPI_(void) RunGetInstallLocation();

    CARAPI RunInstall();

    CARAPI RunInstallCreate();

    CARAPI RunInstallWrite();

    CARAPI RunInstallCommit();

    CARAPI RunInstallAbandon();

    CARAPI RunSetInstaller();

    CARAPI_(void) RunCreateUser();

    CARAPI_(void) RunRemoveUser();

    CARAPI_(void) RunListUsers();

    CARAPI_(void) RunGetMaxUsers();

    CARAPI RunForceDexOpt();

    CARAPI RunUninstall();

    CARAPI_(void) RunClear();

    CARAPI_(void) RunSetEnabledSetting(
        /* [in] */ Int32 state);

    CARAPI_(void) RunSetHiddenSetting(
        /* [in] */ Boolean state);

    CARAPI_(void) RunGrantRevokePermission(
        /* [in] */ Boolean grant);

    CARAPI_(void) RunSetPermissionEnforced();

    CARAPI_(void) RunTrimCaches();

    /**
     * Displays the package file for a package.
     * @param pckg
     */
    CARAPI_(void) DisplayPackageFilePath(
        /* [in] */ const String& pckg);

    CARAPI_(AutoPtr<IResources>) GetResources(
        /* [in] */ IPackageItemInfo* pii);

    CARAPI_(String) NextOption();

    CARAPI_(String) NextOptionData();

    CARAPI_(String) NextArg();

private:
    static CARAPI_(String) EnabledSettingToString(
        /* [in] */ Int32 state);

    static CARAPI_(Boolean) IsNumber(
        /* [in] */ const String& s);

    static CARAPI CheckAbiArgument(
        /* [in] */ const String& abi,
        /* [out] */ String* outAbi);

    static CARAPI_(void) ShowUsage();

private:
    static const String TAG;
    static const String PM_NOT_RUNNING_ERR;

    AutoPtr<IIPackageManager> mPm;
    AutoPtr<IIPackageInstaller> mInstaller;
    AutoPtr<IIUserManager> mUm;

    HashMap<String, AutoPtr<IResources> > mResourceCache; // WeakHashMap

    AutoPtr<ArrayOf<String> > mArgs;
    Int32 mNextArg;
    String mCurArgData;
};

}
}
}
}

#endif // __ELASTOS_DROID_COMMANDS_PM_PM_H__
