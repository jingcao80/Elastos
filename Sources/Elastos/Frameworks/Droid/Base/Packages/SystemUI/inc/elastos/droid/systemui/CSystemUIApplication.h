
#ifndef  __ELASTOS_DROID_SYSTEMUI_CSYSTEMUIAPPLICATION_H__
#define  __ELASTOS_DROID_SYSTEMUI_CSYSTEMUIAPPLICATION_H__

#include "_Elastos_Droid_SystemUI_CSystemUIApplication.h"
// #include "Elastos.CoreLibrary.Utility.h"
#include <elastos/droid/app/Application.h>
#include <elastos/droid/content/BroadcastReceiver.h>
#include <elastos/utility/etl/HashMap.h>

using Elastos::Droid::App::Application;
using Elastos::Droid::Content::BroadcastReceiver;
// using Elastos::Utility::IMap;
using Elastos::Utility::Etl::HashMap;

namespace Elastos {
namespace Droid {
namespace SystemUI {

/**
 * Application class for SystemUI.
 */
CarClass(CSystemUIApplication)
    , public Application
    , public ISystemUIApplication
{
private:
    class UIBroadcastReceiver : public BroadcastReceiver
    {
    public:
        UIBroadcastReceiver(
            /* [in] */ CSystemUIApplication* host);

        // @Override
        CARAPI OnReceive(
            /* [in] */ IContext* context,
            /* [in] */ IIntent* intent);

    private:
        CSystemUIApplication* mHost;
    };

public:
    CAR_OBJECT_DECL();

    CAR_INTERFACE_DECL();

    CSystemUIApplication();

    // @Override
    CARAPI OnCreate();

    /**
     * Makes sure that all the SystemUI services are running. If they are already running, this is a
     * no-op. This is needed to conditinally start all the services, as we only need to have it in
     * the main process.
     *
     * <p>This method must only be called from the main thread.</p>
     */
    CARAPI StartServicesIfNeeded();

    // @Override
    CARAPI OnConfigurationChanged(
        /* [in] */ IConfiguration* newConfig);

    // @SuppressWarnings("unchecked")
    CARAPI GetComponent(
        /* [in] */ const InterfaceID& interfaceType,
        /* [out] */ IInterface** obj);

    CARAPI GetServices(
        /* [out, callee] */ ArrayOf<ISystemUI*>** services);

private:
    static const String TAG;
    static const Boolean DEBUG;

    /**
     * The classes of the stuff to start.
     */
    // private final Class<?>[] SERVICES = new Class[] {
    //         com.android.systemui.keyguard.KeyguardViewMediator.class,
    //         com.android.systemui.recent.Recents.class,
    //         com.android.systemui.volume.VolumeUI.class,
    //         com.android.systemui.statusbar.SystemBars.class,
    //         com.android.systemui.usb.StorageNotification.class,
    //         com.android.systemui.power.PowerUI.class,
    //         com.android.systemui.media.RingtonePlayer.class
    // };
    AutoPtr<ArrayOf<IClassInfo*> > SERVICES;

    /**
     * Hold a reference on the stuff we start.
     */
    AutoPtr<ArrayOf<ISystemUI*> > mServices;
    Boolean mServicesStarted;
    Boolean mBootCompleted;
    // AutoPtr<IMap> mComponents;  /*<Class<?>, Object*/
    AutoPtr<HashMap<InterfaceID, AutoPtr<IInterface> > > mComponents;
};

} // namespace SystemUI
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SYSTEMUI_CSYSTEMUIAPPLICATION_H__
