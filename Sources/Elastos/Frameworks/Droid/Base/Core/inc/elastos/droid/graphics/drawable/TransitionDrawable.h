
#ifndef __ELASTOS_DROID_GRAPHICS_DRAWABLE_TRANSITIONDRAWABLE_H__
#define __ELASTOS_DROID_GRAPHICS_DRAWABLE_TRANSITIONDRAWABLE_H__

#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/graphics/drawable/LayerDrawable.h"

namespace Elastos {
namespace Droid {
namespace Graphics {
namespace Drawable {

class TransitionDrawable
    : public LayerDrawable
    , public ITransitionDrawable
{
public:
    class TransitionState : public LayerState
    {
    public:
        TransitionState(
            /* [in] */ TransitionState* orig,
            /* [in] */ TransitionDrawable* owner,
            /* [in] */ IResources* res);

        //@Override
        CARAPI NewDrawable(
            /* [out] */ IDrawable** drawable);

        //@Override
        CARAPI NewDrawable(
            /* [in] */ IResources* res,
            /* [out] */ IDrawable** drawable);

        // @Override
        CARAPI NewDrawable(
            /* [in] */ IResources* res,
            /* [in] */ IResourcesTheme* theme,
            /* [out] */ IDrawable** drawable);

        //@Override
        CARAPI GetChangingConfigurations(
            /* [out] */ Int32* config);
    };

public:
    CAR_INTERFACE_DECL()

    TransitionDrawable();

    /**
     * Create a new transition drawable with the specified list of layers. At least
     * 2 layers are required for this drawable to work properly.
     */
    CARAPI constructor(
        /* [in] */ ArrayOf<IDrawable*>* layers);

    /**
     * Create a new transition drawable with no layer. To work correctly, at least 2
     * layers must be added to this drawable.
     *
     * @see #TransitionDrawable(Drawable[])
     */
    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ IDrawableConstantState* state,
        /* [in] */ IResources* res,
        /* [in] */ IResourcesTheme* theme);

    CARAPI constructor(
        /* [in] */ IDrawableConstantState* state,
        /* [in] */ ArrayOf<IDrawable*>* layers);

    //@Override
    CARAPI_(AutoPtr<LayerState>) CreateConstantState(
        /* [in] */ LayerState* state,
        /* [in] */ IResources* res);

    /**
     * Begin the second layer on top of the first layer.
     *
     * @param durationMillis The length of the transition in milliseconds
     */
    virtual CARAPI StartTransition(
        /* [in] */ Int32 durationMillis);

    /**
     * Show only the first layer.
     */
    virtual CARAPI ResetTransition();

    /**
     * Reverses the transition, picking up where the transition currently is.
     * If the transition is not currently running, this will start the transition
     * with the specified duration. If the transition is already running, the last
     * known duration will be used.
     *
     * @param duration The duration to use if no transition is running.
     */
    virtual CARAPI ReverseTransition(
        /* [in] */ Int32 duration);

    //@Override
    CARAPI Draw(
        /* [in] */ ICanvas* canvas);

    /**
     * Enables or disables the cross fade of the drawables. When cross fade
     * is disabled, the first drawable is always drawn opaque. With cross
     * fade enabled, the first drawable is drawn with the opposite alpha of
     * the second drawable. Cross fade is disabled by default.
     *
     * @param enabled True to enable cross fading, false otherwise.
     */
    virtual CARAPI SetCrossFadeEnabled(
        /* [in] */ Boolean enabled);

    /**
     * Indicates whether the cross fade is enabled for this transition.
     *
     * @return True if cross fading is enabled, false otherwise.
     */
    virtual CARAPI IsCrossFadeEnabled(
        /* [out] */ Boolean* enabled);

private:
    TransitionDrawable(
        /* [in] */ TransitionState* state,
        /* [in] */ IResources* res,
        /* [in] */ IResourcesTheme* theme);

    TransitionDrawable(
        /* [in] */ TransitionState* state,
        /* [in] */ ArrayOf<IDrawable*>* layers);

private:
    /**
     * A transition is about to start.
     */
    static const Int32 TRANSITION_STARTING = 0;

    /**
     * The transition has started and the animation is in progress
     */
    static const Int32 TRANSITION_RUNNING = 1;

    /**
     * No transition will be applied
     */
    static const Int32 TRANSITION_NONE = 2;

    /**
     * The current state of the transition. One of {@link #TRANSITION_STARTING},
     * {@link #TRANSITION_RUNNING} and {@link #TRANSITION_NONE}
     */
    Int32 mTransitionState;

    Boolean mReverse;
    Int64 mStartTimeMillis;
    Int32 mFrom;
    Int32 mTo;
    Int32 mDuration;
    Int32 mOriginalDuration;
    Int32 mAlpha;
    Boolean mCrossFade;
};

} // namespace Drawable
} // namespace Graphics
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_GRAPHICS_DRAWABLE_TRANSITIONDRAWABLE_H__
