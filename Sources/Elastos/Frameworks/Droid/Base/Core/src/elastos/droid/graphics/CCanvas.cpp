
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Os.h"
#include "elastos/droid/graphics/CCanvas.h"

namespace Elastos {
namespace Droid {
namespace Graphics {

CAR_OBJECT_IMPL(CCanvas);
ECode CCanvas::constructor()
{
    return Canvas::constructor();
}

ECode CCanvas::constructor(
    /* [in] */ IBitmap* bitmap)
{
    return Canvas::constructor(bitmap);
}

ECode CCanvas::constructor(
    /* [in] */ Int32 nativeCanvas)
{
    return Canvas::constructor(nativeCanvas);
}

} // namespace Graphics
} // namepsace Droid
} // namespace Elastos
