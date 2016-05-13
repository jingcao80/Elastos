
#include "Elastos.Droid.Content.h"
#include "elastos/droid/widget/RelativeLayout.h"
#include "elastos/droid/widget/CRelativeLayoutLayoutParams.h"
#include "elastos/droid/graphics/CRect.h"
#include "elastos/droid/R.h"
#include "elastos/droid/os/Build.h"
#include "elastos/droid/view/Gravity.h"
#include "elastos/droid/view/ViewGroup.h"
#include <elastos/core/Math.h>
#include <elastos/utility/logging/Slogger.h>

using Elastos::Droid::Content::Pm::IApplicationInfo;
using Elastos::Droid::Graphics::CRect;
using Elastos::Droid::Os::Build;
using Elastos::Droid::View::Accessibility::IAccessibilityRecord;
using Elastos::Droid::View::Gravity;
using Elastos::Droid::View::IGravity;
using Elastos::Droid::View::IView;
using Elastos::Core::CString;
using Elastos::Core::EIID_IComparator;
using Elastos::Utility::Logging::Slogger;

namespace Elastos {
namespace Droid {
namespace Widget {

// {9cf99c13-56c5-4194-ab03-a862cee2ab06}
extern "C" const InterfaceID EIID_NODE =
    { 0x9cf99c13, 0x56c5, 0x4194, { 0xab, 0x03, 0xa8, 0x62, 0xce, 0xe2, 0xab, 0x06 } };

static String TAG("RelativeLayout");

//==============================================================================
//  TopToBottomLeftToRightComparator
//==============================================================================
CAR_INTERFACE_IMPL(TopToBottomLeftToRightComparator, Object, IComparator)
ECode TopToBottomLeftToRightComparator::Compare(
    /* [in] */ IInterface* lhs,
    /* [in] */ IInterface* rhs,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = 0;

    IView* first = IView::Probe(lhs);
    IView* second = IView::Probe(rhs);

    VALIDATE_NOT_NULL(first);
    VALIDATE_NOT_NULL(second);

    Int32 iFirst, iSecond;

    // top - bottom
    first->GetTop(&iFirst);
    second->GetTop(&iSecond);
    Int32 topDifference = iFirst - iSecond;
    if (topDifference != 0) {
        *result = topDifference;
        return NOERROR;
    }
    // left - right
    first->GetLeft(&iFirst);
    second->GetLeft(&iSecond);
    Int32 leftDifference = iFirst - iSecond;
    if (leftDifference != 0) {
        *result = leftDifference;
        return NOERROR;
    }
    // break tie by height
    first->GetHeight(&iFirst);
    second->GetHeight(&iSecond);
    Int32 heightDiference = iFirst - iSecond;
    if (heightDiference != 0) {
        *result = heightDiference;
        return NOERROR;
    }
    // break tie by width
    first->GetWidth(&iFirst);
    second->GetWidth(&iSecond);
    Int32 widthDiference = iFirst - iSecond;
    if (widthDiference != 0) {
        *result = widthDiference;
        return NOERROR;
    }

    return NOERROR;
}

//==============================================================================
//  Node
//==============================================================================
const Int32 Node::POOL_LIMIT = 100;
AutoPtr<Pools::SynchronizedPool<Node> > Node::sPool = new Pools::SynchronizedPool<Node>(POOL_LIMIT);
AutoPtr<Node> Node::Acquire(
    /* [in] */ IView* view)
{
    AutoPtr<Node> node = sPool->AcquireItem();
    if (node == NULL) {
        node = new Node();
    }
    node->mView = view;
    return node;
}

void Node::Clear()
{
    mView = NULL;
    mDependents.Clear();
    mDependencies.Clear();

    sPool->ReleaseItem(this);
}

//==============================================================================
//  DependencyGraph
//==============================================================================

DependencyGraph::DependencyGraph()
{
}

void DependencyGraph::Clear()
{
    List< AutoPtr<Node> >::Iterator it;
    for (it = mNodes.Begin(); it != mNodes.End(); ++it) {
        (*it)->Clear();
    }
    mNodes.Clear();

    mKeyNodes.Clear();
    mRoots.Clear();
}

void DependencyGraph::Add(
    /* [in] */ IView* view)
{
    assert(view != NULL);

    Int32 id;
    view->GetId(&id);
    AutoPtr<Node> node = Node::Acquire(view);

    if (id != IView::NO_ID) {
        mKeyNodes[id] = node;
    }

    mNodes.PushBack(node);
}

ECode DependencyGraph::GetSortedViews(
    /* [in] */ ArrayOf<IView*>* sorted,
    /* [in] */ ArrayOf<Int32>* rules)
{
    VALIDATE_NOT_NULL(sorted);

    List< AutoPtr<Node> >& roots = FindRoots(rules);
    Int32 index = 0;
    while (!roots.IsEmpty()) {
        AutoPtr<Node> node = roots.GetBack();
        roots.PopBack();

        IView* view = node->mView;
        Int32 key;
        view->GetId(&key);
        assert(view);
        sorted->Set(index, view);
        index++;

        Node::DependentsIterator it = node->mDependents.Begin();
        for (; it != node->mDependents.End(); ++it) {
            AutoPtr<Node> dependent = it->mFirst;
            HashMap<Int32, Node* >* dependencies = &dependent->mDependencies;
            dependencies->Erase(key);
            if (dependencies->IsEmpty()) {
                roots.PushBack(dependent);
            }
        }
    }

    if (index < sorted->GetLength()) {
        Slogger::E(TAG, "Circular dependencies cannot exist in RelativeLayout");
        return E_ILLEGAL_STATE_EXCEPTION;
    }

    return NOERROR;
}

/**
 * Finds the roots of the graph. A root is a node with no dependency and
 * with [0..n] dependents.
 *
 * @param rulesFilter The list of rules to consider when building the
 *        dependencies
 *
 * @return A list of node, each being a root of the graph
 */
List< AutoPtr<Node> >& DependencyGraph::FindRoots(
    /* [in] */ ArrayOf<Int32>* rulesFilter)
{
    assert(rulesFilter != NULL);
    List< AutoPtr<Node> >::Iterator it;

    // Find roots can be invoked several times, so make sure to clear
    // all dependents and dependencies before running the algorithm
    for (it = mNodes.Begin(); it != mNodes.End(); ++it) {
        AutoPtr<Node> node = *it;
        node->mDependents.Clear();
        node->mDependencies.Clear();
    }

    // Builds up the dependents and dependencies for each node of the graph
    for (it = mNodes.Begin(); it != mNodes.End(); ++it) {
        AutoPtr<Node> node = *it;

        AutoPtr<IViewGroupLayoutParams> vglp;
        node->mView->GetLayoutParams((IViewGroupLayoutParams**)&vglp);
        IRelativeLayoutLayoutParams* layoutParams = IRelativeLayoutLayoutParams::Probe(vglp);
        AutoPtr<ArrayOf<Int32> > rules;
        layoutParams->GetRules((ArrayOf<Int32>**)&rules);
        Int32 rulesCount = rulesFilter->GetLength();

        // Look only the the rules passed in parameter, this way we build only the
        // dependencies for a specific set of rules
        for (Int32 j = 0; j < rulesCount; j++) {
            Int32 rule = (*rules)[(*rulesFilter)[j]];
            if (rule > 0) {
                // The node this node depends on
                HashMap<Int32, AutoPtr<Node> >::Iterator it = mKeyNodes.Find(rule);
                AutoPtr<Node> dependency;
                if (it != mKeyNodes.End())
                   dependency = it->mSecond;

                // Skip unknowns and self dependencies
                if (dependency == NULL || dependency == node) {
                    continue;
                }
                // Add the current node as a dependent
                dependency->mDependents.Insert(Node::DependentsValueType(node, this));
                // Add a dependency to the current node
                node->mDependencies[rule] = dependency;
            }
        }
    }

    mRoots.Clear();

    // Finds all the roots in the graph: all nodes with no dependencies
    for (it = mNodes.Begin(); it != mNodes.End(); ++it) {
        AutoPtr<Node> node = *it;
        if (node->mDependencies.IsEmpty()) {
            mRoots.PushBack(node);
        }
    }
    return mRoots;
}

CAR_INTERFACE_IMPL(RelativeLayout::LayoutParams, ViewGroup::MarginLayoutParams, IRelativeLayoutLayoutParams);
RelativeLayout::LayoutParams::LayoutParams()
    : mLeft(0)
    , mTop(0)
    , mRight(0)
    , mBottom(0)
    , mRulesChanged(FALSE)
    , mIsRtlCompatibilityMode(FALSE)
    , mAlignWithParent(FALSE)
{
    mRules = ArrayOf<Int32>::Alloc(IRelativeLayout::VERB_COUNT);
    mInitialRules = ArrayOf<Int32>::Alloc(IRelativeLayout::VERB_COUNT);
}

ECode RelativeLayout::LayoutParams::constructor(
    /* [in] */ IContext* c,
    /* [in] */ IAttributeSet* attrs)
{
    ASSERT_SUCCEEDED(MarginLayoutParams::constructor(c, attrs));
    ASSERT_SUCCEEDED(InitFromAttributes(c, attrs));
    return NOERROR;
}

ECode RelativeLayout::LayoutParams::constructor(
    /* [in] */ Int32 width,
    /* [in] */ Int32 height)
{
    ASSERT_SUCCEEDED(MarginLayoutParams::constructor(width, height));
    return NOERROR;
}

ECode RelativeLayout::LayoutParams::constructor(
    /* [in] */ IViewGroupLayoutParams* source)
{
    ASSERT_SUCCEEDED(MarginLayoutParams::constructor(source));
    return NOERROR;
}

ECode RelativeLayout::LayoutParams::constructor(
    /* [in] */ IViewGroupMarginLayoutParams* source)
{
    ASSERT_SUCCEEDED(MarginLayoutParams::constructor(source));
    return NOERROR;
}

ECode RelativeLayout::LayoutParams::constructor(
    /* [in] */ IRelativeLayoutLayoutParams* _source)
{
    ASSERT_SUCCEEDED(MarginLayoutParams::constructor(IViewGroupMarginLayoutParams::Probe(_source)));

    LayoutParams* source = (LayoutParams*)_source;
    mIsRtlCompatibilityMode = source->mIsRtlCompatibilityMode;
    mRulesChanged = source->mRulesChanged;
    mAlignWithParent = source->mAlignWithParent;

    mRules->Copy(IRelativeLayout::LEFT_OF, source->mRules, IRelativeLayout::LEFT_OF, IRelativeLayout::VERB_COUNT);
    mInitialRules->Copy(IRelativeLayout::LEFT_OF, source->mInitialRules, IRelativeLayout::LEFT_OF, IRelativeLayout::VERB_COUNT);
    return NOERROR;
}

ECode RelativeLayout::LayoutParams::InitFromAttributes(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    AutoPtr<ArrayOf<Int32> > attrIds = ArrayOf<Int32>::Alloc(
            const_cast<Int32 *>(R::styleable::RelativeLayout_Layout),
            ArraySize(R::styleable::RelativeLayout_Layout));
    AutoPtr<ITypedArray> a;
    ASSERT_SUCCEEDED(context->ObtainStyledAttributes(
            attrs, attrIds, (ITypedArray**)&a));

    AutoPtr<IApplicationInfo> info;
    context->GetApplicationInfo((IApplicationInfo**)&info);
    Int32 targetSdkVersion = 0;
    info->GetTargetSdkVersion(&targetSdkVersion);
    Boolean isSupport = FALSE;
    mIsRtlCompatibilityMode = (targetSdkVersion < Build::VERSION_CODES::JELLY_BEAN_MR1 ||
            (info->HasRtlSupport(&isSupport), !isSupport));

    Int32* rules = mRules->GetPayload();
    //noinspection MismatchedReadAndWriteOfArray
    // Int32* initialRules = mInitialRules->GetPayload();

    Int32 N;
    a->GetIndexCount(&N);
    for (Int32 i = 0; i < N; i++) {
        Int32 attr;
        a->GetIndex(i, &attr);
        Boolean val = FALSE;
        switch (attr) {
            case R::styleable::RelativeLayout_Layout_layout_alignWithParentIfMissing:
                a->GetBoolean(attr, FALSE, &mAlignWithParent);
                break;
            case R::styleable::RelativeLayout_Layout_layout_toLeftOf:
                a->GetResourceId(attr, 0, rules + IRelativeLayout::LEFT_OF);
                break;
            case R::styleable::RelativeLayout_Layout_layout_toRightOf:
                a->GetResourceId(attr, 0, rules + IRelativeLayout::RIGHT_OF);
                break;
            case R::styleable::RelativeLayout_Layout_layout_above:
                a->GetResourceId(attr, 0, rules + IRelativeLayout::ABOVE);
                break;
            case R::styleable::RelativeLayout_Layout_layout_below:
                a->GetResourceId(attr, 0, rules + IRelativeLayout::BELOW);
                break;
            case R::styleable::RelativeLayout_Layout_layout_alignBaseline:
                a->GetResourceId(attr, 0, rules + IRelativeLayout::ALIGN_BASELINE);
                break;
            case R::styleable::RelativeLayout_Layout_layout_alignLeft:
                a->GetResourceId(attr, 0, rules + IRelativeLayout::ALIGN_LEFT);
                break;
            case R::styleable::RelativeLayout_Layout_layout_alignTop:
                a->GetResourceId(attr, 0, rules + IRelativeLayout::ALIGN_TOP);
                break;
            case R::styleable::RelativeLayout_Layout_layout_alignRight:
                a->GetResourceId(attr, 0, rules + IRelativeLayout::ALIGN_RIGHT);
                break;
            case R::styleable::RelativeLayout_Layout_layout_alignBottom:
                a->GetResourceId(attr, 0, rules + IRelativeLayout::ALIGN_BOTTOM);
                break;
            case R::styleable::RelativeLayout_Layout_layout_alignParentLeft:
                a->GetBoolean(attr, FALSE, &val);
                *(rules + IRelativeLayout::ALIGN_PARENT_LEFT) = val ? 1 : 0;
                break;
            case R::styleable::RelativeLayout_Layout_layout_alignParentTop:
                a->GetBoolean(attr, FALSE, &val);
                *(rules + IRelativeLayout::ALIGN_PARENT_TOP) = val ? 1 : 0;
                break;
            case R::styleable::RelativeLayout_Layout_layout_alignParentRight:
                a->GetBoolean(attr, FALSE, &val);
                *(rules + IRelativeLayout::ALIGN_PARENT_RIGHT) = val ? 1 : 0;
                break;
            case R::styleable::RelativeLayout_Layout_layout_alignParentBottom:
                a->GetBoolean(attr, FALSE, &val);
                *(rules + IRelativeLayout::ALIGN_PARENT_BOTTOM) = val ? 1 : 0;
                break;
            case R::styleable::RelativeLayout_Layout_layout_centerInParent:
                a->GetBoolean(attr, FALSE, &val);
                *(rules + IRelativeLayout::CENTER_IN_PARENT) = val ? 1 : 0;
                break;
            case R::styleable::RelativeLayout_Layout_layout_centerHorizontal:
                a->GetBoolean(attr, FALSE, &val);
                *(rules + IRelativeLayout::CENTER_HORIZONTAL) = val ? 1 : 0;
                break;
            case R::styleable::RelativeLayout_Layout_layout_centerVertical:
                a->GetBoolean(attr, FALSE, &val);
                *(rules + IRelativeLayout::CENTER_VERTICAL) = val ? 1 : 0;
                break;
            case R::styleable::RelativeLayout_Layout_layout_toStartOf:
                a->GetResourceId(attr, 0, rules + IRelativeLayout::START_OF);
                break;
            case R::styleable::RelativeLayout_Layout_layout_toEndOf:
                a->GetResourceId(attr, 0, rules + IRelativeLayout::END_OF);
                break;
            case R::styleable::RelativeLayout_Layout_layout_alignStart:
                a->GetResourceId(attr, 0, rules + IRelativeLayout::END_OF);
                break;
            case R::styleable::RelativeLayout_Layout_layout_alignEnd:
                a->GetResourceId(attr, 0, rules + IRelativeLayout::ALIGN_END);
                break;
            case R::styleable::RelativeLayout_Layout_layout_alignParentStart:
                a->GetBoolean(attr, FALSE, &val);
                *(rules + IRelativeLayout::ALIGN_PARENT_START) = val ? 1 : 0;
                break;
            case R::styleable::RelativeLayout_Layout_layout_alignParentEnd:
                a->GetBoolean(attr, FALSE, &val);
                *(rules + IRelativeLayout::ALIGN_PARENT_END) = val ? 1 : 0;
                break;
        }
    }

    mRulesChanged = TRUE;
    mInitialRules->Copy(IRelativeLayout::LEFT_OF, mRules, IRelativeLayout::LEFT_OF, IRelativeLayout::VERB_COUNT);

    a->Recycle();
    return NOERROR;
}

ECode RelativeLayout::LayoutParams::AddRule(
    /* [in] */ Int32 verb)
{
    if (verb < 0 || verb >= mRules->GetLength())
        return E_ILLEGAL_ARGUMENT_EXCEPTION;

    (*mRules)[verb] = IRelativeLayout::RelativeLayout_TRUE;
    (*mInitialRules)[verb] = IRelativeLayout::RelativeLayout_TRUE;
    mRulesChanged = TRUE;

    return NOERROR;
}

ECode RelativeLayout::LayoutParams::AddRule(
    /* [in] */ Int32 verb,
    /* [in] */ Int32 anchor)
{
    if (verb < 0 || verb >= mRules->GetLength())
        return E_ILLEGAL_ARGUMENT_EXCEPTION;

    (*mRules)[verb] = anchor;
    (*mInitialRules)[verb] = anchor;
    mRulesChanged = TRUE;
    return NOERROR;
}

ECode RelativeLayout::LayoutParams::RemoveRule(
    /* [in] */ Int32 verb)
{
    if (verb < 0 || verb >= mRules->GetLength())
        return E_ILLEGAL_ARGUMENT_EXCEPTION;

    (*mRules)[verb] = 0;
    (*mInitialRules)[verb] = 0;
    mRulesChanged = TRUE;
    return NOERROR;
}

Boolean RelativeLayout::LayoutParams::HasRelativeRules()
{
    return ((*mInitialRules)[IRelativeLayout::START_OF] != 0
            || (*mInitialRules)[IRelativeLayout::END_OF] != 0
            || (*mInitialRules)[IRelativeLayout::ALIGN_START] != 0
            || (*mInitialRules)[IRelativeLayout::ALIGN_END] != 0
            || (*mInitialRules)[IRelativeLayout::ALIGN_PARENT_START] != 0
            || (*mInitialRules)[IRelativeLayout::ALIGN_PARENT_END] != 0);
}

void RelativeLayout::LayoutParams::ResolveRules(
    /* [in] */ Int32 layoutDirection)
{
    Boolean isLayoutRtl = (layoutDirection == IView::LAYOUT_DIRECTION_RTL);
    // Reset to initial state
    mRules->Copy(IRelativeLayout::LEFT_OF, mInitialRules, IRelativeLayout::LEFT_OF, IRelativeLayout::VERB_COUNT);

    // Apply rules depending on direction and if we are in RTL compatibility mode
    if (mIsRtlCompatibilityMode) {
        if ((*mRules)[IRelativeLayout::ALIGN_START] != 0) {
            if ((*mRules)[IRelativeLayout::ALIGN_LEFT] == 0) {
                // "left" rule is not defined but "start" rule is: use the "start" rule as
                // the "left" rule
                (*mRules)[IRelativeLayout::ALIGN_LEFT] = (*mRules)[IRelativeLayout::ALIGN_START];
            }
            (*mRules)[IRelativeLayout::ALIGN_START] = 0;
        }

        if ((*mRules)[IRelativeLayout::ALIGN_END] != 0) {
            if ((*mRules)[IRelativeLayout::ALIGN_RIGHT] == 0) {
                // "right" rule is not defined but "end" rule is: use the "end" rule as the
                // "right" rule
                (*mRules)[IRelativeLayout::ALIGN_RIGHT] = (*mRules)[IRelativeLayout::ALIGN_END];
            }
            (*mRules)[IRelativeLayout::ALIGN_END] = 0;
        }

        if ((*mRules)[IRelativeLayout::START_OF] != 0) {
            if ((*mRules)[IRelativeLayout::LEFT_OF] == 0) {
                // "left" rule is not defined but "start" rule is: use the "start" rule as
                // the "left" rule
                (*mRules)[IRelativeLayout::LEFT_OF] = (*mRules)[IRelativeLayout::START_OF];
            }
            (*mRules)[IRelativeLayout::START_OF] = 0;
        }

        if ((*mRules)[IRelativeLayout::END_OF] != 0) {
            if ((*mRules)[IRelativeLayout::RIGHT_OF] == 0) {
                // "right" rule is not defined but "end" rule is: use the "end" rule as the
                // "right" rule
                (*mRules)[IRelativeLayout::RIGHT_OF] = (*mRules)[IRelativeLayout::END_OF];
            }
            (*mRules)[IRelativeLayout::END_OF] = 0;
        }

        if ((*mRules)[IRelativeLayout::ALIGN_PARENT_START] != 0) {
            if ((*mRules)[IRelativeLayout::ALIGN_PARENT_LEFT] == 0) {
                // "left" rule is not defined but "start" rule is: use the "start" rule as
                // the "left" rule
                (*mRules)[IRelativeLayout::ALIGN_PARENT_LEFT] = (*mRules)[IRelativeLayout::ALIGN_PARENT_START];
            }
            (*mRules)[IRelativeLayout::ALIGN_PARENT_START] = 0;
        }

        if ((*mRules)[IRelativeLayout::ALIGN_PARENT_END] != 0) {
            if ((*mRules)[IRelativeLayout::ALIGN_PARENT_RIGHT] == 0) {
                // "right" rule is not defined but "end" rule is: use the "end" rule as the
                // "right" rule
                (*mRules)[IRelativeLayout::ALIGN_PARENT_RIGHT] = (*mRules)[IRelativeLayout::ALIGN_PARENT_END];
            }
            (*mRules)[IRelativeLayout::ALIGN_PARENT_END] = 0;
        }
    } else {
        // JB MR1+ case
        if (((*mRules)[IRelativeLayout::ALIGN_START] != 0 || (*mRules)[IRelativeLayout::ALIGN_END] != 0) &&
                ((*mRules)[IRelativeLayout::ALIGN_LEFT] != 0 || (*mRules)[IRelativeLayout::ALIGN_RIGHT] != 0)) {
            // "start"/"end" rules take precedence over "left"/"right" rules
            (*mRules)[IRelativeLayout::ALIGN_LEFT] = 0;
            (*mRules)[IRelativeLayout::ALIGN_RIGHT] = 0;
        }
        if ((*mRules)[IRelativeLayout::ALIGN_START] != 0) {
            // "start" rule resolved to "left" or "right" depending on the direction
            (*mRules)[isLayoutRtl ? IRelativeLayout::ALIGN_RIGHT : IRelativeLayout::ALIGN_LEFT] = (*mRules)[IRelativeLayout::ALIGN_START];
            (*mRules)[IRelativeLayout::ALIGN_START] = 0;
        }
        if ((*mRules)[IRelativeLayout::ALIGN_END] != 0) {
            // "end" rule resolved to "left" or "right" depending on the direction
            (*mRules)[isLayoutRtl ? IRelativeLayout::ALIGN_LEFT : IRelativeLayout::ALIGN_RIGHT] = (*mRules)[IRelativeLayout::ALIGN_END];
            (*mRules)[IRelativeLayout::ALIGN_END] = 0;
        }

        if (((*mRules)[IRelativeLayout::START_OF] != 0 || (*mRules)[IRelativeLayout::END_OF] != 0) &&
                ((*mRules)[IRelativeLayout::LEFT_OF] != 0 || (*mRules)[IRelativeLayout::RIGHT_OF] != 0)) {
            // "start"/"end" rules take precedence over "left"/"right" rules
            (*mRules)[IRelativeLayout::LEFT_OF] = 0;
            (*mRules)[IRelativeLayout::RIGHT_OF] = 0;
        }
        if ((*mRules)[IRelativeLayout::START_OF] != 0) {
            // "start" rule resolved to "left" or "right" depending on the direction
            (*mRules)[isLayoutRtl ? IRelativeLayout::RIGHT_OF : IRelativeLayout::LEFT_OF] = (*mRules)[IRelativeLayout::START_OF];
            (*mRules)[IRelativeLayout::START_OF] = 0;
        }
        if ((*mRules)[IRelativeLayout::END_OF] != 0) {
            // "end" rule resolved to "left" or "right" depending on the direction
            (*mRules)[isLayoutRtl ? IRelativeLayout::LEFT_OF : IRelativeLayout::RIGHT_OF] = (*mRules)[IRelativeLayout::END_OF];
            (*mRules)[IRelativeLayout::END_OF] = 0;
        }

        if (((*mRules)[IRelativeLayout::ALIGN_PARENT_START] != 0 || (*mRules)[IRelativeLayout::ALIGN_PARENT_END] != 0) &&
                ((*mRules)[IRelativeLayout::ALIGN_PARENT_LEFT] != 0 || (*mRules)[IRelativeLayout::ALIGN_PARENT_RIGHT] != 0)) {
            // "start"/"end" rules take precedence over "left"/"right" rules
            (*mRules)[IRelativeLayout::ALIGN_PARENT_LEFT] = 0;
            (*mRules)[IRelativeLayout::ALIGN_PARENT_RIGHT] = 0;
        }
        if ((*mRules)[IRelativeLayout::ALIGN_PARENT_START] != 0) {
            // "start" rule resolved to "left" or "right" depending on the direction
            (*mRules)[isLayoutRtl ? IRelativeLayout::ALIGN_PARENT_RIGHT : IRelativeLayout::ALIGN_PARENT_LEFT]
                    = (*mRules)[IRelativeLayout::ALIGN_PARENT_START];
            (*mRules)[IRelativeLayout::ALIGN_PARENT_START] = 0;
        }
        if ((*mRules)[IRelativeLayout::ALIGN_PARENT_END] != 0) {
            // "end" rule resolved to "left" or "right" depending on the direction
            (*mRules)[isLayoutRtl ? IRelativeLayout::ALIGN_PARENT_LEFT : IRelativeLayout::ALIGN_PARENT_RIGHT] = (*mRules)[IRelativeLayout::ALIGN_PARENT_END];
            (*mRules)[IRelativeLayout::ALIGN_PARENT_END] = 0;
        }
    }
    mRulesChanged = FALSE;
}

ECode RelativeLayout::LayoutParams::GetRules(
    /* [in] */ Int32 layoutDirection,
    /* [out, callee] */ ArrayOf<Int32>** rules)
{
    VALIDATE_NOT_NULL(rules);
    Int32 ld = 0;
    if (HasRelativeRules() &&
            (mRulesChanged || layoutDirection != (GetLayoutDirection(&ld), ld))) {
        ResolveRules(layoutDirection);
        if (layoutDirection != ld) {
            SetLayoutDirection(layoutDirection);
        }
    }
    *rules = mRules;
    REFCOUNT_ADD(*rules);
    return NOERROR;
}

ECode RelativeLayout::LayoutParams::GetRules(
    /* [out, callee] */ ArrayOf<Int32>** rules)
{
    VALIDATE_NOT_NULL(rules);
    *rules = mRules;
    REFCOUNT_ADD(*rules);
    return NOERROR;
}

ECode RelativeLayout::LayoutParams::ResolveLayoutDirection(
    /* [in] */ Int32 layoutDirection)
{
    Boolean isLayoutRtl = FALSE;
    IsLayoutRtl(&isLayoutRtl);

    Int32 ld = 0;
    if (HasRelativeRules() && layoutDirection != (GetLayoutDirection(&ld), ld)) {
        ResolveRules(layoutDirection);
    }
    // This will set the layout direction
    return MarginLayoutParams::ResolveLayoutDirection(layoutDirection);
}

ECode RelativeLayout::LayoutParams::SetAlignWithParent(
    /* [in] */ Boolean align)
{
    mAlignWithParent = align;
    return NOERROR;
}

ECode RelativeLayout::LayoutParams::GetAlignWithParent(
    /* [out] */ Boolean* align)
{
    VALIDATE_NOT_NULL(align);
    *align = mAlignWithParent;
    return NOERROR;
}

//==============================================================================
//  DependencyGraph
//==============================================================================

static AutoPtr<ArrayOf<Int32> > InitRULES_VERTICAL()
{
    AutoPtr<ArrayOf<Int32> > vRules = ArrayOf<Int32>::Alloc(5);
    (*vRules)[0] = IRelativeLayout::ABOVE;
    (*vRules)[1] = IRelativeLayout::BELOW;
    (*vRules)[2] = IRelativeLayout::ALIGN_BASELINE;
    (*vRules)[3] = IRelativeLayout::ALIGN_TOP;
    (*vRules)[4] = IRelativeLayout::ALIGN_BOTTOM;
    return vRules;
}

static AutoPtr<ArrayOf<Int32> > InitRULES_HORIZONTAL()
{
    AutoPtr<ArrayOf<Int32> > hRules = ArrayOf<Int32>::Alloc(8);
    (*hRules)[0] = IRelativeLayout::LEFT_OF;
    (*hRules)[1] = IRelativeLayout::RIGHT_OF;
    (*hRules)[2] = IRelativeLayout::ALIGN_LEFT;
    (*hRules)[3] = IRelativeLayout::ALIGN_RIGHT;
    (*hRules)[4] = IRelativeLayout::START_OF;
    (*hRules)[5] = IRelativeLayout::END_OF;
    (*hRules)[6] = IRelativeLayout::ALIGN_START;
    (*hRules)[7] = IRelativeLayout::ALIGN_END;
    return hRules;
}

const AutoPtr<ArrayOf<Int32> > RelativeLayout::RULES_VERTICAL = InitRULES_VERTICAL();
const AutoPtr<ArrayOf<Int32> > RelativeLayout::RULES_HORIZONTAL = InitRULES_HORIZONTAL();
const Int32 RelativeLayout::VALUE_NOT_SET = Elastos::Core::Math::INT32_MIN_VALUE;
const Int32 RelativeLayout::DEFAULT_WIDTH = 0x00010000;
CAR_INTERFACE_IMPL(RelativeLayout, ViewGroup, IRelativeLayout)
RelativeLayout::RelativeLayout()
    : mHasBaselineAlignedChild(FALSE)
    , mGravity(IGravity::START | IGravity::TOP)
    , mIgnoreGravity(0)
    , mDirtyHierarchy(FALSE)
    , mAllowBrokenMeasureSpecs(FALSE)
    , mMeasureVerticalWithPaddingMargin(FALSE)
{
    ASSERT_SUCCEEDED(CRect::New((IRect**)&mContentBounds));
    ASSERT_SUCCEEDED(CRect::New((IRect**)&mSelfBounds));
    mGraph = new DependencyGraph();
}

RelativeLayout::~RelativeLayout()
{
    mTopToBottomLeftToRightSet.Clear();
    mBaselineView = NULL;
    mContentBounds = NULL;
    mSelfBounds = NULL;
    mSortedHorizontalChildren = NULL;
    mSortedVerticalChildren = NULL;
}

ECode RelativeLayout::constructor(
    /* [in] */ IContext* context)
{
    return constructor(context, NULL);
}

ECode RelativeLayout::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    return constructor(context, attrs, 0);
}

ECode RelativeLayout::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ Int32 defStyleAttr)
{
    return constructor(context, attrs, defStyleAttr, 0);
}

ECode RelativeLayout::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ Int32 defStyleAttr,
    /* [in] */ Int32 defStyleRes)
{
    ASSERT_SUCCEEDED(ViewGroup::constructor(context, attrs, defStyleAttr, defStyleRes));
    ASSERT_SUCCEEDED(InitFromAttributes(context, attrs, defStyleAttr, defStyleRes));
    QueryCompatibilityModes(context);
    return NOERROR;
}

