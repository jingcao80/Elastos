#include "elastos/droid/view/HardwareCanvas.h"
#include "elastos/droid/view/GLES20Canvas.h"

namespace Elastos {
namespace Droid {
namespace View {

CAR_INTERFACE_IMPL(HardwareCanvas, Canvas, IHardwareCanvas)

ECode HardwareCanvas::constructor()
{
    return Canvas::constructor();
}

ECode HardwareCanvas::IsHardwareAccelerated(
    /* [out] */ Boolean* isAccelerated)
{
    VALIDATE_NOT_NULL(isAccelerated)
    *isAccelerated = TRUE;
    return NOERROR;
}

ECode HardwareCanvas::SetBitmap(
    /* [in] */ IBitmap* bitmap)
{
    return E_UNSUPPORTED_OPERATION_EXCEPTION;
}

/**
 * Draws the specified display list onto this canvas. The display list can only
 * be drawn if {@link android.view.RenderNode#isValid()} returns true.
 *
 * @param renderNode The RenderNode to replay.
 */
ECode HardwareCanvas::DrawRenderNode(
    /* [in] */ IRenderNode* renderNode)
{
    return DrawRenderNode(renderNode, NULL, IRenderNode::FLAG_CLIP_CHILDREN);
}

/**
 * Calls the function specified with the drawGLFunction function pointer. This is
 * functionality used by webkit for calling into their renderer from our display lists.
 * This function may return true if an invalidation is needed after the call.
 *
 * @param drawGLFunction A native function pointer
 *
 * @return {@link RenderNode#STATUS_DONE}
 *
 * @hide
 */
ECode HardwareCanvas::CallDrawGLFunction(
    /* [in] */ Int64 drawGLFunction,
    /* [out] */ Int32* res)
{
    VALIDATE_NOT_NULL(res)
    *res = IRenderNode::STATUS_DONE;
    return NOERROR;
}

ECode HardwareCanvas::SetProperty(
    /* [in] */ const String& name,
    /* [in] */ const String& value)
{
    GLES20Canvas::SetProperty(name, value);
    return NOERROR;
}

} // namespace Elastos
} // namespace Droid
} // namespace View
