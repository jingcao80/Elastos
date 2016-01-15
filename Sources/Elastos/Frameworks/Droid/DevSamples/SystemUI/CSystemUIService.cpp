#include "CSystemUIService.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Utility::Logging::Logger;
using Elastos::Droid::Content::EIID_IContext;
using Elastos::Droid::View::IIWindowManager;
using Elastos::Droid::View::IWindowManagerGlobal;
using Elastos::Droid::View::CWindowManagerGlobal;
using Elastos::Droid::SystemUI::StatusBar::Tablet::ITabletStatusBar;
using Elastos::Droid::SystemUI::StatusBar::Tablet::CTabletStatusBar;
using Elastos::Droid::App::EIID_IService;
using Elastos::Droid::Content::EIID_IContextWrapper;
using Elastos::Droid::Content::EIID_IComponentCallbacks2;

namespace Elastos {
namespace Droid {
namespace DevSamples {
namespace SystemUI {

const String CSystemUIService::TAG("SystemUIService");

/**
 * The class names of the stuff to start.
 */
// final Object[] SERVICES = new Object[] {
//         0, // system bar or status bar, filled in below.
//         com.android.systemui.power.PowerUI.class,
//         com.android.systemui.media.RingtonePlayer.class,
//     };
//

CSystemUIService::CSystemUIService()
{
}

PInterface CSystemUIService::Probe(
    /* [in] */ REIID riid)
{
    if (riid == EIID_IInterface) {
        return (PInterface)(IService*)this;
    }
    else if (riid == EIID_IObject) {
        return (IObject*)this;
    }
    if (riid == EIID_IService) {
        return (IService*)this;
    }
    else if (riid == EIID_IContextWrapper) {
       return (IContextWrapper*)this;
    }
    else if (riid == EIID_IContext) {
       return (IContext*)this;
    }
    else if (riid == EIID_IComponentCallbacks2) {
       return (IComponentCallbacks2*)this;
    }
    else if (riid == EIID_IWeakReferenceSource) {
        return (IWeakReferenceSource*)this;
    }

    return Service::Probe(riid);
}

ECode CSystemUIService::OnCreate()
{
    // Tell the accessibility layer that this process will
    // run as the current user, i.e. run across users.
    // AccessibilityManager.createAsSharedAcrossUsers(this);

    const Int32 SERVICE_COUNT = 3;
    mServices = ArrayOf<ISystemUI* >::Alloc(SERVICE_COUNT);

    AutoPtr<IWindowManagerGlobal> wmGlobal;
    CWindowManagerGlobal::AcquireSingleton((IWindowManagerGlobal**)&wmGlobal);
    AutoPtr<IIWindowManager> wm;
    wmGlobal->GetWindowManagerService((IIWindowManager**)&wm);
    Boolean hasSystemNavBar;
    wm->HasSystemNavBar(&hasSystemNavBar);
    if (hasSystemNavBar) {
        // config_systemBarComponent:com.android.systemui.statusbar.tablet.TabletStatusBar
        AutoPtr<ITabletStatusBar> statusBar;
        CTabletStatusBar::New((ITabletStatusBar**)&statusBar);
        mServices->Set(0, ISystemUI::Probe(statusBar.Get()));
    }
    else {
        // config_statusBarComponent:com.android.systemui.statusbar.phone.PhoneStatusBar
        assert(0 && "TODO");
    }

    AutoPtr<IContext> context = THIS_PROBE(IContext);
    for (Int32 i = 0; i < mServices->GetLength(); i++) {
        AutoPtr<ISystemUI> systemUI = (*mServices)[i];
        if (systemUI) {
            systemUI->SetContext(context);
            systemUI->Start();
        }
    }

    return NOERROR;
}

ECode CSystemUIService::OnConfigurationChanged(
    /* [in] */ IConfiguration* newConfig)
{
    for (Int32 i = 0; i < mServices->GetLength(); i++) {
        AutoPtr<ISystemUI> systemUI = (*mServices)[i];
        if (systemUI) {
            systemUI->OnConfigurationChanged(newConfig);
        }
    }
    return NOERROR;
}

ECode CSystemUIService::OnBind(
    /* [in] */ IIntent* intent,
    /* [out] */ IBinder** binder)
{
    VALIDATE_NOT_NULL(binder);
    *binder = NULL;
    return NOERROR;
}

} // SystemUI
} // DevSamples
} // Droid
} // Elastos
