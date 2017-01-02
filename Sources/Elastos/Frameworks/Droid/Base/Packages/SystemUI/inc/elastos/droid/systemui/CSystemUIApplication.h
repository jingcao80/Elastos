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

#ifndef  __ELASTOS_DROID_SYSTEMUI_CSYSTEMUIAPPLICATION_H__
#define  __ELASTOS_DROID_SYSTEMUI_CSYSTEMUIAPPLICATION_H__

#include "_Elastos_Droid_SystemUI_CSystemUIApplication.h"
// #include "Elastos.CoreLibrary.Utility.h"
#include <elastos/droid/app/Application.h>
#include <elastos/utility/etl/HashMap.h>

using Elastos::Droid::App::Application;
// using Elastos::Utility::IMap;
using Elastos::Utility::Etl::HashMap;

namespace Elastos {
namespace Droid {
namespace SystemUI {

class CSystemUIBroadcastReceiver;

/**
 * Application class for SystemUI.
 */
CarClass(CSystemUIApplication)
    , public Application
    , public ISystemUIApplication
{
public:
    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    CSystemUIApplication();

    CARAPI constructor();

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
        /* [in] */ const String& interfaceType,
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
    AutoPtr<HashMap<String, AutoPtr<IInterface> > > mComponents;
    friend class CSystemUIBroadcastReceiver;
};

} // namespace SystemUI
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SYSTEMUI_CSYSTEMUIAPPLICATION_H__
