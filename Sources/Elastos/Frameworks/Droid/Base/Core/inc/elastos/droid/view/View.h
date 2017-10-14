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

#ifndef __ELASTOS_DROID_VIEW_View_H__
#define __ELASTOS_DROID_VIEW_View_H__

#include "Elastos.Droid.Graphics.h"
#include "Elastos.Droid.Internal.h"
#include <elastos/core/Runnable.h>
#include <elastos/droid/utility/FloatProperty.h>
#include <elastos/droid/utility/Pools.h>
#include <elastos/droid/utility/Int64SparseInt64Array.h>
#include <elastos/droid/view/AbsSavedState.h>
#include <elastos/utility/etl/List.h>
#include <elastos/utility/etl/HashMap.h>

using Elastos::Droid::Animation::IStateListAnimator;
using Elastos::Droid::Content::IClipData;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Content::Res::IConfiguration;
using Elastos::Droid::Content::Res::ITypedArray;
using Elastos::Droid::Graphics::IPaint;
using Elastos::Droid::Graphics::IPoint;
using Elastos::Droid::Graphics::IRect;
using Elastos::Droid::Graphics::IRectF;
using Elastos::Droid::Graphics::IMatrix;
using Elastos::Droid::Graphics::ICamera;
using Elastos::Droid::Graphics::IRegion;
using Elastos::Droid::Graphics::IInsets;
using Elastos::Droid::Graphics::ICanvas;
using Elastos::Droid::Graphics::IBitmap;
using Elastos::Droid::Graphics::IShader;
using Elastos::Droid::Graphics::IOutline;
using Elastos::Droid::Graphics::BitmapConfig;
using Elastos::Droid::Graphics::PorterDuffMode;
using Elastos::Droid::Graphics::Drawable::IDrawable;
using Elastos::Core::Runnable;
using Elastos::Droid::Os::IBinder;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::View::AbsSavedState;
using Elastos::Droid::View::Animation::IAnimation;
using Elastos::Droid::View::Animation::ITransformation;
using Elastos::Droid::View::Accessibility::IAccessibilityEvent;
using Elastos::Droid::View::Accessibility::IAccessibilityNodeInfo;
using Elastos::Droid::View::Accessibility::IAccessibilityNodeProvider;
using Elastos::Droid::View::Accessibility::IAccessibilityEventSource;
using Elastos::Droid::View::InputMethod::IEditorInfo;
using Elastos::Droid::View::InputMethod::IInputConnection;
using Elastos::Droid::View::IKeyEventCallback;
using Elastos::Droid::Graphics::Drawable::IDrawableCallback;
using Elastos::Droid::Utility::IAttributeSet;
using Elastos::Droid::Utility::Pools;
using Elastos::Droid::Utility::Int64SparseInt64Array;
using Elastos::Droid::Internal::Utility::IPredicate;
using Elastos::Droid::Widget::IScrollBarDrawable;
using Elastos::Droid::Utility::IProperty;
using Elastos::Droid::Utility::ISparseArray;
using Elastos::Droid::Utility::FloatProperty;
using Elastos::Droid::Content::Res::IColorStateList;
using Elastos::Core::ICharSequence;
using Elastos::Utility::IList;
using Elastos::Utility::IMap;
using Elastos::Utility::IArrayList;
using Elastos::Utility::Etl::List;
using Elastos::Utility::Etl::HashMap;

