
#include "elastos/droid/view/ViewOverlay.h"
#include "elastos/droid/view/COverlayViewGroup.h"

namespace Elastos {
namespace Droid {
namespace View {

CAR_INTERFACE_IMPL(ViewOverlay, Object, IViewOverlay);
ECode ViewOverlay::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IView* hostView)
{
    COverlayViewGroup::New(context, hostView, (IOverlayViewGroup**)&mOverlayViewGroup);
    return NOERROR;
}

ECode ViewOverlay::GetOverlayView(
    /* [out] */ IViewGroup** group)
{
    VALIDATE_NOT_NULL(group);
    *group = IViewGroup::Probe(mOverlayViewGroup);
    REFCOUNT_ADD(*group);
    return NOERROR;
}

ECode ViewOverlay::Add(
    /* [in] */ IDrawable* drawable)
{
    return mOverlayViewGroup->Add(drawable);
}

ECode ViewOverlay::Remove(
    /* [in] */ IDrawable* drawable)
{
    return mOverlayViewGroup->Remove(drawable);
}

ECode ViewOverlay::Clear()
{
    return mOverlayViewGroup->Clear();
}

ECode ViewOverlay::IsEmpty(
    /* [out] */ Boolean* empyt)
{
    VALIDATE_NOT_NULL(empyt);
    return mOverlayViewGroup->IsEmpty(empyt);
}

} // namespace View
} // namepsace Droid
} // namespace Elastos
