
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Os.h"
#include "elastos/droid/graphics/drawable/CAnimatedVectorDrawable.h"

namespace Elastos {
namespace Droid {
namespace Graphics {
namespace Drawable {

CAR_OBJECT_IMPL(CAnimatedVectorDrawable);

ECode CAnimatedVectorDrawable::constructor()
{
    return AnimatedVectorDrawable::constructor();
}

ECode CAnimatedVectorDrawable::constructor(
    /* [in] */ IDrawableConstantState* state,
    /* [in] */ IResources* res,
    /* [in] */ IResourcesTheme* theme)
{
    return AnimatedVectorDrawable::constructor((AnimatedVectorDrawableState*)state, res, theme);
}


} // namespace Drawable
} // namespace Graphics
} // namespace Droid
} // namespace Elastos
