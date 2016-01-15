
#include "elastos/droid/appwidget/CAppWidgetManagerHelper.h"
#include "elastos/droid/appwidget/AppWidgetManager.h"

namespace Elastos {
namespace Droid {
namespace AppWidget {

CAR_SINGLETON_IMPL(CAppWidgetManagerHelper);

CAR_INTERFACE_IMPL(CAppWidgetManagerHelper, Singleton, IAppWidgetManagerHelper);

ECode CAppWidgetManagerHelper::GetInstance(
    /* [in] */ IContext* context,
    /* [out] */ IAppWidgetManager** appWidgetManager)
{
    VALIDATE_NOT_NULL(appWidgetManager);
    AutoPtr<IAppWidgetManager> app = AppWidgetManager::GetInstance(context);
    *appWidgetManager = app;
    REFCOUNT_ADD(*appWidgetManager);
    return NOERROR;
}

} // namespace AppWidget
} // namespace Droid
} // namespace Elastos