namespace Elastos {
namespace Droid {
namespace View {

class LayoutInflater;
class ViewRootImpl;
class TouchDelegate;
class ViewPropertyAnimator;
class CAccessibilityInteractionController;
class COverlayViewGroup;
class RenderNodeAnimator;

#ifndef VIEW_PROBE
#define VIEW_PROBE(host) ((View*)IView::Probe(host))
#endif

class ECO_PUBLIC View
    : public Object
    , public IView
    , public IDrawableCallback
    , public IKeyEventCallback
    , public IAccessibilityEventSource

{
    friend class RenderNode;
    friend class RenderNodeAnimator;
    friend class ThreadedRenderer;
    friend class ViewRootImpl;
    friend class LayoutInflater;
    friend class ViewGroup;
    friend class ViewPropertyAnimator;
    friend class CAccessibilityInteractionController;
    friend class COverlayViewGroup;
    friend class GhostView;

//protected:
public:
    /**
     * Mask for use with setFlags indicating bits used for visibility.
     * {@hide}
     */
    static const Int32 VISIBILITY_MASK = 0x0000000C;

     /**
     * This view is enabled. Intrepretation varies by subclass.
     * Use with ENABLED_MASK when calling setFlags.
     * {@hide}
     */
    static const Int32 ENABLED;

    /**
     * This view is disabled. Intrepretation varies by subclass.
     * Use with ENABLED_MASK when calling setFlags.
     * {@hide}
     */
    static const Int32 DISABLED;

   /**
    * Mask for use with setFlags indicating bits used for indicating whether
    * this view is enabled
    * {@hide}
    */
    static const Int32 ENABLED_MASK;

    /**
     * This view won't draw. {@link #onDraw} won't be called and further
     * optimizations
     * will be performed. It is okay to have this flag set and a background.
     * Use with DRAW_MASK when calling setFlags.
     * {@hide}
     */
    static const Int32 WILL_NOT_DRAW;

    /**
     * Mask for use with setFlags indicating bits used for indicating whether
     * this view is will draw
     * {@hide}
     */
    static const Int32 DRAW_MASK;

    /**
     * <p>This view doesn't show scrollbars.</p>
     * {@hide}
     */
    static const Int32 SCROLLBARS_NONE;

    /**
     * <p>This view shows horizontal scrollbars.</p>
     * {@hide}
     */
    static const Int32 SCROLLBARS_HORIZONTAL;

    /**
     * <p>This view shows vertical scrollbars.</p>
     * {@hide}
     */
    static const Int32 SCROLLBARS_VERTICAL;

    /**
     * <p>Mask for use with setFlags indicating bits used for indicating which
     * scrollbars are enabled.</p>
     * {@hide}
     */
    static const Int32 SCROLLBARS_MASK;

    /**
     * Indicates that the view should filter touches when its window is obscured.
     * Refer to the class comments for more information about this security feature.
     * {@hide}
     */
    static const Int32 FILTER_TOUCHES_WHEN_OBSCURED;

    /**
     * Set for framework elements that use FITS_SYSTEM_WINDOWS, to indicate
     * that they are optional and should be skipped if the window has
     * requested system UI flags that ignore those insets for layout.
     */
    static const Int32 OPTIONAL_FITS_SYSTEM_WINDOWS;

    /**
     * <p>This view doesn't show fading edges.</p>
     * {@hide}
     */
    static const Int32 FADING_EDGE_NONE;

    /**
     * <p>This view shows horizontal fading edges.</p>
     * {@hide}
     */
    static const Int32 FADING_EDGE_HORIZONTAL;

    /**
     * <p>This view shows vertical fading edges.</p>
     * {@hide}
     */
    static const Int32 FADING_EDGE_VERTICAL;

    /**
     * <p>Mask for use with setFlags indicating bits used for indicating which
     * fading edges are enabled.</p>
     * {@hide}
     */
    static const Int32 FADING_EDGE_MASK;

    /**
     * <p>Indicates this view can be clicked. When clickable, a View reacts
     * to clicks by notifying the OnClickListener.<p>
     * {@hide}
     */
    static const Int32 CLICKABLE;

    /**
     * <p>Indicates this view is caching its drawing into a bitmap.</p>
     * {@hide}
     */
    static const Int32 DRAWING_CACHE_ENABLED;

    /**
     * <p>Indicates that no icicle should be saved for this view.<p>
     * {@hide}
     */
    static const Int32 SAVE_DISABLED;

    /**
     * <p>Mask for use with setFlags indicating bits used for the saveEnabled
     * property.</p>
     * {@hide}
     */
    static const Int32 SAVE_DISABLED_MASK;

    /**
     * <p>Indicates that no drawing cache should ever be created for this view.<p>
     * {@hide}
     */
    static const Int32 WILL_NOT_CACHE_DRAWING;

    /**
     * <p>Indicates this view can take / keep focus when int touch mode.</p>
     * {@hide}
     */
    static const Int32 FOCUSABLE_IN_TOUCH_MODE;

    /**
     * <p>Mask for use with setFlags indicating bits used for the cache
     * quality property.</p>
     * {@hide}
     */
    static const Int32 DRAWING_CACHE_QUALITY_MASK;

    /**
     * <p>
     * Indicates this view can be long clicked. When long clickable, a View
     * reacts to long clicks by notifying the OnLongClickListener or showing a
     * context menu.
     * </p>
     * {@hide}
     */
    static const Int32 LONG_CLICKABLE;

    /**
     * <p>Indicates that this view gets its drawable states from its direct parent
     * and ignores its original internal states.</p>
     *
     * @hide
     */
    static const Int32 DUPLICATE_PARENT_STATE;

    /**
     * Mask to check if the scrollbar style is overlay or inset.
     * {@hide}
     */
    static const Int32 SCROLLBARS_INSET_MASK;

    /**
     * Mask to check if the scrollbar style is inside or outside.
     * {@hide}
     */
    static const Int32 SCROLLBARS_OUTSIDE_MASK;

    /**
     * Mask for scrollbar style.
     * {@hide}
     */
    static const Int32 SCROLLBARS_STYLE_MASK;

    /**
     * <p>Indicates that the view hierarchy should stop saving state when
     * it reaches this view.  If state saving is initiated immediately at
     * the view, it will be allowed.
     * {@hide}
     */
    static const Int32 PARENT_SAVE_DISABLED;

    /**
     * <p>Mask for use with setFlags indicating bits used for PARENT_SAVE_DISABLED.</p>
     * {@hide}
     */
    static const Int32 PARENT_SAVE_DISABLED_MASK;

    static const Int32 VIEW_STATE_WINDOW_FOCUSED;
    static const Int32 VIEW_STATE_SELECTED;
    static const Int32 VIEW_STATE_FOCUSED;
    static const Int32 VIEW_STATE_ENABLED;
    static const Int32 VIEW_STATE_PRESSED;
    static const Int32 VIEW_STATE_ACTIVATED;
    static const Int32 VIEW_STATE_ACCELERATED;
    static const Int32 VIEW_STATE_HOVERED;
    static const Int32 VIEW_STATE_DRAG_CAN_ACCEPT;
    static const Int32 VIEW_STATE_DRAG_HOVERED;
    static AutoPtr<ArrayOf<Int32> > VIEW_STATE_IDS;

    // for mPrivateFlags:
    /** {@hide} */
    static const Int32 PFLAG_WANTS_FOCUS;
    /** {@hide} */
    static const Int32 PFLAG_FOCUSED;
    /** {@hide} */
    static const Int32 PFLAG_SELECTED;
    /** {@hide} */
    static const Int32 PFLAG_IS_ROOT_NAMESPACE;
    /** {@hide} */
    static const Int32 PFLAG_HAS_BOUNDS;
    /** {@hide} */
    static const Int32 PFLAG_DRAWN;
    /**
     * When this flag is set, this view is running an animation on behalf of its
     * children and should therefore not cancel invalidate requests, even if they
     * lie outside of this view's bounds.
     *
     * {@hide}
     */
    static const Int32 PFLAG_DRAW_ANIMATION;
    /** {@hide} */
    static const Int32 PFLAG_SKIP_DRAW;
    /** {@hide} */
    static const Int32 PFLAG_ONLY_DRAWS_BACKGROUND;
    /** {@hide} */
    static const Int32 PFLAG_REQUEST_TRANSPARENT_REGIONS;
    /** {@hide} */
    static const Int32 PFLAG_DRAWABLE_STATE_DIRTY;
    /** {@hide} */
    static const Int32 PFLAG_MEASURED_DIMENSION_SET;
    /** {@hide} */
    static const Int32 PFLAG_FORCE_LAYOUT;
    /** {@hide} */
    static const Int32 PFLAG_LAYOUT_REQUIRED;

    /** {@hide} */
    static const Int32 PFLAG_DRAWING_CACHE_VALID;

    /**
     * Flag used to indicate that this view should be drawn once more (and only once
     * more) after its animation has completed.
     * {@hide}
     */
    static const Int32 PFLAG_ANIMATION_STARTED;

    /**
     * Indicates that the View returned true when onSetAlpha() was called and that
     * the alpha must be restored.
     * {@hide}
     */
    static const Int32 PFLAG_ALPHA_SET;

    /**
     * Set by {@link #setScrollContainer(boolean)}.
     */
    static const Int32 PFLAG_SCROLL_CONTAINER;

    /**
     * Set by {@link #setScrollContainer(boolean)}.
     */
    static const Int32 PFLAG_SCROLL_CONTAINER_ADDED;

    /**
     * View flag indicating whether this view was invalidated (fully or partially.)
     *
     * @hide
     */
    static const Int32 PFLAG_DIRTY;

    /**
     * View flag indicating whether this view was invalidated by an opaque
     * invalidate request.
     *
     * @hide
     */
    static const Int32 PFLAG_DIRTY_OPAQUE;

    /**
     * Mask for {@link #PFLAG_DIRTY} and {@link #PFLAG_DIRTY_OPAQUE}.
     *
     * @hide
     */
    static const Int32 PFLAG_DIRTY_MASK;

    /**
     * Indicates whether the background is opaque.
     *
     * @hide
     */
    static const Int32 PFLAG_OPAQUE_BACKGROUND;

    /**
     * Indicates whether the scrollbars are opaque.
     *
     * @hide
     */
    static const Int32 PFLAG_OPAQUE_SCROLLBARS;

    /**
     * Indicates whether the view is opaque.
     *
     * @hide
     */
    static const Int32 PFLAG_OPAQUE_MASK;

    /**
     * Indicates whether the view is temporarily detached.
     *
     * @hide
     */
    static const Int32 PFLAG_CANCEL_NEXT_UP_EVENT;


    /** {@hide} */
    static const Int32 PFLAG_ACTIVATED;

    /**
     * Indicates that this view was specifically invalidated, not just dirtied because some
     * child view was invalidated. The flag is used to determine when we need to recreate
     * a view's display list (as opposed to just returning a reference to its existing
     * display list).
     *
     * @hide
     */
    static const Int32 PFLAG_INVALIDATED;

    /**
     * Masks for mPrivateFlags2, as generated by dumpFlags():
     *
     * -------|-------|-------|-------|
     *                                  PFLAG2_TEXT_ALIGNMENT_FLAGS[0]
     *                                  PFLAG2_TEXT_DIRECTION_FLAGS[0]
     *                                1 PFLAG2_DRAG_CAN_ACCEPT
     *                               1  PFLAG2_DRAG_HOVERED
     *                               1  PFLAG2_LAYOUT_DIRECTION_MASK_SHIFT
     *                              11  PFLAG2_TEXT_DIRECTION_MASK_SHIFT
     *                             1 1  PFLAG2_TEXT_DIRECTION_RESOLVED_MASK_SHIFT
     *                             11   PFLAG2_LAYOUT_DIRECTION_MASK
     *                             11 1 PFLAG2_TEXT_ALIGNMENT_MASK_SHIFT
     *                            1     PFLAG2_LAYOUT_DIRECTION_RESOLVED_RTL
     *                            1   1 PFLAG2_TEXT_ALIGNMENT_RESOLVED_MASK_SHIFT
     *                            1 1   PFLAG2_IMPORTANT_FOR_ACCESSIBILITY_SHIFT
     *                           1      PFLAG2_LAYOUT_DIRECTION_RESOLVED
     *                           11     PFLAG2_LAYOUT_DIRECTION_RESOLVED_MASK
     *                          1       PFLAG2_TEXT_DIRECTION_FLAGS[1]
     *                         1        PFLAG2_TEXT_DIRECTION_FLAGS[2]
     *                         11       PFLAG2_TEXT_DIRECTION_FLAGS[3]
     *                        1         PFLAG2_TEXT_DIRECTION_FLAGS[4]
     *                        1 1       PFLAG2_TEXT_DIRECTION_FLAGS[5]
     *                        111       PFLAG2_TEXT_DIRECTION_MASK
     *                       1          PFLAG2_TEXT_DIRECTION_RESOLVED
     *                      1           PFLAG2_TEXT_DIRECTION_RESOLVED_DEFAULT
     *                    111           PFLAG2_TEXT_DIRECTION_RESOLVED_MASK
     *                   1              PFLAG2_TEXT_ALIGNMENT_FLAGS[1]
     *                  1               PFLAG2_TEXT_ALIGNMENT_FLAGS[2]
     *                  11              PFLAG2_TEXT_ALIGNMENT_FLAGS[3]
     *                 1                PFLAG2_TEXT_ALIGNMENT_FLAGS[4]
     *                 1 1              PFLAG2_TEXT_ALIGNMENT_FLAGS[5]
     *                 11               PFLAG2_TEXT_ALIGNMENT_FLAGS[6]
     *                 111              PFLAG2_TEXT_ALIGNMENT_MASK
     *                1                 PFLAG2_TEXT_ALIGNMENT_RESOLVED
     *               1                  PFLAG2_TEXT_ALIGNMENT_RESOLVED_DEFAULT
     *             111                  PFLAG2_TEXT_ALIGNMENT_RESOLVED_MASK
     *           11                     PFLAG2_IMPORTANT_FOR_ACCESSIBILITY_MASK
     *          1                       PFLAG2_HAS_TRANSIENT_STATE
     *      1                           PFLAG2_ACCESSIBILITY_FOCUSED
     *     1                            PFLAG2_ACCESSIBILITY_STATE_CHANGED
     *    1                             PFLAG2_VIEW_QUICK_REJECTED
     *   1                              PFLAG2_PADDING_RESOLVED
     * -------|-------|-------|-------|
     */

    /**
     * Indicates that this view has reported that it can accept the current drag's content.
     * Cleared when the drag operation concludes.
     * @hide
     */
    static const Int32 PFLAG2_DRAG_CAN_ACCEPT;

    /**
     * Indicates that this view is currently directly under the drag location in a
     * drag-and-drop operation involving content that it can accept.  Cleared when
     * the drag exits the view, or when the drag operation concludes.
     * @hide
     */
    static const Int32 PFLAG2_DRAG_HOVERED;

    /**
     * Bit shift to get the horizontal layout direction. (bits after DRAG_HOVERED)
     * @hide
     */
    static const Int32 PFLAG2_LAYOUT_DIRECTION_MASK_SHIFT;

    /**
     * Mask for use with private flags indicating bits used for horizontal layout direction.
     * @hide
     */
    static const Int32 PFLAG2_LAYOUT_DIRECTION_MASK;

    /**
     * Indicates whether the view horizontal layout direction has been resolved and drawn to the
     * right-to-left direction.
     * @hide
     */
    static const Int32 PFLAG2_LAYOUT_DIRECTION_RESOLVED_RTL;

    /**
     * Indicates whether the view horizontal layout direction has been resolved.
     * @hide
     */
    static const Int32 PFLAG2_LAYOUT_DIRECTION_RESOLVED;

    /**
     * Mask for use with private flags indicating bits used for resolved horizontal layout direction.
     * @hide
     */
    static const Int32 PFLAG2_LAYOUT_DIRECTION_RESOLVED_MASK;

    /**
     * Indicates that the view is tracking some sort of transient state
     * that the app should not need to be aware of, but that the framework
     * should take special care to preserve.
     *
     * @hide
     */
    static const Int32 PFLAG2_HAS_TRANSIENT_STATE;

    static const Int32 LAYOUT_DIRECTION_RESOLVED_DEFAULT;// = LAYOUT_DIRECTION_LTR;

    /**
     * Bit shift to get the horizontal layout direction. (bits after LAYOUT_DIRECTION_RESOLVED)
     * @hide
     */
    static const Int32 PFLAG2_TEXT_DIRECTION_MASK_SHIFT;

    /**
     * Mask for use with private flags indicating bits used for text direction.
     * @hide
     */
    static const Int32 PFLAG2_TEXT_DIRECTION_MASK;

    /**
     * Indicates whether the view text direction has been resolved.
     * @hide
     */
    static const Int32 PFLAG2_TEXT_DIRECTION_RESOLVED;

    /**
     * Bit shift to get the horizontal layout direction. (bits after DRAG_HOVERED)
     * @hide
     */
    static const Int32 PFLAG2_TEXT_DIRECTION_RESOLVED_MASK_SHIFT;

    /**
     * Mask for use with private flags indicating bits used for resolved text direction.
     * @hide
     */
    static const Int32 PFLAG2_TEXT_DIRECTION_RESOLVED_MASK;

    /**
     * Indicates whether the view text direction has been resolved to the "first strong" heuristic.
     * @hide
     */
    static const Int32 PFLAG2_TEXT_DIRECTION_RESOLVED_DEFAULT;

    /**
      * Bit shift to get the horizontal layout direction. (bits after DRAG_HOVERED)
      * @hide
      */
    static const Int32 PFLAG2_TEXT_ALIGNMENT_MASK_SHIFT;

    /**
      * Mask for use with private flags indicating bits used for text alignment.
      * @hide
      */
    static const Int32 PFLAG2_TEXT_ALIGNMENT_MASK;

    /**
     * Indicates whether the view text alignment has been resolved.
     * @hide
     */
    static const Int32 PFLAG2_TEXT_ALIGNMENT_RESOLVED;

    /**
     * Bit shift to get the resolved text alignment.
     * @hide
     */
    static const Int32 PFLAG2_TEXT_ALIGNMENT_RESOLVED_MASK_SHIFT;

    /**
     * Mask for use with private flags indicating bits used for text alignment.
     * @hide
     */
    static const Int32 PFLAG2_TEXT_ALIGNMENT_RESOLVED_MASK;

    // Accessiblity constants for mPrivateFlags2

    /**
     * Shift for the bits in {@link #mPrivateFlags2} related to the
     * "importantForAccessibility" attribute.
     */
    static const Int32 PFLAG2_IMPORTANT_FOR_ACCESSIBILITY_SHIFT;

    /**
     * The default whether the view is important for accessibility.
     */
    static const Int32 IMPORTANT_FOR_ACCESSIBILITY_DEFAULT;

    /**
     * Mask for obtainig the bits which specify how to determine
     * whether a view is important for accessibility.
     */
    static const Int32 PFLAG2_IMPORTANT_FOR_ACCESSIBILITY_MASK;

    static const Int32 PFLAG2_ACCESSIBILITY_LIVE_REGION_SHIFT;// = 23;

    static const Int32 ACCESSIBILITY_LIVE_REGION_DEFAULT;// = ACCESSIBILITY_LIVE_REGION_NONE;

    static const Int32 PFLAG2_ACCESSIBILITY_LIVE_REGION_MASK;

    /**
     * Flag indicating whether a view has accessibility focus.
     */
    static const Int32 PFLAG2_ACCESSIBILITY_FOCUSED;

    /**
     * Flag indicating whether a view state for accessibility has changed.
     */
    static const Int32 PFLAG2_SUBTREE_ACCESSIBILITY_STATE_CHANGED;

    /**
     * Flag indicating whether a view failed the quickReject() check in draw(). This condition
     * is used to check whether later changes to the view's transform should invalidate the
     * view to force the quickReject test to run again.
     */
    static const Int32 PFLAG2_VIEW_QUICK_REJECTED;

    /**
     * Flag indicating that start/end padding has been resolved into left/right padding
     * for use in measurement, layout, drawing, etc. This is set by {@link #resolvePadding()}
     * and checked by {@link #measure(int, int)} to determine if padding needs to be resolved
     * during measurement. In some special cases this is required such as when an adapter-based
     * view measures prospective children without attaching them to a window.
     */
    static const Int32 PFLAG2_PADDING_RESOLVED;

    /**
     * Flag indicating that the start/end drawables has been resolved into left/right ones.
     */
    static const Int32 PFLAG2_DRAWABLE_RESOLVED;

    /**
     * Group of bits indicating that RTL properties resolution is done.
     */
    static const Int32 ALL_RTL_PROPERTIES_RESOLVED;

    // There are a couple of flags left in mPrivateFlags2

    /* End of masks for mPrivateFlags2 */

    /* Masks for mPrivateFlags3 */

    /**
     * Flag indicating that view has a transform animation set on it. This is used to track whether
     * an animation is cleared between successive frames, in order to tell the associated
     * DisplayList to clear its animation matrix.
     */
    static const Int32 PFLAG3_VIEW_IS_ANIMATING_TRANSFORM;

    /**
     * Flag indicating that view has an alpha animation set on it. This is used to track whether an
     * animation is cleared between successive frames, in order to tell the associated
     * DisplayList to restore its alpha value.
     */
    static const Int32 PFLAG3_VIEW_IS_ANIMATING_ALPHA;

    /**
     * Flag indicating that the view has been through at least one layout since it
     * was last attached to a window.
     */
    static const Int32 PFLAG3_IS_LAID_OUT;// = 0x4;

    /**
     * Flag indicating that a call to measure() was skipped and should be done
     * instead when layout() is invoked.
     */
    static const Int32 PFLAG3_MEASURE_NEEDED_BEFORE_LAYOUT;// = 0x8;

    /**
     * Flag indicating that an overridden method correctly called down to
     * the superclass implementation as required by the API spec.
     */
    static const Int32 PFLAG3_CALLED_SUPER;// = 0x10;

    /**
     * Flag indicating that we're in the process of applying window insets.
     */
    static const Int32 PFLAG3_APPLYING_INSETS;// = 0x20;

    /**
     * Flag indicating that we're in the process of fitting system windows using the old method.
     */
    static const Int32 PFLAG3_FITTING_SYSTEM_WINDOWS;// = 0x40;

    /**
     * Flag indicating that nested scrolling is enabled for this view.
     * The view will optionally cooperate with views up its parent chain to allow for
     * integrated nested scrolling along the same axis.
     */
    static const Int32 PFLAG3_NESTED_SCROLLING_ENABLED;// = 0x80;

    /**
     * Flag indicating that outline was invalidated and should be rebuilt the next time
     * the DisplayList is updated.
     */
    static const Int32 PFLAG3_OUTLINE_INVALID;// = 0x100;

    /* End of masks for mPrivateFlags3 */

    static const Int32 DRAG_MASK;

    /**
     * A Property wrapper around the <code>alpha</code> functionality handled by the
     * {@link View#setAlpha(float)} and {@link View#getAlpha()} methods.
     */
    static const AutoPtr<IProperty> ALPHA;

    /**
     * A Property wrapper around the <code>translationX</code> functionality handled by the
     * {@link View#setTranslationX(float)} and {@link View#getTranslationX()} methods.
     */
    static const AutoPtr<IProperty> TRANSLATION_X;

    /**
     * A Property wrapper around the <code>translationY</code> functionality handled by the
     * {@link View#setTranslationY(float)} and {@link View#getTranslationY()} methods.
     */
    static const AutoPtr<IProperty> TRANSLATION_Y;

    /**
    * A Property wrapper around the <code>translationZ</code> functionality handled by the
    * {@link View#setTranslationZ(float)} and {@link View#getTranslationZ()} methods.
    */
    static const AutoPtr<IProperty> TRANSLATION_Z;

    /**
     * A Property wrapper around the <code>x</code> functionality handled by the
     * {@link View#setX(float)} and {@link View#getX()} methods.
     */
    static const AutoPtr<IProperty> X;

    /**
     * A Property wrapper around the <code>y</code> functionality handled by the
     * {@link View#setY(float)} and {@link View#getY()} methods.
     */
    static const AutoPtr<IProperty> Y;

    /**
     * A Property wrapper around the <code>z</code> functionality handled by the
     * {@link View#setZ(float)} and {@link View#getZ()} methods.
     */
    static const AutoPtr<IProperty> Z;

    /**
     * A Property wrapper around the <code>rotation</code> functionality handled by the
     * {@link View#setRotation(float)} and {@link View#getRotation()} methods.
     */
    static const AutoPtr<IProperty> ROTATION;

    /**
     * A Property wrapper around the <code>rotationX</code> functionality handled by the
     * {@link View#setRotationX(float)} and {@link View#getRotationX()} methods.
     */
    static const AutoPtr<IProperty> ROTATION_X;

    /**
     * A Property wrapper around the <code>rotationY</code> functionality handled by the
     * {@link View#setRotationY(float)} and {@link View#getRotationY()} methods.
     */
    static const AutoPtr<IProperty> ROTATION_Y;

    /**
     * A Property wrapper around the <code>scaleX</code> functionality handled by the
     * {@link View#setScaleX(float)} and {@link View#getScaleX()} methods.
     */
    static const AutoPtr<IProperty> SCALE_X;

    /**
     * A Property wrapper around the <code>scaleY</code> functionality handled by the
     * {@link View#setScaleY(float)} and {@link View#getScaleY()} methods.
     */
    static const AutoPtr<IProperty> SCALE_Y;

    // static CARAPI_(AutoPtr<IFloatProperty>) InitProperty(
    //     /* [in] */ const String& name);

protected:
    /**
     * Base View state sets
     */
    // Singles
    /**
     * Indicates the view has no states set. States are used with
     * {@link android.graphics.drawable.Drawable} to change the drawing of the
     * view depending on its state.
     *
     * @see android.graphics.drawable.Drawable
     * @see #getDrawableState()
     */
    static AutoPtr<ArrayOf<Int32> > EMPTY_STATE_SET;
    /**
     * Indicates the view is enabled. States are used with
     * {@link android.graphics.drawable.Drawable} to change the drawing of the
     * view depending on its state.
     *
     * @see android.graphics.drawable.Drawable
     * @see #getDrawableState()
     */
    static AutoPtr<ArrayOf<Int32> > ENABLED_STATE_SET;
    /**
     * Indicates the view is focused. States are used with
     * {@link android.graphics.drawable.Drawable} to change the drawing of the
     * view depending on its state.
     *
     * @see android.graphics.drawable.Drawable
     * @see #getDrawableState()
     */
    static AutoPtr<ArrayOf<Int32> > FOCUSED_STATE_SET;
    /**
     * Indicates the view is selected. States are used with
     * {@link android.graphics.drawable.Drawable} to change the drawing of the
     * view depending on its state.
     *
     * @see android.graphics.drawable.Drawable
     * @see #getDrawableState()
     */
    static AutoPtr<ArrayOf<Int32> > SELECTED_STATE_SET;
    /**
     * Indicates the view is pressed. States are used with
     * {@link android.graphics.drawable.Drawable} to change the drawing of the
     * view depending on its state.
     *
     * @see android.graphics.drawable.Drawable
     * @see #getDrawableState()
     * @hide
     */
    static AutoPtr<ArrayOf<Int32> > PRESSED_STATE_SET;
    /**
     * Indicates the view's window has focus. States are used with
     * {@link android.graphics.drawable.Drawable} to change the drawing of the
     * view depending on its state.
     *
     * @see android.graphics.drawable.Drawable
     * @see #getDrawableState()
     */
    static AutoPtr<ArrayOf<Int32> > WINDOW_FOCUSED_STATE_SET;
    // Doubles
    /**
     * Indicates the view is enabled and has the focus.
     *
     * @see #ENABLED_STATE_SET
     * @see #FOCUSED_STATE_SET
     */
    static AutoPtr<ArrayOf<Int32> > ENABLED_FOCUSED_STATE_SET;
    /**
     * Indicates the view is enabled and selected.
     *
     * @see #ENABLED_STATE_SET
     * @see #SELECTED_STATE_SET
     */
    static AutoPtr<ArrayOf<Int32> > ENABLED_SELECTED_STATE_SET;
    /**
     * Indicates the view is enabled and that its window has focus.
     *
     * @see #ENABLED_STATE_SET
     * @see #WINDOW_FOCUSED_STATE_SET
     */
    static AutoPtr<ArrayOf<Int32> > ENABLED_WINDOW_FOCUSED_STATE_SET;
    /**
     * Indicates the view is focused and selected.
     *
     * @see #FOCUSED_STATE_SET
     * @see #SELECTED_STATE_SET
     */
    static AutoPtr<ArrayOf<Int32> > FOCUSED_SELECTED_STATE_SET;
    /**
     * Indicates the view has the focus and that its window has the focus.
     *
     * @see #FOCUSED_STATE_SET
     * @see #WINDOW_FOCUSED_STATE_SET
     */
    static AutoPtr<ArrayOf<Int32> > FOCUSED_WINDOW_FOCUSED_STATE_SET;
    /**
     * Indicates the view is selected and that its window has the focus.
     *
     * @see #SELECTED_STATE_SET
     * @see #WINDOW_FOCUSED_STATE_SET
     */
    static AutoPtr<ArrayOf<Int32> > SELECTED_WINDOW_FOCUSED_STATE_SET;
    // Triples
    /**
     * Indicates the view is enabled, focused and selected.
     *
     * @see #ENABLED_STATE_SET
     * @see #FOCUSED_STATE_SET
     * @see #SELECTED_STATE_SET
     */
    static AutoPtr<ArrayOf<Int32> > ENABLED_FOCUSED_SELECTED_STATE_SET;
    /**
     * Indicates the view is enabled, focused and its window has the focus.
     *
     * @see #ENABLED_STATE_SET
     * @see #FOCUSED_STATE_SET
     * @see #WINDOW_FOCUSED_STATE_SET
     */
    static AutoPtr<ArrayOf<Int32> > ENABLED_FOCUSED_WINDOW_FOCUSED_STATE_SET;
    /**
     * Indicates the view is enabled, selected and its window has the focus.
     *
     * @see #ENABLED_STATE_SET
     * @see #SELECTED_STATE_SET
     * @see #WINDOW_FOCUSED_STATE_SET
     */
    static AutoPtr<ArrayOf<Int32> > ENABLED_SELECTED_WINDOW_FOCUSED_STATE_SET;
    /**
     * Indicates the view is focused, selected and its window has the focus.
     *
     * @see #FOCUSED_STATE_SET
     * @see #SELECTED_STATE_SET
     * @see #WINDOW_FOCUSED_STATE_SET
     */
    static AutoPtr<ArrayOf<Int32> > FOCUSED_SELECTED_WINDOW_FOCUSED_STATE_SET;
    /**
     * Indicates the view is enabled, focused, selected and its window
     * has the focus.
     *
     * @see #ENABLED_STATE_SET
     * @see #FOCUSED_STATE_SET
     * @see #SELECTED_STATE_SET
     * @see #WINDOW_FOCUSED_STATE_SET
     */
    static AutoPtr<ArrayOf<Int32> > ENABLED_FOCUSED_SELECTED_WINDOW_FOCUSED_STATE_SET;
    /**
     * Indicates the view is pressed and its window has the focus.
     *
     * @see #PRESSED_STATE_SET
     * @see #WINDOW_FOCUSED_STATE_SET
     */
    static AutoPtr<ArrayOf<Int32> > PRESSED_WINDOW_FOCUSED_STATE_SET;
    /**
     * Indicates the view is pressed and selected.
     *
     * @see #PRESSED_STATE_SET
     * @see #SELECTED_STATE_SET
     */
    static AutoPtr<ArrayOf<Int32> > PRESSED_SELECTED_STATE_SET;
    /**
     * Indicates the view is pressed, selected and its window has the focus.
     *
     * @see #PRESSED_STATE_SET
     * @see #SELECTED_STATE_SET
     * @see #WINDOW_FOCUSED_STATE_SET
     */
    static AutoPtr<ArrayOf<Int32> > PRESSED_SELECTED_WINDOW_FOCUSED_STATE_SET;
    /**
     * Indicates the view is pressed and focused.
     *
     * @see #PRESSED_STATE_SET
     * @see #FOCUSED_STATE_SET
     */
    static AutoPtr<ArrayOf<Int32> > PRESSED_FOCUSED_STATE_SET;
    /**
     * Indicates the view is pressed, focused and its window has the focus.
     *
     * @see #PRESSED_STATE_SET
     * @see #FOCUSED_STATE_SET
     * @see #WINDOW_FOCUSED_STATE_SET
     */
    static AutoPtr<ArrayOf<Int32> > PRESSED_FOCUSED_WINDOW_FOCUSED_STATE_SET;
    /**
     * Indicates the view is pressed, focused and selected.
     *
     * @see #PRESSED_STATE_SET
     * @see #SELECTED_STATE_SET
     * @see #FOCUSED_STATE_SET
     */
    static AutoPtr<ArrayOf<Int32> > PRESSED_FOCUSED_SELECTED_STATE_SET;
    /**
     * Indicates the view is pressed, focused, selected and its window has the focus.
     *
     * @see #PRESSED_STATE_SET
     * @see #FOCUSED_STATE_SET
     * @see #SELECTED_STATE_SET
     * @see #WINDOW_FOCUSED_STATE_SET
     */
    static AutoPtr<ArrayOf<Int32> > PRESSED_FOCUSED_SELECTED_WINDOW_FOCUSED_STATE_SET;
    /**
     * Indicates the view is pressed and enabled.
     *
     * @see #PRESSED_STATE_SET
     * @see #ENABLED_STATE_SET
     */
    static AutoPtr<ArrayOf<Int32> > PRESSED_ENABLED_STATE_SET;
    /**
     * Indicates the view is pressed, enabled and its window has the focus.
     *
     * @see #PRESSED_STATE_SET
     * @see #ENABLED_STATE_SET
     * @see #WINDOW_FOCUSED_STATE_SET
     */
    static AutoPtr<ArrayOf<Int32> > PRESSED_ENABLED_WINDOW_FOCUSED_STATE_SET;
    /**
     * Indicates the view is pressed, enabled and selected.
     *
     * @see #PRESSED_STATE_SET
     * @see #ENABLED_STATE_SET
     * @see #SELECTED_STATE_SET
     */
    static AutoPtr<ArrayOf<Int32> > PRESSED_ENABLED_SELECTED_STATE_SET;
    /**
     * Indicates the view is pressed, enabled, selected and its window has the
     * focus.
     *
     * @see #PRESSED_STATE_SET
     * @see #ENABLED_STATE_SET
     * @see #SELECTED_STATE_SET
     * @see #WINDOW_FOCUSED_STATE_SET
     */
    static AutoPtr<ArrayOf<Int32> > PRESSED_ENABLED_SELECTED_WINDOW_FOCUSED_STATE_SET;
    /**
     * Indicates the view is pressed, enabled and focused.
     *
     * @see #PRESSED_STATE_SET
     * @see #ENABLED_STATE_SET
     * @see #FOCUSED_STATE_SET
     */
    static AutoPtr<ArrayOf<Int32> > PRESSED_ENABLED_FOCUSED_STATE_SET;
    /**
     * Indicates the view is pressed, enabled, focused and its window has the
     * focus.
     *
     * @see #PRESSED_STATE_SET
     * @see #ENABLED_STATE_SET
     * @see #FOCUSED_STATE_SET
     * @see #WINDOW_FOCUSED_STATE_SET
     */
    static AutoPtr<ArrayOf<Int32> > PRESSED_ENABLED_FOCUSED_WINDOW_FOCUSED_STATE_SET;
    /**
     * Indicates the view is pressed, enabled, focused and selected.
     *
     * @see #PRESSED_STATE_SET
     * @see #ENABLED_STATE_SET
     * @see #SELECTED_STATE_SET
     * @see #FOCUSED_STATE_SET
     */
    static AutoPtr<ArrayOf<Int32> > PRESSED_ENABLED_FOCUSED_SELECTED_STATE_SET;
    /**
     * Indicates the view is pressed, enabled, focused, selected and its window
     * has the focus.
     *
     * @see #PRESSED_STATE_SET
     * @see #ENABLED_STATE_SET
     * @see #SELECTED_STATE_SET
     * @see #FOCUSED_STATE_SET
     * @see #WINDOW_FOCUSED_STATE_SET
     */
    static AutoPtr<ArrayOf<Int32> > PRESSED_ENABLED_FOCUSED_SELECTED_WINDOW_FOCUSED_STATE_SET;

private:
    static const Boolean DBG;

    /**
     * This view does not want keystrokes. Use with TAKES_FOCUS_MASK when
     * calling setFlags.
     */
    static const Int32 NOT_FOCUSABLE;

    /**
     * This view wants keystrokes. Use with TAKES_FOCUS_MASK when calling
     * setFlags.
     */
    static const Int32 FOCUSABLE;

    /**
     * Mask for use with setFlags indicating bits used for focus.
     */
    static const Int32 FOCUSABLE_MASK;

    /**
     * This view will adjust its padding to fit sytem windows (e.g. status bar)
     */
    static const Int32 FITS_SYSTEM_WINDOWS;

    static const Int32 VISIBILITY_FLAGS[];

    static const Int32 DRAWING_CACHE_QUALITY_FLAGS[];

    /**
     * The order here is very important to {@link #getDrawableState()}
     */
    static const AutoPtr<ArrayOf<Int32Array> > VIEW_STATE_SETS;
    static CARAPI_(const AutoPtr<ArrayOf<Int32Array> >) InitViewStateSets();

    /**
     * Accessibility event types that are dispatched for text population.
     */
    static const Int32 POPULATING_ACCESSIBILITY_EVENT_TYPES;

    static const Int32 PFLAG_PRESSED;

    static const Int32 PFLAG_SAVE_STATE_CALLED;

    /**
     * Indicates a prepressed state;
     * the short time between ACTION_DOWN and recognizing
     * a 'real' press. Prepressed is used to recognize quick taps
     * even when they are shorter than ViewConfiguration.getTapTimeout().
     *
     * @hide
     */
    static const Int32 PFLAG_PREPRESSED;

    /**
     * Indicates that we should awaken scroll bars once attached
     *
     * @hide
     */
    static const Int32 PFLAG_AWAKEN_SCROLL_BARS_ON_ATTACH;

    /**
     * Indicates that the view has received HOVER_ENTER.  Cleared on HOVER_EXIT.
     * @hide
     */
    static const Int32 PFLAG_HOVERED;

    /**
     * Indicates that pivotX or pivotY were explicitly set and we should not assume the center
     * for transform operations
     *
     * @hide
     */
    static const Int32 PFLAG_DOES_NOTHING_REUSE_PLEASE;

    /*
     * Array of horizontal layout direction flags for mapping attribute "layoutDirection" to correct
     * flag value.
     * @hide
     */
    static const Int32 LAYOUT_DIRECTION_FLAGS[];

    /**
     * Default horizontal layout direction.
     */
    static const Int32 LAYOUT_DIRECTION_DEFAULT;

    /**
     * Default text direction is inherited
     */
    static const Int32 TEXT_DIRECTION_DEFAULT;

    static const Int32 TEXT_DIRECTION_RESOLVED_DEFAULT;//= TEXT_DIRECTION_FIRST_STRONG;

    /**
     * Array of text direction flags for mapping attribute "textDirection" to correct
     * flag value.
     * @hide
     */
    static const Int32 PFLAG2_TEXT_DIRECTION_FLAGS[];

    /**
     * Default text alignment is inherited
     */
    static const Int32 TEXT_ALIGNMENT_DEFAULT;

    static const Int32 TEXT_ALIGNMENT_RESOLVED_DEFAULT;// = TEXT_ALIGNMENT_GRAVITY;

    /**
     * Array of text direction flags for mapping attribute "textAlignment" to correct
     * flag value.
     * @hide
     */
    static const Int32 PFLAG2_TEXT_ALIGNMENT_FLAGS[];

    /**
     * Indicates whether if the view text alignment has been resolved to gravity
     */
    static const Int32 PFLAG2_TEXT_ALIGNMENT_RESOLVED_DEFAULT;

    /**
     * Default undefined padding
     */
    static const Int32 UNDEFINED_PADDING;

protected:
    class TransformationInfo
        : public Object
    {
        friend class View;
        friend class ViewPropertyAnimator;
        friend class RenderNodeAnimator;
    public:
        TransformationInfo();

    protected:
        /**
         * The opacity of the View. This is a value from 0 to 1, where 0 means
         * completely transparent and 1 means completely opaque.
         */
        //@ViewDebug.ExportedProperty
        Float mAlpha;

        /**
         * The opacity of the view as manipulated by the Fade transition. This is a hidden
         * property only used by transitions, which is composited with the other alpha
         * values to calculate the final visual alpha value.
         */
        Float mTransitionAlpha;// = 1f;

    private:
        /**
         * The transform matrix for the View. This transform is calculated internally
         * based on the rotation, scaleX, and scaleY properties. The identity matrix
         * is used by default. Do *not* use this variable directly; instead call
         * getMatrix(), which will automatically recalculate the matrix if necessary
         * to get the correct matrix based on the latest rotation and scale properties.
         */
        AutoPtr<IMatrix> mMatrix;

        /**
         * The transform matrix for the View. This transform is calculated internally
         * based on the rotation, scaleX, and scaleY properties. The identity matrix
         * is used by default. Do *not* use this variable directly; instead call
         * getInverseMatrix(), which will automatically recalculate the matrix if necessary
         * to get the correct matrix based on the latest rotation and scale properties.
         */
        AutoPtr<IMatrix> mInverseMatrix;
    };

    class ListenerInfo
        : public Object
    {
        friend class View;
    public:
        /**
         * Listener used to dispatch click events.
         * This field should be made private, so it is hidden from the SDK.
         * {@hide}
         */
        AutoPtr<IViewOnClickListener> mOnClickListener;

    protected:
        /**
         * Listener used to dispatch focus change events.
         * This field should be made private, so it is hidden from the SDK.
         * {@hide}
         */
        AutoPtr<IViewOnFocusChangeListener> mOnFocusChangeListener;

        /**
         * Listener used to dispatch long click events.
         * This field should be made private, so it is hidden from the SDK.
         * {@hide}
         */
        AutoPtr<IViewOnLongClickListener> mOnLongClickListener;

        /**
         * Listener used to build the context menu.
         * This field should be made private, so it is hidden from the SDK.
         * {@hide}
         */
        AutoPtr<IViewOnCreateContextMenuListener> mOnCreateContextMenuListener;

    private:
        /**
         * Listeners for layout change events.
         */
        List<AutoPtr<IViewOnLayoutChangeListener> > mOnLayoutChangeListeners;

        /**
         * Listeners for attach events.
         */
        //private CopyOnWriteArrayList<OnAttachStateChangeListener> mOnAttachStateChangeListeners;
        List<AutoPtr<IViewOnAttachStateChangeListener> > mOnAttachStateChangeListeners;

        AutoPtr<IViewOnKeyListener> mOnKeyListener;

        AutoPtr<IViewOnTouchListener> mOnTouchListener;

        AutoPtr<IViewOnHoverListener> mOnHoverListener;

        AutoPtr<IViewOnGenericMotionListener> mOnGenericMotionListener;

        AutoPtr<IViewOnDragListener> mOnDragListener;

        AutoPtr<IViewOnSystemUiVisibilityChangeListener> mOnSystemUiVisibilityChangeListener;

        AutoPtr<IViewOnApplyWindowInsetsListener> mOnApplyWindowInsetsListener;
    };

public:
    class BaseSavedState
        : public AbsSavedState
        , public IViewBaseSavedState
    {
    public:
        CAR_INTERFACE_DECL()
    };

    class AlpahFloatProperty
        : public FloatProperty
    {
    public:
        AlpahFloatProperty();

        CARAPI SetValue(
            /* [in] */ IInterface* obj,
            /* [in] */ Float value);

        CARAPI Get(
            /* [in] */ IInterface* obj,
            /* [out] */ IInterface** rst);
    };

    class TranslationXFloatProperty
        : public FloatProperty
    {
    public:
        TranslationXFloatProperty();

        CARAPI SetValue(
            /* [in] */ IInterface* obj,
            /* [in] */ Float value);

        CARAPI Get(
            /* [in] */ IInterface* obj,
            /* [out] */ IInterface** rst);
    };

    class TranslationYFloatProperty
        : public FloatProperty
    {
    public:
        TranslationYFloatProperty();

        CARAPI SetValue(
            /* [in] */ IInterface* obj,
            /* [in] */ Float value);

        CARAPI Get(
            /* [in] */ IInterface* obj,
            /* [out] */ IInterface** rst);
    };

    class TranslationZFloatProperty
        : public FloatProperty
    {
    public:
        TranslationZFloatProperty();

        CARAPI SetValue(
            /* [in] */ IInterface* obj,
            /* [in] */ Float value);

        CARAPI Get(
            /* [in] */ IInterface* obj,
            /* [out] */ IInterface** rst);
    };

    class XFloatProperty
        : public FloatProperty
    {
    public:
        XFloatProperty();

        CARAPI SetValue(
            /* [in] */ IInterface* obj,
            /* [in] */ Float value);

        CARAPI Get(
            /* [in] */ IInterface* obj,
            /* [out] */ IInterface** rst);
    };

    class YFloatProperty
        : public FloatProperty
    {
    public:
        YFloatProperty();

        CARAPI SetValue(
            /* [in] */ IInterface* obj,
            /* [in] */ Float value);

        CARAPI Get(
            /* [in] */ IInterface* obj,
            /* [out] */ IInterface** rst);
    };

    class ZFloatProperty
        : public FloatProperty
    {
    public:
        ZFloatProperty();

        CARAPI SetValue(
            /* [in] */ IInterface* obj,
            /* [in] */ Float value);

        CARAPI Get(
            /* [in] */ IInterface* obj,
            /* [out] */ IInterface** rst);
    };

    class RotationFloatProperty
        : public FloatProperty
    {
    public:
        RotationFloatProperty();

        CARAPI SetValue(
            /* [in] */ IInterface* obj,
            /* [in] */ Float value);

        CARAPI Get(
            /* [in] */ IInterface* obj,
            /* [out] */ IInterface** rst);
    };

    class RotationXFloatProperty
        : public FloatProperty
    {
    public:
        RotationXFloatProperty();

        CARAPI SetValue(
            /* [in] */ IInterface* obj,
            /* [in] */ Float value);

        CARAPI Get(
            /* [in] */ IInterface* obj,
            /* [out] */ IInterface** rst);
    };

    class RotationYFloatProperty
        : public FloatProperty
    {
    public:
        RotationYFloatProperty();

        CARAPI SetValue(
            /* [in] */ IInterface* obj,
            /* [in] */ Float value);

        CARAPI Get(
            /* [in] */ IInterface* obj,
            /* [out] */ IInterface** rst);
    };

    class ScaleXFloatProperty
        : public FloatProperty
    {
    public:
        ScaleXFloatProperty();

        CARAPI SetValue(
            /* [in] */ IInterface* obj,
            /* [in] */ Float value);

        CARAPI Get(
            /* [in] */ IInterface* obj,
            /* [out] */ IInterface** rst);
    };

    class ScaleYFloatProperty
        : public FloatProperty
    {
    public:
        ScaleYFloatProperty();

        CARAPI SetValue(
            /* [in] */ IInterface* obj,
            /* [in] */ Float value);

        CARAPI Get(
            /* [in] */ IInterface* obj,
            /* [out] */ IInterface** rst);
    };

    /**
     * A MeasureSpec encapsulates the layout requirements passed from parent to child.
     * Each MeasureSpec represents a requirement for either the width or the height.
     * A MeasureSpec is comprised of a size and a mode. There are three possible
     * modes:
     * <dl>
     * <dt>UNSPECIFIED</dt>
     * <dd>
     * The parent has not imposed any constraint on the child. It can be whatever size
     * it wants.
     * </dd>
     *
     * <dt>EXACTLY</dt>
     * <dd>
     * The parent has determined an exact size for the child. The child is going to be
     * given those bounds regardless of how big it wants to be.
     * </dd>
     *
     * <dt>AT_MOST</dt>
     * <dd>
     * The child can be as large as it wants up to the specified size.
     * </dd>
     * </dl>
     *
     * MeasureSpecs are implemented as ints to reduce object allocation. This class
     * is provided to pack and unpack the &lt;size, mode&gt; tuple into the int.
     */
    class MeasureSpec
    {
    private:
        static const Int32 MODE_SHIFT = 30;
        static const Int32 MODE_MASK  = 0x3 << MODE_SHIFT;

    public:
        /**
         * Measure specification mode: The parent has not imposed any constraint
         * on the child. It can be whatever size it wants.
         */
        static const Int32 UNSPECIFIED = 0 << MODE_SHIFT;

        /**
         * Measure specification mode: The parent has determined an exact size
         * for the child. The child is going to be given those bounds regardless
         * of how big it wants to be.
         */
        static const Int32 EXACTLY     = 1 << MODE_SHIFT;

        /**
         * Measure specification mode: The child can be as large as it wants up
         * to the specified size.
         */
        static const Int32 AT_MOST     = 2 << MODE_SHIFT;

    public:
        /**
         * Creates a measure specification based on the supplied size and mode.
         *
         * The mode must always be one of the following:
         * <ul>
         *  <li>{@link android.view.View.MeasureSpec#UNSPECIFIED}</li>
         *  <li>{@link android.view.View.MeasureSpec#EXACTLY}</li>
         *  <li>{@link android.view.View.MeasureSpec#AT_MOST}</li>
         * </ul>
         *
         * @param size the size of the measure specification
         * @param mode the mode of the measure specification
         * @return the measure specification based on size and mode
         */
        static CARAPI_(Int32) MakeMeasureSpec(
            /* [in] */ Int32 size,
            /* [in] */ Int32 mode);

        /**
         * Extracts the mode from the supplied measure specification.
         *
         * @param measureSpec the measure specification to extract the mode from
         * @return {@link android.view.View.MeasureSpec#UNSPECIFIED},
         *         {@link android.view.View.MeasureSpec#AT_MOST} or
         *         {@link android.view.View.MeasureSpec#EXACTLY}
         */
        static CARAPI_(Int32) GetMode(
            /* [in] */ Int32 measureSpec);

        /**
         * Extracts the size from the supplied measure specification.
         *
         * @param measureSpec the measure specification to extract the size from
         * @return the size in pixels defined in the supplied measure specification
         */
        static CARAPI_(Int32) GetSize(
            /* [in] */ Int32 measureSpec);

        static CARAPI_(Int32) Adjust(
            /* [in] */ Int32 measureSpec,
            /* [in] */ Int32 delta);

        /**
         * Returns a String representation of the specified measure
         * specification.
         *
         * @param measureSpec the measure specification to convert to a String
         * @return a String with the following format: "MeasureSpec: MODE SIZE"
         */
        static CARAPI GetDescription(
            /* [in] */ Int32 measureSpec,
            /* [out] */ String* description);
    };

    class CheckForLongPress
        : public Runnable
    {
    public:
        CheckForLongPress(
            /* [in] */ View* view)
            : mView(view)
        {}

        CARAPI Run();

        CARAPI_(void) RememberWindowAttachCount();

    private:
        View* mView;
        Int32 mOriginalWindowAttachCount;
    };

    class AttachInfo
        : public Object
    {
    public:
        class Callbacks
            : public Object
        {
        public:
            virtual ~Callbacks() {}

            virtual CARAPI PlaySoundEffect(
                /* [in] */ Int32 effectId) = 0;

            virtual CARAPI PerformHapticFeedback(
                /* [in] */ Int32 effectId,
                /* [in] */ Boolean always,
                /* [out] */ Boolean* result) = 0;
        };

        /**
         * InvalidateInfo is used to post invalidate(int, int, int, int) messages
         * to a Handler. This class contains the target (View) to invalidate and
         * the coordinates of the dirty rectangle.
         *
         * For performance purposes, this class also implements a pool of up to
         * POOL_LIMIT objects that get reused. This reduces memory allocations
         * whenever possible.
         */
        class InvalidateInfo
            : public Object
        {
        public:
            static CARAPI_(AutoPtr<InvalidateInfo>) Obtain();

            CARAPI Recycle();

        public:
            AutoPtr<IView> mTarget;

            Int32 mLeft;
            Int32 mTop;
            Int32 mRight;
            Int32 mBottom;

        private:
            static const Int32 POOL_LIMIT;
            static AutoPtr<Pools::SynchronizedPool<InvalidateInfo> > sPool;
        };

    public:
        AttachInfo(
            /* [in] */ IWindowSession* session,
            /* [in] */ IIWindow* window,
            /* [in] */ IDisplay* display,
            /* [in] */ IViewRootImpl* viewRootImpl,
            /* [in] */ IHandler* handler,
            /* [in] */ Callbacks* effectPlayer);

        ~AttachInfo();

    public:

        AutoPtr<IWindowSession> mSession;

        AutoPtr<IIWindow> mWindow;

        AutoPtr<IBinder> mWindowToken;

        AutoPtr<IDisplay> mDisplay;

        Callbacks* mRootCallbacks;

        AutoPtr<IIWindowId> mIWindowId;
        AutoPtr<IWindowId> mWindowId;
        /**
        * The top view of the hierarchy.
        */
        AutoPtr<IView> mRootView;

        AutoPtr<IBinder> mPanelParentWindowToken;

        Boolean mHardwareAccelerated;
        Boolean mHardwareAccelerationRequested;
        AutoPtr<IHardwareRenderer> mHardwareRenderer;

        AutoPtr<IArrayList> mPendingAnimatingRenderNodes;

        /**
         * The state of the display to which the window is attached, as reported
         * by {@link Display#getState()}.  Note that the display state constants
         * declared by {@link Display} do not exactly line up with the screen state
         * constants declared by {@link View} (there are more display states than
         * screen states).
         */
        Int32 mDisplayState;// = Display.STATE_UNKNOWN;

        /**
        * Scale factor used by the compatibility mode
        */
        Float mApplicationScale;

        /**
        * Indicates whether the application is in compatibility mode
        */
        Boolean mScalingRequired;

        /**
         * If set, ViewRootImpl doesn't use its lame animation for when the window resizes.
         */
        Boolean mTurnOffWindowResizeAnim;

        /**
        * Left position of this view's window
        */
        Int32 mWindowLeft;

        /**
        * Top position of this view's window
        */
        Int32 mWindowTop;

        /**
        * Indicates whether views need to use 32-bit drawing caches
        */
        Boolean mUse32BitDrawingCache;

        /**
         * For windows that are full-screen but using insets to layout inside
         * of the screen areas, these are the current insets to appear inside
         * the overscan area of the display.
         */
        AutoPtr<IRect> mOverscanInsets;// = new Rect();

        /**
        * For windows that are full-screen but using insets to layout inside
        * of the screen decorations, these are the current insets for the
        * content of the window.
        */
        AutoPtr<IRect> mContentInsets;

        /**
        * For windows that are full-screen but using insets to layout inside
        * of the screen decorations, these are the current insets for the
        * actual visible parts of the window.
        */
        AutoPtr<IRect> mVisibleInsets;

        /**
         * For windows that are full-screen but using insets to layout inside
         * of the screen decorations, these are the current insets for the
         * stable system windows.
         */
        AutoPtr<IRect> mStableInsets;// = new Rect();

        /**
         * The internal insets given by this window.  This value is
         * supplied by the client (through
         * {@link ViewTreeObserver.OnComputeInternalInsetsListener}) and will
         * be given to the window manager when changed to be used in laying
         * out windows behind it.
         */
        AutoPtr<IInternalInsetsInfo> mGivenInternalInsets;

        /**
         * Set to true when mGivenInternalInsets is non-empty.
         */
        Boolean mHasNonEmptyGivenInternalInsets;

        /**
         * All views in the window's hierarchy that serve as scroll containers,
         * used to determine if the window can be resized or must be panned
         * to adjust for a soft input area.
         */
        List<View*> mScrollContainers;

        AutoPtr<IDispatcherState> mKeyDispatchState;
        /**
        * Indicates whether the view's window currently has the focus.
        */
        Boolean mHasWindowFocus;

        /**
        * The current visibility of the window.
        */
        Int32 mWindowVisibility;

        /**
        * Indicates the time at which drawing started to occur.
        */
        Int64 mDrawingTime;

        /**
         * Indicates whether or not ignoring the DIRTY_MASK flags.
         */
        Boolean mIgnoreDirtyState;

        /**
         * This flag tracks when the mIgnoreDirtyState flag is set during draw(),
         * to avoid clearing that flag prematurely.
         */
        Boolean mSetIgnoreDirtyState;

        /**
        * Indicates whether the view's window is currently in touch mode.
        */
        Boolean mInTouchMode;

        /**
         * Indicates whether the view has requested unbuffered input dispatching for the current
         * event stream.
         */
        Boolean mUnbufferedDispatchRequested;

        /**
        * Indicates that ViewAncestor should trigger a global layout change
        * the next time it performs a traversal
        */
        Boolean mRecomputeGlobalAttributes;

        /**
         * Always report new attributes at next traversal.
         */
        Boolean mForceReportNewAttributes;

        /**
        * Set during a traveral if any views want to keep the screen on.
        */
        Boolean mKeepScreenOn;

        /**
         * Bitwise-or of all of the values that views have passed to setSystemUiVisibility().
         */
        Int32 mSystemUiVisibility;

        /**
         * Hack to force certain system UI visibility flags to be cleared.
         */
        Int32 mDisabledSystemUiVisibility;

        /**
         * Last global system UI visibility reported by the window manager.
         */
        Int32 mGlobalSystemUiVisibility;

        /**
         * True if a view in this hierarchy has an OnSystemUiVisibilityChangeListener
         * attached.
         */
        Boolean mHasSystemUiListeners;

        /**
         * Set if the window has requested to extend into the overscan region
         * via WindowManager.LayoutParams.FLAG_LAYOUT_IN_OVERSCAN.
         */
        Boolean mOverscanRequested;

        /**
        * Set if the visibility of any views has changed.
        */
        Boolean mViewVisibilityChanged;

        /**
        * Set to true if a view has been scrolled.
        */
        Boolean mViewScrollChanged;

        /**
         * Set to true if high contrast mode enabled
         */
        Boolean mHighContrastText;

        AutoPtr<IViewTreeObserver> mTreeObserver;

        /**
         * Global to the view hierarchy used as a temporary for dealing with
         * x/y points in the transparent region computations.
         */
        Int32 mTransparentLocation[2];

        /**
         * Global to the view hierarchy used as a temporary for dealing with
         * x/y points in the ViewGroup.invalidateChild implementation.
         */
        Int32 mInvalidateChildLocation[2];

        /**
         * Global to the view hierarchy used as a temporary for dealng with
         * computing absolute on-screen location.
         */
        AutoPtr< ArrayOf<Int32> > mTmpLocation;

        /**
         * Global to the view hierarchy used as a temporary for dealing with
         * x/y location when view is transformed.
         */
        AutoPtr< ArrayOf<Float> > mTmpTransformLocation;// = new float[2];

        /**
         * A Canvas used by the view hierarchy to perform bitmap caching.
         */
        AutoPtr<ICanvas> mCanvas;

        /**
         * The view root impl.
         */
        IViewRootImpl* mViewRootImpl;

        /**
         * A Handler supplied by a view's {@link android.view.ViewRootImpl}. This
         * handler can be used to pump events in the UI events queue.
         */
        AutoPtr<IHandler> mHandler;

        /**
         * Temporary for use in computing invalidate rectangles while
         * calling up the hierarchy.
         */
        AutoPtr<IRect> mTmpInvalRect;

        /**
         * Temporary for use in computing hit areas with transformed views
         */
        AutoPtr<IRectF> mTmpTransformRect;

        /**
         * Temporary for use in computing hit areas with transformed views
         */
        AutoPtr<IRectF> mTmpTransformRect1;

        /**
         * Temporary list of rectanges.
         */
        AutoPtr<IArrayList> mTmpRectList;

        /**
         * Temporary for use in transforming invalidation rect
         */
        AutoPtr<IMatrix> mTmpMatrix;

        /**
         * Temporary for use in transforming invalidation rect
         */
        AutoPtr<ITransformation> mTmpTransformation;

        /**
         * Temporary for use in querying outlines from OutlineProviders
         */
        AutoPtr<IOutline> mTmpOutline;// = new Outline();

        /**
         * Temporary list for use in collecting focusable descendents of a view.
         */
        AutoPtr<IArrayList> mTempArrayList;// = new ArrayList<View>(24);

        /**
         * The id of the window for accessibility purposes.
         */
        Int32 mAccessibilityWindowId;

        /**
         * Flags related to accessibility processing.
         *
         * @see AccessibilityNodeInfo#FLAG_INCLUDE_NOT_IMPORTANT_VIEWS
         * @see AccessibilityNodeInfo#FLAG_REPORT_VIEW_IDS
         */
        Int32 mAccessibilityFetchFlags;

        /**
         * The drawable for highlighting accessibility focus.
         */
        AutoPtr<IDrawable> mAccessibilityFocusDrawable;

        /**
         * Show where the margins, bounds and layout bounds are for each view.
         */
        Boolean mDebugLayout;

        /**
         * Used to track which View originated a requestLayout() call, used when
         * requestLayout() is called during layout.
         */
        AutoPtr<IView> mViewRequestingLayout;

        /**
         * Point used to compute visible regions.
         */
        AutoPtr<IPoint> mPoint;
    };

public:
    /**
     * <p>
     * This class represents a delegate that can be registered in a {@link View}
     * to enhance accessibility support via composition rather via inheritance.
     * It is specifically targeted to widget developers that extend basic View
     * classes i.e. classes in package android.view, that would like their
     * applications to be backwards compatible.
     * </p>
     * <div class="special reference">
     * <h3>Developer Guides</h3>
     * <p>For more information about making applications accessible, read the
     * <a href="{@docRoot}guide/topics/ui/accessibility/index.html">Accessibility</a>
     * developer guide.</p>
     * </div>
     * <p>
     * A scenario in which a developer would like to use an accessibility delegate
     * is overriding a method introduced in a later API version then the minimal API
     * version supported by the application. For example, the method
     * {@link View#onInitializeAccessibilityNodeInfo(AccessibilityNodeInfo)} is not available
     * in API version 4 when the accessibility APIs were first introduced. If a
     * developer would like his application to run on API version 4 devices (assuming
     * all other APIs used by the application are version 4 or lower) and take advantage
     * of this method, instead of overriding the method which would break the application's
     * backwards compatibility, he can override the corresponding method in this
     * delegate and register the delegate in the target View if the API version of
     * the system is high enough i.e. the API version is same or higher to the API
     * version that introduced
     * {@link View#onInitializeAccessibilityNodeInfo(AccessibilityNodeInfo)}.
     * </p>
     * <p>
     * Here is an example implementation:
     * </p>
     * <code><pre><p>
     * if (Build.VERSION.SDK_INT >= 14) {
     *     // If the API version is equal of higher than the version in
     *     // which onInitializeAccessibilityNodeInfo was introduced we
     *     // register a delegate with a customized implementation.
     *     View view = findViewById(R.id.view_id);
     *     view.setAccessibilityDelegate(new AccessibilityDelegate() {
     *         public void onInitializeAccessibilityNodeInfo(View host,
     *                 AccessibilityNodeInfo info) {
     *             // Let the default implementation populate the info.
     *             super.onInitializeAccessibilityNodeInfo(host, info);
     *             // Set some other information.
     *             info.setEnabled(host.isEnabled());
     *         }
     *     });
     * }
     * </code></pre></p>
     * <p>
     * This delegate contains methods that correspond to the accessibility methods
     * in View. If a delegate has been specified the implementation in View hands
     * off handling to the corresponding method in this delegate. The default
     * implementation the delegate methods behaves exactly as the corresponding
     * method in View for the case of no accessibility delegate been set. Hence,
     * to customize the behavior of a View method, clients can override only the
     * corresponding delegate method without altering the behavior of the rest
     * accessibility related methods of the host view.
     * </p>
     */
    class AccessibilityDelegate
        : public Object
        , public IAccessibilityDelegate
    {
    public:
        CAR_INTERFACE_DECL()

        /**
         * Sends an accessibility event of the given type. If accessibility is not
         * enabled this method has no effect.
         * <p>
         * The default implementation behaves as {@link View#sendAccessibilityEvent(int)
         *  View#sendAccessibilityEvent(int)} for the case of no accessibility delegate
         * been set.
         * </p>
         *
         * @param host The View hosting the delegate.
         * @param eventType The type of the event to send.
         *
         * @see View#sendAccessibilityEvent(int) View#sendAccessibilityEvent(int)
         */
        CARAPI SendAccessibilityEvent(
            /* [in] */ IView* host,
            /* [in] */ Int32 eventType);

        /**
         * Performs the specified accessibility action on the view. For
         * possible accessibility actions look at {@link AccessibilityNodeInfo}.
         * <p>
         * The default implementation behaves as
         * {@link View#performAccessibilityAction(int, Bundle)
         *  View#performAccessibilityAction(int, Bundle)} for the case of
         *  no accessibility delegate been set.
         * </p>
         *
         * @param action The action to perform.
         * @return Whether the action was performed.
         *
         * @see View#performAccessibilityAction(int, Bundle)
         *      View#performAccessibilityAction(int, Bundle)
         */
        CARAPI PerformAccessibilityAction(
            /* [in] */ IView* host,
            /* [in] */ Int32 action,
            /* [in] */ IBundle* args,
            /* [out] */ Boolean* res);

        /**
         * Sends an accessibility event. This method behaves exactly as
         * {@link #sendAccessibilityEvent(View, int)} but takes as an argument an
         * empty {@link AccessibilityEvent} and does not perform a check whether
         * accessibility is enabled.
         * <p>
         * The default implementation behaves as
         * {@link View#sendAccessibilityEventUnchecked(AccessibilityEvent)
         *  View#sendAccessibilityEventUnchecked(AccessibilityEvent)} for
         * the case of no accessibility delegate been set.
         * </p>
         *
         * @param host The View hosting the delegate.
         * @param event The event to send.
         *
         * @see View#sendAccessibilityEventUnchecked(AccessibilityEvent)
         *      View#sendAccessibilityEventUnchecked(AccessibilityEvent)
         */
        CARAPI SendAccessibilityEventUnchecked(
            /* [in] */ IView* host,
            /* [in] */ IAccessibilityEvent* event);

        /**
         * Dispatches an {@link AccessibilityEvent} to the host {@link View} first and then
         * to its children for adding their text content to the event.
         * <p>
         * The default implementation behaves as
         * {@link View#dispatchPopulateAccessibilityEvent(AccessibilityEvent)
         *  View#dispatchPopulateAccessibilityEvent(AccessibilityEvent)} for
         * the case of no accessibility delegate been set.
         * </p>
         *
         * @param host The View hosting the delegate.
         * @param event The event.
         * @return True if the event population was completed.
         *
         * @see View#dispatchPopulateAccessibilityEvent(AccessibilityEvent)
         *      View#dispatchPopulateAccessibilityEvent(AccessibilityEvent)
         */
        CARAPI DispatchPopulateAccessibilityEvent(
            /* [in] */ IView* host,
            /* [in] */ IAccessibilityEvent* event,
            /* [out] */ Boolean* res);

        /**
         * Gives a chance to the host View to populate the accessibility event with its
         * text content.
         * <p>
         * The default implementation behaves as
         * {@link View#onPopulateAccessibilityEvent(AccessibilityEvent)
         *  View#onPopulateAccessibilityEvent(AccessibilityEvent)} for
         * the case of no accessibility delegate been set.
         * </p>
         *
         * @param host The View hosting the delegate.
         * @param event The accessibility event which to populate.
         *
         * @see View#onPopulateAccessibilityEvent(AccessibilityEvent)
         *      View#onPopulateAccessibilityEvent(AccessibilityEvent)
         */
        CARAPI OnPopulateAccessibilityEvent(
            /* [in] */ IView* host,
            /* [in] */ IAccessibilityEvent* event);

        /**
         * Initializes an {@link AccessibilityEvent} with information about the
         * the host View which is the event source.
         * <p>
         * The default implementation behaves as
         * {@link View#onInitializeAccessibilityEvent(AccessibilityEvent)
         *  View#onInitializeAccessibilityEvent(AccessibilityEvent)} for
         * the case of no accessibility delegate been set.
         * </p>
         *
         * @param host The View hosting the delegate.
         * @param event The event to initialize.
         *
         * @see View#onInitializeAccessibilityEvent(AccessibilityEvent)
         *      View#onInitializeAccessibilityEvent(AccessibilityEvent)
         */
        CARAPI OnInitializeAccessibilityEvent(
            /* [in] */ IView* host,
            /* [in] */ IAccessibilityEvent* event);

        /**
         * Initializes an {@link AccessibilityNodeInfo} with information about the host view.
         * <p>
         * The default implementation behaves as
         * {@link View#onInitializeAccessibilityNodeInfo(AccessibilityNodeInfo)
         *  View#onInitializeAccessibilityNodeInfo(AccessibilityNodeInfo)} for
         * the case of no accessibility delegate been set.
         * </p>
         *
         * @param host The View hosting the delegate.
         * @param info The instance to initialize.
         *
         * @see View#onInitializeAccessibilityNodeInfo(AccessibilityNodeInfo)
         *      View#onInitializeAccessibilityNodeInfo(AccessibilityNodeInfo)
         */
        CARAPI OnInitializeAccessibilityNodeInfo(
            /* [in] */ IView* host,
            /* [in] */ IAccessibilityNodeInfo* info);

        /**
         * Called when a child of the host View has requested sending an
         * {@link AccessibilityEvent} and gives an opportunity to the parent (the host)
         * to augment the event.
         * <p>
         * The default implementation behaves as
         * {@link ViewGroup#onRequestSendAccessibilityEvent(View, AccessibilityEvent)
         *  ViewGroup#onRequestSendAccessibilityEvent(View, AccessibilityEvent)} for
         * the case of no accessibility delegate been set.
         * </p>
         *
         * @param host The View hosting the delegate.
         * @param child The child which requests sending the event.
         * @param event The event to be sent.
         * @return True if the event should be sent
         *
         * @see ViewGroup#onRequestSendAccessibilityEvent(View, AccessibilityEvent)
         *      ViewGroup#onRequestSendAccessibilityEvent(View, AccessibilityEvent)
         */
        CARAPI OnRequestSendAccessibilityEvent(
            /* [in] */ IViewGroup* host,
            /* [in] */ IView* child,
            /* [in] */ IAccessibilityEvent* event,
            /* [out] */ Boolean* res);

        /**
         * Gets the provider for managing a virtual view hierarchy rooted at this View
         * and reported to {@link android.accessibilityservice.AccessibilityService}s
         * that explore the window content.
         * <p>
         * The default implementation behaves as
         * {@link View#getAccessibilityNodeProvider() View#getAccessibilityNodeProvider()} for
         * the case of no accessibility delegate been set.
         * </p>
         *
         * @return The provider.
         *
         * @see AccessibilityNodeProvider
         */
        CARAPI GetAccessibilityNodeProvider(
            /* [in] */ IView* host,
            /* [out] */ IAccessibilityNodeProvider** anp);

        /**
         * Returns an {@link AccessibilityNodeInfo} representing the host view from the
         * point of view of an {@link android.accessibilityservice.AccessibilityService}.
         * This method is responsible for obtaining an accessibility node info from a
         * pool of reusable instances and calling
         * {@link #onInitializeAccessibilityNodeInfo(AccessibilityNodeInfo)} on the host
         * view to initialize the former.
         * <p>
         * <strong>Note:</strong> The client is responsible for recycling the obtained
         * instance by calling {@link AccessibilityNodeInfo#recycle()} to minimize object
         * creation.
         * </p>
         * <p>
         * The default implementation behaves as
         * {@link View#createAccessibilityNodeInfo() View#createAccessibilityNodeInfo()} for
         * the case of no accessibility delegate been set.
         * </p>
         * @return A populated {@link AccessibilityNodeInfo}.
         *
         * @see AccessibilityNodeInfo
         *
         * @hide
         */
        CARAPI CreateAccessibilityNodeInfo(
            /* [in] */ IView* host,
            /* [out] */ IAccessibilityNodeInfo** res);
    };

private:
    class TintInfo
        : public Object
    {
    public:
        AutoPtr<IColorStateList> mTintList;
        PorterDuffMode mTintMode;
        Boolean mHasTintMode;
        Boolean mHasTintList;
    };

    class CheckForTap
        : public Runnable
    {
    public:
        TO_STRING_IMPL("View::CheckForTap")

        CheckForTap(
            /* [in] */ IWeakReference* view)
            : mWeakHost(view)
        {}

        CARAPI Run();
    public:
        Float mX;
        Float mY;
    private:
        AutoPtr<IWeakReference> mWeakHost;
    };

    class ViewPerformClick
        : public Runnable
    {
    public:
        TO_STRING_IMPL("View::ViewPerformClick")

        ViewPerformClick(
            /* [in] */ IWeakReference* view)
            : mWeakHost(view)
        {}

        CARAPI Run();

    private:
        AutoPtr<IWeakReference> mWeakHost;
    };

    class UnsetPressedState
        : public Runnable
    {
    public:
        TO_STRING_IMPL("View::UnsetPressedState")

        UnsetPressedState(
            /* [in] */ IWeakReference* view)
            : mWeakHost(view)
        {}

        CARAPI Run();

    private:
        AutoPtr<IWeakReference> mWeakHost;
    };

    /**
     * <p>ScrollabilityCache holds various fields used by a View when scrolling
     * is supported. This avoids keeping too many unused fields in most
     * instances of View.</p>
     */
    class ScrollabilityCache
        : public Runnable
    {
    public:
        /**
         * Scrollbars are not visible
         */
        static const Int32 OFF = 0;

        /**
         * Scrollbars are visible
         */
        static const Int32 ON = 1;

        /**
         * Scrollbars are fading away
         */
        static const Int32 FADING = 2;

    public:
        ScrollabilityCache(
            /* [in] */ IViewConfiguration* configuration,
            /* [in] */ IWeakReference* host);

        CARAPI Run();

    public:
        CARAPI_(void) SetFadeColor(
            /* [in] */ Int32 color);

    public:
        AutoPtr<IWeakReference> mWeakHost;

        Boolean mFadeScrollBars;
        Int32 mFadingEdgeLength;
        Int32 mScrollBarDefaultDelayBeforeFade;
        Int32 mScrollBarFadeDuration;
        Int32 mScrollBarSize;
        AutoPtr<IScrollBarDrawable> mScrollBar;
        AutoPtr<ArrayOf<Float> > mInterpolatorValues;

        AutoPtr<IPaint> mPaint;
        AutoPtr<IMatrix> mMatrix;
        AutoPtr<IShader> mShader;

        AutoPtr<Elastos::Droid::Graphics::IInterpolator> mScrollBarInterpolator;

        static AutoPtr<ArrayOf<Float> > OPAQUE;
        static AutoPtr<ArrayOf<Float> > TRANSPARENT;

        /**
         * When fading should start. This time moves into the future every time
         * a new scroll happens. Measured based on SystemClock.uptimeMillis()
         */
        Int64 mFadeStartTime;

        /**
         * The current state of the scrollbars: ON, OFF, or FADING
         */
        Int32 mState;

        Int32 mLastColor;
    };

    class _Predicate
        : public Object
        , public IPredicate
    {
    public:
        CAR_INTERFACE_DECL()

        _Predicate(
            /* [in] */ Int32 id);

        CARAPI Apply(
            /* [in] */ IInterface* t,
            /* [out] */ Boolean* result);

    private:
        Int32 mId;
    };

    /**
     * Resuable callback for sending
     * {@link AccessibilityEvent#TYPE_VIEW_SCROLLED} accessibility event.
     */
    class SendViewScrolledAccessibilityEvent
        : public Runnable
    {
    public:
        SendViewScrolledAccessibilityEvent(
            /* [in] */ View* host);

        CARAPI Run();

    public:
        volatile Boolean mIsPending;

    private:
        View* mHost;
    };

    class MatchIdPredicate
        : public Object
        , public IPredicate
     {
    public:
        MatchIdPredicate();

        CAR_INTERFACE_DECL()

        //@Override
        CARAPI Apply(
            /* [in] */ IInterface* view,
            /* [out] */ Boolean* apply);

    public:
        Int32 mId;
    };

    class MatchLabelForPredicate
        : public Object
        , public IPredicate
    {
    public:
        MatchLabelForPredicate();

        CAR_INTERFACE_DECL()

        //@Override
        CARAPI Apply(
            /* [in] */ IInterface* view,
            /* [out] */ Boolean* apply);

    public:
        Int32 mLabeledId;
    };

    class SendViewStateChangedAccessibilityEvent
        : public Runnable
    {
    public:
        SendViewStateChangedAccessibilityEvent(
            /* [in] */ View* host);

        CARAPI Run();

        CARAPI RunOrPost(
            /* [in] */ Int32 changeType);

    private:
        View* mHost;
        Int32 mChangeTypes;
        Boolean mPosted;
        Boolean mPostedWithDelay;
        Int64 mLastEventTimeMillis;
    };

    class InnerViewOnClickListener
        : public Object
        , public IViewOnClickListener
    {
    public:
        CAR_INTERFACE_DECL()

        InnerViewOnClickListener(
            /* [in] */ IWeakReference* weakHost,
            /* [in] */ const String& handlerName);

        CARAPI OnClick(
            /* [in] */ IView* v);
    private:
        AutoPtr<IWeakReference> mWeakHost;
        String mHandlerName;
        AutoPtr<IMethodInfo> mHandler;
    };

public:
    CAR_INTERFACE_DECL()

    View();

    virtual ~View();

protected:
    // subclasses must call this method.
    CARAPI constructor();

public:
    CARAPI constructor(
        /* [in] */ IContext* context);

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs);

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs,
        /* [in] */ Int32 defStyleAttr);

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs,
        /* [in] */ Int32 defStyleAttr,
        /* [in] */ Int32 defStyleRes);

