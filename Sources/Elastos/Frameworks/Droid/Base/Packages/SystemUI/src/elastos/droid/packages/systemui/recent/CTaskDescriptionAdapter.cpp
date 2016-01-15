#include "elastos/droid/systemui/recent/CTaskDescriptionAdapter.h"

using Elastos::Droid::View::EIID_View;
using Elastos::Droid::View::EIID_ViewGroup;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace Recent {

IADAPTER_METHODS_IMPL(CTaskDescriptionAdapter, TaskDescriptionAdapter)
ILISTADAPTER_METHODS_IMPL(CTaskDescriptionAdapter, TaskDescriptionAdapter)
IBASEADAPTER_METHODS_IMPL(CTaskDescriptionAdapter, TaskDescriptionAdapter)

ECode CTaskDescriptionAdapter::constructor(
    /* [in] */ IContext* ctx,
    /* [in] */ IRecentsPanelView* host)
{
    return TaskDescriptionAdapter::Init(ctx, host);
}

ECode CTaskDescriptionAdapter::CreateView(
    /* [in] */ IViewGroup* parent,
    /* [out] */ IView** v)
{
    VALIDATE_NOT_NULL(v);
    AutoPtr<IView> temp = TaskDescriptionAdapter::CreateView(parent);
    *v = temp;
    REFCOUNT_ADD(*v);
    return NOERROR;
}

ECode CTaskDescriptionAdapter::RecycleView(
    /* [in] */ IView* v)
{
    return TaskDescriptionAdapter::RecycleView(v);
}

}// namespace Recent
}// namespace SystemUI
}// namespace Droid
}// namespace Elastos
