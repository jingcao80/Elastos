
#ifndef __ELASTOS_DROID_WIDGET_ADAPTERVIEW_H__
#define __ELASTOS_DROID_WIDGET_ADAPTERVIEW_H__

#include "elastos/droid/ext/frameworkext.h"
#include "Elastos.Droid.Database.h"
#include "elastos/droid/view/ViewGroup.h"

using Elastos::Droid::Database::IDataSetObserver;
using Elastos::Droid::View::IContextMenuInfo;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::IViewGroupLayoutParams;
using Elastos::Droid::View::IViewOnClickListener;
using Elastos::Droid::View::ViewGroup;
using Elastos::Droid::Widget::IAdapterContextMenuInfo;
using Elastos::Core::IRunnable;

namespace Elastos {
namespace Droid {
namespace Widget {

class AdapterView
    : public ViewGroup
    , public IAdapterView
{
public:
    class AdapterContextMenuInfo
        : public Object
        , public IContextMenuInfo
        , public IAdapterContextMenuInfo
    {
    public:
        CAR_INTERFACE_DECL();

        AdapterContextMenuInfo(
            /* [in] */ IView* targetView,
            /* [in] */ Int32 position,
            /* [in] */ Int64 id);

        CARAPI GetTargetView(
            /* [out] */ IView** targetView);

        CARAPI SetTargetView(
            /* [in] */ IView* targetView);

        CARAPI GetPosition(
            /* [out] */ Int32* position);

        CARAPI SetPosition(
            /* [in] */ Int32 position);

        CARAPI GetId(
            /* [out] */ Int64* id);

        CARAPI SetId(
            /* [in] */ Int64 id);

    public:
        AutoPtr<IView> mTargetView;
        Int32 mPosition;
        Int64 mId;
    };

    class AdapterDataSetObserver
        : public Object
        , public IDataSetObserver
    {
    public:
        CAR_INTERFACE_DECL()

        AdapterDataSetObserver(
            /* [in] */ AdapterView* host);

        CARAPI OnChanged();

        CARAPI OnInvalidated();

        CARAPI ClearSavedState();

    private:
        AutoPtr<IParcelable> mInstanceState;
        AdapterView* mHost;
    };

private:
    class SelectionNotifier
        : public Object
        , public IRunnable
    {
    public:
        CAR_INTERFACE_DECL()

        SelectionNotifier(
            /* [in] */ AdapterView* host);

        CARAPI Run();

    private:
        AdapterView* mHost;
    };

public:
    CAR_INTERFACE_DECL()

    AdapterView();

    CARAPI constructor(
        /* [in] */ IContext* context);

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs);

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs,
        /* [in] */ Int32 defStyle);

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs,
        /* [in] */ Int32 defStyle,
        /* [in] */ Int32 defStyleRes);

    CARAPI SetOnItemClickListener(
        /* [in] */ IAdapterViewOnItemClickListener* listener);

    CARAPI GetOnItemClickListener(
        /* [out] */ IAdapterViewOnItemClickListener** result);

    CARAPI PerformItemClick(
        /* [in] */ IView* view,
        /* [in] */ Int32 position,
        /* [in] */ Int64 id,
        /* [out] */ Boolean* result);

    virtual CARAPI SetOnItemLongClickListener(
        /* [in] */ IAdapterViewOnItemLongClickListener* listener);

    CARAPI GetOnItemLongClickListener(
        /* [out] */ IAdapterViewOnItemLongClickListener** result);

    virtual CARAPI SetOnItemSelectedListener(
        /* [in] */ IAdapterViewOnItemSelectedListener* listener);

    CARAPI GetOnItemSelectedListener(
        /* [out] */ IAdapterViewOnItemSelectedListener** result);

    //@Override
    CARAPI AddView(
        /* [in] */ IView* child);

    //@Override
    CARAPI AddView(
        /* [in] */ IView* child,
        /* [in] */ Int32 index);

    //@Override
    CARAPI AddView(
        /* [in] */ IView* child,
        /* [in] */ Int32 width,
        /* [in] */ Int32 height);

    //@Override
    CARAPI AddView(
        /* [in] */ IView* child,
        /* [in] */ IViewGroupLayoutParams* params);

    //@Override
    CARAPI AddView(
        /* [in] */ IView* child,
        /* [in] */ Int32 index,
        /* [in] */ IViewGroupLayoutParams* params);

    //@Override
    CARAPI RemoveView(
        /* [in] */ IView* child);

    //@Override
    CARAPI RemoveViewAt(
        /* [in] */ Int32 index);

    //@Override
    CARAPI RemoveAllViews();

    virtual CARAPI GetSelectedItemPosition(
        /* [out] */ Int32* result);

    virtual CARAPI GetSelectedItemId(
        /* [out] */ Int32* result);

    virtual CARAPI GetSelectedItem(
        /* [out] */ IInterface** result);

    virtual CARAPI GetCount(
        /* [out] */ Int32* result);

    virtual CARAPI GetPositionForView(
        /* [in] */ IView* view,
        /* [out] */ Int32* result);

    virtual CARAPI GetFirstVisiblePosition(
        /* [out] */ Int32* result);

