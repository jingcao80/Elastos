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

#include "elastos/droid/server/appwidget/AppWidgetService.h"
#include "elastos/droid/server/appwidget/CAppWidgetServiceImpl.h"
#include <Elastos.Droid.Core.Server.h>

using Elastos::Droid::Server::AppWidget::CAppWidgetServiceImpl;
using Elastos::Droid::Server::CAppWidgetBackupBridge;
using Elastos::Droid::Server::IAppWidgetBackupBridge;
using Elastos::Droid::Server::IWidgetBackupProvider;
using Elastos::Droid::Server::ISystemService;

namespace Elastos {
namespace Droid {
namespace Server {
namespace AppWidget {

AppWidgetService::AppWidgetService()
{
}

ECode AppWidgetService::constructor(
    /* [in] */ IContext* context)
{
    VALIDATE_NOT_NULL(context);
    FAIL_RETURN(SystemService::constructor(context));
    CAppWidgetServiceImpl::New(context, (IIAppWidgetService**)&mImpl);
    return NOERROR;
}

// @Override
ECode AppWidgetService::OnStart()
{
    PublishBinderService(IContext::APPWIDGET_SERVICE, IBinder::Probe(mImpl));
    AutoPtr<IAppWidgetBackupBridge> bridge;
    CAppWidgetBackupBridge::AcquireSingleton((IAppWidgetBackupBridge**)&bridge);
    bridge->Register(IWidgetBackupProvider::Probe(mImpl));
    return NOERROR;
}

// @Override
ECode AppWidgetService::OnBootPhase(
    /* [in] */ Int32 phase)
{
    if (phase == ISystemService::PHASE_THIRD_PARTY_APPS_CAN_START) {
        ((CAppWidgetServiceImpl*)mImpl.Get())->SetSafeMode(FALSE/*IsSafeMode()*/);
    }
    return NOERROR;
}

} // namespace AppWidget
} // namespace Server
} // namespace Droid
} // namespace Elastos
