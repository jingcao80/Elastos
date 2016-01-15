
#ifndef __ELASTOS_DROID_SERVER_CONNECTIVITY_PERMISSION_MONITOR_H__
#define __ELASTOS_DROID_SERVER_CONNECTIVITY_PERMISSION_MONITOR_H__

#include <elastos/droid/ext/frameworkext.h>
#include "_Elastos.Droid.Server.h"
#include <elastos/droid/content/BroadcastReceiver.h>
#include <elastos/utility/etl/HashMap.h>
#include <elastos/utility/etl/HashSet.h>
#include <elastos/utility/etl/List.h>

using Elastos::Droid::Content::BroadcastReceiver;
using Elastos::Droid::Content::IBroadcastReceiver;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::IIntentFilter;
using Elastos::Droid::Content::Pm::IPackageInfo;
using Elastos::Droid::Content::Pm::IPackageManager;
using Elastos::Droid::Content::Pm::IUserInfo;

using Elastos::Droid::Net::IUri;
using Elastos::Droid::Os::IINetworkManagementService;
using Elastos::Droid::Os::IUserHandle;
using Elastos::Droid::Os::IUserManager;

using Elastos::Utility::Etl::HashMap;
using Elastos::Utility::Etl::HashSet;
using Elastos::Utility::Etl::List;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Connectivity {

/**
 * A utility class to inform Netd of UID permisisons.
 * Does a mass update at boot and then monitors for app install/remove.
 *
 * @hide
 */
class PermissionMonitor
    : public Object
{
private:

    class IntentReceiver
        : public BroadcastReceiver
    {
    public:
        IntentReceiver(
            /* [in] */ PermissionMonitor* host);

        CARAPI OnReceive(
            /* [in] */ IContext* context,
            /* [in] */ IIntent* intent);

        CARAPI ToString(
            /* [out] */ String* str)
        {
            VALIDATE_NOT_NULL(str)
            *str = String("PermissionMonitor::IntentReceiver");
            return NOERROR;
        }
    private:
        PermissionMonitor* mHost;
    };

public:

    PermissionMonitor(
        /* [in] */ IContext* context,
        /* [in] */ IINetworkManagementService* netd);

    // Intended to be called only once at startup, after the system is ready. Installs a broadcast
    // receiver to monitor ongoing UID changes, so this shouldn't/needn't be called again.
    void StartMonitoring();

private:
    Boolean HasPermission(
        /* [in] */ IPackageInfo* app,
        /* [in] */ const String& permission);

    Boolean HasNetworkPermission(
        /* [in] */ IPackageInfo* app);

    Boolean HasSystemPermission(
        /* [in] */ IPackageInfo* app);

    AutoPtr<ArrayOf<Int32> > ToInt32Array(
        /* [in] */ List<Int32>* list);

    CARAPI Update(
        /* [in] */ HashSet<Int32>* users,
        /* [in] */ HashMap<Int32, Boolean>* apps,
        /* [in] */ Boolean add);

    CARAPI OnUserAdded(
        /* [in] */ Int32 user);

    CARAPI OnUserRemoved(
        /* [in] */ Int32 user);

    CARAPI OnAppAdded(
        /* [in] */ const String& appName,
        /* [in] */ const Int32 appUid);

    CARAPI OnAppRemoved(
        /* [in] */ Int32 appUid);

private:
    static const String TAG;
    static const Boolean DBG;
    static const Boolean SYSTEM;
    static const Boolean NETWORK;

    AutoPtr<IContext> mContext;
    AutoPtr<IPackageManager> mPackageManager;
    AutoPtr<IUserManager> mUserManager;
    AutoPtr<IINetworkManagementService> mNetd;
    AutoPtr<IBroadcastReceiver> mIntentReceiver;

    // Values are User IDs.
    HashSet<Int32> mUsers;

    // Keys are App IDs. Values are true for SYSTEM permission and false for NETWORK permission.
    HashMap<Int32, Boolean> mApps;
};

} // Connectivity
} // Server
} // Droid
} // Elastos

#endif //__ELASTOS_DROID_SERVER_CONNECTIVITY_PERMISSION_MONITOR_H__
