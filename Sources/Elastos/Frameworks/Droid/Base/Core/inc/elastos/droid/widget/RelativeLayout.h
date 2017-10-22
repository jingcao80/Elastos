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

#ifndef __ELASTOS_DROID_WIDGET_RELATIVELAYOUT_H__
#define __ELASTOS_DROID_WIDGET_RELATIVELAYOUT_H__

#include "Elastos.Droid.Widget.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/view/ViewGroup.h"
#include "elastos/droid/utility/Pools.h"
#include <elastos/utility/etl/Set.h>
#include <elastos/utility/etl/List.h>
#include <elastos/utility/etl/HashMap.h>

using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Utility::Pools;
using Elastos::Droid::View::IViewGroupLayoutParams;
using Elastos::Droid::View::IViewGroupMarginLayoutParams;
using Elastos::Droid::View::ViewGroup;
using Elastos::Droid::View::IView;
using Elastos::Core::IComparator;
using Elastos::Utility::Etl::HashMap;
using Elastos::Utility::Etl::List;
using Elastos::Utility::Etl::Set;

namespace Elastos {
namespace Droid {
namespace Widget {
class Node;
}
}
}

DEFINE_OBJECT_HASH_FUNC_FOR(Elastos::Droid::Widget::Node);
DEFINE_CONVERSION_FOR(Elastos::Droid::Widget::Node, IInterface);

