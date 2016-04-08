
#ifndef __LISTACTIVITY_H__
#define __LISTACTIVITY_H__

#include "elastos/droid/app/Activity.h"

using Elastos::Droid::App::Activity;
using Elastos::Droid::Widget::IAdapterViewOnItemClickListener;
using Elastos::Droid::Widget::IListView;
using Elastos::Droid::Widget::IAdapterView;
using Elastos::Droid::Widget::IListAdapter;

namespace Elastos {
namespace Droid {
namespace DevSamples {
namespace AnimationsDemo {

class ListActivity: public Activity
{
private:
    class MyListener
        : public IAdapterViewOnItemClickListener
        , public ElRefBase
    {
    public:
        MyListener(
            /* [in] */ ListActivity* host);

        CARAPI_(PInterface) Probe(
            /* [in]  */ REIID riid);

        CARAPI_(UInt32) AddRef();

        CARAPI_(UInt32) Release();

        CARAPI GetInterfaceID(
            /* [in] */ IInterface *pObject,
            /* [out] */ InterfaceID *pIID);

        CARAPI OnItemClick(
            /* [in] */ IAdapterView* parent,
            /* [in] */ IView* view,
            /* [in] */ Int32 position,
            /* [in] */ Int64 id);

    private:
        AutoPtr<ListActivity> mHost;
    };

    class RequestRunnable
        : public ElRefBase
        , public IRunnable
    {
    public:
        RequestRunnable(
            /* [in] */ ListActivity* host);

        CARAPI_(PInterface) Probe(
            /* [in]  */ REIID riid);

        CARAPI_(UInt32) AddRef();

        CARAPI_(UInt32) Release();

        CARAPI GetInterfaceID(
            /* [in] */ IInterface *pObject,
            /* [out] */ InterfaceID *pIID);

        CARAPI Run();

    private:
        AutoPtr<ListActivity> mHost;
    };

public:
    ListActivity();

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
    CARAPI_(Int32) GetSelectedItemPosition();

    /**
     * Get the cursor row ID of the currently selected list item.
     */
    CARAPI_(Int64) GetSelectedItemId();

    /**
     * Get the activity's list view widget.
     */
    CARAPI_(AutoPtr<IListView>) GetListView();

    /**
     * Get the ListAdapter associated with this activity's ListView.
     */
    CARAPI_(AutoPtr<IListAdapter>) GetListAdapter();

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
    CARAPI_(void) EnsureList();

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
    Object mLock;
};

} // namespace AnimationsDemo
} // namespace DevSamples
} // namespace Droid
} // namespace Elastos

#endif  // __LISTACTIVITY_H__
