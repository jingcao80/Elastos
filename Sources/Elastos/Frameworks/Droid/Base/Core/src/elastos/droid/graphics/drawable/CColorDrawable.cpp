
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Os.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/graphics/drawable/CColorDrawable.h"

namespace Elastos {
namespace Droid {
namespace Graphics {
namespace Drawable {

CAR_OBJECT_IMPL(CColorDrawable);
ECode CColorDrawable::constructor()
{
    return ColorDrawable::constructor();
}

ECode CColorDrawable::constructor(
    /* [in] */ Int32 color)
{
    return ColorDrawable::constructor(color);
}

ECode CColorDrawable::constructor(
    /* [in] */ IDrawableConstantState* state,
    /* [in] */ IResources* res,
    /* [in] */ IResourcesTheme* theme)
{
    return ColorDrawable::constructor(state, res, theme);
}

} // namespace Drawable
} // namespace Graphics
} // namespace Droid
} // namespace Elastos
