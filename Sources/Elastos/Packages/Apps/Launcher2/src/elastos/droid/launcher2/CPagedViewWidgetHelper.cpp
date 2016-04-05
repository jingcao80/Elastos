
#include "elastos/droid/launcher2/CPagedViewWidgetHelper.h"
#include "elastos/droid/launcher2/PagedViewWidget.h"

namespace Elastos {
namespace Droid {
namespace Launcher2 {

CAR_SINGLETON_IMPL(CPagedViewWidgetHelper)

CAR_INTERFACE_IMPL(CPagedViewWidgetHelper, Singleton, IPagedViewWidgetHelper)

ECode CPagedViewWidgetHelper::SetDeletePreviewsWhenDetachedFromWindow(
    /* [in] */ Boolean value)
{
    return PagedViewWidget::SetDeletePreviewsWhenDetachedFromWindow(value);
}

ECode CPagedViewWidgetHelper::SetRecyclePreviewsWhenDetachedFromWindow(
    /* [in] */ Boolean value)
{
    return PagedViewWidget::SetRecyclePreviewsWhenDetachedFromWindow(value);
}

ECode CPagedViewWidgetHelper::ResetShortPressTarget()
{
    return PagedViewWidget::ResetShortPressTarget();
}

} // namespace Launcher2
} // namespace Droid
} // namespace Elastos