ECode RelativeLayout::InitFromAttributes(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ Int32 defStyleAttr,
    /* [in] */ Int32 defStyleRes)
{
    VALIDATE_NOT_NULL(context);

    AutoPtr<ArrayOf<Int32> > attrIds = ArrayOf<Int32>::Alloc(
            const_cast<Int32 *>(R::styleable::RelativeLayout),
            ArraySize(R::styleable::RelativeLayout));
    AutoPtr<ITypedArray> a;
    FAIL_RETURN(context->ObtainStyledAttributes(
            attrs, attrIds, defStyleAttr, defStyleRes, (ITypedArray**)&a));

    a->GetResourceId(R::styleable::RelativeLayout_ignoreGravity, IView::NO_ID, &mIgnoreGravity);
    a->GetInt32(R::styleable::RelativeLayout_gravity, mGravity, &mGravity);
    a->Recycle();

    return NOERROR;
}

void RelativeLayout::QueryCompatibilityModes(
    /* [in] */ IContext* context)
{
    AutoPtr<IApplicationInfo> info;
    context->GetApplicationInfo((IApplicationInfo**)&info);
    Int32 version = 0;
    info->GetTargetSdkVersion(&version);
    mAllowBrokenMeasureSpecs = version <= Build::VERSION_CODES::JELLY_BEAN_MR1;
    mMeasureVerticalWithPaddingMargin = version >= Build::VERSION_CODES::JELLY_BEAN_MR2;
}

