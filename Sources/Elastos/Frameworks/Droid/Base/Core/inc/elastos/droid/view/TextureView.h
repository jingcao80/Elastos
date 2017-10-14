//=========================================================================
// Copyright (C) 2012 The Elastos Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//=========================================================================

#ifndef __ELASTOS_DROID_VIEW_TEXTUREVIEW_H__
#define __ELASTOS_DROID_VIEW_TEXTUREVIEW_H__

#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/view/View.h"

using Elastos::Droid::Graphics::IOnFrameAvailableListener;
using Elastos::Droid::Graphics::ISurfaceTexture;

namespace Elastos {
namespace Droid {
namespace View {

class ECO_PUBLIC TextureView
    : public View
    , public ITextureView
{
private:
    class ECO_LOCAL OnFrameAvailableListener
        : public Object
        , public IOnFrameAvailableListener
    {
    public:
        CAR_INTERFACE_DECL()

        OnFrameAvailableListener(
            /* [in] */ TextureView* h);

        CARAPI OnFrameAvailable(
            /* [in] */ ISurfaceTexture* surfaceTexture);

    public:
        TextureView* mHost;
    };

public:
    CAR_INTERFACE_DECL()

    TextureView();

    /**
     * Creates a new TextureView.
     *
     * @param context The context to associate this view with.
     */
    CARAPI constructor(
        /* [in] */ IContext* context);

    /**
     * Creates a new TextureView.
     *
     * @param context The context to associate this view with.
     * @param attrs The attributes of the XML tag that is inflating the view.
     */
    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs);

    /**
     * Creates a new TextureView.
     *
     * @param context The context to associate this view with.
     * @param attrs The attributes of the XML tag that is inflating the view.
     * @param defStyleAttr An attribute in the current theme that contains a
     *        reference to a style resource that supplies default values for
     *        the view. Can be 0 to not look for defaults.
     */
    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs,
        /* [in] */ Int32 defStyleAttr);

    /**
     * Creates a new TextureView.
     *
     * @param context The context to associate this view with.
     * @param attrs The attributes of the XML tag that is inflating the view.
     * @param defStyleAttr An attribute in the current theme that contains a
     *        reference to a style resource that supplies default values for
     *        the view. Can be 0 to not look for defaults.
     * @param defStyleRes A resource identifier of a style resource that
     *        supplies default values for the view, used only if
     *        defStyleAttr is 0 or can not be found in the theme. Can be 0
     *        to not look for defaults.
     */
    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs,
        /* [in] */ Int32 defStyleAttr,
        /* [in] */ Int32 defStyleRes);

    /**
     * {@inheritDoc}
     */
    CARAPI IsOpaque(
        /* [out] */ Boolean* opaque);

    /**
     * Indicates whether the content of this TextureView is opaque. The
     * content is assumed to be opaque by default.
     *
     * @param opaque True if the content of this TextureView is opaque,
     *               false otherwise
     */
    CARAPI SetOpaque(
        /* [in] */ Boolean opaque);

    /**
     * The layer type of a TextureView is ignored since a TextureView is always
     * considered to act as a hardware layer. The optional paint supplied to this
     * method will however be taken into account when rendering the content of
     * this TextureView.
     *
     * @param layerType The ype of layer to use with this view, must be one of
     *        {@link #LAYER_TYPE_NONE}, {@link #LAYER_TYPE_SOFTWARE} or
     *        {@link #LAYER_TYPE_HARDWARE}
     * @param paint The paint used to compose the layer. This argument is optional
     *        and can be null. It is ignored when the layer type is
     *        {@link #LAYER_TYPE_NONE}
     */
    CARAPI SetLayerType(
        /* [in] */ Int32 layerType,
        /* [in] */ IPaint* paint);

    CARAPI SetLayerPaint(
        /* [in] */ IPaint* paint);

    /**
     * Always returns {@link #LAYER_TYPE_HARDWARE}.
     */
    CARAPI GetLayerType(
        /* [out] */ Int32* type);

