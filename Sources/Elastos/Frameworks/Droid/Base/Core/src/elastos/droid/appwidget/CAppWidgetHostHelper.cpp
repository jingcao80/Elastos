
#include "elastos/droid/appwidget/CAppWidgetHostHelper.h"
#include "elastos/droid/appwidget/AppWidgetHost.h"

namespace Elastos {
namespace Droid {
namespace AppWidget {

CAR_SINGLETON_IMPL(CAppWidgetHostHelper);

CAR_INTERFACE_IMPL(CAppWidgetHostHelper, Singleton, IAppWidgetHostHelper);

ECode CAppWidgetHostHelper::DeleteAllHosts()
{
    return AppWidgetHost::DeleteAllHosts();
}

} // namespace AppWidget
} // namespace Droid
} // namespace Elastos
