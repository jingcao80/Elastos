
#include "elastos/droid/animation/CAnimatorSetBuilder.h"
#include "elastos/droid/animation/CValueAnimator.h"

namespace Elastos {
namespace Droid {
namespace Animation {

CAR_OBJECT_IMPL(CAnimatorSetBuilder);
CAR_INTERFACE_IMPL(CAnimatorSetBuilder, Object, IAnimatorSetBuilder);

ECode CAnimatorSetBuilder::constructor(
    /* [in] */ IAnimatorSet* host,
    /* [in] */ IAnimator* anim)
{
    mHost = (AnimatorSet*)host;
    AutoPtr<IAnimator> key = anim;
    mCurrentNode = mHost->mNodeMap[key];
    if(mCurrentNode == NULL)
    {
        mCurrentNode = new AnimatorSet::Node(anim);
        mHost->mNodeMap[key] = mCurrentNode;
        mHost->mNodes.PushBack(mCurrentNode);
    }
    return NOERROR;
}

ECode CAnimatorSetBuilder::With(
    /* [in] */ IAnimator* anim)
{
    assert(mHost != NULL);
    AutoPtr<IAnimator> key = anim;
    AutoPtr<AnimatorSet::Node> node = mHost->mNodeMap[key];
    if (node == NULL) {
        node = new AnimatorSet::Node(anim);
        mHost->mNodeMap[key] = node;
        mHost->mNodes.PushBack(node);
    }

    AutoPtr<AnimatorSet::Dependency> dependency = new AnimatorSet::Dependency(
            mCurrentNode, AnimatorSet::Dependency::WITH);
    node->AddDependency(dependency);
    return NOERROR;
}

ECode CAnimatorSetBuilder::Before(
    /* [in] */ IAnimator* anim)
{
    assert(mHost != NULL);
    mHost->mReversible = FALSE;
    AutoPtr<IAnimator> key = anim;
    AutoPtr<AnimatorSet::Node> node = mHost->mNodeMap[key];
    if (node == NULL) {
        node = new AnimatorSet::Node(anim);
        mHost->mNodeMap[key] = node;
        mHost->mNodes.PushBack(node);
    }

    AutoPtr<AnimatorSet::Dependency> dependency = new AnimatorSet::Dependency(
            mCurrentNode, AnimatorSet::Dependency::AFTER);
    node->AddDependency(dependency);
    return NOERROR;
}

ECode CAnimatorSetBuilder::After(
    /* [in] */ IAnimator* anim)
{
    assert(mHost != NULL);
    mHost->mReversible = FALSE;
    AutoPtr<IAnimator> key = anim;
    AutoPtr<AnimatorSet::Node> node = mHost->mNodeMap[key];
    if (node == NULL) {
        node = new AnimatorSet::Node(anim);
        mHost->mNodeMap[key] = node;
        mHost->mNodes.PushBack(node);
    }

    AutoPtr<AnimatorSet::Dependency> dependency = new AnimatorSet::Dependency(
            node, AnimatorSet::Dependency::AFTER);
    mCurrentNode->AddDependency(dependency);
    return NOERROR;
}

ECode CAnimatorSetBuilder::After(
       /* [in] */ Int64 delay)
{
    assert(mHost != NULL);
    // setup dummy ValueAnimator just to run the clock
    AutoPtr<ArrayOf<Float> > values = ArrayOf<Float>::Alloc(2);
    (*values)[0] = 0.0f;
    (*values)[1] = 1.0f;
    AutoPtr<IValueAnimator> anim = CValueAnimator::OfFloat(values);
    IAnimator::Probe(anim)->SetDuration(delay);
    return After(IAnimator::Probe(anim));
}

}   //namespace Animation
}   //namespace Droid
}   //namespace Elastos