    CARAPI_(Boolean) HasStaticLayer();

    /**
     * Calling this method has no effect.
     */
    CARAPI BuildLayer();

    /**
     * Subclasses of TextureView cannot do their own rendering
     * with the {@link Canvas} object.
     *
     * @param canvas The Canvas to which the View is rendered.
     */
    CARAPI Draw(
        /* [in] */ ICanvas* canvas);

    /**
     * <p>Sets the transform to associate with this texture view.
     * The specified transform applies to the underlying surface
     * texture and does not affect the size or position of the view
     * itself, only of its content.</p>
     *
     * <p>Some transforms might prevent the content from drawing
     * all the pixels contained within this view's bounds. In such
     * situations, make sure this texture view is not marked opaque.</p>
     *
     * @param transform The transform to apply to the content of
     *        this view.
     *
     * @see #getTransform(android.graphics.Matrix)
     * @see #isOpaque()
     * @see #setOpaque(boolean)
     */
    CARAPI SetTransform(
        /* [in] */ IMatrix* transform);

    /**
     * Returns the transform associated with this texture view.
     *
     * @param transform The {@link Matrix} in which to copy the current
     *        transform. Can be null.
     *
     * @return The specified matrix if not null or a new {@link Matrix}
     *         instance otherwise.
     *
     * @see #setTransform(android.graphics.Matrix)
     */
    CARAPI GetTransform(
        /* [in] */ IMatrix* transform,
        /* [out] */ IMatrix** result);

    /**
     * <p>Returns a {@link android.graphics.Bitmap} representation of the content
     * of the associated surface texture. If the surface texture is not available,
     * this method returns null.</p>
     *
     * <p>The bitmap returned by this method uses the {@link Bitmap.Config#ARGB_8888}
     * pixel format and its dimensions are the same as this view's.</p>
     *
     * <p><strong>Do not</strong> invoke this method from a drawing method
     * ({@link #onDraw(android.graphics.Canvas)} for instance).</p>
     *
     * <p>If an error occurs during the copy, an empty bitmap will be returned.</p>
     *
     * @return A valid {@link Bitmap.Config#ARGB_8888} bitmap, or null if the surface
     *         texture is not available or the width &lt;= 0 or the height &lt;= 0
     *
     * @see #isAvailable()
     * @see #getBitmap(android.graphics.Bitmap)
     * @see #getBitmap(Int32, int)
     */
    CARAPI GetBitmap(
        /* [out] */ IBitmap** result);

    /**
     * <p>Returns a {@link android.graphics.Bitmap} representation of the content
     * of the associated surface texture. If the surface texture is not available,
     * this method returns null.</p>
     *
     * <p>The bitmap returned by this method uses the {@link Bitmap.Config#ARGB_8888}
     * pixel format.</p>
     *
     * <p><strong>Do not</strong> invoke this method from a drawing method
     * ({@link #onDraw(android.graphics.Canvas)} for instance).</p>
     *
     * <p>If an error occurs during the copy, an empty bitmap will be returned.</p>
     *
     * @param width The width of the bitmap to create
     * @param height The height of the bitmap to create
     *
     * @return A valid {@link Bitmap.Config#ARGB_8888} bitmap, or null if the surface
     *         texture is not available or width is &lt;= 0 or height is &lt;= 0
     *
     * @see #isAvailable()
     * @see #getBitmap(android.graphics.Bitmap)
     * @see #getBitmap()
     */
    CARAPI GetBitmap(
        /* [in] */ Int32 width,
        /* [in] */ Int32 height,
        /* [out] */ IBitmap** result);

