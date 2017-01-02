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

#ifndef __ELASTOS_DROID_SERVER_PM_CLAUNCHERAPPSIMPL_H__
#define __ELASTOS_DROID_SERVER_PM_CLAUNCHERAPPSIMPL_H__

#include "_Elastos_Droid_Server_Pm_CLauncherAppsImpl.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/internal/content/PackageMonitor.h"
#include "elastos/droid/os/RemoteCallbackList.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Os.h"
#include "Elastos.Droid.Graphics.h"
#include <elastos/core/AutoLock.h>
#include <Elastos.CoreLibrary.Utility.h>

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::IComponentName;
using Elastos::Droid::Content::Pm::IILauncherApps;
using Elastos::Droid::Content::Pm::IPackageManager;
using Elastos::Droid::Content::Pm::IOnAppsChangedListener;
using Elastos::Droid::Content::Pm::IResolveInfo;
using Elastos::Droid::Graphics::IRect;
using Elastos::Droid::Internal::Content::PackageMonitor;
using Elastos::Droid::Os::IBinder;
using Elastos::Droid::Os::IUserManager;
using Elastos::Droid::Os::IUserHandle;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Os::RemoteCallbackList;
using Elastos::Utility::IList;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Pm {

CarClass(CLauncherAppsImpl)
    , public Object
    , public IILauncherApps
    , public IBinder
{
public:
    class PackageCallbackList : public RemoteCallbackList
    {
    public:
        PackageCallbackList(
            /* [in] */ CLauncherAppsImpl* host)
            : mHost(host)
        {}

        CARAPI OnCallbackDied(
            /* [in] */ IInterface* callback,
            /* [in] */ IInterface* cookie);

    private:
        CLauncherAppsImpl* mHost;
    };

private:
    class MyPackageMonitor : public PackageMonitor
    {
    public:
        MyPackageMonitor(
            /* [in] */ CLauncherAppsImpl* host)
            : mHost(host)
        {}

        CARAPI OnPackageAdded(
            /* [in] */ const String& packageName,
            /* [in] */ Int32 uid);

        CARAPI OnPackageRemoved(
            /* [in] */ const String& packageName,
            /* [in] */ Int32 uid);

        CARAPI OnPackageModified(
            /* [in] */ const String& packageName);

        CARAPI OnPackagesAvailable(
            /* [in] */ ArrayOf<String>* packages);

        CARAPI OnPackagesUnavailable(
            /* [in] */ ArrayOf<String>* packages);

        TO_STRING_IMPL("CLauncherAppsImpl::MyPackageMonitor")

    private:
        /** Checks if user is a profile of or same as listeningUser.
          * and the user is enabled.
        */
        CARAPI_(Boolean) IsEnabledProfileOf(
            /* [in] */ IUserHandle* user,
            /* [in] */ IUserHandle* listeningUser,
            /* [in] */ const String& debugMsg);

    private:
        CLauncherAppsImpl* mHost;
    };

public:
    CLauncherAppsImpl();

    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor(
        /* [in] */ IContext* ctx);

    CARAPI AddOnAppsChangedListener(
        /* [in] */ IOnAppsChangedListener* listener);

    CARAPI RemoveOnAppsChangedListener(
        /* [in] */ IOnAppsChangedListener* listener);

    CARAPI_(void) CheckCallbackCount();

    CARAPI GetLauncherActivities(
        /* [in] */ const String& packageName,
        /* [in] */ IUserHandle* user,
        /* [out] */ IList** list);

    CARAPI ResolveActivity(
        /* [in] */ IIntent* intent,
        /* [in] */ IUserHandle* user,
        /* [out] */ IResolveInfo** info);

    CARAPI IsPackageEnabled(
        /* [in] */ const String& packageName,
        /* [in] */ IUserHandle* user,
        /* [out] */ Boolean* result);

    CARAPI StartActivityAsUser(
        /* [in] */ IComponentName* component,
        /* [in] */ IRect* sourceBounds,
        /* [in] */ IBundle* opts,
        /* [in] */ IUserHandle* user);

    CARAPI ShowAppDetailsAsUser(
        /* [in] */ IComponentName* component,
        /* [in] */ IRect* sourceBounds,
        /* [in] */ IBundle* opts,
        /* [in] */ IUserHandle* user);

    CARAPI IsActivityEnabled(
        /* [in] */ IComponentName* component,
        /* [in] */ IUserHandle* user,
        /* [out] */ Boolean* result);

    CARAPI ToString(
        /* [out] */ String* str);

private:
    /**
     * Register a receiver to watch for package broadcasts
     */
    CARAPI_(void) StartWatchingPackageBroadcasts();

    /**
     * Unregister package broadcast receiver
     */
    CARAPI_(void) StopWatchingPackageBroadcasts();

    /**
     * Checks if the caller is in the same group as the userToCheck.
     */
    CARAPI EnsureInUserProfiles(
        /* [in] */ IUserHandle* userToCheck,
        /* [in] */ const String& message);

    /**
     * Checks if the user is enabled.
     */
    CARAPI_(Boolean) IsUserEnabled(
        /* [in] */ IUserHandle* user);

private:
    static const Boolean DEBUG = FALSE;
    static const String TAG;
    AutoPtr<IContext> mContext;
    AutoPtr<IPackageManager> mPm;
    AutoPtr<IUserManager> mUm;
    AutoPtr<PackageCallbackList> mListeners;
    Object mListenersLock;

    AutoPtr<MyPackageMonitor> mPackageMonitor;

    friend class MyPackageMonitor;
};

} // namespace Pm
} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SERVER_PM_CLAUNCHERAPPSIMPL_H__