Boolean RelativeLayout::ShouldDelayChildPressedState()
{
    return FALSE;
}

ECode RelativeLayout::SetIgnoreGravity(
    /* [in] */ Int32 viewId)
{
    mIgnoreGravity = viewId;
    return NOERROR;
}

ECode RelativeLayout::GetGravity(
    /* [out] */ Int32* gravity)
{
    VALIDATE_NOT_NULL(gravity);
    *gravity = mGravity;
    return NOERROR;
}

ECode RelativeLayout::SetGravity(
    /* [in] */ Int32 gravity)
{
    if (mGravity != gravity) {
        if ((gravity & IGravity::RELATIVE_HORIZONTAL_GRAVITY_MASK) == 0) {
            gravity |= IGravity::START;
        }

        if ((gravity & IGravity::VERTICAL_GRAVITY_MASK) == 0) {
            gravity |= IGravity::TOP;
        }

        mGravity = gravity;
        return RequestLayout();
    }

    return NOERROR;
}

ECode RelativeLayout::SetHorizontalGravity(
    /* [in] */ Int32 horizontalGravity)
{
    Int32 gravity = horizontalGravity & IGravity::RELATIVE_HORIZONTAL_GRAVITY_MASK;
    if ((mGravity & IGravity::RELATIVE_HORIZONTAL_GRAVITY_MASK) != gravity) {
        mGravity = (mGravity & ~IGravity::RELATIVE_HORIZONTAL_GRAVITY_MASK) | gravity;
        return RequestLayout();
    }

    return NOERROR;
}

