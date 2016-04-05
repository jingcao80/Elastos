
#include "elastos/droid/launcher2/CHolographicImageView.h"
#include "Elastos.Droid.Service.h"
#include "R.h"

namespace Elastos {
namespace Droid {
namespace Launcher2 {

CAR_INTERFACE_IMPL(CHolographicImageView, ImageView, IHolographicImageView);

CAR_OBJECT_IMPL(CHolographicImageView);

ECode CHolographicImageView::constructor()
{
    return NOERROR;
}

ECode CHolographicImageView::constructor(
    /* [in] */ IContext* context)
{
    return constructor(context, NULL);
}

ECode CHolographicImageView::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    return constructor(context, attrs, 0);
}

ECode CHolographicImageView::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ Int32 defStyle)
{
    ImageView::constructor(context, attrs, defStyle);

    mHolographicHelper = new HolographicViewHelper(context);
    return NOERROR;
}

ECode CHolographicImageView::InvalidatePressedFocusedStates()
{
    mHolographicHelper->InvalidatePressedFocusedStates(this);
    return NOERROR;
}

void CHolographicImageView::OnDraw(
    /* [in] */ ICanvas* canvas)
{
    ImageView::OnDraw(canvas);

    // One time call to generate the pressed/focused state -- must be called after
    // measure/layout
    mHolographicHelper->GeneratePressedFocusedStates(this);
}

} // namespace Launcher2
} // namespace Droid
} // namespace Elastos