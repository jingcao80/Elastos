#ifndef __ELASTOS_DROID_VIEW_HARDWARELAYER_H__
#define __ELASTOS_DROID_VIEW_HARDWARELAYER_H__

#include "elastos/droid/ext/frameworkext.h"
#include "Elastos.Droid.Internal.h"
#include "Elastos.Droid.Graphics.h"
#include "Elastos.Droid.View.h"

#include <elastos/core/Object.h>

using Elastos::Droid::Internal::Utility::IVirtualRefBasePtr;
using Elastos::Droid::Graphics::IBitmap;
using Elastos::Droid::Graphics::IMatrix;
using Elastos::Droid::Graphics::IPaint;
using Elastos::Droid::Graphics::ISurfaceTexture;
using Elastos::Droid::View::IHardwareLayer;

namespace Elastos {
namespace Droid {
namespace View {

class HardwareLayer
    : public Object
    , public IHardwareLayer
{
public:
    CAR_INTERFACE_DECL()

    /**
     * Update the paint used when drawing this layer.
     *
     * @param paint The paint used when the layer is drawn into the destination canvas.
     * @see View#setLayerPaint(android.graphics.Paint)
     */
    CARAPI SetLayerPaint(
        /* [in] */ IPaint* paint);

    /**
     * Indicates whether this layer can be rendered.
     *
     * @return True if the layer can be rendered into, false otherwise
     */
    CARAPI IsValid(
        /* [out] */ Boolean* valid);

    /**
     * Destroys resources without waiting for a GC.
     */
    CARAPI Destory();

    CARAPI GetDeferredLayerUpdater(
        /* [out] */ Int64* result);

    /**
     * Copies this layer into the specified bitmap.
     *
     * @param bitmap The bitmap to copy they layer into
     *
     * @return True if the copy was successful, false otherwise
     */
    CARAPI CopyInto(
        /* [in] */ IBitmap* bitmap,
        /* [out] */ Boolean* result);

    /**
     * Update the layer's properties. Note that after calling this isValid() may
     * return false if the requested width/height cannot be satisfied
     *
     * @param width The new width of this layer
     * @param height The new height of this layer
     * @param isOpaque Whether this layer is opaque
     *
     * @return true if the layer's properties will change, false if they already
     *         match the desired values.
     */
    CARAPI Prepare(
        /* [in] */ Int32 width,
        /* [in] */ Int32 height,
        /* [in] */ Boolean isOpaque,
        /* [out] */ Boolean* result);

    /**
     * Sets an optional transform on this layer.
     *
     * @param matrix The transform to apply to the layer.
     */
    CARAPI SetTransform(
        /* [in] */ IMatrix* matrix);

    /**
     * Indicates that this layer has lost its texture.
     */
    CARAPI DetachSurfaceTexture();

    CARAPI GetLayer(
        /* [out] */ Int64* result);

    CARAPI SetSurfaceTexture(
        /* [in] */ ISurfaceTexture* surface);

    CARAPI UpdateSurfaceTexture();

    static CARAPI_(AutoPtr<IHardwareLayer>) AdoptTextureLayer(
        /* [in] */ IHardwareRenderer* renderer,
        /* [in] */ Int64 deferredUpdater);

private:
    HardwareLayer(
        /* [in] */ IHardwareRenderer* renderer,
        /* [in] */ Int64 deferredUpdater);

    CARAPI nPrepare(
        /* [in] */ Int64 layerUpdater,
        /* [in] */ Int32 width,
        /* [in] */ Int32 height,
        /* [in] */ Boolean isOpaque,
        /* [out] */ Boolean* res);

    CARAPI nSetLayerPaint(
        /* [in] */ Int64 layerUpdater,
        /* [in] */ Int64 paint);

    CARAPI nSetTransform(
        /* [in] */ Int64 layerUpdater,
        /* [in] */ Int64 matrix);

    CARAPI nSetSurfaceTexture(
        /* [in] */ Int64 layerUpdater,
        /* [in] */ ISurfaceTexture* surface,
        /* [in] */ Boolean isAlreadyAttached);

    CARAPI nUpdateSurfaceTexture(
        /* [in] */ Int64 layerUpdater);

    CARAPI nUpdateRenderLayer(
        /* [in] */ Int64 layerUpdater,
        /* [in] */ Int64 displayList,
        /* [in] */ Int32 left,
        /* [in] */ Int32 top,
        /* [in] */ Int32 right,
        /* [in] */ Int32 bottom);


    CARAPI nGetLayer(
        /* [in] */ Int64 layerUpdater,
        /* [in] */ Int64* res);

    CARAPI nGetTexName(
        /* [in] */ Int64 layerUpdater,
        /* [in] */ Int32* res);

    AutoPtr<IHardwareRenderer> mRenderer;
    AutoPtr<IVirtualRefBasePtr> mFinalizer;
};

} // namespace View
} // namespace Droid
} // namespace Elastos

#endif