    virtual CARAPI GetVerticalFadingEdgeLength(
         /* [out] */ Int32* length);

    virtual CARAPI SetFadingEdgeLength(
        /* [in] */ Int32 length);

    virtual CARAPI GetHorizontalFadingEdgeLength(
         /* [out] */ Int32* length);

    virtual CARAPI GetVerticalScrollbarWidth(
         /* [out] */ Int32* width);

    virtual CARAPI SetVerticalScrollbarPosition(
        /* [in] */ Int32 position);

    virtual CARAPI GetVerticalScrollbarPosition(
        /* [out] */ Int32* pisition);

    virtual CARAPI SetOnFocusChangeListener(
        /* [in] */ IViewOnFocusChangeListener* l);

    virtual CARAPI AddOnLayoutChangeListener(
        /* [in] */ IViewOnLayoutChangeListener* listener);

    virtual CARAPI RemoveOnLayoutChangeListener(
        /* [in] */ IViewOnLayoutChangeListener* listener);

    virtual CARAPI AddOnAttachStateChangeListener(
        /* [in] */ IViewOnAttachStateChangeListener* listener);

    virtual CARAPI RemoveOnAttachStateChangeListener(
        /* [in] */ IViewOnAttachStateChangeListener* listener);

    virtual CARAPI GetOnFocusChangeListener(
        /* [out] */ IViewOnFocusChangeListener** l);

