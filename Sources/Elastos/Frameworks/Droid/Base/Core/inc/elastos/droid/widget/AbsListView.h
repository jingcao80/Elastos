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

#ifndef __ELASTOS_DROID_WIDGET_ABSLISTVIEW_H__
#define __ELASTOS_DROID_WIDGET_ABSLISTVIEW_H__

#include "elastos/droid/ext/frameworkext.h"
#include "Elastos.Droid.Widget.h"
#include "Elastos.Droid.Text.h"
#include <Elastos.Droid.Graphics.h>
#include "elastos/droid/widget/AdapterView.h"
#include "elastos/droid/view/VelocityTracker.h"
#include "elastos/droid/view/View.h"
#include "elastos/droid/view/ViewGroup.h"
#include <Elastos.CoreLibrary.h>

using Elastos::Droid::Graphics::IRect;

namespace Elastos {
namespace Droid {
namespace Widget {

class CAbsListViewSavedState;

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Graphics::IRect;

using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Os::IStrictModeSpan;
using Elastos::Droid::Os::IStrictMode;
using Elastos::Droid::Text::IEditable;
using Elastos::Droid::Text::ITextWatcher;
using Elastos::Droid::Text::IEditable;
using Elastos::Droid::Utility::IAttributeSet;
using Elastos::Droid::Utility::ISparseArray;
using Elastos::Droid::Utility::IInt64SparseArray;
using Elastos::Droid::Utility::ISparseBooleanArray;
using Elastos::Droid::View::Animation::IInterpolator;
using Elastos::Droid::View::IActionModeCallback;
using Elastos::Droid::View::InputMethod::ICompletionInfo;
using Elastos::Droid::View::InputMethod::ICorrectionInfo;
using Elastos::Droid::View::InputMethod::IEditorInfo;
using Elastos::Droid::View::InputMethod::IExtractedText;
using Elastos::Droid::View::InputMethod::IExtractedTextRequest;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::IOnGlobalLayoutListener;
using Elastos::Droid::View::IOnTouchModeChangeListener;
using Elastos::Droid::View::View;
using Elastos::Droid::View::ViewGroup;
using Elastos::Droid::View::IViewGroupLayoutParams;
using Elastos::Droid::View::Accessibility::IAccessibilityNodeInfo;
using Elastos::Droid::View::IViewGroupLayoutParams;
using Elastos::Droid::View::IActionMode;
using Elastos::Droid::View::IMenu;
using Elastos::Droid::View::IMenuItem;
using Elastos::Droid::View::IMotionEvent;
using Elastos::Droid::View::IKeyEvent;
using Elastos::Droid::View::VelocityTracker;
using Elastos::Droid::Widget::IFilterListener;
using Elastos::Droid::Widget::IAbsListViewLayoutParams;
using Elastos::Droid::Widget::IAbsListViewSavedState;
using Elastos::Droid::Widget::IAdapter;
using Elastos::Droid::Widget::IListAdapter;
using Elastos::Droid::Widget::IMultiChoiceModeListener;
using Elastos::Droid::Widget::IPopupWindow;
using Elastos::Droid::Widget::CAbsListViewSavedState;
using Elastos::Core::IRunnable;
using Elastos::Core::IThread;
using Elastos::Utility::IArrayList;

class FastScroller;

class ECO_PUBLIC AbsListView
    : public AdapterView
    , public IAbsListView
    , public ITextWatcher
    , public IOnGlobalLayoutListener
    , public IFilterListener
    , public IOnTouchModeChangeListener
    , public IRemoteAdapterConnectionCallback
{
    friend class FastScroller;
public:
    class AbsListViewLayoutParams
        : public ViewGroup::LayoutParams
        , public IAbsListViewLayoutParams
    {
        friend class AbsListView;
    public:
        CAR_INTERFACE_DECL()

        AbsListViewLayoutParams();

        ~AbsListViewLayoutParams();

        /**
         * {@inheritDoc}
         */
        CARAPI constructor(
            /* [in] */ IContext* c,
            /* [in] */ IAttributeSet* attrs);

        /**
         * {@inheritDoc}
         */
        CARAPI constructor(
            /* [in] */ Int32 width,
            /* [in] */ Int32 height);

        /**
         * Creates a new set of layout parameters with the specified width, height
         * and weight.
         *
         * @param width the width, either {@link #MATCH_PARENT},
         *        {@link #WRAP_CONTENT} or a fixed size in pixels
         * @param height the height, either {@link #MATCH_PARENT},
         *        {@link #WRAP_CONTENT} or a fixed size in pixels
         * @param gravity the gravity
         *
         * @see android.view.Gravity
         */
        CARAPI constructor(
            /* [in] */ Int32 width,
            /* [in] */ Int32 height,
            /* [in] */ Int32 viewType);

        /**
         * {@inheritDoc}
         */
        CARAPI constructor(
            /* [in] */ IViewGroupLayoutParams* source);

        CARAPI SetItemId(
            /* [in] */ Int64 itemId);

        CARAPI SetScrappedFromPosition(
            /* [in] */ Int32 scrappedFromPosition);

        CARAPI SetForceAdd(
            /* [in] */ Boolean force);

        CARAPI SetRecycledHeaderFooter(
            /* [in] */ Boolean footer);

        CARAPI SetViewType(
            /* [in] */ Int32 type);

        CARAPI GetItemId(
            /* [out] */ Int64* itemId);

        CARAPI GetScrappedFromPosition(
            /* [out] */ Int32* scrappedFromPosition);

        CARAPI GetForceAdd(
            /* [in] */ Boolean* force);

        CARAPI GetRecycledHeaderFooter(
            /* [in] */ Boolean* footer);

        CARAPI GetViewType(
            /* [in] */ Int32* type);
    public:
        /**
         * View type for this view, as returned by
         * {@link android.widget.Adapter#getItemViewType(int) }
         */
        Int32 mViewType;

        /**
         * When this boolean is set, the view has been added to the AbsListView
         * at least once. It is used to know whether headers/footers have already
         * been added to the list view and whether they should be treated as
         * recycled views or not.
         */
        Boolean mRecycledHeaderFooter;

        Boolean mForceAdd;

        /**
         * The position the view was removed from when pulled out of the
         * scrap heap.
         * @hide
         */
        Int32 mScrappedFromPosition;

        /**
         * The ID the view represents
         */
        Int64 mItemId;
    };

    class AbsListViewSavedState
        : public View::BaseSavedState
        , public IAbsListViewSavedState
    {
        friend class AbsListView;
    public:
        CAR_INTERFACE_DECL()

        AbsListViewSavedState();

        ~AbsListViewSavedState();

        // @Override
        CARAPI WriteToParcel(
            /* [in] */ IParcel* dest);

        // @Override
        CARAPI ReadFromParcel(
            /* [in] */ IParcel* source);

        // @Override
        CARAPI ToString(
            /* [out] */ String* str);

    protected:
        Int64 mSelectedId;
        Int64 mFirstId;
        Int32 mViewTop;
        Int32 mPosition;
        Int32 mHeight;
        String mFilter;
        Boolean mInActionMode;
        Int32 mCheckedItemCount;
        AutoPtr<ISparseBooleanArray> mCheckState;
        AutoPtr<IInt64SparseArray> mCheckIdState;
    };

protected:
    class ListItemAccessibilityDelegate
        : public View::AccessibilityDelegate
    {
    public:
        ListItemAccessibilityDelegate(
            /* [in] */ AbsListView* host);

        //@Override
        CARAPI CreateAccessibilityNodeInfo(
            /* [in] */ IView* host,
            /* [out] */ IAccessibilityNodeInfo** info);

        //@Override
        CARAPI OnInitializeAccessibilityNodeInfo(
            /* [in] */ IView* host,
            /* [in] */ IAccessibilityNodeInfo* info);

        //@Override
        CARAPI PerformAccessibilityAction(
            /* [in] */ IView* host,
            /* [in] */ Int32 action,
            /* [in] */ IBundle* arguments,
            /* [out] */ Boolean* result);
    private:
        AbsListView* mHost;
    };

    class AdapterDataSetObserver
        : public AdapterView::AdapterDataSetObserver
    {
    public:

        AdapterDataSetObserver(
            /* [in] */ AbsListView* host);

        //@Override
        CARAPI OnChanged();

        //@Override
        CARAPI OnInvalidated();

    private:
        AbsListView* mHost;
    };

    class MultiChoiceModeWrapper
        : public Object
        , public IMultiChoiceModeListener
        , public IActionModeCallback
    {
    public:
        CAR_INTERFACE_DECL()

        MultiChoiceModeWrapper(
            /* [in] */ AbsListView* host);

        CARAPI SetWrapped(
            /* [in] */ IMultiChoiceModeListener* wrapped);

        CARAPI HasWrappedCallback(
            /* [out] */ Boolean* result);

        // @Override
        CARAPI OnCreateActionMode(
            /* [in] */ IActionMode* mode,
            /* [in] */ IMenu* menu,
            /* [out] */ Boolean* result);

        // @Override
        CARAPI OnPrepareActionMode(
            /* [in] */ IActionMode* mode,
            /* [in] */ IMenu* menu,
            /* [out] */ Boolean* result);

        // @Override
        CARAPI OnActionItemClicked(
            /* [in] */ IActionMode* mode,
            /* [in] */ IMenuItem* item,
            /* [out] */ Boolean* result);

        // @Override
        CARAPI OnDestroyActionMode(
            /* [in] */ IActionMode* mode);

        // @Override
        CARAPI OnItemCheckedStateChanged(
            /* [in] */ IActionMode* mode,
            /* [in] */ Int32 position,
            /* [in] */ Int64 id,
            /* [in] */ Boolean checked);

    private:
        AbsListView* mHost;
        AutoPtr<IMultiChoiceModeListener> mWrapped;
    };

    /**
     * The RecycleBin facilitates reuse of views across layouts. The RecycleBin has two levels of
     * storage: ActiveViews and ScrapViews. ActiveViews are those views which were onscreen at the
     * start of a layout. By construction, they are displaying current information. At the end of
     * layout, all views in ActiveViews are demoted to ScrapViews. ScrapViews are old views that
     * could potentially be used by the adapter to avoid allocating views unnecessarily.
     *
     * @see android.widget.AbsListView#setRecyclerListener(android.widget.AbsListView.RecyclerListener)
     * @see android.widget.AbsListView.RecyclerListener
     */
    class RecycleBin
        : public Object
    {
        friend class AbsListView;
    public:
        RecycleBin(
            /* [in] */ AbsListView* host);

        ~RecycleBin();

        CARAPI SetViewTypeCount(
            /* [in] */ Int32 viewTypeCount);

        CARAPI_(void) MarkChildrenDirty();

        CARAPI_(Boolean) ShouldRecycleViewType(
            /* [in] */ Int32 viewType);

        /**
         * Clears the scrap heap.
         */
        CARAPI_(void) Clear();

        /**
         * Fill ActiveViews with all of the children of the AbsListView.
         *
         * @param childCount The minimum number of views mActiveViews should hold
         * @param firstActivePosition The position of the first view that will be stored in
         *        mActiveViews
         */
        CARAPI_(void) FillActiveViews(
            /* [in] */ Int32 childCount,
            /* [in] */ Int32 firstActivePosition);

        /**
         * Get the view corresponding to the specified position. The view will be removed from
         * mActiveViews if it is found.
         *
         * @param position The position to look up in mActiveViews
         * @return The view if it is found, NULL otherwise
         */
        CARAPI_(AutoPtr<IView>) GetActiveView(
            /* [in] */ Int32 position);

        CARAPI_(AutoPtr<IView>) GetTransientStateView(
            /* [in] */ Int32 position);

        /**
         * Dumps and fully detaches any currently saved views with transient
         * state.
         */
        CARAPI_(void) ClearTransientStateViews();

        /**
         * @return A view from the ScrapViews collection. These are unordered.
         */
        CARAPI_(AutoPtr<IView>) GetScrapView(
            /* [in] */ Int32 position);

        /**
         * Puts a view into the list of scrap views.
         * <p>
         * If the list data hasn't changed or the adapter has stable IDs, views
         * with transient state will be preserved for later retrieval.
         *
         * @param scrap The view to add
         * @param position The view's position within its parent
         */
        CARAPI_(void) AddScrapView(
            /* [in ]*/ IView* scrap,
            /* [in] */ Int32 position);

        /**
         * Finish the removal of any views that skipped the scrap heap.
         */
        CARAPI_(void) RemoveSkippedScrap();

        /**
         * Move all views remaining in mActiveViews to mScrapViews.
         */
        CARAPI_(void) ScrapActiveViews();

        /**
         * Puts all views in the scrap heap into the supplied list.
         */
        CARAPI_(void) ReclaimScrapViews(
            /* [in] */ IList* views);

        /**
         * Updates the cache color hint of all known views.
         *
         * @param color The new cache color hint.
         */
        CARAPI_(void) SetCacheColorHint(
            /* [in] */ Int32 color);

    private:
        /**
         * Makes sure that the size of mScrapViews does not exceed the size of
         * mActiveViews, which can happen if an adapter does not recycle its
         * views. Removes cached transient state views that no longer have
         * transient state.
         */
        CARAPI_(void) PruneScrapViews();

        CARAPI_(AutoPtr<IView>) RetrieveFromScrap(
            /* [in] */ IArrayList* scrapViews,
            /* [in] */ Int32 position);

        CARAPI_(void) ClearScrap(
            /* [in] */ IArrayList* scrap);

        CARAPI_(void) ClearAccessibilityFromScrap(
            /* [in] */ IView* view);

        CARAPI_(void) RemoveDetachedView(
            /* [in] */ IView* child,
            /* [in] */ Boolean animate);

    private:
        AutoPtr<IRecyclerListener> mRecyclerListener;

        /**
         * The position of the first view stored in mActiveViews.
         */
        Int32 mFirstActivePosition;

        /**
         * Views that were on screen at the start of layout. This array is populated at the start of
         * layout, and at the end of layout all view in mActiveViews are moved to mScrapViews.
         * Views in mActiveViews represent a contiguous range of Views, with position of the first
         * view store in mFirstActivePosition.
         */
        AutoPtr< ArrayOf<IView*> > mActiveViews;

        /**
         * Unsorted views that can be used by the adapter as a convert view.
         */
        AutoPtr< ArrayOf<IArrayList*> > mScrapViews;


        Int32 mViewTypeCount;

        AutoPtr<IArrayList> mCurrentScrap;

        AutoPtr<IArrayList> mSkippedScrap;

        AutoPtr<ISparseArray> mTransientStateViews;
        AutoPtr<IInt64SparseArray> mTransientStateViewsById;

        AbsListView* mHost;
    };

    class AbsPositionScroller
        : public Object
    {
    public:
        virtual ~AbsPositionScroller();

        virtual CARAPI_(void) Start(
            /* [in] */ const Int32 position) = 0;

        virtual CARAPI_(void) Start(
            /* [in] */ const Int32 position,
            /* [in] */ const Int32 boundPosition) = 0;

        virtual CARAPI_(void) StartWithOffset(
            /* [in] */ Int32 position,
            /* [in] */ Int32 offset) = 0;

        virtual CARAPI_(void) StartWithOffset(
            /* [in] */ Int32 position,
            /* [in] */ Int32 offset,
            /* [in] */ const Int32 duration) = 0;

        virtual CARAPI_(void) Stop() = 0;
    };

    class PositionScroller
        : public AbsPositionScroller
        , public IRunnable
    {
    public:
        CAR_INTERFACE_DECL()

        PositionScroller(
            /* [in] */ AbsListView* host);

        virtual ~PositionScroller();

        CARAPI_(void) Start(
            /* [in] */ const Int32 position);

        CARAPI_(void) Start(
            /* [in] */ const Int32 position,
            /* [in] */ const Int32 boundPosition);

        CARAPI_(void) StartWithOffset(
            /* [in] */ Int32 position,
            /* [in] */ Int32 offset);

        CARAPI_(void) StartWithOffset(
            /* [in] */ Int32 position,
            /* [in] */ Int32 offset,
            /* [in] */ const Int32 duration);

        CARAPI_(void) Stop();

        virtual CARAPI Run();

    private:
        CARAPI_(void) ScrollToVisible(
            /* [in] */ Int32 targetPos,
            /* [in] */ Int32 boundPos,
            /* [in] */ Int32 duration);

    private:
        static const Int32 SCROLL_DURATION;

        static const Int32 MOVE_DOWN_POS;
        static const Int32 MOVE_UP_POS;
        static const Int32 MOVE_DOWN_BOUND;
        static const Int32 MOVE_UP_BOUND;
        static const Int32 MOVE_OFFSET;

    private:
        AbsListView* mHost;
        Int32 mMode;
        Int32 mTargetPos;
        Int32 mBoundPos;
        Int32 mLastSeenPos;
        Int32 mScrollDuration;
        Int32 mExtraScroll;
        Int32 mOffsetFromTop;
        friend class AbsListView;
    };

private:
    /**
     * A base class for Runnables that will check that their view is still attached to
     * the original window as when the Runnable was created.
     *
     */
    class WindowRunnnable
        : public Runnable
    {
    public:
        WindowRunnnable(
            /* [in] */ AbsListView* host);

        CARAPI_(void) RememberWindowAttachCount();

        CARAPI_(Boolean) SameWindow();

        virtual CARAPI Run() { return NOERROR; }

    public:
        AbsListView* mHost;

    private:
        Int32 mOriginalAttachCount;
    };

    class PerformClick
        : public WindowRunnnable
    {
    public:
        PerformClick(
            /* [in] */ AbsListView* host);

        virtual CARAPI Run();

    public:
        Int32 mClickMotionPosition;
    };

    class CheckForLongPress
        : public WindowRunnnable
    {
    public:
        CheckForLongPress(
            /* [in] */ AbsListView* host);

        virtual CARAPI Run();
    };

    class CheckForKeyLongPress
        : public WindowRunnnable
    {
    public:
        CheckForKeyLongPress(
            /* [in] */ AbsListView* host);

        CARAPI Run();
    };

    class CheckForTap
        : public Runnable
    {
    public:
        CheckForTap(
            /* [in] */ AbsListView* host);

        CARAPI Run();

    public:
        Float mX;
        Float mY;
    private:
        AbsListView* mHost;
    };

    /*
     * The member mCheckFlywheel in FlingRunnable is a sub-Runnable
     * so you need implement it here
     */
    class FlingRunnable;
    class FlingRunnableInner
        : public Runnable
    {
    public:
        FlingRunnableInner(
            /* [in] */ FlingRunnable* host,
            /* [in] */ AbsListView* mainHost);

        CARAPI Run();

    private:
        FlingRunnable* mHost;
        AbsListView* mMainHost;
    };

    /**
     * Responsible for fling behavior. Use {@link #start(Int32)} to
     * initiate a fling. Each frame of the fling is handled in {@link #run()}.
     * A FlingRunnable will keep re-posting itself until the fling is done.
     *
     */
    class FlingRunnable
        : public Runnable
    {
    public:
        FlingRunnable(
            /* [in] */ AbsListView* host);

        CARAPI Run();

        CARAPI_(void) Start(
            /* [in] */ Int32 initialVelocity);

        CARAPI_(void) StartSpringback();

        CARAPI_(void) StartOverfling(
            /* [in] */ Int32 initialVelocity);

        CARAPI_(void) EdgeReached(
            /* [in] */ Int32 delta);

        CARAPI_(void) StartScroll(
            /* [in] */ Int32 distance,
            /* [in] */ Int32 duration,
            /* [in] */ Boolean linear);

        CARAPI_(void) EndFling();

        CARAPI_(void) EndFling(
            /* [in] */ Boolean clearCache);

        CARAPI_(void) FlywheelTouch();

    private:
        AbsListView* mHost;

        /**
         * Tracks the decay of a fling scroll
         */
        AutoPtr<IOverScroller> mScroller;

        Int32 mLastFlingY;

        /**
         * Y value reported by mScroller on the previous fling
         */
        AutoPtr<FlingRunnableInner> mCheckFlywheel;

        static const Int32 FLYWHEEL_TIMEOUT = 40; // milliseconds

        friend class FlingRunnableInner;
        friend class AbsListView;
    };

    class InputConnectionWrapper
        : public Object
        , public IInputConnection
    {
    public:
        CAR_INTERFACE_DECL()

        InputConnectionWrapper(
            /* [in] */ AbsListView* host,
            /* [in] */ IEditorInfo* outAttrs);

        ~InputConnectionWrapper();

        CARAPI_(AutoPtr<IInputConnection>) GetTarget();

        // @Override
        CARAPI ReportFullscreenMode(
            /* [in] */ Boolean enabled,
            /* [out] */ Boolean* result);

        // @Override
        CARAPI PerformEditorAction(
            /* [in] */ Int32 actionCode,
            /* [out] */ Boolean* result);

        // @Override
        CARAPI SendKeyEvent(
            /* [in] */ IKeyEvent* event,
            /* [out] */ Boolean* result);

        // @Override
        CARAPI GetTextBeforeCursor(
            /* [in] */ Int32 length,
            /* [in] */ Int32 flags,
            /* [out] */ ICharSequence** seq);

        // @Override
        CARAPI GetTextAfterCursor(
            /* [in] */ Int32 length,
            /* [in] */ Int32 flags,
            /* [out] */ ICharSequence** seq);

        // @Override
        CARAPI GetSelectedText(
            /* [in] */ Int32 flags,
            /* [out] */ ICharSequence** seq);

        // @Override
        CARAPI GetCursorCapsMode(
            /* [in] */ Int32 reqModes,
            /* [out] */ Int32* afterLength);

        // @Override
        CARAPI GetExtractedText(
            /* [in] */ IExtractedTextRequest* request,
            /* [in] */ Int32 flags,
            /* [out] */ IExtractedText** text);

        // @Override
        CARAPI DeleteSurroundingText(
            /* [in] */ Int32 beforeLength,
            /* [in] */ Int32 afterLength,
            /* [out] */ Boolean* result);

        // @Override
        CARAPI SetComposingText(
            /* [in] */ ICharSequence* text,
            /* [in] */ Int32 newCursorPosition,
            /* [out] */ Boolean* result);

        // @Override
        CARAPI SetComposingRegion(
            /* [in] */ Int32 start,
            /* [in] */ Int32 end,
            /* [out] */ Boolean* result);

        // @Override
        CARAPI FinishComposingText(
            /* [out] */ Boolean* result);

        // @Override
        CARAPI CommitText(
            /* [in] */ ICharSequence* text,
            /* [in] */ Int32 newCursorPosition,
            /* [out] */ Boolean* result);

        // @Override
        CARAPI CommitCompletion(
            /* [in] */ ICompletionInfo* text,
            /* [out] */ Boolean* result);

        // @Override
        CARAPI CommitCorrection(
            /* [in] */ ICorrectionInfo* correctionInfo,
            /* [out] */ Boolean* result);

        // @Override
        CARAPI SetSelection(
            /* [in] */ Int32 start,
            /* [in] */ Int32 end,
            /* [out] */ Boolean* result);

        // @Override
        CARAPI PerformContextMenuAction(
            /* [in] */ Int32 id,
            /* [out] */ Boolean* result);

        // @Override
        CARAPI BeginBatchEdit(
            /* [out] */ Boolean* result);

        // @Override
        CARAPI EndBatchEdit(
            /* [out] */ Boolean* result);

        // @Override
        CARAPI ClearMetaKeyStates(
            /* [in] */ Int32 states,
            /* [out] */ Boolean* result);

        // @Override
        CARAPI PerformPrivateCommand(
            /* [in] */ const String& action,
            /* [in] */ IBundle* data,
            /* [out] */ Boolean* result);

        // @Override
        CARAPI RequestCursorUpdates(
            /* [in] */ Int32 cursorUpdateMode,
            /* [out] */ Boolean* result);

    private:
        AbsListView* mHost;
        AutoPtr<IEditorInfo> mOutAttrs;
        AutoPtr<IInputConnection> mTarget;
    };

    // This class is create for the runnable in PositionScroller::Start functions
    class PositionScrollerStartRunnable
        : public Runnable
    {
    public:
        PositionScrollerStartRunnable(
            /* [in] */ Int32 paramsCount,
            /* [in] */ Int32 param1,
            /* [in] */ Int32 param2,
            /* [in] */ Int32 param3,
            /* [in] */ PositionScroller* host);

        CARAPI Run();

    private:
        const Int32 mParamsCount;
        Int32 mParam1;
        Int32 mParam2;
        Int32 mParam3;
        PositionScroller* mHost;
    };

private:
    // The sub-Runnable in AbsListView::SetFastScrollEnabled, SetFastScrollAlwaysVisible
    class SetFastScrollRunnable
        : public Runnable
    {
    public:
        SetFastScrollRunnable(
            /* [in] */ AbsListView* host,
            /* [in] */ Boolean res,
            /* [in] */ Int32 id);

        CARAPI Run();

    private:
        AbsListView* mHost;
        Boolean mRes;
        Int32 mId;
    };

    // The sub-Runnable in AbsListView::ClearScrollingCache
    class ClearScrollingCacheRunnable
        : public Runnable
    {
    public:
        ClearScrollingCacheRunnable(
            /* [in] */ AbsListView* host);

        CARAPI Run();

    private:
        AbsListView* mHost;
    };

    // The sub-Runnable for AbsListView::OnTouchUp
    class TouchModeResetRunnable
        : public Runnable
    {
    public:
        TouchModeResetRunnable(
            /* [in] */ IRunnable* r,
            /* [in] */ AbsListView* host,
            /* [in] */ IView* v);

        CARAPI Run();

    private:
        AutoPtr<IRunnable> mRunnable;
        AbsListView* mHost;
        AutoPtr<IView> mView;
    };

public:
    CAR_INTERFACE_DECL()

    AbsListView();

    ~AbsListView();

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

    //@Override
    CARAPI SetOverScrollMode(
        /* [in] */ Int32 mode);

    /**
     * {@inheritDoc}
     */
    //@Override
    CARAPI SetAdapter(
        /* [in] */ IAdapter* adapter);

    /**
     * Returns the number of items currently selected. This will only be valid
     * if the choice mode is not {@link #CHOICE_MODE_NONE} (default).
     *
     * <p>To determine the specific items that are currently selected, use one of
     * the <code>getChecked*</code> methods.
     *
     * @return The number of items currently selected
     *
     * @see #getCheckedItemPosition()
     * @see #getCheckedItemPositions()
     * @see #getCheckedItemIds()
     */
    CARAPI GetCheckedItemCount(
        /* [out] */ Int32* count);

    /**
     * Returns the checked state of the specified position. The result is only
     * valid if the choice mode has been set to {@link #CHOICE_MODE_SINGLE}
     * or {@link #CHOICE_MODE_MULTIPLE}.
     *
     * @param position The item whose checked state to return
     * @return The item's checked state or <code>false</code> if choice mode
     *         is invalid
     *
     * @see #setChoiceMode(int)
     */
    CARAPI IsItemChecked(
        /* [in] */ Int32 position,
        /* [out] */ Boolean* result);

    /**
     * Returns the currently checked item. The result is only valid if the choice
     * mode has been set to {@link #CHOICE_MODE_SINGLE}.
     *
     * @return The position of the currently checked item or
     *         {@link #INVALID_POSITION} if nothing is selected
     *
     * @see #setChoiceMode(int)
     */
    CARAPI GetCheckedItemPosition(
            /* [out] */ Int32* result);

    /**
     * Returns the set of checked items in the list. The result is only valid if
     * the choice mode has not been set to {@link #CHOICE_MODE_NONE}.
     *
     * @return  A SparseBooleanArray which will return true for each call to
     *          get(int position) where position is a checked position in the
     *          list and false otherwise, or <code>null</code> if the choice
     *          mode is set to {@link #CHOICE_MODE_NONE}.
     */
    CARAPI GetCheckedItemPositions(
        /* [out] */ ISparseBooleanArray** arr);

    /**
     * Returns the set of checked items ids. The result is only valid if the
     * choice mode has not been set to {@link #CHOICE_MODE_NONE} and the adapter
     * has stable IDs. ({@link ListAdapter#hasStableIds()} == {@code true})
     *
     * @return A new array which contains the id of each checked item in the
     *         list.
     */
    CARAPI GetCheckedItemIds(
        /* [out] */ ArrayOf<Int64>** result);

    /**
     * Clear any choices previously set
     */
    CARAPI ClearChoices();

    /**
     * Sets the checked state of the specified position. The is only valid if
     * the choice mode has been set to {@link #CHOICE_MODE_SINGLE} or
     * {@link #CHOICE_MODE_MULTIPLE}.
     *
     * @param position The item whose checked state is to be checked
     * @param value The new checked state for the item
     */
    CARAPI SetItemChecked(
        /* [in] */ Int32 position,
        /* [in] */ Boolean value);

    // @Override
    CARAPI PerformItemClick(
        /* [in] */ IView* view,
        /* [in] */ Int32 position,
        /* [in] */ Int64 id,
        /* [out] */ Boolean* result);

    /**
     * @see #setChoiceMode(int)
     *
     * @return The current choice mode
     */
    CARAPI GetChoiceMode(
        /* [out] */ Int32* result);

    /**
     * Defines the choice behavior for the List. By default, Lists do not have any choice behavior
     * ({@link #CHOICE_MODE_NONE}). By setting the choiceMode to {@link #CHOICE_MODE_SINGLE}, the
     * List allows up to one item to  be in a chosen state. By setting the choiceMode to
     * {@link #CHOICE_MODE_MULTIPLE}, the list allows any number of items to be chosen.
     *
     * @param choiceMode One of {@link #CHOICE_MODE_NONE}, {@link #CHOICE_MODE_SINGLE}, or
     * {@link #CHOICE_MODE_MULTIPLE}
     */
    CARAPI SetChoiceMode(
        /* [in] */ Int32 choiceMode);

    /**
     * Set a {@link MultiChoiceModeListener} that will manage the lifecycle of the
     * selection {@link ActionMode}. Only used when the choice mode is set to
     * {@link #CHOICE_MODE_MULTIPLE_MODAL}.
     *
     * @param listener Listener that will manage the selection mode
     *
     * @see #setChoiceMode(int)
     */
    CARAPI SetMultiChoiceModeListener(
        /* [in] */ IMultiChoiceModeListener* listener);

    /**
     * Specifies whether fast scrolling is enabled or disabled.
     * <p>
     * When fast scrolling is enabled, the user can quickly scroll through lists
     * by dragging the fast scroll thumb.
     * <p>
     * If the adapter backing this list implements {@link SectionIndexer}, the
     * fast scroller will display section header previews as the user scrolls.
     * Additionally, the user will be able to quickly jump between sections by
     * tapping along the length of the scroll bar.
     *
     * @see SectionIndexer
     * @see #isFastScrollEnabled()
     * @param enabled true to enable fast scrolling, false otherwise
     */
    CARAPI SetFastScrollEnabled(
        /* [in] */ Boolean enabled);

    /**
     * Specifies the style of the fast scroller decorations.
     *
     * @param styleResId style resource containing fast scroller properties
     * @see android.R.styleable#FastScroll
     */
    CARAPI SetFastScrollStyle(
        /* [in] */ Int32 styleResId);

    /**
     * Set whether or not the fast scroller should always be shown in place of
     * the standard scroll bars. This will enable fast scrolling if it is not
     * already enabled.
     * <p>
     * Fast scrollers shown in this way will not fade out and will be a
     * permanent fixture within the list. This is best combined with an inset
     * scroll bar style to ensure the scroll bar does not overlap content.
     *
     * @param alwaysShow true if the fast scroller should always be displayed,
     *            false otherwise
     * @see #setScrollBarStyle(int)
     * @see #setFastScrollEnabled(boolean)
     */
    CARAPI SetFastScrollAlwaysVisible(
        /* [in] */ Boolean alwaysShow);

    /**
     * Returns true if the fast scroller is set to always show on this view.
     *
     * @return true if the fast scroller will always show
     * @see #setFastScrollAlwaysVisible(boolean)
     */
    CARAPI IsFastScrollAlwaysVisible(
        /* [out] */ Boolean* result);

    //@Override
    CARAPI GetVerticalScrollbarWidth(
        /* [out] */ Int32* width);

    /**
     * Returns true if the fast scroller is enabled.
     *
     * @see #setFastScrollEnabled(boolean)
     * @return true if fast scroll is enabled, false otherwise
     */
    // @ViewDebug.ExportedProperty
    CARAPI IsFastScrollEnabled(
        /* [out] */ Boolean* isEnabled);

    //@Override
    CARAPI SetVerticalScrollbarPosition(
        /* [in] */ Int32 result);

    //@Override
    CARAPI SetScrollBarStyle(
        /* [in] */ Int32 style);

    /**
     * When smooth scrollbar is enabled, the position and size of the scrollbar thumb
     * is computed based on the number of visible pixels in the visible items. This
     * however assumes that all list items have the same height. If you use a list in
     * which items have different heights, the scrollbar will change appearance as the
     * user scrolls through the list. To avoid this issue, you need to disable this
     * property.
     *
     * When smooth scrollbar is disabled, the position and size of the scrollbar thumb
     * is based solely on the number of items in the adapter and the position of the
     * visible items inside the adapter. This provides a stable scrollbar as the user
     * navigates through a list of items with varying heights.
     *
     * @param enabled Whether or not to enable smooth scrollbar.
     *
     * @see #setSmoothScrollbarEnabled(Boolean)
     * @attr ref android.R.styleable#AbsListView_smoothScrollbar
     */
    CARAPI SetSmoothScrollbarEnabled(
        /* [in] */ Boolean enabled);

    /**
     * Returns the current state of the fast scroll feature.
     *
     * @return True if smooth scrollbar is enabled is enabled, FALSE otherwise.
     *
     * @see #setSmoothScrollbarEnabled(Boolean)
     */
    //@ViewDebug.ExportedProperty
    CARAPI IsSmoothScrollbarEnabled(
        /* [out] */ Boolean* isEnabled);

    /**
     * Set the listener that will receive notifications every time the list scrolls.
     *
     * @param l the scroll listener
     */
    CARAPI SetOnScrollListener(
        /* [in] */ IAbsListViewOnScrollListener* l);

    // @Override
    CARAPI SendAccessibilityEvent(
        /* [in] */ Int32 eventType);

    //@Override
    CARAPI OnInitializeAccessibilityEvent(
        /* [in] */ IAccessibilityEvent* event);

    //@Override
    CARAPI OnInitializeAccessibilityNodeInfo(
        /* [in] */ IAccessibilityNodeInfo* info);

    CARAPI PerformAccessibilityAction(
        /* [in] */ Int32 action,
        /* [in] */ IBundle* arguments,
        /* [out] */ Boolean* res);

    /** @hide */
    // @Override
    CARAPI FindViewByAccessibilityIdTraversal(
        /* [in] */ Int32 accessibilityId,
        /* [out] */ IView** view);

    /**
     * Indicates whether the children's drawing cache is used during a scroll.
     * By default, the drawing cache is enabled but this will consume more memory.
     *
     * @return TRUE if the scrolling cache is enabled, FALSE otherwise
     *
     * @see #setScrollingCacheEnabled(Boolean)
     * @see View#setDrawingCacheEnabled(Boolean)
     */
    //@ViewDebug.ExportedProperty
    CARAPI IsScrollingCacheEnabled(
        /* [out] */ Boolean* isEnabled);

    /**
     * Enables or disables the children's drawing cache during a scroll.
     * By default, the drawing cache is enabled but this will use more memory.
     *
     * When the scrolling cache is enabled, the caches are kept after the
     * first scrolling. You can manually clear the cache by calling
     * {@link android.view.ViewGroup#setChildrenDrawingCacheEnabled(Boolean)}.
     *
     * @param enabled TRUE to enable the scroll cache, FALSE otherwise
     *
     * @see #isScrollingCacheEnabled()
     * @see View#setDrawingCacheEnabled(Boolean)
     */
    CARAPI SetScrollingCacheEnabled(
        /* [in] */ Boolean enabled);

    /**
     * Enables or disables the type filter window. If enabled, typing when
     * this view has focus will filter the children to match the users input.
     * Note that the {@link Adapter} used by this view must implement the
     * {@link Filterable} interface.
     *
     * @param textFilterEnabled TRUE to enable type filtering, FALSE otherwise
     *
     * @see Filterable
     */
    CARAPI SetTextFilterEnabled(
        /* [in] */ Boolean textFilterEnabled);

    /**
     * Indicates whether type filtering is enabled for this view
     *
     * @return TRUE if type filtering is enabled, FALSE otherwise
     *
     * @see #setTextFilterEnabled(Boolean)
     * @see Filterable
     */
    //@ViewDebug.ExportedProperty
    CARAPI IsTextFilterEnabled(
        /* [out] */ Boolean* isEnabled);

    //@Override
    CARAPI GetFocusedRect(
        /* [in, out] */ IRect* r);

    /**
     * Indicates whether the content of this view is pinned to, or stacked from,
     * the bottom edge.
     *
     * @return TRUE if the content is stacked from the bottom edge, FALSE otherwise
     */
    //@ViewDebug.ExportedProperty
    CARAPI IsStackFromBottom(
        /* [out] */ Boolean* isStackFromBottom);

    /**
     * When stack from bottom is set to TRUE, the list fills its content starting from
     * the bottom of the view.
     *
     * @param stackFromBottom TRUE to pin the view's content to the bottom edge,
     *        FALSE to pin the view's content to the top edge
     */
    CARAPI SetStackFromBottom(
        /* [in] */ Boolean stackFromBottom);

    //@Override
    CARAPI_(AutoPtr<IParcelable>) OnSaveInstanceState();

    //@Override
    CARAPI OnRestoreInstanceState(
        /* [in] */ IParcelable* state);

    /**
     * Sets the initial value for the text filter.
     * @param filterText The text to use for the filter.
     *
     * @see #setTextFilterEnabled
     */
    CARAPI SetFilterText(
        /* [in] */ const String& filterText);

    /**
     * Returns the list's text filter, if available.
     * @return the list's text filter or NULL if filtering isn't enabled
     */
    CARAPI GetTextFilter(
        /* [out] */ ICharSequence** textFilter);

    //@Override
    CARAPI RequestLayout();

    //@Override
    //@ViewDebug.ExportedProperty
    CARAPI GetSelectedView(
        /* [out] */ IView** view);

    /**
     * List padding is the maximum of the normal view's padding and the padding of the selector.
     *
     * @see android.view.View#getPaddingTop()
     * @see #getSelector()
     *
     * @return The top list padding.
     */
    CARAPI GetListPaddingTop(
        /* [out] */ Int32* top);

    /**
     * List padding is the maximum of the normal view's padding and the padding of the selector.
     *
     * @see android.view.View#getPaddingBottom()
     * @see #getSelector()
     *
     * @return The bottom list padding.
     */
    CARAPI GetListPaddingBottom(
        /* [out] */ Int32* bottom);

    /**
     * List padding is the maximum of the normal view's padding and the padding of the selector.
     *
     * @see android.view.View#getPaddingLeft()
     * @see #getSelector()
     *
     * @return The left list padding.
     */
    CARAPI GetListPaddingLeft(
        /* [out] */ Int32* left);

    /**
     * List padding is the maximum of the normal view's padding and the padding of the selector.
     *
     * @see android.view.View#getPaddingRight()
     * @see #getSelector()
     *
     * @return The right list padding.
     */
    CARAPI GetListPaddingRight(
        /* [out] */ Int32* right);

    /**
     * Initializes an {@link AccessibilityNodeInfo} with information about a
     * particular item in the list.
     *
     * @param view View representing the list item.
     * @param position Position of the list item within the adapter.
     * @param info Node info to populate.
     */
    CARAPI OnInitializeAccessibilityNodeInfoForItem(
        /* [in] */ IView* view,
        /* [in] */ Int32 position,
        /* [in] */ IAccessibilityNodeInfo* info);

    /**
     * Controls whether the selection highlight drawable should be drawn on top of the item or
     * behind it.
     *
     * @param onTop If TRUE, the selector will be drawn on the item it is highlighting. The default
     *        is FALSE.
     *
     * @attr ref android.R.styleable#AbsListView_drawSelectorOnTop
     */
    CARAPI SetDrawSelectorOnTop(
        /* [in] */ Boolean onTop);

    /**
     * Set a Drawable that should be used to highlight the currently selected item.
     *
     * @param resID A Drawable resource to use as the selection highlight.
     *
     * @attr ref android.R.styleable#AbsListView_listSelector
     */
    CARAPI SetSelector(
        /* [in] */ Int32 resID);

    CARAPI SetSelector(
        /* [in] */ IDrawable* sel);

    /**
     * Returns the selector {@link android.graphics.drawable.Drawable} that is used to draw the
     * selection in the list.
     *
     * @return the drawable used to display the selector
     */
    CARAPI GetSelector(
        /* [out] */ IDrawable** selector);

    CARAPI SetScrollIndicators(
        /* [in] */ IView* up,
        /* [in] */ IView* down);

    //@Override
    CARAPI_(Boolean) VerifyDrawable(
        /* [in] */ IDrawable* dr);

    CARAPI JumpDrawablesToCurrentState();

    //@Override
    CARAPI OnWindowFocusChanged(
        /* [in] */ Boolean hasWindowFocus);

    //@Override
    CARAPI OnRtlPropertiesChanged(
        /* [in] */ Int32 layoutDirection);

    //@Override
    CARAPI OnCancelPendingInputEvents();

    // You must override all ShowContextMenu function in super class
    // to avoid hiding for C++；
    // The function will invoke the one in super class directly；
    CARAPI ShowContextMenu(
        /* [out] */ Boolean* res);

    //@Override
    CARAPI ShowContextMenu(
        /* [in] */ Float x,
        /* [in] */ Float y,
        /* [in] */ Int32 metaState,
        /* [out] */ Boolean* res);

    //@Override
    CARAPI ShowContextMenuForChild(
        /* [in] */ IView* originalView,
        /* [out] */ Boolean* res);

    //@Override
    CARAPI OnKeyDown(
        /* [in] */ Int32 keyCode,
        /* [in] */ IKeyEvent* event,
        /* [out] */ Boolean* res);

    //@Override
    CARAPI OnKeyUp(
        /* [in] */ Int32 keyCode,
        /* [in] */ IKeyEvent* event,
        /* [out] */ Boolean* res);

    /**
     * Maps a point to a position in the list.
     *
     * @param x X in local coordinate
     * @param y Y in local coordinate
     * @return The position of the item which contains the specified point, or
     *         {@link #AdapterView_INVALID_POSITION} if the point does not intersect an item.
     */
    CARAPI PointToPosition(
        /* [in] */ Int32 x,
        /* [in] */ Int32 y,
        /* [out] */ Int32* position);

    /**
     * Maps a point to a the rowId of the item which intersects that point.
     *
     * @param x X in local coordinate
     * @param y Y in local coordinate
     * @return The rowId of the item which contains the specified point, or {@link #AdapterView_INVALID_ROW_ID}
     *         if the point does not intersect an item.
     */
    CARAPI PointToRowId(
        /* [in] */ Int32 x,
        /* [in] */ Int32 y,
        /* [out] */ Int64* rowId);

    CARAPI OnTouchModeChanged(
        /* [in] */ Boolean isInTouchMode);

    //@Override
    CARAPI OnTouchEvent(
        /* [in] */ IMotionEvent* ev,
        /* [out] */ Boolean* res);

    CARAPI OnGenericMotionEvent(
        /* [in] */ IMotionEvent* event,
        /* [out] */ Boolean* res);

    /**
     * Initiate a fling with the given velocity.
     *
     * <p>Applications can use this method to manually initiate a fling as if the user
     * initiated it via touch interaction.</p>
     *
     * @param velocityY Vertical velocity in pixels per second. Note that this is velocity of
     *                  content, not velocity of a touch that initiated the fling.
     */
    CARAPI Fling(
        /* [in] */ Int32 velocityY);

    // @Override
    CARAPI OnStartNestedScroll(
        /* [in] */ IView* child,
        /* [in] */ IView* target,
        /* [in] */ Int32 nestedScrollAxes,
        /* [out] */ Boolean* res);

    // @Override
    CARAPI OnNestedScrollAccepted(
        /* [in] */ IView* child,
        /* [in] */ IView* target,
        /* [in] */ Int32 axes);

    // @Override
    CARAPI OnNestedScroll(
        /* [in] */ IView* target,
        /* [in] */ Int32 dxConsumed,
        /* [in] */ Int32 dyConsumed,
        /* [in] */ Int32 dxUnconsumed,
        /* [in] */ Int32 dyUnconsumed);

    // @Override
    CARAPI OnNestedFling(
        /* [in] */ IView* target,
        /* [in] */ Float velocityX,
        /* [in] */ Float velocityY,
        /* [in] */ Boolean consumed,
        /* [out] */ Boolean* res);

    //@Override
    CARAPI Draw(
        /* [in] */ ICanvas* canvas);

    CARAPI SetOverScrollEffectPadding(
        /* [in] */ Int32 leftPadding,
        /* [in] */ Int32 rightPadding);

    CARAPI RequestDisallowInterceptTouchEvent(
        /* [in] */ Boolean disallowIntercept);

    CARAPI OnInterceptHoverEvent(
        /* [in] */ IMotionEvent* event,
        /* [out] */ Boolean* res);

    //@Override
    CARAPI OnInterceptTouchEvent(
        /* [in] */ IMotionEvent* ev,
        /* [out] */ Boolean* res);

    /**
     * {@inheritDoc}
     */
    //@Override
    CARAPI AddTouchables(
        /* [in] */ IArrayList* views);

     /**
     * The amount of friction applied to flings. The default value
     * is {@link ViewConfiguration#getScrollFriction}.
     */
    CARAPI SetFriction(
        /* [in] */ Float friction);

    /**
     * Sets a scale factor for the fling velocity. The initial scale
     * factor is 1.0.
     *
     * @param scale The scale factor to multiply the velocity by.
     */
    CARAPI SetVelocityScale(
        /* [in] */ Float scale);

    /**
     * Smoothly scroll to the specified adapter position. The view will
     * scroll such that the indicated position is displayed.
     * @param position Scroll to this adapter position.
     */
    CARAPI SmoothScrollToPosition(
        /* [in] */ Int32 position);

    /**
     * Smoothly scroll to the specified adapter position. The view will scroll
     * such that the indicated position is displayed <code>offset</code> pixels below
     * the top edge of the view. If this is impossible, (e.g. the offset would scroll
     * the first or last item beyond the boundaries of the list) it will get as close
     * as possible. The scroll will take <code>duration</code> milliseconds to complete.
     *
     * @param position Position to scroll to
     * @param offset Desired distance in pixels of <code>position</code> from the top
     *               of the view when scrolling is finished
     * @param duration Number of milliseconds to use for the scroll
     */
    CARAPI SmoothScrollToPositionFromTop(
        /* [in] */ Int32 position,
        /* [in] */ Int32 offset,
        /* [in] */ Int32 duration);

    /**
     * Smoothly scroll to the specified adapter position. The view will scroll
     * such that the indicated position is displayed <code>offset</code> pixels below
     * the top edge of the view. If this is impossible, (e.g. the offset would scroll
     * the first or last item beyond the boundaries of the list) it will get as close
     * as possible.
     *
     * @param position Position to scroll to
     * @param offset Desired distance in pixels of <code>position</code> from the top
     *               of the view when scrolling is finished
     */
    CARAPI SmoothScrollToPositionFromTop(
        /* [in] */ Int32 position,
        /* [in] */ Int32 offset);

    /**
     * Smoothly scroll to the specified adapter position. The view will
     * scroll such that the indicated position is displayed, but it will
     * stop early if scrolling further would scroll boundPosition out of
     * view.
     * @param position Scroll to this adapter position.
     * @param boundPosition Do not scroll if it would move this adapter
     *          position out of view.
     */
    CARAPI SmoothScrollToPosition(
        /* [in] */ Int32 position,
        /* [in] */ Int32 boundPosition);

    /**
     * Smoothly scroll by distance pixels over duration milliseconds.
     * @param distance Distance to scroll in pixels.
     * @param duration Duration of the scroll animation in milliseconds.
     */
    CARAPI SmoothScrollBy(
        /* [in] */ Int32 distance,
        /* [in] */ Int32 duration);

    /**
     * Scrolls the list items within the view by a specified number of pixels.
     *
     * @param y the amount of pixels to scroll by vertically
     * @see #canScrollList(int)
     */
    CARAPI ScrollListBy(
        /* [in] */ Int32 y);

    /**
     * Check if the items in the list can be scrolled in a certain direction.
     *
     * @param direction Negative to check scrolling up, positive to check
     *            scrolling down.
     * @return true if the list can be scrolled in the specified direction,
     *         false otherwise.
     * @see #scrollListBy(int)
     */
    CARAPI CanScrollList(
        /* [in] */ Int32 direction,
        /* [out] */ Boolean* canScrollList);

    /**
     * Causes all the views to be rebuilt and redrawn.
     */
    CARAPI InvalidateViews();

    /**
     * Return an InputConnection for editing of the filter text.
     */
    //@Override
    CARAPI OnCreateInputConnection(
        /* [in] */ IEditorInfo* outAttrs,
        /* [out] */ IInputConnection** connection);

    /**
     * For filtering we proxy an input connection to an internal text editor,
     * and this allows the proxying to happen.
     */
    //@Override
    CARAPI CheckInputConnectionProxy(
        /* [in] */ IView* view,
        /* [out] */ Boolean* res);

    /**
     * Clear the text filter.
     */
    CARAPI ClearTextFilter();

    /**
     * Returns if the ListView currently has a text filter.
     */
    CARAPI HasTextFilter(
        /* [out] */ Boolean* hasTextFilter);

    //@Override
    CARAPI OnGlobalLayout();

    /**
     * For our text watcher that is associated with the text filter.  Does
     * nothing.
     */
    //@Override
    CARAPI BeforeTextChanged(
        /* [in] */ ICharSequence* s,
        /* [in] */ Int32 start,
        /* [in] */ Int32 count,
        /* [in] */ Int32 after);

    /**
     * For our text watcher that is associated with the text filter. Performs
     * the actual filtering as the text changes, and takes care of hiding and
     * showing the popup displaying the currently entered filter text.
     */
    //@Override
    CARAPI OnTextChanged(
        /* [in] */ ICharSequence* s,
        /* [in] */ Int32 start,
        /* [in] */ Int32 before,
        /* [in] */ Int32 count);

    /**
     * For our text watcher that is associated with the text filter.  Does
     * nothing.
     */
    //@Override
    CARAPI AfterTextChanged(
        /* [in] */ IEditable* s);

    //@Override
    CARAPI OnFilterComplete(
        /* [in] */ Int32 count);

    //@Override
    CARAPI GenerateLayoutParams(
        /* [in] */ IAttributeSet* attrs,
        /* [out] */ IViewGroupLayoutParams** params);

    /**
     * Puts the list or grid into transcript mode. In this mode the list or grid will always scroll
     * to the bottom to show new items.
     *
     * @param mode the transcript mode to set
     *
     * @see #AbsListView_TRANSCRIPT_MODE_DISABLED
     * @see #AbsListView_TRANSCRIPT_MODE_NORMAL
     * @see #AbsListView_TRANSCRIPT_MODE_ALWAYS_SCROLL
     */
    CARAPI SetTranscriptMode(
        /* [in] */ Int32 mode);

    /**
     * Returns the current transcript mode.
     *
     * @return {@link #AbsListView_TRANSCRIPT_MODE_DISABLED}, {@link #AbsListView_TRANSCRIPT_MODE_NORMAL} or
     *         {@link #AbsListView_TRANSCRIPT_MODE_ALWAYS_SCROLL}
     */
    CARAPI GetTranscriptMode(
        /* [out] */ Int32* mode);

    //@Override
    CARAPI GetSolidColor(
        /* [out] */ Int32* color);

    /**
     * When set to a non-zero value, the cache color hint indicates that this list is always drawn
     * on top of a solid, single-color, opaque background
     *
     * @param color The background color
     */
    CARAPI SetCacheColorHint(
        /* [in] */ Int32 color);

    /**
     * When set to a non-zero value, the cache color hint indicates that this list is always drawn
     * on top of a solid, single-color, opaque background
     *
     * @return The cache color hint
     */
    CARAPI GetCacheColorHint(
        /* [out] */ Int32* colorHint);

    /**
     * Move all views (excluding headers and footers) held by this AbsListView into the supplied
     * List. This includes views displayed on the screen as well as views stored in AbsListView's
     * internal view recycler.
     *
     * @param views A list into which to put the reclaimed views
     */
    CARAPI ReclaimViews(
        /* [in] */ IList* views);

    /**
     * Sets up this AbsListView to use a remote views adapter which connects to a RemoteViewsService
     * through the specified intent.
     * @param intent the intent used to identify the RemoteViewsService for the adapter to connect to.
     */
    CARAPI SetRemoteViewsAdapter(
        /* [in] */ IIntent* intent);

    /**
     * Sets up the onClickHandler to be used by the RemoteViewsAdapter when inflating RemoteViews
     *
     * @param handler The OnClickHandler to use when inflating RemoteViews.
     *
     * @hide
     */
    CARAPI SetRemoteViewsOnClickHandler(
        /* [in] */ IRemoteViewsOnClickHandler* handler);

    /**
     * This defers a notifyDataSetChanged on the pending RemoteViewsAdapter if it has not
     * connected yet.
     */
    // @Override
    CARAPI DeferNotifyDataSetChanged();

    /**
     * Called back when the adapter connects to the RemoteViewsService.
     */
    // @Override
    CARAPI OnRemoteAdapterConnected(
        /* [out] */ Boolean* result);

    /**
     * Called back when the adapter disconnects from the RemoteViewsService.
     */
    // @Override
    CARAPI OnRemoteAdapterDisconnected();

    /**
     * Sets the recycler listener to be notified whenever a View is set aside in
     * the recycler for later reuse. This listener can be used to free resources
     * associated to the View::
     *
     * @param listener The recycler listener to be notified of views set aside
     *        in the recycler.
     *
     * @see android.widget.AbsListView.RecycleBin
     * @see android.widget.AbsListView.RecyclerListener
     */
    CARAPI SetRecyclerListener(
        /* [in] */ IRecyclerListener* listener);

    /**
     * Sets the selected item and positions the selection y pixels from the top edge
     * of the ListView. (If in touch mode, the item will not be selected but it will
     * still be positioned appropriately.)
     *
     * @param position Index (starting at 0) of the data item to be selected.
     * @param y The distance from the top edge of the ListView (plus padding) that the
     *        item will be positioned.
     */
    CARAPI SetSelectionFromTop(
        /* [in] */ Int32 position,
        /* [in] */ Int32 y);

    virtual CARAPI HideSelector();

protected:
    /**
     * What is the distance between the source and destination rectangles given the direction of
     * focus navigation between them? The direction basically helps figure out more quickly what is
     * self evident by the relationship between the rects...
     *
     * @param source the source rectangle
     * @param dest the destination rectangle
     * @param direction the direction
     * @return the distance between the rectangles
     */
    static CARAPI GetDistance(
        /* [in] */ IRect* sourceRect,
        /* [in] */ IRect* destRect,
        /* [in] */ Int32 direction,
        /* [out] */ Int32* result);

protected:
    /**
     * If fast scroll is enabled, then don't draw the vertical scrollbar.
     * @hide
     */
    // @Override
    CARAPI_(Boolean) IsVerticalScrollBarHidden();

    virtual CARAPI InvokeOnItemScrollListener();

    CARAPI_(Int32) GetSelectionModeForAccessibility();

    virtual CARAPI RequestLayoutIfNecessary();

    //@Override
    CARAPI_(void) OnFocusChanged(
        /* [in] */ Boolean gainFocus,
        /* [in] */ Int32 direction,
        /* [in] */ IRect* previouslyFocusedRect);

    virtual CARAPI_(void) ResetList();

    //@Override
    CARAPI_(Int32) ComputeVerticalScrollExtent();

    //@Override
    CARAPI_(Int32) ComputeVerticalScrollOffset();

    //@Override
    CARAPI_(Int32) ComputeVerticalScrollRange();

    //@Override
    CARAPI_(Float) GetTopFadingEdgeStrength();

    //@Override
    CARAPI_(Float) GetBottomFadingEdgeStrength();

    //@Override
    CARAPI OnMeasure(
        /* [in] */ Int32 widthMeasureSpec,
        /* [in] */ Int32 heightMeasureSpec);

    //@Override
    CARAPI OnLayout(
        /* [in] */ Boolean changed,
        /* [in] */ Int32 l,
        /* [in] */ Int32 t,
        /* [in] */ Int32 r,
        /* [in] */ Int32 b);

    //@Override
    CARAPI_(Boolean) SetFrame(
        /* [in] */ Int32 left,
        /* [in] */ Int32 top,
        /* [in] */ Int32 right,
        /* [in] */ Int32 bottom);

    virtual CARAPI LayoutChildren();

    /*
     * @param focusedView view that holds accessibility focus
     * @return direct child that contains accessibility focus, or null if no
     *         child contains accessibility focus
     */
    virtual CARAPI_(AutoPtr<IView>) GetAccessibilityFocusedChild(
        /* [in] */ IView* focusedView);

    virtual CARAPI_(void) UpdateScrollIndicators();

    virtual CARAPI_(AutoPtr<IView>) ObtainView(
        /* [in] */ Int32 position,
        /* [in] */ ArrayOf<Boolean>* isScrap);

    /**
     * Positions the selector in a way that mimics touch.
     */
    virtual CARAPI_(void) PositionSelectorLikeTouch(
        /* [in] */ Int32 position,
        /* [in] */ IView* sel,
        /* [in] */ Float x,
        /* [in] */ Float y);

    /**
     * Positions the selector in a way that mimics keyboard focus.
     */
    virtual CARAPI_(void) PositionSelectorLikeFocus(
        /* [in] */ Int32 position,
        /* [in] */ IView* sel);

    CARAPI_(void) PositionSelector(
        /* [in] */ Int32 position,
        /* [in] */ IView* sel);

    //@Override
    CARAPI DispatchDraw(
        /* [in] */ ICanvas* canvas);

    //@Override
    CARAPI_(Boolean) IsPaddingOffsetRequired();

    //@Override
    CARAPI_(Int32) GetLeftPaddingOffset();

    //@Override
    CARAPI_(Int32) GetTopPaddingOffset();

    //@Override
    CARAPI_(Int32) GetRightPaddingOffset();

    //@Override
    CARAPI_(Int32) GetBottomPaddingOffset();

    //@Override
    CARAPI OnSizeChanged(
        /* [in] */ Int32 w,
        /* [in] */ Int32 h,
        /* [in] */ Int32 oldw,
        /* [in] */ Int32 oldh);

    /**
     * @return True if the current touch mode requires that we draw the selector in the pressed
     *         state.
     */
    virtual CARAPI_(Boolean) TouchModeDrawsInPressedState();

    /**
     * Indicates whether this view is in a state where the selector should be drawn. This will
     * happen if we have focus but are not in touch mode, or we are in the middle of displaying
     * the pressed state for an item.
     *
     * @return True if the selector should be shown
     */
    virtual CARAPI_(Boolean) ShouldShowSelector();

    /**
     * Sets the selector state to "pressed" and posts a CheckForKeyLongPress to see if
     * this is a Int64 press.
     */
    virtual CARAPI_(void) KeyPressed();

    virtual CARAPI_(void) UpdateSelectorState();

    //@Override
    virtual CARAPI DrawableStateChanged();

    //@Override
    virtual CARAPI OnCreateDrawableState(
        /* [in] */ Int32 extraSpace,
        /* [out] */ ArrayOf<Int32>** drawableState);

    //@Override
    virtual CARAPI OnAttachedToWindow();

    //@Override
    virtual CARAPI OnDetachedFromWindow();

    /**
     * Creates the ContextMenuInfo returned from {@link #getContextMenuInfo()}. This
     * methods knows the view, position and ID of the item that received the
     * Int64 press.
     *
     * @param view The view that received the Int64 press.
     * @param position The position of the item that received the Int64 press.
     * @param id The ID of the item that received the Int64 press.
     * @return The extra information that should be returned by
     *         {@link #getContextMenuInfo()}.
     */
    virtual CARAPI_(AutoPtr<IContextMenuInfo>) CreateContextMenuInfo(
        /* [in] */ IView* view,
        /* [in] */ Int32 position,
        /* [in] */ Int64 id);

    virtual CARAPI_(Boolean) PerformLongPress(
        /* [in] */ IView* child,
        /* [in] */ Int32 longPressPosition,
        /* [in] */ Int64 longPressId);

    //@Override
    virtual CARAPI_(AutoPtr<IContextMenuInfo>) GetContextMenuInfo();

    //@Override
    virtual CARAPI_(void) DispatchSetPressed(
        /* [in] */ Boolean pressed);

    //@Override
    virtual CARAPI OnOverScrolled(
        /* [in] */ Int32 scrollX,
        /* [in] */ Int32 scrollY,
        /* [in] */ Boolean clampedX,
        /* [in] */ Boolean clampedY);

    /**
     * Fires an "on scroll state changed" event to the registered
     * {@link android.widget.AbsListView.OnScrollListener}, if any. The state change
     * is fired only if the specified state is different from the previously known state.
     *
     * @param newState The new scroll state.
     */
    virtual CARAPI_(void) ReportScrollStateChange(
        /* [in] */ Int32 newState);

    /**
     * Override this for better control over position scrolling.
     */
    virtual CARAPI_(AutoPtr<AbsListView::AbsPositionScroller>) CreatePositionScroller();

    virtual CARAPI_(void) SmoothScrollBy(
        /* [in] */ Int32 distance,
        /* [in] */ Int32 duration,
        /* [in] */ Boolean linear);

    /**
     * Allows RemoteViews to scroll relatively to a position.
     */
    virtual CARAPI SmoothScrollByOffset(
        /* [in] */ Int32 position);

    virtual CARAPI_(Boolean) TrackMotionScroll(
        /* [in] */ Int32 deltaY,
        /* [in] */ Int32 incrementalDeltaY);

    /**
     * Returns the number of header views in the list. Header views are special views
     * at the top of the list that should not be recycled during a layout.
     *
     * @return The number of header views, 0 in the default implementation.
     */
    virtual CARAPI_(Int32) GetHeaderViewsCount();

    /**
     * Returns the number of footer views in the list. Footer views are special views
     * at the bottom of the list that should not be recycled during a layout.
     *
     * @return The number of footer views, 0 in the default implementation.
     */
    virtual CARAPI_(Int32) GetFooterViewsCount();

    /**
     * Fills the gap left open by a touch-scroll. During a touch scroll, children that
     * remain on screen are shifted and the other ones are discarded. The role of this
     * method is to fill the gap thus created by performing a partial layout in the
     * empty space.
     *
     * @param down true if the scroll is going down, false if it is going up
     */
    virtual CARAPI FillGap(
        /* [in] */ Boolean down) = 0;

    /**
     * @return A position to select. First we try mSelectedPosition. If that has been clobbered by
     * entering touch mode, we then try mResurrectToPosition. Values are pinned to the range
     * of items available in the adapter
     */
    virtual CARAPI_(Int32) ReconcileSelectedPosition();

    /**
     * Find the row closest to y. This row will be used as the motion row when scrolling
     *
     * @param y Where the user touched
     * @return The position of the first (or only) item in the row containing y
     */
    virtual CARAPI_(Int32) FindMotionRow(
        /* [in] */ Int32 y) = 0;

    /**
     * Find the row closest to y. This row will be used as the motion row when scrolling.
     *
     * @param y Where the user touched
     * @return The position of the first (or only) item in the row closest to y
     */
    virtual CARAPI_(Int32) FindClosestMotionRow(
        /* [in] */ Int32 y);

    /**
     * If there is a selection returns false.
     * Otherwise resurrects the selection and returns true if resurrected.
     */
    virtual CARAPI_(Boolean) ResurrectSelectionIfNeeded();

    /**
     * Makes the item at the supplied position selected.
     *
     * @param position the position of the new selection
     */
    virtual CARAPI SetSelectionInt(
        /* [in] */ Int32 position) = 0;

    /**
     * Attempt to bring the selection back if the user is switching from touch
     * to trackball mode
     * @return Whether selection was set to something.
     */
    virtual CARAPI_(Boolean) ResurrectSelection();

    virtual CARAPI_(void) ConfirmCheckedPositionsById();

    //@Override
    virtual CARAPI_(void) HandleDataChanged();

    //@Override
    virtual CARAPI_(void) OnDisplayHint(
        /* [in] */ Int32 hint);

    //@Override
    virtual CARAPI IsInFilterMode(
        /* [out] */ Boolean* result);

    /**
     * Sends a key to the text filter window
     *
     * @param keyCode The keycode for the event
     * @param event The actual key event
     *
     * @return True if the text filter handled the event, FALSE otherwise.
     */
    virtual CARAPI_(Boolean) SendToTextFilter(
        /* [in] */ Int32 keyCode,
        /* [in] */ Int32 count,
        /* [in] */ IKeyEvent* event);

    //@Override
    CARAPI GenerateDefaultLayoutParams(
        /* [out] */ IViewGroupLayoutParams** result);

    CARAPI_(AutoPtr<IViewGroupLayoutParams>) GenerateLayoutParams(
        /* [in] */ IViewGroupLayoutParams* p);

    //@Override
    virtual CARAPI_(Boolean) CheckLayoutParams(
        /* [in] */ IViewGroupLayoutParams* p);

    /**
     * Hints the RemoteViewsAdapter, if it exists, about which views are currently
     * being displayed by the AbsListView.
     */
    virtual CARAPI_(void) SetVisibleRangeHint(
        /* [in] */ Int32 start,
        /* [in] */ Int32 end);

    /**
     * Returns the height of the view for the specified position.
     *
     * @param position the item position
     * @return view height in pixels
     */
    virtual CARAPI_(Int32) GetHeightForPosition(
        /* [in] */ Int32 position);

private:
    CARAPI InitAbsListView();

    /**
     * Perform a quick, in-place update of the checked or activated state
     * on all visible item views. This should only be called when a valid
     * choice mode is active.
     */
    CARAPI_(void) UpdateOnScreenCheckedViews();

    CARAPI_(Boolean) ContentFits();

    CARAPI_(void) SetFastScrollerEnabledUiThread(
        /* [in] */ Boolean enabled);

    CARAPI_(void) SetFastScrollerAlwaysVisibleUiThread(
        /* [in] */ Boolean alwaysShow);

    /**
     * @return whether the current thread is the one that created the view
     */
    CARAPI_(Boolean) IsOwnerThread();

    CARAPI UseDefaultSelector();

    CARAPI_(Boolean) AcceptFilter();

    CARAPI_(Boolean) CanScrollUp();

    CARAPI_(Boolean) CanScrollDown();

    CARAPI_(void) SetItemViewLayoutParams(
        /* [in] */ IView* child,
        /* [in] */ Int32 position);

    CARAPI_(void) DrawSelector(
        /* [in] */ ICanvas* canvas);

    CARAPI_(Boolean) StartScrollIfNeeded(
        /* [in] */ Int32 x,
        /* [in] */ Int32 y,
        /* [in] */ IMotionEvent* vtev);

    CARAPI_(void) ScrollIfNeeded(
        /* [in] */ Int32 x,
        /* [in] */ Int32 y,
        /* [in] */ IMotionEvent* vtev);

    CARAPI_(void) OnTouchDown(
        /* [in] */ IMotionEvent* ev);

    CARAPI_(void) OnTouchMove(
        /* [in] */ IMotionEvent* ev,
        /* [in] */ IMotionEvent* vtev);

    CARAPI_(void) OnTouchUp(
        /* [in] */ IMotionEvent* ev);

    CARAPI_(void) OnTouchCancel();

    CARAPI_(void) InitOrResetVelocityTracker();

    CARAPI_(void) InitVelocityTrackerIfNotExists();

    CARAPI_(void) RecycleVelocityTracker();

    CARAPI_(void) OnSecondaryPointerUp(
        /* [in] */ IMotionEvent* ev);

    CARAPI_(void) CreateScrollingCache();

    CARAPI_(void) ClearScrollingCache();

    /**
     * Removes the filter window
     */
    CARAPI_(void) DismissPopup();

    /**
     * Shows the filter window
     */
    CARAPI_(void) ShowPopup();

    CARAPI_(void) PositionPopup();

    /**
     * Creates the window for the text filter and populates it with an EditText field;
     *
     * @param animateEntrance true if the window should appear with an animation
     */
    CARAPI_(void) CreateTextFilter(
        /* [in] */ Boolean animateEntrance);

    CARAPI_(AutoPtr<IEditText>) GetTextFilterInput();

    CARAPI_(void) FinishGlows();

    static AutoPtr<IStrictMode> GetStrictMode();

protected:
    /**
     * Indicates that we are not in the middle of a touch gesture
     */
    static const Int32 TOUCH_MODE_REST = -1;

    /**
     * Indicates we just received the touch event and we are waiting to see if the it is a tap or a
     * scroll gesture.
     */
    static const Int32 TOUCH_MODE_DOWN = 0;

    /**
     * Indicates the touch has been recognized as a tap and we are now waiting to see if the touch
     * is a longpress
     */
    static const Int32 TOUCH_MODE_TAP = 1;

    /**
     * Indicates we have waited for everything we can wait for, but the user's finger is still down
     */
    static const Int32 TOUCH_MODE_DONE_WAITING = 2;

    /**
     * Indicates the touch gesture is a scroll
     */
    static const Int32 TOUCH_MODE_SCROLL = 3;

    /**
     * Indicates the view is in the process of being flung
     */
    static const Int32 TOUCH_MODE_FLING = 4;

    /**
     * Indicates the touch gesture is an overscroll - a scroll beyond the beginning or end.
     */
    static const Int32 TOUCH_MODE_OVERSCROLL = 5;

    /**
     * Indicates the view is being flung outside of normal content bounds
     * and will spring back.
     */
    static const Int32 TOUCH_MODE_OVERFLING = 6;

    /**
     * Regular layout - usually an unsolicited layout from the view system
     */
    static const Int32 LAYOUT_NORMAL = 0;

    /**
     * Show the first item
     */
    static const Int32 LAYOUT_FORCE_TOP = 1;

    /**
     * Force the selected item to be on somewhere on the screen
     */
    static const Int32 LAYOUT_SET_SELECTION = 2;

    /**
     * Show the last item
     */
    static const Int32 LAYOUT_FORCE_BOTTOM = 3;

    /**
     * Make a mSelectedItem appear in a specific location and build the rest of
     * the views from there. The top is specified by mSpecificTop.
     */
    static const Int32 LAYOUT_SPECIFIC = 4;

    /**
     * Layout to sync as a result of a data change. Restore mSyncPosition to have its top
     * at mSpecificTop
     */
    static const Int32 LAYOUT_SYNC = 5;

    /**
     * Layout as a result of using the navigation keys
     */
    static const Int32 LAYOUT_MOVE_SELECTION = 6;

    /**
     * Content height divided by this is the overscroll limit.
     */
    static const Int32 OVERSCROLL_LIMIT_DIVISOR = 3;

    /**
     * How many positions in either direction we will search to try to
     * find a checked item with a stable ID that moved position across
     * a data set change. If the item isn't found it will be unselected.
     */
    static const Int32 CHECK_POSITION_SEARCH_DISTANCE = 20;

    /**
     * Used for smooth scrolling at a consistent rate
     */
    static AutoPtr<IInterpolator> sLinearInterpolator;

private:
    /**
     * The thread that created this view.
     */
    AutoPtr<IThread> mOwnerThread;

    /**
     * Used to request a layout when we changed touch mode
     */
    static const Int32 TOUCH_MODE_UNKNOWN;
    static const Int32 TOUCH_MODE_ON;
    static const Int32 TOUCH_MODE_OFF;
    static const Boolean PROFILE_SCROLLING;
    static const Boolean PROFILE_FLINGING;
    /**
     * Sentinel value for no current active pointer.
     * Used by {@link #mActivePointerId}.
     */
    static const Int32 INVALID_POINTER;

protected:
    /**
     * Controls if/how the user may choose/check items in the list
     */
    Int32 mChoiceMode;

    /**
     * Controls CHOICE_MODE_MULTIPLE_MODAL. null when inactive.
     */
    AutoPtr<IActionMode> mChoiceActionMode;

    /**
     * Wrapper for the multiple choice mode callback; AbsListView needs to perform
     * a few extra actions around what application code does.
     */
    AutoPtr<MultiChoiceModeWrapper> mMultiChoiceModeCallback;

    /**
     * Running count of how many items are currently checked
     */
    Int32 mCheckedItemCount;

    /**
     * Running state of which positions are currently checked
     */
    AutoPtr<ISparseBooleanArray> mCheckStates;

    /**
     * Running state of which IDs are currently checked.
     * If there is a value for a given key, the checked state for that ID is true
     * and the value holds the last known position in the adapter for that id.
     */
    AutoPtr<IInt64SparseArray> mCheckedIdStates;

    /**
     * Controls how the next layout will happen
     */
    Int32 mLayoutMode;

    /**
     * Should be used by subclasses to listen to changes in the dataset
     */
    AutoPtr<AdapterDataSetObserver> mDataSetObserver;

    /**
     * The adapter containing the data to be displayed by this view
     */
    AutoPtr<IListAdapter> mAdapter;

    /**
     * If mAdapter != null, whenever this is true the adapter has stable IDs.
     */
    Boolean mAdapterHasStableIds;

    /**
     * Indicates whether the list selector should be drawn on top of the children or behind
     */
    Boolean mDrawSelectorOnTop;

    /**
     * The drawable used to draw the selector
     */
    AutoPtr<IDrawable> mSelector;

    /**
     * The current position of the selector in the list.
     */
    Int32 mSelectorPosition;

    /**
     * Defines the selector's location and dimension at drawing time
     */
    AutoPtr<IRect> mSelectorRect;

    /**
     * The data set used to store unused views that should be reused during the next layout
     * to avoid creating new ones
     */
    AutoPtr<RecycleBin> mRecycler;

    /**
     * The selection's left padding
     */
    Int32 mSelectionLeftPadding;

    /**
     * The selection's top padding
     */
    Int32 mSelectionTopPadding;

    /**
     * The selection's right padding
     */
    Int32 mSelectionRightPadding;

    /**
     * The selection's bottom padding
     */
    Int32 mSelectionBottomPadding;

    /**
     * This view's padding
     */
    AutoPtr<IRect> mListPadding;

    /**
     * Subclasses must retain their measure spec from onMeasure() into this member
     */
    Int32 mWidthMeasureSpec;

    /**
     * The top scroll indicator
     */
    AutoPtr<IView> mScrollUp;

    /**
     * The down scroll indicator
     */
    AutoPtr<IView> mScrollDown;

    /**
     * When the view is scrolling, this flag is set to TRUE to indicate subclasses that
     * the drawing cache was enabled on the children
     */
    Boolean mCachingStarted;

    Boolean mCachingActive;

    /**
     * The position of the view that received the down motion event
     */
    Int32 mMotionPosition;

    /**
     * The offset to the top of the mMotionPosition view when the down motion event was received
     */
    Int32 mMotionViewOriginalTop;

    /**
     * The desired offset to the top of the mMotionPosition view after a scroll
     */
    Int32 mMotionViewNewTop;

    /**
     * The X value associated with the the down motion event
     */
    Int32 mMotionX;

    /**
     * The Y value associated with the the down motion event
     */
    Int32 mMotionY;

    /**
     * One of TOUCH_MODE_REST, TOUCH_MODE_DOWN, TOUCH_MODE_TAP, TOUCH_MODE_SCROLL, or
     * TOUCH_MODE_DONE_WAITING
     */
    Int32 mTouchMode;

    /**
     * Y value from on the previous motion event (if any)
     */
    Int32 mLastY;

    /**
     * How far the finger moved before we started scrolling
     */
    Int32 mMotionCorrection;

    /**
     * The offset in pixels form the top of the AdapterView to the top
     * of the currently selected view. Used to save and restore state.
     */
    Int32 mSelectedTop;

    /**
     * Indicates whether the list is stacked from the bottom edge or
     * the top edge.
     */
    Boolean mStackFromBottom;

    /**
     * When set to TRUE, the list automatically discards the children's
     * bitmap cache after scrolling.
     */
    Boolean mScrollingCacheEnabled;

    /**
     * Whether or not to enable the fast scroll feature on this list
     */
    Boolean mFastScrollEnabled;

    /**
     * Whether or not to always show the fast scroll feature on this list
     */
    Boolean mFastScrollAlwaysVisible;

    /**
     * Keeps track of our accessory window
     */
    AutoPtr<IPopupWindow> mPopup;

    /**
     * Used with type filter window
     */
    AutoPtr<IEditText> mTextFilter;

    /**
     * The position to resurrect the selected position to.
     */
    Int32 mResurrectToPosition;

    /**
     * Maximum distance to record overscroll
     */
    Int32 mOverscrollMax;

    AutoPtr<ArrayOf<Boolean> > mIsScrap;

    /**
     * Maximum distance to overscroll by during edge effects
     */
    Int32 mOverscrollDistance;

    /**
     * Maximum distance to overfling during edge effects
     */
    Int32 mOverflingDistance;

    /**
     * Handles scrolling between positions within the list.
     */
    AutoPtr<AbsPositionScroller> mPositionScroller;

    AutoPtr<IRunnable> mPositionScrollAfterLayout;

private:
    static AutoPtr<IStrictMode> sStrictMode;

    /**
     * The remote adapter containing the data to be displayed by this view to be set
     */
    AutoPtr<IRemoteViewsAdapter> mRemoteAdapter;

    /**
     * This flag indicates the a full notify is required when the RemoteViewsAdapter connects
     */
    Boolean mDeferNotifyDataSetChanged;


    /**
     * Optional callback to notify client when scroll position has changed
     */
    AutoPtr<IAbsListViewOnScrollListener> mOnScrollListener;

    /**
     * Determines speed during touch scrolling
     */
    AutoPtr<VelocityTracker> mVelocityTracker;

    /**
     * Handles one frame of a fling
     */
    AutoPtr<FlingRunnable> mFlingRunnable;

    /**
     * Indicates whether to use pixels-based or position-based scrollbar
     * properties.
     */
    Boolean mSmoothScrollbarEnabled;

    /**
     * Indicates that this view supports filtering
     */
    Boolean mTextFilterEnabled;

    /**
     * Indicates that this view is currently displaying a filtered view of the data
     */
    Boolean mFiltered;

    /**
     * Rectangle used for hit testing children
     */
    AutoPtr<IRect> mTouchFrame;

    AutoPtr<IContextMenuInfo> mContextMenuInfo;

    Int32 mLastTouchMode;
    Boolean mScrollProfilingStarted;
    Boolean mFlingProfilingStarted;

    /**
     * The StrictMode "critical time span" objects to catch animation
     * stutters.  Non-null when a time-sensitive animation is
     * in-flight.  Must call finish() on them when done animating.
     * These are no-ops on user builds.
     */
    AutoPtr<IStrictModeSpan> mScrollStrictSpan;

    AutoPtr<IStrictModeSpan> mFlingStrictSpan;

    /**
     * The last CheckForLongPress runnable we posted, if any
     */
    AutoPtr<CheckForLongPress> mPendingCheckForLongPress;

    /**
     * The last CheckForTap runnable we posted, if any
     */
    AutoPtr<CheckForTap> mPendingCheckForTap;

    /**
     * The last CheckForKeyLongPress runnable we posted, if any
     */
    AutoPtr<CheckForKeyLongPress> mPendingCheckForKeyLongPress;

    /**
     * Acts upon click
     */
    AutoPtr<PerformClick> mPerformClick;

    /**
     * Delayed action for touch mode.
     */
    AutoPtr<IRunnable> mTouchModeReset;

    /**
     * This view is in transcript mode -- it shows the bottom of the list when the data
     * changes
     */
    Int32 mTranscriptMode;

    /**
     * Indicates that this list is always drawn on top of a solid, single-color, opaque
     * background
     */
    Int32 mCacheColorHint;

    /**
     * The select child's view (from the adapter's getView) is enabled.
     */
    Boolean mIsChildViewEnabled;

    /**
     * The last scroll state reported to clients through {@link OnScrollListener}.
     */
    Int32 mLastScrollState;

    /**
     * Helper object that renders and controls the fast scroll thumb.
     */
    AutoPtr<FastScroller> mFastScroll;

    /**
     * Temporary holder for fast scroller style until a FastScroller object
     * is created.
     */
    Int32 mFastScrollStyle;

    Boolean mGlobalLayoutListenerAddedFilter;

    Int32 mTouchSlop;
    Float mDensityScale;

    AutoPtr<IInputConnection> mDefInputConnection;
    AutoPtr<IInputConnection> mPublicInputConnection; // InputConnectionWrapper

    AutoPtr<IRunnable> mClearScrollingCache;

    Int32 mMinimumVelocity;
    Int32 mMaximumVelocity;

    Int32 mDecacheThreshold;

    Float mVelocityScale;

    AutoPtr< ArrayOf<Int32> > mScrollOffset;
    AutoPtr< ArrayOf<Int32> > mScrollConsumed;

    // Used for offsetting MotionEvents that we feed to the VelocityTracker.
    // In the future it would be nice to be able to give this to the VelocityTracker
    // directly, or alternatively put a VT into absolute-positioning mode that only
    // reads the raw screen-coordinate x/y values.
    Int32 mNestedYOffset;

    // True when the popup should be hidden because of a call to
    // dispatchDisplayHint()
    Boolean mPopupHidden;

    /**
     * ID of the active pointer. This is used to retain consistency during
     * drags/flings if multiple pointers are used.
     */
    Int32 mActivePointerId;

    // These two EdgeGlows are always set and used together.
    // Checking one for NULL is as good as checking both.

    /**
     * Tracks the state of the top edge glow.
     */
    AutoPtr<IEdgeEffect> mEdgeGlowTop;

    /**
     * Tracks the state of the bottom edge glow.
     */
    AutoPtr<IEdgeEffect> mEdgeGlowBottom;

    /**
     * An estimate of how many pixels are between the top of the list and
     * the top of the first position in the adapter, based on the last time
     * we saw it. Used to hint where to draw edge glows.
     */
    Int32 mFirstPositionDistanceGuess;

    /**
     * An estimate of how many pixels are between the bottom of the list and
     * the bottom of the last position in the adapter, based on the last time
     * we saw it. Used to hint where to draw edge glows.
     */
    Int32 mLastPositionDistanceGuess;

    /**
     * Used for determining when to cancel out of overscroll.
     */
    Int32 mDirection;

    /**
     * Tracked on measurement in transcript mode. Makes sure that we can still pin to
     * the bottom correctly on resizes.
     */
    Boolean mForceTranscriptScroll;

    Int32 mGlowPaddingLeft;
    Int32 mGlowPaddingRight;

    /**
     * Used for interacting with list items from an accessibility service.
     */
    AutoPtr<ListItemAccessibilityDelegate> mAccessibilityDelegate;

    Int32 mLastAccessibilityScrollEventFromIndex;
    Int32 mLastAccessibilityScrollEventToIndex;

    /**
     * Track the item count from the last time we handled a data change.
     */
    Int32 mLastHandledItemCount;

    /**
     * The saved state that we will be restoring from when we next sync.
     * Kept here so that if we happen to be asked to save our state before
     * the sync happens, we can return this existing data rather than losing
     * it.
     */
    AutoPtr<CAbsListViewSavedState> mPendingSync;

    /**
     * Whether the view is in the process of detaching from its window.
     */
    Boolean mIsDetaching;

};

}// namespace Widget
}// namespace Droid
}// namespace Elastos
#endif //__ELASTOS_DROID_WIDGET_ABSLISTVIEW_H__
