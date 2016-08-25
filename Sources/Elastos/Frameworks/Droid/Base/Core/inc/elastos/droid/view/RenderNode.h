#ifndef __ELASTOS_DROID_VIEW_RENDERNODE_H__
#define __ELASTOS_DROID_VIEW_RENDERNODE_H__

#include "Elastos.Droid.View.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Graphics::IMatrix;
using Elastos::Droid::Graphics::IOutline;
using Elastos::Droid::Graphics::IPaint;
using Elastos::Droid::Graphics::IRect;
using Elastos::Core::Object;

namespace Elastos {
namespace Droid {
namespace View {

class RenderNode
    : public Object
    , public IRenderNode
{
    friend class ThreadedRenderer;

public:
    CAR_INTERFACE_DECL()

    ~RenderNode();

    CARAPI Start(
        /* [in] */ Int32 width,
        /* [in] */ Int32 height,
        /* [out] */ IHardwareCanvas** canvas);

    CARAPI End(
        /* [in] */ IHardwareCanvas* endCanvas);

    CARAPI DestroyDisplayListData();

    CARAPI IsValid(
        /* [out] */ Boolean* isValid);

    CARAPI GetNativeDisplayList(
        /* [out] */ Int64* list);

    CARAPI HasIdentityMatrix(
        /* [out] */ Boolean* hasIdentityMatrix);

    CARAPI GetMatrix(
        /* [in] */ IMatrix* outMatrix);

    CARAPI GetInverseMatrix(
        /* [in] */ IMatrix* outMatrix);

    CARAPI SetLayerType(
        /* [in] */ Int32 layerType,
        /* [out] */ Boolean* result);

    CARAPI SetLayerPaint(
        /* [in] */ IPaint* paint,
        /* [out] */ Boolean* result);

    CARAPI SetClipBounds(
        /* [in] */ IRect* rect,
        /* [out] */ Boolean* result);

    /**
     * Set whether the Render node should clip itself to its bounds. This property is controlled by
     * the view's parent.
     *
     * @param clipToBounds true if the display list should clip to its bounds
     */
    CARAPI SetClipToBounds(
        /* [in] */ Boolean clipToBounds,
        /* [out] */ Boolean* result);

    /**
     * Sets whether the display list should be drawn immediately after the
     * closest ancestor display list containing a projection receiver.
     *
     * @param shouldProject true if the display list should be projected onto a
     *            containing volume.
     */
    CARAPI SetProjectBackwards(
        /* [in] */ Boolean shouldProject,
        /* [out] */ Boolean* result);

    /**
     * Sets whether the display list is a projection receiver - that its parent
     * DisplayList should draw any descendent DisplayLists with
     * ProjectBackwards=true directly on top of it. Default value is false.
     */
    CARAPI SetProjectionReceiver(
        /* [in] */ Boolean  shouldRecieve,
        /* [out] */ Boolean* result);

    /**
     * Sets the outline, defining the shape that casts a shadow, and the path to
     * be clipped if setClipToOutline is set.
     *
     * Deep copies the data into native to simplify reference ownership.
     */
    CARAPI SetOutline(
        /* [in] */ IOutline* outline,
        /* [out] */ Boolean* result);

    CARAPI HasShadow(
        /* [out] */ Boolean* result);

    /**
     * Enables or disables clipping to the outline.
     *
     * @param clipToOutline true if clipping to the outline.
     */
    CARAPI SetClipToOutline(
        /* [in] */ Boolean clipToOutline,
        /* [out] */ Boolean* result);

    CARAPI GetClipToOutline(
        /* [out] */ Boolean* result);

    /**
     * Controls the RenderNode's circular reveal clip.
     */
    CARAPI SetRevealClip(
        /* [in] */ Boolean shouldClip,
        /* [in] */ Float x,
        /* [in] */ Float y,
        /* [in] */ Float radius,
        /* [out] */ Boolean* result);

    /**
     * Set the static matrix on the display list. The specified matrix is combined with other
     * transforms (such as {@link #setScaleX(float)}, {@link #setRotation(float)}, etc.)
     *
     * @param matrix A transform matrix to apply to this display list
     */
    CARAPI SetStaticMatrix(
        /* [in] */ IMatrix* matrix,
        /* [out] */ Boolean* result);

    /**
     * Set the Animation matrix on the display list. This matrix exists if an Animation is
     * currently playing on a View, and is set on the display list during at draw() time. When
     * the Animation finishes, the matrix should be cleared by sending <code>null</code>
     * for the matrix parameter.
     *
     * @param matrix The matrix, null indicates that the matrix should be cleared.
     */
    CARAPI SetAnimationMatrix(
        /* [in] */ IMatrix* matrix,
        /* [out] */ Boolean* result);

    /**
     * Sets the translucency level for the display list.
     *
     * @param alpha The translucency of the display list, must be a value between 0.0f and 1.0f
     *
     * @see View#setAlpha(float)
     * @see #getAlpha()
     */
    CARAPI SetAlpha(
        /* [in] */ Float alpha,
        /* [out] */ Boolean* result);

    /**
     * Returns the translucency level of this display list.
     *
     * @return A value between 0.0f and 1.0f
     *
     * @see #setAlpha(float)
     */
    CARAPI GetAlpha(
        /* [out] */ Float* result);

    /**
     * Sets whether the display list renders content which overlaps. Non-overlapping rendering
     * can use a fast path for alpha that avoids rendering to an offscreen buffer. By default
     * display lists consider they do not have overlapping content.
     *
     * @param hasOverlappingRendering False if the content is guaranteed to be non-overlapping,
     *                                true otherwise.
     *
     * @see android.view.View#hasOverlappingRendering()
     * @see #hasOverlappingRendering()
     */
    CARAPI SetHasOverlappingRendering(
        /* [in] */ Boolean hasOverlappingRendering,
        /* [out] */ Boolean* result);

    /**
     * Indicates whether the content of this display list overlaps.
     *
     * @return True if this display list renders content which overlaps, false otherwise.
     *
     * @see #setHasOverlappingRendering(boolean)
     */
    CARAPI HasOverlappingRendering(
        /* [out] */ Boolean* result);

    CARAPI SetElevation(
        /* [in] */ Float lift,
        /* [out] */ Boolean* result);

    CARAPI GetElevation(
        /* [out] */ Float* result);

    /**
     * Sets the translation value for the display list on the X axis.
     *
     * @param translationX The X axis translation value of the display list, in pixels
     *
     * @see View#setTranslationX(float)
     * @see #getTranslationX()
     */
    CARAPI SetTranslationX(
        /* [in] */ Float translationX,
        /* [out] */ Boolean* result);

    /**
     * Returns the translation value for this display list on the X axis, in pixels.
     *
     * @see #setTranslationX(float)
     */
    CARAPI GetTranslationX(
        /* [out] */ Float* result);

    /**
     * Sets the translation value for the display list on the Y axis.
     *
     * @param translationY The Y axis translation value of the display list, in pixels
     *
     * @see View#setTranslationY(float)
     * @see #getTranslationY()
     */
    CARAPI SetTranslationY(
        /* [in] */ Float translationY,
        /* [out] */ Boolean* result);

    /**
     * Returns the translation value for this display list on the Y axis, in pixels.
     *
     * @see #setTranslationY(float)
     */
    CARAPI GetTranslationY(
        /* [out] */ Float* result);

    /**
     * Sets the translation value for the display list on the Z axis.
     *
     * @see View#setTranslationZ(float)
     * @see #getTranslationZ()
     */
    CARAPI SetTranslationZ(
        /* [in] */ Float translationZ,
        /* [out] */ Boolean* result);

    /**
     * Returns the translation value for this display list on the Z axis.
     *
     * @see #setTranslationZ(float)
     */
    CARAPI GetTranslationZ(
        /* [out] */ Float* result);

    /**
     * Sets the rotation value for the display list around the Z axis.
     *
     * @param rotation The rotation value of the display list, in degrees
     *
     * @see View#setRotation(float)
     * @see #getRotation()
     */
    CARAPI SetRotation(
        /* [in] */ Float rotation,
        /* [out] */ Boolean* result);

    /**
     * Returns the rotation value for this display list around the Z axis, in degrees.
     *
     * @see #setRotation(float)
     */
    CARAPI GetRotation(
        /* [out] */ Float* result);

    /**
     * Sets the rotation value for the display list around the X axis.
     *
     * @param rotationX The rotation value of the display list, in degrees
     *
     * @see View#setRotationX(float)
     * @see #getRotationX()
     */
    CARAPI SetRotationX(
        /* [in] */ Float rotationX,
        /* [out] */ Boolean* result);

    /**
     * Returns the rotation value for this display list around the X axis, in degrees.
     *
     * @see #setRotationX(float)
     */
    CARAPI GetRotationX(
        /* [out] */ Float* result);

    /**
     * Sets the rotation value for the display list around the Y axis.
     *
     * @param rotationY The rotation value of the display list, in degrees
     *
     * @see View#setRotationY(float)
     * @see #getRotationY()
     */
    CARAPI SetRotationY(
        /* [in] */ Float rotationY,
        /* [out] */ Boolean* result);

    /**
     * Returns the rotation value for this display list around the Y axis, in degrees.
     *
     * @see #setRotationY(float)
     */
    CARAPI GetRotationY(
        /* [out] */ Float* result);

    /**
     * Sets the scale value for the display list on the X axis.
     *
     * @param scaleX The scale value of the display list
     *
     * @see View#setScaleX(float)
     * @see #getScaleX()
     */
    CARAPI SetScaleX(
        /* [in] */ Float scaleX,
        /* [out] */ Boolean* result);

    /**
     * Returns the scale value for this display list on the X axis.
     *
     * @see #setScaleX(float)
     */
    CARAPI GetScaleX(
        /* [out] */ Float* result);

    /**
     * Sets the scale value for the display list on the Y axis.
     *
     * @param scaleY The scale value of the display list
     *
     * @see View#setScaleY(float)
     * @see #getScaleY()
     */
    CARAPI SetScaleY(
        /* [in] */ Float scaleY,
        /* [out] */ Boolean* result);

    /**
     * Returns the scale value for this display list on the Y axis.
     *
     * @see #setScaleY(float)
     */
    CARAPI GetScaleY(
        /* [out] */ Float* result);

    /**
     * Sets the pivot value for the display list on the X axis
     *
     * @param pivotX The pivot value of the display list on the X axis, in pixels
     *
     * @see View#setPivotX(float)
     * @see #getPivotX()
     */
    CARAPI SetPivotX(
        /* [in] */ Float pivotX,
        /* [out] */ Boolean* result);

    /**
     * Returns the pivot value for this display list on the X axis, in pixels.
     *
     * @see #setPivotX(float)
     */
    CARAPI GetPivotX(
        /* [out] */ Float* result);

    /**
     * Sets the pivot value for the display list on the Y axis
     *
     * @param pivotY The pivot value of the display list on the Y axis, in pixels
     *
     * @see View#setPivotY(float)
     * @see #getPivotY()
     */
    CARAPI SetPivotY(
        /* [in] */ Float pivotY,
        /* [out] */ Boolean* result);

    /**
     * Returns the pivot value for this display list on the Y axis, in pixels.
     *
     * @see #setPivotY(float)
     */
    CARAPI GetPivotY(
        /* [out] */ Float* result);

    CARAPI IsPivotExplicitlySet(
        /* [out] */ Boolean* result);

    /**
     * Sets the camera distance for the display list. Refer to
     * {@link View#setCameraDistance(float)} for more information on how to
     * use this property.
     *
     * @param distance The distance in Z of the camera of the display list
     *
     * @see View#setCameraDistance(float)
     * @see #getCameraDistance()
     */
    CARAPI SetCameraDistance(
        /* [in] */ Float distance,
        /* [out] */ Boolean* result);

    /**
     * Returns the distance in Z of the camera of the display list.
     *
     * @see #setCameraDistance(float)
     */
    CARAPI GetCameraDistance(
        /* [out] */ Float* result);

    /**
     * Sets the left position for the display list.
     *
     * @param left The left position, in pixels, of the display list
     *
     * @see View#setLeft(int)
     */
    CARAPI SetLeft(
        /* [in] */ Int32 left,
        /* [out] */ Boolean* result);

    /**
     * Sets the top position for the display list.
     *
     * @param top The top position, in pixels, of the display list
     *
     * @see View#setTop(int)
     */
    CARAPI SetTop(
        /* [in] */ Int32 top,
        /* [out] */ Boolean* result);

    /**
     * Sets the right position for the display list.
     *
     * @param right The right position, in pixels, of the display list
     *
     * @see View#setRight(int)
     */
    CARAPI SetRight(
        /* [in] */ Int32 right,
        /* [out] */ Boolean* result);

    /**
     * Sets the bottom position for the display list.
     *
     * @param bottom The bottom position, in pixels, of the display list
     *
     * @see View#setBottom(int)
     */
    CARAPI SetBottom(
        /* [in] */ Int32 bottom,
        /* [out] */ Boolean* result);

    /**
     * Sets the left and top positions for the display list
     *
     * @param left The left position of the display list, in pixels
     * @param top The top position of the display list, in pixels
     * @param right The right position of the display list, in pixels
     * @param bottom The bottom position of the display list, in pixels
     *
     * @see View#setLeft(int)
     * @see View#setTop(int)
     * @see View#setRight(int)
     * @see View#setBottom(int)
     */
    CARAPI SetLeftTopRightBottom(
        /* [in] */ Int32 left,
        /* [in] */ Int32 top,
        /* [in] */ Int32 right,
        /* [in] */ Int32 bottom,
        /* [out] */ Boolean* result);

    /**
     * Offsets the left and right positions for the display list
     *
     * @param offset The amount that the left and right positions of the display
     *               list are offset, in pixels
     *
     * @see View#offsetLeftAndRight(int)
     */
    CARAPI OffsetLeftAndRight(
        /* [in] */ Int32 offset,
        /* [out] */ Boolean* result);

    /**
     * Offsets the top and bottom values for the display list
     *
     * @param offset The amount that the top and bottom positions of the display
     *               list are offset, in pixels
     *
     * @see View#offsetTopAndBottom(int)
     */
    CARAPI OffsetTopAndBottom(
        /* [in] */ Int32 offset,
        /* [out] */ Boolean* result);

    /**
     * Outputs the display list to the log. This method exists for use by
     * tools to output display lists for selected nodes to the log.
     */
    CARAPI Output();

    /**
     * Gets the size of the DisplayList for debug purposes.
     */
    CARAPI GetDebugSize(
        /* [out] */ Int32* size);

    ///////////////////////////////////////////////////////////////////////////
    // Animations
    ///////////////////////////////////////////////////////////////////////////

    CARAPI AddAnimator(
        /* [in] */ IRenderNodeAnimator* animator);

    CARAPI EndAllAnimators();

    static CARAPI_(AutoPtr<IRenderNode>) Create(
        /* [in] */ const String& name,
        /* [in] */ IView* owningView);

    static CARAPI_(AutoPtr<IRenderNode>) Adopt(
        /* [in] */ Int64 nativePtr);

    CARAPI ToString(
        /* [out] */ String* str);

    ///////////////////////////////////////////////////////////////////////////
    // Native methods
    ///////////////////////////////////////////////////////////////////////////
private:
    RenderNode(
        /* [in] */ const String& name,
        /* [in] */ IView* owningView);

    RenderNode(
        /* [in] */ Int64 nativePtr);

    static CARAPI_(Int64) nCreate(
        /* [in] */ const String& name);

    static CARAPI_(void) nDestroyRenderNode(
        /* [in] */ Int64 renderNode);

    static CARAPI_(void) nSetDisplayListData(
        /* [in] */ Int64 renderNode,
        /* [in] */ Int64 newData);

    // Matrix

    static CARAPI_(void) nGetTransformMatrix(
        /* [in] */ Int64 renderNode,
        /* [in] */ Int64 nativeMatrix);

    static CARAPI_(void) nGetInverseTransformMatrix(
        /* [in] */ Int64 renderNode,
        /* [in] */ Int64 nativeMatrix);

    static CARAPI_(Boolean) nHasIdentityMatrix(
        /* [in] */ Int64 renderNode);

    // Properties

    static CARAPI_(Boolean) nOffsetTopAndBottom(
        /* [in] */ Int64 renderNode,
        /* [in] */ Int32 offset);

    static CARAPI_(Boolean) nOffsetLeftAndRight(
        /* [in] */ Int64 renderNode,
        /* [in] */ Int32 offset);

    static CARAPI_(Boolean) nSetLeftTopRightBottom(
        /* [in] */ Int64 renderNode,
        /* [in] */ Int32 left,
        /* [in] */ Int32 top,
        /* [in] */ Int32 right,
        /* [in] */ Int32 bottom);

    static CARAPI_(Boolean) nSetBottom(
        /* [in] */ Int64 renderNode,
        /* [in] */ Int32 bottom);

    static CARAPI_(Boolean) nSetRight(
        /* [in] */ Int64 renderNode,
        /* [in] */ Int32 right);

    static CARAPI_(Boolean) nSetTop(
        /* [in] */ Int64 renderNode,
        /* [in] */ Int32 top);

    static CARAPI_(Boolean) nSetLeft(
        /* [in] */ Int64 renderNode,
        /* [in] */ Int32 left);

    static CARAPI_(Boolean) nSetCameraDistance(
        /* [in] */ Int64 renderNode,
        /* [in] */ Float distance);

    static CARAPI_(Boolean) nSetPivotY(
        /* [in] */ Int64 renderNode,
        /* [in] */ Float pivotY);

    static CARAPI_(Boolean) nSetPivotX(
        /* [in] */ Int64 renderNode,
        /* [in] */ Float pivotX);

    static CARAPI_(Boolean) nSetLayerType(
        /* [in] */ Int64 renderNode,
        /* [in] */ Int32 layerType);

    static CARAPI_(Boolean) nSetLayerPaint(
        /* [in] */ Int64 renderNode,
        /* [in] */ Int64 paint);

    static CARAPI_(Boolean) nSetClipToBounds(
        /* [in] */ Int64 renderNode,
        /* [in] */ Boolean clipToBounds);

    static CARAPI_(Boolean) nSetClipBounds(
        /* [in] */ Int64 renderNode,
        /* [in] */ Int32 left,
        /* [in] */ Int32 top,
        /* [in] */ Int32 right,
        /* [in] */ Int32 bottom);

    static CARAPI_(Boolean) nSetClipBoundsEmpty(
        /* [in] */ Int64 renderNode);

    static CARAPI_(Boolean) nSetProjectBackwards(
        /* [in] */ Int64 renderNode,
        /* [in] */ Boolean shouldProject);

    static CARAPI_(Boolean) nSetProjectionReceiver(
        /* [in] */ Int64 renderNode,
        /* [in] */ Boolean shouldRecieve);

    static CARAPI_(Boolean) nSetOutlineRoundRect(
        /* [in] */ Int64 renderNode,
        /* [in] */ Int32 left,
        /* [in] */ Int32 top,
        /* [in] */ Int32 right,
        /* [in] */ Int32 bottom,
        /* [in] */ Float radius,
        /* [in] */ Float alpha);

    static CARAPI_(Boolean) nSetOutlineConvexPath(
        /* [in] */ Int64 renderNode,
        /* [in] */ Int64 nativePath,
        /* [in] */ Float alpha);

    static CARAPI_(Boolean) nSetOutlineEmpty(
        /* [in] */ Int64 renderNode);

    static CARAPI_(Boolean) nSetOutlineNone(
        /* [in] */ Int64 renderNode);

    static CARAPI_(Boolean) nHasShadow(
        /* [in] */ Int64 renderNode);

    static CARAPI_(Boolean) nSetClipToOutline(
        /* [in] */ Int64 renderNode,
        /* [in] */ Boolean clipToOutline);

    static CARAPI_(Boolean) nSetRevealClip(
        /* [in] */ Int64 renderNode,
        /* [in] */ Boolean shouldClip,
        /* [in] */ Float x,
        /* [in] */ Float y,
        /* [in] */ Float radius);

    static CARAPI_(Boolean) nSetAlpha(
        /* [in] */ Int64 renderNode,
        /* [in] */ Float alpha);

    static CARAPI_(Boolean) nSetHasOverlappingRendering(
        /* [in] */ Int64 renderNode,
        /* [in] */ Boolean hasOverlappingRendering);

    static CARAPI_(Boolean) nSetElevation(
        /* [in] */ Int64 renderNode,
        /* [in] */ Float lift);

    static CARAPI_(Boolean) nSetTranslationX(
        /* [in] */ Int64 renderNode,
        /* [in] */ Float translationX);

    static CARAPI_(Boolean) nSetTranslationY(
        /* [in] */ Int64 renderNode,
        /* [in] */ Float translationY);

    static CARAPI_(Boolean) nSetTranslationZ(
        /* [in] */ Int64 renderNode,
        /* [in] */ Float translationZ);

    static CARAPI_(Boolean) nSetRotation(
        /* [in] */ Int64 renderNode,
        /* [in] */ Float rotation);

    static CARAPI_(Boolean) nSetRotationX(
        /* [in] */ Int64 renderNode,
        /* [in] */ Float rotationX);

    static CARAPI_(Boolean) nSetRotationY(
        /* [in] */ Int64 renderNode,
        /* [in] */ Float rotationY);

    static CARAPI_(Boolean) nSetScaleX(
        /* [in] */ Int64 renderNode,
        /* [in] */ Float scaleX);

    static CARAPI_(Boolean) nSetScaleY(
        /* [in] */ Int64 renderNode,
        /* [in] */ Float scaleY);

    static CARAPI_(Boolean) nSetStaticMatrix(
        /* [in] */ Int64 renderNode,
        /* [in] */ Int64 nativeMatrix);

    static CARAPI_(Boolean) nSetAnimationMatrix(
        /* [in] */ Int64 renderNode,
        /* [in] */ Int64 animationMatrix);

    static CARAPI_(Boolean) nHasOverlappingRendering(
        /* [in] */ Int64 renderNode);

    static CARAPI_(Boolean) nGetClipToOutline(
        /* [in] */ Int64 renderNode);

    static CARAPI_(Float) nGetAlpha(
        /* [in] */ Int64 renderNode);

    static CARAPI_(Float) nGetCameraDistance(
        /* [in] */ Int64 renderNode);

    static CARAPI_(Float) nGetScaleX(
        /* [in] */ Int64 renderNode);

    static CARAPI_(Float) nGetScaleY(
        /* [in] */ Int64 renderNode);

    static CARAPI_(Float) nGetElevation(
        /* [in] */ Int64 renderNode);

    static CARAPI_(Float) nGetTranslationX(
        /* [in] */ Int64 renderNode);

    static CARAPI_(Float) nGetTranslationY(
        /* [in] */ Int64 renderNode);

    static CARAPI_(Float) nGetTranslationZ(
        /* [in] */ Int64 renderNode);

    static CARAPI_(Float) nGetRotation(
        /* [in] */ Int64 renderNode);

    static CARAPI_(Float) nGetRotationX(
        /* [in] */ Int64 renderNode);

    static CARAPI_(Float) nGetRotationY(
        /* [in] */ Int64 renderNode);

    static CARAPI_(Boolean) nIsPivotExplicitlySet(
        /* [in] */ Int64 renderNode);

    static CARAPI_(Float) nGetPivotX(
        /* [in] */ Int64 renderNode);

    static CARAPI_(Float) nGetPivotY(
        /* [in] */ Int64 renderNode);

    static CARAPI_(void) nOutput(
        /* [in] */ Int64 renderNode);

    static CARAPI_(Int32) nGetDebugSize(
        /* [in] */ Int64 renderNode);

    ///////////////////////////////////////////////////////////////////////////
    // Animations
    ///////////////////////////////////////////////////////////////////////////

    static CARAPI_(void) nAddAnimator(
        /* [in] */ Int64 renderNode,
        /* [in] */ Int64 animatorPtr);

    static CARAPI_(void) nEndAllAnimators(
        /* [in] */ Int64 renderNode);

private:
    Boolean mValid;
    // Do not access directly unless you are ThreadedRenderer
    Int64 mNativeRenderNode;
    IView* mOwningView; // weak-ref
};

} // view
} // droid
} // elastos
#endif