    virtual CARAPI SetOnClickListener(
        /* [in] */ IViewOnClickListener* l);

    virtual CARAPI HasOnClickListeners(
        /* [out] */ Boolean* res);

    virtual CARAPI SetOnLongClickListener(
        /* [in] */ IViewOnLongClickListener* l);

    virtual CARAPI SetOnCreateContextMenuListener(
        /* [in] */ IViewOnCreateContextMenuListener* l);

    virtual CARAPI PerformClick(
        /* [out] */ Boolean* res);

    /**
     * Directly call any attached OnClickListener.  Unlike {@link #performClick()},
     * this only calls the listener, and does not do any associated clicking
     * actions like reporting an accessibility event.
     *
     * @return True there was an assigned OnClickListener that was called, false
     *         otherwise is returned.
     */
    virtual CARAPI CallOnClick(
        /* [out] */ Boolean* res);

    virtual CARAPI PerformLongClick(
        /* [out] */ Boolean* res);

    virtual CARAPI ShowContextMenu(
        /* [out] */ Boolean* res);

    virtual CARAPI ShowContextMenu(
        /* [in] */ Float x,
        /* [in] */ Float y,
        /* [in] */ Int32 metaState,
        /* [out] */ Boolean* res);

    virtual CARAPI StartActionMode(
        /* [in] */ IActionModeCallback* callback,
        /* [out] */ IActionMode** mode);

    virtual CARAPI SetOnKeyListener(
        /* [in] */ IViewOnKeyListener* l);

    virtual CARAPI SetOnTouchListener(
        /* [in] */ IViewOnTouchListener* l);

    virtual CARAPI SetOnGenericMotionListener(
        /* [in] */ IViewOnGenericMotionListener* l);

    virtual CARAPI SetOnHoverListener(
        /* [in] */ IViewOnHoverListener* l);

    virtual CARAPI SetOnDragListener(
        /* [in] */ IViewOnDragListener* l);

    /**
     * Give this view focus. This will cause
     * {@link #onFocusChanged(boolean, int, android.graphics.Rect)} to be called.
     *
     * Note: this does not check whether this {@link View} should get focus, it just
     * gives it focus no matter what.  It should only be called internally by framework
     * code that knows what it is doing, namely {@link #requestFocus(int, Rect)}.
     *
     * @param direction values are {@link View#FOCUS_UP}, {@link View#FOCUS_DOWN},
     *        {@link View#FOCUS_LEFT} or {@link View#FOCUS_RIGHT}. This is the direction which
     *        focus moved when requestFocus() is called. It may not always
     *        apply, in which case use the default View.FOCUS_DOWN.
     * @param previouslyFocusedRect The rectangle of the view that had focus
     *        prior in this View's coordinate system.
     */
    virtual CARAPI HandleFocusGainInternal(
        /* [in] */ Int32 direction,
        /* [in] */ IRect* previouslyFocusedRect);

    virtual CARAPI RequestRectangleOnScreen(
        /* [in] */ IRect* rectangle,
        /* [out] */ Boolean* res);

    virtual CARAPI RequestRectangleOnScreen(
        /* [in] */ IRect* rectangle,
        /* [in] */ Boolean immediate,
        /* [out] */ Boolean* res);

    virtual CARAPI ClearFocus();

    virtual CARAPI ClearFocusInternal(
    /* [in] */ IView* focused,
    /* [in] */ Boolean propagate,
    /* [in] */ Boolean refocus);

    /**
     * Called to clear the focus of a view that is about to be removed.
     * Doesn't call clearChildFocus, which prevents this view from taking
     * focus again before it has been removed from the parent
     */
    virtual CARAPI NotifyGlobalFocusCleared(
        /* [in] */ IView* oldFocus);

    virtual CARAPI RootViewRequestFocus(
        /* [out] */ Boolean* res);

    /**
     * Called internally by the view system when a new view is getting focus.
     * This is what clears the old focus.
     */
    virtual CARAPI UnFocus(
        /* [in] */ IView* focused);

    virtual CARAPI HasFocus(
        /* [out] */ Boolean* res);

    virtual CARAPI HasFocusable(
        /* [out] */ Boolean* res);

    /**
     * Convenience method for sending a {@link AccessibilityEvent#TYPE_ANNOUNCEMENT}
     * {@link AccessibilityEvent} to make an announcement which is related to some
     * sort of a context change for which none of the events representing UI transitions
     * is a good fit. For example, announcing a new page in a book. If accessibility
     * is not enabled this method does nothing.
     *
     * @param text The announcement text.
     */
    virtual CARAPI AnnounceForAccessibility(
        /* [in] */ ICharSequence* text);

    /**
     * Sends an accessibility event of the given type. If accessibility is
     * not enabled this method has no effect. The default implementation calls
     * {@link #onInitializeAccessibilityEvent(AccessibilityEvent)} first
     * to populate information about the event source (this View), then calls
     * {@link #dispatchPopulateAccessibilityEvent(AccessibilityEvent)} to
     * populate the text content of the event source including its descendants,
     * and last calls
     * {@link ViewParent#requestSendAccessibilityEvent(View, AccessibilityEvent)}
     * on its parent to resuest sending of the event to interested parties.
     * <p>
     * If an {@link AccessibilityDelegate} has been specified via calling
     * {@link #setAccessibilityDelegate(AccessibilityDelegate)} its
     * {@link AccessibilityDelegate#sendAccessibilityEvent(View, int)} is
     * responsible for handling this call.
     * </p>
     *
     * @param eventType The type of the event to send, as defined by several types from
     * {@link android.view.accessibility.AccessibilityEvent}, such as
     * {@link android.view.accessibility.AccessibilityEvent#TYPE_VIEW_CLICKED} or
     * {@link android.view.accessibility.AccessibilityEvent#TYPE_VIEW_HOVER_ENTER}.
     *
     * @see #onInitializeAccessibilityEvent(AccessibilityEvent)
     * @see #dispatchPopulateAccessibilityEvent(AccessibilityEvent)
     * @see ViewParent#requestSendAccessibilityEvent(View, AccessibilityEvent)
     * @see AccessibilityDelegate
     */
    virtual CARAPI SendAccessibilityEvent(
        /* [in] */ Int32 eventType);

    /**
     * This method behaves exactly as {@link #sendAccessibilityEvent(int)} but
     * takes as an argument an empty {@link AccessibilityEvent} and does not
     * perform a check whether accessibility is enabled.
     * <p>
     * If an {@link AccessibilityDelegate} has been specified via calling
     * {@link #setAccessibilityDelegate(AccessibilityDelegate)} its
     * {@link AccessibilityDelegate#sendAccessibilityEventUnchecked(View, AccessibilityEvent)}
     * is responsible for handling this call.
     * </p>
     *
     * @param event The event to send.
     *
     * @see #sendAccessibilityEvent(int)
     */
    virtual CARAPI SendAccessibilityEventUnchecked(
        /* [in] */ IAccessibilityEvent* event);

    /**
     * Dispatches an {@link AccessibilityEvent} to the {@link View} first and then
     * to its children for adding their text content to the event. Note that the
     * event text is populated in a separate dispatch path since we add to the
     * event not only the text of the source but also the text of all its descendants.
     * A typical implementation will call
     * {@link #onPopulateAccessibilityEvent(AccessibilityEvent)} on the this view
     * and then call the {@link #dispatchPopulateAccessibilityEvent(AccessibilityEvent)}
     * on each child. Override this method if custom population of the event text
     * content is required.
     * <p>
     * If an {@link AccessibilityDelegate} has been specified via calling
     * {@link #setAccessibilityDelegate(AccessibilityDelegate)} its
     * {@link AccessibilityDelegate#dispatchPopulateAccessibilityEvent(View, AccessibilityEvent)}
     * is responsible for handling this call.
     * </p>
     * <p>
     * <em>Note:</em> Accessibility events of certain types are not dispatched for
     * populating the event text via this method. For details refer to {@link AccessibilityEvent}.
     * </p>
     *
     * @param event The event.
     *
     * @return True if the event population was completed.
     */
    virtual CARAPI DispatchPopulateAccessibilityEvent(
        /* [in] */ IAccessibilityEvent* event,
        /* [out] */ Boolean* res);

    /**
     * Called from {@link #dispatchPopulateAccessibilityEvent(AccessibilityEvent)}
     * giving a chance to this View to populate the accessibility event with its
     * text content. While this method is free to modify event
     * attributes other than text content, doing so should normally be performed in
     * {@link #onInitializeAccessibilityEvent(AccessibilityEvent)}.
     * <p>
     * Example: Adding formatted date string to an accessibility event in addition
     *          to the text added by the super implementation:
     * <pre> public void onPopulateAccessibilityEvent(AccessibilityEvent event) {
     *     super.onPopulateAccessibilityEvent(event);
     *     final int flags = DateUtils.FORMAT_SHOW_DATE | DateUtils.FORMAT_SHOW_WEEKDAY;
     *     String selectedDateUtterance = DateUtils.formatDateTime(mContext,
     *         mCurrentDate.getTimeInMillis(), flags);
     *     event.getText().add(selectedDateUtterance);
     * }</pre>
     * <p>
     * If an {@link AccessibilityDelegate} has been specified via calling
     * {@link #setAccessibilityDelegate(AccessibilityDelegate)} its
     * {@link AccessibilityDelegate#onPopulateAccessibilityEvent(View, AccessibilityEvent)}
     * is responsible for handling this call.
     * </p>
     * <p class="note"><strong>Note:</strong> Always call the super implementation before adding
     * information to the event, in case the default implementation has basic information to add.
     * </p>
     *
     * @param event The accessibility event which to populate.
     *
     * @see #sendAccessibilityEvent(int)
     * @see #dispatchPopulateAccessibilityEvent(AccessibilityEvent)
     */
    virtual CARAPI OnPopulateAccessibilityEvent(
        /* [in] */ IAccessibilityEvent* event);

    /**
     * Initializes an {@link AccessibilityEvent} with information about
     * this View which is the event source. In other words, the source of
     * an accessibility event is the view whose state change triggered firing
     * the event.
     * <p>
     * Example: Setting the password property of an event in addition
     *          to properties set by the super implementation:
     * <pre> public void onInitializeAccessibilityEvent(AccessibilityEvent event) {
     *     super.onInitializeAccessibilityEvent(event);
     *     event.setPassword(true);
     * }</pre>
     * <p>
     * If an {@link AccessibilityDelegate} has been specified via calling
     * {@link #setAccessibilityDelegate(AccessibilityDelegate)} its
     * {@link AccessibilityDelegate#onInitializeAccessibilityEvent(View, AccessibilityEvent)}
     * is responsible for handling this call.
     * </p>
     * <p class="note"><strong>Note:</strong> Always call the super implementation before adding
     * information to the event, in case the default implementation has basic information to add.
     * </p>
     * @param event The event to initialize.
     *
     * @see #sendAccessibilityEvent(int)
     * @see #dispatchPopulateAccessibilityEvent(AccessibilityEvent)
     */
    virtual CARAPI OnInitializeAccessibilityEvent(
        /* [in] */ IAccessibilityEvent* event);

    /**
     * Returns an {@link AccessibilityNodeInfo} representing this view from the
     * point of view of an {@link android.accessibilityservice.AccessibilityService}.
     * This method is responsible for obtaining an accessibility node info from a
     * pool of reusable instances and calling
     * {@link #onInitializeAccessibilityNodeInfo(AccessibilityNodeInfo)} on this view to
     * initialize the former.
     * <p>
     * Note: The client is responsible for recycling the obtained instance by calling
     *       {@link AccessibilityNodeInfo#recycle()} to minimize object creation.
     * </p>
     *
     * @return A populated {@link AccessibilityNodeInfo}.
     *
     * @see AccessibilityNodeInfo
     */
    virtual CARAPI CreateAccessibilityNodeInfo(
        /* [out] */ IAccessibilityNodeInfo** res);

    /**
     * Returns an {@link AccessibilityNodeInfo} representing this view from the
     * point of view of an {@link android.accessibilityservice.AccessibilityService}.
     * This method is responsible for obtaining an accessibility node info from a
     * pool of reusable instances and calling
     * {@link #onInitializeAccessibilityNodeInfo(AccessibilityNodeInfo)} on this view to
     * initialize the former.
     * <p>
     * Note: The client is responsible for recycling the obtained instance by calling
     *       {@link AccessibilityNodeInfo#recycle()} to minimize object creation.
     * </p>
     *
     * @return A populated {@link AccessibilityNodeInfo}.
     *
     * @see AccessibilityNodeInfo
     */
    virtual CARAPI CreateAccessibilityNodeInfoInternal(
        /* [out] */ IAccessibilityNodeInfo** res);

    /**
     * Initializes an {@link AccessibilityNodeInfo} with information about this view.
     * The base implementation sets:
     * <ul>
     *   <li>{@link AccessibilityNodeInfo#setParent(View)},</li>
     *   <li>{@link AccessibilityNodeInfo#setBoundsInParent(Rect)},</li>
     *   <li>{@link AccessibilityNodeInfo#setBoundsInScreen(Rect)},</li>
     *   <li>{@link AccessibilityNodeInfo#setPackageName(CharSequence)},</li>
     *   <li>{@link AccessibilityNodeInfo#setClassName(CharSequence)},</li>
     *   <li>{@link AccessibilityNodeInfo#setContentDescription(CharSequence)},</li>
     *   <li>{@link AccessibilityNodeInfo#setEnabled(boolean)},</li>
     *   <li>{@link AccessibilityNodeInfo#setClickable(boolean)},</li>
     *   <li>{@link AccessibilityNodeInfo#setFocusable(boolean)},</li>
     *   <li>{@link AccessibilityNodeInfo#setFocused(boolean)},</li>
     *   <li>{@link AccessibilityNodeInfo#setLongClickable(boolean)},</li>
     *   <li>{@link AccessibilityNodeInfo#setSelected(boolean)},</li>
     * </ul>
     * <p>
     * Subclasses should override this method, call the super implementation,
     * and set additional attributes.
     * </p>
     * <p>
     * If an {@link AccessibilityDelegate} has been specified via calling
     * {@link #setAccessibilityDelegate(AccessibilityDelegate)} its
     * {@link AccessibilityDelegate#onInitializeAccessibilityNodeInfo(View, AccessibilityNodeInfo)}
     * is responsible for handling this call.
     * </p>
     *
     * @param info The instance to initialize.
     */
    virtual CARAPI OnInitializeAccessibilityNodeInfo(
        /* [in] */ IAccessibilityNodeInfo* info);

    CARAPI ComputeClickPointInScreenForAccessibility(
        /* [in] */ IRegion* interactiveRegion,
        /* [in] */ IPoint* outPoint,
        /* [out] */ Boolean* res);

    /**
     * Returns the delegate for implementing accessibility support via
     * composition. For more details see {@link AccessibilityDelegate}.
     *
     * @return The delegate, or null if none set.
     *
     * @hide
     */
    virtual CARAPI GetAccessibilityDelegate(
        /* [out] */ IAccessibilityDelegate** res);

    /**
     * Sets a delegate for implementing accessibility support via composition as
     * opposed to inheritance. The delegate's primary use is for implementing
     * backwards compatible widgets. For more details see {@link AccessibilityDelegate}.
     *
     * @param delegate The delegate instance.
     *
     * @see AccessibilityDelegate
     */
    virtual CARAPI SetAccessibilityDelegate(
        /* [in] */ IAccessibilityDelegate* delegate);

    /**
     * Gets the provider for managing a virtual view hierarchy rooted at this View
     * and reported to {@link android.accessibilityservice.AccessibilityService}s
     * that explore the window content.
     * <p>
     * If this method returns an instance, this instance is responsible for managing
     * {@link AccessibilityNodeInfo}s describing the virtual sub-tree rooted at this
     * View including the one representing the View itself. Similarly the returned
     * instance is responsible for performing accessibility actions on any virtual
     * view or the root view itself.
     * </p>
     * <p>
     * If an {@link AccessibilityDelegate} has been specified via calling
     * {@link #setAccessibilityDelegate(AccessibilityDelegate)} its
     * {@link AccessibilityDelegate#getAccessibilityNodeProvider(View)}
     * is responsible for handling this call.
     * </p>
     *
     * @return The provider.
     *
     * @see AccessibilityNodeProvider
     */
    virtual CARAPI GetAccessibilityNodeProvider(
        /* [out] */ IAccessibilityNodeProvider** res);

    /**
     * Gets the unique identifier of this view on the screen for accessibility purposes.
     * If this {@link View} is not attached to any window, {@value #NO_ID} is returned.
     *
     * @return The view accessibility id.
     *
     * @hide
     */
    virtual CARAPI GetAccessibilityViewId(
        /* [out] */ Int32* id);

    /**
     * Gets the unique identifier of the window in which this View reseides.
     *
     * @return The window accessibility id.
     *
     * @hide
     */
    virtual CARAPI GetAccessibilityWindowId(
        /* [out] */ Int32* id);

    /**
     * Gets the id of a view for which this view serves as a label for
     * accessibility purposes.
     *
     * @return The labeled view id.
     */
    //@ViewDebug.ExportedProperty(category = "accessibility")
    virtual CARAPI GetLabelFor(
        /* [out] */ Int32* res);

    /**
     * Sets the id of a view for which this view serves as a label for
     * accessibility purposes.
     *
     * @param id The labeled view id.
     */
    //@RemotableViewMethod
    virtual CARAPI SetLabelFor(
        /* [in] */ Int32 id);

    virtual CARAPI GetContentDescription(
        /* [out] */ ICharSequence** res);

    virtual CARAPI SetContentDescription(
        /* [in] */ ICharSequence* contentDescription);

    virtual CARAPI IsFocused(
        /* [out] */ Boolean* res);

    virtual CARAPI FindFocus(
        /* [out] */ IView** res);

    /**
     * Indicates whether this view is one of the set of scrollable containers in
     * its window.
     *
     * @return whether this view is one of the set of scrollable containers in
     * its window
     *
     * @attr ref android.R.styleable#View_isScrollContainer
     */
    virtual CARAPI IsScrollContainer(
        /* [out] */ Boolean* res);

    virtual CARAPI SetScrollContainer(
        /* [in] */ Boolean isScrollContainer);

    virtual CARAPI GetDrawingCacheQuality(
        /* [out] */ Int32* res);

    virtual CARAPI SetDrawingCacheQuality(
        /* [in] */ Int32 quality);

    virtual CARAPI GetKeepScreenOn(
        /* [out] */ Boolean* res);

    virtual CARAPI SetKeepScreenOn(
        /* [in] */ Boolean keepScreenOn);

    virtual CARAPI GetNextFocusLeftId(
        /* [out] */ Int32* res);

    virtual CARAPI SetNextFocusLeftId(
        /* [in] */ Int32 nextFocusLeftId);

    virtual CARAPI GetNextFocusRightId(
        /* [out] */ Int32* res);

    virtual CARAPI SetNextFocusRightId(
        /* [in] */ Int32 nextFocusRightId);

    virtual CARAPI GetNextFocusUpId(
        /* [out] */ Int32* res);

    virtual CARAPI SetNextFocusUpId(
        /* [in] */ Int32 nextFocusUpId);

    virtual CARAPI GetNextFocusDownId(
        /* [out] */ Int32* res);

    virtual CARAPI SetNextFocusDownId(
        /* [in] */ Int32 nextFocusDownId);

    /**
     * Gets the id of the view to use when the next focus is {@link #FOCUS_FORWARD}.
     * @return The next focus ID, or {@link #NO_ID} if the framework should decide automatically.
     *
     * @attr ref android.R.styleable#View_nextFocusForward
     */
    virtual CARAPI GetNextFocusForwardId(
        /* [out] */ Int32* res);

