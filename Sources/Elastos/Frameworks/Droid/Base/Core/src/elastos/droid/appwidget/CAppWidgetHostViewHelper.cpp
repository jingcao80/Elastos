
#include "elastos/droid/appwidget/CAppWidgetHostViewHelper.h"
#include "elastos/droid/appwidget/AppWidgetHostView.h"

namespace Elastos {
namespace Droid {
namespace AppWidget {

CAR_SINGLETON_IMPL(CAppWidgetHostViewHelper);

CAR_INTERFACE_IMPL(CAppWidgetHostViewHelper, Singleton, IAppWidgetHostViewHelper);

ECode CAppWidgetHostViewHelper::GetDefaultPaddingForWidget(
    /* [in] */ IContext* context,
    /* [in] */ IComponentName* component,
    /* [in] */ IRect* padding,
    /* [out] */ IRect** rect)
{
    VALIDATE_NOT_NULL(rect);
    AutoPtr<IRect> r = AppWidgetHostView::GetDefaultPaddingForWidget(context, component, padding);
    *rect = r;
    REFCOUNT_ADD(*rect);
    return NOERROR;
}

} // namespace AppWidget
} // namespace Droid
} // namespace Elastos
