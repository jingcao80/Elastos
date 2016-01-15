
#include "Elastos.Droid.Content.h"
#include "elastos/droid/app/ActionBarLayoutParams.h"
#include "elastos/droid/R.h"

using Elastos::Droid::R;
using Elastos::Droid::Content::Res::ITypedArray;
using Elastos::Droid::View::IGravity;

namespace Elastos {
namespace Droid {
namespace App {

CAR_INTERFACE_IMPL(ActionBarLayoutParams, ViewGroup::MarginLayoutParams, IActionBarLayoutParams)

ActionBarLayoutParams::ActionBarLayoutParams()
    : mGravity(IGravity::NO_GRAVITY)
{}

ActionBarLayoutParams::~ActionBarLayoutParams()
{}

ECode ActionBarLayoutParams::constructor(
    /* [in] */ IContext* ctx,
    /* [in] */ IAttributeSet* attrs)
{
    MarginLayoutParams::constructor(ctx, attrs);

    AutoPtr<ArrayOf<Int32> > attrIds = ArrayOf<Int32>::Alloc(
        const_cast<Int32 *>(R::styleable::ActionBar_LayoutParams),
        ArraySize(R::styleable::ActionBar_LayoutParams));

    AutoPtr<ITypedArray> a;
    ctx->ObtainStyledAttributes(attrs, attrIds, (ITypedArray**)&a);
    a->GetInt32(R::styleable::ActionBar_LayoutParams_layout_gravity,
        IGravity::NO_GRAVITY, &mGravity);
    a->Recycle();
    return NOERROR;
}

ECode ActionBarLayoutParams::constructor(
    /* [in] */ Int32 width,
    /* [in] */ Int32 height)
{
    MarginLayoutParams::constructor(width, height);
    mGravity = IGravity::CENTER_VERTICAL | IGravity::START;
    return NOERROR;
}

ECode ActionBarLayoutParams::constructor(
    /* [in] */ Int32 width,
    /* [in] */ Int32 height,
    /* [in] */ Int32 gravity)
{
    MarginLayoutParams::constructor(width, height);
    mGravity = gravity;
    return NOERROR;
}

ECode ActionBarLayoutParams::constructor(
    /* [in] */ Int32 gravity)
{
    MarginLayoutParams::constructor(IViewGroupLayoutParams::WRAP_CONTENT,
        IViewGroupLayoutParams::MATCH_PARENT);
    mGravity = gravity;
    return NOERROR;
}

ECode ActionBarLayoutParams::constructor(
    /* [in] */ IActionBarLayoutParams* ablp)
{
    MarginLayoutParams::constructor(IViewGroupMarginLayoutParams::Probe(ablp));
    ablp->GetGravity(&mGravity);
    return NOERROR;
}

ECode ActionBarLayoutParams::constructor(
    /* [in] */ IViewGroupLayoutParams* vglp)
{
    MarginLayoutParams::constructor(vglp);
    return NOERROR;
}

ECode ActionBarLayoutParams::GetGravity(
    /* [out] */ Int32* gravity)
{
    VALIDATE_NOT_NULL(gravity)
    *gravity = mGravity;
    return NOERROR;
}

ECode ActionBarLayoutParams::SetGravity(
    /* [in] */ Int32 gravity)
{
    mGravity = gravity;
    return NOERROR;
}

} // namespace App
} // namespace Droid
} // namespace Elastos