ECode RelativeLayout::SetVerticalGravity(
    /* [in] */ Int32 verticalGravity)
{
    Int32 gravity = verticalGravity & IGravity::VERTICAL_GRAVITY_MASK;
    if ((mGravity & IGravity::VERTICAL_GRAVITY_MASK) != gravity) {
        mGravity = (mGravity & ~IGravity::VERTICAL_GRAVITY_MASK) | gravity;
        return RequestLayout();
    }

    return NOERROR;
}

ECode RelativeLayout::GetBaseline(
    /* [out] */ Int32* baseline)
{
    VALIDATE_NOT_NULL(baseline);
    return mBaselineView != NULL ?
            mBaselineView->GetBaseline(baseline) : ViewGroup::GetBaseline(baseline);
}

ECode RelativeLayout::RequestLayout()
{
    FAIL_RETURN(ViewGroup::RequestLayout());
    mDirtyHierarchy = TRUE;
    return NOERROR;
}

void RelativeLayout::SortChildren()
{
    Int32 count = 0;
    GetChildCount(&count);
    if (mSortedVerticalChildren == NULL || mSortedVerticalChildren->GetLength() != count) {
        mSortedVerticalChildren = ArrayOf<IView*>::Alloc(count);
    }
    if (mSortedHorizontalChildren == NULL || mSortedHorizontalChildren->GetLength() != count) {
        mSortedHorizontalChildren = ArrayOf<IView*>::Alloc(count);
    }

    AutoPtr<DependencyGraph> graph = mGraph;
    graph->Clear();

    for (Int32 i = 0; i < count; i++) {
        AutoPtr<IView> child;
        GetChildAt(i, (IView**)&child);
        graph->Add(child);
    }

    mGraph->GetSortedViews(mSortedVerticalChildren, RULES_VERTICAL);
    mGraph->GetSortedViews(mSortedHorizontalChildren, RULES_HORIZONTAL);
}

