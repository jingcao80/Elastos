
#include "Elastos.Droid.Accounts.h"
#include "Elastos.Droid.App.h"
#include "Elastos.Droid.Animation.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Location.h"
#include "Elastos.Droid.Os.h"
#include "Elastos.Droid.Widget.h"
#include "elastos/droid/view/RenderNode.h"
#include "elastos/droid/view/GLES20RecordingCanvas.h"
#include "elastos/droid/view/View.h"
#include "elastos/droid/graphics/Matrix.h"
#include "elastos/droid/graphics/Paint.h"
#include "elastos/droid/graphics/COutline.h"
#include "elastos/droid/graphics/CPath.h"
#include <elastos/utility/logging/Logger.h>
#include "elastos/droid/graphics/NativePaint.h"
#include "elastos/droid/view/RenderNodeAnimator.h"

#include <hwui/RenderNode.h>
#include <hwui/Animator.h>
#include <hwui/DisplayListRenderer.h>

using Elastos::Droid::Graphics::COutline;
using Elastos::Droid::Graphics::CPath;
using Elastos::Droid::Graphics::ICanvas;
using Elastos::Droid::Graphics::IPath;
using Elastos::Droid::Graphics::Matrix;
using Elastos::Droid::Graphics::Paint;
using Elastos::Droid::Graphics::NativePaint;
using Elastos::Utility::Logging::Logger;

using android::uirenderer::DisplayListData;
using android::uirenderer::LayerType;
using android::uirenderer::RenderPropertyAnimator;

#define NRenderNode android::uirenderer::RenderNode

#define SET_AND_DIRTY(prop, val, dirtyFlag) \
    (reinterpret_cast<NRenderNode*>(renderNodePtr)->mutateStagingProperties().prop(val) \
        ? (reinterpret_cast<NRenderNode*>(renderNodePtr)->setPropertyFieldsDirty(dirtyFlag), true) \
        : false)