    virtual CARAPI GetLastVisiblePosition(
        /* [out] */ Int32* result);

    virtual CARAPI SetEmptyView(
        /* [in] */ IView* emptyView);

    virtual CARAPI GetEmptyView(
        /* [out] */ IView** result);

    virtual CARAPI IsInFilterMode(
        /* [out] */ Boolean* result);

    //@Override
    CARAPI SetFocusable(
        /* [in] */ Boolean focusable);

    //@Override
    CARAPI SetFocusableInTouchMode(
        /* [in] */ Boolean focusable);

    virtual CARAPI CheckFocus();

    virtual CARAPI GetItemAtPosition(
        /* [in] */ Int32 position,
        /* [out] */ IInterface** result);

    virtual CARAPI GetItemIdAtPosition(
        /* [in] */ Int32 position,
        /* [out] */ Int64* result);

    virtual CARAPI GetAdapter(
        /* [out] */ IAdapter** adapter);

    virtual CARAPI SetAdapter(
        /* [in] */ IAdapter* adapter);

    virtual CARAPI GetSelectedItemId(
        /* [out] */ Int64* itemId);

    virtual CARAPI GetSelectedView(
        /* [out] */ IView** view);

    virtual CARAPI SetSelection(
        /* [in] */ Int32 position);

    //@Override
    CARAPI SetOnClickListener(
        /* [in] */ IViewOnClickListener* l);

    //@Override
    CARAPI DispatchPopulateAccessibilityEvent(
        /* [in] */ IAccessibilityEvent* event,
        /* [out] */ Boolean* result);

    //@Override
    CARAPI OnRequestSendAccessibilityEvent(
        /* [in] */ IView* child,
        /* [in] */ IAccessibilityEvent* event,
        /* [out] */ Boolean* result);

    virtual CARAPI OnInitializeAccessibilityNodeInfo(
        /* [in] */ IAccessibilityNodeInfo* info);

    virtual CARAPI OnInitializeAccessibilityEvent(
        /* [in] */ IAccessibilityEvent* event);

    virtual CARAPI_(void) HandleDataChanged();

    virtual CARAPI_(void) SelectionChanged();

    virtual CARAPI_(void) CheckSelectionChanged();

    virtual CARAPI_(Int32) FindSyncPosition();

    virtual CARAPI_(Int32) LookForSelectablePosition(
        /* [in] */ Int32 position,
        /* [in] */ Boolean lookDown);

    virtual CARAPI_(void) SetSelectedPositionInt(
        /* [in] */ Int32 position);

    virtual CARAPI_(void) SetNextSelectedPositionInt(
        /* [in] */ Int32 position);

    virtual CARAPI_(void) RememberSyncState();

protected:
    //@Override
    CARAPI OnLayout(
        /* [in] */ Boolean changed,
        /* [in] */ Int32 l,
        /* [in] */ Int32 t,
        /* [in] */ Int32 r,
        /* [in] */ Int32 b);

    //@Override
    CARAPI DispatchSaveInstanceState(
        /* [in] */ ISparseArray* container);

    //@Override
    CARAPI DispatchRestoreInstanceState(
        /* [in] */ ISparseArray* container);

    //@Override
    CARAPI OnDetachedFromWindow();

    //@Override
    CARAPI_(Boolean) CanAnimate();

private:
    CARAPI_(void) UpdateEmptyStatus(
        /* [in] */ Boolean empty);

    CARAPI_(void) FireOnSelected();

    CARAPI_(void) PerformAccessibilityActionsOnSelected();

    CARAPI_(Boolean) IsScrollableForAccessibility();

public:
    static const Int32 SYNC_MAX_DURATION_MILLIS = 100;
    static const Int32 SYNC_SELECTED_POSITION = 0;
    static const Int32 SYNC_FIRST_POSITION = 1;
    Int32 mFirstPosition;
    Int32 mSpecificTop;
    Int32 mSyncPosition;
    Int64 mSyncRowId;
    Int64 mSyncHeight;
    Boolean mNeedSync;
    Int32 mSyncMode;
    Boolean mInLayout;
    AutoPtr<IAdapterViewOnItemSelectedListener> mOnItemSelectedListener;
    AutoPtr<IAdapterViewOnItemClickListener> mOnItemClickListener;
    AutoPtr<IAdapterViewOnItemLongClickListener> mOnItemLongClickListener;
    Boolean mDataChanged;
    Int32 mNextSelectedPosition;
    Int64 mNextSelectedRowId;
    Int32 mSelectedPosition;
    Int64 mSelectedRowId;
    Int32 mItemCount;
    Int32 mOldItemCount;
    Int32 mOldSelectedPosition;
    Int64 mOldSelectedRowId;
    Boolean mBlockLayoutRequests;

private:
    Int32 mLayoutHeight;
    AutoPtr<IView> mEmptyView;
    Boolean mDesiredFocusableState;
    Boolean mDesiredFocusableInTouchModeState;
    AutoPtr<SelectionNotifier> mSelectionNotifier;
};

}// namespace Widget
}// namespace Droid
}// namespace Elastos

#endif  //__ELASTOS_DROID_WIDGET_ADAPTERVIEW_H__

