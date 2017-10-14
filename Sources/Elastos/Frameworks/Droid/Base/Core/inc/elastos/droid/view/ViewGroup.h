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

#ifndef __ELASTOS_DROID_VIEW_ViewGroup_H__
#define __ELASTOS_DROID_VIEW_ViewGroup_H__

#include <Elastos.CoreLibrary.Utility.h>
#include "Elastos.Droid.Animation.h"
#include "Elastos.Droid.Content.h"
#include <elastos/droid/view/View.h>

using Elastos::Droid::Animation::ITransitionListener;
using Elastos::Droid::Animation::ILayoutTransition;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::Res::ITypedArray;
using Elastos::Droid::Graphics::IPointF;
using Elastos::Droid::Graphics::ICanvas;
using Elastos::Droid::Graphics::IPaint;
using Elastos::Droid::Utility::IAttributeSet;
using Elastos::Droid::View::Animation::IAnimationAnimationListener;
using Elastos::Droid::View::Animation::IAnimationParameters;
using Elastos::Droid::View::Animation::ILayoutAnimationController;
using Elastos::Core::IComparable;

namespace Elastos {
namespace Droid {
namespace View {

class COverlayViewGroup;

class ECO_PUBLIC ViewGroup
    : public View
    , public IViewGroup
    , public IViewParent
    , public IViewManager
{
    friend class View;
    friend class COverlayViewGroup;

public:
    class LayoutParams
        : public Object
        , public IViewGroupLayoutParams
    {
    public:
        CAR_INTERFACE_DECL()

        /**
         * Used internally by MarginLayoutParams.
         * @hide
         */
        LayoutParams();

        virtual ~LayoutParams();

        CARAPI constructor();

        CARAPI constructor(
            /* [in] */ IContext* c,
            /* [in] */ IAttributeSet* attrs);

        CARAPI constructor(
            /* [in] */ Int32 width,
            /* [in] */ Int32 height);

        CARAPI constructor(
            /* [in] */ IViewGroupLayoutParams* source);

        virtual CARAPI SetWidth(
            /* [in] */ Int32 width);

        virtual CARAPI SetHeight(
            /* [in] */ Int32 height);

        virtual CARAPI GetWidth(
            /* [out] */ Int32* width);

        virtual CARAPI GetHeight(
            /* [out] */ Int32* height);

        virtual CARAPI SetLayoutAnimationParameters(
            /* [in] */ IAnimationParameters* ap);

        virtual CARAPI GetLayoutAnimationParameters(
            /* [out] */ IAnimationParameters** ap);

        virtual CARAPI ResolveLayoutDirection(
            /* [in] */ Int32 layoutDirection);

        /**
         * Returns a String representation of this set of layout parameters.
         *
         * @param output the String to prepend to the internal representation
         * @return a String with the following format: output +
         *         "ViewGroup.LayoutParams={ width=WIDTH, height=HEIGHT }"
         *
         * @hide
         */
        virtual CARAPI Debug(
            /* [in] */ const String& output,
            /* [out] */ String* result);

        virtual CARAPI OnDebugDraw(
            /* [in] */ IView* view,
            /* [in] */ ICanvas* canvas,
            /* [in] */ IPaint* paint);

    protected:
        /**
         * Extracts the layout parameters from the supplied attributes.
         *
         * @param a the style attributes to extract the parameters from
         * @param widthAttr the identifier of the width attribute
         * @param heightAttr the identifier of the height attribute
         */
        virtual CARAPI SetBaseAttributes(
            /* [in] */ ITypedArray* a,
            /* [in] */ Int32 widthAttr,
            /* [in] */ Int32 heightAttr);

        /**
         * Converts the specified size to a readable String.
         *
         * @param size the size to convert
         * @return a String instance representing the supplied size
         *
         * @hide
         */
        static CARAPI SizeToString(
            /* [in] */ Int32 size,
            /* [out] */ String* des);

    public:
        /**
         * Information about how wide the view wants to be. Can be one of the
         * constants FILL_PARENT (replaced by MATCH_PARENT ,
         * in API Level 8) or WRAP_CONTENT. or an exact size.
         */
        Int32 mWidth;

        /**
         * Information about how tall the view wants to be. Can be one of the
         * constants FILL_PARENT (replaced by MATCH_PARENT ,
         * in API Level 8) or WRAP_CONTENT. or an exact size.
         */
        Int32 mHeight;

        /**
         * Used to animate layouts.
         */
        AutoPtr<IAnimationParameters> mLayoutAnimationParameters;
    };

    class MarginLayoutParams
        : public LayoutParams
        , public IViewGroupMarginLayoutParams
    {
    public:
        MarginLayoutParams();

        CAR_INTERFACE_DECL()

        virtual CARAPI GetLeftMargin(
            /* [out] */ Int32* leftMargin);

        virtual CARAPI SetLeftMargin(
            /* [in] */ Int32 leftMargin);

        /**
         * The top margin in pixels of the child.
         * Call {@link ViewGroup#setLayoutParams(LayoutParams)} after reassigning a new value
         * to this field.
         */
        virtual CARAPI GetTopMargin(
            /* [out] */ Int32* topMargin);

        virtual CARAPI SetTopMargin(
            /* [in] */ Int32 topMargin);

        /**
         * The right margin in pixels of the child.
         * Call {@link ViewGroup#setLayoutParams(LayoutParams)} after reassigning a new value
         * to this field.
         */
        virtual CARAPI GetRightMargin(
            /* [out] */ Int32* rightMargin);

        virtual CARAPI SetRightMargin(
            /* [in] */ Int32 rightMargin);

        /**
         * The bottom margin in pixels of the child.
         * Call {@link ViewGroup#setLayoutParams(LayoutParams)} after reassigning a new value
         * to this field.
         */
        virtual CARAPI GetBottomMargin(
            /* [out] */ Int32* bottomMargin);

        virtual CARAPI SetBottomMargin(
            /* [in] */ Int32 bottomMargin);

        /**
         * Sets the margins, in pixels.
         *
         * @param left the left margin size
         * @param top the top margin size
         * @param right the right margin size
         * @param bottom the bottom margin size
         *
         * @attr ref android.R.styleable#ViewGroup_MarginLayout_layout_marginLeft
         * @attr ref android.R.styleable#ViewGroup_MarginLayout_layout_marginTop
         * @attr ref android.R.styleable#ViewGroup_MarginLayout_layout_marginRight
         * @attr ref android.R.styleable#ViewGroup_MarginLayout_layout_marginBottom
         */
        virtual CARAPI SetMargins(
            /* [in] */ Int32 left,
            /* [in] */ Int32 top,
            /* [in] */ Int32 right,
            /* [in] */ Int32 bottom);

        virtual CARAPI GetMargins(
            /* [out] */ Int32* left,
            /* [out] */ Int32* top,
            /* [out] */ Int32* right,
            /* [out] */ Int32* bottom);

        virtual CARAPI SetMarginsRelative(
            /* [in] */ Int32 start,
            /* [in] */ Int32 top,
            /* [in] */ Int32 end,
            /* [in] */ Int32 bottom);

        virtual CARAPI SetMarginStart(
            /* [in] */ Int32 start);

        virtual CARAPI GetMarginStart(
            /* [out] */ Int32* start);

        virtual CARAPI SetMarginEnd(
            /* [in] */ Int32 end);

        virtual CARAPI GetMarginEnd(
            /* [out] */ Int32* end);

        virtual CARAPI IsMarginRelative(
            /* [out] */ Boolean* set);

        virtual CARAPI SetLayoutDirection(
            /* [in] */ Int32 layoutDirection);

        virtual CARAPI GetLayoutDirection(
            /* [out] */ Int32* layoutDirection);

        virtual CARAPI GetMarginFlags(
            /* [out] */ Byte* leftMargin);

        virtual CARAPI SetMarginFlags(
            /* [in] */ Byte leftMargin);

        /**
         * This will be called by {@link android.view.View#requestLayout()}. Left and Right margins
         * may be overridden depending on layout direction.
         */
        //@Override
        virtual CARAPI ResolveLayoutDirection(
            /* [in] */ Int32 layoutDirection);

        using LayoutParams::OnDebugDraw;

        virtual CARAPI OnDebugDraw(
            /* [in] */ IView* view,
            /* [in] */ ICanvas* canvas);

        virtual CARAPI IsLayoutRtl(
            /* [out] */ Boolean* rtl);

        CARAPI constructor(
            /* [in] */ IContext* c,
            /* [in] */ IAttributeSet* attrs);

        CARAPI constructor(
            /* [in] */ Int32 width,
            /* [in] */ Int32 height);

        CARAPI constructor(
            /* [in] */ IViewGroupMarginLayoutParams* source);

        CARAPI constructor(
            /* [in] */ IViewGroupLayoutParams* source);

        virtual CARAPI CopyMarginsFrom(
            /* [in] */ IViewGroupMarginLayoutParams* source);

    private:
        CARAPI_(void) DoResolveMargins();

    public:
        /**
         * The left margin in pixels of the child.
         * Call {@link ViewGroup#setLayoutParams(LayoutParams)} after reassigning a new value
         * to this field.
         */
        Int32 mLeftMargin;

        /**
         * The top margin in pixels of the child.
         * Call {@link ViewGroup#setLayoutParams(LayoutParams)} after reassigning a new value
         * to this field.
         */
        Int32 mTopMargin;

        /**
         * The right margin in pixels of the child.
         * Call {@link ViewGroup#setLayoutParams(LayoutParams)} after reassigning a new value
         * to this field.
         */
        Int32 mRightMargin;

        /**
         * The bottom margin in pixels of the child.
         * Call {@link ViewGroup#setLayoutParams(LayoutParams)} after reassigning a new value
         * to this field.
         */
        Int32 mBottomMargin;

    private:
        /**
         * The start margin in pixels of the child.
         * Call {@link ViewGroup#setLayoutParams(LayoutParams)} after reassigning a new value
         * to this field.
         */
        //@ViewDebug.ExportedProperty(category = "layout")
        Int32 mStartMargin;

        /**
         * The end margin in pixels of the child.
         * Call {@link ViewGroup#setLayoutParams(LayoutParams)} after reassigning a new value
         * to this field.
         */
        //@ViewDebug.ExportedProperty(category = "layout")
        Int32 mEndMargin;

        Byte mMarginFlags;

        static const Int32 DEFAULT_MARGIN_RELATIVE;
        static const Int32 LAYOUT_DIRECTION_MASK;
        static const Int32 LEFT_MARGIN_UNDEFINED_MASK;// = 0x00000004;
        static const Int32 RIGHT_MARGIN_UNDEFINED_MASK;// = 0x00000008;
        static const Int32 RTL_COMPATIBILITY_MODE_MASK;// = 0x00000010;
        static const Int32 NEED_RESOLUTION_MASK;// = 0x00000020;

        static const Int32 DEFAULT_MARGIN_RESOLVED;// = 0;
        static const Int32 UNDEFINED_MARGIN;// = DEFAULT_MARGIN_RELATIVE;
    };

    friend class GhostView;

private:
    class NotifyAnimationListenerRunnable
        : public Runnable
    {
    public:
        NotifyAnimationListenerRunnable(
            /* [in] */ ViewGroup* host);

        CARAPI Run();

    private:
        ViewGroup*   mHost;
    };

    class DispatchDrawRunnable
        : public Runnable
    {
    public:
        DispatchDrawRunnable(
            /* [in] */ ViewGroup* host);

        CARAPI Run();

    private:
        ViewGroup*   mHost;
    };

    class LayoutTransitionListener
        : public Object
        , public ITransitionListener
    {
    public:
        CAR_INTERFACE_DECL()

        LayoutTransitionListener(
            /* [in] */ ViewGroup* host);

        //@Override
        CARAPI StartTransition(
            /* [in] */ ILayoutTransition* transition,
            /* [in] */ IViewGroup* container,
            /* [in] */ IView* view,
            /* [in] */ Int32 transitionType);

        //@Override
        CARAPI EndTransition(
            /* [in] */ ILayoutTransition* transition,
            /* [in] */ IViewGroup* container,
            /* [in] */ IView* view,
            /* [in] */ Int32 transitionType);

    private:
        ViewGroup*  mHost;
    };

    /* Describes a touched view and the ids of the pointers that it has captured.
     *
     * This code assumes that pointer ids are always in the range 0..31 such that
     * it can use a bitfield to track which pointer ids are present.
     * As it happens, the lower layers of the input dispatch pipeline also use the
     * same trick so the assumption should be safe here...
     */
    class TouchTarget
        : public Object
    {
    public:
        static CARAPI_(AutoPtr<TouchTarget>) Obtain(
            /* [in] */ IView* child,
            /* [in] */ Int32 pointerIdBits);

        CARAPI_(void) Recycle();

    private:
        TouchTarget();

    public:
        static const Int32 ALL_POINTER_IDS; // all ones

        // The touched child view.
        AutoPtr<IView> mChild;

        // The combined bit mask of pointer ids for all pointers captured by the target.
        Int32 mPointerIdBits;

        // The next target in the target list.
        AutoPtr<TouchTarget> mNext;

    private:
        static const Int32 MAX_RECYCLED;
        static Object sRecycleLock;
        static AutoPtr<TouchTarget> sRecycleBin;
        static Int32 sRecycledCount;
    };

    /* Describes a hovered view. */
    class HoverTarget
        : public Object
    {
    public:
        static CARAPI_(AutoPtr<HoverTarget>) Obtain(
            /* [in] */ IView* child);

        CARAPI_(void) Recycle();

    private:
        HoverTarget();

    public:
        // The hovered child view.
        AutoPtr<IView> mChild;

        // The next target in the target list.
        AutoPtr<HoverTarget> mNext;

    private:
        static const Int32 MAX_RECYCLED;
        static Object sRecycleLock;
        static AutoPtr<HoverTarget> sRecycleBin;
        static Int32 sRecycledCount;
    };

    class ViewLocationHolder;

    /**
     * Pooled class that orderes the children of a ViewGroup from start
     * to end based on how they are laid out and the layout direction.
     */
    class ChildListForAccessibility
        : public Object
    {
    public:
        static CARAPI_(AutoPtr<ChildListForAccessibility>) Obtain(
            /* [in] */ IViewGroup* parent,
            /* [in] */ Boolean sort);

        CARAPI_(void) Recycle();

        CARAPI_(Int32) GetChildCount();

        CARAPI_(AutoPtr<IView>) GetChildAt(
            /* [in] */ Int32 index);

        CARAPI_(Int32) GetChildIndex(
            /* [in] */ IView* child);

    private:
        CARAPI Sort(
            /* [in] */ IList* holders);

    private:
        CARAPI_(void) Init(
            /* [in] */ IViewGroup* parent,
            /* [in] */ Boolean sort);

        CARAPI_(void) Clear();

        ChildListForAccessibility();

    private:
        static const Int32 MAX_POOL_SIZE;

        static AutoPtr< Pools::SynchronizedPool<ChildListForAccessibility> > sPool;//= new SynchronizedPool<ChildListForAccessibility>(MAX_POOL_SIZE);

        AutoPtr<IList> mChildren;

        AutoPtr<IList> mHolders;
    };

    /**
     * Pooled class that holds a View and its location with respect to
     * a specified root. This enables sorting of views based on their
     * coordinates without recomputing the position relative to the root
     * on every comparison.
     */
    class ViewLocationHolder
        : public Object
    {
    public:
        static CARAPI_(AutoPtr<ViewLocationHolder>) Obtain(
            /* [in] */ IViewGroup* root,
            /* [in] */ IView* view);

        static CARAPI_(void) SetComparisonStrategy(
            /* [in] */ Int32 strategy);

        CARAPI_(void) Recycle();

        CARAPI_(Int32) CompareTo(
            /* [in] */ ViewLocationHolder* _another);

    private:
        CARAPI_(void) Init(
            /* [in] */ IViewGroup* root,
            /* [in] */ IView* view);

        CARAPI_(void) Clear();

        ViewLocationHolder();

    public:
        View* mView;

        static const Int32 COMPARISON_STRATEGY_STRIPE = 1;

        static const Int32 COMPARISON_STRATEGY_LOCATION = 2;

    private:
        static const Int32 MAX_POOL_SIZE;
        static AutoPtr<Pools::SynchronizedPool<ViewLocationHolder> > sPool;// = new SynchronizedPool<ViewLocationHolder>(MAX_POOL_SIZE);

        static Int32 sComparisonStrategy;
        AutoPtr<IRect> mLocation;

        Int32 mLayoutDirection;
    };


public:

    CAR_INTERFACE_DECL()

    ViewGroup();

    ~ViewGroup();

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

    virtual CARAPI GetDescendantFocusability(
        /* [out] */ Int32* res);

    virtual CARAPI SetDescendantFocusability(
        /* [in] */ Int32 focusability);

    CARAPI HandleFocusGainInternal(
        /* [in] */ Int32 direction,
        /* [in] */ IRect* previouslyFocusedRect);

    virtual CARAPI RequestChildFocus(
        /* [in] */ IView* child,
        /* [in] */ IView* focused);

    virtual CARAPI FocusableViewAvailable(
        /* [in] */ IView* v);

    virtual CARAPI ShowContextMenuForChild(
        /* [in] */ IView* originalView,
        /* [out] */ Boolean* res);

    /**
     * {@inheritDoc}
     */
    virtual CARAPI StartActionModeForChild(
        /* [in] */ IView* originalView,
        /* [in] */ IActionModeCallback* callback,
        /* [out] */ IActionMode** res);

    using View::FocusSearch;

    virtual CARAPI FocusSearch(
        /* [in] */ IView* focused,
        /* [in] */ Int32 direction,
        /* [out] */ IView** res);

    virtual CARAPI RequestChildRectangleOnScreen(
        /* [in] */ IView* child,
        /* [in] */ IRect* rectangle,
        /* [in] */ Boolean immediate,
        /* [out] */ Boolean* res);

    /**
     * {@inheritDoc}
     */
    //@Override
    virtual CARAPI RequestSendAccessibilityEvent(
        /* [in] */ IView* child,
        /* [in] */ IAccessibilityEvent* event,
        /* [out] */ Boolean* res);

    virtual CARAPI OnRequestSendAccessibilityEvent(
        /* [in] */ IView* child,
        /* [in] */ IAccessibilityEvent* event,
        /* [out] */ Boolean* res);

    virtual CARAPI ChildHasTransientStateChanged(
        /* [in] */ IView* child,
        /* [in] */ Boolean childHasTransientState);

    /**
     * @hide
     */
    //@Override
    virtual CARAPI HasTransientState(
        /* [out] */ Boolean* res);

    CARAPI DispatchUnhandledMove(
        /* [in] */ IView* focused,
        /* [in] */ Int32 direction,
        /* [out] */ Boolean* res);

    virtual CARAPI ClearChildFocus(
        /* [in] */ IView* child);

    CARAPI ClearFocus();

    CARAPI UnFocus(
        /* [in] */ IView* focused);

    CARAPI GetDeepestFocusedChild(
        /* [out] */ IView** focused);

    virtual CARAPI GetFocusedChild(
        /* [out] */ IView** res);

    CARAPI HasFocus(
        /* [out] */ Boolean* res);

    CARAPI FindFocus(
        /* [out] */ IView** res);

    CARAPI HasFocusable(
        /* [out] */ Boolean* res);

    using View::AddFocusables;

    CARAPI AddFocusables(
        /* [in] */ IList* views,
        /* [in] */ Int32 direction,
        /* [in] */ Int32 focusableMode);

    CARAPI FindViewsWithText(
        /* [in, out] */ IArrayList* outViews,
        /* [in] */ ICharSequence* searched,
        /* [in] */ Int32 flags);

    CARAPI FindViewByAccessibilityIdTraversal(
        /* [in] */ Int32 accessibilityId,
        /* [out] */ IView** res);

    CARAPI AddTouchables(
        /* [in] */ IArrayList* views);

    CARAPI MakeOptionalFitsSystemWindows();

    CARAPI DispatchWindowFocusChanged(
        /* [in] */ Boolean hasFocus);

    CARAPI DispatchDisplayHint(
        /* [in] */ Int32 hint);

    CARAPI_(void) DispatchVisibilityChanged(
        /* [in] */ IView* changedView,
        /* [in] */ Int32 visibility);

    CARAPI DispatchWindowVisibilityChanged(
        /* [in] */ Int32 visibility);

    CARAPI DispatchConfigurationChanged(
        /* [in] */ IConfiguration* newConfig);

    virtual CARAPI RecomputeViewAttributes(
        /* [in] */ IView* child);

    CARAPI DispatchCollectViewAttributes(
        /* [in] */ AttachInfo* info,
        /* [in] */ Int32 visibility);

    virtual CARAPI BringChildToFront(
        /* [in] */ IView* child);

    /**
     * {@inheritDoc}
     */
    // TODO: Write real docs
    //@Override
    virtual CARAPI DispatchDragEvent(
        /* [in] */ IDragEvent* event,
        /* [out] */ Boolean* res);

    virtual CARAPI DispatchWindowSystemUiVisiblityChanged(
        /* [in] */ Int32 visible);

    virtual CARAPI DispatchSystemUiVisibilityChanged(
        /* [in] */ Int32 visible);

    virtual CARAPI DispatchKeyEventPreIme(
        /* [in] */ IKeyEvent* event,
        /* [out] */ Boolean* res);

    virtual CARAPI DispatchKeyEvent(
        /* [in] */ IKeyEvent* event,
        /* [out] */ Boolean* res);

    virtual CARAPI DispatchKeyShortcutEvent(
        /* [in] */ IKeyEvent* event,
        /* [out] */ Boolean* res);

    virtual CARAPI DispatchTrackballEvent(
        /* [in] */ IMotionEvent* event,
        /* [out] */ Boolean* res);

    virtual CARAPI AddChildrenForAccessibility(
        /* [in] */ IArrayList* childrenForAccessibility);

    virtual CARAPI OnInterceptHoverEvent(
        /* [in] */ IMotionEvent* event,
        /* [out] */ Boolean* res);

    virtual CARAPI DispatchTouchEvent(
        /* [in] */ IMotionEvent* ev,
        /* [out] */ Boolean* res);

    virtual CARAPI RequestDisallowInterceptTouchEvent(
        /* [in] */ Boolean disallowIntercept);

    virtual CARAPI OnInterceptTouchEvent(
        /* [in] */ IMotionEvent* ev,
        /* [out] */ Boolean* res);

    CARAPI RequestFocus(
        /* [in] */ Int32 direction,
        /* [in] */ IRect* previouslyFocusedRect,
        /* [out] */ Boolean* res);

    using View::RequestFocus;

    CARAPI DispatchStartTemporaryDetach();

    CARAPI DispatchFinishTemporaryDetach();

    CARAPI DispatchAttachedToWindow(
        /* [in] */ AttachInfo* info,
        /* [in] */ Int32 visibility);

    CARAPI DispatchDetachedFromWindow();

    CARAPI SetClipChildren(
        /* [in] */ Boolean clipChildren);

    CARAPI SetClipToPadding(
        /* [in] */ Boolean clipToPadding);

    CARAPI DispatchSetSelected(
        /* [in] */ Boolean selected);

    /**
     * {@inheritDoc}
     */
    //@Override
    CARAPI DispatchSetActivated(
        /* [in] */ Boolean activated);

    virtual CARAPI AddView(
        /* [in] */ IView* child);

    virtual CARAPI AddView(
        /* [in] */ IView* child,
        /* [in] */ Int32 index);

    virtual CARAPI AddView(
        /* [in] */ IView* child,
        /* [in] */ Int32 width,
        /* [in] */ Int32 height);

    virtual CARAPI AddView(
        /* [in] */ IView* child,
        /* [in] */ IViewGroupLayoutParams* params);

    virtual CARAPI AddView(
        /* [in] */ IView* child,
        /* [in] */ Int32 index,
        /* [in] */ IViewGroupLayoutParams* params);

    virtual CARAPI UpdateViewLayout(
        /* [in] */ IView* view,
        /* [in] */ IViewGroupLayoutParams*  params);

    /**
     * Register a callback to be invoked when a child is added to or removed
     * from this view.
     *
     * @param listener the callback to invoke on hierarchy change
     */
    virtual CARAPI SetOnHierarchyChangeListener(
        /* [in] */ IViewGroupOnHierarchyChangeListener* listener);

    virtual CARAPI RemoveView(
        /* [in] */ IView* view);

    /**
     * Removes a view during layout. This is useful if in your onLayout() method,
     * you need to remove more views.
     *
     * @param view the view to remove from the group
     */
    virtual CARAPI RemoveViewInLayout(
        /* [in] */ IView* view);

    /**
     * Removes a range of views during layout. This is useful if in your onLayout() method,
     * you need to remove more views.
     *
     * @param start the index of the first view to remove from the group
     * @param count the number of views to remove from the group
     */
    virtual CARAPI RemoveViewsInLayout(
        /* [in] */ Int32 start,
        /* [in] */ Int32 count);

    /**
     * Removes the view at the specified position in the group.
     *
     * @param index the position in the group of the view to remove
     */
    virtual CARAPI RemoveViewAt(
        /* [in] */ Int32 index);

    /**
     * Removes the specified range of views from the group.
     *
     * @param start the first position in the group of the range of views to remove
     * @param count the number of views to remove
     */
    virtual CARAPI RemoveViews(
        /* [in] */ Int32 start,
        /* [in] */ Int32 count);

    /**
     * Call this method to remove all child views from the
     * ViewGroup.
     */
    virtual CARAPI RemoveAllViews();

    /**
     * Called by a ViewGroup subclass to remove child views from itself,
     * when it must first know its size on screen before it can calculate how many
     * child views it will render. An example is a Gallery or a ListView, which
     * may "have" 50 children, but actually only render the number of children
     * that can currently fit inside the object on screen. Do not call
     * this method unless you are extending ViewGroup and understand the
     * view measuring and layout pipeline.
     */
    virtual CARAPI RemoveAllViewsInLayout();

    /**
     * Don't call or override this method. It is used for the implementation of
     * the view hierarchy.
     */
    virtual CARAPI InvalidateChild(
        /* [in] */ IView* child,
        /* [in] */ IRect* dirty);

    /**
     * Don't call or override this method. It is used for the implementation of
     * the view hierarchy.
     *
     * This implementation returns NULL if this ViewGroup does not have a parent,
     * if this ViewGroup is already fully invalidated or if the dirty rectangle
     * does not intersect with this ViewGroup's bounds.
     */
    virtual CARAPI InvalidateChildInParent(
        /* [in] */ ArrayOf<Int32>* location,
        /* [in] */ IRect* dirty,
        /* [out] */ IViewParent** parent);

    /**
     * Quick invalidation method called by View.invalidateViewProperty. This doesn't set the
     * DRAWN flags and doesn't handle the Animation logic that the default invalidation methods
     * do; all we want to do here is schedule a traversal with the appropriate dirty rect.
     *
     * @hide
     */
    virtual CARAPI DamageChild(
        /* [in] */ IView* child,
        /* [in] */ IRect* dirty);

    /**
     * Offset a rectangle that is in a descendant's coordinate
     * space into our coordinate space.
     * @param descendant A descendant of this view
     * @param rect A rectangle defined in descendant's coordinate space.
     */
    CARAPI OffsetDescendantRectToMyCoords(
        /* [in] */ IView* descendant,
        /* [in] */ IRect* rect);

    /**
     * Offset a rectangle that is in our coordinate space into an ancestor's
     * coordinate space.
     * @param descendant A descendant of this view
     * @param rect A rectangle defined in descendant's coordinate space.
     */
    CARAPI OffsetRectIntoDescendantCoords(
        /* [in] */ IView* descendant,
        /* [in] */ IRect* rect);

    /**
     * Helper method that offsets a rect either from parent to descendant or
     * descendant to parent.
     */
    virtual CARAPI OffsetRectBetweenParentAndChild(
        /* [in] */ IView* descendant,
        /* [in] */ IRect* rect,
        /* [in] */ Boolean offsetFromChildToParent,
        /* [in] */ Boolean clipToBounds);

    /**
     * Offset the vertical location of all children of this view by the specified number of pixels.
     *
     * @param offset the number of pixels to offset
     *
     * @hide
     */
    virtual CARAPI OffsetChildrenTopAndBottom(
        /* [in] */ Int32 offset);

    virtual CARAPI GetChildVisibleRect(
        /* [in] */ IView* child,
        /* [in] */ IRect* r,
        /* [in] */ IPoint* offset,
        /* [out] */ Boolean* result);

    /**
     * Runs the layout animation. Calling this method triggers a relayout of
     * this view group.
     */
    virtual CARAPI StartLayoutAnimation();

    /**
     * Schedules the layout animation to be played after the next layout pass
     * of this view group. This can be used to restart the layout animation
     * when the content of the view group changes or when the activity is
     * paused and resumed.
     */
    virtual CARAPI ScheduleLayoutAnimation();

    /**
     * Sets the layout animation controller used to animate the group's
     * children after the first layout.
     *
     * @param controller the animation controller
     */
    virtual CARAPI SetLayoutAnimation(
        /* [in] */ ILayoutAnimationController* controller);

    /**
     * Returns the layout animation controller used to animate the group's
     * children.
     *
     * @return the current animation controller
     */
    virtual CARAPI GetLayoutAnimation(
        /* [out] */ ILayoutAnimationController** res);

    /**
     * Indicates whether the children's drawing cache is used during a layout
     * animation. By default, the drawing cache is enabled but this will prevent
     * nested layout animations from working. To nest animations, you must disable
     * the cache.
     *
     * @return TRUE if the animation cache is enabled, FALSE otherwise
     *
     * @see #setAnimationCacheEnabled(Boolean)
     * @see View#setDrawingCacheEnabled(Boolean)
     */
    virtual CARAPI IsAnimationCacheEnabled(
        /* [out] */ Boolean* res);

    /**
     * Enables or disables the children's drawing cache during a layout animation.
     * By default, the drawing cache is enabled but this will prevent nested
     * layout animations from working. To nest animations, you must disable the
     * cache.
     *
     * @param enabled TRUE to enable the animation cache, FALSE otherwise
     *
     * @see #isAnimationCacheEnabled()
     * @see View#setDrawingCacheEnabled(Boolean)
     */
    virtual CARAPI SetAnimationCacheEnabled(
        /* [in] */ Boolean enabled);

    /**
     * Indicates whether this ViewGroup will always try to draw its children using their
     * drawing cache. By default this property is enabled.
     *
     * @return TRUE if the animation cache is enabled, FALSE otherwise
     *
     * @see #setAlwaysDrawnWithCacheEnabled(Boolean)
     * @see #setChildrenDrawnWithCacheEnabled(Boolean)
     * @see View#setDrawingCacheEnabled(Boolean)
     */
    virtual CARAPI IsAlwaysDrawnWithCacheEnabled(
        /* [out] */ Boolean* res);

    /**
     * Indicates whether this ViewGroup will always try to draw its children using their
     * drawing cache. This property can be set to TRUE when the cache rendering is
     * slightly different from the children's normal rendering. Renderings can be different,
     * for instance, when the cache's quality is set to low.
     *
     * When this property is disabled, the ViewGroup will use the drawing cache of its
     * children only when asked to. It's usually the task of subclasses to tell ViewGroup
     * when to start using the drawing cache and when to stop using it.
     *
     * @param always TRUE to always draw with the drawing cache, FALSE otherwise
     *
     * @see #isAlwaysDrawnWithCacheEnabled()
     * @see #setChildrenDrawnWithCacheEnabled(Boolean)
     * @see View#setDrawingCacheEnabled(Boolean)
     * @see View#setDrawingCacheQuality(Int32)
     */
    virtual CARAPI SetAlwaysDrawnWithCacheEnabled(
        /* [in] */ Boolean always);

    /**
     * Returns an integer indicating what types of drawing caches are kept in memory.
     *
     * @see #setPersistentDrawingCache(Int32)
     * @see #setAnimationCacheEnabled(Boolean)
     *
     * @return one or a combination of {@link #PERSISTENT_NO_CACHE},
     *         {@link #PERSISTENT_ANIMATION_CACHE}, {@link #PERSISTENT_SCROLLING_CACHE}
     *         and {@link #PERSISTENT_ALL_CACHES}
     */
    virtual CARAPI GetPersistentDrawingCache(
        /* [out] */ Int32* res);

    /**
     * Indicates what types of drawing caches should be kept in memory after
     * they have been created.
     *
     * @see #getPersistentDrawingCache()
     * @see #setAnimationCacheEnabled(Boolean)
     *
     * @param drawingCacheToKeep one or a combination of {@link #PERSISTENT_NO_CACHE},
     *        {@link #PERSISTENT_ANIMATION_CACHE}, {@link #PERSISTENT_SCROLLING_CACHE}
     *        and {@link #PERSISTENT_ALL_CACHES}
     */
    virtual CARAPI SetPersistentDrawingCache(
        /* [in] */ Int32 drawingCacheToKeep);

    virtual CARAPI GetLayoutMode(
        /* [out] */ Int32* res);

    virtual CARAPI SetLayoutMode(
        /* [in] */ Int32 layoutMode);

    /**
     * Returns a new set of layout parameters based on the supplied attributes set.
     *
     * @param attrs the attributes to build the layout parameters from
     *
     * @return an instance of {@link android.view.ViewGroup.LayoutParams} or one
     *         of its descendants
     */
    virtual CARAPI GenerateLayoutParams(
        /* [in] */ IAttributeSet* attrs,
        /* [out] */ IViewGroupLayoutParams** params);

    virtual CARAPI IndexOfChild(
        /* [in] */ IView* child,
        /* [out] */ Int32* res);

    virtual CARAPI GetChildCount(
        /* [out] */ Int32* res);

    virtual CARAPI GetChildAt(
        /* [in] */ Int32 index,
        /* [out] */ IView** res);

    static Int32 GetChildMeasureSpec(
        /* [in] */ Int32 spec,
        /* [in] */ Int32 padding,
        /* [in] */ Int32 childDimension);

    /**
     * Removes any pending animations for views that have been removed. Call
     * this if you don't want animations for exiting views to stack up.
     */
    virtual CARAPI ClearDisappearingChildren();

    CARAPI GatherTransparentRegion(
        /* [in, out] */ IRegion* region,
        /* [out] */ Boolean* res);

    virtual CARAPI RequestTransparentRegion(
        /* [in] */ IView* child);

    /**
     * Returns the animation listener to which layout animation events are
     * sent.
     *
     * @return an {@link android.view.animation.Animation.AnimationListener}
     */
    virtual CARAPI GetLayoutAnimationListener(
        /* [out] */ IAnimationAnimationListener** res);

    /**
     * Sets whether this ViewGroup's drawable states also include
     * its children's drawable states.  This is used, for example, to
     * make a group appear to be focused when its child EditText or button
     * is focused.
     */
    virtual CARAPI SetAddStatesFromChildren(
        /* [in] */ Boolean addsStates);

    /**
     * Returns whether this ViewGroup's drawable states also include
     * its children's drawable states.  This is used, for example, to
     * make a group appear to be focused when its child EditText or button
     * is focused.
     */
    virtual CARAPI AddStatesFromChildren(
        /* [out] */ Boolean* res);

    /**
     * If {link #addStatesFromChildren} is TRUE, refreshes this group's
     * drawable state (to include the states from its children).
     */
    virtual CARAPI ChildDrawableStateChanged(
        /* [in] */ IView* child);

    /**
     * Specifies the animation listener to which layout animation events must
     * be sent. Only
     * {@link android.view.animation.Animation.AnimationListener#onAnimationStart(Animation)}
     * and
     * {@link android.view.animation.Animation.AnimationListener#onAnimationEnd(Animation)}
     * are invoked.
     *
     * @param animationListener the layout animation listener
     */
    virtual CARAPI SetLayoutAnimationListener(
        /* [in] */ IAnimationAnimationListener* animationListener);

    virtual CARAPI SetMotionEventSplittingEnabled(
        /* [in] */ Boolean split);

    virtual CARAPI IsMotionEventSplittingEnabled(
        /* [out] */ Boolean* res);

    virtual CARAPI ResetSubtreeAccessibilityStateChanged();

    virtual CARAPI SetLayoutTransition(
        /* [in] */ ILayoutTransition* transition);

    virtual CARAPI GetLayoutTransition(
        /* [out] */ ILayoutTransition** res);

    /**
     * {@inheritDoc}
     */
    //@Override
    virtual CARAPI Layout(
        /* [in] */ Int32 l,
        /* [in] */ Int32 t,
        /* [in] */ Int32 r,
        /* [in] */ Int32 b);

    virtual CARAPI StartViewTransition(
        /* [in] */ IView* view);

    virtual CARAPI EndViewTransition(
        /* [in] */ IView* view);

    virtual CARAPI RequestTransitionStart(
        /* [in] */ ILayoutTransition* transition);

    virtual CARAPI ResolveRtlPropertiesIfNeeded(
        /* [out] */ Boolean* res);

    virtual CARAPI ResolveLayoutDirection(
        /* [out] */ Boolean* res);

    virtual CARAPI ResolveTextDirection(
        /* [out] */ Boolean* res);

    virtual CARAPI ResolveTextAlignment(
        /* [out] */ Boolean* res);

    virtual CARAPI ResolvePadding();

    virtual CARAPI ResolveLayoutParams();

    virtual CARAPI ResetResolvedLayoutDirection();

    virtual CARAPI ResetResolvedTextDirection();

    virtual CARAPI ResetResolvedTextAlignment();

    virtual CARAPI ResetResolvedPadding();

    virtual CARAPI ShouldDelayChildPressedState(
        /* [out] */ Boolean* res);

    virtual CARAPI SetTouchscreenBlocksFocus(
        /* [in] */ Boolean touchscreenBlocksFocus);

    virtual CARAPI GetTouchscreenBlocksFocus(
        /* [out] */ Boolean* res);

    virtual CARAPI IsTransitionGroup(
        /* [out] */ Boolean* res);

    virtual CARAPI SetTransitionGroup(
        /* [in] */ Boolean isTransitionGroup);

    virtual CARAPI NotifySubtreeAccessibilityStateChanged(
        /* [in] */ IView* child,
        /* [in] */ IView* source,
        /* [in] */ Int32 changeType);

    virtual CARAPI GetOverlay(
        /* [out] */ IViewOverlay** overlay);

    virtual CARAPI GetClipChildren(
        /* [out] */ Boolean* res);

    virtual CARAPI GetClipToPadding(
        /* [out] */ Boolean* res);

    virtual CARAPI DamageChildDeferred(
        /* [in] */ IView* child,
        /* [out] */ Boolean* res);

    virtual CARAPI SuppressLayout(
        /* [in] */ Boolean suppress);

    virtual CARAPI IsLayoutSuppressed(
        /* [out] */ Boolean* res);

    virtual CARAPI DispatchApplyWindowInsets(
        /* [in] */ IWindowInsets* insets,
        /* [out] */ IWindowInsets** res);

    virtual CARAPI DrawableHotspotChanged(
        /* [in] */ Float x,
        /* [in] */ Float y);

    virtual CARAPI OnStartNestedScroll(
        /* [in] */ IView* child,
        /* [in] */ IView* target,
        /* [in] */ Int32 nestedScrollAxes,
        /* [out] */ Boolean* res);

    virtual CARAPI OnNestedScrollAccepted(
        /* [in] */ IView* child,
        /* [in] */ IView* target,
        /* [in] */ Int32 axes);

    virtual CARAPI OnStopNestedScroll(
        /* [in] */ IView* child);

    virtual CARAPI OnNestedScroll(
        /* [in] */ IView* target,
        /* [in] */ Int32 dxConsumed,
        /* [in] */ Int32 dyConsumed,
        /* [in] */ Int32 dxUnconsumed,
        /* [in] */ Int32 dyUnconsumed);

    virtual CARAPI OnNestedPreScroll(
        /* [in] */ IView* target,
        /* [in] */ Int32 dx,
        /* [in] */ Int32 dy,
        /* [in] */ ArrayOf<Int32>* consumed);

    virtual CARAPI OnNestedFling(
        /* [in] */ IView* target,
        /* [in] */ Float velocityX,
        /* [in] */ Float velocityY,
        /* [in] */ Boolean consumed,
        /* [out] */ Boolean* res);

    virtual CARAPI OnNestedPreFling(
        /* [in] */ IView* target,
        /* [in] */ Float velocityX,
        /* [in] */ Float velocityY,
        /* [out] */ Boolean* res);

    virtual CARAPI GetNestedScrollAxes(
        /* [out] */ Int32* res);

    virtual CARAPI CaptureTransitioningViews(
        /* [in] */ IList* transitioningViews);

    virtual CARAPI FindNamedViews(
        /* [in] */ IMap* namedElements);

    virtual CARAPI ShouldBlockFocusForTouchscreen(
        /* [out] */ Boolean* should);

    // method of IViewParent
    CARAPI RequestLayout();

    CARAPI GetParent(
        /* [out] */ IViewParent** res);

    CARAPI IsLayoutRequested(
        /* [out] */ Boolean* result);

    CARAPI CreateContextMenu(
        /* [in] */ IContextMenu* menu);

    CARAPI RequestFitSystemWindows();

    CARAPI GetParentForAccessibility(
        /* [out] */ IViewParent** parent);

    CARAPI CanResolveLayoutDirection(
        /* [out] */ Boolean* result);

    CARAPI IsLayoutDirectionResolved(
        /* [out] */ Boolean* result);

    CARAPI GetLayoutDirection(
        /* [out] */ Int32* direction);

    CARAPI CanResolveTextDirection(
        /* [out] */ Boolean* result);

    CARAPI IsTextDirectionResolved(
        /* [out] */ Boolean* result);

    CARAPI GetTextDirection(
        /* [out] */ Int32* direction);

    CARAPI CanResolveTextAlignment(
        /* [out] */ Boolean* result);

    CARAPI IsTextAlignmentResolved(
        /* [out] */ Boolean* result);

    CARAPI GetTextAlignment(
        /* [out] */ Int32* result);

protected:
    CARAPI_(Boolean) OnRequestFocusInDescendants(
        /* [in] */ Int32 direction,
        /* [in] */ IRect* previouslyFocusedRect);

    CARAPI_(void) InternalSetPadding(
        /* [in] */ Int32 left,
        /* [in] */ Int32 top,
        /* [in] */ Int32 right,
        /* [in] */ Int32 bottom);

    CARAPI DispatchSaveInstanceState(
        /* [in] */ ISparseArray* container);

    /**
     * Perform dispatching of a {@link #saveHierarchyState(android.util.SparseArray)}  freeze()}
     * to only this view, not to its children.  For use when overriding
     * {@link #dispatchSaveInstanceState(android.util.SparseArray)}  dispatchFreeze()} to allow
     * subclasses to freeze their own state but not the state of their children.
     *
     * @param container the container
     */
    virtual CARAPI DispatchFreezeSelfOnly(
        /* [in] */ ISparseArray* container);

    CARAPI DispatchRestoreInstanceState(
        /* [in] */ ISparseArray* container);

    /**
     * Perform dispatching of a {@link #restoreHierarchyState(android.util.SparseArray)}
     * to only this view, not to its children.  For use when overriding
     * {@link #dispatchRestoreInstanceState(android.util.SparseArray)} to allow
     * subclasses to thaw their own state but not the state of their children.
     *
     * @param container the container
     */
    virtual CARAPI DispatchThawSelfOnly(
        /* [in] */ ISparseArray* container);

    CARAPI_(void) SetChildrenDrawingCacheEnabled(
        /* [in] */ Boolean enabled);

    CARAPI OnAnimationStart();

    CARAPI OnAnimationEnd();

    CARAPI CreateSnapshot(
        /* [in] */ BitmapConfig quality,
        /* [in] */ Int32 backgroundColor,
        /* [in] */ Boolean skipChildren,
        /* [out] */ IBitmap** bitmap);

    CARAPI DispatchDraw(
        /* [in] */ ICanvas* canvas);

    CARAPI_(Int32) GetChildDrawingOrder(
        /* [in] */ Int32 childCount,
        /* [in] */ Int32 i);

    virtual CARAPI_(Boolean) DrawChild(
        /* [in] */ ICanvas* canvas,
        /* [in] */ IView* child,
        /* [in] */ Int64 drawingTime);

    CARAPI_(void) DispatchSetPressed(
        /* [in] */ Boolean pressed);

    CARAPI_(void) SetStaticTransformationsEnabled(
        /* [in] */ Boolean enabled);

    virtual CARAPI_(Boolean) GetChildStaticTransformation(
        /* [in] */ IView* child,
        /* [in] */ ITransformation* t);

    CARAPI_(AutoPtr<IView>) FindViewTraversal(
        /* [in] */ Int32 id);

    CARAPI_(AutoPtr<IView>) FindViewWithTagTraversal(
        /* [in] */ IInterface* tag);

    virtual CARAPI_(Boolean) CheckLayoutParams(
        /* [in] */ IViewGroupLayoutParams* p);

    /**
     * Adds a view during layout. This is useful if in your onLayout() method,
     * you need to add more views (as does the list view for example).
     *
     * If index is negative, it means put it at the end of the list.
     *
     * @param child the view to add to the group
     * @param index the index at which the child must be added
     * @param params the layout parameters to associate with the child
     * @return TRUE if the child was added, FALSE otherwise
     */
    virtual CARAPI_(Boolean) AddViewInLayout(
        /* [in] */ IView* child,
        /* [in] */ Int32 index,
        /* [in] */ IViewGroupLayoutParams* params);

    /**
     * Adds a view during layout. This is useful if in your onLayout() method,
     * you need to add more views (as does the list view for example).
     *
     * If index is negative, it means put it at the end of the list.
     *
     * @param child the view to add to the group
     * @param index the index at which the child must be added
     * @param params the layout parameters to associate with the child
     * @param preventRequestLayout if TRUE, calling this method will not trigger a
     *        layout request on child
     * @return TRUE if the child was added, FALSE otherwise
     */
    virtual CARAPI_(Boolean) AddViewInLayout(
        /* [in] */ IView* child,
        /* [in] */ Int32 index,
        /* [in] */ IViewGroupLayoutParams* params,
        /* [in] */ Boolean preventRequestLayout);

    /**
     * Prevents the specified child to be laid out during the next layout pass.
     *
     * @param child the child on which to perform the cleanup
     */
    virtual CARAPI_(void) CleanupLayoutState(
        /* [in] */ IView* child);

    /**
     * Subclasses should override this method to set layout animation
     * parameters on the supplied child.
     *
     * @param child the child to associate with animation parameters
     * @param params the child's layout parameters which hold the animation
     *        parameters
     * @param index the index of the child in the view group
     * @param count the number of children in the view group
     */
    virtual CARAPI AttachLayoutAnimationParameters(
        /* [in] */ IView* child,
        /* [in] */ IViewGroupLayoutParams* params,
        /* [in] */ Int32 index,
        /* [in] */ Int32 count);

    /**
     * Finishes the removal of a detached view. This method will dispatch the detached from
     * window event and notify the hierarchy change listener.
     *
     * @param child the child to be definitely removed from the view hierarchy
     * @param animate if TRUE and the view has an animation, the view is placed in the
     *                disappearing views list, otherwise, it is detached from the window
     *
     * @see #attachViewToParent(View, Int32, android.view.ViewGroup.LayoutParams)
     * @see #detachAllViewsFromParent()
     * @see #detachViewFromParent(View)
     * @see #detachViewFromParent(Int32)
     */
    virtual CARAPI_(void) RemoveDetachedView(
        /* [in] */ IView* child,
        /* [in] */ Boolean animate);

    /**
     * Attaches a view to this view group. Attaching a view assigns this group as the parent,
     * sets the layout parameters and puts the view in the list of children so that
     * it can be retrieved by calling {@link #getChildAt(int)}.
     * <p>
     * This method is intended to be lightweight and makes no assumptions about whether the
     * parent or child should be redrawn. Proper use of this method will include also making
     * any appropriate {@link #requestLayout()} or {@link #invalidate()} calls.
     * For example, callers can {@link #post(Runnable) post} a {@link Runnable}
     * which performs a {@link #requestLayout()} on the next frame, after all detach/attach
     * calls are finished, causing layout to be run prior to redrawing the view hierarchy.
     * <p>
     * This method should be called only for views which were detached from their parent.
     *
     * @param child the child to attach
     * @param index the index at which the child should be attached
     * @param params the layout parameters of the child
     *
     * @see #removeDetachedView(View, boolean)
     * @see #detachAllViewsFromParent()
     * @see #detachViewFromParent(View)
     * @see #detachViewFromParent(int)
     */
    virtual CARAPI_(void) AttachViewToParent(
        /* [in] */ IView* child,
        /* [in] */ Int32 index,
        /* [in] */ IViewGroupLayoutParams* params);

    /**
     * Detaches a view from its parent. Detaching a view should be followed
     * either by a call to
     * {@link #attachViewToParent(View, int, android.view.ViewGroup.LayoutParams)}
     * or a call to {@link #removeDetachedView(View, boolean)}. Detachment should only be
     * temporary; reattachment or removal should happen within the same drawing cycle as
     * detachment. When a view is detached, its parent is null and cannot be retrieved by a
     * call to {@link #getChildAt(int)}.
     *
     * @param child the child to detach
     *
     * @see #detachViewFromParent(int)
     * @see #detachViewsFromParent(int, int)
     * @see #detachAllViewsFromParent()
     * @see #attachViewToParent(View, int, android.view.ViewGroup.LayoutParams)
     * @see #removeDetachedView(View, boolean)
     */
    virtual CARAPI_(void) DetachViewFromParent(
        /* [in] */ IView* child);

    /**
     * Detaches a view from its parent. Detaching a view should be followed
     * either by a call to
     * {@link #attachViewToParent(View, int, android.view.ViewGroup.LayoutParams)}
     * or a call to {@link #removeDetachedView(View, boolean)}. Detachment should only be
     * temporary; reattachment or removal should happen within the same drawing cycle as
     * detachment. When a view is detached, its parent is null and cannot be retrieved by a
     * call to {@link #getChildAt(int)}.
     *
     * @param index the index of the child to detach
     *
     * @see #detachViewFromParent(View)
     * @see #detachAllViewsFromParent()
     * @see #detachViewsFromParent(int, int)
     * @see #attachViewToParent(View, int, android.view.ViewGroup.LayoutParams)
     * @see #removeDetachedView(View, boolean)
     */
    virtual CARAPI_(void) DetachViewFromParent(
        /* [in] */ Int32 index);

    /**
     * Detaches a range of views from their parents. Detaching a view should be followed
     * either by a call to
     * {@link #attachViewToParent(View, int, android.view.ViewGroup.LayoutParams)}
     * or a call to {@link #removeDetachedView(View, boolean)}. Detachment should only be
     * temporary; reattachment or removal should happen within the same drawing cycle as
     * detachment. When a view is detached, its parent is null and cannot be retrieved by a
     * call to {@link #getChildAt(int)}.
     *
     * @param start the first index of the childrend range to detach
     * @param count the number of children to detach
     *
     * @see #detachViewFromParent(View)
     * @see #detachViewFromParent(int)
     * @see #detachAllViewsFromParent()
     * @see #attachViewToParent(View, int, android.view.ViewGroup.LayoutParams)
     * @see #removeDetachedView(View, boolean)
     */
    virtual CARAPI_(void) DetachViewsFromParent(
        /* [in] */ Int32 start,
        /* [in] */ Int32 count);

    /**
     * Detaches all views from the parent. Detaching a view should be followed
     * either by a call to
     * {@link #attachViewToParent(View, int, android.view.ViewGroup.LayoutParams)}
     * or a call to {@link #removeDetachedView(View, boolean)}. Detachment should only be
     * temporary; reattachment or removal should happen within the same drawing cycle as
     * detachment. When a view is detached, its parent is null and cannot be retrieved by a
     * call to {@link #getChildAt(int)}.
     *
     * @see #detachViewFromParent(View)
     * @see #detachViewFromParent(int)
     * @see #detachViewsFromParent(int, int)
     * @see #attachViewToParent(View, int, android.view.ViewGroup.LayoutParams)
     * @see #removeDetachedView(View, boolean)
     */
    virtual CARAPI_(void) DetachAllViewsFromParent();

    virtual CARAPI OnLayout(
        /* [in] */ Boolean changed,
        /* [in] */ Int32 l,
        /* [in] */ Int32 t,
        /* [in] */ Int32 r,
        /* [in] */ Int32 b) = 0;

    /**
     * Indicates whether the view group has the ability to animate its children
     * after the first layout.
     *
     * @return true if the children can be animated, false otherwise
     */
    virtual CARAPI_(Boolean) CanAnimate();

    /**
     * Indicates whether the ViewGroup is currently drawing its children using
     * their drawing cache.
     *
     * @return TRUE if children should be drawn with their cache, FALSE otherwise
     *
     * @see #setAlwaysDrawnWithCacheEnabled(Boolean)
     * @see #setChildrenDrawnWithCacheEnabled(Boolean)
     */
    virtual CARAPI_(Boolean) IsChildrenDrawnWithCacheEnabled();

    /**
     * Tells the ViewGroup to draw its children using their drawing cache. This property
     * is ignored when {@link #isAlwaysDrawnWithCacheEnabled()} is TRUE. A child's drawing cache
     * will be used only if it has been enabled.
     *
     * Subclasses should call this method to start and stop using the drawing cache when
     * they perform performance sensitive operations, like scrolling or animating.
     *
     * @param enabled TRUE if children should be drawn with their cache, FALSE otherwise
     *
     * @see #setAlwaysDrawnWithCacheEnabled(Boolean)
     * @see #isChildrenDrawnWithCacheEnabled()
     */
    virtual CARAPI_(void) SetChildrenDrawnWithCacheEnabled(
        /* [in] */ Boolean enabled);

    /**
     * Indicates whether the ViewGroup is drawing its children in the order defined by
     * {@link #getChildDrawingOrder(Int32, Int32)}.
     *
     * @return TRUE if children drawing order is defined by {@link #getChildDrawingOrder(Int32, Int32)},
     *         FALSE otherwise
     *
     * @see #setChildrenDrawingOrderEnabled(Boolean)
     * @see #getChildDrawingOrder(Int32, Int32)
     */
    virtual CARAPI_(Boolean) IsChildrenDrawingOrderEnabled();

    /**
     * Tells the ViewGroup whether to draw its children in the order defined by the method
     * {@link #getChildDrawingOrder(Int32, Int32)}.
     *
     * @param enabled TRUE if the order of the children when drawing is determined by
     *        {@link #getChildDrawingOrder(Int32, Int32)}, FALSE otherwise
     *
     * @see #isChildrenDrawingOrderEnabled()
     * @see #getChildDrawingOrder(Int32, Int32)
     */
    virtual CARAPI_(void) SetChildrenDrawingOrderEnabled(
        /* [in] */ Boolean enabled);

    /**
     * Returns a safe set of layout parameters based on the supplied layout params.
     * When a ViewGroup is passed a View whose layout params do not pass the test of
     * {@link #checkLayoutParams(android.view.ViewGroup.LayoutParams)}, this method
     * is invoked. This method should return a new set of layout params suitable for
     * this ViewGroup, possibly by copying the appropriate attributes from the
     * specified set of layout params.
     *
     * @param p The layout parameters to convert into a suitable set of layout parameters
     *          for this ViewGroup.
     *
     * @return an instance of {@link android.view.ViewGroup.LayoutParams} or one
     *         of its descendants
     */
    virtual CARAPI_(AutoPtr<IViewGroupLayoutParams>) GenerateLayoutParams(
        /* [in] */ IViewGroupLayoutParams* p);

    virtual CARAPI GenerateDefaultLayoutParams(
        /* [out] */ IViewGroupLayoutParams** params);

    virtual CARAPI_(Boolean) Debug(
        /* [in] */ Int32 depth);

    /**
     * Ask all of the children of this view to measure themselves, taking into
     * account both the MeasureSpec requirements for this view and its padding.
     * We skip children that are in the GONE state The heavy lifting is done in
     * getChildMeasureSpec.
     *
     * @param widthMeasureSpec The width requirements for this view
     * @param heightMeasureSpec The height requirements for this view
     */
    virtual CARAPI_(void) MeasureChildren(
        /* [in] */ Int32 widthMeasureSpec,
        /* [in] */ Int32 heightMeasureSpec);

    /**
     * Ask one of the children of this view to measure itself, taking into
     * account both the MeasureSpec requirements for this view and its padding.
     * The heavy lifting is done in getChildMeasureSpec.
     *
     * @param child The child to measure
     * @param parentWidthMeasureSpec The width requirements for this view
     * @param parentHeightMeasureSpec The height requirements for this view
     */
    virtual CARAPI_(void) MeasureChild(
        /* [in] */ IView* child,
        /* [in] */ Int32 parentWidthMeasureSpec,
        /* [in] */ Int32 parentHeightMeasureSpec);

    virtual CARAPI_(void) MeasureChildWithMargins(
        /* [in] */ IView* child,
        /* [in] */ Int32 parentWidthMeasureSpec,
        /* [in] */ Int32 widthUsed,
        /* [in] */ Int32 parentHeightMeasureSpec,
        /* [in] */ Int32 heightUsed);

    CARAPI DrawableStateChanged();

    virtual CARAPI JumpDrawablesToCurrentState();

    virtual CARAPI OnCreateDrawableState(
        /* [in] */ Int32 extraSpace,
        /* [out] */ ArrayOf<Int32>** drawableState);

    /**
     * @see #onRequestSendAccessibilityEvent(View, AccessibilityEvent)
     *
     * Note: Called from the default {@link View.AccessibilityDelegate}.
     */
    CARAPI_(Boolean) OnRequestSendAccessibilityEventInternal(
        /* [in] */ IView* child,
        /* [in] */ IAccessibilityEvent* event);

    /**
     * Called when a view's visibility has changed. Notify the parent to take any appropriate
     * action.
     *
     * @param child The view whose visibility has changed
     * @param oldVisibility The previous visibility value (GONE, INVISIBLE, or VISIBLE).
     * @param newVisibility The new visibility value (GONE, INVISIBLE, or VISIBLE).
     * @hide
     */
    CARAPI_(void) OnChildVisibilityChanged(
        /* [in] */ IView* child,
        /* [in] */ Int32 oldVisibility,
        /* [in] */ Int32 newVisibility);

    // Find the frontmost child view that lies under the given point, and calculate
    // the position within its own local coordinate system.
    CARAPI_(AutoPtr<IView>) FindFrontmostDroppableChildAt(
        /* [in] */ Float x,
        /* [in] */ Float y,
        /* [in] */ IPointF* outLocalPoint);

    CARAPI_(Boolean) NotifyChildOfDrag(
        /* [in] */ IView* child);

    CARAPI_(Boolean) UpdateLocalSystemUiVisibility(
        /* [in] */ Int32 localValue,
        /* [in] */ Int32 localChanges);

    /**
     * {@inheritDoc}
     */
    // @SuppressWarnings({"ConstantConditions"})
    // @Override
    CARAPI_(Boolean) DispatchHoverEvent(
        /* [in] */ IMotionEvent* event);

    CARAPI_(Boolean) HasHoveredChild();

    CARAPI_(Boolean) DispatchGenericPointerEvent(
        /* [in] */ IMotionEvent* event);

    CARAPI_(Boolean) DispatchGenericFocusedEvent(
        /* [in] */ IMotionEvent* event);

    /**
     * Returns true if a child view contains the specified point when transformed
     * into its coordinate space.
     * Child must not be null.
     * @hide
     */
    CARAPI_(Boolean) IsTransformedTouchPointInView(
        /* [in] */ Float x,
        /* [in] */ Float y,
        /* [in] */ IView* child,
        /* [in] */ IPointF* outLocalPoint);

    CARAPI_(void) DispatchScreenStateChanged(
        /* [in] */ Int32 screenState);

    CARAPI_(Boolean) DispatchPopulateAccessibilityEventInternal(
        /* [in] */ IAccessibilityEvent* event);

    CARAPI OnInitializeAccessibilityNodeInfoInternal(
        /* [in] */ IAccessibilityNodeInfo* info);

    CARAPI OnInitializeAccessibilityEventInternal(
        /* [in] */ IAccessibilityEvent* event);

    /**
     * @hide
     */
    CARAPI_(void) OnDebugDrawMargins(
        /* [in] */ ICanvas* canvas,
        /* [in] */ IPaint* paint);

    /**
     * @hide
     */
    CARAPI_(void) OnDebugDraw(
        /* [in] */ ICanvas* canvas);

    /**
     * This method is used to cause children of this ViewGroup to restore or recreate their
     * display lists. It is called by getDisplayList() when the parent ViewGroup does not need
     * to recreate its own display list, which would happen if it went through the normal
     * draw/dispatchDraw mechanisms.
     *
     * @hide
     */
    //@Override
    CARAPI_(void) DispatchGetDisplayList();

    /**
     * {@hide}
     */
    //@Override
    CARAPI_(AutoPtr<IView>) FindViewByPredicateTraversal(
        /* [in] */ IPredicate* predicate,
        /* [in] */ IView* childToSkip);

    /**
     * @hide
     */
    virtual CARAPI_(void) OnViewAdded(
        /* [in] */ IView* child);
    /**
     * @hide
     */
    virtual CARAPI_(void) OnViewRemoved(
        /* [in] */ IView* child);

    /**
     * Cleanup a view when its animation is done. This may mean removing it from
     * the list of disappearing views.
     *
     * @param view The view whose animation has finished
     * @param animation The animation, cannot be NULL
     */
    virtual CARAPI_(void) FinishAnimatingView(
        /* [in] */ IView* view,
        /* [in] */ IAnimation* animation);

    /**
     * Utility function called by View during invalidation to determine whether a view that
     * is invisible or gone should still be invalidated because it is being transitioned (and
     * therefore still needs to be drawn).
     */
    Boolean IsViewTransitioning(
        /* [in] */ IView* view);

    virtual CARAPI_(void) ResolveDrawables();

    virtual CARAPI_(void) ResetResolvedDrawables();

    /** @hide */
    virtual CARAPI_(void) OnSetLayoutParams(
        /* [in] */ IView* child,
        /* [in] */ IViewGroupLayoutParams* layoutParams);

    virtual CARAPI OnAttachedToWindow();

    virtual CARAPI OnDetachedFromWindow();

    /**
     * Quick invalidation method that simply transforms the dirty rect into the parent's
     * coordinate system, pruning the invalidation if the parent has already been invalidated.
     */
    CARAPI_(AutoPtr<IViewParent>) DamageChildInParent(
        /* [in] */ Int32 left,
        /* [in] */ Int32 top,
        /* [in] */ IRect* dirty);

private:
    CARAPI_(Boolean) DebugDraw();

    CARAPI_(void) InitViewGroup();

    CARAPI_(void) InitFromAttributes(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs,
        /* [in] */ Int32 defStyleAttr,
        /* [in] */ Int32 defStyleRes);

    CARAPI_(void) NotifyAnimationListener();

    CARAPI AddViewInner(
        /* [in] */ IView* child,
        /* [in] */ Int32 index,
        /* [in] */ IViewGroupLayoutParams* params,
        /* [in] */ Boolean preventRequestLayout);

    CARAPI AddInArray(
        /* [in] */ IView* child,
        /* [in] */ Int32 index);

    CARAPI RemoveFromArray(
        /* [in] */ Int32 index);

    CARAPI RemoveFromArray(
        /* [in] */ Int32 start,
        /* [in] */ Int32 count);

    CARAPI_(void) BindLayoutAnimation(
        /* [in] */ IView* child);

    CARAPI_(void) RemoveViewInternal(
        /* [in] */ IView* view);

    CARAPI_(void) RemoveViewInternal(
        /* [in] */ Int32 index,
        /* [in] */ IView* view);

    CARAPI_(void) RemoveViewsInternal(
        /* [in] */ Int32 start,
        /* [in] */ Int32 count);

    CARAPI_(void) SetBooleanFlag(
        /* [in] */ Int32 flag,
        /* [in] */ Boolean value);

    /**
     * Add a view which is removed from mChildren but still needs animation
     *
     * @param v View to add
     */
    CARAPI_(void) AddDisappearingView(
        /* [in] */ IView* view);

    CARAPI_(void) ExitHoverTargets();

    CARAPI_(void) CancelHoverTarget(
        /* [in] */ IView* view);

    /**
     * Dispatches a generic pointer event to a child, taking into account
     * transformations that apply to the child.
     *
     * @param event The event to send.
     * @param child The view to send the event to.
     * @return {@code true} if the child handled the event.
     */
    CARAPI_(Boolean) DispatchTransformedGenericPointerEvent(
        /* [in] */ IMotionEvent* event,
        /* [in] */ IView* child);

    static CARAPI_(AutoPtr<IMotionEvent>) ObtainMotionEventNoHistoryOrSelf(
        /* [in] */ IMotionEvent* event);

    /**
     * Clears all touch targets.
     */
    CARAPI_(void) ClearTouchTargets();

    /**
     * Cancels and clears all touch targets.
     */
    CARAPI_(void) CancelAndClearTouchTargets(
        /* [in] */IMotionEvent* event);

    /**
     * Gets the touch target for specified child view.
     * Returns null if not found.
     */
    CARAPI_(AutoPtr<TouchTarget>) GetTouchTarget(
        /* [in] */ IView* child);

    /**
     * Adds a touch target for specified child to the beginning of the list.
     * Assumes the target child is not already present.
     */
    CARAPI_(AutoPtr<TouchTarget>) AddTouchTarget(
        /* [in] */ IView* child,
        /* [in] */ Int32 pointerIdBits);

    /**
     * Removes the pointer ids from consideration.
     */
    CARAPI_(void) RemovePointersFromTouchTargets(
        /* [in] */ Int32 pointerIdBits);

    CARAPI_(void) CancelTouchTarget(
        /* [in] */ IView* view);

    /**
     * Returns true if a child view can receive pointer events.
     * @hide
     */
    static CARAPI_(Boolean) CanViewReceivePointerEvents(
        /* [in] */ IView* child);

    /**
     * Transforms a motion event into the coordinate space of a particular child view,
     * filters out irrelevant pointer ids, and overrides its action if necessary.
     * If child is null, assumes the MotionEvent will be sent to this ViewGroup instead.
     */
    CARAPI_(Boolean) DispatchTransformedTouchEvent(
        /* [in] */ IMotionEvent* event,
        /* [in] */ Boolean cancel,
        /* [in] */ IView* child,
        /* [in] */ Int32 desiredPointerIdBits);

    /**
     * Resets all touch state in preparation for a new cycle.
     */
    CARAPI_(void) ResetTouchState();

    /**
     * Resets the cancel next up flag.
     * Returns true if the flag was previously set.
     */
    static CARAPI_(Boolean) ResetCancelNextUpFlag(
        /* [in] */ IView* view);

    static CARAPI_(void) DrawRect(
        /* [in] */ ICanvas* canvas,
        /* [in] */ IPaint* paint,
        /* [in] */ Int32 x1,
        /* [in] */ Int32 y1,
        /* [in] */ Int32 x2,
        /* [in] */ Int32 y2);

    CARAPI TranslateBoundsAndIntersectionsInWindowCoordinates(
        /* [in] */ IView* child,
        /* [in] */ IRectF* bounds,
        /* [in] */ IList* intersections,
        /* [out] */ Boolean* res);

    static CARAPI_(AutoPtr<IPaint>) GetDebugPaint();

    static CARAPI_(AutoPtr<ArrayOf<Float> >) GetDebugLines(
        /* [in] */ Int32 x1,
        /* [in] */ Int32 y1,
        /* [in] */ Int32 x2,
        /* [in] */ Int32 y2);

    CARAPI_(AutoPtr<IPointF>) GetLocalPoint();

    CARAPI_(Boolean) IsLayoutModeOptical();

    CARAPI ComputeOpticalInsets(
        /* [out] */ IInsets** insets);

    static CARAPI_(void) FillRect(
        /* [in] */ ICanvas* canvas,
        /* [in] */ IPaint* paint,
        /* [in] */ Int32 x1,
        /* [in] */ Int32 y1,
        /* [in] */ Int32 x2,
        /* [in] */ Int32 y2);

    static CARAPI_(Int32) Sign(
        /* [in] */ Int32 x);

    static CARAPI_(void) DrawCorner(
        /* [in] */ ICanvas* canvas,
        /* [in] */ IPaint* paint,
        /* [in] */ Int32 x1,
        /* [in] */ Int32 y1,
        /* [in] */ Int32 dx,
        /* [in] */ Int32 dy,
        /* [in] */ Int32 lw);

    CARAPI_(Int32) DipsToPixels(
        /* [in] */ Int32 dips);

    static CARAPI_(void) DrawRectCorners(
        /* [in] */ ICanvas* canvas,
        /* [in] */ Int32 x1,
        /* [in] */ Int32 y1,
        /* [in] */ Int32 x2,
        /* [in] */ Int32 y2,
        /* [in] */ IPaint* paint,
        /* [in] */ Int32 lineLength,
        /* [in] */ Int32 lineWidth);

    static CARAPI_(void) FillDifference(
        /* [in] */ ICanvas* canvas,
        /* [in] */ Int32 x2,
        /* [in] */ Int32 y2,
        /* [in] */ Int32 x3,
        /* [in] */ Int32 y3,
        /* [in] */ Int32 dx1,
        /* [in] */ Int32 dy1,
        /* [in] */ Int32 dx2,
        /* [in] */ Int32 dy2,
        /* [in] */ IPaint* paint);

    CARAPI_(Boolean) HasChildWithZ();

    CARAPI_(AutoPtr<IList>) BuildOrderedChildList();

    CARAPI_(void) RecreateChildDisplayList(
        /* [in] */ IView* child);

    CARAPI_(void) DispatchCancelPendingInputEvents();

    CARAPI_(AutoPtr<ITransformation>) GetChildTransformation();

    CARAPI_(void) ClearCachedLayoutMode();

    CARAPI_(Boolean) HasBooleanFlag(
        /* [in] */ Int32 flag);

    CARAPI_(void) SetLayoutMode(
        /* [in] */ Int32 layoutMode,
        /* [in] */ Boolean explicitly);

    CARAPI_(void) InvalidateInheritedLayoutMode(
        /* [in] */ Int32 layoutModeOfRoot);

public:
    /**
     * This view will get focus before any of its descendants.
     */
    static const Int32 FOCUS_BEFORE_DESCENDANTS = 0x20000;

    /**
     * This view will get focus only if none of its descendants want it.
     */
    static const Int32 FOCUS_AFTER_DESCENDANTS = 0x40000;

    /**
     * This view will block any of its descendants from getting focus, even
     * if they are focusable.
     */
    static const Int32 FOCUS_BLOCK_DESCENDANTS = 0x60000;

    /**
     * Used to indicate that no drawing cache should be kept in memory.
     */
    static const Int32 PERSISTENT_NO_CACHE = 0x0;

    /**
     * Used to indicate that the animation drawing cache should be kept in memory.
     */
    static const Int32 PERSISTENT_ANIMATION_CACHE = 0x1;

    /**
     * Used to indicate that the scrolling drawing cache should be kept in memory.
     */
    static const Int32 PERSISTENT_SCROLLING_CACHE = 0x2;

    /**
     * Used to indicate that all drawing caches should be kept in memory.
     */
    static const Int32 PERSISTENT_ALL_CACHES = 0x3;

    // Layout Modes

    /**
     * This constant is a {@link #setLayoutMode(int) layoutMode}.
     * Clip bounds are the raw values of {@link #getLeft() left}, {@link #getTop() top},
     * {@link #getRight() right} and {@link #getBottom() bottom}.
     *
     * @hide
     */
    static const Int32 LAYOUT_MODE_CLIP_BOUNDS = 0;

    /**
     * This constant is a {@link #setLayoutMode(int) layoutMode}.
     * Optical bounds describe where a widget appears to be. They sit inside the clip
     * bounds which need to cover a larger area to allow other effects,
     * such as shadows and glows, to be drawn.
     *
     * @hide
     */
    static const Int32 LAYOUT_MODE_OPTICAL_BOUNDS = 1;

    static Int32 LAYOUT_MODE_DEFAULT;// = LAYOUT_MODE_CLIP_BOUNDS;

    static Boolean DEBUG_DRAW;

protected:
    // When set, ViewGroup invalidates only the child's rectangle
    // Set by default
    static const Int32 FLAG_CLIP_CHILDREN = 0x1;

    // When set, dispatchDraw() will invoke invalidate(); this is set by drawChild() when
    // a child needs to be invalidated and FLAG_OPTIMIZE_INVALIDATE is set
    static const Int32 FLAG_INVALIDATE_REQUIRED  = 0x4;

    // When set, there is either no layout animation on the ViewGroup or the layout
    // animation is over
    // Set by default
    static const Int32 FLAG_ANIMATION_DONE = 0x10;

    // When set, this ViewGroup converts calls to invalidate(Rect) to invalidate() during a
    // layout animation; this avoid clobbering the hierarchy
    // Automatically set when the layout animation starts, depending on the animation's
    // characteristics
    static const Int32 FLAG_OPTIMIZE_INVALIDATE = 0x80;

    // When set, the next call to drawChild() will clear mChildTransformation's matrix
    static const Int32 FLAG_CLEAR_TRANSFORMATION = 0x100;

    /**
     * When set, this ViewGroup tries to always draw its children using their drawing cache.
     */
    static const Int32 FLAG_ALWAYS_DRAWN_WITH_CACHE = 0x4000;

    /**
     * When set, and if FLAG_ALWAYS_DRAWN_WITH_CACHE is not set, this ViewGroup will try to
     * draw its children with their drawing cache.
     */
    static const Int32 FLAG_CHILDREN_DRAWN_WITH_CACHE = 0x8000;


private:
    static const Boolean DBG;

    // When set, ViewGroup excludes the padding area from the invalidate rectangle
    // Set by default
    static const Int32 FLAG_CLIP_TO_PADDING = 0x2;

    // When set, dispatchDraw() will run the layout animation and unset the flag
    static const Int32 FLAG_RUN_ANIMATION = 0x8;

    // If set, this ViewGroup has padding; if unset there is no padding and we don't need
    // to clip it, even if FLAG_CLIP_TO_PADDING is set
    static const Int32 FLAG_PADDING_NOT_NULL = 0x20;

    // When set, this ViewGroup caches its children in a Bitmap before starting a layout animation
    // Set by default
    static const Int32 FLAG_ANIMATION_CACHE = 0x40;

    // When set, this ViewGroup invokes mAnimationListener.onAnimationEnd() and removes
    // the children's Bitmap caches if necessary
    // This flag is set when the layout animation is over (after FLAG_ANIMATION_DONE is set)

    static const Int32 FLAG_NOTIFY_ANIMATION_LISTENER = 0x200;

    /**
     * When set, this ViewGroup's drawable states also include those
     * of its children.
     */
    static const Int32 FLAG_ADD_STATES_FROM_CHILDREN = 0x2000;

    /**
     * When set, this group will go through its list of children to notify them of
     * any drawable state change.
     */
    static const Int32 FLAG_NOTIFY_CHILDREN_ON_DRAWABLE_STATE_CHANGE = 0x10000;

    static const Int32 FLAG_MASK_FOCUSABILITY = 0x60000;

    /**
     * Used to map between enum in attrubutes and flag values.
     */
    static const Int32 DESCENDANT_FOCUSABILITY_FLAGS[] ;/*=
            {FOCUS_BEFORE_DESCENDANTS, FOCUS_AFTER_DESCENDANTS,
                    FOCUS_BLOCK_DESCENDANTS};*/

    static const Int32 ARRAY_INITIAL_CAPACITY = 12;

    static const Int32 ARRAY_CAPACITY_INCREMENT = 12;

    /**
     * When set, this ViewGroup will split MotionEvents to multiple child Views when appropriate.
     */
    static const Int32 FLAG_SPLIT_MOTION_EVENTS = 0x200000;

    /**
     * When set, this ViewGroup will not dispatch onAttachedToWindow calls
     * to children when adding new views. This is used to prevent multiple
     * onAttached calls when a ViewGroup adds children in its own onAttached method.
     */
    static const Int32 FLAG_PREVENT_DISPATCH_ATTACHED_TO_WINDOW = 0x400000;

    static AutoPtr<IPaint> sDebugPaint;

    static AutoPtr<ArrayOf<Float> > sDebugLines;

    /**
     * When true, indicates that a layoutMode has been explicitly set, either with
     * an explicit call to {@link #setLayoutMode(int)} in code or from an XML resource.
     * This distinguishes the situation in which a layout mode was inherited from
     * one of the ViewGroup's ancestors and cached locally.
     */
    static const Int32 FLAG_LAYOUT_MODE_WAS_EXPLICITLY_SET = 0x800000;

    static const Int32 FLAG_IS_TRANSITION_GROUP = 0x1000000;

    static const Int32 FLAG_IS_TRANSITION_GROUP_SET = 0x2000000;

    /**
     * When set, focus will not be permitted to enter this group if a touchscreen is present.
     */
    static const Int32 FLAG_TOUCHSCREEN_BLOCKS_FOCUS = 0x4000000;

    static const Int32 LAYOUT_MODE_UNDEFINED = -1;

protected:
    /**
     * When set, the drawing method will call {@link #getChildDrawingOrder(Int32, Int32)}
     * to get the index of the child to draw for that iteration.
     *
     * @hide
     */
    static const Int32 FLAG_USE_CHILD_DRAWING_ORDER = 0x400;

    /**
     * When set, this ViewGroup supports static transformations on children; this causes
     * {@link #getChildStaticTransformation(View, android.view.animation.Transformation)} to be
     * invoked when a child is drawn.
     *
     * Any subclass overriding
     * {@link #getChildStaticTransformation(View, android.view.animation.Transformation)} should
     * set this flags in {@link #mGroupFlags}.
     *
     * {@hide}
     */
    static const Int32 FLAG_SUPPORT_STATIC_TRANSFORMATIONS = 0x800;

    /**
     * When set, this ViewGroup should not intercept touch events.
     * {@hide}
     */
    static const Int32 FLAG_DISALLOW_INTERCEPT = 0x80000;

    /**
     * We clip to padding when FLAG_CLIP_TO_PADDING and FLAG_PADDING_NOT_NULL
     * are set at the same time.
     */
    static const Int32 CLIP_TO_PADDING_MASK = FLAG_CLIP_TO_PADDING | FLAG_PADDING_NOT_NULL;

    // Index of the child's left position in the mLocation array
    static const Int32 CHILD_LEFT_INDEX = 0;
    // Index of the child's top position in the mLocation array
    static const Int32 CHILD_TOP_INDEX = 1;

    /**
     * Views which have been hidden or removed which need to be animated on
     * their way out.
     * This field should be made private, so it is hidden from the SDK.
     * {@hide}
     */
    AutoPtr<IList> mDisappearingChildren;

    /**
     * Listener used to propagate events indicating when children are added
     * and/or removed from a view group.
     * This field should be made private, so it is hidden from the SDK.
     * {@hide}
     */
    AutoPtr<IViewGroupOnHierarchyChangeListener> mOnHierarchyChangeListener;

    /**
     * Internal flags.
     *
     * This field should be made private, so it is hidden from the SDK.
     * {@hide}
     */
    // @ViewDebug.ExportedProperty(flagMapping = {
    //         @ViewDebug.FlagToString(mask = FLAG_CLIP_CHILDREN, equals = FLAG_CLIP_CHILDREN,
    //                 name = "CLIP_CHILDREN"),
    //         @ViewDebug.FlagToString(mask = FLAG_CLIP_TO_PADDING, equals = FLAG_CLIP_TO_PADDING,
    //                 name = "CLIP_TO_PADDING"),
    //         @ViewDebug.FlagToString(mask = FLAG_PADDING_NOT_NULL, equals = FLAG_PADDING_NOT_NULL,
    //                 name = "PADDING_NOT_NULL")
    // })
    Int32 mGroupFlags;

    /**
     * Indicates which types of drawing caches are to be kept in memory.
     * This field should be made private, so it is hidden from the SDK.
     * {@hide}
     */
    Int32 mPersistentDrawingCache;

private:
    // The view contained within this ViewGroup that has or contains focus.
    AutoPtr<IView> mFocused;

    /**
     * A Transformation used when drawing children, to
     * apply on the child being drawn.
     */
    AutoPtr<ITransformation> mChildTransformation;

    /**
     * Used to track the current invalidation region.
     */
    AutoPtr<IRectF> mInvalidateRegion;

    /**
     * A Transformation used to calculate a correct
     * invalidation area when the application is autoscaled.
     */
    AutoPtr<ITransformation> mInvalidationTransformation;

    // View currently under an ongoing drag
    View* mCurrentDragView;

    // Metadata about the ongoing drag
    AutoPtr<IDragEvent> mCurrentDrag;

    List<AutoPtr<IView> > mDragNotifiedChildren;

    // Does this group have a child that can accept the current drag payload?
    Boolean mChildAcceptsDrag;

    // Used during drag dispatch
    AutoPtr<IPointF> mLocalPoint;

    // Layout animation
    AutoPtr<ILayoutAnimationController> mLayoutAnimationController;
    AutoPtr<IAnimationAnimationListener> mAnimationListener;

    // First touch target in the linked list of touch targets.
    AutoPtr<TouchTarget> mFirstTouchTarget;

    // For debugging only.  You can see these in hierarchyviewer.
    // @SuppressWarnings({"FieldCanBeLocal", "UnusedDeclaration"})
    // @ViewDebug.ExportedProperty(category = "events")
    Int64 mLastTouchDownTime;

    //@ViewDebug.ExportedProperty(category = "events")
    Int32 mLastTouchDownIndex;

    // @SuppressWarnings({"FieldCanBeLocal", "UnusedDeclaration"})
    // @ViewDebug.ExportedProperty(category = "events")
    Float mLastTouchDownX;

    // @SuppressWarnings({"FieldCanBeLocal", "UnusedDeclaration"})
    // @ViewDebug.ExportedProperty(category = "events")
    Float mLastTouchDownY;

    // First hover target in the linked list of hover targets.
    // The hover targets are children which have received ACTION_HOVER_ENTER.
    // They might not have actually handled the hover event, but we will
    // continue sending hover events to them as long as the pointer remains over
    // their bounds and the view group does not intercept hover.
    AutoPtr<HoverTarget> mFirstHoverTarget;

    // True if the view group itself received a hover event.
    // It might not have actually handled the hover event.
    Boolean mHoveredSelf;

    /*
     * The layout mode: either {@link #CLIP_BOUNDS} or {@link #OPTICAL_BOUNDS}
     */
    Int32 mLayoutMode;

    // Child views of this ViewGroup
    AutoPtr<ArrayOf<IView*> > mChildren;
    // Number of valid children in the mChildren array, the rest should be null or not
    // considered as children
    Int32 mChildrenCount;

    // Whether layout calls are currently being suppressed, controlled by calls to
    // suppressLayout()
    Boolean mSuppressLayout;// = false;

    // Whether any layout calls have actually been suppressed while mSuppressLayout
    // has been true. This tracks whether we need to issue a requestLayout() when
    // layout is later re-enabled.
    Boolean mLayoutCalledWhileSuppressed;// = false;

    // Used to draw cached views
    AutoPtr<IPaint> mCachePaint;

    // Used to animate add/remove changes in layout
    AutoPtr<ILayoutTransition> mTransition;

    // The set of views that are currently being transitioned. This list is used to track views
    // being removed that should not actually be removed from the parent yet because they are
    // being animated.
    List<AutoPtr<IView> > mTransitioningViews;

    // List of children changing visibility. This is used to potentially keep rendering
    // views during a transition when they otherwise would have become gone/invisible
    List<AutoPtr<IView> > mVisibilityChangingChildren;

    // Temporary holder of presorted children, only used for
    // input/software draw dispatch for correctly Z ordering.
    AutoPtr<IList> mPreSortedChildren;

    // Indicates how many of this container's child subtrees contain transient state
    //@ViewDebug.ExportedProperty(category = "layout")
    Int32 mChildCountWithTransientState;

     /**
     * Currently registered axes for nested scrolling. Flag set consisting of
     * {@link #SCROLL_AXIS_HORIZONTAL} {@link #SCROLL_AXIS_VERTICAL} or {@link #SCROLL_AXIS_NONE}
     * for null.
     */
    Int32 mNestedScrollAxes;

    AutoPtr<ITransitionListener> mLayoutTransitionListener;
};

}   //namespace View
}   //namespace Droid
}   //namespace Elastos

DEFINE_CONVERSION_FOR(Elastos::Droid::View::ViewGroup::ChildListForAccessibility, IInterface)
DEFINE_CONVERSION_FOR(Elastos::Droid::View::ViewGroup::ViewLocationHolder, IInterface)

#endif // __ELASTOS_DROID_VIEW_ViewGroup_H__