    /**
     * Sets the id of the view to use when the next focus is {@link #FOCUS_FORWARD}.
     * @param nextFocusForwardId The next focus ID, or {@link #NO_ID} if the framework should
     * decide automatically.
     *
     * @attr ref android.R.styleable#View_nextFocusForward
     */
    virtual CARAPI SetNextFocusForwardId(
        /* [in] */ Int32 nextFocusForwardId);

    virtual CARAPI IsShown(
        /* [out] */ Boolean* res);

    /**
     * Sets whether or not this view should account for system screen decorations
     * such as the status bar and inset its content; that is, controlling whether
     * the default implementation of {@link #fitSystemWindows(Rect)} will be
     * executed.  See that method for more details.
     *
     * <p>Note that if you are providing your own implementation of
     * {@link #fitSystemWindows(Rect)}, then there is no need to set this
     * flag to true -- your implementation will be overriding the default
     * implementation that checks this flag.
     *
     * @param fitSystemWindows If true, then the default implementation of
     * {@link #fitSystemWindows(Rect)} will be executed.
     *
     * @attr ref android.R.styleable#View_fitsSystemWindows
     * @see #getFitsSystemWindows()
     * @see #fitSystemWindows(Rect)
     * @see #setSystemUiVisibility(int)
     */
    virtual CARAPI SetFitsSystemWindows(
        /* [in] */ Boolean fitSystemWindows);

    /**
     * Check for state of {@link #setFitsSystemWindows(boolean). If this method
     * returns true, the default implementation of {@link #fitSystemWindows(Rect)}
     * will be executed.
     *
     * @return Returns true if the default implementation of
     * {@link #fitSystemWindows(Rect)} will be executed.
     *
     * @attr ref android.R.styleable#View_fitsSystemWindows
     * @see #setFitsSystemWindows()
     * @see #fitSystemWindows(Rect)
     * @see #setSystemUiVisibility(int)
     */
    virtual CARAPI GetFitsSystemWindows(
        /* [out] */ Boolean* res);

    /** @hide */
    virtual CARAPI FitsSystemWindows(
        /* [out] */ Boolean* res);

    /**
     * Ask that a new dispatch of {@link #fitSystemWindows(Rect)} be performed.
     */
    virtual CARAPI RequestFitSystemWindows();

    /**
     * For use by PhoneWindow to make its own system window fitting optional.
     * @hide
     */
    virtual CARAPI MakeOptionalFitsSystemWindows();

    virtual CARAPI GetVisibility(
        /* [out] */ Int32* res);

    virtual CARAPI SetVisibility(
        /* [in] */ Int32 visibility);

    virtual CARAPI IsEnabled(
        /* [out] */ Boolean* res);

    virtual CARAPI SetEnabled(
        /* [in] */ Boolean enabled);

    virtual CARAPI SetFocusable(
        /* [in] */ Boolean focusable);

    virtual CARAPI SetFocusableInTouchMode(
        /* [in] */ Boolean focusableInTouchMode);

    virtual CARAPI SetSoundEffectsEnabled(
        /* [in] */ Boolean soundEffectsEnabled);

    virtual CARAPI IsSoundEffectsEnabled(
        /* [out] */ Boolean* res);

    virtual CARAPI SetHapticFeedbackEnabled(
        /* [in] */ Boolean hapticFeedbackEnabled);

    virtual CARAPI IsHapticFeedbackEnabled(
        /* [out] */ Boolean* res);

    virtual CARAPI GetRawLayoutDirection(
        /* [out] */ Int32* res);

    virtual CARAPI SetLayoutDirection(
        /* [in] */ Int32 layoutDirection);

    virtual CARAPI GetLayoutDirection(
        /* [out] */ Int32* res);

    virtual CARAPI IsLayoutRtl(
        /* [out] */ Boolean* res);

    virtual CARAPI HasTransientState(
        /* [out] */ Boolean* res);

    virtual CARAPI SetHasTransientState(
        /* [in] */ Boolean hasTransientState);

    virtual CARAPI SetWillNotDraw(
        /* [in] */ Boolean willNotDraw);

    virtual CARAPI WillNotDraw(
        /* [out] */ Boolean* res);

    virtual CARAPI SetWillNotCacheDrawing(
        /* [in] */ Boolean willNotCacheDrawing);

    virtual CARAPI WillNotCacheDrawing(
        /* [out] */ Boolean* res);

    virtual CARAPI IsClickable(
        /* [out] */ Boolean* res);

    virtual CARAPI SetClickable(
        /* [in] */ Boolean clickable);

    virtual CARAPI IsLongClickable(
        /* [out] */ Boolean* res);

    virtual CARAPI SetLongClickable(
        /* [in] */ Boolean longClickable);

    virtual CARAPI SetPressed(
        /* [in] */ Boolean pressed);

    virtual CARAPI IsPressed(
        /* [out] */ Boolean* res);

    virtual CARAPI IsSaveEnabled(
        /* [out] */ Boolean* res);

    virtual CARAPI SetSaveEnabled(
        /* [in] */ Boolean enabled);

    virtual CARAPI GetFilterTouchesWhenObscured(
        /* [out] */ Boolean* res);

    virtual CARAPI SetFilterTouchesWhenObscured(
        /* [in] */ Boolean enabled);

    virtual CARAPI IsSaveFromParentEnabled(
        /* [out] */ Boolean* res);

    virtual CARAPI SetSaveFromParentEnabled(
        /* [in] */ Boolean enabled);

    virtual CARAPI IsFocusable(
        /* [out] */ Boolean* res);

    virtual CARAPI IsFocusableInTouchMode(
        /* [out] */ Boolean* res);

    virtual CARAPI FocusSearch(
        /* [in] */ Int32 direction,
        /* [out] */ IView** res);

    virtual CARAPI DispatchUnhandledMove(
        /* [in] */ IView* focused,
        /* [in] */ Int32 direction,
        /* [out] */ Boolean* res);

    /**
     * If a user manually specified the next view id for a particular direction,
     * use the root to look up the view.  Once a view is found, it is cached
     * for future lookups.
     * @param root The root view of the hierarchy containing this view.
     * @param direction One of FOCUS_UP, FOCUS_DOWN, FOCUS_LEFT, and FOCUS_RIGHT
     * @return The user specified next view, or NULL if there is none.
     */
    virtual CARAPI FindUserSetNextFocus(
        /* [in] */ IView* root,
        /* [in] */ Int32 direction,
        /* [out] */ IView** res);

    virtual CARAPI GetFocusables(
        /* [in] */ Int32 direction,
        /* [out] */ IArrayList** views);

    virtual CARAPI AddFocusables(
        /* [in] */ IArrayList* views,
        /* [in] */ Int32 direction);

    virtual CARAPI AddFocusables(
        /* [in] */ IArrayList* views,
        /* [in] */ Int32 direction,
        /* [in] */ Int32 focusableMode);

    virtual CARAPI FindViewsWithText(
        /* [in, out] */ IArrayList* outViews,
        /* [in] */ ICharSequence* searched,
        /* [in] */ Int32 flags);

    virtual CARAPI GetTouchables(
        /* [out] */ IArrayList** views);

    virtual CARAPI AddTouchables(
        /* [in] */ IArrayList* views);

    virtual CARAPI RequestAccessibilityFocus(
        /* [out] */ Boolean* res);

    virtual CARAPI ClearAccessibilityFocus();

    virtual CARAPI RequestFocus(
        /* [out] */ Boolean* res);

    virtual CARAPI RequestFocus(
        /* [in] */ Int32 direction,
        /* [out] */ Boolean* res);

    virtual CARAPI RequestFocus(
        /* [in] */ Int32 direction,
        /* [in] */ IRect* previouslyFocusedRect,
        /* [out] */ Boolean* res);

    virtual CARAPI RequestFocusFromTouch(
        /* [out] */ Boolean* res);

    virtual CARAPI GetImportantForAccessibility(
        /* [out] */ Int32* res);

    virtual CARAPI SetImportantForAccessibility(
        /* [in] */ Int32 mode);

    virtual CARAPI IsImportantForAccessibility(
        /* [out] */ Boolean* isImportant);

    virtual CARAPI GetParentForAccessibility(
        /* [out] */ IViewParent** res);

    virtual CARAPI AddChildrenForAccessibility(
        /* [in] */ IArrayList* children);

    virtual CARAPI IncludeForAccessibility(
        /* [out] */ Boolean* res);

    virtual CARAPI IsActionableForAccessibility(
        /* [out] */ Boolean* res);

    virtual CARAPI NotifyViewAccessibilityStateChangedIfNeeded(
        /* [in] */ Int32 changeType);

    virtual CARAPI NotifySubtreeAccessibilityStateChangedIfNeeded();

    virtual CARAPI ResetSubtreeAccessibilityStateChanged();

    virtual CARAPI PerformAccessibilityAction(
        /* [in] */ Int32 action,
        /* [in] */ IBundle* arguments,
        /* [out] */ Boolean* res);

    virtual CARAPI GetIterableTextForAccessibility(
        /* [out] */ ICharSequence** res);

    virtual CARAPI GetAccessibilitySelectionStart(
        /* [out] */ Int32* res);

    virtual CARAPI SetAccessibilitySelection(
        /* [in] */ Int32 start,
        /* [in] */ Int32 end);

    virtual CARAPI GetIteratorForGranularity(
        /* [in] */ Int32 granularity,
        /* [out] */ ITextSegmentIterator** result);

    virtual CARAPI DispatchStartTemporaryDetach();

    virtual CARAPI OnStartTemporaryDetach();

    virtual CARAPI DispatchFinishTemporaryDetach();

    virtual CARAPI OnFinishTemporaryDetach();

    virtual CARAPI GetKeyDispatcherState(
        /* [out] */ IDispatcherState** state);

    virtual CARAPI DispatchKeyEventPreIme(
        /* [in] */ IKeyEvent* event,
        /* [out] */ Boolean* res);

    virtual CARAPI DispatchKeyEvent(
        /* [in] */ IKeyEvent* event,
        /* [out] */ Boolean* res);

    virtual CARAPI DispatchKeyShortcutEvent(
        /* [in] */ IKeyEvent* event,
        /* [out] */ Boolean* res);

    virtual CARAPI DispatchTouchEvent(
        /* [in] */ IMotionEvent* event,
        /* [out] */ Boolean* res);

    virtual CARAPI OnFilterTouchEventForSecurity(
        /* [in] */ IMotionEvent* event,
        /* [out] */ Boolean* res);

    virtual CARAPI DispatchTrackballEvent(
        /* [in] */ IMotionEvent* event,
        /* [out] */ Boolean* res);

    virtual CARAPI DispatchGenericMotionEvent(
        /* [in] */ IMotionEvent* event,
        /* [out] */ Boolean* res);

    virtual CARAPI DispatchPointerEvent(
        /* [in] */ IMotionEvent* event,
        /* [out] */ Boolean* res);

    virtual CARAPI DispatchWindowFocusChanged(
        /* [in] */ Boolean hasFocus);

    virtual CARAPI OnWindowFocusChanged(
        /* [in] */ Boolean hasWindowFocus);

    virtual CARAPI HasWindowFocus(
        /* [out] */ Boolean* res);

    virtual CARAPI DispatchDisplayHint(
        /* [in] */ Int32 hint);

    virtual CARAPI DispatchWindowVisibilityChanged(
        /* [in] */ Int32 visibility);

    virtual CARAPI GetWindowVisibility(
        /* [out] */ Int32* res);

    virtual CARAPI GetWindowVisibleDisplayFrame(
        /* [in, out] */ IRect* outRect);

    virtual CARAPI DispatchConfigurationChanged(
        /* [in] */ IConfiguration* newConfig);

    /**
     * Private function to aggregate all per-view attributes in to the view
     * root.
     */
    virtual CARAPI DispatchCollectViewAttributes(
        /* [in] */ AttachInfo* info,
        /* [in] */ Int32 visibility);

    virtual CARAPI PerformCollectViewAttributes(
        /* [in] */ AttachInfo* info,
        /* [in] */ Int32 visibility);

    virtual CARAPI NeedGlobalAttributesUpdate(
        /* [in] */ Boolean force);

    virtual CARAPI IsInTouchMode(
        /* [out] */ Boolean* res);

    virtual CARAPI GetContext(
        /* [out] */ IContext** res);

    virtual CARAPI OnKeyPreIme(
        /* [in] */ Int32 keyCode,
        /* [in] */ IKeyEvent* event,
        /* [out] */ Boolean* res);

    virtual CARAPI OnKeyDown(
        /* [in] */ Int32 keyCode,
        /* [in] */ IKeyEvent* event,
        /* [out] */ Boolean* res);

    virtual CARAPI OnKeyLongPress(
        /* [in] */ Int32 keyCode,
        /* [in] */ IKeyEvent* event,
        /* [out] */ Boolean* res);

    virtual CARAPI OnKeyUp(
        /* [in] */ Int32 keyCode,
        /* [in] */ IKeyEvent* event,
        /* [out] */ Boolean* res);

    virtual CARAPI OnKeyMultiple(
        /* [in] */ Int32 keyCode,
        /* [in] */ Int32 repeatCount,
        /* [in] */ IKeyEvent* event,
        /* [out] */ Boolean* res);

    virtual CARAPI OnKeyShortcut(
        /* [in] */ Int32 keyCode,
        /* [in] */ IKeyEvent* event,
        /* [out] */ Boolean* res);

    virtual CARAPI OnCheckIsTextEditor(
        /* [out] */ Boolean* res);

    virtual CARAPI OnCreateInputConnection(
        /* [in] */ IEditorInfo* outAttrs,
        /* [out] */ IInputConnection** res);

    virtual CARAPI CheckInputConnectionProxy(
        /* [in] */ IView* view,
        /* [out] */ Boolean* res);

    virtual CARAPI CreateContextMenu(
        /* [in] */ IContextMenu* menu);

    virtual CARAPI OnTrackballEvent(
        /* [in] */ IMotionEvent* event,
        /* [out] */ Boolean* res);

    virtual CARAPI OnGenericMotionEvent(
        /* [in] */ IMotionEvent* event,
        /* [out] */ Boolean* res);

    virtual CARAPI OnHoverEvent(
        /* [in] */ IMotionEvent* event,
        /* [out] */ Boolean* res);

    virtual CARAPI IsHovered(
        /* [out] */ Boolean* res);

    virtual CARAPI SetHovered(
        /* [in] */ Boolean hovered);

    virtual CARAPI OnHoverChanged(
        /* [in] */ Boolean hovered);

    virtual CARAPI OnTouchEvent(
        /* [in] */ IMotionEvent* event,
        /* [out] */ Boolean* res);

    virtual CARAPI IsInScrollingContainer(
        /* [out] */ Boolean* res);

    virtual CARAPI CancelLongPress();

    virtual CARAPI SetTouchDelegate(
        /* [in] */ ITouchDelegate* touchDelegate);

    virtual CARAPI GetTouchDelegate(
        /* [out] */ ITouchDelegate** touchDelegate);

    virtual CARAPI BringToFront();

    virtual CARAPI GetParent(
        /* [out] */ IViewParent** res);

    virtual CARAPI SetScrollX(
        /* [in] */ Int32 value);

    virtual CARAPI SetScrollY(
        /* [in] */ Int32 value);

    virtual CARAPI GetScrollX(
        /* [out] */ Int32* res);

    virtual CARAPI GetScrollY(
        /* [out] */ Int32* res);

    virtual CARAPI GetWidth(
        /* [out] */ Int32* res);

    virtual CARAPI GetHeight(
        /* [out] */ Int32* res);

    virtual CARAPI GetDrawingRect(
        /* [out] */ IRect* outRect);

    virtual CARAPI GetMeasuredWidth(
        /* [out] */ Int32* res);

    virtual CARAPI GetMeasuredWidthAndState(
        /* [out] */ Int32* res);

    virtual CARAPI GetMeasuredHeight(
        /* [out] */ Int32* res);

    virtual CARAPI GetMeasuredHeightAndState(
        /* [out] */ Int32* res);

    virtual CARAPI GetMeasuredState(
        /* [out] */ Int32* res);

    virtual CARAPI GetMatrix(
        /* [out] */ IMatrix** res);

    virtual CARAPI GetCameraDistance(
        /* [out] */ Float* res);

    virtual CARAPI SetCameraDistance(
        /* [in] */ Float distance);

    virtual CARAPI GetRotation(
        /* [out] */ Float* res);

    virtual CARAPI SetRotation(
        /* [in] */ Float rotation);

    virtual CARAPI GetRotationY(
        /* [out] */ Float* res);

    virtual CARAPI SetRotationY(
        /* [in] */ Float rotationY);

    virtual CARAPI GetRotationX(
        /* [out] */ Float* res);

    virtual CARAPI SetRotationX(
        /* [in] */ Float rotationX);

    virtual CARAPI GetScaleX(
        /* [out] */ Float* res);

    virtual CARAPI SetScaleX(
        /* [in] */ Float scaleX);

    virtual CARAPI GetScaleY(
        /* [out] */ Float* res);

    virtual CARAPI SetScaleY(
        /* [in] */ Float scaleY);

    virtual CARAPI GetPivotX(
        /* [out] */ Float* res);

    virtual CARAPI SetPivotX(
        /* [in] */ Float pivotX);

    virtual CARAPI GetPivotY(
        /* [out] */ Float* res);

    virtual CARAPI SetPivotY(
        /* [in] */ Float pivotY);

    virtual CARAPI GetAlpha(
        /* [out] */ Float* res);

    virtual CARAPI HasOverlappingRendering(
        /* [out] */ Boolean* res);

    virtual CARAPI SetAlpha(
        /* [in] */ Float alpha);

    virtual CARAPI GetTop(
        /* [out] */ Int32* res);

    virtual CARAPI SetTop(
        /* [in] */ Int32 top);

    virtual CARAPI GetBottom(
        /* [out] */ Int32* res);

    virtual CARAPI IsDirty(
        /* [out] */ Boolean* res);

    virtual CARAPI SetBottom(
        /* [in] */ Int32 bottom);

    virtual CARAPI GetLeft(
        /* [out] */ Int32* res);

    virtual CARAPI SetLeft(
        /* [in] */ Int32 left);

    virtual CARAPI GetRight(
        /* [out] */ Int32* res);

    virtual CARAPI SetRight(
        /* [in] */ Int32 right);

    virtual CARAPI GetX(
        /* [out] */ Float* x);

    virtual CARAPI SetX(
        /* [in] */ Float x);

    virtual CARAPI GetY(
        /* [out] */ Float* y);

    virtual CARAPI SetY(
        /* [in] */ Float y);

    virtual CARAPI GetTranslationX(
        /* [out] */ Float* res);

    virtual CARAPI SetTranslationX(
        /* [in] */ Float translationX);

    virtual CARAPI GetTranslationY(
        /* [out] */ Float* translationY);

    virtual CARAPI SetTranslationY(
        /* [in] */ Float translationY);

    virtual CARAPI GetHitRect(
        /* [in, out] */ IRect* outRect);

    virtual CARAPI GetFocusedRect(
        /* [in, out] */ IRect* outRect);

    virtual CARAPI GetGlobalVisibleRect(
        /* [in, out] */ IRect* outRect,
        /* [in, out] */ IPoint* outGlobalOffset,
        /* [out] */ Boolean* res);

    virtual CARAPI GetGlobalVisibleRect(
        /* [in, out] */ IRect* outRect,
        /* [out] */ Boolean* res);

    virtual CARAPI GetLocalVisibleRect(
        /* [in, out] */ IRect* outRect,
        /* [out] */ Boolean* res);

    virtual CARAPI OffsetTopAndBottom(
        /* [in] */ Int32 offset);

    virtual CARAPI OffsetLeftAndRight(
        /* [in] */ Int32 offset);

    virtual CARAPI GetLayoutParams(
        /* [out] */ IViewGroupLayoutParams** res);

    virtual CARAPI SetLayoutParams(
        /* [in] */ IViewGroupLayoutParams* params);

    virtual CARAPI ResolveLayoutParams();

    virtual CARAPI ScrollTo(
        /* [in] */ Int32 x,
        /* [in] */ Int32 y);

    virtual CARAPI ScrollBy(
        /* [in] */ Int32 x,
        /* [in] */ Int32 y);

    virtual CARAPI Invalidate(
        /* [in] */ IRect* dirty);

    virtual CARAPI Invalidate(
        /* [in] */ Int32 left,
        /* [in] */ Int32 top,
        /* [in] */ Int32 right,
        /* [in] */ Int32 bottom);

    virtual CARAPI Invalidate();

    virtual CARAPI IsOpaque(
        /* [out] */ Boolean* res);

    virtual CARAPI GetHandler(
        /* [out] */ IHandler** res);

    /**
     * Causes the Runnable to be added to the message queue.
     * The runnable will be run on the user interface thread.
     *
     * @param action The Runnable that will be executed.
     *
     * @return Returns true if the Runnable was successfully placed in to the
     *         message queue.  Returns false on failure, usually because the
     *         looper processing the message queue is exiting.
     */
    virtual CARAPI Post(
        /* [in] */ IRunnable* action,
        /* [out] */ Boolean* res);

    /**
     * Causes the Runnable to be added to the message queue, to be run
     * after the specified amount of time elapses.
     * The runnable will be run on the user interface thread.
     *
     * @param action The Runnable that will be executed.
     * @param delayMillis The delay (in milliseconds) until the Runnable
     *        will be executed.
     *
     * @return true if the Runnable was successfully placed in to the
     *         message queue.  Returns false on failure, usually because the
     *         looper processing the message queue is exiting.  Note that a
     *         result of true does not mean the Runnable will be processed --
     *         if the looper is quit before the delivery time of the message
     *         occurs then the message will be dropped.
     */
    virtual CARAPI PostDelayed(
        /* [in] */ IRunnable* action,
        /* [in] */ Int64 delayMillis,
        /* [out] */ Boolean* res);

    /**
     * <p>Causes the Runnable to execute on the next animation time step.
     * The runnable will be run on the user interface thread.</p>
     *
     * @param action The Runnable that will be executed.
     *
     * @see #postOnAnimationDelayed
     * @see #removeCallbacks
     */
    virtual CARAPI PostOnAnimation(
        /* [in] */ IRunnable* action);

    /**
     * <p>Causes the Runnable to execute on the next animation time step,
     * after the specified amount of time elapses.
     * The runnable will be run on the user interface thread.</p>
     *
     * @param action The Runnable that will be executed.
     * @param delayMillis The delay (in milliseconds) until the Runnable
     *        will be executed.
     *
     * @see #postOnAnimation
     * @see #removeCallbacks
     */
    virtual CARAPI PostOnAnimationDelayed(
        /* [in] */ IRunnable* action,
        /* [in] */ Int64 delayMillis);

    /**
     * <p>Removes the specified Runnable from the message queue.</p>
     *
     * @param action The Runnable to remove from the message handling queue
     *
     * @return true if this view could ask the Handler to remove the Runnable,
     *         false otherwise. When the returned value is true, the Runnable
     *         may or may not have been actually removed from the message queue
     *         (for instance, if the Runnable was not in the queue already.)
     *
     * @see #post
     * @see #postDelayed
     * @see #postOnAnimation
     * @see #postOnAnimationDelayed
     */
    virtual CARAPI RemoveCallbacks(
        /* [in] */ IRunnable* action,
        /* [out] */ Boolean* res);

    /**
     * <p>Cause an invalidate to happen on a subsequent cycle through the event loop.
     * Use this to invalidate the View from a non-UI thread.</p>
     *
     * <p>This method can be invoked from outside of the UI thread
     * only when this View is attached to a window.</p>
     *
     * @see #invalidate()
     * @see #postInvalidateDelayed(long)
     */
    virtual CARAPI PostInvalidate();

    /**
     * <p>Cause an invalidate of the specified area to happen on a subsequent cycle
     * through the event loop. Use this to invalidate the View from a non-UI thread.</p>
     *
     * <p>This method can be invoked from outside of the UI thread
     * only when this View is attached to a window.</p>
     *
     * @param left The left coordinate of the rectangle to invalidate.
     * @param top The top coordinate of the rectangle to invalidate.
     * @param right The right coordinate of the rectangle to invalidate.
     * @param bottom The bottom coordinate of the rectangle to invalidate.
     *
     * @see #invalidate(int, int, int, int)
     * @see #invalidate(Rect)
     * @see #postInvalidateDelayed(long, int, int, int, int)
     */
    virtual CARAPI PostInvalidate(
        /* [in] */ Int32 left,
        /* [in] */ Int32 top,
        /* [in] */ Int32 right,
        /* [in] */ Int32 bottom);

    /**
     * <p>Cause an invalidate to happen on a subsequent cycle through the event
     * loop. Waits for the specified amount of time.</p>
     *
     * <p>This method can be invoked from outside of the UI thread
     * only when this View is attached to a window.</p>
     *
     * @param delayMilliseconds the duration in milliseconds to delay the
     *         invalidation by
     *
     * @see #invalidate()
     * @see #postInvalidate()
     */
    virtual CARAPI PostInvalidateDelayed(
        /* [in] */ Int64 delayMilliseconds);

    /**
     * <p>Cause an invalidate of the specified area to happen on a subsequent cycle
     * through the event loop. Waits for the specified amount of time.</p>
     *
     * <p>This method can be invoked from outside of the UI thread
     * only when this View is attached to a window.</p>
     *
     * @param delayMilliseconds the duration in milliseconds to delay the
     *         invalidation by
     * @param left The left coordinate of the rectangle to invalidate.
     * @param top The top coordinate of the rectangle to invalidate.
     * @param right The right coordinate of the rectangle to invalidate.
     * @param bottom The bottom coordinate of the rectangle to invalidate.
     *
     * @see #invalidate(int, int, int, int)
     * @see #invalidate(Rect)
     * @see #postInvalidate(int, int, int, int)
     */
    virtual CARAPI PostInvalidateDelayed(
        /* [in] */ Int64 delayMilliseconds,
        /* [in] */ Int32 left,
        /* [in] */ Int32 top,
        /* [in] */ Int32 right,
        /* [in] */ Int32 bottom);

