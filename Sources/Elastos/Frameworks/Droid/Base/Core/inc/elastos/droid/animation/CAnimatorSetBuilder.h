
#ifndef __ELASTOS_DROID_ANIMATION_CANIMATORSETBUILDER_H__
#define __ELASTOS_DROID_ANIMATION_CANIMATORSETBUILDER_H__

#include "_Elastos_Droid_Animation_CAnimatorSetBuilder.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/animation/AnimatorSet.h"

namespace Elastos {
namespace Droid {
namespace Animation {

/**
 * The <code>Builder</code> object is a utility class to facilitate adding animations to a
 * <code>AnimatorSet</code> along with the relationships between the various animations. The
 * intention of the <code>Builder</code> methods, along with the {@link
 * AnimatorSet#play(Animator) play()} method of <code>AnimatorSet</code> is to make it possible
 * to express the dependency relationships of animations in a natural way. Developers can also
 * use the {@link AnimatorSet#playTogether(Animator[]) playTogether()} and {@link
 * AnimatorSet#playSequentially(Animator[]) playSequentially()} methods if these suit the need,
 * but it might be easier in some situations to express the AnimatorSet of animations in pairs.
 * <p/>
 * <p>The <code>Builder</code> object cannot be constructed directly, but is rather constructed
 * internally via a call to {@link AnimatorSet#play(Animator)}.</p>
 * <p/>
 * <p>For example, this sets up a AnimatorSet to play anim1 and anim2 at the same time, anim3 to
 * play when anim2 finishes, and anim4 to play when anim3 finishes:</p>
 * <pre>
 *     AnimatorSet s = new AnimatorSet();
 *     s.play(anim1).with(anim2);
 *     s.play(anim2).before(anim3);
 *     s.play(anim4).after(anim3);
 * </pre>
 * <p/>
 * <p>Note in the example that both {@link Builder#before(Animator)} and {@link
 * Builder#after(Animator)} are used. These are just different ways of expressing the same
 * relationship and are provided to make it easier to say things in a way that is more natural,
 * depending on the situation.</p>
 * <p/>
 * <p>It is possible to make several calls into the same <code>Builder</code> object to express
 * multiple relationships. However, note that it is only the animation passed into the initial
 * {@link AnimatorSet#play(Animator)} method that is the dependency in any of the successive
 * calls to the <code>Builder</code> object. For example, the following code starts both anim2
 * and anim3 when anim1 ends; there is no direct dependency relationship between anim2 and
 * anim3:
 * <pre>
 *   AnimatorSet s = new AnimatorSet();
 *   s.play(anim1).before(anim2).before(anim3);
 * </pre>
 * If the desired result is to play anim1 then anim2 then anim3, this code expresses the
 * relationship correctly:</p>
 * <pre>
 *   AnimatorSet s = new AnimatorSet();
 *   s.play(anim1).before(anim2);
 *   s.play(anim2).before(anim3);
 * </pre>
 * <p/>
 * <p>Note that it is possible to express relationships that cannot be resolved and will not
 * result in sensible results. For example, <code>play(anim1).after(anim1)</code> makes no
 * sense. In general, circular dependencies like this one (or more indirect ones where a depends
 * on b, which depends on c, which depends on a) should be avoided. Only create AnimatorSets
 * that can boil down to a simple, one-way relationship of animations starting with, before, and
 * after other, different, animations.</p>
 */

CarClass(CAnimatorSetBuilder)
    , public Object
    , public IAnimatorSetBuilder
{
public:
    CAR_OBJECT_DECL();

    CAR_INTERFACE_DECL();

    /**
     * package-private constructor. Builders are only constructed by AnimatorSet, when the
     * play() method is called.
     *
     * @param anim The animation that is the dependency for the other animations passed into
     * the other methods of this Builder object.
     */

    CARAPI constructor(
        /* [in] */ IAnimatorSet* host,
        /* [in] */ IAnimator* anim);
    /**
     * Sets up the given animation to play at the same time as the animation supplied in the
     * {@link AnimatorSet#play(Animator)} call that created this <code>Builder</code> object.
     *
     * @param anim The animation that will play when the animation supplied to the
     * {@link AnimatorSet#play(Animator)} method starts.
     */
    CARAPI With(
        /* [in] */ IAnimator* anim);

    /**
     * Sets up the given animation to play when the animation supplied in the
     * {@link AnimatorSet#play(Animator)} call that created this <code>Builder</code> object
     * ends.
     *
     * @param anim The animation that will play when the animation supplied to the
     * {@link AnimatorSet#play(Animator)} method ends.
     */
    CARAPI Before(
        /* [in] */ IAnimator* anim);

    /**
     * Sets up the given animation to play when the animation supplied in the
     * {@link AnimatorSet#play(Animator)} call that created this <code>Builder</code> object
     * to start when the animation supplied in this method call ends.
     *
     * @param anim The animation whose end will cause the animation supplied to the
     * {@link AnimatorSet#play(Animator)} method to play.
     */
    CARAPI After(
        /* [in] */ IAnimator* anim);

    /**
     * Sets up the animation supplied in the
     * {@link AnimatorSet#play(Animator)} call that created this <code>Builder</code> object
     * to play when the given amount of time elapses.
     *
     * @param delay The number of milliseconds that should elapse before the
     * animation starts.
     */
    CARAPI After(
        /* [in] */ Int64 delay);

private:
    AnimatorSet* mHost;

    /**
     * This tracks the current node being processed. It is supplied to the play() method
     * of AnimatorSet and passed into the constructor of Builder.
     */
    AutoPtr<AnimatorSet::Node> mCurrentNode;
};

}   //namespace Animation
}   //namespace Droid
}   //namespace Elastos

#endif  // __ELASTOS_DROID_ANIMATION_CANIMATORSETBUILDER_H__
