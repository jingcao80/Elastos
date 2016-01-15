
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Os.h"
#include "elastos/droid/graphics/drawable/CRippleDrawable.h"

namespace Elastos {
namespace Droid {
namespace Graphics {
namespace Drawable {

CAR_OBJECT_IMPL(CRippleDrawable);
ECode CRippleDrawable::constructor()
{
    return NOERROR;
}

ECode CRippleDrawable::constructor(
    /* [in] */ /*@NonNull*/ IColorStateList* color,
    /* [in] */ /*@Nullable*/ IDrawable* content,
    /* [in] */ /*@Nullable*/ IDrawable* mask)
{
    return RippleDrawable::constructor(color, content, mask);
}

ECode CRippleDrawable::constructor(
    /* [in] */ IDrawableConstantState* state,
    /* [in] */ IResources* res,
    /* [in] */ IResourcesTheme* theme)
{
    return RippleDrawable::constructor((RippleState*)state, res, theme);
}

} // namespace Drawable
} // namespace Graphics
} // namespace Droid
} // namespace Elastos
