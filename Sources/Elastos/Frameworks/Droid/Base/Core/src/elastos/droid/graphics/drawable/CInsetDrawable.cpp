
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Os.h"
#include "elastos/droid/graphics/drawable/CInsetDrawable.h"

namespace Elastos {
namespace Droid {
namespace Graphics {
namespace Drawable {

CAR_OBJECT_IMPL(CInsetDrawable);
ECode CInsetDrawable::constructor()
{
    return InsetDrawable::constructor();
}

ECode CInsetDrawable::constructor(
    /* [in] */ IDrawable* drawable,
    /* [in] */ Int32 inset)
{
    return InsetDrawable::constructor(drawable, inset);
}

ECode CInsetDrawable::constructor(
    /* [in] */ IDrawable* drawable,
    /* [in] */ Int32 insetLeft,
    /* [in] */ Int32 insetTop,
    /* [in] */ Int32 insetRight,
    /* [in] */ Int32 insetBottom)
{
    return InsetDrawable::constructor(
            drawable, insetLeft, insetTop, insetRight, insetBottom);
}

ECode CInsetDrawable::constructor(
    /* [in] */ IDrawableConstantState* state,
    /* [in] */ IResources* res)
{
    return InsetDrawable::constructor((InsetState*)state, res);
}

} // namespace Drawable
} // namespace Graphics
} // namespace Droid
} // namespace Elastos
