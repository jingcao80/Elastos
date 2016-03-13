
#ifndef __ELASTOS_DROID_GRAPHICS_DRAWABLE_ANIMATIONDRAWABLE_H__
#define __ELASTOS_DROID_GRAPHICS_DRAWABLE_ANIMATIONDRAWABLE_H__

#include "elastos/droid/graphics/drawable/DrawableContainer.h"

namespace Elastos {
namespace Droid {
namespace Graphics {
namespace Drawable {

/**
 *
 * An object used to create frame-by-frame animations, defined by a series of Drawable objects,
 * which can be used as a View object's background.
 * <p>
 * The simplest way to create a frame-by-frame animation is to define the animation in an XML
 * file, placed in the res/drawable/ folder, and set it as the background to a View object. Then, call
 * {@link #start()} to run the animation.
 * <p>
 * An AnimationDrawable defined in XML consists of a single <code>&lt;animation-list></code> element,
 * and a series of nested <code>&lt;item></code> tags. Each item defines a frame of the animation.
 * See the example below.
 * </p>
 * <p>spin_animation.xml file in res/drawable/ folder:</p>
 * <pre>&lt;!-- Animation frames are wheel0.png -- wheel5.png files inside the
 * res/drawable/ folder --&gt;
 * &lt;animation-list android:id=&quot;@+id/selected&quot; android:oneshot=&quot;false&quot;&gt;
 *    &lt;item android:drawable=&quot;@drawable/wheel0&quot; android:duration=&quot;50&quot; /&gt;
 *    &lt;item android:drawable=&quot;@drawable/wheel1&quot; android:duration=&quot;50&quot; /&gt;
 *    &lt;item android:drawable=&quot;@drawable/wheel2&quot; android:duration=&quot;50&quot; /&gt;
 *    &lt;item android:drawable=&quot;@drawable/wheel3&quot; android:duration=&quot;50&quot; /&gt;
 *    &lt;item android:drawable=&quot;@drawable/wheel4&quot; android:duration=&quot;50&quot; /&gt;
 *    &lt;item android:drawable=&quot;@drawable/wheel5&quot; android:duration=&quot;50&quot; /&gt;
 * &lt;/animation-list&gt;</pre>
 *
 * <p>Here is the code to load and play this animation.</p>
 * <pre>
 * // Load the ImageView that will host the animation and
 * // set its background to our AnimationDrawable XML resource.
 * ImageView img = (ImageView)findViewById(R.id.spinning_wheel_image);
 * img.setBackgroundResource(R.drawable.spin_animation);
 *
 * // Get the background, which has been compiled to an AnimationDrawable object.
 * AnimationDrawable frameAnimation = (AnimationDrawable) img.getBackground();
 *
 * // Start the animation (looped playback by default).
 * frameAnimation.start();
 * </pre>
 *
 * <div class="special reference">
 * <h3>Developer Guides</h3>
 * <p>For more information about animating with {@code AnimationDrawable}, read the
 * <a href="{@docRoot}guide/topics/graphics/drawable-animation.html">Drawable Animation</a>
 * developer guide.</p>
 * </div>
 *
 * @attr ref android.R.styleable#AnimationDrawable_visible
 * @attr ref android.R.styleable#AnimationDrawable_variablePadding
 * @attr ref android.R.styleable#AnimationDrawable_oneshot
 * @attr ref android.R.styleable#AnimationDrawableItem_duration
 * @attr ref android.R.styleable#AnimationDrawableItem_drawable
 */
class AnimationDrawable
    : public DrawableContainer
    , public IAnimationDrawable
    , public IRunnable
    , public IAnimatable
{
protected:
    class AnimationState : public DrawableContainer::DrawableContainerState
    {
    public:
        AnimationState(
            /* [in] */ AnimationState* orig,
            /* [in] */ AnimationDrawable* owner,
            /* [in] */ IResources* res);

        ~AnimationState();

        //@Override
        CARAPI NewDrawable(
            /* [out] */ IDrawable** drawable);

        //@Override
        CARAPI NewDrawable(
            /* [in] */ IResources* res,
            /* [out] */ IDrawable** drawable);

        CARAPI_(void) AddFrame(
            /* [in] */ IDrawable* dr,
            /* [in] */ Int32 dur);

        //@Override
        CARAPI_(void) GrowArray(
            /* [in] */ Int32 oldSize,
            /* [in] */ Int32 newSize);

    public:
        AutoPtr< ArrayOf<Int32> > mDurations;
        Boolean mOneShot;
    };

public:
    CAR_INTERFACE_DECL();

    AnimationDrawable();

    /**
     * Sets whether this AnimationDrawable is visible.
     * <p>
     * When the drawable becomes invisible, it will pause its animation. A
     * subsequent change to visible with <code>restart</code> set to true will
     * restart the animation from the first frame. If <code>restart</code> is
     * false, the animation will resume from the most recent frame.
     *
     * @param visible true if visible, false otherwise
     * @param restart when visible, true to force the animation to restart
     *                from the first frame
     * @return true if the new visibility is different than its previous state
     */
    //@Override
    CARAPI SetVisible(
        /* [in] */ Boolean visible,
        /* [in] */ Boolean restart,
        /* [out] */ Boolean* isDifferent);

    /**
     * <p>Starts the animation, looping if necessary. This method has no effect
     * if the animation is running. Do not call this in the {@link android.app.Activity#onCreate}
     * method of your activity, because the {@link android.graphics.drawable.AnimationDrawable} is
     * not yet fully attached to the window. If you want to play
     * the animation immediately, without requiring interaction, then you might want to call it
     * from the {@link android.app.Activity#onWindowFocusChanged} method in your activity,
     * which will get called when Android brings your window into focus.</p>
     *
     * @see #isRunning()
     * @see #stop()
     */
    virtual CARAPI Start();

    /**
     * <p>Stops the animation. This method has no effect if the animation is
     * not running.</p>
     *
     * @see #isRunning()
     * @see #start()
     */
    virtual CARAPI Stop();

    /**
     * <p>Indicates whether the animation is currently running or not.</p>
     *
     * @return true if the animation is running, false otherwise
     */
    virtual CARAPI IsRunning(
        /* [out] */ Boolean* running);

    /**
     * <p>This method exists for implementation purpose only and should not be
     * called directly. Invoke {@link #start()} instead.</p>
     *
     * @see #start()
     */
    virtual CARAPI Run();

    //@Override
    CARAPI UnscheduleSelf(
        /* [in] */ IRunnable* what);

    /**
     * @return The number of frames in the animation
     */
    virtual CARAPI GetNumberOfFrames(
        /* [out] */ Int32* number);

    /**
     * @return The Drawable at the specified frame index
     */
    virtual CARAPI GetFrame(
        /* [in] */ Int32 index,
        /* [out] */ IDrawable** drawable);

    /**
     * @return The duration in milliseconds of the frame at the
     * specified index
     */
    virtual CARAPI GetDuration(
        /* [in] */ Int32 i,
        /* [out] */ Int32* duration);

    /**
     * @return True of the animation will play once, false otherwise
     */
    virtual CARAPI IsOneShot(
        /* [out] */ Boolean* result);

    /**
     * Sets whether the animation should play once or repeat.
     *
     * @param oneShot Pass true if the animation should only play once
     */
    virtual CARAPI SetOneShot(
        /* [in] */ Boolean oneShot);

    /**
     * Add a frame to the animation
     *
     * @param frame The frame to add
     * @param duration How long in milliseconds the frame should appear
     */
    virtual CARAPI AddFrame(
        /* [in] */ IDrawable* frame,
        /* [in] */ Int32 duration);

    //@Override
    CARAPI Inflate(
        /* [in] */ IResources* r,
        /* [in] */ IXmlPullParser* parser,
        /* [in] */ IAttributeSet* attrs,
        /* [in] */ IResourcesTheme* theme);

    //@Override
    CARAPI Mutate();

protected:
    CARAPI constructor(
        /* [in] */ AnimationState* state = NULL,
        /* [in] */ IResources* res = NULL);

private:
    AnimationDrawable(
        /* [in] */ AnimationState* state,
        /* [in] */ IResources* res);

    CARAPI NextFrame(
        /* [in] */ Boolean unschedule);

    CARAPI SetFrame(
        /* [in] */ Int32 frame,
        /* [in] */ Boolean unschedule,
        /* [in] */ Boolean animate);

private:
    AutoPtr<AnimationState> mAnimationState;
    Int32 mCurFrame;
    /** Whether the drawable has an animation callback posted. */
    Boolean mRunning;

    /** Whether the drawable should animate when visible. */
    Boolean mAnimating;
    Boolean mMutated;
};

} // namespace Drawable
} // namespace Graphics
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_GRAPHICS_DRAWABLE_ANIMATIONDRAWABLE_H__
