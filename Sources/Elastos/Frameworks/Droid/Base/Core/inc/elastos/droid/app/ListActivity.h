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

#ifndef __ELASTOS_DROID_APP_LISTACTIVITY_H__
#define __ELASTOS_DROID_APP_LISTACTIVITY_H__

#include "Elastos.Droid.Widget.h"
#include "elastos/droid/app/Activity.h"
#include <elastos/core/Runnable.h>

using Elastos::Core::Runnable;
using Elastos::Droid::Widget::IAdapterViewOnItemClickListener;
using Elastos::Droid::Widget::IListView;
using Elastos::Droid::Widget::IAdapterView;
using Elastos::Droid::Widget::IListAdapter;

namespace Elastos {
namespace Droid {
namespace App {

class ECO_PUBLIC ListActivity
    : public Activity
    , public IListActivity
{
private:
    class ECO_LOCAL MyListener
        : public Object
        , public IAdapterViewOnItemClickListener
    {
    public:
        CAR_INTERFACE_DECL()

        MyListener(
            /* [in] */ ListActivity* host);

        CARAPI OnItemClick(
            /* [in] */ IAdapterView* parent,
            /* [in] */ IView* view,
            /* [in] */ Int32 position,
            /* [in] */ Int64 id);

    private:
        ListActivity* mHost;
    };

    class ECO_LOCAL RequestRunnable
        : public Runnable
    {
    public:
        RequestRunnable(
            /* [in] */ ListActivity* host);

        CARAPI Run();

    private:
        ListActivity* mHost;
    };

public:
    CAR_INTERFACE_DECL()

    ListActivity();

    virtual ~ListActivity();

    CARAPI constructor();

    /**
     * Updates the screen state (current list and other views) when the
     * content changes.
     *
     * @see Activity#onContentChanged()
     */
    CARAPI OnContentChanged();

    /**
     * Provide the cursor for the list view.
     */
    CARAPI SetListAdapter(
        /* [in] */ IListAdapter* adapter);

    /**
     * Set the currently selected list item to the specified
     * position with the adapter's data
     *
     * @param position
     */
    CARAPI SetSelection(
        /* [in] */ Int32 position);

    /**
     * Get the position of the currently selected list item.
     */
    CARAPI GetSelectedItemPosition(
        /* [out] */ Int32* pos);

    /**
     * Get the cursor row ID of the currently selected list item.
     */
    CARAPI GetSelectedItemId(
        /* [out] */ Int64* id);

    /**
     * Get the activity's list view widget.
     */
    CARAPI GetListView(
        /* [out] */ IListView** listView);

    /**
     * Get the ListAdapter associated with this activity's ListView.
     */
    CARAPI GetListAdapter(
        /* [out] */ IListAdapter** listAdapter);

protected:
    /**
     * This method will be called when an item in the list is selected.
     * Subclasses should override. Subclasses can call
     * getListView().getItemAtPosition(position) if they need to access the
     * data associated with the selected item.
     *
     * @param l The ListView where the click happened
     * @param v The view that was clicked within the ListView
     * @param position The position of the view in the list
     * @param id The row id of the item that was clicked
     */
    virtual CARAPI OnListItemClick(
        /* [in] */ IListView* l,
        /* [in] */ IView* v,
        /* [in] */ Int32 position,
        /* [in] */ Int64 id);

    /**
     * Ensures the list view has been created before Activity restores all
     * of the view states.
     *
     *@see Activity#onRestoreInstanceState(Bundle)
     */
    CARAPI OnRestoreInstanceState(
        /* [in] */ IBundle* state);

    /**
     * @see Activity#onDestroy()
     */
    CARAPI OnDestroy();

private:
    CARAPI EnsureList();

protected:
    /**
     * This field should be made private, so it is hidden from the SDK.
     * {@hide}
     */
    AutoPtr<IListAdapter> mAdapter;

    /**
     * This field should be made private, so it is hidden from the SDK.
     * {@hide}
     */
    AutoPtr<IListView> mList;

private:
    AutoPtr<IHandler> mHandler;
    Boolean mFinishedStart;

    AutoPtr<IRunnable> mRequestFocus;

    AutoPtr<IAdapterViewOnItemClickListener> mOnClickListener;
};

} // namespace App
} // namespace Droid
} // namespace Elastos

#endif  // __ELASTOS_DROID_APP_LISTACTIVITY_H__
