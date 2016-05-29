
#include <Elastos.CoreLibrary.Utility.h>
#include "elastos/droid/animation/AnimatorSet.h"
#include "elastos/droid/animation/ValueAnimator.h"
#include "elastos/droid/animation/CAnimatorSet.h"
#include "elastos/droid/animation/CAnimatorSetBuilder.h"
#include <elastos/utility/logging/Logger.h>
#include <utils/CallStack.h>

using Elastos::Core::ICloneable;
using Elastos::Utility::IIterator;
using Elastos::Utility::CArrayList;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Animation {

static const String TAG("AnimatorSet");

const Int32 AnimatorSet::Dependency::WITH = 0;
const Int32 AnimatorSet::Dependency::AFTER = 1;

//==============================================================================
//                  AnimatorSet::DependencyListener
//==============================================================================
CAR_INTERFACE_IMPL_2(AnimatorSet::DependencyListener, Object, IDependencyListener, IAnimatorListener);

AnimatorSet::DependencyListener::DependencyListener(
    /* [in] */ AnimatorSet* animatorset,
    /* [in] */ Node* node,
    /* [in] */ Int32 rule)
    : mRule(rule)
{
    mAnimatorSet = animatorset;
    mNode = node;
}

ECode AnimatorSet::DependencyListener::OnAnimationStart(
    /* [in] */ IAnimator* animation)
{
    if (mRule == Dependency::WITH) {
        StartIfReady(animation);
    }
    return NOERROR;
}

ECode AnimatorSet::DependencyListener::OnAnimationEnd(
    /* [in] */ IAnimator* animation)
{
    if (mRule == Dependency::AFTER) {
        StartIfReady(animation);
    }
    return NOERROR;
}

ECode AnimatorSet::DependencyListener::OnAnimationCancel(
    /* [in] */ IAnimator* animation)
{
    return NOERROR;
}

ECode AnimatorSet::DependencyListener::OnAnimationRepeat(
    /* [in] */ IAnimator* animation)
{
    return NOERROR;
}

void AnimatorSet::DependencyListener::StartIfReady(
    /* [in] */ IAnimator* dependencyAnimation)
{
    if (mAnimatorSet->mTerminated) {
        // if the parent AnimatorSet was canceled, then don't start any dependent anims
        return;
    }
    AutoPtr<Dependency> dependencyToRemove = NULL;
    List<AutoPtr<Dependency> >::Iterator it = mNode->mTmpDependencies.Begin();
    for (; it != mNode->mTmpDependencies.End(); ++it) {
        AutoPtr<Dependency> dependency = *it;
        if (dependency->mRule == mRule &&
                dependencyAnimation == dependency->mNode->mAnimation) {
            // rule fired - remove the dependency and listener and check to
            // see whether it's time to start the animation
            dependencyToRemove = dependency;
            dependencyAnimation->RemoveListener(this);
            break;
        }
    }
    mNode->mTmpDependencies.Remove(dependencyToRemove);
    if (mNode->mTmpDependencies.IsEmpty()) {
        // all dependencies satisfied: start the animation
        Animator* anim = (Animator*)(mNode->mAnimation.Get());
        anim->Start();
        mAnimatorSet->mPlayingSet.PushBack(mNode->mAnimation);
    }
}

//==============================================================================
//                  AnimatorSet::AnimatorSetListener
//==============================================================================
CAR_INTERFACE_IMPL_2(AnimatorSet::AnimatorSetListener, Object, IAnimatorSetListener, IAnimatorListener);

ECode AnimatorSet::AnimatorSetListener::constructor(
    /* [in] */ AnimatorSet* animatorSet)
{
    return animatorSet->GetWeakReference((IWeakReference**)&mWeakAnimatorSet);
}

ECode AnimatorSet::AnimatorSetListener::OnAnimationCancel(
    /* [in] */ IAnimator* animation)
{
    AutoPtr<IAnimatorSet> obj;
    mWeakAnimatorSet->Resolve(EIID_IAnimatorSet, (IInterface**)&obj);
    if (obj == NULL) {
        Logger::E(TAG, "Error: AnimatorSet has been released! backtrace is:");
        android::CallStack stack;
        stack.update();
        String backtrace(stack.toString("").string());
        Logger::E(TAG, "%s", backtrace.string());
        assert(0 && "Error: AnimatorSet has been released!");
        return E_ILLEGAL_STATE_EXCEPTION;
    }

    AnimatorSet* animatorSet = (AnimatorSet*)obj.Get();

    if (!animatorSet->mTerminated) {
        // Listeners are already notified of the AnimatorSet canceling in cancel().
        // The logic below only kicks in when animations end normally
        if (animatorSet->mPlayingSet.IsEmpty()) {
            if (animatorSet->mListeners != NULL) {
                Int32 numListeners;
                animatorSet->mListeners->GetSize(&numListeners);
                for (Int32 i = 0; i < numListeners; ++i) {
                    AutoPtr<IInterface> obj;
                    animatorSet->mListeners->Get(i, (IInterface**)&obj);
                    IAnimatorListener::Probe(obj)->OnAnimationCancel((IAnimator*)animatorSet);
                }
            }
        }
    }
    return NOERROR;
}

ECode AnimatorSet::AnimatorSetListener::OnAnimationEnd(
    /* [in] */ IAnimator* animation)
{
    AutoPtr<IAnimatorSet> obj;
    mWeakAnimatorSet->Resolve(EIID_IAnimatorSet, (IInterface**)&obj);
    if (obj == NULL) {
        Logger::E(TAG, "Error: %s, %s, AnimatorSet has been released! backtrace is:",
            TO_CSTR(this), TO_CSTR(animation));
        android::CallStack stack;
        stack.update();
        String backtrace(stack.toString("").string());
        Logger::E(TAG, "%s", backtrace.string());
        assert(0 && "Error: AnimatorSet has been released!");
        return E_ILLEGAL_STATE_EXCEPTION;
    }

    AnimatorSet* animatorSet = (AnimatorSet*)obj.Get();

    animation->RemoveListener(this);
    animatorSet->mPlayingSet.Remove(animation);
    AutoPtr<IAnimator> key = animation;
    AutoPtr<Node> animNode = (animatorSet->mNodeMap)[key];
    animNode->mDone = TRUE;
    if (!animatorSet->mTerminated) {
        // Listeners are already notified of the AnimatorSet ending in cancel() or
        // end(); the logic below only kicks in when animations end normally
        List<AutoPtr<Node> >& sortedNodes = animatorSet->mSortedNodes;
        Boolean allDone = TRUE;
        List<AutoPtr<Node> >::Iterator it;
        for (it = sortedNodes.Begin(); it != sortedNodes.End(); ++it) {
            if (!(*it)->mDone) {
                allDone = FALSE;
                break;
            }
        }
        if (allDone) {
            // If this was the last child animation to end, then notify listeners that this
            // AnimatorSet has ended
            if (animatorSet->mListeners != NULL) {
                AutoPtr<IInterface> cloneObj;
                ICloneable::Probe(animatorSet->mListeners)->Clone((IInterface**)&cloneObj);
                IArrayList* tmpListeners = IArrayList::Probe(cloneObj);
                Int32 numListeners;
                tmpListeners->GetSize(&numListeners);
                for (Int32 i = 0; i < numListeners; ++i) {
                    AutoPtr<IInterface> obj;
                    tmpListeners->Get(i, (IInterface**)&obj);
                    IAnimatorListener::Probe(obj)->OnAnimationEnd((IAnimator*)animatorSet);
                }
            }
            animatorSet->mStarted = FALSE;
            animatorSet->mPaused = FALSE;
        }
    }

    return NOERROR;
}

ECode AnimatorSet::AnimatorSetListener::OnAnimationRepeat(
    /* [in] */ IAnimator* animation)
{
    return NOERROR;
}

ECode AnimatorSet::AnimatorSetListener::OnAnimationStart(
    /* [in] */ IAnimator* animation)
{
    return NOERROR;
}

//==============================================================================
//                  AnimatorSet::Dependency
//==============================================================================
AnimatorSet::Dependency::Dependency(
    /* [in] */ Node* node,
    /* [in] */ Int32 rule)
    : mNode(node)
    , mRule(rule)
{
}

//==============================================================================
//                  AnimatorSet::Node
//==============================================================================
CAR_INTERFACE_IMPL(AnimatorSet::Node, Object, ICloneable);

AnimatorSet::Node::Node(
    /* [in] */ IAnimator* animation)
    : mAnimation(animation)
    , mDone(FALSE)
{}

AnimatorSet::Node::Node()
    : mDone(FALSE)
{}

AnimatorSet::Node::~Node()
{}

void AnimatorSet::Node::AddDependency(
    /* [in] */ Dependency* dependency)
{
    AutoPtr<Dependency> dependencyTmp = dependency;
    mDependencies.PushBack(dependencyTmp);
    List<AutoPtr<Node> >::Iterator itFind = Find(mNodeDependencies.Begin(), mNodeDependencies.End(), dependency->mNode);
    if (itFind == mNodeDependencies.End()) {
        mNodeDependencies.PushBack(dependency->mNode);
    }
    AutoPtr<Node> dependencyNode = dependency->mNode;
    dependencyNode->mNodeDependents.PushBack(this);
}

ECode AnimatorSet::Node::Clone(
    /* [out] */ IInterface** obj)
{
    VALIDATE_NOT_NULL(obj);
    AutoPtr<Node> newObject = new Node();
    AutoPtr<IInterface> tmp;
    ICloneable::Probe(mAnimation)->Clone((IInterface**)&tmp);
    newObject->mAnimation = IAnimator::Probe(tmp);
    newObject->mDependencies.Assign(mDependencies.Begin(), mDependencies.End());
    newObject->mTmpDependencies.Assign(mTmpDependencies.Begin(), mTmpDependencies.End());
    newObject->mNodeDependencies.Assign(mNodeDependencies.Begin(), mNodeDependencies.End());
    newObject->mNodeDependents.Assign(mNodeDependents.Begin(), mNodeDependents.End());
    newObject->mDone = mDone;
    *obj = ICloneable::Probe(newObject);
    REFCOUNT_ADD(*obj);
    return NOERROR;
}

//==============================================================================
//                  AnimatorSet::AnimatorListenerAdapterIMPL
//==============================================================================
AnimatorSet::AnimatorListenerAdapterIMPL::AnimatorListenerAdapterIMPL(
    /* [in] */ AnimatorSet* host,
    /* [in] */ List<AutoPtr<Node> >* nodes)
    : mHost(host)
    , mNodes(nodes)
    , mCanceled(FALSE)
{}

AnimatorSet::AnimatorListenerAdapterIMPL::~AnimatorListenerAdapterIMPL()
{
}

ECode AnimatorSet::AnimatorListenerAdapterIMPL::OnAnimationStart(
    /* [in] */ IAnimator* animation)
{
    return NOERROR;
}

ECode AnimatorSet::AnimatorListenerAdapterIMPL::OnAnimationEnd(
    /* [in] */ IAnimator* animation)
{
    if (!mCanceled) {
        List<AutoPtr<Node> >::Iterator it;
        for (it = mNodes->Begin(); it != mNodes->End(); ++it) {
            Animator* anim = (Animator*)((*it)->mAnimation.Get());
            anim->Start();
            mHost->mPlayingSet.PushBack((*it)->mAnimation);
        }
    }
    mHost->mDelayAnim = NULL;
    return NOERROR;
}

ECode AnimatorSet::AnimatorListenerAdapterIMPL::OnAnimationCancel(
    /* [in] */ IAnimator* animation)
{
    mCanceled = TRUE;
    return NOERROR;
}

ECode AnimatorSet::AnimatorListenerAdapterIMPL::OnAnimationRepeat(
    /* [in] */ IAnimator* animation)
{
    return NOERROR;
}

//==============================================================================
//                  AnimatorSet
//==============================================================================
CAR_INTERFACE_IMPL(AnimatorSet, Animator, IAnimatorSet);

AnimatorSet::AnimatorSet()
    : mTerminated(FALSE)
    , mNeedsSort(TRUE)
    , mStarted(FALSE)
    , mStartDelay(0)
    , mDuration(-1)
    , mReversible(TRUE)
{
}

ECode AnimatorSet::constructor()
{
    return NOERROR;
}

AnimatorSet::~AnimatorSet()
{
}

ECode AnimatorSet::PlayTogether(
    /* [in] */ ArrayOf<IAnimator*>* items)
{
    if (items != NULL && items->GetLength() > 0) {
        mNeedsSort = TRUE;
        AutoPtr<IAnimatorSetBuilder> builder;
        Play((*items)[0], (IAnimatorSetBuilder**)&builder);
        for (Int32 i = 1; i < items->GetLength(); ++i) {
            builder->With((*items)[i]);
        }
    }
    return NOERROR;
}

ECode AnimatorSet::PlayTogether(
    /* [in] */ ICollection* items)
{
    if (items != NULL) {
        Int32 count;
        items->GetSize(&count);

        if (count > 0) {
            mNeedsSort = TRUE;
            AutoPtr<IAnimatorSetBuilder> builder;
            AutoPtr<IIterator> it;
            items->GetIterator((IIterator**)&it);
            Boolean hasNext = FALSE;
            while (it->HasNext(&hasNext), hasNext) {
                AutoPtr<IInterface> obj;
                it->GetNext((IInterface**)&obj);
                AutoPtr<IAnimator> anim = IAnimator::Probe(obj);
                if (builder == NULL) {
                    Play(anim, (IAnimatorSetBuilder**)&builder);
                }
                else {
                    builder->With(anim);
                }
            }
        }
    }
    return NOERROR;
}

ECode AnimatorSet::PlaySequentially(
    /* [in] */ ArrayOf<IAnimator*>* items)
{
    if (items != NULL) {
        mNeedsSort = TRUE;
        if (items->GetLength() == 1) {
            AutoPtr<IAnimatorSetBuilder> builder;
            Play((*items)[0], (IAnimatorSetBuilder**)&builder);
        }
        else {
            mReversible = FALSE;
            for (Int32 i = 0; i < items->GetLength() - 1; ++i) {
                AutoPtr<IAnimatorSetBuilder> builder;
                Play((*items)[i], (IAnimatorSetBuilder**)&builder);
                builder->Before((*items)[i+1]);
            }
        }
    }
    return NOERROR;
}

ECode AnimatorSet::PlaySequentially(
    /* [in] */ IList* items)
{
    Int32 size = 0;
    if (items != NULL && (items->GetSize(&size), size) > 0) {
        mNeedsSort = TRUE;
        if (size == 1) {
            AutoPtr<IInterface> obj;
            items->Get(0, (IInterface**)&obj);
            AutoPtr<IAnimator> animator = IAnimator::Probe(obj);
            AutoPtr<IAnimatorSetBuilder> as;
            Play(animator, (IAnimatorSetBuilder**)&as);
        }
        else {
            mReversible = FALSE;
            for (Int32 i = 0; i < size - 1; ++i) {
                AutoPtr<IInterface> obj;
                items->Get(i, (IInterface**)&obj);
                AutoPtr<IAnimator> animator = IAnimator::Probe(obj);
                obj = NULL;
                items->Get(i + 1, (IInterface**)&obj);
                AutoPtr<IAnimator> animator2 = IAnimator::Probe(obj);
                AutoPtr<IAnimatorSetBuilder> as;
                Play(animator, (IAnimatorSetBuilder**)&as);
                as->Before(animator2);
            }
        }
    }

    return NOERROR;
}

ECode AnimatorSet::GetChildAnimations(
    /* [out, callee] */ ArrayOf<IAnimator*>** childAnimations)
{
    VALIDATE_NOT_NULL(childAnimations);
    Int32 size = mNodes.GetSize();

    AutoPtr<ArrayOf<IAnimator*> > array = ArrayOf<IAnimator*>::Alloc(size);
    List<AutoPtr<Node> >::Iterator it = mNodes.Begin();
    Int32 i = 0;
    for (; it != mNodes.End(); ++it, ++i) {
        array->Set(i, (*it)->mAnimation);
    }
    *childAnimations = array;
    REFCOUNT_ADD(*childAnimations);
    return NOERROR;
}

ECode AnimatorSet::GetChildAnimations(
    /* [out] */ IArrayList** childAnimations)
{
    VALIDATE_NOT_NULL(childAnimations);
    AutoPtr<IArrayList> array;
    CArrayList::New((IArrayList**)&array);

    Int32 pos = 0;
    List<AutoPtr<Node> >::Iterator it = mNodes.Begin();
    for (; it != mNodes.End(); ++it) {
        array->Add(pos++, ((*it)->mAnimation).Get());
    }
    *childAnimations = array;
    REFCOUNT_ADD(*childAnimations);
    return NOERROR;
}

ECode AnimatorSet::SetTarget(
    /* [in] */ IInterface* target)
{
    List<AutoPtr<Node> >::Iterator it = mNodes.Begin();
    for (; it != mNodes.End(); ++it) {
        AutoPtr<IAnimator> animation = (*it)->mAnimation;
        if (IAnimatorSet::Probe(animation) || IObjectAnimator::Probe(animation)) {
            animation->SetTarget(target);
        }
    }
    return NOERROR;
}

ECode AnimatorSet::SetInterpolator(
    /* [in] */ ITimeInterpolator* interpolator)
{
    mInterpolator = interpolator;
    return NOERROR;
}

ECode AnimatorSet::GetInterpolator(
    /* [out] */ ITimeInterpolator** interpolator)
{
    VALIDATE_NOT_NULL(interpolator);
    *interpolator = mInterpolator;
    REFCOUNT_ADD(*interpolator);
    return NOERROR;
}

ECode AnimatorSet::Play(
    /* [in] */ IAnimator* anim,
    /* [out] */ IAnimatorSetBuilder** builder)
{
    VALIDATE_NOT_NULL(builder);
    if (anim != NULL) {
        mNeedsSort = TRUE;
        return CAnimatorSetBuilder::New(this, anim, builder);
    }

    *builder = NULL;
    return NOERROR;
}

ECode AnimatorSet::Cancel()
{
    AutoPtr<AnimatorSet> holdThis(this); // hold refcount of AnimationSet

    mTerminated = TRUE;
    Boolean started = FALSE;
    if (IsStarted(&started), started) {
        AutoPtr<IArrayList> tmpListeners;
        if (mListeners != NULL) {
            AutoPtr<IInterface> cloneObj;
            ICloneable::Probe(mListeners)->Clone((IInterface**)&cloneObj);
            tmpListeners = IArrayList::Probe(cloneObj);
            Int32 numListeners;
            tmpListeners->GetSize(&numListeners);
            for (Int32 i = 0; i < numListeners; ++i) {
                AutoPtr<IInterface> obj;
                tmpListeners->Get(i, (IInterface**)&obj);
                IAnimatorListener::Probe(obj)->OnAnimationCancel(this);
            }
        }
        Boolean running;
        if (mDelayAnim != NULL && (IAnimator::Probe(mDelayAnim)->IsRunning(&running), running)) {
            // If we're currently in the startDelay period, just cancel that animator and
            // send out the end event to all listeners
            IAnimator::Probe(mDelayAnim)->Cancel();
        }
        else  if (mSortedNodes.IsEmpty() == FALSE) {
            List<AutoPtr<Node> >::Iterator it = mSortedNodes.Begin();
            for (; it != mSortedNodes.End(); it++) {
                (*it)->mAnimation->Cancel();
            }
        }
        if (tmpListeners != NULL) {
            Int32 numListeners;
            tmpListeners->GetSize(&numListeners);
            for (Int32 i = 0; i < numListeners; ++i) {
                AutoPtr<IInterface> obj;
                tmpListeners->Get(i, (IInterface**)&obj);
                IAnimatorListener::Probe(obj)->OnAnimationEnd(this);
            }
        }
        mStarted = FALSE;
    }
    return NOERROR;
}

ECode AnimatorSet::End()
{
    AutoPtr<AnimatorSet> holdThis(this); // hold refcount of AnimationSet

    mTerminated = TRUE;
    Boolean started = FALSE;
    if (IsStarted(&started), started) {
        if (mSortedNodes.GetSize() != mNodes.GetSize()) {
            if (mSetListener == NULL) {
                mSetListener = new AnimatorSetListener();
                mSetListener->constructor(this);
            }

            // hasn't been started yet - sort the nodes now, then end them
            SortNodes();
            List<AutoPtr<Node> >::Iterator it = mSortedNodes.Begin();
            for (; it != mSortedNodes.End(); it++) {
                (*it)->mAnimation->AddListener(mSetListener);
            }
        }
        if (mDelayAnim != NULL) {
            IAnimator::Probe(mDelayAnim)->Cancel();
        }
        if (mSortedNodes.IsEmpty() == FALSE) {
            List<AutoPtr<Node> >::Iterator it = mSortedNodes.Begin();
            for (; it != mSortedNodes.End(); it++) {
                // hold refcount of AnimationSet
                ((Animator*)(*it)->mAnimation.Get())->SetParent(this);
                (*it)->mAnimation->End();
            }
        }
        if (mListeners != NULL) {
            AutoPtr<IInterface> cloneObj;
            ICloneable::Probe(mListeners)->Clone((IInterface**)&cloneObj);
            IArrayList* tmpListeners = IArrayList::Probe(cloneObj);
            Int32 numListeners;
            tmpListeners->GetSize(&numListeners);
            for (Int32 i = 0; i < numListeners; ++i) {
                AutoPtr<IInterface> obj;
                tmpListeners->Get(i, (IInterface**)&obj);
                IAnimatorListener::Probe(obj)->OnAnimationEnd(this);
            }
        }
        mStarted = FALSE;
    }
    return NOERROR;
}

ECode AnimatorSet::IsRunning(
    /* [out] */ Boolean* _running)
{
    VALIDATE_NOT_NULL(_running);
    List<AutoPtr<Node> >::Iterator it;
    for (it = mNodes.Begin(); it != mNodes.End(); it++) {
        Boolean running;
        (*it)->mAnimation->IsRunning(&running);
        if (running) {
            *_running = TRUE;
            return NOERROR;
        }
    }
    *_running = FALSE;
    return NOERROR;
}

ECode AnimatorSet::IsStarted(
    /* [out] */ Boolean* started)
{
    VALIDATE_NOT_NULL(started);
    *started = mStarted;
    return NOERROR;
}

ECode AnimatorSet::GetStartDelay(
    /* [out] */ Int64* delay)
{
    VALIDATE_NOT_NULL(delay);
    *delay = mStartDelay;
    return NOERROR;
}

ECode AnimatorSet::SetStartDelay(
    /* [in] */ Int64 startDelay)
{
    if (mStartDelay > 0) {
        mReversible = FALSE;
    }
    mStartDelay = startDelay;
    return NOERROR;
}

ECode AnimatorSet::GetDuration(
    /* [out] */ Int64* duration)
{
    VALIDATE_NOT_NULL(duration);
    *duration = mDuration;
    return NOERROR;
}

ECode AnimatorSet::SetDuration(
    /* [in] */ Int64 duration)
{
    if (duration < 0) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    // Just record the value for now - it will be used later when the AnimatorSet starts
    mDuration = duration;
    return NOERROR;
}

ECode AnimatorSet::SetupStartValues()
{
    List<AutoPtr<Node> >::Iterator it = mNodes.Begin();
    for (; it != mNodes.End(); it++) {
        (*it)->mAnimation->SetupStartValues();
    }
    return NOERROR;
}

ECode AnimatorSet::SetupEndValues()
{
    List<AutoPtr<Node> >::Iterator it = mNodes.Begin();
    for (; it != mNodes.End(); it++) {
        (*it)->mAnimation->SetupEndValues();
    }
    return NOERROR;
}

ECode AnimatorSet::Pause()
{
    Boolean previouslyPaused = mPaused;
    Animator::Pause();
    if (!previouslyPaused && mPaused) {
        if (mDelayAnim != NULL) {
            IAnimator::Probe(mDelayAnim)->Pause();
        }
        else {
            List<AutoPtr<Node> >::Iterator it = mNodes.Begin();
            for (; it != mNodes.End(); it++) {
                (*it)->mAnimation->Pause();
            }
        }
    }
    return NOERROR;
}

ECode AnimatorSet::Resume()
{
    Boolean previouslyPaused = mPaused;
    Animator::Resume();
    if (previouslyPaused && !mPaused) {
        if (mDelayAnim != NULL) {
            IAnimator::Probe(mDelayAnim)->Resume();
        }
        else {
            List<AutoPtr<Node> >::Iterator it = mNodes.Begin();
            for (; it != mNodes.End(); it++) {
                (*it)->mAnimation->Resume();
            }
        }
    }
    return NOERROR;
}

ECode AnimatorSet::Start()
{
    mTerminated = FALSE;
    mStarted = TRUE;
    mPaused = FALSE;

    List<AutoPtr<Node> >::Iterator it = mNodes.Begin();
    for (; it != mNodes.End(); it++) {
        (*it)->mAnimation->SetAllowRunningAsynchronously(FALSE);
    }

    if (mDuration >= 0) {
        // If the duration was set on this AnimatorSet, pass it along to all child animations
        List<AutoPtr<Node> >::Iterator it = mNodes.Begin();
        for (; it != mNodes.End(); it++) {
            // TODO: don't set the duration of the timing-only nodes created by AnimatorSet to
            // insert "play-after" delays
            (*it)->mAnimation->SetDuration(mDuration);
        }
    }

    if (mInterpolator != NULL) {
        List<AutoPtr<Node> >::Iterator it = mNodes.Begin();
        for (; it != mNodes.End(); it++) {
            (*it)->mAnimation->SetInterpolator(mInterpolator);
        }
    }

    // First, sort the nodes (if necessary). This will ensure that sortedNodes
    // contains the animation nodes in the correct order.
    SortNodes();

    it = mSortedNodes.Begin();
    for (; it!= mSortedNodes.End(); ++it) {
        AutoPtr<Node> node = *it;
        // First, clear out the old listeners
        AutoPtr<IArrayList> oldListeners;
        node->mAnimation->GetListeners((IArrayList**)&oldListeners);
        Int32 size;
        if (oldListeners != NULL && (oldListeners->GetSize(&size), size > 0)) {
            AutoPtr<IInterface> cloneObj;
            ICloneable::Probe(oldListeners)->Clone((IInterface**)&cloneObj);
            IArrayList* clonedListeners = IArrayList::Probe(cloneObj);
            Int32 numListeners;
            clonedListeners->GetSize(&numListeners);
            for (Int32 i = 0; i < numListeners; ++i) {
                AutoPtr<IInterface> obj;
                clonedListeners->Get(i, (IInterface**)&obj);
                IAnimatorListener* listener = IAnimatorListener::Probe(obj);
                if (IDependencyListener::Probe(listener) || IAnimatorSetListener::Probe(listener)) {
                    node->mAnimation->RemoveListener(listener);
                }
            }
        }
    }

    // nodesToStart holds the list of nodes to be started immediately. We don't want to
    // start the animations in the loop directly because we first need to set up
    // dependencies on all of the nodes. For example, we don't want to start an animation
    // when some other animation also wants to start when the first animation begins.
    AutoPtr< List<AutoPtr<Node> > > nodesToStart = new List<AutoPtr<Node> >;
    if (!mSortedNodes.IsEmpty()) {
        if (mSetListener == NULL) {
            mSetListener = new AnimatorSetListener();
            mSetListener->constructor(this);
        }

        for (it = mSortedNodes.Begin(); it != mSortedNodes.End(); ++it) {
            AutoPtr<Node> node = *it;
            if (node->mDependencies.IsEmpty()) {
                nodesToStart->PushBack(node);
            }
            else {
                List<AutoPtr<Dependency> >::Iterator dependencyIt= node->mDependencies.Begin();
                for (; dependencyIt != node->mDependencies.End(); ++dependencyIt) {
                    AutoPtr<Dependency> dependency = *dependencyIt;
                    AutoPtr<DependencyListener> lTemp = new DependencyListener(this, node, dependency->mRule);
                    dependency->mNode->mAnimation->AddListener(lTemp);
                }
                node->mTmpDependencies.Clear();
                node->mTmpDependencies.Assign(node->mDependencies.Begin(), node->mDependencies.End());
            }

            // hold refcount of AnimationSet
            ((Animator*)node->mAnimation.Get())->SetParent(this);
            node->mAnimation->AddListener(mSetListener);
        }
    }

    // Now that all dependencies are set up, start the animations that should be started.
    if (mStartDelay <= 0) {
        for (it = nodesToStart->Begin(); it != nodesToStart->End(); it++) {
            Animator* anim = (Animator*)((*it)->mAnimation.Get());
            anim->Start();
            mPlayingSet.PushBack((*it)->mAnimation);
        }
    }
    else {
        AutoPtr<ArrayOf<Float> > fArray = ArrayOf<Float>::Alloc(2);
        (*fArray)[0] = 0.0f; (*fArray)[1] = 1.0f;
        mDelayAnim = ValueAnimator::OfFloat(fArray);
        IAnimator* da = IAnimator::Probe(mDelayAnim);
        da->SetDuration(mStartDelay);
        AutoPtr<IAnimatorListener> aladapter = new AnimatorListenerAdapterIMPL(this, nodesToStart);
        da->AddListener(aladapter);
        da->Start();
    }

    if (mListeners != NULL) {
        AutoPtr<IInterface> cloneObj;
        ICloneable::Probe(mListeners)->Clone((IInterface**)&cloneObj);
        IArrayList* tmpListeners = IArrayList::Probe(cloneObj);
        Int32 numListeners;
        tmpListeners->GetSize(&numListeners);
        for (Int32 i = 0; i < numListeners; ++i) {
            AutoPtr<IInterface> obj;
            tmpListeners->Get(i, (IInterface**)&obj);
            IAnimatorListener::Probe(obj)->OnAnimationStart(this);
        }
    }
    if (mNodes.IsEmpty() && mStartDelay == 0) {
        // Handle unusual case where empty AnimatorSet is started - should send out
        // end event immediately since the event will not be sent out at all otherwise
        mStarted = FALSE;
        if (mListeners != NULL) {
            AutoPtr<IInterface> cloneObj;
            ICloneable::Probe(mListeners)->Clone((IInterface**)&cloneObj);
            IArrayList* tmpListeners = IArrayList::Probe(cloneObj);
            Int32 numListeners;
            tmpListeners->GetSize(&numListeners);
            for (Int32 i = 0; i < numListeners; ++i) {
                AutoPtr<IInterface> obj;
                tmpListeners->Get(i, (IInterface**)&obj);
                IAnimatorListener::Probe(obj)->OnAnimationEnd(this);
            }
        }
    }
    return NOERROR;
}

ECode AnimatorSet::Clone(
    /* [out] */ IInterface** object)
{
    VALIDATE_NOT_NULL(object)

    AutoPtr<IAnimatorSet> newObject = new CAnimatorSet();
    CloneImpl(newObject);
    *object = newObject;
    REFCOUNT_ADD(*object);
    return NOERROR;
}

ECode AnimatorSet::CloneImpl(
    /* [in] */ IAnimatorSet* object)
{
    Animator::CloneImpl(IAnimator::Probe(object));

    AutoPtr<AnimatorSet> anim = (AnimatorSet*)object;
    /*
     * The basic clone() operation copies all items. This doesn't work very well for
     * AnimatorSet, because it will copy references that need to be recreated and state
     * that may not apply. What we need to do now is put the clone in an uninitialized
     * state, with fresh, empty data structures. Then we will build up the nodes list
     * manually, as we clone each Node (and its animation). The clone will then be sorted,
     * and will populate any appropriate lists, when it is started.
     */
    anim->mNeedsSort = TRUE;
    anim->mTerminated = FALSE;
    anim->mStarted = FALSE;
    anim->mReversible = mReversible;
    anim->mSetListener = NULL;

    // Walk through the old nodes list, cloning each node and adding it to the new nodemap.
    // One problem is that the old node dependencies point to nodes in the old AnimatorSet.
    // We need to track the old/new nodes in order to reconstruct the dependencies in the clone.
    HashMap<AutoPtr<Node>, AutoPtr<Node> > nodeCloneMap; // <old, new>
    List<AutoPtr<Node> >::Iterator it = mNodes.Begin();
    for (; it != mNodes.End(); it++) {
        AutoPtr<Node> node = *it;
        AutoPtr<IInterface> tmp;
        node->Clone((IInterface**)&tmp);
        AutoPtr<Node> nodeClone = (Node*)ICloneable::Probe(tmp);
        nodeCloneMap[node] = nodeClone;
        anim->mNodes.PushBack(nodeClone);
        (anim->mNodeMap)[nodeClone->mAnimation] = nodeClone;
        // Clear out the dependencies in the clone; we'll set these up manually later
        nodeClone->mDependencies.Clear();
        nodeClone->mTmpDependencies.Clear();
        nodeClone->mNodeDependents.Clear();
        nodeClone->mNodeDependencies.Clear();
        // clear out any listeners that were set up by the AnimatorSet; these will
        // be set up when the clone's nodes are sorted
        AutoPtr<IArrayList> cloneListeners;
        nodeClone->mAnimation->GetListeners((IArrayList**)&cloneListeners);
        if (cloneListeners != NULL) {
            AutoPtr<IArrayList> listenersToRemove;
            Int32 size;
            cloneListeners->GetSize(&size);
            for (Int32 i = 0; i < size; ++i) {
                AutoPtr<IInterface> obj;
                cloneListeners->Get(i, (IInterface**)&obj);
                IAnimatorListener* listener = IAnimatorListener::Probe(obj);
                if (IAnimatorSetListener::Probe(listener) != NULL) {
                    if (listenersToRemove == NULL) {
                        CArrayList::New((IArrayList**)&listenersToRemove);
                    }
                    listenersToRemove->Add(listener);
                }
            }
            if (listenersToRemove != NULL) {
                Int32 numListeners;
                listenersToRemove->GetSize(&numListeners);
                for (Int32 i = 0; i < numListeners; ++i) {
                    AutoPtr<IInterface> listener;
                    listenersToRemove->Get(i, (IInterface**)&listener);
                    cloneListeners->Remove(listener);
                }
            }
        }
    }
    // Now that we've cloned all of the nodes, we're ready to walk through their
    // dependencies, mapping the old dependencies to the new nodes
    List<AutoPtr<Node> >::Iterator nodeIt = mNodes.Begin();
    for (; nodeIt != mNodes.End(); nodeIt++) {
        AutoPtr<Node> node = *nodeIt;
        AutoPtr<Node> nodeClone = nodeCloneMap[node];
        if (node->mDependencies.IsEmpty() == FALSE) {
            List<AutoPtr<Dependency> >::Iterator dependencyIt = node->mDependencies.Begin();
            for (; dependencyIt != node->mDependencies.End(); dependencyIt++) {
                AutoPtr<Dependency> dependency = *dependencyIt;
                AutoPtr<Node> clonedDependencyNode = nodeCloneMap[dependency->mNode];
                AutoPtr<Dependency> cloneDependency = new Dependency(clonedDependencyNode,
                        dependency->mRule);
                nodeClone->AddDependency(cloneDependency);
            }
        }
    }

    return NOERROR;
}

ECode AnimatorSet::SortNodes()
{
    if (mNeedsSort) {
        mSortedNodes.Clear();
        List<AutoPtr<Node> > roots;
        List<AutoPtr<Node> >::Iterator nodeIt = mNodes.Begin();
        for (; nodeIt != mNodes.End(); ++nodeIt) {
            AutoPtr<Node> node = (*nodeIt);
            if (node->mDependencies.IsEmpty()) {
                roots.PushBack(node);
            }
        }
        List<AutoPtr<Node> > tmpRoots;
        while (roots.IsEmpty() == FALSE) {
            List<AutoPtr<Node> >::Iterator rootsIt = roots.Begin();
            for (; rootsIt != roots.End(); ++rootsIt) {
                AutoPtr<Node> root = (*rootsIt);
                mSortedNodes.PushBack(root);
                if (root->mNodeDependents.IsEmpty() == FALSE) {
                    List<Node* >::Iterator itDependents = root->mNodeDependents.Begin();
                    for (; itDependents != root->mNodeDependents.End(); ++itDependents) {
                        AutoPtr<Node> node = *itDependents;
                        node->mNodeDependencies.Remove(root);
                        if (node->mNodeDependencies.IsEmpty()) {
                            tmpRoots.PushBack(node);
                        }
                    }
                }
            }
            roots.Clear();
            roots.Insert(roots.End(), tmpRoots.Begin(), tmpRoots.End());
            tmpRoots.Clear();
        }
        mNeedsSort = FALSE;
        if (mSortedNodes.GetSize() != mNodes.GetSize()) {
//            throw new IllegalStateException("Circular dependencies cannot exist"
//                    + " in AnimatorSet");
            return E_ILLEGAL_STATE_EXCEPTION;
        }
    }
    else {
        // Doesn't need sorting, but still need to add in the nodeDependencies list
        // because these get removed as the event listeners fire and the dependencies
        // are satisfied
        List<AutoPtr<Node> >::Iterator nodeIt = mNodes.Begin();
        for (; nodeIt != mNodes.End(); ++nodeIt) {
            AutoPtr<Node> node = *nodeIt;
            if (node->mDependencies.IsEmpty() == FALSE) {
                List<AutoPtr<Dependency> >::Iterator dependencyIt = node->mDependencies.Begin();
                for (; dependencyIt != node->mDependencies.End(); ++dependencyIt) {
                    AutoPtr<Dependency> dependency = *dependencyIt;
                    List<AutoPtr<Node> >::Iterator itFind =
                            Find(node->mNodeDependencies.Begin(), node->mNodeDependencies.End(), dependency->mNode);
                    if (itFind == node->mNodeDependencies.End()) {
                        node->mNodeDependencies.PushBack(dependency->mNode);
                    }
                }
            }
            // nodes are 'done' by default; they become un-done when started, and done
            // again when ended
            node->mDone = FALSE;
        }
    }
    return NOERROR;
}

ECode AnimatorSet::CanReverse(
    /* [out] */ Boolean* can)
{
    VALIDATE_NOT_NULL(can);
    *can = FALSE;
    if (!mReversible)  {
        return NOERROR;
    }
    // Loop to make sure all the Nodes can reverse.
    List<AutoPtr<Node> >::Iterator it = mNodes.Begin();
    for (; it != mNodes.End(); it++) {
        Boolean tmp = FALSE;
        Int64 delay = 0;
        if (!((*it)->mAnimation->CanReverse(&tmp), tmp) || ((*it)->mAnimation->GetStartDelay(&delay), delay) > 0) {
            return NOERROR;
        }
    }
    *can = TRUE;
    return NOERROR;
}

ECode AnimatorSet::Reverse()
{
    Boolean can = FALSE;
    if (CanReverse(&can), can) {
        List<AutoPtr<Node> >::Iterator it = mNodes.Begin();
        for (; it != mNodes.End(); it++) {
            (*it)->mAnimation->Reverse();
        }
    }
    return NOERROR;
}

}   //namespace Animation
}   //namespace Droid
}   //namespace Elastos
