
#ifndef __ELASTOS_DROID_GRAPHICS_DRAWABLE_DRAWABLE_H__
#define __ELASTOS_DROID_GRAPHICS_DRAWABLE_DRAWABLE_H__

#include "Elastos.Droid.Graphics.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Content::Res::IResourcesTheme;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Content::Res::ITypedArray;
using Elastos::Droid::Content::Res::IColorStateList;
using Elastos::Droid::Graphics::IXfermode;
using Elastos::Droid::Graphics::IColorFilter;
using Elastos::Droid::Utility::ITypedValue;
using Elastos::Droid::Utility::IAttributeSet;
using Elastos::Core::Object;
using Elastos::Core::IRunnable;
using Elastos::IO::IInputStream;
using Org::Xmlpull::V1::IXmlPullParser;

namespace Elastos {
namespace Droid {
namespace Graphics {
namespace Drawable {

class ECO_PUBLIC Drawable
    : public Object
    , public IDrawable
{
public:
    /**
     * This abstract class is used by {@link Drawable}s to store shared constant state and data
     * between Drawables. {@link BitmapDrawable}s created from the same resource will for instance
     * share a unique bitmap stored in their ConstantState.
     *
     * <p>
     * {@link #newDrawable(Resources)} can be used as a factory to create new Drawable instances
     * from this ConstantState.
     * </p>
     *
     * Use {@link Drawable#getConstantState()} to retrieve the ConstantState of a Drawable. Calling
     * {@link Drawable#mutate()} on a Drawable should typically create a new ConstantState for that
     * Drawable.
     */
    class ConstantState
        : public Object
        , public IDrawableConstantState
    {
    public:
        CAR_INTERFACE_DECL();

        virtual ~ConstantState()
        {}

        CARAPI GetWeakReference(
            /* [out] */ IWeakReference** weakReference);

        /**
         * Create a new drawable without supplying resources the caller
         * is running in.  Note that using this means the density-dependent
         * drawables (like bitmaps) will not be able to update their target
         * density correctly. One should use {@link #newDrawable(Resources)}
         * instead to provide a resource.
         */
        virtual CARAPI NewDrawable(
            /* [out] */ IDrawable** drawable) = 0;

        /**
         * Create a new Drawable instance from its constant state.  This
         * must be implemented for drawables that change based on the target
         * density of their caller (that is depending on whether it is
         * in compatibility mode).
         */
        virtual CARAPI NewDrawable(
            /* [in] */ IResources* res,
            /* [out] */ IDrawable** drawable);

        virtual CARAPI NewDrawable(
            /* [in] */ IResources* res,
            /* [in] */ IResourcesTheme* theme,
            /* [out] */ IDrawable** drawable);

        /**
         * Return a bit mask of configuration changes that will impact
         * this drawable (and thus require completely reloading it).
         */
        virtual CARAPI GetChangingConfigurations(
            /* [out] */ Int32* configuration) = 0;

        /**
         * @hide
         */
        virtual CARAPI GetBitmap(
            /* [out] */ IBitmap** bp);

        /**
         * Return whether this constant state can have a theme applied.
         */
        virtual CARAPI CanApplyTheme(
            /* [out] */ Boolean* can);
    };

public:
    CAR_INTERFACE_DECL();

    Drawable();

    virtual ~Drawable();

    /**
     * Draw in its bounds (set via setBounds) respecting optional effects such
     * as alpha (set via setAlpha) and color filter (set via setColorFilter).
     *
     * @param canvas The canvas to draw into
     */
    virtual CARAPI Draw(
        /* [in] */ ICanvas* canvas) = 0;

    /**
     * Specify a bounding rectangle for the Drawable. This is where the drawable
     * will draw when its draw() method is called.
     */
    virtual CARAPI SetBounds(
        /* [in] */ Int32 left,
        /* [in] */ Int32 top,
        /* [in] */ Int32 right,
        /* [in] */ Int32 bottom);

    /**
     * Specify a bounding rectangle for the Drawable. This is where the drawable
     * will draw when its draw() method is called.
     */
    virtual CARAPI SetBounds(
        /* [in] */ IRect* bounds);

    /**
     * Return a copy of the drawable's bounds in the specified Rect (allocated
     * by the caller). The bounds specify where this will draw when its draw()
     * method is called.
     *
     * @param bounds Rect to receive the drawable's bounds (allocated by the
     *               caller).
     */
    CARAPI CopyBounds(
        /* [in] */ IRect* bounds);

    /**
     * Return a copy of the drawable's bounds in a new Rect. This returns the
     * same values as getBounds(), but the returned object is guaranteed to not
     * be changed later by the drawable (i.e. it retains no reference to this
     * rect). If the caller already has a Rect allocated, call copyBounds(rect).
     *
     * @return A copy of the drawable's bounds
     */
    CARAPI CopyBounds(
        /* [out] */ IRect** bounds);

    /**
     * Return the drawable's bounds Rect. Note: for efficiency, the returned
     * object may be the same object stored in the drawable (though this is not
     * guaranteed), so if a persistent copy of the bounds is needed, call
     * copyBounds(rect) instead.
     * You should also not change the object returned by this method as it may
     * be the same object stored in the drawable.
     *
     * @return The bounds of the drawable (which may change later, so caller
     *         beware). DO NOT ALTER the returned object as it may change the
     *         stored bounds of this drawable.
     *
     * @see #copyBounds()
     * @see #copyBounds(android.graphics.Rect)
     */
    CARAPI GetBounds(
        /* [out] */ IRect** bounds);

    /**
     * Return the drawable's dirty bounds Rect. Note: for efficiency, the
     * returned object may be the same object stored in the drawable (though
     * this is not guaranteed).
     * <p>
     * By default, this returns the full drawable bounds. Custom drawables may
     * override this method to perform more precise invalidation.
     *
     * @return The dirty bounds of this drawable
     */
    virtual CARAPI GetDirtyBounds(
        /* [out] */ IRect** bounds);

    /**
     * Set a mask of the configuration parameters for which this drawable
     * may change, requiring that it be re-created.
     *
     * @param configs A mask of the changing configuration parameters, as
     * defined by {@link android.content.pm.ActivityInfo}.
     *
     * @see android.content.pm.ActivityInfo
     */
    virtual CARAPI SetChangingConfigurations(
        /* [in] */ Int32 configs);

    /**
     * Return a mask of the configuration parameters for which this drawable
     * may change, requiring that it be re-created.  The default implementation
     * returns whatever was provided through
     * {@link #setChangingConfigurations(int)} or 0 by default.  Subclasses
     * may extend this to or in the changing configurations of any other
     * drawables they hold.
     *
     * @return Returns a mask of the changing configuration parameters, as
     * defined by {@link android.content.pm.ActivityInfo}.
     *
     * @see android.content.pm.ActivityInfo
     */
    virtual CARAPI GetChangingConfigurations(
        /* [out] */ Int32* configs);

    /**
     * Set to true to have the drawable dither its colors when drawn to a device
     * with fewer than 8-bits per color component. This can improve the look on
     * those devices, but can also slow down the drawing a little.
     */
    virtual CARAPI SetDither(
        /* [in] */ Boolean dither);

    /**
     * Set to true to have the drawable filter its bitmap when scaled or rotated
     * (for drawables that use bitmaps). If the drawable does not use bitmaps,
     * this call is ignored. This can improve the look when scaled or rotated,
     * but also slows down the drawing.
     */
    virtual CARAPI SetFilterBitmap(
        /* [in] */ Boolean filter);

    /**
     * Bind a {@link Callback} object to this Drawable.  Required for clients
     * that want to support animated drawables.
     *
     * @param cb The client's Callback implementation.
     */
    virtual CARAPI SetCallback(
        /* [in] */ IDrawableCallback* cb);

    /**
     * Return the current {@link Callback} implementation attached to this
     * Drawable.
     *
     * @return A {@link Callback} instance or null if no callback was set.
     *
     * @see #setCallback(android.graphics.drawable.Drawable.Callback)
     */
    virtual CARAPI GetCallback(
        /* [out] */ IDrawableCallback** callback);

    /**
     * Use the current {@link Callback} implementation to have this Drawable
     * redrawn.  Does nothing if there is no Callback attached to the
     * Drawable.
     *
     * @see Callback#invalidateDrawable
     */
    virtual CARAPI InvalidateSelf();

    /**
     * Use the current {@link Callback} implementation to have this Drawable
     * scheduled.  Does nothing if there is no Callback attached to the
     * Drawable.
     *
     * @param what The action being scheduled.
     * @param when The time (in milliseconds) to run.
     *
     * @see Callback#scheduleDrawable
     */
    virtual CARAPI ScheduleSelf(
        /* [in] */ IRunnable* what,
        /* [in] */ Int64 when);

    /**
     * Use the current {@link Callback} implementation to have this Drawable
     * unscheduled.  Does nothing if there is no Callback attached to the
     * Drawable.
     *
     * @param what The runnable that you no longer want called.
     *
     * @see Callback#unscheduleDrawable
     */
    virtual CARAPI UnscheduleSelf(
        /* [in] */ IRunnable* what);

    /**
     * Returns the resolved layout direction for this Drawable.
     *
     * @return One of {@link android.view.View#LAYOUT_DIRECTION_LTR},
     *   {@link android.view.View#LAYOUT_DIRECTION_RTL}
     *
     * @hide
     */
    virtual CARAPI GetLayoutDirection(
        /* [out] */ Int32* dir);

    /**
     * Set the layout direction for this drawable. Should be a resolved direction as the
     * Drawable as no capacity to do the resolution on his own.
     *
     * @param layoutDirection One of {@link android.view.View#LAYOUT_DIRECTION_LTR},
     *   {@link android.view.View#LAYOUT_DIRECTION_RTL}
     *
     * @hide
     */
    virtual CARAPI SetLayoutDirection(
        /* [in] */ Int32 layoutDirection);

    /**
     * Specify an alpha value for the drawable. 0 means fully transparent, and
     * 255 means fully opaque.
     */
    virtual CARAPI SetAlpha(
        /* [in] */ Int32 alpha) = 0;

    /**
     * Gets the current alpha value for the drawable. 0 means fully transparent,
     * 255 means fully opaque. This method is implemented by
     * Drawable subclasses and the value returned is specific to how that class treats alpha.
     * The default return value is 255 if the class does not override this method to return a value
     * specific to its use of alpha.
     */
    virtual CARAPI GetAlpha(
        /* [out] */ Int32* alpha);

    /**
     * @hide Consider for future API inclusion
     */
    virtual CARAPI SetXfermode(
        /* [in] */ IXfermode* mode);

    /**
     * Specify an optional color filter for the drawable. Pass {@code null} to
     * remove any existing color filter.
     *
     * @param cf the color filter to apply, or {@code null} to remove the
     *            existing color filter
     */
    virtual CARAPI SetColorFilter(
        /* [in] */ IColorFilter* cf) = 0;

    /**
     * Specify a color and Porter-Duff mode to be the color filter for this
     * drawable.
     */
    virtual CARAPI SetColorFilter(
        /* [in] */ Int32 color,
        /* [in] */ PorterDuffMode mode);

    /**
     * Specifies a tint for this drawable.
     * <p>
     * Setting a color filter via {@link #setColorFilter(ColorFilter)} overrides
     * tint.
     *
     * @param tint Color to use for tinting this drawable
     * @see #setTintMode(PorterDuff.Mode)
     */
    virtual CARAPI SetTint(
        /* [in] */ Int32 tint);

    /**
     * Specifies a tint for this drawable as a color state list.
     * <p>
     * Setting a color filter via {@link #setColorFilter(ColorFilter)} overrides
     * tint.
     *
     * @param tint Color state list to use for tinting this drawable, or null to
     *            clear the tint
     * @see #setTintMode(PorterDuff.Mode)
     */
    virtual CARAPI SetTintList(
        /* [in] */ IColorStateList* tint);

    /**
     * Specifies a tint blending mode for this drawable.
     * <p>
     * Setting a color filter via {@link #setColorFilter(ColorFilter)} overrides
     * tint.
     *
     * @param tintMode Color state list to use for tinting this drawable, or null to
     *            clear the tint
     * @param tintMode A Porter-Duff blending mode
     */
    virtual CARAPI SetTintMode(
        /* [in] */ PorterDuffMode tintMode);

    /**
     * Returns the current color filter, or {@code null} if none set.
     *
     * @return the current color filter, or {@code null} if none set
     */
    virtual CARAPI GetColorFilter(
        /* [out] */ IColorFilter** filter);

    virtual CARAPI ClearColorFilter();

    /**
     * Specifies the hotspot's location within the drawable.
     *
     * @param x The X coordinate of the center of the hotspot
     * @param y The Y coordinate of the center of the hotspot
     */
    virtual CARAPI SetHotspot(
        /* [in] */ Float x,
        /* [in] */ Float y);

    /**
     * Sets the bounds to which the hotspot is constrained, if they should be
     * different from the drawable bounds.
     *
     * @param left
     * @param top
     * @param right
     * @param bottom
     */
    virtual CARAPI SetHotspotBounds(
        /* [in] */ Int32 left,
        /* [in] */ Int32 top,
        /* [in] */ Int32 right,
        /* [in] */ Int32 bottom);

    /** @hide For internal use only. Individual results may vary. */
    virtual CARAPI GetHotspotBounds(
        /* [in] */ IRect* outRect);

    /**
     * Whether this drawable requests projection.
     *
     * @hide magic!
     */
    virtual CARAPI IsProjected(
        /* [out] */ Boolean* projected);

    /**
     * Indicates whether this view will change its appearance based on state.
     * Clients can use this to determine whether it is necessary to calculate
     * their state and call setState.
     *
     * @return True if this view changes its appearance based on state, false
     *         otherwise.
     *
     * @see #setState(int[])
     */
    virtual CARAPI IsStateful(
        /* [out] */ Boolean* isStateful);

    /**
     * Specify a set of states for the drawable. These are use-case specific,
     * so see the relevant documentation. As an example, the background for
     * widgets like Button understand the following states:
     * [{@link android.R.attr#state_focused},
     *  {@link android.R.attr#state_pressed}].
     *
     * <p>If the new state you are supplying causes the appearance of the
     * Drawable to change, then it is responsible for calling
     * {@link #invalidateSelf} in order to have itself redrawn, <em>and</em>
     * true will be returned from this function.
     *
     * <p>Note: The Drawable holds a reference on to <var>stateSet</var>
     * until a new state array is given to it, so you must not modify this
     * array during that time.</p>
     *
     * @param stateSet The new set of states to be displayed.
     *
     * @return Returns true if this change in state has caused the appearance
     * of the Drawable to change (hence requiring an invalidate), otherwise
     * returns false.
     */
    virtual CARAPI SetState(
        /* [in] */ ArrayOf<Int32>* stateSet,
        /* [out] */ Boolean* isStateful);

    /**
     * Describes the current state, as a union of primitve states, such as
     * {@link android.R.attr#state_focused},
     * {@link android.R.attr#state_selected}, etc.
     * Some drawables may modify their imagery based on the selected state.
     * @return An array of resource Ids describing the current state.
     */
    virtual CARAPI GetState(
        /* [out, callee] */ ArrayOf<Int32>** stateSet);

    /**
     * If this Drawable does transition animations between states, ask that
     * it immediately jump to the current state and skip any active animations.
     */
    virtual CARAPI JumpToCurrentState();

    /**
     * @return The current drawable that will be used by this drawable. For simple drawables, this
     *         is just the drawable itself. For drawables that change state like
     *         {@link StateListDrawable} and {@link LevelListDrawable} this will be the child drawable
     *         currently in use.
     */
    virtual CARAPI GetCurrent(
        /* [out] */ IDrawable** drawable);

    /**
     * Specify the level for the drawable.  This allows a drawable to vary its
     * imagery based on a continuous controller, for example to show progress
     * or volume level.
     *
     * <p>If the new level you are supplying causes the appearance of the
     * Drawable to change, then it is responsible for calling
     * {@link #invalidateSelf} in order to have itself redrawn, <em>and</em>
     * true will be returned from this function.
     *
     * @param level The new level, from 0 (minimum) to 10000 (maximum).
     *
     * @return Returns true if this change in level has caused the appearance
     * of the Drawable to change (hence requiring an invalidate), otherwise
     * returns false.
     */
    CARAPI SetLevel(
        /* [in] */ Int32 level,
        /* [out] */ Boolean* change);

    /**
     * Retrieve the current level.
     *
     * @return int Current level, from 0 (minimum) to 10000 (maximum).
     */
    CARAPI GetLevel(
        /* [out] */ Int32* level);

    /**
     * Set whether this Drawable is visible.  This generally does not impact
     * the Drawable's behavior, but is a hint that can be used by some
     * Drawables, for example, to decide whether run animations.
     *
     * @param visible Set to true if visible, false if not.
     * @param restart You can supply true here to force the drawable to behave
     *                as if it has just become visible, even if it had last
     *                been set visible.  Used for example to force animations
     *                to restart.
     *
     * @return boolean Returns true if the new visibility is different than
     *         its previous state.
     */
    virtual CARAPI SetVisible(
        /* [in] */ Boolean visible,
        /* [in] */ Boolean restart,
        /* [out] */ Boolean* isDifferent);

    CARAPI IsVisible(
        /* [out] */ Boolean* visible);

    /**
     * Set whether this Drawable is automatically mirrored when its layout direction is RTL
     * (right-to left). See {@link android.util.LayoutDirection}.
     *
     * @param mirrored Set to true if the Drawable should be mirrored, false if not.
     */
    virtual CARAPI SetAutoMirrored(
        /* [in] */ Boolean mirrored);

    /**
     * Tells if this Drawable will be automatically mirrored  when its layout direction is RTL
     * right-to-left. See {@link android.util.LayoutDirection}.
     *
     * @return boolean Returns true if this Drawable will be automatically mirrored.
     */
    virtual CARAPI IsAutoMirrored(
        /* [out] */ Boolean* mirrored);

    /**
     * Applies the specified theme to this Drawable and its children.
     */
    virtual CARAPI ApplyTheme(
        /* [in] */ /*@SuppressWarnings("unused")*/ IResourcesTheme* t);

    virtual CARAPI CanApplyTheme(
        /* [out] */ Boolean* can);

    /**
     * Return the opacity/transparency of this Drawable.  The returned value is
     * one of the abstract format constants in
     * {@link android.graphics.PixelFormat}:
     * {@link android.graphics.PixelFormat#UNKNOWN},
     * {@link android.graphics.PixelFormat#TRANSLUCENT},
     * {@link android.graphics.PixelFormat#TRANSPARENT}, or
     * {@link android.graphics.PixelFormat#OPAQUE}.
     *
     * <p>Generally a Drawable should be as conservative as possible with the
     * value it returns.  For example, if it contains multiple child drawables
     * and only shows one of them at a time, if only one of the children is
     * TRANSLUCENT and the others are OPAQUE then TRANSLUCENT should be
     * returned.  You can use the method {@link #resolveOpacity} to perform a
     * standard reduction of two opacities to the appropriate single output.
     *
     * <p>Note that the returned value does <em>not</em> take into account a
     * custom alpha or color filter that has been applied by the client through
     * the {@link #setAlpha} or {@link #setColorFilter} methods.
     *
     * @return int The opacity class of the Drawable.
     *
     * @see android.graphics.PixelFormat
     */
    virtual CARAPI GetOpacity(
        /* [out] */ Int32* opacity) = 0;

    /**
     * Return the appropriate opacity value for two source opacities.  If
     * either is UNKNOWN, that is returned; else, if either is TRANSLUCENT,
     * that is returned; else, if either is TRANSPARENT, that is returned;
     * else, OPAQUE is returned.
     *
     * <p>This is to help in implementing {@link #getOpacity}.
     *
     * @param op1 One opacity value.
     * @param op2 Another opacity value.
     *
     * @return int The combined opacity value.
     *
     * @see #getOpacity
     */
    static CARAPI ResolveOpacity(
        /* [in] */ Int32 op1,
        /* [in] */ Int32 op2,
        /* [out] */ Int32* opacity);

    /**
     * Returns a Region representing the part of the Drawable that is completely
     * transparent.  This can be used to perform drawing operations, identifying
     * which parts of the target will not change when rendering the Drawable.
     * The default implementation returns null, indicating no transparent
     * region; subclasses can optionally override this to return an actual
     * Region if they want to supply this optimization information, but it is
     * not required that they do so.
     *
     * @return Returns null if the Drawables has no transparent region to
     * report, else a Region holding the parts of the Drawable's bounds that
     * are transparent.
     */
    virtual CARAPI GetTransparentRegion(
        /* [out] */ IRegion** bounds);

    /**
     * Return the intrinsic width of the underlying drawable object.  Returns
     * -1 if it has no intrinsic width, such as with a solid color.
     */
    virtual CARAPI GetIntrinsicWidth(
        /* [out] */ Int32* width);

    /**
     * Return the intrinsic height of the underlying drawable object. Returns
     * -1 if it has no intrinsic height, such as with a solid color.
     */
    virtual CARAPI GetIntrinsicHeight(
        /* [out] */ Int32* height);

    /**
     * Returns the minimum width suggested by this Drawable. If a View uses this
     * Drawable as a background, it is suggested that the View use at least this
     * value for its width. (There will be some scenarios where this will not be
     * possible.) This value should INCLUDE any padding.
     *
     * @return The minimum width suggested by this Drawable. If this Drawable
     *         doesn't have a suggested minimum width, 0 is returned.
     */
    virtual CARAPI GetMinimumWidth(
        /* [out] */ Int32* width);

    /**
     * Returns the minimum height suggested by this Drawable. If a View uses this
     * Drawable as a background, it is suggested that the View use at least this
     * value for its height. (There will be some scenarios where this will not be
     * possible.) This value should INCLUDE any padding.
     *
     * @return The minimum height suggested by this Drawable. If this Drawable
     *         doesn't have a suggested minimum height, 0 is returned.
     */
    virtual CARAPI GetMinimumHeight(
        /* [out] */ Int32* height);

    /**
     * Return in padding the insets suggested by this Drawable for placing
     * content inside the drawable's bounds. Positive values move toward the
     * center of the Drawable (set Rect.inset). Returns true if this drawable
     * actually has a padding, else false. When false is returned, the padding
     * is always set to 0.
     */
    virtual CARAPI GetPadding(
        /* [in, out] */ IRect* padding,
        /* [out] */ Boolean* isPadding);

    /**
     * Return in insets the layout insets suggested by this Drawable for use with alignment
     * operations during layout.
     *
     * @hide
     */
    virtual CARAPI GetOpticalInsets(
        /* [out] */ IInsets** sets);

    /**
     * Called to get the drawable to populate the Outline that defines its drawing area.
     * <p>
     * This method is called by the default {@link android.view.ViewOutlineProvider} to define
     * the outline of the View.
     * <p>
     * The default behavior defines the outline to be the bounding rectangle of 0 alpha.
     * Subclasses that wish to convey a different shape or alpha value must override this method.
     *
     * @see android.view.View#setOutlineProvider(android.view.ViewOutlineProvider)
     */
    virtual CARAPI GetOutline(
        /* [in] */ /*@NonNull*/ IOutline* outline);

    /**
     * Make this drawable mutable. This operation cannot be reversed. A mutable
     * drawable is guaranteed to not share its state with any other drawable.
     * This is especially useful when you need to modify properties of drawables
     * loaded from resources. By default, all drawables instances loaded from
     * the same resource share a common state; if you modify the state of one
     * instance, all the other instances will receive the same modification.
     *
     * Calling this method on a mutable Drawable will have no effect.
     *
     * @return This drawable.
     */
    virtual CARAPI Mutate(
        /* [out] */ IDrawable** drawable);

    /**
     * Create a drawable from an inputstream
     */
    static CARAPI CreateFromStream(
        /* [in] */ IInputStream* is,
        /* [in] */ const String& srcName,
        /* [out] */ IDrawable** drawable);

    /**
     * Create a drawable from an inputstream, using the given resources and
     * value to determine density information.
     */
    static CARAPI CreateFromResourceStream(
        /* [in] */ IResources* res,
        /* [in] */ ITypedValue* value,
        /* [in] */ IInputStream* is,
        /* [in] */ const String& srcName,
        /* [out] */ IDrawable** drawable);

    /**
     * Create a drawable from an inputstream, using the given resources and
     * value to determine density information.
     */
    static CARAPI CreateFromResourceStream(
        /* [in] */ IResources* res,
        /* [in] */ ITypedValue* value,
        /* [in] */ IInputStream* is,
        /* [in] */ const String& srcName,
        /* [in] */ IBitmapFactoryOptions* opts,
        /* [out] */ IDrawable** drawable);

    /**
     * Create a drawable from an XML document. For more information on how to
     * create resources in XML, see
     * <a href="{@docRoot}guide/topics/resources/drawable-resource.html">Drawable Resources</a>.
     */
    static CARAPI CreateFromXml(
        /* [in] */ IResources* r,
        /* [in] */ IXmlPullParser* parser,
        /* [out] */ IDrawable** drawable);

    /**
     * Create a drawable from an XML document using an optional {@link Theme}.
     * For more information on how to create resources in XML, see
     * <a href="{@docRoot}guide/topics/resources/drawable-resource.html">Drawable Resources</a>.
     */
    static CARAPI CreateFromXml(
        /* [in] */ IResources* r,
        /* [in] */ IXmlPullParser* parser,
        /* [in] */ IResourcesTheme* theme,
        /* [out] */ IDrawable** draw);

    /**
     * Create from inside an XML document.  Called on a parser positioned at
     * a tag in an XML document, tries to create a Drawable from that tag.
     * Returns null if the tag is not a valid drawable.
     */
    static CARAPI CreateFromXmlInner(
        /* [in] */ IResources* r,
        /* [in] */ IXmlPullParser* parser,
        /* [in] */ IAttributeSet* attrs,
        /* [out] */ IDrawable** drawable);

    /**
     * Create a drawable from inside an XML document using an optional
     * {@link Theme}. Called on a parser positioned at a tag in an XML
     * document, tries to create a Drawable from that tag. Returns {@code null}
     * if the tag is not a valid drawable.
     */
    static CARAPI CreateFromXmlInner(
        /* [in] */ IResources* r,
        /* [in] */ IXmlPullParser* parser,
        /* [in] */ IAttributeSet* attrs,
        /* [in] */ IResourcesTheme* theme,
        /* [out] */ IDrawable** draw);

    /**
     * Create a drawable from file path name.
     */
    static CARAPI CreateFromPath(
        /* [in] */ const String& pathName,
        /* [out] */ IDrawable** drawable);

    static CARAPI ParseTintMode(
        /* [in] */ Int32 value,
        /* [in] */ PorterDuffMode defaultMode,
        /* [out] */ PorterDuffMode* mode);

    virtual CARAPI Inflate(
        /* [in] */ IResources* r,
        /* [in] */ IXmlPullParser* parser,
        /* [in] */ IAttributeSet* attrs)/* throws XmlPullParserException, IOException*/;

    /**
     * Inflate this Drawable from an XML resource optionally styled by a theme.
     *
     * @param r Resources used to resolve attribute values
     * @param parser XML parser from which to inflate this Drawable
     * @param attrs Base set of attribute values
     * @param theme Theme to apply, may be null
     * @throws XmlPullParserException
     * @throws IOException
     */
    virtual CARAPI Inflate(
        /* [in] */ IResources* r,
        /* [in] */ IXmlPullParser* parser,
        /* [in] */ IAttributeSet* attrs,
        /* [in] */ IResourcesTheme* theme)/* throws XmlPullParserException, IOException*/;

    /* package */ virtual CARAPI InflateWithAttributes(
        /* [in] */ IResources* r,
        /* [in] */ IXmlPullParser* parser,
        /* [in] */ ITypedArray* attrs,
        /* [in] */ Int32 visibleAttr);

    virtual CARAPI GetConstantState(
        /* [out] */ IDrawableConstantState** state);

protected:
    /**
     * Override this in your subclass to change appearance if you recognize the
     * specified state.
     *
     * @return Returns true if the state change has caused the appearance of
     * the Drawable to change (that is, it needs to be drawn), else false
     * if it looks the same and there is no need to redraw it since its
     * last state.
     */
    virtual CARAPI_(Boolean) OnStateChange(
        /* [in] */ const ArrayOf<Int32>* state);

    /** Override this in your subclass to change appearance if you vary based
     *  on level.
     * @return Returns true if the level change has caused the appearance of
     * the Drawable to change (that is, it needs to be drawn), else false
     * if it looks the same and there is no need to redraw it since its
     * last level.
     */
    virtual CARAPI_(Boolean) OnLevelChange(
        /* [in] */ Int32 level);

    /**
     * Override this in your subclass to change appearance if you vary based on
     * the bounds.
     */
    virtual CARAPI_(void) OnBoundsChange(
        /* [in] */ IRect* bounds);

    /**
     * Ensures the tint filter is consistent with the current tint color and
     * mode.
     */
    virtual CARAPI_(AutoPtr<IPorterDuffColorFilter>) UpdateTintFilter(
        /* [in] */ IPorterDuffColorFilter* tintFilter,
        /* [in] */ IColorStateList* tint,
        /* [in] */ PorterDuffMode tintMode);

    /**
     * Obtains styled attributes from the theme, if available, or unstyled
     * resources if the theme is null.
     */
    static CARAPI ObtainAttributes(
        /* [in] */ IResources* res,
        /* [in] */ IResourcesTheme* theme,
        /* [in] */ IAttributeSet* set,
        /* [in] */ ArrayOf<Int32>* attrs,
        /* [out] */ ITypedArray** a);

private:
    static CARAPI DrawableFromBitmap(
        /* [in] */ IResources* res,
        /* [in] */ IBitmap* bm,
        /* [in] */ ArrayOf<Byte>* np,
        /* [in] */ IRect* pad,
        /* [in] */ IRect* layoutBounds,
        /* [in] */ const String& srcName,
        /* [out] */ IDrawable** drawable);

protected:
    static AutoPtr<IRect> ZERO_BOUNDS_RECT;
    static const PorterDuffMode DEFAULT_TINT_MODE;

    AutoPtr< ArrayOf<Int32> > mStateSet;
    Int32 mLevel;
    Int32 mChangingConfigurations;
    AutoPtr<IRect> mBounds;
    AutoPtr<IWeakReference> mCallback;
    Boolean mVisible;

    Int32 mLayoutDirection;
};

} // namespace Drawable
} // namespace Graphics
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_GRAPHICS_DRAWABLE_DRAWABLE_H__

