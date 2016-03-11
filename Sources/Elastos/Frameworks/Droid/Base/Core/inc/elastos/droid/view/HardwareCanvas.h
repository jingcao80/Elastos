#ifndef __ELASTOS_DROID_VIEW_HARDWARECANVAS_H__
#define __ELASTOS_DROID_VIEW_HARDWARECANVAS_H__

#include "Elastos.Droid.Graphics.h"
#include "Elastos.Droid.View.h"
#include "elastos/droid/graphics/Canvas.h"

using Elastos::Droid::Graphics::Canvas;
using Elastos::Droid::Graphics::IBitmap;

namespace Elastos {
namespace Droid {
namespace View {

class HardwareCanvas
    : public Canvas
    , public IHardwareCanvas
{
public:
    CAR_INTERFACE_DECL()

    CARAPI IsHardwareAccelerated(
        /* [out] */ Boolean* isAccelerated);

    CARAPI SetBitmap(
        /* [in] */ IBitmap* bitmap);

    /**
     * Draws the specified display list onto this canvas. The display list can only
     * be drawn if {@link android.view.RenderNode#isValid()} returns true.
     *
     * @param renderNode The RenderNode to replay.
     */
    CARAPI DrawRenderNode(
        /* [in] */ IRenderNode* renderNode);

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
    CARAPI CallDrawGLFunction(
        /* [in] */ Int64 drawGLFunction,
        /* [in] */ Int32* res);

    static CARAPI SetProperty(
        /* [in] */ const String& name,
        /* [in] */ const String& value);
};

} // namespace Elastos
} // namespace Droid
} // namespace View

#endif