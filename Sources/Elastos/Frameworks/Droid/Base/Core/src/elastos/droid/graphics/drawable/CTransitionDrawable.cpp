
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Os.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/graphics/drawable/CTransitionDrawable.h"

namespace Elastos {
namespace Droid {
namespace Graphics {
namespace Drawable {

CAR_OBJECT_IMPL(CTransitionDrawable);
ECode CTransitionDrawable::constructor(
    /* [in] */ ArrayOf<IDrawable*>* layers)
{
    return TransitionDrawable::constructor(layers);
}

ECode CTransitionDrawable::constructor()
{
    return TransitionDrawable::constructor();
}

ECode CTransitionDrawable::constructor(
    /* [in] */ IDrawableConstantState* state,
    /* [in] */ IResources* res,
    /* [in] */ IResourcesTheme* theme)
{
    return TransitionDrawable::constructor((TransitionState*)state, res, theme);
}

ECode CTransitionDrawable::constructor(
    /* [in] */ IDrawableConstantState* state,
    /* [in] */ ArrayOf<IDrawable*>* layers)
{
    return TransitionDrawable::constructor((TransitionState*)state, layers);
}

} // namespace Drawable
} // namespace Graphics
} // namespace Droid
} // namespace Elastos
