
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Os.h"
#include "elastos/droid/graphics/drawable/CAnimatedStateListDrawable.h"

namespace Elastos {
namespace Droid {
namespace Graphics {
namespace Drawable {

CAR_OBJECT_IMPL(CAnimatedStateListDrawable);
ECode CAnimatedStateListDrawable::constructor()
{
    return AnimatedStateListDrawable::constructor(NULL, NULL);
}

ECode CAnimatedStateListDrawable::constructor(
    /* [in] */ /*@Nullable*/ IDrawableConstantState* state,
    /* [in] */ /*@Nullable*/ IResources* res)
{
    return AnimatedStateListDrawable::constructor((AnimatedStateListState*)state, res);
}

} // namespace Drawable
} // namespace Graphics
} // namespace Droid
} // namespace Elastos