    /**
     * <p>Copies the content of this view's surface texture into the specified
     * bitmap. If the surface texture is not available, the copy is not executed.
     * The content of the surface texture will be scaled to fit exactly inside
     * the specified bitmap.</p>
     *
     * <p><strong>Do not</strong> invoke this method from a drawing method
     * ({@link #onDraw(android.graphics.Canvas)} for instance).</p>
     *
     * <p>If an error occurs, the bitmap is left unchanged.</p>
     *
     * @param bitmap The bitmap to copy the content of the surface texture into,
     *               cannot be null, all configurations are supported
     *
     * @return The bitmap specified as a parameter
     *
     * @see #isAvailable()
     * @see #getBitmap(int, int)
     * @see #getBitmap()
     *
     * @throws IllegalStateException if the hardware rendering context cannot be
     *         acquired to capture the bitmap
     */
    CARAPI GetBitmap(
        /* [in] */ IBitmap* bitmap,
        /* [out] */ IBitmap** result);

    /**
     * Returns true if the {@link SurfaceTexture} associated with this
     * TextureView is available for rendering. When this method returns
     * true, {@link #getSurfaceTexture()} returns a valid surface texture.
     */
    CARAPI IsAvailable(
        /* [out] */ Boolean* result);

    /**
     * <p>Start editing the pixels in the surface.  The returned Canvas can be used
     * to draw into the surface's bitmap.  A null is returned if the surface has
     * not been created or otherwise cannot be edited. You will usually need
     * to implement
     * {@link SurfaceTextureListener#onSurfaceTextureAvailable(android.graphics.SurfaceTexture, int, int)}
     * to find out when the Surface is available for use.</p>
     *
     * <p>The content of the Surface is never preserved between unlockCanvas()
     * and lockCanvas(), for this reason, every pixel within the Surface area
     * must be written. The only exception to this rule is when a dirty
     * rectangle is specified, in which case, non-dirty pixels will be
     * preserved.</p>
     *
     * <p>This method can only be used if the underlying surface is not already
     * owned by another producer. For instance, if the TextureView is being used
     * to render the camera's preview you cannot invoke this method.</p>
     *
     * @return A Canvas used to draw into the surface.
     *
     * @see #lockCanvas(android.graphics.Rect)
     * @see #unlockCanvasAndPost(android.graphics.Canvas)
     */
    CARAPI LockCanvas(
        /* [out] */ ICanvas** canvas);

    /**
     * Just like {@link #lockCanvas()} but allows specification of a dirty
     * rectangle. Every pixel within that rectangle must be written; however
     * pixels outside the dirty rectangle will be preserved by the next call
     * to lockCanvas().
     *
     * This method can return null if the underlying surface texture is not
     * available (see {@link #isAvailable()} or if the surface texture is
     * already connected to an image producer (for instance: the camera,
     * OpenGL, a media player, etc.)
     *
     * @param dirty Area of the surface that will be modified.

     * @return A Canvas used to draw into the surface.
     *
     * @see #lockCanvas()
     * @see #unlockCanvasAndPost(android.graphics.Canvas)
     * @see #isAvailable()
     */
    CARAPI LockCanvas(
        /* [in] */ IRect* dirty,
        /* [out] */ ICanvas** canvas);

    /**
     * Finish editing pixels in the surface. After this call, the surface's
     * current pixels will be shown on the screen, but its content is lost,
     * in particular there is no guarantee that the content of the Surface
     * will remain unchanged when lockCanvas() is called again.
     *
     * @param canvas The Canvas previously returned by lockCanvas()
     *
     * @see #lockCanvas()
     * @see #lockCanvas(android.graphics.Rect)
     */
    CARAPI UnlockCanvasAndPost(
        /* [in] */ ICanvas* canvas);

    /**
     * Returns the {@link SurfaceTexture} used by this view. This method
     * may return null if the view is not attached to a window or if the surface
     * texture has not been initialized yet.
     *
     * @see #isAvailable()
     */
    CARAPI GetSurfaceTexture(
        /* [out] */ ISurfaceTexture** result);