void RelativeLayout::OnMeasure(
    /* [in] */ Int32 widthMeasureSpec,
    /* [in] */ Int32 heightMeasureSpec)
{
    if (mDirtyHierarchy) {
        mDirtyHierarchy = FALSE;
        SortChildren();
    }

    Int32 myWidth = -1;
    Int32 myHeight = -1;

    Int32 width = 0;
    Int32 height = 0;

    Int32 widthMode = MeasureSpec::GetMode(widthMeasureSpec);
    Int32 heightMode = MeasureSpec::GetMode(heightMeasureSpec);
    Int32 widthSize = MeasureSpec::GetSize(widthMeasureSpec);
    Int32 heightSize = MeasureSpec::GetSize(heightMeasureSpec);

    // Record our dimensions if they are known;
    if (widthMode != MeasureSpec::UNSPECIFIED) {
        myWidth = widthSize;
    }

    if (heightMode != MeasureSpec::UNSPECIFIED) {
        myHeight = heightSize;
    }

    if (widthMode == MeasureSpec::EXACTLY) {
        width = myWidth;
    }

    if (heightMode == MeasureSpec::EXACTLY) {
        height = myHeight;
    }

    mHasBaselineAlignedChild = FALSE;

    AutoPtr<IView> ignore;
    Int32 gravity = mGravity & IGravity::RELATIVE_HORIZONTAL_GRAVITY_MASK;
    Boolean horizontalGravity = gravity != IGravity::START && gravity != 0;
    gravity = mGravity & IGravity::VERTICAL_GRAVITY_MASK;
    Boolean verticalGravity = gravity != IGravity::TOP && gravity != 0;

    Int32 left = Elastos::Core::Math::INT32_MAX_VALUE;
    Int32 top = Elastos::Core::Math::INT32_MAX_VALUE;
    Int32 right = Elastos::Core::Math::INT32_MIN_VALUE;
    Int32 bottom = Elastos::Core::Math::INT32_MIN_VALUE;

    Boolean offsetHorizontalAxis = FALSE;
    Boolean offsetVerticalAxis = FALSE;

    if ((horizontalGravity || verticalGravity) && mIgnoreGravity != IView::NO_ID) {
        FindViewById(mIgnoreGravity, (IView**)&ignore);
    }

    Boolean isWrapContentWidth = widthMode != MeasureSpec::EXACTLY;
    Boolean isWrapContentHeight = heightMode != MeasureSpec::EXACTLY;

    // We need to know our size for doing the correct computation of children positioning in RTL
    // mode but there is no practical way to get it instead of running the code below.
    // So, instead of running the code twice, we just set the width to a "default display width"
    // before the computation and then, as a last pass, we will update their real position with
    // an offset equals to "DEFAULT_WIDTH - width".
    Int32 layoutDirection = 0;
    GetLayoutDirection(&layoutDirection);
    Boolean rtl = FALSE;
    if ((IsLayoutRtl(&rtl), rtl) && myWidth == -1) {
        myWidth = DEFAULT_WIDTH;
    }

    AutoPtr<ArrayOf<IView*> > views = mSortedHorizontalChildren;
    Int32 count = views->GetLength();

    for (Int32 i = 0; i < count; i++) {
        AutoPtr<IView> child = (*views)[i];
        assert(child);

        Int32 visibility;
        child->GetVisibility(&visibility);
        if (visibility != IView::GONE) {
            AutoPtr<IViewGroupLayoutParams> vglp;
            child->GetLayoutParams((IViewGroupLayoutParams**)&vglp);
            IRelativeLayoutLayoutParams* params = IRelativeLayoutLayoutParams::Probe(vglp);

            AutoPtr<ArrayOf<Int32> > rules;
            params->GetRules(layoutDirection, (ArrayOf<Int32>**)&rules);

            LayoutParams* lp = (LayoutParams*)params;
            ApplyHorizontalSizeRules(lp, myWidth, rules);
            MeasureChildHorizontal(child, lp, myWidth, myHeight);
            if (PositionChildHorizontal(child, lp, myWidth, isWrapContentWidth)) {
                offsetHorizontalAxis = TRUE;
            }
        }
    }

    views = mSortedVerticalChildren;
    count = views->GetLength();
    AutoPtr<IContext> context;
    GetContext((IContext**)&context);
    AutoPtr<IApplicationInfo> info;
    context->GetApplicationInfo((IApplicationInfo**)&info);
    Int32 targetSdkVersion = 0;
    info->GetTargetSdkVersion(&targetSdkVersion);

    for (Int32 i = 0; i < count; i++) {
        AutoPtr<IView> child = (*views)[i];
        assert(child);

        Int32 visibility;
        child->GetVisibility(&visibility);
        if (visibility != IView::GONE) {
            AutoPtr<IViewGroupLayoutParams> vglp;
            child->GetLayoutParams((IViewGroupLayoutParams**)&vglp);
            IRelativeLayoutLayoutParams* params = IRelativeLayoutLayoutParams::Probe(vglp);

            LayoutParams* lp = (LayoutParams*)params;
            assert(lp);
            ApplyVerticalSizeRules(lp, myHeight);
            MeasureChild(child, lp, myWidth, myHeight);
            if (PositionChildVertical(child, lp, myHeight, isWrapContentHeight)) {
                offsetVerticalAxis = TRUE;
            }

            if (isWrapContentWidth) {
                if ((IsLayoutRtl(&rtl), rtl)) {
                    if (targetSdkVersion < Build::VERSION_CODES::KITKAT) {
                        width = Elastos::Core::Math::Max(width, myWidth - lp->mLeft);
                    } else {
                        width = Elastos::Core::Math::Max(width, myWidth - lp->mLeft - lp->mLeftMargin);
                    }
                } else {
                    if (targetSdkVersion < Build::VERSION_CODES::KITKAT) {
                        width = Elastos::Core::Math::Max(width, lp->mRight);
                    } else {
                        width = Elastos::Core::Math::Max(width, lp->mRight + lp->mRightMargin);
                    }
                }
            }

            if (isWrapContentHeight) {
                if (targetSdkVersion < Build::VERSION_CODES::KITKAT) {
                    height = Elastos::Core::Math::Max(height, lp->mBottom);
                } else {
                    height = Elastos::Core::Math::Max(height, lp->mBottom + lp->mBottomMargin);
                }
            }

            if (child != ignore || verticalGravity) {
                left = Elastos::Core::Math::Min(left, lp->mLeft - lp->mLeftMargin);
                top = Elastos::Core::Math::Min(top, lp->mTop - lp->mTopMargin);
            }

            if (child != ignore || horizontalGravity) {
                right = Elastos::Core::Math::Max(right, lp->mRight + lp->mRightMargin);
                bottom = Elastos::Core::Math::Max(bottom, lp->mBottom + lp->mBottomMargin);
            }
        }
    }

    if (mHasBaselineAlignedChild) {
        for (Int32 i = 0; i < count; i++) {
            AutoPtr<IView> child = (*views)[i];
            Int32 visibility;
            child->GetVisibility(&visibility);
            if (visibility != IView::GONE) {
                AutoPtr<IViewGroupLayoutParams> vglp;
                child->GetLayoutParams((IViewGroupLayoutParams**)&vglp);
                IRelativeLayoutLayoutParams* params = IRelativeLayoutLayoutParams::Probe(vglp);

                LayoutParams* lp = (LayoutParams*)params;
                AlignBaseline(child, lp);
                if (child != ignore || verticalGravity) {
                    left = Elastos::Core::Math::Min(left, lp->mLeft - lp->mLeftMargin);
                    top = Elastos::Core::Math::Min(top, lp->mTop - lp->mTopMargin);
                }

                if (child != ignore || horizontalGravity) {
                    right = Elastos::Core::Math::Max(right, lp->mRight + lp->mRightMargin);
                    bottom = Elastos::Core::Math::Max(bottom, lp->mBottom + lp->mBottomMargin);
                }
            }
        }
    }

    if (isWrapContentWidth) {
        // Width already has left padding in it since it was calculated by looking at
        // the right of each child view
        width += mPaddingRight;

        Int32 lpWidth = 0;
        if (mLayoutParams != NULL && (mLayoutParams->GetWidth(&lpWidth), lpWidth) >= 0) {
            width = Elastos::Core::Math::Max(width, lpWidth);
        }

        width = Elastos::Core::Math::Max(width, GetSuggestedMinimumWidth());
        width = ResolveSize(width, widthMeasureSpec);

        if (offsetHorizontalAxis) {
            for (Int32 i = 0; i < count; ++i) {
                AutoPtr<IView> child;
                GetChildAt(i, (IView**)&child);

                Int32 visibility;
                child->GetVisibility(&visibility);
                if (visibility != IView::GONE) {
                    AutoPtr<IViewGroupLayoutParams> vglp;
                    child->GetLayoutParams((IViewGroupLayoutParams**)&vglp);

                    LayoutParams* lp = (LayoutParams*)IRelativeLayoutLayoutParams::Probe(vglp);
                    AutoPtr<ArrayOf<Int32> > rules;
                    lp->GetRules(layoutDirection, (ArrayOf<Int32>**)&rules);
                    if ((*rules)[IRelativeLayout::CENTER_IN_PARENT] != 0 ||
                            (*rules)[IRelativeLayout::CENTER_HORIZONTAL] != 0) {
                        CenterHorizontal(child, lp, width);
                    }
                    else if ((*rules)[IRelativeLayout::ALIGN_PARENT_RIGHT] != 0) {
                        Int32 childWidth;
                        child->GetMeasuredWidth(&childWidth);
                        lp->mLeft = width - mPaddingRight - childWidth;
                        lp->mRight = lp->mLeft + childWidth;
                    }
                }
            }
        }
    }

    if (isWrapContentHeight) {
        // Height already has top padding in it since it was calculated by looking at
        // the bottom of each child view
        height += mPaddingBottom;

        Int32 lpHeight = 0;
        if (mLayoutParams != NULL && (mLayoutParams->GetHeight(&lpHeight), lpHeight) >= 0) {
            height = Elastos::Core::Math::Max(height, lpHeight);
        }

        height = Elastos::Core::Math::Max(height, GetSuggestedMinimumHeight());
        height = ResolveSize(height, heightMeasureSpec);

        if (offsetVerticalAxis) {
            for (Int32 i = 0; i < count; i++) {
                AutoPtr<IView> child;
                GetChildAt(i, (IView**)&child);

                Int32 visibility;
                child->GetVisibility(&visibility);
                if (visibility != IView::GONE) {
                    AutoPtr<IViewGroupLayoutParams> vglp;
                    child->GetLayoutParams((IViewGroupLayoutParams**)&vglp);
                    IRelativeLayoutLayoutParams* params = IRelativeLayoutLayoutParams::Probe(vglp);

                    LayoutParams* lp = (LayoutParams*)params;
                    AutoPtr<ArrayOf<Int32> > rules;
                    lp->GetRules(layoutDirection, (ArrayOf<Int32>**)&rules);
                    if ((*rules)[IRelativeLayout::CENTER_IN_PARENT] != 0 ||
                            (*rules)[IRelativeLayout::CENTER_VERTICAL] != 0) {
                        CenterVertical(child, lp, height);
                    }
                    else if ((*rules)[IRelativeLayout::ALIGN_PARENT_BOTTOM] != 0) {
                        Int32 childHeight;
                        child->GetMeasuredHeight(&childHeight);
                        lp->mTop = height - mPaddingBottom - childHeight;
                        lp->mBottom = lp->mTop + childHeight;
                    }
                }
            }
        }
    }

    if (horizontalGravity || verticalGravity) {
        mSelfBounds->Set(mPaddingLeft, mPaddingTop, width - mPaddingRight,
                height - mPaddingBottom);

        Gravity::Apply(mGravity, right - left, bottom - top,
            mSelfBounds, mContentBounds, layoutDirection);

        Int32 horizontalOffset = ((CRect*)mContentBounds.Get())->mLeft - left;
        Int32 verticalOffset = ((CRect*)mContentBounds.Get())->mTop - top;
        if (horizontalOffset != 0 || verticalOffset != 0) {
            for (Int32 i = 0; i < count; ++i) {
                AutoPtr<IView> child;
                GetChildAt(i, (IView**)&child);

                Int32 visibility;
                child->GetVisibility(&visibility);
                if (visibility != IView::GONE && child != ignore) {
                    AutoPtr<IViewGroupLayoutParams> vglp;
                    child->GetLayoutParams((IViewGroupLayoutParams**)&vglp);
                    IRelativeLayoutLayoutParams* params = IRelativeLayoutLayoutParams::Probe(vglp);
                    LayoutParams* lp = (LayoutParams*)params;

                    if (horizontalGravity) {
                        lp->mLeft += horizontalOffset;
                        lp->mRight += horizontalOffset;
                    }
                    if (verticalGravity) {
                        lp->mTop += verticalOffset;
                        lp->mBottom += verticalOffset;
                    }
                }
            }
        }
    }

    if (IsLayoutRtl(&rtl), &rtl) {
        const Int32 offsetWidth = myWidth - width;
        for (Int32 i = 0; i < count; i++) {
            AutoPtr<IView> child;
            GetChildAt(i, (IView**)&child);
            Int32 visibility = 0;
            if ((child->GetVisibility(&visibility), visibility) != IView::GONE) {
                AutoPtr<IViewGroupLayoutParams> vglp;
                child->GetLayoutParams((IViewGroupLayoutParams**)&vglp);

                AutoPtr<IRelativeLayoutLayoutParams> params = IRelativeLayoutLayoutParams::Probe(vglp);
                ((LayoutParams*)params.Get())->mLeft -= offsetWidth;
                ((LayoutParams*)params.Get())->mRight -= offsetWidth;
            }
        }
    }

    SetMeasuredDimension(width, height);
}

