
#include "elastos/droid/view/CViewGroupOverlay.h"

namespace Elastos {
namespace Droid {
namespace View {

CAR_OBJECT_IMPL(CViewGroupOverlay);
CAR_INTERFACE_IMPL(CViewGroupOverlay, ViewOverlay, IViewGroupOverlay);
ECode CViewGroupOverlay::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IView* hostView)
{
    return ViewOverlay::constructor(context, hostView);
}

ECode CViewGroupOverlay::Add(
    /* [in] */ IView* view)
{
    return mOverlayViewGroup->Add(view);
}

ECode CViewGroupOverlay::Remove(
    /* [in] */ IView* view)
{
    return mOverlayViewGroup->Remove(view);
}

} // namespace View
} // namepsace Droid
} // namespace Elastos
