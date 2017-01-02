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
