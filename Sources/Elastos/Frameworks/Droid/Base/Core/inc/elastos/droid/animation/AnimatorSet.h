
#ifndef __ELASTOS_DROID_ANIMATION_ANIMATORSET_H__
#define __ELASTOS_DROID_ANIMATION_ANIMATORSET_H__

#include "elastos/droid/animation/Animator.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/animation/AnimatorListenerAdapter.h"
#include <elastos/utility/etl/HashMap.h>

using Elastos::Core::EIID_ICloneable;
using Elastos::Utility::ICollection;
using Elastos::Utility::IList;
using Elastos::Utility::Etl::HashMap;

DEFINE_OBJECT_HASH_FUNC_FOR(Elastos::Droid::Animation::IAnimator)

namespace Elastos {
namespace Droid {
namespace Animation {

class AnimatorSet
    : public Animator
    , public IAnimatorSet
{
public:
    class Node;

private:
    /**
     * This class is the mechanism by which animations are started based on events in other
     * animations. If an animation has multiple dependencies on other animations, then
     * all dependencies must be satisfied before the animation is started.
     */
    class DependencyListener
        : public Object
        , public IAnimatorListener
        , public IDependencyListener
    {
    public:
        CAR_INTERFACE_DECL();

        DependencyListener(
            /* [in] */ AnimatorSet* animatorset,
            /* [in] */ Node* node,
            /* [in] */ Int32 rule);

        /**
         * A start event is received - see if this is an event we are listening for
         */
        CARAPI OnAnimationStart(
            /* [in] */ IAnimator* animation);

        /**
         * An end event is received - see if this is an event we are listening for
         */
        CARAPI OnAnimationEnd(
            /* [in] */ IAnimator* animation);

        /**
         * Ignore cancel events for now. We may want to handle this eventually,
         * to prevent follow-on animations from running when some dependency
         * animation is canceled.
         */
        CARAPI OnAnimationCancel(
            /* [in] */ IAnimator* animation);

        /**
         * Ignore repeat events for now
         */
        CARAPI OnAnimationRepeat(
            /* [in] */ IAnimator* animation);

        CARAPI_(void) StartIfReady(
            /* [in] */ IAnimator* dependencyAnimation);

    private:
        AnimatorSet* mAnimatorSet;

        // The node upon which the dependency is based.
        AutoPtr<Node> mNode;

        // The Dependency rule (WITH or AFTER) that the listener should wait for on
        // the node
        Int32 mRule;
    };

    class AnimatorSetListener
        : public Object
        , public IAnimatorListener
        , public IAnimatorSetListener
    {
    public:
        CAR_INTERFACE_DECL();

        AnimatorSetListener(
            /* [in] */ AnimatorSet* animatorSet);

        CARAPI OnAnimationCancel(
            /* [in] */ IAnimator* animation);

        CARAPI OnAnimationEnd(
            /* [in] */ IAnimator* animation);

        CARAPI OnAnimationRepeat(
            /* [in] */ IAnimator* animation);

        CARAPI OnAnimationStart(
            /* [in] */ IAnimator* animation);
    private:
        AnimatorSet* mAnimatorSet;
    };

    /**
     * Dependency holds information about the node that some other node is
     * dependent upon and the nature of that dependency.
     *
     */
    class Dependency : public Object
    {
    public:
        static const Int32 WITH = 0;    // dependent node must start with this dependency node
        static const Int32 AFTER = 1;   // dependent node must start when this dependency node finishes

        // The node that the other node with this Dependency is dependent upon
        AutoPtr<Node> mNode;

        // The nature of the dependency (WITH or AFTER)
        Int32 mRule;

        Dependency(
            /* [in] */ Node* node,
            /* [in] */ Int32 rule);
    };

public:
    /**
     * A Node is an embodiment of both the Animator that it wraps as well as
     * any dependencies that are associated with that Animation. This includes
     * both dependencies upon other nodes (in the dependencies list) as
     * well as dependencies of other nodes upon this (in the nodeDependents list).
     */
    class Node
        : public Object
        , public ICloneable
    {
    public:
        CAR_INTERFACE_DECL();

        Node(
            /* [in] */ IAnimator* animation);

        ~Node();

        /**
         * Add a dependency to this Node. The dependency includes information about the
         * node that this node is dependency upon and the nature of the dependency.
         * @param dependency
         */
        void AddDependency(
            /* [in] */ Dependency* dependency);

        CARAPI Clone(
            /* [out] */ IInterface** obj);

    private:
        Node();

    public:
        AutoPtr<IAnimator> mAnimation;

        /**
         *  These are the dependencies that this node's animation has on other
         *  nodes. For example, if this node's animation should begin with some
         *  other animation ends, then there will be an item in this node's
         *  dependencies list for that other animation's node.
         */
        List<AutoPtr<Dependency> > mDependencies;

        /**
         * tmpDependencies is a runtime detail. We use the dependencies list for sorting.
         * But we also use the list to keep track of when multiple dependencies are satisfied,
         * but removing each dependency as it is satisfied. We do not want to remove
         * the dependency itself from the list, because we need to retain that information
         * if the AnimatorSet is launched in the future. So we create a copy of the dependency
         * list when the AnimatorSet starts and use this tmpDependencies list to track the
         * list of satisfied dependencies.
         */
        List<AutoPtr<Dependency> > mTmpDependencies;

        /**
         * nodeDependencies is just a list of the nodes that this Node is dependent upon.
         * This information is used in sortNodes(), to determine when a node is a root.
         */
        List<AutoPtr<Node> > mNodeDependencies;

        /**
         * nodeDepdendents is the list of nodes that have this node as a dependency. This
         * is a utility field used in sortNodes to facilitate removing this node as a
         * dependency when it is a root node.
         */
        // mNodeDependents's element has this's reference in it's mNodeDependencies
        List<Node* > mNodeDependents;

        /**
         * Flag indicating whether the animation in this node is finished. This flag
         * is used by AnimatorSet to check, as each animation ends, whether all child animations
         * are done and it's time to send out an end event for the entire AnimatorSet.
         */
        Boolean mDone;
    };

private:
    class AnimatorListenerAdapterIMPL : public AnimatorListenerAdapter
    {
    public:
        AnimatorListenerAdapterIMPL(
            /* [in] */ AnimatorSet* host,
            /* [in] */ List<AutoPtr<Node> >* node);

        ~AnimatorListenerAdapterIMPL();

        CARAPI OnAnimationStart(
            /* [in] */ IAnimator* animation);

        CARAPI OnAnimationEnd(
            /* [in] */ IAnimator* animation);

        CARAPI OnAnimationCancel(
            /* [in] */ IAnimator* animation);

        CARAPI OnAnimationRepeat(
            /* [in] */ IAnimator* animation);
    private:
        AnimatorSet* mHost;
        AutoPtr< List<AutoPtr<Node> > > mNodes;
        Boolean mCanceled;
    };

public:
    CAR_INTERFACE_DECL();

    /**
     * Sets up this AnimatorSet to play all of the supplied animations at the same time.
     * This is equivalent to calling {@link #play(Animator)} with the first animator in the
     * set and then {@link Builder#with(Animator)} with each of the other animators. Note that
     * an Animator with a {@link Animator#setStartDelay(long) startDelay} will not actually
     * start until that delay elapses, which means that if the first animator in the list
     * supplied to this constructor has a startDelay, none of the other animators will start
     * until that first animator's startDelay has elapsed.
     *
     * @param items The animations that will be started simultaneously.
     */
    virtual CARAPI PlayTogether(
        /* [in] */ ArrayOf<IAnimator*>* items);

    /**
     * Sets up this AnimatorSet to play all of the supplied animations at the same time.
     *
     * @param items The animations that will be started simultaneously.
     */
    virtual CARAPI PlayTogether(
        /* [in] */ ICollection* items);

    /**
     * Sets up this AnimatorSet to play each of the supplied animations when the
     * previous animation ends.
     *
     * @param items The animations that will be started one after another.
     */
    virtual CARAPI PlaySequentially(
        /* [in] */ ArrayOf<IAnimator*>* items);

    /**
     * Sets up this AnimatorSet to play each of the supplied animations when the
     * previous animation ends.
     *
     * @param items The animations that will be started one after another.
     */
    virtual CARAPI PlaySequentially(
        /* [in] */ IList* items);

    /**
     * Returns the current list of child Animator objects controlled by this
     * AnimatorSet. This is a copy of the internal list; modifications to the returned list
     * will not affect the AnimatorSet, although changes to the underlying Animator objects
     * will affect those objects being managed by the AnimatorSet.
     *
     * @return ArrayList<Animator> The list of child animations of this AnimatorSet.
     */
    virtual CARAPI GetChildAnimations(
        /* [out, callee] */ ArrayOf<IAnimator*>** childAnimations);

    virtual CARAPI GetChildAnimations(
        /* [out] */ IArrayList** childAnimations);


    //     @Override

    /**
     * Sets the target object for all current {@link #getChildAnimations() child animations}
     * of this AnimatorSet that take targets ({@link ObjectAnimator} and
     * AnimatorSet).
     *
     * @param target The object being animated
     */
    virtual CARAPI SetTarget(
        /* [in] */ IInterface* target);

    /**
     * Sets the TimeInterpolator for all current {@link #getChildAnimations() child animations}
     * of this AnimatorSet. The default value is null, which means that no interpolator
     * is set on this AnimatorSet. Setting the interpolator to any non-null value
     * will cause that interpolator to be set on the child animations
     * when the set is started.
     *
     * @param interpolator the interpolator to be used by each child animation of this AnimatorSet
     */
    virtual CARAPI SetInterpolator(
        /* [in] */ ITimeInterpolator* interpolator);

    virtual CARAPI GetInterpolator(
        /* [out] */ ITimeInterpolator** interpolator);

    /**
     * This method creates a <code>Builder</code> object, which is used to
     * set up playing constraints. This initial <code>play()</code> method
     * tells the <code>Builder</code> the animation that is the dependency for
     * the succeeding commands to the <code>Builder</code>. For example,
     * calling <code>play(a1).with(a2)</code> sets up the AnimatorSet to play
     * <code>a1</code> and <code>a2</code> at the same time,
     * <code>play(a1).before(a2)</code> sets up the AnimatorSet to play
     * <code>a1</code> first, followed by <code>a2</code>, and
     * <code>play(a1).after(a2)</code> sets up the AnimatorSet to play
     * <code>a2</code> first, followed by <code>a1</code>.
     *
     * <p>Note that <code>play()</code> is the only way to tell the
     * <code>Builder</code> the animation upon which the dependency is created,
     * so successive calls to the various functions in <code>Builder</code>
     * will all refer to the initial parameter supplied in <code>play()</code>
     * as the dependency of the other animations. For example, calling
     * <code>play(a1).before(a2).before(a3)</code> will play both <code>a2</code>
     * and <code>a3</code> when a1 ends; it does not set up a dependency between
     * <code>a2</code> and <code>a3</code>.</p>
     *
     * @param anim The animation that is the dependency used in later calls to the
     * methods in the returned <code>Builder</code> object. A null parameter will result
     * in a null <code>Builder</code> return value.
     * @return Builder The object that constructs the AnimatorSet based on the dependencies
     * outlined in the calls to <code>play</code> and the other methods in the
     * <code>Builder</code object.
     */
    virtual CARAPI Play(
        /* [in] */ IAnimator* anim,
        /* [out] */ IAnimatorSetBuilder** builder);

    /**
     * {@inheritDoc}
     *
     * <p>Note that canceling a <code>AnimatorSet</code> also cancels all of the animations that it
     * is responsible for.</p>
     */
    virtual CARAPI Cancel();

    /**
     * {@inheritDoc}
     *
     * <p>Note that ending a <code>AnimatorSet</code> also ends all of the animations that it is
     * responsible for.</p>
     */
    virtual CARAPI End();

    /**
     * Returns true if any of the child animations of this AnimatorSet have been started and have
     * not yet ended.
     * @return Whether this AnimatorSet has been started and has not yet ended.
     */
    virtual CARAPI IsRunning(
        /* [out] */ Boolean* running);

    virtual CARAPI IsStarted(
        /* [out] */ Boolean* started);

    virtual CARAPI GetStartDelay(
        /* [out] */ Int64* delay);

    virtual CARAPI SetStartDelay(
        /* [in] */ Int64 startDelay);

    virtual CARAPI GetDuration(
        /* [out] */ Int64* duration);

    virtual CARAPI SetDuration(
        /* [in] */ Int64 duration);

    virtual CARAPI SetupStartValues();

    virtual CARAPI SetupEndValues();

    virtual CARAPI Pause();

    virtual CARAPI Resume();

    virtual CARAPI Start();

    virtual CARAPI Clone(
        /* [out] */ IInterface** object);

    /**
     * @hide
     */
    // @Override
    virtual CARAPI CanReverse(
        /* [out] */ Boolean* can);

    /**
     * @hide
     */
    // @Override
    virtual CARAPI Reverse();

    ~AnimatorSet();

protected:
    AnimatorSet();

private:
    /**
     * This method sorts the current set of nodes, if needed. The sort is a simple
     * DependencyGraph sort, which goes like this:
     * - All nodes without dependencies become 'roots'
     * - while roots list is not null
     * -   for each root r
     * -     add r to sorted list
     * -     remove r as a dependency from any other node
     * -   any nodes with no dependencies are added to the roots list
     */
    CARAPI SortNodes();

protected:

    /**
     * Flag indicating that the AnimatorSet has been manually
     * terminated (by calling cancel() or end()).
     * This flag is used to avoid starting other animations when currently-playing
     * child animations of this AnimatorSet end. It also determines whether cancel/end
     * notifications are sent out via the normal AnimatorSetListener mechanism.
     */
    Boolean mTerminated;

private:
    friend class CAnimatorSetBuilder;
    /**
     * Tracks animations currently being played, so that we know what to
     * cancel or end when cancel() or end() is called on this AnimatorSet
     */
    List<AutoPtr<IAnimator> > mPlayingSet;

    /**
     * Contains all nodes, mapped to their respective Animators. When new
     * dependency information is added for an Animator, we want to add it
     * to a single node representing that Animator, not create a new Node
     * if one already exists.
     */
    HashMap<AutoPtr<IAnimator>, AutoPtr<Node> > mNodeMap;

    /**
     * Set of all nodes created for this AnimatorSet. This list is used upon
     * starting the set, and the nodes are placed in sorted order into the
     * sortedNodes collection.
     */
    List<AutoPtr<Node> > mNodes;

    /**
     * The sorted list of nodes. This is the order in which the animations will
     * be played. The details about when exactly they will be played depend
     * on the dependency relationships of the nodes.
     */
    List<AutoPtr<Node> > mSortedNodes;

    /**
     * Flag indicating whether the nodes should be sorted prior to playing. This
     * flag allows us to cache the previous sorted nodes so that if the sequence
     * is replayed with no changes, it does not have to re-sort the nodes again.
     */
    Boolean mNeedsSort;

    AutoPtr<AnimatorSetListener> mSetListener;

    /**
     * Indicates whether an AnimatorSet has been start()'d, whether or
     * not there is a nonzero startDelay.
     */
    Boolean mStarted;

    // The amount of time in ms to delay starting the animation after start() is called
    Int64 mStartDelay;

    // Animator used for a nonzero startDelay
    AutoPtr<IValueAnimator> mDelayAnim;

    // How long the child animations should last in ms. The default value is negative, which
    // simply means that there is no duration set on the AnimatorSet. When a real duration is
    // set, it is passed along to the child animations.
    Int64 mDuration;

    // Records the interpolator for the set. Null value indicates that no interpolator
    // was set on this AnimatorSet, so it should not be passed down to the children.
    AutoPtr<ITimeInterpolator> mInterpolator;

    Boolean mReversible;
};

}   //namespace Animation
}   //namespace Droid
}   //namespace Elastos

