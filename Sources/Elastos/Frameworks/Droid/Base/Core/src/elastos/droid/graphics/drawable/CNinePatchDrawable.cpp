
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Os.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/graphics/drawable/CNinePatchDrawable.h"

namespace Elastos {
namespace Droid {
namespace Graphics {
namespace Drawable {

CAR_OBJECT_IMPL(CNinePatchDrawable);
ECode CNinePatchDrawable::constructor()
{
    return NOERROR;
}

ECode CNinePatchDrawable::constructor(
    /* [in] */ IBitmap* bitmap,
    /* [in] */ ArrayOf<Byte>* chunk,
    /* [in] */ IRect* padding,
    /* [in] */ const String& srcName)
{
    return NinePatchDrawable::constructor(bitmap, chunk, padding, srcName);
}

ECode CNinePatchDrawable::constructor(
    /* [in] */ IResources* res,
    /* [in] */ IBitmap* bitmap,
    /* [in] */ ArrayOf<Byte>* chunk,
    /* [in] */ IRect* padding,
    /* [in] */ const String& srcName)
{
    return NinePatchDrawable::constructor(res, bitmap, chunk, padding, srcName);
}

ECode CNinePatchDrawable::constructor(
    /* [in] */ IResources* res,
    /* [in] */ IBitmap* bitmap,
    /* [in] */ ArrayOf<Byte>* chunk,
    /* [in] */ IRect* padding,
    /* [in] */ IRect* layoutInsets,
    /* [in] */ const String& srcName)
{
    return NinePatchDrawable::constructor(res, bitmap, chunk, padding, layoutInsets, srcName);
}

ECode CNinePatchDrawable::constructor(
    /* [in] */ INinePatch* patch)
{
    return NinePatchDrawable::constructor(patch);
}

ECode CNinePatchDrawable::constructor(
    /* [in] */ IResources* res,
    /* [in] */ INinePatch* patch)
{
    return NinePatchDrawable::constructor(res, patch);
}

ECode CNinePatchDrawable::constructor(
    /* [in] */ IDrawableConstantState* state,
    /* [in] */ IResources* res,
    /* [in] */ IResourcesTheme* theme)
{
    return NinePatchDrawable::constructor((NinePatchState*)state, res, theme);
}

} // namespace Drawable
} // namespace Graphics
} // namespace Droid
} // namespace Elastos
