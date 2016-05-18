
#include "elastos/droid/systemui/CSystemUIService.h"

using Elastos::Droid::App::IApplication;

namespace Elastos {
namespace Droid {
namespace SystemUI {

CAR_OBJECT_IMPL(CSystemUIService)
CAR_INTERFACE_IMPL(CSystemUIService, Service, ISystemUIService)
ECode CSystemUIService::OnCreate()
{
    Service::OnCreate();
    AutoPtr<IApplication> app;
    GetApplication((IApplication**)&app);
    ISystemUIApplication::Probe(app)->StartServicesIfNeeded();

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

ECode CSystemUIService::Dump(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ IPrintWriter* pw,
    /* [in] */ ArrayOf<String>* args)
{
    AutoPtr<IApplication> app;
    GetApplication((IApplication**)&app);
    AutoPtr<ArrayOf<ISystemUI*> > services;
    ISystemUIApplication::Probe(app)->GetServices((ArrayOf<ISystemUI*>**)&services);
    assert(0 && "TODO");
    // if (args == NULL || args->GetLength() == 0) {
    //     for (Int32 i = 0; i < services->GetLength(); i++) {
    //         AutoPtr<ISystemUI> ui = (*services)[i];
    //         pw->Println("dumping service: " + ui.getClass().getName());
    //         ui->Dump(fd, pw, args);
    //     }
    // }
    // else {
    //     String svc = args[0];
    //     for (SystemUI ui: services) {
    //         String name = ui.getClass().getName();
    //         if (name.endsWith(svc)) {
    //             ui.dump(fd, pw, args);
    //         }
    //     }
    // }
    return NOERROR;
}

} // SystemUI
} // Droid
} // Elastos
