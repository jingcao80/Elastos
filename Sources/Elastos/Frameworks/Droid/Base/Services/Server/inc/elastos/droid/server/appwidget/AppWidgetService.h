
#ifndef __ELASTOS_DROID_SERVER_APPWIDGET_APPWIDGETSERVICE_H__
#define __ELASTOS_DROID_SERVER_APPWIDGET_APPWIDGETSERVICE_H__

#include <_Elastos.Droid.Server.h>
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/server/SystemService.h"
#include "elastos/droid/server/appwidget/AppWidgetServiceImpl.h"
#include <Elastos.Droid.Content.h>
#include "elastos/core/Object.h"

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Server::SystemService;
using Elastos::Droid::Server::AppWidget::AppWidgetServiceImpl;
using Elastos::Core::Object;

namespace Elastos {
namespace Droid {
namespace Server {
namespace AppWidget {

/**
 * SystemService that publishes an IAppWidgetService.
 */
class AppWidgetService
    : public SystemService
{
public:
    AppWidgetService();

    CARAPI constructor(
        /* [in] */ IContext* context);

    // @Override
    CARAPI OnStart();

    // @Override
    CARAPI OnBootPhase(
        /* [in] */ Int32 phase);

private:
    AutoPtr<IIAppWidgetService> mImpl;
};

} // namespace AppWidget
} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SERVER_APPWIDGET_APPWIDGETSERVICE_H__
