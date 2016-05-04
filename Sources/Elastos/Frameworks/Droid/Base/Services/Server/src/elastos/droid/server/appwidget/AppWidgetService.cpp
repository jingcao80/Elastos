
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