    /**
     * <p>Cause an invalidate to happen on the next animation time step, typically the
     * next display frame.</p>
     *
     * <p>This method can be invoked from outside of the UI thread
     * only when this View is attached to a window.</p>
     *
     * @see #invalidate()
     */
    virtual CARAPI PostInvalidateOnAnimation();

    /**
     * <p>Cause an invalidate of the specified area to happen on the next animation
     * time step, typically the next display frame.</p>
     *
     * <p>This method can be invoked from outside of the UI thread
     * only when this View is attached to a window.</p>
     *
     * @param left The left coordinate of the rectangle to invalidate.
     * @param top The top coordinate of the rectangle to invalidate.
     * @param right The right coordinate of the rectangle to invalidate.
     * @param bottom The bottom coordinate of the rectangle to invalidate.
     *
     * @see #invalidate(int, int, int, int)
     * @see #invalidate(Rect)
     */
    virtual CARAPI PostInvalidateOnAnimation(
        /* [in] */ Int32 left,
        /* [in] */ Int32 top,
        /* [in] */ Int32 right,
        /* [in] */ Int32 bottom);

    virtual CARAPI ComputeScroll();

    virtual CARAPI IsHorizontalFadingEdgeEnabled(
        /* [out] */ Boolean* res);

    virtual CARAPI SetHorizontalFadingEdgeEnabled(
        /* [in] */ Boolean horizontalFadingEdgeEnabled);

    virtual CARAPI IsVerticalFadingEdgeEnabled(
        /* [out] */ Boolean* res);

    virtual CARAPI SetVerticalFadingEdgeEnabled(
        /* [in] */ Boolean verticalFadingEdgeEnabled);

    virtual CARAPI IsHorizontalScrollBarEnabled(
        /* [out] */ Boolean* res);

    virtual CARAPI SetHorizontalScrollBarEnabled(
        /* [in] */ Boolean horizontalScrollBarEnabled);

    virtual CARAPI IsVerticalScrollBarEnabled(
        /* [out] */ Boolean* res);

    virtual CARAPI SetVerticalScrollBarEnabled(
        /* [in] */ Boolean verticalScrollBarEnabled);

    virtual CARAPI SetScrollbarFadingEnabled(
        /* [in] */ Boolean fadeScrollbars);

    virtual CARAPI IsScrollbarFadingEnabled(
        /* [out] */ Boolean* res);

    virtual CARAPI GetScrollBarDefaultDelayBeforeFade(
        /* [out] */ Int32* res);

    virtual CARAPI SetScrollBarDefaultDelayBeforeFade(
        /* [in] */ Int32 scrollBarDefaultDelayBeforeFade);

    virtual CARAPI GetScrollBarFadeDuration(
        /* [out] */ Int32* res);

    virtual CARAPI SetScrollBarFadeDuration(
        /* [in] */ Int32 scrollBarFadeDuration);

    virtual CARAPI GetScrollBarSize(
        /* [out] */ Int32* res);

    virtual CARAPI SetScrollBarSize(
        /* [in] */ Int32 scrollBarSize);

    virtual CARAPI SetScrollBarStyle(
        /* [in] */ Int32 style);

    virtual CARAPI GetScrollBarStyle(
        /* [out] */ Int32* res);

    virtual CARAPI CanScrollHorizontally(
        /* [in] */ Int32 direction,
        /* [out] */ Boolean* res);

    virtual CARAPI CanScrollVertically(
        /* [in] */ Int32 direction,
        /* [out] */ Boolean* res);

    virtual CARAPI ResolveRtlPropertiesIfNeeded(
        /* [out] */ Boolean* res);

    virtual CARAPI ResetRtlProperties();

    virtual CARAPI OnScreenStateChanged(
        /* [in] */ Int32 screenState);

    virtual CARAPI OnRtlPropertiesChanged(
        /* [in] */ Int32 layoutDirection);

    virtual CARAPI ResolveLayoutDirection(
        /* [out] */ Boolean* res);

    virtual CARAPI CanResolveLayoutDirection(
        /* [out] */ Boolean* res);

    virtual CARAPI ResetResolvedLayoutDirection();

    virtual CARAPI IsLayoutDirectionInherited(
        /* [out] */ Boolean* res);

    virtual CARAPI ResolvePadding();

    virtual CARAPI ResetResolvedPadding();

    virtual CARAPI AssignParent(
        /* [in] */ IViewParent* parent);

    virtual CARAPI GetWindowToken(
        /* [out] */ IBinder** res);

    virtual CARAPI GetApplicationWindowToken(
        /* [out] */ IBinder** res);

    virtual CARAPI GetDisplay(
        /* [out] */ IDisplay** res);

    virtual CARAPI GetWindowSession(
        /* [out] */ IWindowSession** res);

    virtual CARAPI DispatchAttachedToWindow(
        /* [in] */ AttachInfo* info,
        /* [in] */ Int32 visibility);

    virtual CARAPI DispatchDetachedFromWindow();

    virtual CARAPI SaveHierarchyState(
        /* [in] */ ISparseArray* container);

    virtual CARAPI RestoreHierarchyState(
        /* [in] */ ISparseArray* container);

    virtual CARAPI GetDrawingTime(
        /* [out] */ Int64* res);

    virtual CARAPI SetDuplicateParentStateEnabled(
        /* [in] */ Boolean enabled);

    virtual CARAPI IsDuplicateParentStateEnabled(
        /* [out] */ Boolean* res);

    virtual CARAPI SetLayerType(
        /* [in] */ Int32 layerType,
        /* [in] */ IPaint* paint);

    virtual CARAPI SetLayerPaint(
        /* [in] */ IPaint* paint);

    virtual CARAPI GetLayerType(
        /* [out] */ Int32* res);

    virtual CARAPI BuildLayer();

    virtual CARAPI SetDrawingCacheEnabled(
        /* [in] */ Boolean enabled);

    virtual CARAPI IsDrawingCacheEnabled(
        /* [out] */ Boolean* res);

    virtual CARAPI CanHaveDisplayList(
        /* [out] */ Boolean* res);

    virtual CARAPI GetHardwareRenderer(
        /* [out] */ IHardwareRenderer** res);

    virtual CARAPI GetDisplayList(
        /* [out] */ IRenderNode** node);

    virtual CARAPI GetDrawingCache(
        /* [out] */ IBitmap** res);

    virtual CARAPI GetDrawingCache(
        /* [in] */ Boolean autoScale,
        /* [out] */ IBitmap** res);

    virtual CARAPI DestroyDrawingCache();

    virtual CARAPI SetDrawingCacheBackgroundColor(
        /* [in] */ Int32 color);

    virtual CARAPI GetDrawingCacheBackgroundColor(
        /* [out] */ Int32* res);

    virtual CARAPI BuildDrawingCache();

    virtual CARAPI BuildDrawingCache(
        /* [in] */ Boolean autoScale);

    virtual CARAPI CreateSnapshot(
        /* [in] */ BitmapConfig quality,
        /* [in] */ Int32 backgroundColor,
        /* [in] */ Boolean skipChildren,
        /* [out] */ IBitmap** bitmap);

    virtual CARAPI IsInEditMode(
        /* [out] */ Boolean* res);

    virtual CARAPI IsHardwareAccelerated(
        /* [out] */ Boolean* res);

    virtual CARAPI Draw(
        /* [in] */ ICanvas* canvas);

    virtual CARAPI GetSolidColor(
        /* [out] */ Int32* res);

    virtual CARAPI IsLayoutRequested(
        /* [out] */ Boolean* res);

    virtual CARAPI Layout(
        /* [in] */ Int32 l,
        /* [in] */ Int32 t,
        /* [in] */ Int32 r,
        /* [in] */ Int32 b);

    virtual CARAPI GetResources(
        /* [out] */ IResources** res);

    virtual CARAPI InvalidateDrawable(
        /* [in] */ IDrawable* drawable);

    virtual CARAPI ScheduleDrawable(
        /* [in] */ IDrawable* who,
        /* [in] */ IRunnable* what,
        /* [in] */ Int64 when);

    virtual CARAPI UnscheduleDrawable(
        /* [in] */ IDrawable* who,
        /* [in] */ IRunnable* what);

    virtual CARAPI UnscheduleDrawable(
        /* [in] */ IDrawable* who);

    virtual CARAPI OnResolveDrawables(
        /* [in] */ Int32 layoutDirection);

    virtual CARAPI RefreshDrawableState();

    virtual CARAPI GetDrawableState(
        /* [out, callee] */ ArrayOf<Int32>** res);

    virtual CARAPI JumpDrawablesToCurrentState();

    virtual CARAPI SetBackgroundColor(
        /* [in] */ Int32 color);

    virtual CARAPI SetBackgroundResource(
        /* [in] */ Int32 resid);

    virtual CARAPI SetBackground(
        /* [in] */ IDrawable* background);

    virtual CARAPI SetBackgroundDrawable(
        /* [in] */ IDrawable* d);

    virtual CARAPI GetBackground(
        /* [out] */ IDrawable** res);

    virtual CARAPI SetPadding(
        /* [in] */ Int32 left,
        /* [in] */ Int32 top,
        /* [in] */ Int32 right,
        /* [in] */ Int32 bottom);

    virtual CARAPI SetPaddingRelative(
        /* [in] */ Int32 start,
        /* [in] */ Int32 top,
        /* [in] */ Int32 end,
        /* [in] */ Int32 bottom);

    virtual CARAPI GetPaddingTop(
        /* [out] */ Int32* res);

    virtual CARAPI GetPaddingBottom(
        /* [out] */ Int32* res);

    virtual CARAPI GetPaddingLeft(
        /* [out] */ Int32* res);

    virtual CARAPI GetPaddingStart(
        /* [out] */ Int32* res);

    virtual CARAPI GetPaddingRight(
        /* [out] */ Int32* res);

    virtual CARAPI GetPaddingEnd(
        /* [out] */ Int32* res);

    virtual CARAPI IsPaddingRelative(
        /* [out] */ Boolean* res);

    virtual CARAPI ResetPaddingToInitialValues();

    virtual CARAPI GetOpticalInsets(
        /* [out] */ IInsets** res);

    virtual CARAPI SetOpticalInsets(
        /* [in] */ IInsets* insets);

    virtual CARAPI SetSelected(
        /* [in] */ Boolean selected);

    virtual CARAPI IsSelected(
        /* [out] */ Boolean* res);

    virtual CARAPI SetActivated(
        /* [in] */ Boolean activated);

    virtual CARAPI IsActivated(
        /* [out] */ Boolean* res);

    virtual CARAPI GetViewTreeObserver(
        /* [out] */ IViewTreeObserver** res);

    virtual CARAPI GetRootView(
        /* [out] */ IView** res);

    virtual CARAPI GetLocationOnScreen(
        /* [in] */ ArrayOf<Int32>* location);

    virtual CARAPI GetLocationInWindow(
        /* [in] */ ArrayOf<Int32>* location);

    CARAPI FindViewById(
        /* [in] */ Int32 id,
        /* [out] */ IView** res);

    /**
     * Finds a view by its unuque and stable accessibility id.
     *
     * @param accessibilityId The searched accessibility id.
     * @return The found view.
     */
    virtual CARAPI_(AutoPtr<IView>) FindViewByAccessibilityId(
        /* [in] */ Int32 accessibilityId);

    CARAPI FindViewWithTag(
        /* [in] */ IInterface* tag,
        /* [out] */ IView** res);

    CARAPI FindViewByPredicate(
        /* [in] */ IPredicate* predicate,
        /* [out] */ IView** res);

    CARAPI FindViewByPredicateInsideOut(
        /* [in] */ IView* start,
        /* [in] */ IPredicate* predicate,
        /* [out] */ IView** res);

    virtual CARAPI SetId(
        /* [in] */ Int32 id);

    virtual CARAPI SetIsRootNamespace(
        /* [in] */ Boolean isRoot);

    virtual CARAPI IsRootNamespace(
        /* [out] */ Boolean* res);

    virtual CARAPI GetId(
        /* [out] */ Int32* res);

    virtual CARAPI GetTag(
        /* [out] */ IInterface** res);

    virtual CARAPI SetTag(
        /* [in] */ IInterface* tag);

    virtual CARAPI GetTag(
        /* [in] */ Int32 key,
        /* [out] */ IInterface** res);

    virtual CARAPI SetTag(
       /* [in] */ Int32 key,
       /* [in] */ IInterface* tag);

    virtual CARAPI SetTagInternal(
       /* [in] */ Int32 key,
       /* [in] */ IInterface* tag);

    virtual CARAPI HackTurnOffWindowResizeAnim(
        /* [in] */ Boolean off);

    virtual CARAPI Animate(
        /* [out] */ IViewPropertyAnimator** res);

    /**
     * Prints information about this view in the log output, with the tag
     * {@link #VIEW_LOG_TAG}.
     *
     * @hide
     */
    virtual CARAPI_(void) Debug();

    virtual CARAPI GetBaseline(
        /* [out] */ Int32* baseline);

    virtual CARAPI RequestLayout();

    virtual CARAPI ForceLayout();

    CARAPI Measure(
        /* [in] */ Int32 widthMeasureSpec,
        /* [in] */ Int32 heightMeasureSpec);

    /**
     * Version of {@link #resolveSizeAndState(int, int, int)}
     * returning only the {@link #MEASURED_SIZE_MASK} bits of the result.
     */
    ///////////////jiazhenjiang
    static CARAPI_(Int32) ResolveSize(
        /* [in] */ Int32 size,
        /* [in] */ Int32 measureSpec);

    /**
     * Utility to reconcile a desired size and state, with constraints imposed
     * by a MeasureSpec.  Will take the desired size, unless a different size
     * is imposed by the constraints.  The returned value is a compound integer,
     * with the resolved size in the {@link #MEASURED_SIZE_MASK} bits and
     * optionally the bit {@link #MEASURED_STATE_TOO_SMALL} set if the resulting
     * size is smaller than the size the view wants to be.
     *
     * @param size How big the view wants to be
     * @param measureSpec Constraints imposed by the parent
     * @return Size information bit mask as defined by
     * {@link #MEASURED_SIZE_MASK} and {@link #MEASURED_STATE_TOO_SMALL}.
     */
    static CARAPI_(Int32) ResolveSizeAndState(
        /* [in] */ Int32 size,
        /* [in] */ Int32 measureSpec,
        /* [in] */ Int32 childMeasuredState);

    /**
     * Utility to return a default size. Uses the supplied size if the
     * MeasureSpec imposed no contraints. Will get larger if allowed
     * by the MeasureSpec.
     *
     * @param size Default size for this view
     * @param measureSpec Constraints imposed by the parent
     * @return The size this view should be.
     */
    static CARAPI_(Int32) GetDefaultSize(
        /* [in] */ Int32 size,
        /* [in] */ Int32 measureSpec);

    virtual CARAPI GetMinimumHeight(
        /* [out] */ Int32* res);

    virtual CARAPI SetMinimumHeight(
        /* [in] */ Int32 minHeight);

    virtual CARAPI GetMinimumWidth(
        /* [out] */ Int32* res);

    virtual CARAPI SetMinimumWidth(
        /* [in] */ Int32 minWidth);

    virtual CARAPI GetAnimation(
        /* [out] */ IAnimation** res);

    virtual CARAPI StartAnimation(
        /* [in] */ IAnimation* animation);

    virtual CARAPI ClearAnimation();

    virtual CARAPI SetAnimation(
        /* [in] */ IAnimation* animation);

    virtual CARAPI GatherTransparentRegion(
        /* [in] */ IRegion* region,
        /* [out] */ Boolean* res);

    virtual CARAPI PlaySoundEffect(
        /* [in] */ Int32 soundConstant);

    virtual CARAPI PerformHapticFeedback(
        /* [in] */ Int32 feedbackConstant,
        /* [out] */ Boolean* res);

    virtual CARAPI PerformHapticFeedback(
        /* [in] */ Int32 feedbackConstant,
        /* [in] */ Int32 flags,
        /* [out] */ Boolean* res);

    virtual CARAPI SetSystemUiVisibility(
        /* [in] */ Int32 visibility);

    virtual CARAPI GetSystemUiVisibility(
        /* [out] */ Int32* res);

    virtual CARAPI GetWindowSystemUiVisibility(
        /* [out] */ Int32* res);

    virtual CARAPI OnWindowSystemUiVisibilityChanged(
        /* [in] */ Int32 visible);

    virtual CARAPI DispatchWindowSystemUiVisiblityChanged(
        /* [in] */ Int32 visible);

    virtual CARAPI SetOnSystemUiVisibilityChangeListener(
        /* [in] */ IViewOnSystemUiVisibilityChangeListener* l);

    virtual CARAPI DispatchSystemUiVisibilityChanged(
        /* [in] */ Int32 visibility);

    virtual CARAPI SetDisabledSystemUiVisibility(
        /* [in] */ Int32 flags);

    virtual CARAPI StartDrag(
        /* [in] */ IClipData* data,
        /* [in] */ IDragShadowBuilder* shadowBuilder,
        /* [in] */ IInterface* myLocalState,
        /* [in] */ Int32 flags,
        /* [out] */ Boolean* res);

    virtual CARAPI OnDragEvent(
        /* [in] */ IDragEvent* event,
        /* [out] */ Boolean* res);

    virtual CARAPI DispatchDragEvent(
        /* [in] */ IDragEvent* event,
        /* [out] */ Boolean* res);

    virtual CARAPI OnCloseSystemDialogs(
        /* [in] */ const String& reason);

    virtual CARAPI ApplyDrawableToTransparentRegion(
        /* [in] */ IDrawable* dr,
        /* [in] */ IRegion* region);

    static CARAPI Inflate(
        /* [in] */ IContext* context,
        /* [in] */ Int32 resource,
        /* [in] */ IViewGroup* root,
        /* [out] */ IView** view);

    virtual CARAPI GetOverScrollMode(
        /* [out] */ Int32* res);

    virtual CARAPI SetOverScrollMode(
        /* [in] */ Int32 overScrollMode);

    virtual CARAPI GetRawTextDirection(
        /* [out] */ Int32* res);

    virtual CARAPI SetTextDirection(
        /* [in] */ Int32 textDirection);

    virtual CARAPI GetTextDirection(
        /* [out] */ Int32* res);

    virtual CARAPI ResolveTextDirection(
        /* [out] */ Boolean* res);

    virtual CARAPI ResetResolvedTextDirection();

    virtual CARAPI IsTextDirectionInherited(
        /* [out] */ Boolean* res);

    virtual CARAPI GetRawTextAlignment(
        /* [out] */ Int32* res);

    virtual CARAPI SetTextAlignment(
        /* [in] */ Int32 textAlignment);

    virtual CARAPI GetTextAlignment(
        /* [out] */ Int32* res);

    virtual CARAPI ResolveTextAlignment(
        /* [out] */ Boolean* res);

    virtual CARAPI ResetResolvedTextAlignment();

    virtual CARAPI IsTextAlignmentInherited(
        /* [out] */ Boolean* res);

    virtual CARAPI GetViewRootImpl(
        /* [out] */ IViewRootImpl** impl);

    /**
     * Merge two states as returned by {@link #getMeasuredState()}.
     * @param curState The current state as returned from a view or the result
     * of combining multiple views.
     * @param newState The new view state to combine.
     * @return Returns a new integer reflecting the combination of the two
     * states.
     */
    static CARAPI_(Int32) CombineMeasuredStates(
        /* [in] */ Int32 curState,
        /* [in] */ Int32 newState);

    /**
     * Generate a value suitable for use in {@link #setId(int)}.
     * This value will not collide with ID values generated at build time by aapt for R.id.
     *
     * @return a generated ID value
     */
    static CARAPI_(Int32) GenerateViewId();

    // For debug print out.
    CARAPI Log();

    CARAPI GetHotspotBounds(
        /* [in] */ IRect* outRect);

    static CARAPI_(void) OffsetRects(
        /* [in] */ IArrayList* rects,
        /* [in] */ Float offsetX,
        /* [in] */ Float offsetY);

    CARAPI OnApplyWindowInsets(
        /* [in] */ IWindowInsets* insets,
        /* [out] */ IWindowInsets** res);

    CARAPI SetOnApplyWindowInsetsListener(
        /* [in] */ IViewOnApplyWindowInsetsListener* listener);

    CARAPI DispatchApplyWindowInsets(
        /* [in] */ IWindowInsets* insets,
        /* [out] */ IWindowInsets** res);

    CARAPI ComputeSystemWindowInsets(
        /* [in] */ IWindowInsets* in,
        /* [in] */ IRect* outLocalInsets,
        /* [out] */ IWindowInsets** res);

    virtual CARAPI RequestApplyInsets();

    virtual CARAPI IsAttachedToWindow(
        /* [out] */ Boolean* res);

    virtual CARAPI IsLaidOut(
        /* [out] */ Boolean* res);

    virtual CARAPI SetAccessibilityLiveRegion(
        /* [in] */ Int32 mode);

    virtual CARAPI GetAccessibilityLiveRegion(
        /* [out] */ Int32* res);

    virtual CARAPI IsAccessibilitySelectionExtendable(
        /* [out] */ Boolean* res);

    virtual CARAPI GetAccessibilitySelectionEnd(
        /* [out] */ Int32* res);

    virtual CARAPI RequestUnbufferedDispatch(
        /* [in] */ IMotionEvent* event);

    virtual CARAPI SetTransitionAlpha(
        /* [in] */ Float alpha);

    virtual CARAPI GetTransitionAlpha(
        /* [out] */ Float* alpha);

    virtual CARAPI GetZ(
        /* [out] */ Float* z);

    virtual CARAPI SetZ(
        /* [in] */ Float z);

    virtual CARAPI GetElevation(
        /* [out] */ Float* elevation);

    virtual CARAPI SetElevation(
        /* [in] */ Float elevation);

    virtual CARAPI GetTranslationZ(
        /* [out] */ Float* translationZ);

    virtual CARAPI SetTranslationZ(
        /* [in] */ Float translationZ);

    virtual CARAPI SetAnimationMatrix(
        /* [in] */ IMatrix* matrix);

    virtual CARAPI GetStateListAnimator(
        /* [out] */ IStateListAnimator** animator);

    virtual CARAPI SetStateListAnimator(
        /* [in] */ IStateListAnimator* stateListAnimator);

    virtual CARAPI GetClipToOutline(
        /* [out] */ Boolean* res);

    virtual CARAPI SetClipToOutline(
        /* [in] */ Boolean clipToOutline);

    virtual CARAPI SetOutlineProvider(
        /* [in] */ IViewOutlineProvider* provider);

    virtual CARAPI GetOutlineProvider(
        /* [out] */ IViewOutlineProvider** res);

    virtual CARAPI InvalidateOutline();

    virtual CARAPI HasShadow(
        /* [out] */ Boolean* res);

    virtual CARAPI SetRevealClip(
        /* [in] */ Boolean shouldClip,
        /* [in] */ Float x,
        /* [in] */ Float y,
        /* [in] */ Float radius);

    virtual CARAPI PointInView(
        /* [in] */ Float localX,
        /* [in] */ Float localY,
        /* [in] */ Float slop,
        /* [out] */ Boolean* res);

    virtual CARAPI DamageInParent();

        /**
     * @return true if layout direction has been resolved.
     */
    virtual CARAPI IsLayoutDirectionResolved(
        /* [out] */ Boolean* res);

    virtual CARAPI GetWindowId(
        /* [out] */ IWindowId** id);

    virtual CARAPI CancelPendingInputEvents();

    virtual CARAPI OnCancelPendingInputEvents();

    virtual CARAPI SetClipBounds(
        /* [in] */ IRect* clipBounds);

    virtual CARAPI GetClipBounds(
        /* [out] */ IRect** clipBounds);

    virtual CARAPI GetOverlay(
        /* [out] */ IViewOverlay** clipBounds);

    static CARAPI_(Boolean) IsLayoutModeOptical(
        /* [in] */ IInterface* o);

    virtual CARAPI DrawableHotspotChanged(
        /* [in] */ Float x,
        /* [in] */ Float y);

    virtual CARAPI SetBackgroundTintList(
        /* [in] */ IColorStateList* tint);

    virtual CARAPI GetBackgroundTintList(
        /* [out] */ IColorStateList** res);

    virtual CARAPI SetBackgroundTintMode(
         /* [in] */ PorterDuffMode tintMode);

    virtual CARAPI GetBackgroundTintMode(
        /* [out] */ PorterDuffMode* res);

    virtual CARAPI ComputeOpticalInsets(
        /* [out] */ IInsets** res);

    virtual CARAPI ToGlobalMotionEvent(
        /* [in] */ IMotionEvent* ev,
        /* [out] */ Boolean* res);

    virtual CARAPI ToLocalMotionEvent(
        /* [in] */ IMotionEvent* ev,
        /* [out] */ Boolean* res);

    virtual CARAPI TransformMatrixToGlobal(
        /* [in] */ IMatrix* m);

    virtual CARAPI TransformMatrixToLocal(
        /* [in] */ IMatrix* m);

    virtual CARAPI GetLocationOnScreen(
        /* [out, callee] */ ArrayOf<Int32>** res);

