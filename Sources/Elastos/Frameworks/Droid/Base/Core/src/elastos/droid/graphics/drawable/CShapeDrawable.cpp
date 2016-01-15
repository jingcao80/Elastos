
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Os.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/graphics/drawable/CShapeDrawable.h"

namespace Elastos {
namespace Droid {
namespace Graphics {
namespace Drawable {

CAR_OBJECT_IMPL(CShapeDrawable);
ECode CShapeDrawable::constructor()
{
    return ShapeDrawable::constructor();
}

ECode CShapeDrawable::constructor(
    /* [in] */ IShape* s)
{
    return ShapeDrawable::constructor(s);
}

ECode CShapeDrawable::constructor(
    /* [in] */ IDrawableConstantState* state,
    /* [in] */ IResources* res,
    /* [in] */ IResourcesTheme* theme)
{
    return ShapeDrawable::constructor((ShapeState*)state, res, theme);
}

} // namespace Drawable
} // namespace Graphics
} // namespace Droid
} // namespace Elastos