void RelativeLayout::AlignBaseline(
    /* [in] */ IView* child,
    /* [in] */ LayoutParams* params)
{
    assert(child != NULL && params != NULL);

    Int32 layoutDirection = 0;
    GetLayoutDirection(&layoutDirection);

    AutoPtr<ArrayOf<Int32> > rules;
    params->GetRules(layoutDirection, (ArrayOf<Int32>**)&rules);
    Int32 anchorBaseline = GetRelatedViewBaseline(rules, IRelativeLayout::ALIGN_BASELINE);

    if (anchorBaseline != -1) {
        AutoPtr<LayoutParams> anchorParams =
                GetRelatedViewParams(rules, IRelativeLayout::ALIGN_BASELINE);
        if (anchorParams != NULL) {
            Int32 offset = anchorParams->mTop + anchorBaseline;
            Int32 baseline;
            child->GetBaseline(&baseline);
            if (baseline != -1) {
                offset -= baseline;
            }
            Int32 height = params->mBottom - params->mTop;
            params->mTop = offset;
            params->mBottom = params->mTop + height;
        }
    }

    if (mBaselineView == NULL) {
        mBaselineView = child;
    }
    else {
        AutoPtr<IViewGroupLayoutParams> vglp;
        mBaselineView->GetLayoutParams((IViewGroupLayoutParams**)&vglp);
        IRelativeLayoutLayoutParams* layoutParams = IRelativeLayoutLayoutParams::Probe(vglp);
        LayoutParams* lp = (LayoutParams*)layoutParams;

        if (params->mTop < lp->mTop || (params->mTop == lp->mTop && params->mLeft < lp->mLeft)) {
            mBaselineView = child;
        }
    }
}

void RelativeLayout::MeasureChild(
    /* [in] */ IView* child,
    /* [in] */ LayoutParams* params,
    /* [in] */ Int32 myWidth,
    /* [in] */ Int32 myHeight)
{
    assert(child != NULL);

    Int32 childWidthMeasureSpec = GetChildMeasureSpec(params->mLeft,
            params->mRight, params->mWidth,
            params->mLeftMargin, params->mRightMargin,
            mPaddingLeft, mPaddingRight, myWidth);
    Int32 childHeightMeasureSpec = GetChildMeasureSpec(params->mTop,
            params->mBottom, params->mHeight,
            params->mTopMargin, params->mBottomMargin,
            mPaddingTop, mPaddingBottom, myHeight);
    child->Measure(childWidthMeasureSpec, childHeightMeasureSpec);
}

void RelativeLayout::MeasureChildHorizontal(
    /* [in] */ IView* child,
    /* [in] */ LayoutParams* params,
    /* [in] */ Int32 myWidth,
    /* [in] */ Int32 myHeight)
{
    assert(child != NULL);

    Int32 childWidthMeasureSpec = GetChildMeasureSpec(params->mLeft,
            params->mRight, params->mWidth,
            params->mLeftMargin, params->mRightMargin,
            mPaddingLeft, mPaddingRight, myWidth);

    Int32 maxHeight = myHeight;
    if (mMeasureVerticalWithPaddingMargin) {
        maxHeight = Elastos::Core::Math::Max(0, myHeight - mPaddingTop - mPaddingBottom -
                params->mTopMargin - params->mBottomMargin);
    }
    Int32 childHeightMeasureSpec;
    if (myHeight < 0 && !mAllowBrokenMeasureSpecs) {
        if (params->mHeight >= 0) {
            childHeightMeasureSpec = MeasureSpec::MakeMeasureSpec(
                    params->mHeight, MeasureSpec::EXACTLY);
        } else {
            // Negative values in a mySize/myWidth/myWidth value in RelativeLayout measurement
            // is code for, "we got an unspecified mode in the RelativeLayout's measurespec."
            // Carry it forward.
            childHeightMeasureSpec = MeasureSpec::MakeMeasureSpec(0, MeasureSpec::UNSPECIFIED);
        }
    } else if (params->mWidth == IViewGroupLayoutParams::MATCH_PARENT) {
        childHeightMeasureSpec = MeasureSpec::MakeMeasureSpec(maxHeight, MeasureSpec::EXACTLY);
    } else {
        childHeightMeasureSpec = MeasureSpec::MakeMeasureSpec(maxHeight, MeasureSpec::AT_MOST);
    }

    child->Measure(childWidthMeasureSpec, childHeightMeasureSpec);
}

Int32 RelativeLayout::GetChildMeasureSpec(
    /* [in] */ Int32 childStart,
    /* [in] */ Int32 childEnd,
    /* [in] */ Int32 childSize,
    /* [in] */ Int32 startMargin,
    /* [in] */ Int32 endMargin,
    /* [in] */ Int32 startPadding,
    /* [in] */ Int32 endPadding,
    /* [in] */ Int32 mySize)
{
    Int32 childSpecMode = 0;
    Int32 childSpecSize = 0;

    // Negative values in a mySize value in RelativeLayout
    // measurement is code for, "we got an unspecified mode in the
    // RelativeLayout's measure spec."
    if (mySize < 0 && !mAllowBrokenMeasureSpecs) {
        if (childStart != VALUE_NOT_SET && childEnd != VALUE_NOT_SET) {
            // Constraints fixed both edges, so child has an exact size.
            childSpecSize = Elastos::Core::Math::Max(0, childEnd - childStart);
            childSpecMode = MeasureSpec::EXACTLY;
        } else if (childSize >= 0) {
            // The child specified an exact size.
            childSpecSize = childSize;
            childSpecMode = MeasureSpec::EXACTLY;
        } else {
            // Allow the child to be whatever size it wants.
            childSpecSize = 0;
            childSpecMode = MeasureSpec::UNSPECIFIED;
        }

        return MeasureSpec::MakeMeasureSpec(childSpecSize, childSpecMode);
    }

    // Figure out start and end bounds.
    Int32 tempStart = childStart;
    Int32 tempEnd = childEnd;

    // If the view did not express a layout constraint for an edge, use
    // view's margins and our padding
    if (tempStart == VALUE_NOT_SET) {
        tempStart = startPadding + startMargin;
    }
    if (tempEnd == VALUE_NOT_SET) {
        tempEnd = mySize - endPadding - endMargin;
    }

    // Figure out maximum size available to this view
    Int32 maxAvailable = tempEnd - tempStart;

    if (childStart != VALUE_NOT_SET && childEnd != VALUE_NOT_SET) {
        // Constraints fixed both edges, so child must be an exact size
        childSpecMode = MeasureSpec::EXACTLY;
        childSpecSize = maxAvailable;
    }
    else {
        if (childSize >= 0) {
            // Child wanted an exact size. Give as much as possible
            childSpecMode = MeasureSpec::EXACTLY;

            if (maxAvailable >= 0) {
                // We have a maxmum size in this dimension.
                childSpecSize = Elastos::Core::Math::Min(maxAvailable, childSize);
            }
            else {
                // We can grow in this dimension.
                childSpecSize = childSize;
            }
        }
        else if (childSize == IViewGroupLayoutParams::MATCH_PARENT) {
            // Child wanted to be as big as possible. Give all availble
            // space
            childSpecMode = MeasureSpec::EXACTLY;
            childSpecSize = maxAvailable;
        }
        else if (childSize == IViewGroupLayoutParams::WRAP_CONTENT) {
            // Child wants to wrap content. Use AT_MOST
            // to communicate available space if we know
            // our max size
            if (maxAvailable >= 0) {
                // We have a maxmum size in this dimension.
                childSpecMode = MeasureSpec::AT_MOST;
                childSpecSize = maxAvailable;
            }
            else {
                // We can grow in this dimension. Child can be as big as it
                // wants
                childSpecMode = MeasureSpec::UNSPECIFIED;
                childSpecSize = 0;
            }
        }
    }

    return MeasureSpec::MakeMeasureSpec(childSpecSize, childSpecMode);
}

