
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Os.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/graphics/drawable/CLayerDrawable.h"

namespace Elastos {
namespace Droid {
namespace Graphics {
namespace Drawable {

CAR_OBJECT_IMPL(CLayerDrawable);
ECode CLayerDrawable::constructor()
{
    return LayerDrawable::constructor(NULL, NULL, NULL);
}
ECode CLayerDrawable::constructor(
    /* [in] */ ArrayOf<IDrawable*>* layers)
{
    return LayerDrawable::constructor(layers);
}

ECode CLayerDrawable::constructor(
    /* [in] */ ArrayOf<IDrawable*>* layers,
    /* [in] */ IDrawableConstantState* state)
{
    return LayerDrawable::constructor(layers, (LayerState*)state);
}

ECode CLayerDrawable::constructor(
    /* [in] */ IDrawableConstantState* state,
    /* [in] */ IResources* res,
    /* [in] */ IResourcesTheme* theme)
{
    return LayerDrawable::constructor((LayerState*)state, res, theme);
}

} // namespace Drawable
} // namespace Graphics
} // namespace Droid
} // namespace Elastos