namespace Elastos {
namespace Droid {
namespace View {

CAR_INTERFACE_IMPL(RenderNode, Object, IRenderNode)

RenderNode::RenderNode()
    : mValid(FALSE)
    , mNativeRenderNode(0)
{}

ECode RenderNode::Start(
    /* [in] */ Int32 width,
    /* [in] */ Int32 height,
    /* [out] */ IHardwareCanvas** result)
{
    VALIDATE_NOT_NULL(result)
    *result = NULL;

    AutoPtr<IHardwareCanvas> canvas = GLES20RecordingCanvas::Obtain(this);
    ICanvas::Probe(canvas)->SetViewport(width, height);
    // The dirty rect should always be null for a display list
    Int32 r;
    canvas->OnPreDraw(NULL, &r);
    *result = canvas;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode RenderNode::End(
    /* [in] */ IHardwareCanvas* endCanvas)
{
    if (IGLES20RecordingCanvas::Probe(endCanvas) == NULL) {
        Logger::E("RenderNode", "Passed an invalid canvas to end!");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    GLES20RecordingCanvas* canvas = (GLES20RecordingCanvas*) endCanvas;
    canvas->OnPostDraw();
    Int64 renderNodeData;
    canvas->FinishRecording(&renderNodeData);
    nSetDisplayListData(mNativeRenderNode, renderNodeData);
    canvas->Recycle();
    mValid = TRUE;
    return NOERROR;
}

ECode RenderNode::DestroyDisplayListData()
{
    if (!mValid)
        return NOERROR;

    nSetDisplayListData(mNativeRenderNode, 0);
    mValid = FALSE;
    return NOERROR;
}

ECode RenderNode::IsValid(
    /* [out] */ Boolean* isValid)
{
    VALIDATE_NOT_NULL(isValid)
    *isValid = mValid;
    return NOERROR;
}

ECode RenderNode::GetNativeDisplayList(
    /* [out] */ Int64* list)
{
    VALIDATE_NOT_NULL(list)
    if (!mValid) {
        // throw new IllegalStateException("The display list is not valid.");
        Logger::E("RenderNode", "The display list is not valid.");
        return E_ILLEGAL_STATE_EXCEPTION;
    }
    *list = mNativeRenderNode;
    return NOERROR;
}

ECode RenderNode::HasIdentityMatrix(
    /* [out] */ Boolean* hasIdentityMatrix)
{
    VALIDATE_NOT_NULL(hasIdentityMatrix)
    *hasIdentityMatrix = nHasIdentityMatrix(mNativeRenderNode);
    return NOERROR;
}

ECode RenderNode::GetMatrix(
    /* [in] */ IMatrix* outMatrix)
{
    nGetTransformMatrix(mNativeRenderNode, ((Matrix*)outMatrix)->mNativeMatrix);
    return NOERROR;
}

ECode RenderNode::GetInverseMatrix(
    /* [in] */ IMatrix* outMatrix)
{
    nGetInverseTransformMatrix(mNativeRenderNode, ((Matrix*)outMatrix)->mNativeMatrix);
    return NOERROR;
}

ECode RenderNode::SetLayerType(
    /* [in] */ Int32 layerType,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = nSetLayerType(mNativeRenderNode, layerType);
    return NOERROR;
}

ECode RenderNode::SetLayerPaint(
    /* [in] */ IPaint* paint,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = nSetLayerPaint(mNativeRenderNode, paint != NULL ? ((Paint*)paint)->mNativePaint : 0);
    return NOERROR;
}

ECode RenderNode::SetClipBounds(
    /* [in] */ IRect* rect,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    if (rect == NULL) {
        *result = nSetClipBoundsEmpty(mNativeRenderNode);
    } else {
        Int32 left, top, right, bottom;
        rect->GetLeft(&left);
        rect->GetTop(&top);
        rect->GetRight(&right);
        rect->GetBottom(&bottom);
        *result = nSetClipBounds(mNativeRenderNode, left, top, right, bottom);
    }
    return NOERROR;
}

/**
 * Set whether the Render node should clip itself to its bounds. This property is controlled by
 * the view's parent.
 *
 * @param clipToBounds true if the display list should clip to its bounds
 */
ECode RenderNode::SetClipToBounds(
    /* [in] */ Boolean clipToBounds,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = nSetClipToBounds(mNativeRenderNode, clipToBounds);
    return NOERROR;
}

/**
 * Sets whether the display list should be drawn immediately after the
 * closest ancestor display list containing a projection receiver.
 *
 * @param shouldProject true if the display list should be projected onto a
 *            containing volume.
 */
ECode RenderNode::SetProjectBackwards(
    /* [in] */ Boolean shouldProject,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = nSetProjectBackwards(mNativeRenderNode, shouldProject);
    return NOERROR;
}

/**
 * Sets whether the display list is a projection receiver - that its parent
 * DisplayList should draw any descendent DisplayLists with
 * ProjectBackwards=true directly on top of it. Default value is false.
 */
ECode RenderNode::SetProjectionReceiver(
    /* [in] */ Boolean  shouldRecieve,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = nSetProjectionReceiver(mNativeRenderNode, shouldRecieve);
    return NOERROR;
}

/**
 * Sets the outline, defining the shape that casts a shadow, and the path to
 * be clipped if setClipToOutline is set.
 *
 * Deep copies the data into native to simplify reference ownership.
 */
ECode RenderNode::SetOutline(
    /* [in] */ IOutline* outline,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    if (outline == NULL) {
        *result = nSetOutlineNone(mNativeRenderNode);
        return NOERROR;
    }
    Boolean empty;
    if (outline->IsEmpty(&empty), empty) {
        *result = nSetOutlineEmpty(mNativeRenderNode);
        return NOERROR;
    }
    COutline* ol = (COutline*)outline;
    AutoPtr<IRect> rect = ol->mRect;
    if (rect != NULL) {
        Int32 left, top, right, bottom;
        rect->GetLeft(&left);
        rect->GetTop(&top);
        rect->GetRight(&right);
        rect->GetBottom(&bottom);
        Float radius = ol->mRadius;
        Float alpha = ol->mAlpha;
        *result = nSetOutlineRoundRect(mNativeRenderNode, left, top,
                right, bottom, radius, alpha);
        return NOERROR;
    }
    else if (ol->mPath != NULL) {
        AutoPtr<IPath> path = ol->mPath;
        Float alpha = ol->mAlpha;
        *result = nSetOutlineConvexPath(mNativeRenderNode,((CPath*)path.Get())->mNativePath, alpha);
        return NOERROR;
    }
    Logger::E("RenderNode","Unrecognized outline?");
    return E_ILLEGAL_ARGUMENT_EXCEPTION;
}

ECode RenderNode::HasShadow(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = nHasShadow(mNativeRenderNode);
    return NOERROR;
}

/**
 * Enables or disables clipping to the outline.
 *
 * @param clipToOutline true if clipping to the outline.
 */
ECode RenderNode::SetClipToOutline(
    /* [in] */ Boolean clipToOutline,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = nSetClipToOutline(mNativeRenderNode, clipToOutline);
    return NOERROR;
}

ECode RenderNode::GetClipToOutline(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = nGetClipToOutline(mNativeRenderNode);
    return NOERROR;
}

/**
 * Controls the RenderNode's circular reveal clip.
 */
ECode RenderNode::SetRevealClip(
    /* [in] */ Boolean shouldClip,
    /* [in] */ Float x,
    /* [in] */ Float y,
    /* [in] */ Float radius,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = nSetRevealClip(mNativeRenderNode, shouldClip, x, y, radius);
    return NOERROR;
}

/**
 * Set the static matrix on the display list. The specified matrix is combined with other
 * transforms (such as {@link #setScaleX(float)}, {@link #setRotation(float)}, etc.)
 *
 * @param matrix A transform matrix to apply to this display list
 */
ECode RenderNode::SetStaticMatrix(
    /* [in] */ IMatrix* matrix,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = nSetStaticMatrix(mNativeRenderNode, ((Matrix*)matrix)->mNativeMatrix);
    return NOERROR;
}

/**
 * Set the Animation matrix on the display list. This matrix exists if an Animation is
 * currently playing on a View, and is set on the display list during at draw() time. When
 * the Animation finishes, the matrix should be cleared by sending <code>null</code>
 * for the matrix parameter.
 *
 * @param matrix The matrix, null indicates that the matrix should be cleared.
 */
ECode RenderNode::SetAnimationMatrix(
    /* [in] */ IMatrix* matrix,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = nSetAnimationMatrix(mNativeRenderNode,
            (matrix != NULL) ? ((Matrix*)matrix)->mNativeMatrix : 0);
    return NOERROR;
}

/**
 * Sets the translucency level for the display list.
 *
 * @param alpha The translucency of the display list, must be a value between 0.0f and 1.0f
 *
 * @see View#setAlpha(float)
 * @see #getAlpha()
 */
ECode RenderNode::SetAlpha(
    /* [in] */ Float alpha,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = nSetAlpha(mNativeRenderNode, alpha);
    return NOERROR;
}

/**
 * Returns the translucency level of this display list.
 *
 * @return A value between 0.0f and 1.0f
 *
 * @see #setAlpha(float)
 */
ECode RenderNode::GetAlpha(
    /* [out] */ Float* result)
{
    VALIDATE_NOT_NULL(result)
    *result = nGetAlpha(mNativeRenderNode);
    return NOERROR;
}

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
ECode RenderNode::SetHasOverlappingRendering(
    /* [in] */ Boolean hasOverlappingRendering,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = nSetHasOverlappingRendering(mNativeRenderNode, hasOverlappingRendering);
    return NOERROR;
}

/**
 * Indicates whether the content of this display list overlaps.
 *
 * @return True if this display list renders content which overlaps, false otherwise.
 *
 * @see #setHasOverlappingRendering(boolean)
 */
ECode RenderNode::HasOverlappingRendering(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = nHasOverlappingRendering(mNativeRenderNode);
    return NOERROR;
}

ECode RenderNode::SetElevation(
    /* [in] */ Float lift,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = nSetElevation(mNativeRenderNode, lift);
    return NOERROR;
}

ECode RenderNode::GetElevation(
    /* [out] */ Float* result)
{
    VALIDATE_NOT_NULL(result)
    *result = nGetElevation(mNativeRenderNode);
    return NOERROR;
}

/**
 * Sets the translation value for the display list on the X axis.
 *
 * @param translationX The X axis translation value of the display list, in pixels
 *
 * @see View#setTranslationX(float)
 * @see #getTranslationX()
 */
ECode RenderNode::SetTranslationX(
    /* [in] */ Float translationX,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = nSetTranslationX(mNativeRenderNode, translationX);
    return NOERROR;
}

/**
 * Returns the translation value for this display list on the X axis, in pixels.
 *
 * @see #setTranslationX(float)
 */
ECode RenderNode::GetTranslationX(
    /* [out] */ Float* result)
{
    VALIDATE_NOT_NULL(result)
    *result = nGetTranslationX(mNativeRenderNode);
    return NOERROR;
}

/**
 * Sets the translation value for the display list on the Y axis.
 *
 * @param translationY The Y axis translation value of the display list, in pixels
 *
 * @see View#setTranslationY(float)
 * @see #getTranslationY()
 */
ECode RenderNode::SetTranslationY(
    /* [in] */ Float translationY,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = nSetTranslationY(mNativeRenderNode, translationY);
    return NOERROR;
}

/**
 * Returns the translation value for this display list on the Y axis, in pixels.
 *
 * @see #setTranslationY(float)
 */
ECode RenderNode::GetTranslationY(
    /* [out] */ Float* result)
{
    VALIDATE_NOT_NULL(result)
    *result = nGetTranslationY(mNativeRenderNode);
    return NOERROR;
}

/**
 * Sets the translation value for the display list on the Z axis.
 *
 * @see View#setTranslationZ(float)
 * @see #getTranslationZ()
 */
ECode RenderNode::SetTranslationZ(
    /* [in] */ Float translationZ,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = nSetTranslationZ(mNativeRenderNode, translationZ);
    return NOERROR;
}

/**
 * Returns the translation value for this display list on the Z axis.
 *
 * @see #setTranslationZ(float)
 */
ECode RenderNode::GetTranslationZ(
    /* [out] */ Float* result)
{
    VALIDATE_NOT_NULL(result)
    *result = nGetTranslationZ(mNativeRenderNode);
    return NOERROR;
}

/**
 * Sets the rotation value for the display list around the Z axis.
 *
 * @param rotation The rotation value of the display list, in degrees
 *
 * @see View#setRotation(float)
 * @see #getRotation()
 */
ECode RenderNode::SetRotation(
    /* [in] */ Float rotation,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = nSetRotation(mNativeRenderNode, rotation);
    return NOERROR;
}

/**
 * Returns the rotation value for this display list around the Z axis, in degrees.
 *
 * @see #setRotation(float)
 */
ECode RenderNode::GetRotation(
    /* [out] */ Float* result)
{
    VALIDATE_NOT_NULL(result)
    *result = nGetRotation(mNativeRenderNode);
    return NOERROR;
}

/**
 * Sets the rotation value for the display list around the X axis.
 *
 * @param rotationX The rotation value of the display list, in degrees
 *
 * @see View#setRotationX(float)
 * @see #getRotationX()
 */
ECode RenderNode::SetRotationX(
    /* [in] */ Float rotationX,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = nSetRotationX(mNativeRenderNode, rotationX);
    return NOERROR;
}

/**
 * Returns the rotation value for this display list around the X axis, in degrees.
 *
 * @see #setRotationX(float)
 */
ECode RenderNode::GetRotationX(
    /* [out] */ Float* result)
{
    VALIDATE_NOT_NULL(result)
    *result = nGetRotationX(mNativeRenderNode);
    return NOERROR;
}

/**
 * Sets the rotation value for the display list around the Y axis.
 *
 * @param rotationY The rotation value of the display list, in degrees
 *
 * @see View#setRotationY(float)
 * @see #getRotationY()
 */
ECode RenderNode::SetRotationY(
    /* [in] */ Float rotationY,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = nSetRotationY(mNativeRenderNode, rotationY);
    return NOERROR;
}

/**
 * Returns the rotation value for this display list around the Y axis, in degrees.
 *
 * @see #setRotationY(float)
 */
ECode RenderNode::GetRotationY(
    /* [out] */ Float* result)
{
    VALIDATE_NOT_NULL(result)
    *result = nGetRotationY(mNativeRenderNode);
    return NOERROR;
}

/**
 * Sets the scale value for the display list on the X axis.
 *
 * @param scaleX The scale value of the display list
 *
 * @see View#setScaleX(float)
 * @see #getScaleX()
 */
ECode RenderNode::SetScaleX(
    /* [in] */ Float scaleX,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = nSetScaleX(mNativeRenderNode, scaleX);
    return NOERROR;
}

/**
 * Returns the scale value for this display list on the X axis.
 *
 * @see #setScaleX(float)
 */
ECode RenderNode::GetScaleX(
    /* [out] */ Float* result)
{
    VALIDATE_NOT_NULL(result)
    *result = nGetScaleX(mNativeRenderNode);
    return NOERROR;
}

/**
 * Sets the scale value for the display list on the Y axis.
 *
 * @param scaleY The scale value of the display list
 *
 * @see View#setScaleY(float)
 * @see #getScaleY()
 */
ECode RenderNode::SetScaleY(
    /* [in] */ Float scaleY,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = nSetScaleY(mNativeRenderNode, scaleY);
    return NOERROR;
}

/**
 * Returns the scale value for this display list on the Y axis.
 *
 * @see #setScaleY(float)
 */
ECode RenderNode::GetScaleY(
    /* [out] */ Float* result)
{
    VALIDATE_NOT_NULL(result)
    *result = nGetScaleY(mNativeRenderNode);
    return NOERROR;
}

/**
 * Sets the pivot value for the display list on the X axis
 *
 * @param pivotX The pivot value of the display list on the X axis, in pixels
 *
 * @see View#setPivotX(float)
 * @see #getPivotX()
 */
ECode RenderNode::SetPivotX(
    /* [in] */ Float pivotX,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = nSetPivotX(mNativeRenderNode, pivotX);
    return NOERROR;
}

/**
 * Returns the pivot value for this display list on the X axis, in pixels.
 *
 * @see #setPivotX(float)
 */
ECode RenderNode::GetPivotX(
    /* [out] */ Float* result)
{
    VALIDATE_NOT_NULL(result)
    *result = nGetPivotX(mNativeRenderNode);
    return NOERROR;
}

/**
 * Sets the pivot value for the display list on the Y axis
 *
 * @param pivotY The pivot value of the display list on the Y axis, in pixels
 *
 * @see View#setPivotY(float)
 * @see #getPivotY()
 */
ECode RenderNode::SetPivotY(
    /* [in] */ Float pivotY,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = nSetPivotY(mNativeRenderNode, pivotY);
    return NOERROR;
}

/**
 * Returns the pivot value for this display list on the Y axis, in pixels.
 *
 * @see #setPivotY(float)
 */
ECode RenderNode::GetPivotY(
    /* [out] */ Float* result)
{
    VALIDATE_NOT_NULL(result)
    *result = nGetPivotY(mNativeRenderNode);
    return NOERROR;
}

ECode RenderNode::IsPivotExplicitlySet(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = nIsPivotExplicitlySet(mNativeRenderNode);
    return NOERROR;
}

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
ECode RenderNode::SetCameraDistance(
    /* [in] */ Float distance,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = nSetCameraDistance(mNativeRenderNode, distance);
    return NOERROR;
}

/**
 * Returns the distance in Z of the camera of the display list.
 *
 * @see #setCameraDistance(float)
 */
ECode RenderNode::GetCameraDistance(
    /* [out] */ Float* result)
{
    VALIDATE_NOT_NULL(result)
    *result = nGetCameraDistance(mNativeRenderNode);
    return NOERROR;
}

/**
 * Sets the left position for the display list.
 *
 * @param left The left position, in pixels, of the display list
 *
 * @see View#setLeft(int)
 */
ECode RenderNode::SetLeft(
    /* [in] */ Int32 left,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = nSetLeft(mNativeRenderNode, left);
    return NOERROR;
}

/**
 * Sets the top position for the display list.
 *
 * @param top The top position, in pixels, of the display list
 *
 * @see View#setTop(int)
 */
ECode RenderNode::SetTop(
    /* [in] */ Int32 top,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = nSetTop(mNativeRenderNode, top);
    return NOERROR;
}

/**
 * Sets the right position for the display list.
 *
 * @param right The right position, in pixels, of the display list
 *
 * @see View#setRight(int)
 */
ECode RenderNode::SetRight(
    /* [in] */ Int32 right,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = nSetRight(mNativeRenderNode, right);
    return NOERROR;
}

/**
 * Sets the bottom position for the display list.
 *
 * @param bottom The bottom position, in pixels, of the display list
 *
 * @see View#setBottom(int)
 */
ECode RenderNode::SetBottom(
    /* [in] */ Int32 bottom,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = nSetBottom(mNativeRenderNode, bottom);
    return NOERROR;
}

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
ECode RenderNode::SetLeftTopRightBottom(
    /* [in] */ Int32 left,
    /* [in] */ Int32 top,
    /* [in] */ Int32 right,
    /* [in] */ Int32 bottom,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = nSetLeftTopRightBottom(mNativeRenderNode, left, top, right, bottom);
    return NOERROR;
}

/**
 * Offsets the left and right positions for the display list
 *
 * @param offset The amount that the left and right positions of the display
 *               list are offset, in pixels
 *
 * @see View#offsetLeftAndRight(int)
 */
ECode RenderNode::OffsetLeftAndRight(
    /* [in] */ Int32 offset,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = nOffsetLeftAndRight(mNativeRenderNode, offset);
    return NOERROR;
}

/**
 * Offsets the top and bottom values for the display list
 *
 * @param offset The amount that the top and bottom positions of the display
 *               list are offset, in pixels
 *
 * @see View#offsetTopAndBottom(int)
 */
ECode RenderNode::OffsetTopAndBottom(
    /* [in] */ Int32 offset,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = nOffsetTopAndBottom(mNativeRenderNode, offset);
    return NOERROR;
}

/**
 * Outputs the display list to the log. This method exists for use by
 * tools to output display lists for selected nodes to the log.
 */
ECode RenderNode::Output()
{
    nOutput(mNativeRenderNode);
    return NOERROR;
}

/**
 * Gets the size of the DisplayList for debug purposes.
 */
ECode RenderNode::GetDebugSize(
    /* [out] */ Int32* size)
{
    VALIDATE_NOT_NULL(size)
    *size = nGetDebugSize(mNativeRenderNode);
    return NOERROR;
}

///////////////////////////////////////////////////////////////////////////
// Animations
///////////////////////////////////////////////////////////////////////////

ECode RenderNode::AddAnimator(
    /* [in] */ IRenderNodeAnimator* animator)
{
    if (mOwningView == NULL || ((View*)mOwningView.Get())->mAttachInfo == NULL) {
        Logger::E("RenderNode", "Cannot start this animator on a detached view!");
        return E_ILLEGAL_STATE_EXCEPTION;
    }

    Int64 ptr;
    animator->GetNativeAnimator(&ptr);
    nAddAnimator(mNativeRenderNode, ptr);
    ((View*)mOwningView.Get())->mAttachInfo->mViewRootImpl->RegisterAnimatingRenderNode(this);
    return NOERROR;
}

ECode RenderNode::EndAllAnimators()
{
    nEndAllAnimators(mNativeRenderNode);
    return NOERROR;
}

AutoPtr<IRenderNode> RenderNode::Create(
    /* [in] */ const String& name,
    /* [in] */ IView* owningView)
{
    return new RenderNode(name, owningView);
}

AutoPtr<IRenderNode> RenderNode::Adopt(
    /* [in] */ Int64 nativePtr)
{
    return new RenderNode(nativePtr);
}

RenderNode::RenderNode(
    /* [in] */ const String& name,
    /* [in] */ IView* owningView)
{
    mNativeRenderNode = nCreate(name);
    mOwningView = owningView;
}

RenderNode::RenderNode(
    /* [in] */ Int64 nativePtr)
{
    mNativeRenderNode = nativePtr;
}

///////////////////////////////////////////////////////////////////////////
// Native methods
///////////////////////////////////////////////////////////////////////////

Int64 RenderNode::nCreate(
    /* [in] */ const String& name)
{
    NRenderNode* renderNode = new NRenderNode();
    renderNode->incStrong(0);
    if (name != NULL) {
        const char* textArray = name.string();
        renderNode->setName(textArray);
    }
    return reinterpret_cast<Int64>(renderNode);
}

void RenderNode::nDestroyRenderNode(
    /* [in] */ Int64 renderNodePtr)
{
    NRenderNode* renderNode = reinterpret_cast<NRenderNode*>(renderNodePtr);
    renderNode->decStrong(0);
}

void RenderNode::nSetDisplayListData(
    /* [in] */ Int64 renderNodePtr,
    /* [in] */ Int64 newDataPtr)
{
    NRenderNode* renderNode = reinterpret_cast<NRenderNode*>(renderNodePtr);
    DisplayListData* newData = reinterpret_cast<DisplayListData*>(newDataPtr);
    renderNode->setStagingDisplayList(newData);
}

// Matrix

void RenderNode::nGetTransformMatrix(
    /* [in] */ Int64 renderNodePtr,
    /* [in] */ Int64 outMatrixPtr)
{
    NRenderNode* renderNode = reinterpret_cast<NRenderNode*>(renderNodePtr);
    SkMatrix* outMatrix = reinterpret_cast<SkMatrix*>(outMatrixPtr);

    renderNode->mutateStagingProperties().updateMatrix();
    const SkMatrix* transformMatrix = renderNode->stagingProperties().getTransformMatrix();

    if (transformMatrix) {
        *outMatrix = *transformMatrix;
    } else {
        outMatrix->setIdentity();
    }
}

void RenderNode::nGetInverseTransformMatrix(
    /* [in] */ Int64 renderNodePtr,
    /* [in] */ Int64 outMatrixPtr)
{
    // load transform matrix
    nGetTransformMatrix(renderNodePtr, outMatrixPtr);
    SkMatrix* outMatrix = reinterpret_cast<SkMatrix*>(outMatrixPtr);

    // return it inverted
    if (!outMatrix->invert(outMatrix)) {
        // failed to load inverse, pass back identity
        outMatrix->setIdentity();
    }
}

Boolean RenderNode::nHasIdentityMatrix(
    /* [in] */ Int64 renderNodePtr)
{
    NRenderNode* renderNode = reinterpret_cast<NRenderNode*>(renderNodePtr);
    renderNode->mutateStagingProperties().updateMatrix();
    return !renderNode->stagingProperties().hasTransformMatrix();
}
// Properties

Boolean RenderNode::nOffsetTopAndBottom(
    /* [in] */ Int64 renderNodePtr,
    /* [in] */ Int32 offset)
{
    return SET_AND_DIRTY(offsetTopBottom, offset, NRenderNode::Y);
}

Boolean RenderNode::nOffsetLeftAndRight(
    /* [in] */ Int64 renderNodePtr,
    /* [in] */ Int32 offset)
{
    return SET_AND_DIRTY(offsetLeftRight, offset, NRenderNode::X);
}

Boolean RenderNode::nSetLeftTopRightBottom(
    /* [in] */ Int64 renderNodePtr,
    /* [in] */ Int32 left,
    /* [in] */ Int32 top,
    /* [in] */ Int32 right,
    /* [in] */ Int32 bottom)
{
    NRenderNode* renderNode = reinterpret_cast<NRenderNode*>(renderNodePtr);
    if (renderNode->mutateStagingProperties().setLeftTopRightBottom(left, top, right, bottom)) {
        renderNode->setPropertyFieldsDirty(NRenderNode::X | NRenderNode::Y);
        return TRUE;
    }
    return FALSE;
}

Boolean RenderNode::nSetBottom(
    /* [in] */ Int64 renderNodePtr,
    /* [in] */ Int32 bottom)
{
    return SET_AND_DIRTY(setBottom, bottom, NRenderNode::Y);
}

Boolean RenderNode::nSetRight(
    /* [in] */ Int64 renderNodePtr,
    /* [in] */ Int32 right)
{
    return SET_AND_DIRTY(setRight, right, NRenderNode::X);
}

Boolean RenderNode::nSetTop(
    /* [in] */ Int64 renderNodePtr,
    /* [in] */ Int32 top)
{
    return SET_AND_DIRTY(setTop, top, NRenderNode::Y);
}

Boolean RenderNode::nSetLeft(
    /* [in] */ Int64 renderNodePtr,
    /* [in] */ Int32 left)
{
    return SET_AND_DIRTY(setTop, left, NRenderNode::X);
}

Boolean RenderNode::nSetCameraDistance(
    /* [in] */ Int64 renderNodePtr,
    /* [in] */ Float distance)
{
    return SET_AND_DIRTY(setCameraDistance, distance, NRenderNode::GENERIC);
}

Boolean RenderNode::nSetPivotY(
    /* [in] */ Int64 renderNodePtr,
    /* [in] */ Float pivotY)
{
    return SET_AND_DIRTY(setPivotY, pivotY, NRenderNode::GENERIC);
}

Boolean RenderNode::nSetPivotX(
    /* [in] */ Int64 renderNodePtr,
    /* [in] */ Float pivotX)
{
    return SET_AND_DIRTY(setPivotX, pivotX, NRenderNode::GENERIC);;
}

Boolean RenderNode::nSetLayerType(
    /* [in] */ Int64 renderNodePtr,
    /* [in] */ Int32 layerType)
{
    LayerType nlayerType = static_cast<LayerType>(layerType);
    return SET_AND_DIRTY(mutateLayerProperties().setType, nlayerType, NRenderNode::GENERIC);
}

Boolean RenderNode::nSetLayerPaint(
    /* [in] */ Int64 renderNodePtr,
    /* [in] */ Int64 paintPtr)
{
    NativePaint* paint = reinterpret_cast<NativePaint*>(paintPtr);
    return SET_AND_DIRTY(mutateLayerProperties().setFromPaint, paint, NRenderNode::GENERIC);
}

Boolean RenderNode::nSetClipToBounds(
    /* [in] */ Int64 renderNodePtr,
    /* [in] */ Boolean clipToBounds)
{
    return SET_AND_DIRTY(setClipToBounds, clipToBounds, NRenderNode::GENERIC);
}

Boolean RenderNode::nSetClipBounds(
    /* [in] */ Int64 renderNodePtr,
    /* [in] */ Int32 left,
    /* [in] */ Int32 top,
    /* [in] */ Int32 right,
    /* [in] */ Int32 bottom)
{
    android::uirenderer::Rect clipBounds(left, top, right, bottom);
    return SET_AND_DIRTY(setClipBounds, clipBounds, NRenderNode::GENERIC);
}

Boolean RenderNode::nSetClipBoundsEmpty(
    /* [in] */ Int64 renderNodePtr)
{
    return SET_AND_DIRTY(setClipBoundsEmpty,, NRenderNode::GENERIC);
}

Boolean RenderNode::nSetProjectBackwards(
    /* [in] */ Int64 renderNodePtr,
    /* [in] */ Boolean shouldProject)
{
    return SET_AND_DIRTY(setProjectBackwards, shouldProject, NRenderNode::GENERIC);
}

Boolean RenderNode::nSetProjectionReceiver(
    /* [in] */ Int64 renderNodePtr,
    /* [in] */ Boolean shouldRecieve)
{
    return SET_AND_DIRTY(setProjectionReceiver, shouldRecieve, NRenderNode::GENERIC);
}

Boolean RenderNode::nSetOutlineRoundRect(
    /* [in] */ Int64 renderNodePtr,
    /* [in] */ Int32 left,
    /* [in] */ Int32 top,
    /* [in] */ Int32 right,
    /* [in] */ Int32 bottom,
    /* [in] */ Float radius,
    /* [in] */ Float alpha)
{
    NRenderNode* renderNode = reinterpret_cast<NRenderNode*>(renderNodePtr);
    renderNode->mutateStagingProperties().mutableOutline().setRoundRect(left, top, right, bottom,
            radius, alpha);
    renderNode->setPropertyFieldsDirty(NRenderNode::GENERIC);
    return TRUE;
}

Boolean RenderNode::nSetOutlineConvexPath(
    /* [in] */ Int64 renderNodePtr,
    /* [in] */ Int64 outlinePathPtr,
    /* [in] */ Float alpha)
{
    NRenderNode* renderNode = reinterpret_cast<NRenderNode*>(renderNodePtr);
    SkPath* outlinePath = reinterpret_cast<SkPath*>(outlinePathPtr);
    renderNode->mutateStagingProperties().mutableOutline().setConvexPath(outlinePath, alpha);
    renderNode->setPropertyFieldsDirty(NRenderNode::GENERIC);
    return TRUE;
}

Boolean RenderNode::nSetOutlineEmpty(
    /* [in] */ Int64 renderNodePtr)
{
    NRenderNode* renderNode = reinterpret_cast<NRenderNode*>(renderNodePtr);
    renderNode->mutateStagingProperties().mutableOutline().setEmpty();
    renderNode->setPropertyFieldsDirty(NRenderNode::GENERIC);
    return TRUE;
}

Boolean RenderNode::nSetOutlineNone(
    /* [in] */ Int64 renderNodePtr)
{
    NRenderNode* renderNode = reinterpret_cast<NRenderNode*>(renderNodePtr);
    renderNode->mutateStagingProperties().mutableOutline().setNone();
    renderNode->setPropertyFieldsDirty(NRenderNode::GENERIC);
    return TRUE;
}

Boolean RenderNode::nHasShadow(
    /* [in] */ Int64 renderNodePtr)
{
    NRenderNode* renderNode = reinterpret_cast<NRenderNode*>(renderNodePtr);
    return renderNode->stagingProperties().hasShadow();
}

Boolean RenderNode::nSetClipToOutline(
    /* [in] */ Int64 renderNodePtr,
    /* [in] */ Boolean clipToOutline)
{
    NRenderNode* renderNode = reinterpret_cast<NRenderNode*>(renderNodePtr);
    renderNode->mutateStagingProperties().mutableOutline().setShouldClip(clipToOutline);
    renderNode->setPropertyFieldsDirty(NRenderNode::GENERIC);
    return TRUE;
}

Boolean RenderNode::nSetRevealClip(
    /* [in] */ Int64 renderNodePtr,
    /* [in] */ Boolean shouldClip,
    /* [in] */ Float x,
    /* [in] */ Float y,
    /* [in] */ Float radius)
{
    NRenderNode* renderNode = reinterpret_cast<NRenderNode*>(renderNodePtr);
    renderNode->mutateStagingProperties().mutableRevealClip().set(
            shouldClip, x, y, radius);
    renderNode->setPropertyFieldsDirty(NRenderNode::GENERIC);
    return true;
}

Boolean RenderNode::nSetAlpha(
    /* [in] */ Int64 renderNodePtr,
    /* [in] */ Float alpha)
{
    return SET_AND_DIRTY(setAlpha, alpha, NRenderNode::ALPHA);
}

Boolean RenderNode::nSetHasOverlappingRendering(
    /* [in] */ Int64 renderNodePtr,
    /* [in] */ Boolean hasOverlappingRendering)
{
    return SET_AND_DIRTY(setHasOverlappingRendering, hasOverlappingRendering,
            NRenderNode::GENERIC);
}

Boolean RenderNode::nSetElevation(
    /* [in] */ Int64 renderNodePtr,
    /* [in] */ Float elevation)
{
    return SET_AND_DIRTY(setElevation, elevation, NRenderNode::Z);
}

Boolean RenderNode::nSetTranslationX(
    /* [in] */ Int64 renderNodePtr,
    /* [in] */ Float translationX)
{
    return SET_AND_DIRTY(setTranslationX, translationX, NRenderNode::TRANSLATION_X | NRenderNode::X);
}

Boolean RenderNode::nSetTranslationY(
    /* [in] */ Int64 renderNodePtr,
    /* [in] */ Float translationY)
{
    return SET_AND_DIRTY(setTranslationY, translationY, NRenderNode::TRANSLATION_Y | NRenderNode::Y);
}

Boolean RenderNode::nSetTranslationZ(
    /* [in] */ Int64 renderNodePtr,
    /* [in] */ Float translationZ)
{
    return SET_AND_DIRTY(setTranslationZ, translationZ, NRenderNode::TRANSLATION_Z | NRenderNode::Z);
}

Boolean RenderNode::nSetRotation(
    /* [in] */ Int64 renderNodePtr,
    /* [in] */ Float rotation)
{
    return SET_AND_DIRTY(setRotation, rotation, NRenderNode::ROTATION);
}

Boolean RenderNode::nSetRotationX(
    /* [in] */ Int64 renderNodePtr,
    /* [in] */ Float rotationX)
{
    return SET_AND_DIRTY(setRotationX, rotationX, NRenderNode::ROTATION_X);
}

Boolean RenderNode::nSetRotationY(
    /* [in] */ Int64 renderNodePtr,
    /* [in] */ Float rotationY)
{
    return SET_AND_DIRTY(setRotationY, rotationY, NRenderNode::ROTATION_Y);
}

Boolean RenderNode::nSetScaleX(
    /* [in] */ Int64 renderNodePtr,
    /* [in] */ Float scaleX)
{
    return SET_AND_DIRTY(setScaleX, scaleX, NRenderNode::SCALE_X);
}

Boolean RenderNode::nSetScaleY(
    /* [in] */ Int64 renderNodePtr,
    /* [in] */ Float scaleY)
{
    return SET_AND_DIRTY(setScaleY, scaleY, NRenderNode::SCALE_Y);
}

Boolean RenderNode::nSetStaticMatrix(
    /* [in] */ Int64 renderNodePtr,
    /* [in] */ Int64 matrixPtr)
{
    SkMatrix* matrix = reinterpret_cast<SkMatrix*>(matrixPtr);
    return SET_AND_DIRTY(setStaticMatrix, matrix, NRenderNode::GENERIC);
}

Boolean RenderNode::nSetAnimationMatrix(
    /* [in] */ Int64 renderNodePtr,
    /* [in] */ Int64 matrixPtr)
{
    SkMatrix* matrix = reinterpret_cast<SkMatrix*>(matrixPtr);
    return SET_AND_DIRTY(setAnimationMatrix, matrix, NRenderNode::GENERIC);
}

Boolean RenderNode::nHasOverlappingRendering(
    /* [in] */ Int64 renderNodePtr)
{
    NRenderNode* renderNode = reinterpret_cast<NRenderNode*>(renderNodePtr);
    return renderNode->stagingProperties().hasOverlappingRendering();
}

Boolean RenderNode::nGetClipToOutline(
    /* [in] */ Int64 renderNodePtr)
{
    NRenderNode* renderNode = reinterpret_cast<NRenderNode*>(renderNodePtr);
    return renderNode->stagingProperties().getOutline().getShouldClip();
}

Float RenderNode::nGetAlpha(
    /* [in] */ Int64 renderNodePtr)
{
    NRenderNode* renderNode = reinterpret_cast<NRenderNode*>(renderNodePtr);
    return renderNode->stagingProperties().getAlpha();
}

Float RenderNode::nGetCameraDistance(
    /* [in] */ Int64 renderNodePtr)
{
    NRenderNode* renderNode = reinterpret_cast<NRenderNode*>(renderNodePtr);
    return renderNode->stagingProperties().getCameraDistance();
}

Float RenderNode::nGetScaleX(
    /* [in] */ Int64 renderNodePtr)
{
    NRenderNode* renderNode = reinterpret_cast<NRenderNode*>(renderNodePtr);
    return renderNode->stagingProperties().getScaleX();
}

Float RenderNode::nGetScaleY(
    /* [in] */ Int64 renderNodePtr)
{
    NRenderNode* renderNode = reinterpret_cast<NRenderNode*>(renderNodePtr);
    return renderNode->stagingProperties().getScaleY();
}

Float RenderNode::nGetElevation(
    /* [in] */ Int64 renderNodePtr)
{
    NRenderNode* renderNode = reinterpret_cast<NRenderNode*>(renderNodePtr);
    return renderNode->stagingProperties().getElevation();
}

Float RenderNode::nGetTranslationX(
    /* [in] */ Int64 renderNodePtr)
{
    NRenderNode* renderNode = reinterpret_cast<NRenderNode*>(renderNodePtr);
    return renderNode->stagingProperties().getTranslationX();
}

Float RenderNode::nGetTranslationY(
    /* [in] */ Int64 renderNodePtr)
{
    NRenderNode* renderNode = reinterpret_cast<NRenderNode*>(renderNodePtr);
    return renderNode->stagingProperties().getTranslationY();
}

Float RenderNode::nGetTranslationZ(
    /* [in] */ Int64 renderNodePtr)
{
    NRenderNode* renderNode = reinterpret_cast<NRenderNode*>(renderNodePtr);
    return renderNode->stagingProperties().getTranslationZ();
}

Float RenderNode::nGetRotation(
    /* [in] */ Int64 renderNodePtr)
{
    NRenderNode* renderNode = reinterpret_cast<NRenderNode*>(renderNodePtr);
    return renderNode->stagingProperties().getRotation();
}

Float RenderNode::nGetRotationX(
    /* [in] */ Int64 renderNodePtr)
{
    NRenderNode* renderNode = reinterpret_cast<NRenderNode*>(renderNodePtr);
    return renderNode->stagingProperties().getRotationX();
}

Float RenderNode::nGetRotationY(
    /* [in] */ Int64 renderNodePtr)
{
    NRenderNode* renderNode = reinterpret_cast<NRenderNode*>(renderNodePtr);
    return renderNode->stagingProperties().getRotationY();
}

Boolean RenderNode::nIsPivotExplicitlySet(
    /* [in] */ Int64 renderNodePtr)
{
    NRenderNode* renderNode = reinterpret_cast<NRenderNode*>(renderNodePtr);
    return renderNode->stagingProperties().isPivotExplicitlySet();
}

Float RenderNode::nGetPivotX(
    /* [in] */ Int64 renderNodePtr)
{
    NRenderNode* renderNode = reinterpret_cast<NRenderNode*>(renderNodePtr);
    renderNode->mutateStagingProperties().updateMatrix();
    return renderNode->stagingProperties().getPivotX();
}

Float RenderNode::nGetPivotY(
    /* [in] */ Int64 renderNodePtr)
{
    NRenderNode* renderNode = reinterpret_cast<NRenderNode*>(renderNodePtr);
    renderNode->mutateStagingProperties().updateMatrix();
    return renderNode->stagingProperties().getPivotY();
}

void RenderNode::nOutput(
    /* [in] */ Int64 renderNodePtr)
{
    NRenderNode* renderNode = reinterpret_cast<NRenderNode*>(renderNodePtr);
    renderNode->output();
}

Int32 RenderNode::nGetDebugSize(
    /* [in] */ Int64 renderNodePtr)
{
    NRenderNode* renderNode = reinterpret_cast<NRenderNode*>(renderNodePtr);
    return renderNode->getDebugSize();
}

///////////////////////////////////////////////////////////////////////////
// Animations
///////////////////////////////////////////////////////////////////////////

void RenderNode::nAddAnimator(
    /* [in] */ Int64 renderNodePtr,
    /* [in] */ Int64 animatorPtr)
{
    NRenderNode* renderNode = reinterpret_cast<NRenderNode*>(renderNodePtr);
    RenderPropertyAnimator* animator = reinterpret_cast<RenderPropertyAnimator*>(animatorPtr);
    renderNode->addAnimator(animator);
}

void RenderNode::nEndAllAnimators(
    /* [in] */ Int64 renderNodePtr)
{
    NRenderNode* renderNode = reinterpret_cast<NRenderNode*>(renderNodePtr);
    renderNode->animators().endAllStagingAnimators();
}

} // view
} // droid
} // elastos
