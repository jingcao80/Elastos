
#include "elastos/droid/view/CGLES20CanvasHelper.h"
#include "elastos/droid/view/GLES20Canvas.h"

namespace Elastos {
namespace Droid {
namespace View {

CAR_INTERFACE_IMPL(CGLES20CanvasHelper, Singleton, IGLES20CanvasHelper)

CAR_SINGLETON_IMPL(CGLES20CanvasHelper)

ECode CGLES20CanvasHelper::SetProperty(
    /* [in] */ const String& name,
    /* [in] */ const String& value)
{
    return GLES20Canvas::SetProperty(name, value);
}

} // namespace View
} // namespace Droid
} // namespace Elastos
