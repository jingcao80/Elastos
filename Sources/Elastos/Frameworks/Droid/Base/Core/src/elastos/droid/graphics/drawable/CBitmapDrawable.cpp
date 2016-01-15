
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Os.h"
#include "elastos/droid/graphics/drawable/CBitmapDrawable.h"

namespace Elastos {
namespace Droid {
namespace Graphics {
namespace Drawable {

CAR_OBJECT_IMPL(CBitmapDrawable);
ECode CBitmapDrawable::constructor()
{
    return BitmapDrawable::constructor();
}

ECode CBitmapDrawable::constructor(
        /* [in] */ IResources* res)
{
    return BitmapDrawable::constructor(res);
}

ECode CBitmapDrawable::constructor(
        /* [in] */ IBitmap* bitmap)
{
    return BitmapDrawable::constructor(bitmap);
}

ECode CBitmapDrawable::constructor(
    /* [in] */ IResources* res,
    /* [in] */ IBitmap* bitmap)
{
    return BitmapDrawable::constructor(res, bitmap);
}

ECode CBitmapDrawable::constructor(
    /* [in] */ const String& filepath)
{
    return BitmapDrawable::constructor(filepath);
}

ECode CBitmapDrawable::constructor(
    /* [in] */ IResources* res,
    /* [in] */ const String& filepath)
{
    return BitmapDrawable::constructor(res, filepath);
}

ECode CBitmapDrawable::constructor(
    /* [in] */ IInputStream* is)
{
    return BitmapDrawable::constructor(is);
}

ECode CBitmapDrawable::constructor(
    /* [in] */ IResources* res,
    /* [in] */ IInputStream* is)
{
    return BitmapDrawable::constructor(res, is);
}

ECode CBitmapDrawable::constructor(
    /* [in] */ IDrawableConstantState* state,
    /* [in] */ IResources* res,
    /* [in] */ IResourcesTheme* theme)
{
    return BitmapDrawable::constructor((BitmapState*)state, res, theme);
}

} // namespace Drawable
} // namespace Graphics
} // namespace Droid
} // namespace Elastos