    /**
     * Performs the traversal to find a view by its unuque and stable accessibility id.
     *
     * <strong>Note:</strong>This method does not stop at the root namespace
     * boundary since the user can touch the screen at an arbitrary location
     * potentially crossing the root namespace bounday which will send an
     * accessibility event to accessibility services and they should be able
     * to obtain the event source. Also accessibility ids are guaranteed to be
     * unique in the window.
     *
     * @param accessibilityId The accessibility id.
     * @return The found view.
     */
    virtual CARAPI FindViewByAccessibilityIdTraversal(
        /* [in] */ Int32 accessibilityId,
        /* [out] */ IView** res);

    virtual CARAPI IsInLayout(
        /* [out] */ Boolean* res);

    virtual CARAPI SetNestedScrollingEnabled(
        /* [in] */ Boolean enabled);

    virtual CARAPI IsNestedScrollingEnabled(
        /* [out] */ Boolean* res);

    virtual CARAPI StartNestedScroll(
        /* [in] */ Int32 axes,
        /* [out] */ Boolean* res);

    virtual CARAPI StopNestedScroll();

    virtual CARAPI HasNestedScrollingParent(
        /* [out] */ Boolean* res);

    virtual CARAPI DispatchNestedScroll(
        /* [in] */ Int32 dxConsumed,
        /* [in] */ Int32 dyConsumed,
        /* [in] */ Int32 dxUnconsumed,
        /* [in] */ Int32 dyUnconsumed,
        /* [in] */ ArrayOf<Int32>* offsetInWindow,
        /* [out] */ Boolean* res);

    virtual CARAPI DispatchNestedPreScroll(
        /* [in] */ Int32 dx,
        /* [in] */ Int32 dy,
        /* [in] */ ArrayOf<Int32>* consumed,
        /* [in] */ ArrayOf<Int32>* offsetInWindow,
        /* [out] */ Boolean* res);

    virtual CARAPI DispatchNestedFling(
        /* [in] */ Float velocityX,
        /* [in] */ Float velocityY,
        /* [in] */ Boolean consumed,
        /* [out] */ Boolean* res);

    virtual CARAPI DispatchNestedPreFling(
        /* [in] */ Float velocityX,
        /* [in] */ Float velocityY,
        /* [out] */ Boolean* res);

    /**
     * Check if text direction resolution can be done.
     *
     * @return true if text direction resolution can be done otherwise return false.
     */
    virtual CARAPI CanResolveTextDirection(
        /* [out] */ Boolean* res);

    /**
     * @return true if text direction is resolved.
     */
    virtual CARAPI IsTextDirectionResolved(
        /* [out] */ Boolean* res);

    /**
     * Check if text alignment resolution can be done.
     *
     * @return true if text alignment resolution can be done otherwise return false.
     */
    virtual CARAPI CanResolveTextAlignment(
        /* [out] */ Boolean* res);

    /**
     * @return true if text alignment is resolved.
     */
    virtual CARAPI IsTextAlignmentResolved(
        /* [out] */ Boolean* res);

    virtual CARAPI CaptureTransitioningViews(
        /* [in] */ IList* transitioningViews);

    virtual CARAPI FindNamedViews(
        /* [in] */ IMap* namedElements);

    virtual CARAPI SetTransitionName(
        /* [in] */ const String& transitionName);

    virtual CARAPI GetTransitionName(
        /* [out] */ String* name);

    /**
     * Returns whether this View is accessibility focused.
     *
     * @return True if this View is accessibility focused.
     */
    virtual CARAPI IsAccessibilityFocused(
        /* [out] */ Boolean* res);

    CARAPI GetPadding(
        /* [out] */ Int32* left,
        /* [out] */ Int32* top,
        /* [out] */ Int32* right,
        /* [out] */ Int32* bottom);

    CARAPI_(void) HoldContext();

protected:
    virtual CARAPI_(void) InitializeFadingEdge(
        /* [in] */ ITypedArray* a);

    virtual CARAPI_(void) InitializeFadingEdgeInternal(
        /* [in] */ ITypedArray* a);

    virtual CARAPI GetHorizontalScrollbarHeight(
        /* [out] */ Int32* height);

    virtual CARAPI_(void) InitializeScrollbars(
        /* [in] */ ITypedArray* a);

    virtual CARAPI_(void) InitializeScrollbarsInternal(
        /* [in] */ ITypedArray* a);

    virtual CARAPI_(void) OnFocusChanged(
        /* [in] */ Boolean gainFocus,
        /* [in] */ Int32 direction,
        /* [in] */ IRect* previouslyFocusedRect);

    virtual CARAPI_(void) OnFocusLost();

    /**
     * Called by the view hierarchy when the content insets for a window have
     * changed, to allow it to adjust its content to fit within those windows.
     * The content insets tell you the space that the status bar, input method,
     * and other system windows infringe on the application's window.
     *
     * <p>You do not normally need to deal with this function, since the default
     * window decoration given to applications takes care of applying it to the
     * content of the window.  If you use {@link #SYSTEM_UI_FLAG_LAYOUT_FULLSCREEN}
     * or {@link #SYSTEM_UI_FLAG_LAYOUT_HIDE_NAVIGATION} this will not be the case,
     * and your content can be placed under those system elements.  You can then
     * use this method within your view hierarchy if you have parts of your UI
     * which you would like to ensure are not being covered.
     *
     * <p>The default implementation of this method simply applies the content
     * inset's to the view's padding, consuming that content (modifying the
     * insets to be 0), and returning true.  This behavior is off by default, but can
     * be enabled through {@link #setFitsSystemWindows(boolean)}.
     *
     * <p>This function's traversal down the hierarchy is depth-first.  The same content
     * insets object is propagated down the hierarchy, so any changes made to it will
     * be seen by all following views (including potentially ones above in
     * the hierarchy since this is a depth-first traversal).  The first view
     * that returns true will abort the entire traversal.
     *
     * <p>The default implementation works well for a situation where it is
     * used with a container that covers the entire window, allowing it to
     * apply the appropriate insets to its content on all edges.  If you need
     * a more complicated layout (such as two different views fitting system
     * windows, one on the top of the window, and one on the bottom),
     * you can override the method and handle the insets however you would like.
     * Note that the insets provided by the framework are always relative to the
     * far edges of the window, not accounting for the location of the called view
     * within that window.  (In fact when this method is called you do not yet know
     * where the layout will place the view, as it is done before layout happens.)
     *
     * <p>Note: unlike many View methods, there is no dispatch phase to this
     * call.  If you are overriding it in a ViewGroup and want to allow the
     * call to continue to your children, you must be sure to call the super
     * implementation.
     *
     * <p>Here is a sample layout that makes use of fitting system windows
     * to have controls for a video view placed inside of the window decorations
     * that it hides and shows.  This can be used with code like the second
     * sample (video player) shown in {@link #setSystemUiVisibility(int)}.
     *
     * {@sample development/samples/ApiDemos/res/layout/video_player.xml complete}
     *
     * @param insets Current content insets of the window.  Prior to
     * {@link android.os.Build.VERSION_CODES#JELLY_BEAN} you must not modify
     * the insets or else you and Android will be unhappy.
     *
     * @return Return true if this view applied the insets and it should not
     * continue propagating further down the hierarchy, false otherwise.
     * @see #getFitsSystemWindows()
     * @see #setFitsSystemWindows(boolean)
     * @see #setSystemUiVisibility(int)
     */
    virtual CARAPI FitSystemWindows(
        /* [in] */ IRect* _insets,
        /* [out] */ Boolean* res);

    virtual CARAPI_(void) DispatchSetPressed(
        /* [in] */ Boolean pressed);

    virtual CARAPI_(void) DispatchVisibilityChanged(
        /* [in] */ IView* changedView,
        /* [in] */ Int32 visibility);

    virtual CARAPI OnVisibilityChanged(
        /* [in] */ IView* changedView,
        /* [in] */ Int32 visibility);

    virtual CARAPI_(void) OnDisplayHint(
        /* [in] */ Int32 hint);

    virtual CARAPI OnWindowVisibilityChanged(
        /* [in] */ Int32 visibility);

    virtual CARAPI OnConfigurationChanged(
        /* [in] */ IConfiguration* newConfig);

    virtual CARAPI_(AutoPtr<IContextMenuInfo>) GetContextMenuInfo();

    virtual CARAPI OnCreateContextMenu(
        /* [in] */ IContextMenu* menu);

    virtual CARAPI OnScrollChanged(
        /* [in] */ Int32 l,
        /* [in] */ Int32 t,
        /* [in] */ Int32 oldl,
        /* [in] */ Int32 oldt);

    virtual CARAPI_(void) SetFlags(
        /* [in] */ Int32 flags,
        /* [in] */ Int32 mask);

    virtual CARAPI OnSizeChanged(
        /* [in] */ Int32 w,
        /* [in] */ Int32 h,
        /* [in] */ Int32 oldw,
        /* [in] */ Int32 oldh);

    virtual CARAPI DispatchDraw(
        /* [in] */ ICanvas* canvas);

    virtual CARAPI_(Boolean) AwakenScrollBars();

    virtual CARAPI_(Boolean) AwakenScrollBars(
        /* [in] */ Int32 startDelay);

    virtual CARAPI_(Boolean) AwakenScrollBars(
        /* [in] */ Int32 startDelay,
        /* [in] */ Boolean invalidate);

    virtual CARAPI_(Boolean) HasOpaqueScrollbars();

    virtual CARAPI_(Float) GetTopFadingEdgeStrength();

    virtual CARAPI_(Float) GetBottomFadingEdgeStrength();

    virtual CARAPI_(Float) GetLeftFadingEdgeStrength();

    virtual CARAPI_(Float) GetRightFadingEdgeStrength();

    virtual CARAPI_(Int32) ComputeHorizontalScrollRange();

    virtual CARAPI_(Int32) ComputeHorizontalScrollOffset();

    virtual CARAPI_(Int32) ComputeHorizontalScrollExtent();

    virtual CARAPI_(Int32) ComputeVerticalScrollRange();

    virtual CARAPI_(Int32) ComputeVerticalScrollOffset();

    virtual CARAPI_(Int32) ComputeVerticalScrollExtent();

    virtual CARAPI_(void) OnDrawScrollBars(
        /* [in] */ ICanvas* canvas);

    virtual CARAPI_(Boolean) IsVerticalScrollBarHidden();

    virtual CARAPI_(void) OnDrawHorizontalScrollBar(
        /* [in] */ ICanvas* canvas,
        /* [in] */ IDrawable* scrollBar,
        /* [in] */ Int32 left,
        /* [in] */ Int32 top,
        /* [in] */ Int32 right,
        /* [in] */ Int32 bottom);

    virtual CARAPI_(void) OnDrawVerticalScrollBar(
        /* [in] */ ICanvas* canvas,
        /* [in] */ IDrawable* scrollBar,
        /* [in] */ Int32 left,
        /* [in] */ Int32 top,
        /* [in] */ Int32 right,
        /* [in] */ Int32 bottom);

    virtual CARAPI_(void) OnDraw(
        /* [in] */ ICanvas* canvas);

    virtual CARAPI OnAttachedToWindow();

    virtual CARAPI OnDetachedFromWindow();

    virtual CARAPI OnDetachedFromWindowInternal();

    virtual CARAPI_(Int32) GetWindowAttachCount();

    virtual CARAPI DispatchSaveInstanceState(
        /* [in] */ ISparseArray* container);

    virtual CARAPI_(AutoPtr<IParcelable>) OnSaveInstanceState();

    virtual CARAPI DispatchRestoreInstanceState(
        /* [in] */ ISparseArray* container);

    virtual CARAPI OnRestoreInstanceState(
        /* [in] */ IParcelable* state);

    virtual CARAPI_(Boolean) IsPaddingOffsetRequired();

    virtual CARAPI_(Int32) GetLeftPaddingOffset();

    virtual CARAPI_(Int32) GetRightPaddingOffset();

    virtual CARAPI_(Int32) GetTopPaddingOffset();

    virtual CARAPI_(Int32) GetBottomPaddingOffset();

    virtual CARAPI OnLayout(
        /* [in] */ Boolean changed,
        /* [in] */ Int32 left,
        /* [in] */ Int32 top,
        /* [in] */ Int32 right,
        /* [in] */ Int32 bottom);

    virtual CARAPI_(Boolean) SetFrame(
        /* [in] */ Int32 left,
        /* [in] */ Int32 top,
        /* [in] */ Int32 right,
        /* [in] */ Int32 bottom);

    virtual CARAPI OnFinishInflate();

    virtual CARAPI_(Boolean) VerifyDrawable(
        /* [in] */ IDrawable* who);

    virtual CARAPI DrawableStateChanged();

    virtual CARAPI OnCreateDrawableState(
        /* [in] */ Int32 extraSpace,
        /* [out, callee] */ ArrayOf<Int32>** drawableState);

    static CARAPI_(AutoPtr<ArrayOf<Int32> >) MergeDrawableStates(
        /* [in] */ ArrayOf<Int32>* baseState,
        /* [in] */ ArrayOf<Int32>* additionalState);

    virtual CARAPI DispatchSetSelected(
        /* [in] */ Boolean selected);

    virtual CARAPI_(AutoPtr<IView>) FindViewTraversal(
        /* [in] */ Int32 id);

    virtual CARAPI_(AutoPtr<IView>) FindViewWithTagTraversal(
        /* [in] */ IInterface* tag);

    virtual CARAPI_(Boolean) Debug(
       /* [in] */ Int32 depth);

    static CARAPI_(String) DebugIndent(
        /* [in] */ Int32 depth);

    virtual CARAPI OnMeasure(
        /* [in] */ Int32 widthMeasureSpec,
        /* [in] */ Int32 heightMeasureSpec);

    CARAPI_(void) SetMeasuredDimension(
        /* [in] */ Int32 measuredWidth,
        /* [in] */ Int32 measuredHeight);

    virtual CARAPI_(Int32) GetSuggestedMinimumHeight();

    virtual CARAPI_(Int32) GetSuggestedMinimumWidth();

    virtual CARAPI OnAnimationStart();

    virtual CARAPI OnAnimationEnd();

    virtual CARAPI_(Boolean) OnSetAlpha(
        /* [in] */ Int32 alpha);

    virtual CARAPI_(Boolean) OverScrollBy(
        /* [in] */ Int32 deltaX,
        /* [in] */ Int32 deltaY,
        /* [in] */ Int32 scrollX,
        /* [in] */ Int32 scrollY,
        /* [in] */ Int32 scrollRangeX,
        /* [in] */ Int32 scrollRangeY,
        /* [in] */ Int32 maxOverScrollX,
        /* [in] */ Int32 maxOverScrollY,
        /* [in] */ Boolean isTouchEvent);

    virtual CARAPI OnOverScrolled(
        /* [in] */ Int32 scrollX,
        /* [in] */ Int32 scrollY,
        /* [in] */ Boolean clampedX,
        /* [in] */ Boolean clampedY);

    CARAPI_(AutoPtr<ListenerInfo>) GetListenerInfo();

    virtual CARAPI_(Boolean) PerformButtonActionOnTouchDown(
        /* [in] */ IMotionEvent* event);

    /**
     * @see #sendAccessibilityEvent(int)
     *
     * Note: Called from the default {@link AccessibilityDelegate}.
     */
    virtual CARAPI_(void) SendAccessibilityEventInternal(
        /* [in] */ Int32 eventType);

    /**
     * @see #sendAccessibilityEventUnchecked(AccessibilityEvent)
     *
     * Note: Called from the default {@link AccessibilityDelegate}.
     */
    virtual CARAPI_(void) SendAccessibilityEventUncheckedInternal(
        /* [in] */ IAccessibilityEvent* event);

    /**
     * @see #dispatchPopulateAccessibilityEvent(AccessibilityEvent)
     *
     * Note: Called from the default {@link AccessibilityDelegate}.
     */
    virtual CARAPI_(Boolean) DispatchPopulateAccessibilityEventInternal(
        /* [in] */ IAccessibilityEvent* event);

    /**
     * @see #onPopulateAccessibilityEvent(AccessibilityEvent)
     *
     * Note: Called from the default {@link AccessibilityDelegate}.
     */
    virtual CARAPI OnPopulateAccessibilityEventInternal(
        /* [in] */ IAccessibilityEvent* event);

    /**
     * @see #onInitializeAccessibilityEvent(AccessibilityEvent)
     *
     * Note: Called from the default {@link AccessibilityDelegate}.
     */
    virtual CARAPI OnInitializeAccessibilityEventInternal(
        /* [in] */ IAccessibilityEvent* event);

    /**
     * Gets the location of this view in screen coordintates.
     *
     * @param outRect The output location
     */
    virtual CARAPI GetBoundsOnScreen(
        /* [in] */ IRect* outRect);

    /**
     * @see #onInitializeAccessibilityNodeInfo(AccessibilityNodeInfo)
     *
     * Note: Called from the default {@link AccessibilityDelegate}.
     */
    virtual CARAPI OnInitializeAccessibilityNodeInfoInternal(
        /* [in] */ IAccessibilityNodeInfo* info);

    /**
     * Computes whether this view is visible to the user. Such a view is
     * attached, visible, all its predecessors are visible, it is not clipped
     * entirely by its predecessors, and has an alpha greater than zero.
     *
     * @return Whether the view is visible on the screen.
     *
     * @hide
     */
    virtual CARAPI_(Boolean) IsVisibleToUser();

    /**
     * Computes whether the given portion of this view is visible to the user.
     * Such a view is attached, visible, all its predecessors are visible,
     * has an alpha greater than zero, and the specified portion is not
     * clipped entirely by its predecessors.
     *
     * @param boundInView the portion of the view to test; coordinates should be relative; may be
     *                    <code>null</code>, and the entire view will be tested in this case.
     *                    When <code>true</code> is returned by the function, the actual visible
     *                    region will be stored in this parameter; that is, if boundInView is fully
     *                    contained within the view, no modification will be made, otherwise regions
     *                    outside of the visible area of the view will be clipped.
     *
     * @return Whether the specified portion of the view is visible on the screen.
     *
     * @hide
     */
    virtual CARAPI_(Boolean) IsVisibleToUser(
        /* [in] */ IRect* boundInView);

    /**
     * Clears accessibility focus without calling any callback methods
     * normally invoked in {@link #clearAccessibilityFocus()}. This method
     * is used for clearing accessibility focus when giving this focus to
     * another view.
     */
    virtual CARAPI_(void) ClearAccessibilityFocusNoCallbacks();

    /**
    * @see #performAccessibilityAction(int, Bundle)
    *
    * Note: Called from the default {@link AccessibilityDelegate}.
    */
    virtual CARAPI_(Boolean) PerformAccessibilityActionInternal(
        /* [in] */ Int32 action,
        /* [in] */ IBundle* arguments);

    /**
     * Dispatch a hover event.
     * <p>
     * Do not call this method directly.
     * Call {@link #dispatchGenericMotionEvent(MotionEvent)} instead.
     * </p>
     *
     * @param event The motion event to be dispatched.
     * @return True if the event was handled by the view, false otherwise.
     */
    virtual CARAPI_(Boolean) DispatchHoverEvent(
        /* [in] */ IMotionEvent* event);

    /**
     * Returns true if the view has a child to which it has recently sent
     * {@link MotionEvent#ACTION_HOVER_ENTER}.  If this view is hovered and
     * it does not have a hovered child, then it must be the innermost hovered view.
     * @hide
     */
    virtual CARAPI_(Boolean) HasHoveredChild();

    /**
     * Dispatch a generic motion event to the view under the first pointer.
     * <p>
     * Do not call this method directly.
     * Call {@link #dispatchGenericMotionEvent(MotionEvent)} instead.
     * </p>
     *
     * @param event The motion event to be dispatched.
     * @return True if the event was handled by the view, false otherwise.
     */
    virtual CARAPI_(Boolean) DispatchGenericPointerEvent(
        /* [in] */ IMotionEvent* event);

    /**
     * Dispatch a generic motion event to the currently focused view.
     * <p>
     * Do not call this method directly.
     * Call {@link #dispatchGenericMotionEvent(MotionEvent)} instead.
     * </p>
     *
     * @param event The motion event to be dispatched.
     * @return True if the event was handled by the view, false otherwise.
     */
    virtual CARAPI_(Boolean) DispatchGenericFocusedEvent(
        /* [in] */ IMotionEvent* event);

    /**
     * Returns true if the transform matrix is the identity matrix.
     * Recomputes the matrix if necessary.
     *
     * @return True if the transform matrix is the identity matrix, false otherwise.
     */
    virtual CARAPI_(Boolean) HasIdentityMatrix();

    virtual CARAPI_(void) EnsureTransformationInfo();

    /**
     * Utility method to retrieve the inverse of the current mMatrix property.
     * We cache the matrix to avoid recalculating it when transform properties
     * have not changed.
     *
     * @return The inverse of the current matrix of this view.
     */
    virtual CARAPI GetInverseMatrix(
        /* [out] */ IMatrix** res);

    /**
     * Faster version of setAlpha() which performs the same steps except there are
     * no calls to invalidate(). The caller of this function should perform proper invalidation
     * on the parent and this object. The return value indicates whether the subclass handles
     * alpha (the return value for onSetAlpha()).
     *
     * @param alpha The new value for the alpha property
     * @return true if the View subclass handles alpha (the return value for onSetAlpha()) and
     *         the new value for the alpha property is different from the old value
     */
    virtual CARAPI_(Boolean) SetAlphaNoInvalidation(
        /* [in] */ Float alpha);

    /**
     * Determines whether the given point, in local coordinates is inside the view.
     */
    /*package*/ virtual CARAPI_(Boolean) PointInView(
        /* [in] */ Float localX,
        /* [in] */ Float localY);

    /**
     * This is where the invalidate() work actually happens. A full invalidate()
     * causes the drawing cache to be invalidated, but this function can be called with
     * invalidateCache set to false to skip that invalidation step for cases that do not
     * need it (for example, a component that remains at the same dimensions with the same
     * content).
     *
     * @param invalidateCache Whether the drawing cache for this view should be invalidated as
     * well. This is usually true for a full invalidate, but may be set to false if the
     * View's contents or dimensions have not changed.
     */
    virtual CARAPI_(void) Invalidate(
        /* [in] */ Boolean invalidateCache);

    /**
     * Quick invalidation for View property changes (alpha, translationXY, etc.). We don't want to
     * set any flags or handle all of the cases handled by the default invalidation methods.
     * Instead, we just want to schedule a traversal in ViewRootImpl with the appropriate
     * dirty rect. This method calls into fast invalidation methods in ViewGroup that
     * walk up the hierarchy, transforming the dirty rect as necessary.
     *
     * The method also handles normal invalidation logic if display list properties are not
     * being used in this view. The invalidateParent and forceRedraw flags are used by that
     * backup approach, to handle these cases used in the various property-setting methods.
     *
     * @param invalidateParent Force a call to invalidateParentCaches() if display list properties
     * are not being used in this view
     * @param forceRedraw Mark the view as DRAWN to force the invalidation to propagate, if display
     * list properties are not being used in this view
     */
    virtual CARAPI_(void) InvalidateViewProperty(
        /* [in] */ Boolean invalidateParent,
        /* [in] */ Boolean forceRedraw);

    /**
     * Utility method to transform a given Rect by the current matrix of this view.
     */
    virtual CARAPI_(void) TransformRect(
        /* [in] */ IRect* rect);

    /**
     * Used to indicate that the parent of this view should clear its caches. This functionality
     * is used to force the parent to rebuild its display list (when hardware-accelerated),
     * which is necessary when various parent-managed properties of the view change, such as
     * alpha, translationX/Y, scrollX/Y, scaleX/Y, and rotation/X/Y. This method only
     * clears the parent caches and does not causes an invalidate event.
     *
     * @hide
     */
    virtual CARAPI_(void) InvalidateParentCaches();

    /**
     * Used to indicate that the parent of this view should be invalidated. This functionality
     * is used to force the parent to rebuild its display list (when hardware-accelerated),
     * which is necessary when various parent-managed properties of the view change, such as
     * alpha, translationX/Y, scrollX/Y, scaleX/Y, and rotation/X/Y. This method will propagate
     * an invalidation event to the parent.
     *
     * @hide
     */
    virtual CARAPI_(void) InvalidateParentIfNeeded();

    virtual CARAPI_(void) ComputeOpaqueFlags();

    virtual CARAPI_(void) RecomputePadding();

    /**
     * @see #onScreenStateChanged(int)
     */
    virtual CARAPI_(void) DispatchScreenStateChanged(
        /* [in] */ Int32 screenState);


    /**
     * Return if padding has been resolved
     *
     * @hide
     */
    virtual CARAPI IsPaddingResolved(
        /* [out] */ Boolean* res);

    /**
     * Indicates whether this view has a static layer. A view with layer type
     * {@link #LAYER_TYPE_NONE} is a static layer. Other types of layers are
     * dynamic.
     */
    virtual CARAPI_(Boolean) HasStaticLayer();


    /**
     * <p>Returns a hardware layer that can be used to draw this view again
     * without executing its draw method.</p>
     *
     * @return A HardwareLayer ready to render, or null if an error occurred.
     */
    virtual CARAPI_(AutoPtr<IHardwareLayer>) GetHardwareLayer();

    /**
     * Destroys all hardware rendering resources. This method is invoked
     * when the system needs to reclaim resources. Upon execution of this
     * method, you should free any OpenGL resources created by the view.
     *
     * Note: you <strong>must</strong> call
     * <code>super.destroyHardwareResources()</code> when overriding
     * this method.
     *
     * @hide
     */
    virtual CARAPI_(void) DestroyHardwareResources();

    /**
     * This method is used by ViewGroup to cause its children to restore or recreate their
     * display lists. It is called by getDisplayList() when the parent ViewGroup does not need
     * to recreate its own display list, which would happen if it went through the normal
     * draw/dispatchDraw mechanisms.
     *
     * @hide
     */
    virtual CARAPI_(void) DispatchGetDisplayList();

