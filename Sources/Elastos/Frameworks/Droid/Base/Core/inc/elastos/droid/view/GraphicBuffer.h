#ifndef __ELASTOS_DROID_VIEW_GRAPHICBUFFER_H__
#define __ELASTOS_DROID_VIEW_GRAPHICBUFFER_H__

#include "Elastos.Droid.Graphics.h"
#include "Elastos.Droid.View.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Graphics::IRect;
using Elastos::Droid::Graphics::ICanvas;

namespace Elastos {
namespace Droid {
namespace View {

class GraphicBuffer
    : public Object
    , public IGraphicBuffer
    , public IParcelable
{
public:
    CAR_INTERFACE_DECL()

    GraphicBuffer();

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ Int32 width,
        /* [in] */ Int32 height,
        /* [in] */ Int32 format,
        /* [in] */ Int32 usage,
        /* [in] */ Int64 nativeObject);

    CARAPI ReadFromParcel(
        /* [in] */ IParcel* source);

    CARAPI WriteToParcel(
        /* [in] */ IParcel* dest);

    /**
     * Returns the width of this buffer in pixels.
     */
    CARAPI GetWidth(
        /* [out] */ Int32* width);

    /**
     * Returns the height of this buffer in pixels.
     */
    CARAPI GetHeight(
        /* [out] */ Int32* height);

    /**
     * Returns the pixel format of this buffer. The pixel format must be one of
     * the formats defined in {@link PixelFormat}.
     */
    CARAPI GetFormat(
        /* [out] */ Int32* format);

    /**
     * Returns the usage hint set on this buffer.
     */
    CARAPI GetUsage(
        /* [out] */ Int32* usage);

    /**
     * <p>Start editing the pixels in the buffer. A null is returned if the buffer
     * cannot be locked for editing.</p>
     *
     * <p>The content of the buffer is preserved between unlockCanvas()
     * and lockCanvas().</p>
     *
     * <p>If this method is called after {@link #destroy()}, the return value will
     * always be null.</p>
     *
     * @return A Canvas used to draw into the buffer, or null.
     *
     * @see #lockCanvas(android.graphics.Rect)
     * @see #unlockCanvasAndPost(android.graphics.Canvas)
     * @see #isDestroyed()
     */
    CARAPI LockCanvas(
        /* [out] */ ICanvas** canvas);

    /**
     * Just like {@link #lockCanvas()} but allows specification of a dirty
     * rectangle.
     *
     * <p>If this method is called after {@link #destroy()}, the return value will
     * always be null.</p>
     *
     * @param dirty Area of the buffer that may be modified.

     * @return A Canvas used to draw into the surface, or null.
     *
     * @see #lockCanvas()
     * @see #unlockCanvasAndPost(android.graphics.Canvas)
     * @see #isDestroyed()
     */
    CARAPI LockCanvas(
        /* [in] */ IRect* dirty,
        /* [out] */ ICanvas** canvas);

    /**
     * Finish editing pixels in the buffer.
     *
     * <p>This method doesn't do anything if {@link #destroy()} was
     * previously called.</p>
     *
     * @param canvas The Canvas previously returned by lockCanvas()
     *
     * @see #lockCanvas()
     * @see #lockCanvas(android.graphics.Rect)
     * @see #isDestroyed()
     */
    CARAPI UnlockCanvasAndPost(
        /* [in] */ ICanvas* canvas);

    /**
     * Destroyes this buffer immediately. Calling this method frees up any
     * underlying resources. After calling this method, this buffer
     * must not be used in any way ({@link #lockCanvas()} must not be called,
     * etc.)
     *
     * @see #isDestroyed()
     */
    CARAPI Destroy();

    /**
     * Indicates whether this buffer has been destroyed. A destroyed buffer
     * cannot be used in any way: locking a Canvas will return null, the buffer
     * cannot be written to a parcel, etc.
     *
     * @return True if this <code>GraphicBuffer</code> is in a destroyed state,
     *         false otherwise.
     *
     * @see #destroy()
     */
    CARAPI IsDestroyed(
        /* [out] */ Boolean* isDestroyed);

public:
    static CARAPI Create(
        /* [in] */ Int32 width,
        /* [in] */ Int32 height,
        /* [in] */ Int32 format,
        /* [in] */ Int32 usage,
        /* [out] */ IGraphicBuffer** buf);

private:
    static CARAPI_(Int64) nCreateGraphicBuffer(
        /* [in] */ Int32 width,
        /* [in] */ Int32 height,
        /* [in] */ Int32 format,
        /* [in] */ Int32 usage);

    static CARAPI_(void) nDestroyGraphicBuffer(
        /* [in] */ Int64 nativeObject);

    static CARAPI nWriteGraphicBufferToParcel(
        /* [in] */ Int64 nativeObject,
        /* [in] */ IParcel* dest);

    static CARAPI nReadGraphicBufferFromParcel(
        /* [in] */ IParcel* in,
        /* [out] */ Int64* nativeObject);

    static CARAPI_(Boolean) nLockCanvas(
        /* [in] */ Int64 nativeObject,
        /* [in] */ ICanvas* canvas,
        /* [in] */ IRect* dirty);

    static CARAPI_(Boolean) nUnlockCanvasAndPost(
        /* [in] */ Int64 nativeObject,
        /* [in] */ ICanvas* canvas);

private:
    Int32 mWidth;
    Int32 mHeight;
    Int32 mFormat;
    Int32 mUsage;
    // Note: do not rename, this field is used by code
    Int64 mNativeObject;

    // These two fields are only used by lock/unlockCanvas()
    AutoPtr<ICanvas> mCanvas;
    Int32 mSaveCount;

    // If set to true, this GraphicBuffer instance cannot be used anymore
    Boolean mDestroyed;

    Boolean mInit;
};

} // namespace View
} // namespace Droid
} // namespace Elastos

#endif