Boolean RelativeLayout::PositionChildHorizontal(
    /* [in] */ IView* child,
    /* [in] */ LayoutParams* params,
    /* [in] */ Int32 myWidth,
    /* [in] */ Boolean wrapContent)
{
    assert(child != NULL && params != NULL);

    Int32 layoutDirection = 0;
    GetLayoutDirection(&layoutDirection);

    AutoPtr<ArrayOf<Int32> > rules;
    params->GetRules(layoutDirection, (ArrayOf<Int32>**)&rules);

    Int32 width;
    child->GetMeasuredWidth(&width);

    if (params->mLeft == VALUE_NOT_SET && params->mRight != VALUE_NOT_SET) {
        // Right is fixed, but left varies
        params->mLeft = params->mRight - width;
    }
    else if (params->mLeft != VALUE_NOT_SET && params->mRight == VALUE_NOT_SET) {
        // Left is fixed, but right varies
        params->mRight = params->mLeft + width;
    }
    else if (params->mLeft == VALUE_NOT_SET && params->mRight == VALUE_NOT_SET) {
        // Both left and right vary
        if ((*rules)[IRelativeLayout::CENTER_IN_PARENT] != 0
                || (*rules)[IRelativeLayout::CENTER_HORIZONTAL] != 0) {
            if (!wrapContent) {
                CenterHorizontal(child, params, myWidth);
            }
            else {
                params->mLeft = mPaddingLeft + params->mLeftMargin;
                params->mRight = params->mLeft + width;
            }
            return TRUE;
        }
        else {
            // This is the default case. For RTL we start from the right and for LTR we start
            // from the left. This will give LEFT/TOP for LTR and RIGHT/TOP for RTL.
            Boolean rtl = FALSE;
            if (IsLayoutRtl(&rtl), rtl) {
                params->mRight = myWidth - mPaddingRight- params->mRightMargin;
                params->mLeft = params->mRight - width;
            } else {
                params->mLeft = mPaddingLeft + params->mLeftMargin;
                params->mRight = params->mLeft + width;
            }
        }
    }
    return (*rules)[IRelativeLayout::ALIGN_PARENT_END] != 0;
}

Boolean RelativeLayout::PositionChildVertical(
    /* [in] */ IView* child,
    /* [in] */ LayoutParams* params,
    /* [in] */ Int32 myHeight,
    /* [in] */ Boolean wrapContent)
{
    assert(child != NULL && params != NULL);

    Int32 height;
    child->GetMeasuredHeight(&height);

    AutoPtr<ArrayOf<Int32> > rules;
    params->GetRules((ArrayOf<Int32>**)&rules);

    if (params->mTop == VALUE_NOT_SET && params->mBottom != VALUE_NOT_SET) {
        // Bottom is fixed, but top varies
        params->mTop = params->mBottom - height;
    }
    else if (params->mTop != VALUE_NOT_SET && params->mBottom == VALUE_NOT_SET) {
        // Top is fixed, but bottom varies
        params->mBottom = params->mTop + height;
    }
    else if (params->mTop == VALUE_NOT_SET && params->mBottom == VALUE_NOT_SET) {
        // Both top and bottom vary
        if ((*rules)[IRelativeLayout::CENTER_IN_PARENT] != 0 || (*rules)[IRelativeLayout::CENTER_VERTICAL] != 0) {
            if (!wrapContent) {
                CenterVertical(child, params, myHeight);
            }
            else {
                params->mTop = mPaddingTop + params->mTopMargin;
                params->mBottom = params->mTop + height;
            }
            return TRUE;
        }
        else {
            params->mTop = mPaddingTop + params->mTopMargin;
            params->mBottom = params->mTop + height;
        }
    }
    return (*rules)[IRelativeLayout::ALIGN_PARENT_BOTTOM] != 0;
}

void RelativeLayout::ApplyHorizontalSizeRules(
    /* [in] */ LayoutParams* childParams,
    /* [in] */ Int32 myWidth,
    /* [in] */ ArrayOf<Int32>* rules)
{
    assert(childParams != NULL);
    AutoPtr<LayoutParams> anchorParams;

    // VALUE_NOT_SET indicates a "soft requirement" in that direction. For example:
    // left=10, right=VALUE_NOT_SET means the view must start at 10, but can go as far as it
    // wants to the right
    // left=VALUE_NOT_SET, right=10 means the view must end at 10, but can go as far as it
    // wants to the left
    // left=10, right=20 means the left and right ends are both fixed
    childParams->mLeft = VALUE_NOT_SET;
    childParams->mRight = VALUE_NOT_SET;

    anchorParams = GetRelatedViewParams(rules, IRelativeLayout::LEFT_OF);
    if (anchorParams != NULL) {
        childParams->mRight =
                anchorParams->mLeft - (anchorParams->mLeftMargin + childParams->mRightMargin);
    }
    else if (childParams->mAlignWithParent && (*rules)[IRelativeLayout::LEFT_OF] != 0) {
        if (myWidth >= 0) {
            childParams->mRight =
                    myWidth - mPaddingRight - childParams->mRightMargin;
        }
    }

    anchorParams = GetRelatedViewParams(rules, IRelativeLayout::RIGHT_OF);
    if (anchorParams != NULL) {
        childParams->mLeft =
                anchorParams->mRight + (anchorParams->mRightMargin + childParams->mLeftMargin);
    }
    else if (childParams->mAlignWithParent && (*rules)[IRelativeLayout::RIGHT_OF] != 0) {
        childParams->mLeft = mPaddingLeft + childParams->mLeftMargin;
    }

    anchorParams = GetRelatedViewParams(rules, IRelativeLayout::ALIGN_LEFT);
    if (anchorParams != NULL) {
        childParams->mLeft = anchorParams->mLeft + childParams->mLeftMargin;
    }
    else if (childParams->mAlignWithParent && (*rules)[IRelativeLayout::ALIGN_LEFT] != 0) {
        childParams->mLeft = mPaddingLeft + childParams->mLeftMargin;
    }

    anchorParams = GetRelatedViewParams(rules, IRelativeLayout::ALIGN_RIGHT);
    if (anchorParams != NULL) {
        childParams->mRight = anchorParams->mRight - childParams->mRightMargin;
    }
    else if (childParams->mAlignWithParent && (*rules)[IRelativeLayout::ALIGN_RIGHT] != 0) {
        if (myWidth >= 0) {
            childParams->mRight = myWidth - mPaddingRight - childParams->mRightMargin;
        }
    }

    if (0 != (*rules)[IRelativeLayout::ALIGN_PARENT_LEFT]) {
        childParams->mLeft = mPaddingLeft + childParams->mLeftMargin;
    }

    if (0 != (*rules)[IRelativeLayout::ALIGN_PARENT_RIGHT]) {
        if (myWidth >= 0) {
            childParams->mRight = myWidth - mPaddingRight - childParams->mRightMargin;
        }
    }
}