namespace Elastos {
namespace Droid {
namespace Widget {

/**
 * Compares two views in left-to-right and top-to-bottom fashion.
 */
class TopToBottomLeftToRightComparator
    : public Object
    , public IComparator
{
public:
    CAR_INTERFACE_DECL()

    CARAPI Compare(
        /* [in] */ IInterface* lhs,
        /* [in] */ IInterface* rhs,
        /* [out] */ Int32* result);
};

class DependencyGraph
    : public Object
{
public:
    DependencyGraph();

    /**
     * Clears the graph.
     */
    CARAPI_(void) Clear();

    /**
     * Adds a view to the graph.
     *
     * @param view The view to be added as a node to the graph.
     */
    CARAPI_(void) Add(
        /* [in] */ IView* view);

    /**
     * Builds a sorted list of views. The sorting order depends on the dependencies
     * between the view. For instance, if view C needs view A to be processed first
     * and view A needs view B to be processed first, the dependency graph
     * is: B -> A -> C. The sorted array will contain views B, A and C in this order.
     *
     * @param sorted The sorted list of views. The length of this array must
     *        be equal to getChildCount().
     * @param rules The list of rules to take into account.
     */
    CARAPI GetSortedViews(
        /* [in] */ ArrayOf<IView*>* sorted,
        /* [in] */ ArrayOf<Int32>* rules);

private:
    /**
     * Finds the roots of the graph. A root is a node with no dependency and
     * with [0..n] dependents.
     *
     * @param rulesFilter The list of rules to consider when building the
     *        dependencies
     *
     * @return A list of node, each being a root of the graph
     */
    CARAPI_(List< AutoPtr<Node> > &) FindRoots(
        /* [in] */ ArrayOf<Int32>* rulesFilter);

private:
    friend class RelativeLayout;

    /**
     * List of all views in the graph.
     */
    List< AutoPtr<Node> > mNodes;

    /**
     * List of nodes in the graph. Each node is identified by its
     * view id (see View#getId()).
     */
    HashMap<Int32, AutoPtr<Node> > mKeyNodes;

    /**
     * Temporary data structure used to build the list of roots
     * for this graph.
     */
    List< AutoPtr<Node> > mRoots;
};

/**
 * A node in the dependency graph. A node is a view, its list of dependencies
 * and its list of dependents.
 *
 * A node with no dependent is considered a root of the graph.
 */
class Node
    : public Object
{
public:
    static CARAPI_(AutoPtr<Node>) Acquire(
        /* [in] */ IView* view);

    // Method name Release is conflic with IInterface->Release, rename Release to Clear
    CARAPI_(void) Clear();

public:
    /**
     * The view representing this node in the layout.
     */
    AutoPtr<IView> mView;

    /**
     * The list of dependents for this node; a dependent is a node
     * that needs this node to be processed first.
     */
    HashMap<AutoPtr<Node>, DependencyGraph*> mDependents;
    typedef typename HashMap<AutoPtr<Node>, DependencyGraph*>::Iterator DependentsIterator;
    typedef typename HashMap<AutoPtr<Node>, DependencyGraph*>::ValueType DependentsValueType;

    /**
     * The list of dependencies for this node.
     */
    HashMap<Int32, Node*> mDependencies;

    // The pool is static, so all nodes instances are shared across
    // activities, that's why we give it a rather high limit
    const static Int32 POOL_LIMIT;

    static AutoPtr<Pools::SynchronizedPool<Node> > sPool;
};

class ECO_PUBLIC RelativeLayout
    : public ViewGroup
    , public IRelativeLayout
{
public:
    class RelativeLayoutLayoutParams
        : public ViewGroup::MarginLayoutParams
        , public IRelativeLayoutLayoutParams
    {
    public:
        CAR_INTERFACE_DECL()

        RelativeLayoutLayoutParams();

        CARAPI constructor(
            /* [in] */ IContext* c,
            /* [in] */ IAttributeSet* attrs);

        CARAPI constructor(
            /* [in] */ Int32 width,
            /* [in] */ Int32 height);

        CARAPI constructor(
            /* [in] */ IViewGroupLayoutParams* source);

        CARAPI constructor(
            /* [in] */ IViewGroupMarginLayoutParams* source);

        /**
         * Copy constructor. Clones the width, height, margin values, and rules
         * of the source.
         *
         * @param source The layout params to copy from.
         */
        CARAPI constructor(
            /* [in] */ IRelativeLayoutLayoutParams* source);

        CARAPI AddRule(
            /* [in] */ Int32 verb);

        CARAPI AddRule(
            /* [in] */ Int32 verb,
            /* [in] */ Int32 anchor);

        CARAPI RemoveRule(
            /* [in] */ Int32 verb);

        CARAPI GetRules(
            /* [out, callee] */ ArrayOf<Int32>** rules);

        CARAPI GetRules(
            /* [in] */ Int32 layoutDirection,
            /* [out, callee] */ ArrayOf<Int32>** rules);

        CARAPI SetAlignWithParent(
            /* [in] */ Boolean align);

        CARAPI GetAlignWithParent(
            /* [out] */ Boolean* align);

        virtual CARAPI ResolveLayoutDirection(
            /* [in] */ Int32 layoutDirection);

    private:
        // The way we are resolving rules depends on the layout direction and if we are pre JB MR1
        // or not.
        //
        // If we are pre JB MR1 (said as "RTL compatibility mode"), "left"/"right" rules are having
        // predominance over any "start/end" rules that could have been defined. A special case:
        // if no "left"/"right" rule has been defined and "start"/"end" rules are defined then we
        // resolve those "start"/"end" rules to "left"/"right" respectively.
        //
        // If we are JB MR1+, then "start"/"end" rules are having predominance over "left"/"right"
        // rules. If no "start"/"end" rule is defined then we use "left"/"right" rules.
        //
        // In all cases, the result of the resolution should clear the "start"/"end" rules to leave
        // only the "left"/"right" rules at the end.
        CARAPI_(void) ResolveRules(
            /* [in] */ Int32 layoutDirection);

        CARAPI_(Boolean) HasRelativeRules();

    public:
        AutoPtr<ArrayOf<Int32> > mRules;
        AutoPtr<ArrayOf<Int32> > mInitialRules;

        Int32 mLeft;
        Int32 mTop;
        Int32 mRight;
        Int32 mBottom;

        Boolean mRulesChanged;
        Boolean mIsRtlCompatibilityMode;

        /**
         * When true, uses the parent as the anchor if the anchor doesn't exist or if
         * the anchor's visibility is GONE.
         */
        Boolean mAlignWithParent;
    };

public:
    CAR_INTERFACE_DECL()

    RelativeLayout();

    ~RelativeLayout();

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

    virtual CARAPI SetIgnoreGravity(
        /* [in] */ Int32 viewId);

    virtual CARAPI GetGravity(
        /* [out] */ Int32* gravity);

    virtual CARAPI SetGravity(
        /* [in] */ Int32 gravity);

    virtual CARAPI SetHorizontalGravity(
        /* [in] */ Int32 horizontalGravity);

    virtual CARAPI SetVerticalGravity(
        /* [in] */ Int32 verticalGravity);

    virtual CARAPI GetBaseline(
        /* [out] */ Int32* baseline);

    virtual CARAPI RequestLayout();

    virtual CARAPI GenerateLayoutParams(
        /* [in] */ IAttributeSet* attrs,
        /* [out] */ IViewGroupLayoutParams** params);

    virtual CARAPI_(AutoPtr<IViewGroupLayoutParams>) GenerateLayoutParams(
        /* [in] */ IViewGroupLayoutParams* p);

    // @Override
    virtual CARAPI DispatchPopulateAccessibilityEvent(
        /* [in] */ IAccessibilityEvent* event,
        /* [out] */ Boolean* res);

    virtual CARAPI ShouldDelayChildPressedState(
        /* [out] */ Boolean* res);

    virtual CARAPI OnInitializeAccessibilityEvent(
        /* [in] */ IAccessibilityEvent* event);

    //@Override
    virtual CARAPI OnInitializeAccessibilityNodeInfo(
        /* [in] */ IAccessibilityNodeInfo* info);

protected:
    //TODO: we need to find another way to implement RelativeLayout
    // This implementation cannot handle every case
    virtual CARAPI OnMeasure(
        /* [in] */ Int32 widthMeasureSpec,
        /* [in] */ Int32 heightMeasureSpec);

    virtual CARAPI OnLayout(
        /* [in] */ Boolean changed,
        /* [in] */ Int32 l,
        /* [in] */ Int32 t,
        /* [in] */ Int32 r,
        /* [in] */ Int32 b);

    /**
     * Returns a set of layout parameters with a width of
     * {@link android.view.ViewGroup.LayoutParams#WRAP_CONTENT},
     * a height of {@link android.view.ViewGroup.LayoutParams#WRAP_CONTENT} and no spanning.
     */
    virtual CARAPI GenerateDefaultLayoutParams(
        /* [out] */ IViewGroupLayoutParams** lp);

    // Override to allow type-checking of LayoutParams.
    virtual CARAPI_(Boolean) CheckLayoutParams(
        /* [in] */ IViewGroupLayoutParams* p);

private:
    CARAPI_(void) SortChildren();

    CARAPI_(void) AlignBaseline(
        /* [in] */ IView* child,
        /* [in] */ RelativeLayoutLayoutParams* params);

    using ViewGroup::MeasureChild;

    /**
     * Measure a child. The child should have left, top, right and bottom information
     * stored in its RelativeLayoutLayoutParams. If any of these values is -1 it means that the view
     * can extend up to the corresponding edge.
     *
     * @param child Child to measure
     * @param params LayoutParams associated with child
     * @param myWidth Width of the the RelativeLayout
     * @param myHeight Height of the RelativeLayout
     */
    CARAPI_(void) MeasureChild(
        /* [in] */ IView* child,
        /* [in] */ RelativeLayoutLayoutParams* params,
        /* [in] */ Int32 myWidth,
        /* [in] */ Int32 myHeight);

    CARAPI_(void) MeasureChildHorizontal(
        /* [in] */ IView* child,
        /* [in] */ RelativeLayoutLayoutParams* params,
        /* [in] */ Int32 myWidth,
        /* [in] */ Int32 myHeight);

    /**
     * Get a measure spec that accounts for all of the constraints on this view.
     * This includes size contstraints imposed by the RelativeLayout as well as
     * the View's desired dimension.
     *
     * @param childStart The left or top field of the child's layout params
     * @param childEnd The right or bottom field of the child's layout params
     * @param childSize The child's desired size (the width or height field of
     *        the child's layout params)
     * @param startMargin The left or top margin
     * @param endMargin The right or bottom margin
     * @param startPadding mPaddingLeft or mPaddingTop
     * @param endPadding mPaddingRight or mPaddingBottom
     * @param mySize The width or height of this view (the RelativeLayout)
     * @return MeasureSpec for the child
     */
    CARAPI_(Int32) GetChildMeasureSpec(
        /* [in] */ Int32 childStart,
        /* [in] */ Int32 childEnd,
        /* [in] */ Int32 childSize,
        /* [in] */ Int32 startMargin,
        /* [in] */ Int32 endMargin,
        /* [in] */ Int32 startPadding,
        /* [in] */ Int32 endPadding,
        /* [in] */ Int32 mySize);

    CARAPI_(Boolean) PositionChildHorizontal(
        /* [in] */ IView* child,
        /* [in] */ RelativeLayoutLayoutParams* params,
        /* [in] */ Int32 myWidth,
        /* [in] */ Boolean wrapContent);

    CARAPI_(Boolean) PositionChildVertical(
        /* [in] */ IView* child,
        /* [in] */ RelativeLayoutLayoutParams* params,
        /* [in] */ Int32 myHeight,
        /* [in] */ Boolean wrapContent);

    CARAPI_(void) ApplyHorizontalSizeRules(
        /* [in] */ RelativeLayoutLayoutParams* childParams,
        /* [in] */ Int32 myWidth,
        /* [in] */ ArrayOf<Int32>* rules);

    CARAPI_(void) ApplyVerticalSizeRules(
        /* [in] */ RelativeLayoutLayoutParams* childParams,
        /* [in] */ Int32 myHeight);

    CARAPI_(AutoPtr<IView>) GetRelatedView(
        /* [in] */ ArrayOf<Int32>* rules,
        /* [in] */ Int32 relation);

    CARAPI_(AutoPtr<RelativeLayoutLayoutParams>) GetRelatedViewParams(
        /* [in] */ ArrayOf<Int32>* rules,
        /* [in] */ Int32 relation);

    CARAPI_(Int32) GetRelatedViewBaseline(
        /* [in] */ ArrayOf<Int32>* rules,
        /* [in] */ Int32 relation);

    static CARAPI_(void) CenterHorizontal(
        /* [in] */ IView* child,
        /* [in] */ RelativeLayoutLayoutParams* params,
        /* [in] */ Int32 myWidth);

    static CARAPI_(void) CenterVertical(
        /* [in] */ IView* child,
        /* [in] */ RelativeLayoutLayoutParams* params,
        /* [in] */ Int32 myHeight);

    CARAPI InitFromAttributes(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs,
        /* [in] */ Int32 defStyleAttr,
        /* [in] */ Int32 defStyleRes);

    CARAPI_(void) QueryCompatibilityModes(
        /* [in] */ IContext* context);

private:
    friend class RelativeLayoutLayoutParams;

    ECO_LOCAL static const Int32 VERB_COUNT;

    ECO_LOCAL static const AutoPtr<ArrayOf<Int32> > RULES_VERTICAL;
    ECO_LOCAL static const AutoPtr<ArrayOf<Int32> > RULES_HORIZONTAL;
    /**
     * Used to indicate left/right/top/bottom should be inferred from constraints
     */
    ECO_LOCAL static const Int32 VALUE_NOT_SET;

    AutoPtr<IView> mBaselineView;
    Boolean mHasBaselineAlignedChild;
    Int32 mGravity;
    AutoPtr<IRect> mContentBounds;
    AutoPtr<IRect> mSelfBounds;
    Int32 mIgnoreGravity;

    Set< AutoPtr<IView> > mTopToBottomLeftToRightSet;
    Boolean mDirtyHierarchy;
    AutoPtr<ArrayOf<IView*> > mSortedHorizontalChildren;
    AutoPtr<ArrayOf<IView*> > mSortedVerticalChildren;
    AutoPtr<DependencyGraph> mGraph;
    // Compatibility hack. Old versions of the platform had problems
    // with MeasureSpec value overflow and RelativeLayout was one source of them.
    // Some apps came to rely on them. :(
    Boolean mAllowBrokenMeasureSpecs;
    // Compatibility hack. Old versions of the platform would not take
    // margins and padding into account when generating the height measure spec
    // for children during the horizontal measure pass.
    Boolean mMeasureVerticalWithPaddingMargin;

    // A default width used for RTL measure pass
    /**
     * Value reduced so as not to interfere with View's measurement spec. flags. See:
     * {@link View#MEASURED_SIZE_MASK}.
     * {@link View#MEASURED_STATE_TOO_SMALL}.
     **/
    ECO_LOCAL static const Int32 DEFAULT_WIDTH;
};

} // namespace Widget
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_WIDGET_RELATIVELAYOUT_H__
