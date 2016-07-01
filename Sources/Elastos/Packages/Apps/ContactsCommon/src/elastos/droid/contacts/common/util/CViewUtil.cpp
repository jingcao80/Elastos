
#include "ViewUtil.h"
#include "CViewUtil.h"

namespace Elastos{
namespace Apps{
namespace Contacts {
namespace Common {
namespace Util {

CAR_INTERFACE_IMPL(CViewUtil, Singleton, IViewUtil)

CAR_SINGLETON_IMPL(CViewUtil)

ECode CViewUtil::GetConstantPreLayoutWidth(
    /* [in] */ IView* view,
    /* [out] */ Int32* width)
{
    return ViewUtil::GetConstantPreLayoutWidth(view, width);
}

ECode CViewUtil::IsViewLayoutRtl(
    /* [in] */ IView* view,
    /* [out] */ Boolean* result)
{
    VALUE_NOT_NULL(result);
    *result = ViewUtil::IsViewLayoutRtl(view);
    return NOERROR;
}

ECode CViewUtil::AddRectangularOutlineProvider(
    /* [in] */ IView* view,
    /* [in] */ IResources* res)
{
    ViewUtil::AddRectangularOutlineProvider(view, res);
    return NOERROR;
}

ECode CViewUtil::SetupFloatingActionButton(
    /* [in] */ IView* view,
    /* [in] */ IResources* res)
{
    ViewUtil::SetupFloatingActionButton(view, res);
    return NOERROR;
}


ECode CViewUtil::AddBottomPaddingToListViewForFab(
    /* [in] */ IListView* listView,
    /* [in] */ IResources* res)
{
    ViewUtil::AddBottomPaddingToListViewForFab(listView, res);
    return NOERROR;
}

} // Util
} // Common
} // Contacts
} // Apps
} // Elastos