    /**
     * Set the {@link SurfaceTexture} for this view to use. If a {@link
     * SurfaceTexture} is already being used by this view, it is immediately
     * released and not be usable any more.  The {@link
     * SurfaceTextureListener#onSurfaceTextureDestroyed} callback is <b>not</b>
     * called for the previous {@link SurfaceTexture}.  Similarly, the {@link
     * SurfaceTextureListener#onSurfaceTextureAvailable} callback is <b>not</b>
     * called for the {@link SurfaceTexture} passed to setSurfaceTexture.
     *
     * The {@link SurfaceTexture} object must be detached from all OpenGL ES
     * contexts prior to calling this method.
     *
     * @param surfaceTexture The {@link SurfaceTexture} that the view should use.
     * @see SurfaceTexture#detachFromGLContext()
     */
    CARAPI SetSurfaceTexture(
        /* [in] */ ISurfaceTexture* surfaceTexture);

    /**
     * Returns the {@link SurfaceTextureListener} currently associated with this
     * texture view.
     *
     * @see #setSurfaceTextureListener(android.view.TextureView.SurfaceTextureListener)
     * @see SurfaceTextureListener
     */
    CARAPI GetSurfaceTextureListener(
        /* [out] */ ISurfaceTextureListener** result);

    /**
     * Sets the {@link SurfaceTextureListener} used to listen to surface
     * texture events.
     *
     * @see #getSurfaceTextureListener()
     * @see SurfaceTextureListener
     */
    CARAPI SetSurfaceTextureListener(
        /* [in] */ ISurfaceTextureListener* listener);

protected:
    CARAPI OnAttachedToWindow();

    /** @hide */
    CARAPI OnDetachedFromWindowInternal();

    /**
     * Subclasses of TextureView cannot do their own rendering
     * with the {@link Canvas} object.
     *
     * @param canvas The Canvas to which the View is rendered.
     */
    CARAPI_(void) OnDraw(
        /* [in] */ ICanvas* canvas);

    CARAPI OnSizeChanged(
        /* [in] */ Int32 w,
        /* [in] */ Int32 h,
        /* [in] */ Int32 oldw,
        /* [in] */ Int32 oldh);

    /**
     * @hide
     */
    CARAPI_(void) DestroyHardwareResources();

    CARAPI_(AutoPtr<IHardwareLayer>) GetHardwareLayer();

    CARAPI OnVisibilityChanged(
        /* [in] */ IView* changedView,
        /* [in] */ Int32 visibility);

private:
    CARAPI_(void) Init();

    CARAPI_(void) DestroySurface();

    CARAPI_(void) UpdateLayer();

    CARAPI_(void) UpdateLayerAndInvalidate();

    CARAPI_(void) ApplyUpdate();

    CARAPI_(void) ApplyTransformMatrix();

    CARAPI_(void) NativeCreateNativeWindow(
        /* [in] */ ISurfaceTexture* surface);

    CARAPI_(void) NativeDestroyNativeWindow();

    ECO_LOCAL static CARAPI_(Boolean) NativeLockCanvas(
        /* [in] */ Int64 nativeWindow,
        /* [in] */ ICanvas* canvas,
        /* [in] */ IRect* dirty);

    ECO_LOCAL static CARAPI_(void) NativeUnlockCanvasAndPost(
        /* [in] */ Int64 nativeWindow,
        /* [in] */ ICanvas* canvas);

private:
//    static const String LOG_TAG;

    AutoPtr<IHardwareLayer> mLayer;
    AutoPtr<ISurfaceTexture> mSurface;
    AutoPtr<ISurfaceTextureListener> mListener;
    Boolean mHadSurface;

    Boolean mOpaque;

    AutoPtr<IMatrix> mMatrix;
    Boolean mMatrixChanged;

    Object mLock;
    Boolean mUpdateLayer;
    Boolean mUpdateSurface;

    AutoPtr<ICanvas> mCanvas;
    Int32 mSaveCount;

    Object mNativeWindowLock;
    // Set by native code, do not write!
    Int64 mNativeWindow;

    AutoPtr<IOnFrameAvailableListener> mUpdateListener;
};

} // namespace View
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_VIEW_TEXTUREVIEW_H__