    /**
     * @hide
     * @param offsetRequired
     */
    virtual CARAPI_(Int32) GetFadeTop(
        /* [in] */ Boolean offsetRequired);

    /**
     * @hide
     * @param offsetRequired
     */
    virtual CARAPI_(Int32) GetFadeHeight(
        /* [in] */ Boolean offsetRequired);

    /**
     * This method is called by getDisplayList() when a display list is created or re-rendered.
     * It sets or resets the current value of all properties on that display list (resetting is
     * necessary when a display list is being re-created, because we need to make sure that
     * previously-set transform values
     */
    virtual CARAPI_(void) SetDisplayListProperties(
        /* [in] */ IRenderNode* renderNode);

    /**
     * This method is called by ViewGroup.drawChild() to have each child view draw itself.
     * This draw() method is an implementation detail and is not intended to be overridden or
     * to be called from anywhere else other than ViewGroup.drawChild().
     */
    virtual CARAPI_(Boolean) Draw(
        /* [in] */ ICanvas* canvas,
        /* [in] */ IViewGroup* parent,
        /* [in] */ Int64 drawingTime);

    /**
     * Resolve the Drawables depending on the layout direction. This is implicitly supposing
     * that the View directionality can and will be resolved before its Drawables.
     *
     * Will call {@link View#onResolveDrawables} when resolution is done.
     *
     * @hide
     */
    virtual CARAPI_(void) ResolveDrawables();

    /**
     * @hide
     */
    virtual CARAPI_(void) ResetResolvedDrawables();

    /**
     * @hide
     */
    virtual CARAPI_(void) InternalSetPadding(
        /* [in] */ Int32 left,
        /* [in] */ Int32 top,
        /* [in] */ Int32 right,
        /* [in] */ Int32 bottom);

    /**
     * Dispatch setActivated to all of this View's children.
     *
     * @see #setActivated(boolean)
     *
     * @param activated The new activated state
     */
    virtual CARAPI DispatchSetActivated(
        /* [in] */ Boolean activated);

    /**
     * {@hide}
     * @param predicate The predicate to evaluate.
     * @param childToSkip If not null, ignores this child during the recursive traversal.
     * @return The first view that matches the predicate or null.
     */
    virtual CARAPI_(AutoPtr<IView>) FindViewByPredicateTraversal(
        /* [in] */ IPredicate* predicate,
        /* [in] */ IView* childToSkip);

    virtual CARAPI_(Boolean) UpdateLocalSystemUiVisibility(
        /* [in] */ Int32 localValue,
        /* [in] */ Int32 localChanges);

    virtual CARAPI_(Boolean) CanAcceptDrag();

    /**
     * Gets a scale factor that determines the distance the view should scroll
     * vertically in response to {@link MotionEvent#ACTION_SCROLL}.
     * @return The vertical scroll scale factor.
     * @hide
     */
    virtual CARAPI GetVerticalScrollFactor(
        /* [out] */ Float* scroll);

    /**
     * Gets a scale factor that determines the distance the view should scroll
     * horizontally in response to {@link MotionEvent#ACTION_SCROLL}.
     * @return The horizontal scroll scale factor.
     * @hide
     */
    virtual CARAPI GetHorizontalScrollFactor(
        /* [out] */ Float* scroll);

    virtual CARAPI_(Boolean) ComputeFitSystemWindows(
        /* [in] */ IRect* inoutInsets,
        /* [in] */ IRect* outLocalInsets);

    virtual CARAPI_(void) InvalidateInternal(
        /* [in] */ Int32 l,
        /* [in] */ Int32 t,
        /* [in] */ Int32 r,
        /* [in] */ Int32 b,
        /* [in] */ Boolean invalidateCache,
        /* [in] */ Boolean fullInvalidate);

    virtual CARAPI_(void) InvalidateParentIfNeededAndWasQuickRejected();

private:
    CARAPI_(void) InitScrollCache();

    CARAPI_(AutoPtr<ScrollabilityCache>) GetScrollCache();

    CARAPI_(void) ResetPressedState();

    static CARAPI_(AutoPtr<IView>) FindViewShouldExist(
        /* [in] */ IView* root,
        /* [in] */ Int32 childViewId);

    CARAPI_(Boolean) HasAncestorThatBlocksDescendantFocus();

    static CARAPI_(void) CaptureViewInfo(
        /* [in] */ const char* subTag,
        /* [in] */ IView* v);

    CARAPI_(void) RemoveLongPressCallback();

    CARAPI_(void) RemoveUnsetPressCallback();

    CARAPI_(void) RemoveTapCallback();

    CARAPI_(Boolean) InitialAwakenScrollBars();

    static CARAPI_(String) PrintFlags(
        /* [in] */ Int32 flags);

    static CARAPI_(String) PrintPrivateFlags(
        /* [in] */ Int32 privateFlags);

    static CARAPI_(void) SetTagInternal(
        /* [in] */ IView* view,
        /* [in] */ Int32 key,
        /* [in] */ IObject* tag);

    CARAPI_(void) CheckForLongClick(
        /* [in] */ Int32 delayOffset);

    CARAPI HandleInvalidate();

    CARAPI HandleInvalidateRect(
        /* [in] */ Int32 left,
        /* [in] */ Int32 top,
        /* [in] */ Int32 right,
        /* [in] */ Int32 bottom);

    CARAPI_(AutoPtr<IView>) FindLabelForView(
        /* [in] */ IView* view,
        /* [in] */ Int32 labeledId);

    CARAPI_(AutoPtr<IView>) FindViewInsideOutShouldExist(
        /* [in] */ IView* root,
        /* [in] */ Int32 id);

    CARAPI_(void)  SendAccessibilityHoverEvent(
        /* [in] */ Int32 eventType);

    CARAPI_(Boolean) RequestFocusNoSearch(
        /* [in] */ Int32 direction,
        /* [in] */IRect* previouslyFocusedRect);

    /**
     * Returns whether the View has registered callbacks wich makes it
     * important for accessibility.
     *
     * @return True if the view is actionable for accessibility.
     */
    CARAPI_(Boolean) HasListenersForAccessibility();

    CARAPI_(Boolean) TraverseAtGranularity(
        /* [in] */ Int32 granularity,
        /* [in] */ Boolean forward,
        /* [in] */ Boolean extendSelection);

    CARAPI_(void) SendViewTextTraversedAtGranularityEvent(
        /* [in] */ Int32 action,
        /* [in] */ Int32 granularity,
        /* [in] */ Int32 fromIndex,
        /* [in] */ Int32 toIndex);

    CARAPI_(Boolean) DispatchGenericMotionEventInternal(
        /* [in] */ IMotionEvent* event);

    /**
     * Returns true if the view should handle {@link #onHoverEvent}
     * by calling {@link #setHovered} to change its hovered state.
     *
     * @return True if the view is hoverable.
     */
    CARAPI_(Boolean) IsHoverable();

    CARAPI_(void) RemovePerformClickCallback();

    /**
     * Remove the pending callback for sending a
     * {@link AccessibilityEvent#TYPE_VIEW_SCROLLED} accessibility event.
     */
    CARAPI_(void) RemoveSendViewScrolledAccessibilityEventCallback();

    /**
     * Do not invalidate views which are not visible and which are not running an animation. They
     * will not get drawn and they should not set dirty flags as if they will be drawn
     */
    CARAPI_(Boolean) SkipInvalidate();

    /**
     * Post a callback to send a {@link AccessibilityEvent#TYPE_VIEW_SCROLLED} event.
     * This event is sent at most once every
     * {@link ViewConfiguration#getSendRecurringAccessibilityEventsInterval()}.
     */
    CARAPI_(void) PostSendViewScrolledAccessibilityEventCallback();

    /**
     * Return true if the application tag in the AndroidManifest has set "supportRtl" to true
     */
    CARAPI_(Boolean)  HasRtlSupport();

    /**
     * Return true if we are in RTL compatibility mode (either before Jelly Bean MR1 or
     * RTL not supported)
     */
    CARAPI_(Boolean)  IsRtlCompatibilityMode();

    /**
     * @return true if RTL properties need resolution.
     */
    CARAPI_(Boolean)  NeedRtlPropertiesResolution();

    CARAPI_(void) ResetDisplayList();

    /**
     * Utility function, called by draw(canvas, parent, drawingTime) to handle the less common
     * case of an active Animation being run on the view.
     */
    CARAPI_(Boolean) DrawAnimation(
        /* [in] */ IViewGroup* parent,
        /* [in] */ Int64 drawingTime,
        /* [in] */ IAnimation* a,
        /* [in] */ Boolean scalingRequired);

    CARAPI_(Boolean) IsDrawablesResolved();

    CARAPI_(void) SetKeyedTag(
        /* [in] */ Int32 key,
        /* [in] */ IInterface* tag);

    static CARAPI_(AutoPtr<ISparseArray>) GetAttributeMap(); /*SparseArray<String>*/

    CARAPI_(void) SaveAttributeData(
        /* [in] */ IAttributeSet* attrs,
        /* [in] */ ITypedArray* a);

    CARAPI FitSystemWindowsInt(
        /* [in] */ IRect* insets,
        /* [in] */ Boolean* res);

    CARAPI_(void) SetPressed(
        /* [in] */ Boolean pressed,
        /* [in] */ Float x,
        /* [in] */ Float y);

    CARAPI_(Float) GetFinalAlpha();

    CARAPI_(void) SetOutlineProviderFromAttribute(
        /* [in] */ Int32 providerInt);

    CARAPI RebuildOutline();

    CARAPI_(AutoPtr<IView>) GetProjectionReceiver();

    CARAPI_(Boolean) IsProjectionReceiver();

    CARAPI_(void) DamageShadowReceiver();

    CARAPI_(void) CleanupDraw();

    CARAPI_(void) InvalidateInheritedLayoutMode(
        /* [in] */ Int32 layoutModeOfRoot);

    CARAPI DispatchCancelPendingInputEvents();

    CARAPI UpdateDisplayListIfDirty();

    CARAPI_(void) DrawAccessibilityFocus(
        /* [in] */ ICanvas* canvas);

    CARAPI_(void) DrawBackground(
        /* [in] */ ICanvas* canvas);

    CARAPI_(void) SetBackgroundDisplayListProperties(
        /* [in] */ IRenderNode* displayList);

    CARAPI_(AutoPtr<IRenderNode>) GetDrawableRenderNode(
        /* [in] */ IDrawable* drawable,
        /* [in] */ IRenderNode* renderNode);

    CARAPI_(Boolean) SetOpticalFrame(
        /* [in] */ Int32 left,
        /* [in] */ Int32 top,
        /* [in] */ Int32 right,
        /* [in] */ Int32 bottom);

    CARAPI SizeChange(
        /* [in] */ Int32 newWidth,
        /* [in] */ Int32 newHeight,
        /* [in] */ Int32 oldWidth,
        /* [in] */ Int32 oldHeight);

    CARAPI ApplyBackgroundTint();

    CARAPI SetMeasuredDimensionRaw(
        /* [in] */ Int32 measuredWidth,
        /* [in] */ Int32 measuredHeight);

    CARAPI_(Boolean) InLiveRegion();

public:
    /**
     * Set to true when drawing cache is enabled and cannot be created.
     *
     * @hide
     */
    Boolean mCachingFailed;

    static Boolean mDebugViewAttributes;

    AutoPtr<AttachInfo> mAttachInfo;

    AutoPtr< ArrayOf<String> > mAttributes;

protected:
    /**
     * The animation currently associated with this view.
     * @hide
     */
    AutoPtr<IAnimation> mCurrentAnimation;

    /**
     * Width as measured during measure pass.
     * {@hide}
     */
    Int32 mMeasuredWidth;

    /**
     * Height as measured during measure pass.
     * {@hide}
     */
    Int32 mMeasuredHeight;

    /**
     * Flag to indicate that this view was marked INVALIDATED, or had its display list
     * invalidated, prior to the current drawing iteration. If true, the view must re-draw
     * its display list. This flag, used only when hw accelerated, allows us to clear the
     * flag while retaining this information until it's needed (at getDisplayList() time and
     * in drawChild(), when we decide to draw a view's children's display lists into our own).
     *
     * {@hide}
     */
    Boolean mRecreateDisplayList;

    /**
     * The view's identifier.
     * {@hide}
     *
     * @see #setId(int)
     * @see #getId()
     */
    Int32 mID;

    /**
     * The stable ID of this view for accessibility purposes.
     */
    Int32 mAccessibilityViewId;

    /**
     * @hide
     */
    Int32 mAccessibilityCursorPosition;

    /**
     * The view's tag.
     * {@hide}
     *
     * @see #setTag(Object)
     * @see #getTag()
     */
    AutoPtr<IInterface> mTag;

    /**
     * Controls the over-scroll mode for this view.
     * See {@link #overScrollBy(int, int, int, int, int, int, int, int, boolean)},
     * {@link #OVER_SCROLL_ALWAYS}, {@link #OVER_SCROLL_IF_CONTENT_SCROLLS},
     * and {@link #OVER_SCROLL_NEVER}.
     */
    Int32 mOverScrollMode;
    /**
     * The parent this view is attached to.
     * {@hide}
     *
     * @see #getParent()
     */
    // Parent has child's reference
    IViewParent* mParent;

    Int32 mPrivateFlags;
    Int32 mPrivateFlags2;
    Int32 mPrivateFlags3;

    /**
     * This view's request for the visibility of the status bar.
     * @hide
     */
    // @ViewDebug.ExportedProperty(flagMapping = {
    //     @ViewDebug.FlagToString(mask = SYSTEM_UI_FLAG_LOW_PROFILE,
    //                             equals = SYSTEM_UI_FLAG_LOW_PROFILE,
    //                             name = "SYSTEM_UI_FLAG_LOW_PROFILE", outputIf = true),
    //     @ViewDebug.FlagToString(mask = SYSTEM_UI_FLAG_HIDE_NAVIGATION,
    //                             equals = SYSTEM_UI_FLAG_HIDE_NAVIGATION,
    //                             name = "SYSTEM_UI_FLAG_HIDE_NAVIGATION", outputIf = true),
    //     @ViewDebug.FlagToString(mask = PUBLIC_STATUS_BAR_VISIBILITY_MASK,
    //                             equals = SYSTEM_UI_FLAG_VISIBLE,
    //                             name = "SYSTEM_UI_FLAG_VISIBLE", outputIf = true)
    // })
    Int32 mSystemUiVisibility;

    /**
     * Reference count for transient state.
     * @see #setHasTransientState(boolean)
     */
    Int32 mTransientStateCount;

    /**
     * Count of how many windows this view has been attached to.
     */
    Int32 mWindowAttachCount;

    /**
     * The layout parameters associated with this view and used by the parent
     * {@link android.view.ViewGroup} to determine how this view should be
     * laid out.
     * {@hide}
     */
    AutoPtr<IViewGroupLayoutParams> mLayoutParams;

    /**
     * The view flags hold various views states.
     * {@hide}
     */
    Int32 mViewFlags;

    AutoPtr<TransformationInfo> mTransformationInfo;

    Boolean mLastIsOpaque;

    AutoPtr<IRect> mClipBounds;

    /**
     * The distance in pixels from the left edge of this view's parent
     * to the left edge of this view.
     * {@hide}
     */
    Int32 mLeft;
    /**
     * The distance in pixels from the left edge of this view's parent
     * to the right edge of this view.
     * {@hide}
     */
    Int32 mRight;
    /**
     * The distance in pixels from the top edge of this view's parent
     * to the top edge of this view.
     * {@hide}
     */
    Int32 mTop;
    /**
     * The distance in pixels from the top edge of this view's parent
     * to the bottom edge of this view.
     * {@hide}
     */
    Int32 mBottom;

    /**
     * The offset, in pixels, by which the content of this view is scrolled
     * horizontally.
     * {@hide}
     */
    Int32 mScrollX;
    /**
     * The offset, in pixels, by which the content of this view is scrolled
     * vertically.
     * {@hide}
     */
    Int32 mScrollY;

    /**
     * The left padding in pixels, that is the distance in pixels between the
     * left edge of this view and the left edge of its content.
     * {@hide}
     */
    Int32 mPaddingLeft;
    /**
     * The right padding in pixels, that is the distance in pixels between the
     * right edge of this view and the right edge of its content.
     * {@hide}
     */
    Int32 mPaddingRight;
    /**
     * The top padding in pixels, that is the distance in pixels between the
     * top edge of this view and the top edge of its content.
     * {@hide}
     */
    Int32 mPaddingTop;
    /**
     * The bottom padding in pixels, that is the distance in pixels between the
     * bottom edge of this view and the bottom edge of its content.
     * {@hide}
     */
    Int32 mPaddingBottom;

    /**
     * Cache the paddingRight set by the user to append to the scrollbar's size.
     */
    Int32 mUserPaddingRight;

    /**
     * Cache the paddingBottom set by the user to append to the scrollbar's size.
     */
    Int32 mUserPaddingBottom;

    /**
     * Cache the paddingLeft set by the user to append to the scrollbar's size.
     *
     * @hide
     */
    //@ViewDebug.ExportedProperty(category = "padding")
    Int32 mUserPaddingLeft;

    /**
     * Cache the paddingStart set by the user to append to the scrollbar's size.
     *
     */
    //@ViewDebug.ExportedProperty(category = "padding")
    Int32 mUserPaddingStart;

    /**
     * Cache the paddingEnd set by the user to append to the scrollbar's size.
     *
     */
    //@ViewDebug.ExportedProperty(category = "padding")
    Int32 mUserPaddingEnd;

    /**
     * Cache initial left padding.
     *
     * @hide
     */
    Int32 mUserPaddingLeftInitial;

    /**
     * Cache initial right padding.
     *
     * @hide
     */
    Int32 mUserPaddingRightInitial;

    Int32 mOldWidthMeasureSpec;
    /**
     * @hide
     */
    Int32 mOldHeightMeasureSpec;

    /**
     * The layout insets in pixels, that is the distance in pixels between the
     * visible edges of this view its bounds.
     */
    AutoPtr<IInsets> mLayoutInsets;

    /**
     * Briefly describes the view and is primarily used for accessibility support.
     */
    AutoPtr<ICharSequence> mContentDescription;

    /**
     * Specifies the id of a view for which this view serves as a label for
     * accessibility purposes.
     */
    Int32 mLabelForId;

    /**
     * Predicate for matching labeled view id with its label for
     * accessibility purposes.
     */
    AutoPtr<MatchLabelForPredicate> mMatchLabelForPredicate;

    /**
     * Predicate for matching a view by its id.
     */
    AutoPtr<MatchIdPredicate> mMatchIdPredicate;

    AutoPtr<IResources> mResources;

    AutoPtr<IDrawable> mBGDrawable;

    AutoPtr<IDrawable> mBackground;
    Int32 mBackgroundResource;
    Boolean mBackgroundSizeChanged;

    AutoPtr<ListenerInfo> mListenerInfo;

    /**
     * The application environment this view lives in.
     * This field should be made private, so it is hidden from the SDK.
     * {@hide}
     */
    IContext* mContext;

    AutoPtr<ScrollabilityCache> mScrollCache;

    AutoPtr<ArrayOf<Int32> > mDrawableState;

    AutoPtr<IViewOutlineProvider> mOutlineProvider;

    AutoPtr<IStateListAnimator> mStateListAnimator;

    AutoPtr<IBitmap> mDrawingCache;
    AutoPtr<IBitmap> mUnscaledDrawingCache;

    /**
     * When this view has focus and the next focus is {@link #FOCUS_LEFT},
     * the user may specify which view to go to next.
     */
    Int32 mNextFocusLeftId;// = View.NO_ID;

    /**
     * When this view has focus and the next focus is {@link #FOCUS_RIGHT},
     * the user may specify which view to go to next.
     */
    Int32 mNextFocusRightId;// = View.NO_ID;

    /**
     * When this view has focus and the next focus is {@link #FOCUS_UP},
     * the user may specify which view to go to next.
     */
    Int32 mNextFocusUpId;// = View.NO_ID;

    /**
     * When this view has focus and the next focus is {@link #FOCUS_DOWN},
     * the user may specify which view to go to next.
     */
    Int32 mNextFocusDownId;// = View.NO_ID;

    /**
     * When this view has focus and the next focus is {@link #FOCUS_FORWARD},
     * the user may specify which view to go to next.
     */
    Int32 mNextFocusForwardId;// = View.NO_ID;

    AutoPtr<CheckForLongPress> mPendingCheckForLongPress;
    AutoPtr<CheckForTap> mPendingCheckForTap;
    AutoPtr<ViewPerformClick> mPerformClick;
    AutoPtr<SendViewScrolledAccessibilityEvent> mSendViewScrolledAccessibilityEvent;

    AutoPtr<UnsetPressedState> mUnsetPressedState;

    /**
     * Whether the long press's action has been invoked.  The tap's action is invoked on the
     * up event while a long press is invoked as soon as the long press duration is reached, so
     * a long press could be performed before the tap is checked, in which case the tap's action
     * should not be invoked.
     */
    Boolean mHasPerformedLongPress;

    /**
     * The minimum height of the view. We'll try our best to have the height
     * of this view to at least this amount.
     */
    //@ViewDebug.ExportedProperty(category = "measurement")
    Int32 mMinHeight;

    /**
     * The minimum width of the view. We'll try our best to have the width
     * of this view to at least this amount.
     */
    //@ViewDebug.ExportedProperty(category = "measurement")
    Int32 mMinWidth;

    /**
     * The delegate to handle touch events that are physically in this view
     * but should be handled by another view.
     */
    AutoPtr<ITouchDelegate> mTouchDelegate;

    /**
     * Solid color to use as a background when creating the drawing cache. Enables
     * the cache to use 16 bit bitmaps instead of 32 bit.
     */
    Int32 mDrawingCacheBackgroundColor;

    /**
     * Special tree observer used when mAttachInfo is null.
     */
    AutoPtr<IViewTreeObserver> mFloatingTreeObserver;

    /**
     * Cache the touch slop from the context that created the view.
     */
    Int32 mTouchSlop;

    /**
     * Object that handles automatic animation of view properties.
     */
    AutoPtr<IViewPropertyAnimator> mAnimator;

    /**
     * Vertical scroll factor cached by {@link #getVerticalScrollFactor}.
     */
    Float mVerticalScrollFactor;

    /**
     * Position of the vertical scroll bar.
     */
    Int32 mVerticalScrollbarPosition;

    // @ViewDebug.ExportedProperty(category = "drawing", mapping = {
    //         @ViewDebug.IntToString(from = LAYER_TYPE_NONE, to = "NONE"),
    //         @ViewDebug.IntToString(from = LAYER_TYPE_SOFTWARE, to = "SOFTWARE"),
    //         @ViewDebug.IntToString(from = LAYER_TYPE_HARDWARE, to = "HARDWARE")
    // })
    Int32 mLayerType;
    AutoPtr<IPaint> mLayerPaint;

    AutoPtr<IRenderNode> mRenderNode;

    AutoPtr<IViewOverlay> mOverlay;

    /**
     * An overlay is going to draw this View instead of being drawn as part of this
     * View's parent. mGhostView is the View in the Overlay that must be invalidated
     * when this view is invalidated.
     */
    AutoPtr<IGhostView> mGhostView;

    /**
     * Set to true when the view is sending hover accessibility events because it
     * is the innermost hovered view.
     */
    Boolean mSendingHoverAccessibilityEvents;

    /**
     * Delegate for injecting accessibility functionality.
     */
    AutoPtr<IAccessibilityDelegate> mAccessibilityDelegate;

    /**
     * Consistency verifier for debugging purposes.
     * @hide
     */
    AutoPtr<IInputEventConsistencyVerifier> mInputEventConsistencyVerifier;

    AutoPtr<SendViewStateChangedAccessibilityEvent> mSendViewStateChangedAccessibilityEvent;

private:
    /**
     * The next available accessibility id.
     */
    static Boolean sCompatibilityDone;
    static Boolean sUseBrokenMakeMeasureSpec;
    static Boolean sIgnoreMeasureCache;

    static Int32 sNextAccessibilityViewId;

    static Int32 sNextGeneratedId;
    static Object sNextGeneratedIdLock;

    /**
     * Maps a Resource id to its name.
     */
    static AutoPtr<ISparseArray> sAttributeMap;

    // correspond to the enum values of View_outlineProvider
    static const Int32 PROVIDER_BACKGROUND;
    static const Int32 PROVIDER_NONE;
    static const Int32 PROVIDER_BOUNDS;
    static const Int32 PROVIDER_PADDED_BOUNDS;

public:
    /**
     * Temporary Rect currently for use in setBackground().  This will probably
     * be extended in the future to hold our own class with more than just
     * a Rect. :)
     */
    static pthread_key_t sKey;
    static pthread_once_t sKeyOnce;

private:
    /**
     * Map used to store views' tags.
     */
    AutoPtr<ISparseArray> mKeyedTags;

    /**
     * Cache if a left padding has been defined
     */
    Boolean mLeftPaddingDefined;

    /**
     * Cache if a right padding has been defined
     */
    Boolean mRightPaddingDefined;

    AutoPtr<Int64SparseInt64Array> mMeasureCache;
    AutoPtr<TintInfo> mBackgroundTint;
    AutoPtr<IRenderNode> mBackgroundRenderNode;
    String mTransitionName;
    IViewParent* mNestedScrollingParent;

    AutoPtr< ArrayOf<Int32> > mTempNestedScrollConsumed;

protected:
    Boolean mHoldContext;
};

} // namespace View
} // namespace Droid
} // namespace Elastos


DEFINE_CONVERSION_FOR(Elastos::Droid::View::View::AttachInfo::InvalidateInfo, IInterface)

#endif //__ELASTOS_DROID_VIEW_View_H__
