
#include "elastos/droid/systemui/CSystemUIApplication.h"
#include "elastos/droid/systemui/CSystemUIBroadcastReceiver.h"
#include "elastos/droid/systemui/SystemUI.h"
#include "elastos/droid/systemui/Utils.h"
#include "Elastos.Droid.Os.h"
#include "R.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Content::CIntentFilter;
using Elastos::Droid::Content::EIID_IContext;
using Elastos::Droid::Content::IIntentFilter;
using Elastos::Droid::Content::IBroadcastReceiver;
using Elastos::Droid::Os::CSystemProperties;
using Elastos::Droid::Os::ISystemProperties;
// using Elastos::Utility::CHashMap;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace SystemUI {

const String CSystemUIApplication::TAG("SystemUIService");
const Boolean CSystemUIApplication::DEBUG = TRUE;

CAR_OBJECT_IMPL(CSystemUIApplication)

CAR_INTERFACE_IMPL(CSystemUIApplication, Application, ISystemUIApplication)

CSystemUIApplication::CSystemUIApplication()
    : mServicesStarted(FALSE)
    , mBootCompleted(FALSE)
{
}

ECode CSystemUIApplication::constructor()
{
    SERVICES = ArrayOf<IClassInfo*>::Alloc(7);
    AutoPtr<IClassInfo> clsInfo = Utils::GetClassInfo(String("Elastos.Droid.SystemUI.Keyguard.CKeyguardViewMediator"));
    SERVICES->Set(0, clsInfo);
    clsInfo = Utils::GetClassInfo(String("Elastos.Droid.SystemUI.Recent.CRecents"));
    SERVICES->Set(1, clsInfo);
    clsInfo = Utils::GetClassInfo(String("Elastos.Droid.SystemUI.Volume.CVolumeUI"));
    SERVICES->Set(2, clsInfo);
    clsInfo = Utils::GetClassInfo(String("Elastos.Droid.SystemUI.StatusBar.CSystemBars"));
    SERVICES->Set(3, clsInfo);
    clsInfo = Utils::GetClassInfo(String("Elastos.Droid.SystemUI.Usb.CStorageNotification"));
    SERVICES->Set(4, clsInfo);
    clsInfo = Utils::GetClassInfo(String("Elastos.Droid.SystemUI.Power.CPowerUI"));
    SERVICES->Set(5, clsInfo);
    clsInfo = Utils::GetClassInfo(String("Elastos.Droid.SystemUI.Media.CRingtonePlayer"));
    SERVICES->Set(6, clsInfo);

    mServices = ArrayOf<ISystemUI*>::Alloc(SERVICES->GetLength());
    mComponents = new HashMap<String, AutoPtr<IInterface> >();
    return Application::constructor();
}

ECode CSystemUIApplication::OnCreate()
{
    Application::OnCreate();
    // Set the application theme that is inherited by all services. Note that setting the
    // application theme in the manifest does only work for activities. Keep this in sync with
    // the theme set there.
    SetTheme(R::style::systemui_theme);

    AutoPtr<IIntentFilter> filter;
    CIntentFilter::New(IIntent::ACTION_BOOT_COMPLETED, (IIntentFilter**)&filter);
    filter->SetPriority(IIntentFilter::SYSTEM_HIGH_PRIORITY);
    AutoPtr<IBroadcastReceiver> receiver;
    CSystemUIBroadcastReceiver::New(this, (IBroadcastReceiver**)&receiver);
    AutoPtr<IIntent> intent;
    RegisterReceiver(receiver, filter, (IIntent**)&intent);
    return NOERROR;
}

ECode CSystemUIApplication::StartServicesIfNeeded()
{
    if (mServicesStarted) {
        return NOERROR;
    }

    if (!mBootCompleted) {
        // check to see if maybe it was already completed long before we began
        // see ActivityManagerService.finishBooting()
        AutoPtr<ISystemProperties> sp;
        CSystemProperties::AcquireSingleton((ISystemProperties**)&sp);
        String property;
        sp->Get(String("sys.boot_completed"), &property);
        if (String("1").Equals(property)) {
            mBootCompleted = TRUE;
            if (DEBUG) Logger::V(TAG, "BOOT_COMPLETED was already sent");
        }
    }

    Logger::V(TAG, "Starting SystemUI services.");
    Int32 N = SERVICES->GetLength();
    for (Int32 i = 0; i < N; i++) {
        AutoPtr<IClassInfo> cl = (*SERVICES)[i];
        if (DEBUG) Logger::D(TAG, String("loading: %s"), TO_CSTR(cl));

        AutoPtr<IInterface> object;
        if (FAILED(cl->CreateObject((IInterface**)&object))) {
            return E_RUNTIME_EXCEPTION;
        }

        mServices->Set(i, ISystemUI::Probe(object));

        (*mServices)[i]->SetContext(this);
        ((SystemUI*)(*mServices)[i])->SetComponents(mComponents);
        if (DEBUG) Logger::D(TAG, "running: %p", object.Get());
        (*mServices)[i]->Start();

        if (mBootCompleted) {
            (*mServices)[i]->OnBootCompleted();
        }
    }
    mServicesStarted = TRUE;
    return NOERROR;
}

ECode CSystemUIApplication::OnConfigurationChanged(
    /* [in] */ IConfiguration* newConfig)
{
    if (mServicesStarted) {
        Int32 len = mServices->GetLength();
        for (Int32 i = 0; i < len; i++) {
            (*mServices)[i]->OnConfigurationChanged(newConfig);
        }
    }
    return NOERROR;
}

ECode CSystemUIApplication::GetComponent(
    /* [in] */ const String& interfaceType,
    /* [out] */ IInterface** obj)
{
    VALIDATE_NOT_NULL(obj);
    *obj = (*mComponents)[interfaceType];
    REFCOUNT_ADD(*obj);
    return NOERROR;
}

ECode CSystemUIApplication::GetServices(
    /* [out, callee] */ ArrayOf<ISystemUI*>** services)
{
    VALIDATE_NOT_NULL(services);
    *services = mServices;
    REFCOUNT_ADD(*services);
    return NOERROR;
}

} // namespace SystemUI
} // namespace Droid
} // namespace Elastos
