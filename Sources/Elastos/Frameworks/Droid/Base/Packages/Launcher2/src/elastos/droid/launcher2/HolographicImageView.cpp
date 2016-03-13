
#include "elastos/droid/launcher2/HolographicImageView.h"
#include "Elastos.Droid.Service.h"
#include "R.h"

namespace Elastos {
namespace Droid {
namespace Launcher2 {

CAR_INTERFACE_IMPL(HolographicImageView, ImageView, IHolographicImageView);

ECode HolographicImageView::constructor(
    /* [in] */ IContext* context)
{
    return constructor(context, NULL);
}

ECode HolographicImageView::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    return constructor(context, attrs, 0);
}

ECode HolographicImageView::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ Int32 defStyle)
{
    ImageView::constructor(context, attrs, defStyle);

    mHolographicHelper = new HolographicViewHelper(context);
    return NOERROR;
}

ECode HolographicImageView::InvalidatePressedFocusedStates()
{
    return mHolographicHelper->InvalidatePressedFocusedStates(this);
}

ECode HolographicImageView::OnDraw(
    /* [in] */ ICanvas* canvas)
{
    ImageView::OnDraw(canvas);

    // One time call to generate the pressed/focused state -- must be called after
    // measure/layout
    return mHolographicHelper->GeneratePressedFocusedStates(this);
}

} // namespace Launcher2
} // namespace Droid
} // namespace Elastos