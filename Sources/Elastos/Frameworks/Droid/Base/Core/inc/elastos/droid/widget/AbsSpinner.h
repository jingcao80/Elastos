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

#ifndef __ELASTOS_DROID_WIDGET_ABSSPANNER_H__
#define __ELASTOS_DROID_WIDGET_ABSSPANNER_H__

#include "elastos/droid/widget/AdapterView.h"
#include "elastos/droid/graphics/CRect.h"
#include <elastos/utility/etl/HashMap.h>
#include "elastos/droid/R.h"
#include "elastos/droid/view/View.h"

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Graphics::CRect;
using Elastos::Droid::R;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::IViewGroup;
using Elastos::Droid::View::EIID_IViewGroup;
using Elastos::Droid::View::View;
using Elastos::Droid::Utility::IAttributeSet;
using Elastos::Core::CString;
using Elastos::Utility::Etl::HashMap;

namespace Elastos {
namespace Droid {
namespace Widget {

/**
 * An abstract base class for spinner widgets. SDK users will probably not
 * need to use this class.
 *
 * @attr ref android.R.styleable#AbsSpinner_entries
 */
class AbsSpinner
    : public AdapterView
    , public IAbsSpinner
{
public:
    class AbsSpinnerSavedState
        : public View::BaseSavedState
        , public IAbsSpinnerSavedState
    {
    public:
        CAR_INTERFACE_DECL()

        AbsSpinnerSavedState();

        ~AbsSpinnerSavedState();

        CARAPI ReadFromParcel(
            /* [in] */ IParcel* source);

        // @Override
        CARAPI WriteToParcel(
            /* [in] */ IParcel* out);

        // @Override
        CARAPI ToString(
            /* [out] */ String* str);

    public:
        Int64 mSelectedId;
        Int32 mPosition;
    };

    class RecycleBin : public Object
    {
    public:
        RecycleBin(
            /* [in] */ AbsSpinner* mHost);

        CARAPI_(void) Put(
            /* [in] */ Int32 position,
            /* [in] */ IView* v);

        CARAPI_(AutoPtr<IView>) Get(
            /* [in] */ Int32 position);

        CARAPI_(void) Clear();

    private:
        HashMap<Int32, AutoPtr<IView> > mScrapHeap;
        AbsSpinner* mHost;
    };

public:
    CAR_INTERFACE_DECL()

    AbsSpinner();

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

    virtual ~AbsSpinner();

    /**
     * The Adapter is used to provide the data which backs this Spinner.
     * It also provides methods to transform spinner items based on their position
     * relative to the selected item.
     * @param adapter The SpinnerAdapter to use for this Spinner
     */
    //@Override
    CARAPI SetAdapter(
        /* [in] */ IAdapter* adapter);

    /**
     * Clear out all children from the list
     */
    virtual CARAPI_(void) ResetList();

    virtual CARAPI_(Int32) GetChildHeight(
        /* [in] */ IView* child);

    virtual CARAPI_(Int32) GetChildWidth(
        /* [in] */ IView* child);

    virtual CARAPI_(void) RecycleAllViews();

    /**
     * Jump directly to a specific item in the adapter data.
     */
    virtual CARAPI SetSelection(
        /* [in] */ Int32 position,
        /* [in] */ Boolean animate);

    //@Override
    virtual CARAPI SetSelection(
        /* [in] */ Int32 position);

    /**
     * Makes the item at the supplied position selected.
     *
     * @param position Position to select
     * @param animate Should the transition be animated
     *
     */
    CARAPI_(void) SetSelectionInt(
        /* [in] */ Int32 position,
        /* [in] */ Boolean animate);

    using AdapterView::Layout;

    virtual CARAPI_(void) Layout(
        /* [in] */ Int32 delta,
        /* [in] */ Boolean animate) = 0;

    //@Override
    CARAPI GetSelectedView(
        /* [out] */ IView** view);

    /**
     * Override to prevent spamming ourselves with layout requests
     * as we place views
     *
     * @see android.view.View#requestLayout()
     */
    //@Override
    CARAPI RequestLayout();

    //@Override
    CARAPI GetAdapter(
        /* [out] */ IAdapter** adapter);

    //@Override
    CARAPI GetCount(
        /* [out] */ Int32* count);

    /**
     * Maps a point to a position in the list.
     *
     * @param x X in local coordinate
     * @param y Y in local coordinate
     * @return The position of the item which contains the specified point, or
     *         {@link #INVALID_POSITION} if the point does not intersect an item.
     */
    virtual CARAPI PointToPosition(
        /* [in] */ Int32 x,
        /* [in] */ Int32 y,
        /* [out] */ Int32* position);

    //@Override
    CARAPI_(AutoPtr<IParcelable>) OnSaveInstanceState();

    //@Override
    CARAPI OnRestoreInstanceState(
        /* [in] */ IParcelable* state);

    virtual CARAPI OnInitializeAccessibilityEvent(
        /* [in] */ IAccessibilityEvent* event);

    virtual CARAPI OnInitializeAccessibilityNodeInfo(
        /* [in] */ IAccessibilityNodeInfo* info);

protected:
    /**
     * @see android.view.View#measure(int, int)
     *
     * Figure out the dimensions of this Spinner. The width comes from
     * the widthMeasureSpec as Spinnners can't have their width set to
     * UNSPECIFIED. The height is based on the height of the selected item
     * plus padding.
     */
    //@Override
    CARAPI OnMeasure(
        /* [in] */ Int32 widthMeasureSpec,
        /* [in] */ Int32 heightMeasureSpec);

    //@Override
    CARAPI GenerateDefaultLayoutParams(
        /* [out] */ IViewGroupLayoutParams** params);

private:
    /**
     * Common code for different constructor flavors
     */
    CARAPI_(void) InitAbsSpinner();

    CARAPI InitFromAttributes(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs,
        /* [in] */ Int32 defStyle,
        /* [in] */ Int32 defStyleRes);

public:
    AutoPtr<ISpinnerAdapter> mAdapter;

    Int32 mHeightMeasureSpec;
    Int32 mWidthMeasureSpec;

    Int32 mSelectionLeftPadding;
    Int32 mSelectionTopPadding;
    Int32 mSelectionRightPadding;
    Int32 mSelectionBottomPadding;
    AutoPtr<IRect> mSpinnerPadding;

    AutoPtr<RecycleBin> mRecycler;

private:
    AutoPtr<IDataSetObserver> mDataSetObserver;

    /** Temporary frame to hold a child View's frame rectangle */
    AutoPtr<IRect> mTouchFrame;
};

}// namespace Widget
}// namespace Droid
}// namespace Elastos

#endif // __ELASTOS_DROID_WIDGET_ABSSPANNER_H__
