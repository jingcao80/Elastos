
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Os.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/graphics/drawable/CPictureDrawable.h"

namespace Elastos {
namespace Droid {
namespace Graphics {
namespace Drawable {

CAR_OBJECT_IMPL(CPictureDrawable);
ECode CPictureDrawable::constructor(
    /* [in] */ IPicture* picture)
{
    return PictureDrawable::constructor(picture);
}

} // namespace Drawable
} // namespace Graphics
} // namespace Droid
} // namespace Elastos
