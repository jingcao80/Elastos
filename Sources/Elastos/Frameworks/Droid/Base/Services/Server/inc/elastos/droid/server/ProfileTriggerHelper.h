#ifndef __ELASTOS_DROID_SERVER_PROFILETRIGGERHELPER_H__
#define __ELASTOS_DROID_SERVER_PROFILETRIGGERHELPER_H__

#include <elastos/droid/content/BroadcastReceiver.h>
#include <elastos/droid/database/ContentObserver.h>

using Elastos::Droid::App::IIProfileManager;
using Elastos::Droid::Content::BroadcastReceiver;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::IIntentFilter;
using Elastos::Droid::Database::ContentObserver;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Wifi::IWifiManager;

namespace Elastos {
namespace Droid {
namespace Server {

class ProfileTriggerHelper
    : public BroadcastReceiver
{
private:
    class MyContentObserver
        : public ContentObserver
    {
    public:
        MyContentObserver(
            /* [in] */ IHandler* handler,
            /* [in] */ ProfileTriggerHelper* host);

        CARAPI OnChange(
            /* [in] */ Boolean selfChange);

    private:
        AutoPtr<IHandler> mHandler;
        ProfileTriggerHelper* mHost;
    };

public:
    ProfileTriggerHelper(
        /* [in] */ IContext* context,
        /* [in] */ IIProfileManager* service);

    CARAPI_(void) UpdateEnabled();

    CARAPI OnReceive(
        /* [in] */ IContext* context,
        /* [in] */ IIntent* intent);

private:
    CARAPI_(void) CheckTriggers(
        /* [in] */ Int32 type,
        /* [in] */ const String& id,
        /* [in] */ Int32 newState);

    CARAPI_(String) GetActiveSSID();

private:
    AutoPtr<IContext> mContext;
    AutoPtr<IIProfileManager> mService;
    AutoPtr<IWifiManager> mWifiManager;
    String mLastConnectedSSID;
    AutoPtr<IIntentFilter> mIntentFilter;
    Boolean mFilterRegistered;

    AutoPtr<IContentObserver> mSettingsObserver;

private:
    static const String TAG;
};

} // Server
} // Droid
} // Elastos

#endif // __ELASTOS_DROID_SERVER_PROFILETRIGGERHELPER_H__