void RelativeLayout::ApplyVerticalSizeRules(
    /* [in] */ LayoutParams* childParams,
    /* [in] */ Int32 myHeight)
{
    assert(childParams != NULL);

    AutoPtr<ArrayOf<Int32> > rules;
    childParams->GetRules((ArrayOf<Int32>**)&rules);
    AutoPtr<LayoutParams> anchorParams;

    childParams->mTop = VALUE_NOT_SET;
    childParams->mBottom = VALUE_NOT_SET;

    anchorParams = GetRelatedViewParams(rules, IRelativeLayout::ABOVE);
    if (anchorParams != NULL) {
        childParams->mBottom =
                anchorParams->mTop - (anchorParams->mTopMargin + childParams->mBottomMargin);
    }
    else if (childParams->mAlignWithParent && (*rules)[IRelativeLayout::ABOVE] != 0) {
        if (myHeight >= 0) {
            childParams->mBottom = myHeight - mPaddingBottom - childParams->mBottomMargin;
        }
    }

    anchorParams = GetRelatedViewParams(rules, IRelativeLayout::BELOW);
    if (anchorParams != NULL) {
        childParams->mTop =
                anchorParams->mBottom + (anchorParams->mBottomMargin + childParams->mTopMargin);
    }
    else if (childParams->mAlignWithParent && (*rules)[IRelativeLayout::BELOW] != 0) {
        childParams->mTop = mPaddingTop + childParams->mTopMargin;
    }

    anchorParams = GetRelatedViewParams(rules, IRelativeLayout::ALIGN_TOP);
    if (anchorParams != NULL) {
        childParams->mTop = anchorParams->mTop + childParams->mTopMargin;
    }
    else if (childParams->mAlignWithParent && (*rules)[IRelativeLayout::ALIGN_TOP] != 0) {
        childParams->mTop = mPaddingTop + childParams->mTopMargin;
    }

    anchorParams = GetRelatedViewParams(rules, IRelativeLayout::ALIGN_BOTTOM);
    if (anchorParams != NULL) {
        childParams->mBottom = anchorParams->mBottom - childParams->mBottomMargin;
    }
    else if (childParams->mAlignWithParent && (*rules)[IRelativeLayout::ALIGN_BOTTOM] != 0) {
        if (myHeight >= 0) {
            childParams->mBottom = myHeight - mPaddingBottom - childParams->mBottomMargin;
        }
    }

    if (0 != (*rules)[IRelativeLayout::ALIGN_PARENT_TOP]) {
        childParams->mTop = mPaddingTop + childParams->mTopMargin;
    }

    if (0 != (*rules)[IRelativeLayout::ALIGN_PARENT_BOTTOM]) {
        if (myHeight >= 0) {
            childParams->mBottom = myHeight - mPaddingBottom - childParams->mBottomMargin;
        }
    }

    if ((*rules)[IRelativeLayout::ALIGN_BASELINE] != 0) {
        mHasBaselineAlignedChild = TRUE;
    }
}

AutoPtr<IView> RelativeLayout::GetRelatedView(
    /* [in] */ ArrayOf<Int32>* rules,
    /* [in] */ Int32 relation)
{
    assert(rules != NULL);

    AutoPtr<IView> v;
    Int32 id = (*rules)[relation];
    if (id != 0) {
        HashMap<Int32, AutoPtr<Node> >::Iterator it = mGraph->mKeyNodes.Find(id);
        Node* node = it == mGraph->mKeyNodes.End() ? NULL : it->mSecond;
        if (node == NULL)
            return v;

        v = node->mView;

        // Find the first non-GONE view up the chain
        Int32 visibility;
        v->GetVisibility(&visibility);
        while (visibility == IView::GONE) {
            AutoPtr<IViewGroupLayoutParams> vglp;
            v->GetLayoutParams((IViewGroupLayoutParams**)&vglp);
            IRelativeLayoutLayoutParams* params = IRelativeLayoutLayoutParams::Probe(vglp);

            AutoPtr<ArrayOf<Int32> > rules;
            Int32 direction;
            v->GetLayoutDirection(&direction);
            params->GetRules(direction, (ArrayOf<Int32>**)&rules);

            it = mGraph->mKeyNodes.Find((*rules)[relation]);
            node = it == mGraph->mKeyNodes.End() ? NULL : it->mSecond;
            if (node == NULL) return v;
            v = node->mView;
            v->GetVisibility(&visibility);
        }

        return v;
    }

    return v;
}

AutoPtr<RelativeLayout::LayoutParams> RelativeLayout::GetRelatedViewParams(
    /* [in] */ ArrayOf<Int32>* rules,
    /* [in] */ Int32 relation)
{
    AutoPtr<LayoutParams> lp;
    AutoPtr<IView> v = GetRelatedView(rules, relation);
    if (v != NULL) {
        AutoPtr<IViewGroupLayoutParams> params;
        v->GetLayoutParams((IViewGroupLayoutParams**)&params);
        if (params != NULL && params->Probe(EIID_IRelativeLayoutLayoutParams) != NULL) {
             lp = (LayoutParams*)IRelativeLayoutLayoutParams::Probe(params);
        }
    }

    return lp;
}

Int32 RelativeLayout::GetRelatedViewBaseline(
    /* [in] */ ArrayOf<Int32>* rules,
    /* [in] */ Int32 relation)
{
    assert(rules != NULL);

    AutoPtr<IView> v = GetRelatedView(rules, relation);
    if (v != NULL) {
        Int32 baseline;
        v->GetBaseline(&baseline);
        return baseline;
    }
    return -1;
}

void RelativeLayout::CenterHorizontal(
    /* [in] */ IView* child,
    /* [in] */ LayoutParams* params,
    /* [in] */ Int32 myWidth)
{
    assert(child != NULL);

    Int32 childWidth;
    child->GetMeasuredWidth(&childWidth);
    Int32 left = (myWidth - childWidth) / 2;

    params->mLeft = left;
    params->mRight = left + childWidth;
}

void RelativeLayout::CenterVertical(
    /* [in] */ IView* child,
    /* [in] */ LayoutParams* params,
    /* [in] */ Int32 myHeight)
{
    assert(child != NULL);

    Int32 childHeight;
    child->GetMeasuredHeight(&childHeight);
    Int32 top = (myHeight - childHeight) / 2;

    params->mTop = top;
    params->mBottom = top + childHeight;
}

ECode RelativeLayout::OnLayout(
    /* [in] */ Boolean changed,
    /* [in] */ Int32 l,
    /* [in] */ Int32 t,
    /* [in] */ Int32 r,
    /* [in] */ Int32 b)
{
    //  The layout has actually already been performed and the positions
    //  cached.  Apply the cached values to the children.
    Int32 count = 0;
    GetChildCount(&count);

    for (Int32 i = 0; i < count; i++) {
        AutoPtr<IView> child;
        GetChildAt(i, (IView**)&child);
        Int32 visibility;
        child->GetVisibility(&visibility);
        if (visibility != IView::GONE) {
            AutoPtr<IViewGroupLayoutParams> vglp;
            child->GetLayoutParams((IViewGroupLayoutParams**)&vglp);
            LayoutParams* st = (LayoutParams*)IRelativeLayoutLayoutParams::Probe(vglp);
            child->Layout(st->mLeft, st->mTop, st->mRight, st->mBottom);
        }
    }
    return NOERROR;
}

ECode RelativeLayout::GenerateLayoutParams(
    /* [in] */ IAttributeSet* attrs,
    /* [out] */ IViewGroupLayoutParams** params)
{
    VALIDATE_NOT_NULL(params);
    *params = NULL;
    AutoPtr<IViewGroupLayoutParams> lp;
    AutoPtr<IContext> context;
    GetContext((IContext**)&context);
    FAIL_RETURN(CRelativeLayoutLayoutParams::New(context, attrs, (IViewGroupLayoutParams**)&lp));
    *params = lp;
    REFCOUNT_ADD(*params);
    return NOERROR;
}

ECode RelativeLayout::GenerateDefaultLayoutParams(
    /* [out] */ IViewGroupLayoutParams** lp)
{
    VALIDATE_NOT_NULL(lp);
    return CRelativeLayoutLayoutParams::New(
        IViewGroupLayoutParams::WRAP_CONTENT,
        IViewGroupLayoutParams::WRAP_CONTENT,
        lp);
}

Boolean RelativeLayout::CheckLayoutParams(
    /* [in] */ IViewGroupLayoutParams* p)
{
    return p != NULL && IRelativeLayoutLayoutParams::Probe(p) != NULL;
}

AutoPtr<IViewGroupLayoutParams> RelativeLayout::GenerateLayoutParams(
    /* [in] */ IViewGroupLayoutParams* p)
{
    assert(p != NULL);

    AutoPtr<IViewGroupLayoutParams> lp;
    ASSERT_SUCCEEDED(CRelativeLayoutLayoutParams::New(p,
            (IViewGroupLayoutParams**)&lp));

    return lp;
}

Boolean RelativeLayout::DispatchPopulateAccessibilityEvent(
    /* [in] */ IAccessibilityEvent* p)
{
    // sort children top-to-bottom and left-to-right
    Int32 childCount = 0;
    GetChildCount(&childCount);
    for (Int32 i = 0; i < childCount; i++) {
        AutoPtr<IView> child;
        GetChildAt(i, (IView**)&child);
        mTopToBottomLeftToRightSet.Insert(child);
    }

    Int32 visibility;
    Set<AutoPtr<IView> >::Iterator it = mTopToBottomLeftToRightSet.Begin();
    for (; it != mTopToBottomLeftToRightSet.End(); ++it) {
        AutoPtr<IView> view = *it;
        Boolean result;
        view->DispatchPopulateAccessibilityEvent(p, &result);
        view->GetVisibility(&visibility);
        if (visibility == IView::VISIBLE && result) {
            mTopToBottomLeftToRightSet.Clear();
            return TRUE;
        }
    }

    mTopToBottomLeftToRightSet.Clear();
    return FALSE;
}

ECode RelativeLayout::OnInitializeAccessibilityEvent(
    /* [in] */ IAccessibilityEvent* event)
{
    ViewGroup::OnInitializeAccessibilityEvent(event);
    AutoPtr<ICharSequence> seq;
    CString::New(String("CRelativeLayout"), (ICharSequence**)&seq);
    return IAccessibilityRecord::Probe(event)->SetClassName(seq);
}

ECode RelativeLayout::OnInitializeAccessibilityNodeInfo(
    /* [in] */ IAccessibilityNodeInfo* info)
{
    ViewGroup::OnInitializeAccessibilityNodeInfo(info);
    AutoPtr<ICharSequence> seq;
    CString::New(String("CRelativeLayout"), (ICharSequence**)&seq);
    return info->SetClassName(seq);
}


} // namespace Widget
} // namespace Droid
} // namespace Elastos
