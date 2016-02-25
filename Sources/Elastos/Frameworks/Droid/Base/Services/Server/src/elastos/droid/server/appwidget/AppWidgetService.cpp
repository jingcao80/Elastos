
#include "elastos/droid/server/appwidget/AppWidgetService.h"
#include "elastos/droid/server/appwidget/CAppWidgetServiceImpl.h"
#include <Elastos.Droid.Core.Server.h>

using Elastos::Droid::Server::AppWidget::CAppWidgetServiceImpl;
using Elastos::Droid::Server::CAppWidgetBackupBridge;
using Elastos::Droid::Server::IAppWidgetBackupBridge;
using Elastos::Droid::Server::IWidgetBackupProvider;

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
    // SystemService::constructor(context);
    mImpl = new AppWidgetServiceImpl();
    mImpl->constructor(context);
    return NOERROR;
}

// @Override
ECode AppWidgetService::OnStart()
{
    //PublishBinderService(IContext::APPWIDGET_SERVICE, mImpl);
    AutoPtr<IAppWidgetBackupBridge> bridge;
    CAppWidgetBackupBridge::AcquireSingleton((IAppWidgetBackupBridge**)&bridge);
    bridge->Register(IWidgetBackupProvider::Probe(mImpl));
    return NOERROR;
}

// @Override
ECode AppWidgetService::OnBootPhase(
    /* [in] */ Int32 phase)
{
    if (phase == -1/*PHASE_THIRD_PARTY_APPS_CAN_START*/) {
        mImpl->SetSafeMode(FALSE/*IsSafeMode()*/);
    }
    return NOERROR;
}

} // namespace AppWidget
} // namespace Server
} // namespace Droid
} // namespace Elastos
