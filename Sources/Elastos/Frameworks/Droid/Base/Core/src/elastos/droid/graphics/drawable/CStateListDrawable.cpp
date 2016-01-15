
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Os.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/graphics/drawable/CStateListDrawable.h"

namespace Elastos {
namespace Droid {
namespace Graphics {
namespace Drawable {

CAR_OBJECT_IMPL(CStateListDrawable);
ECode CStateListDrawable::constructor()
{
    return StateListDrawable::constructor(NULL, NULL);
}

ECode CStateListDrawable::constructor(
    /* [in] */ IDrawableConstantState* state,
    /* [in] */ IResources* res)
{
    return StateListDrawable::constructor((StateListState*)state, res);
}

ECode CStateListDrawable::constructor(
    /* [in] */ IDrawableConstantState* state)
{
    return StateListDrawable::constructor((StateListState*)state);
}

} // namespace Drawable
} // namespace Graphics
} // namespace Droid
} // namespace Elastos