#ifndef NODE_PROBE
#define NODE_PROBE(x) ((ICloneable *)x->Probe(EIID_ICloneable))
#endif

#ifndef DEFINE_OBJECT_HASH_FUNC_FOR_ANIMATORSET_NODE
#define DEFINE_OBJECT_HASH_FUNC_FOR_ANIMATORSET_NODE(TypeName)                          \
_ETL_NAMESPACE_BEGIN                                                                    \
template<> struct Hash<TypeName *>                                                      \
{                                                                                       \
    size_t operator()(TypeName * s) const                                               \
    {                                                                                   \
        return (size_t)Object::GetHashCode(s);                                          \
    }                                                                                   \
};                                                                                      \
                                                                                        \
template<> struct Hash<AutoPtr<TypeName> >                                              \
{                                                                                       \
    size_t operator()(const AutoPtr<TypeName> & s) const                                \
    {                                                                                   \
        return (size_t)Object::GetHashCode(s.Get());                                    \
    }                                                                                   \
};                                                                                      \
                                                                                        \
template<> struct EqualTo<TypeName *>                                                   \
{                                                                                       \
    size_t operator()(TypeName * x, TypeName * y) const                                 \
    {                                                                                   \
        return (size_t)Object::Equals(NODE_PROBE(x), NODE_PROBE(y));                    \
    }                                                                                   \
};                                                                                      \
                                                                                        \
template<> struct EqualTo<AutoPtr<TypeName> >                                           \
{                                                                                       \
    size_t operator()(const AutoPtr<TypeName> & x, const AutoPtr<TypeName> & y) const   \
    {                                                                                   \
        return (size_t)Object::Equals(NODE_PROBE(x), NODE_PROBE(y));                    \
    }                                                                                   \
};                                                                                      \
                                                                                        \
_ETL_NAMESPACE_END
#endif // DEFINE_OBJECT_HASH_FUNC_FOR_ANIMATORSET_NODE

DEFINE_CONVERSION_FOR(Elastos::Droid::Animation::AnimatorSet::Node, IInterface)
DEFINE_OBJECT_HASH_FUNC_FOR_ANIMATORSET_NODE(Elastos::Droid::Animation::AnimatorSet::Node)

#endif  // __ELASTOS_DROID_ANIMATION_ANIMATORSET_H__
