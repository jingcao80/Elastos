
#ifndef __ELASTOS_DROID_WIDGET_CURSORADAPTER_H__
#define __ELASTOS_DROID_WIDGET_CURSORADAPTER_H__

#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.View.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/widget/BaseAdapter.h"
#include "elastos/droid/database/ContentObserver.h"
#include "elastos/droid/database/DataSetObserver.h"

using Elastos::Droid::View::IViewGroup;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Database::ICursor;
using Elastos::Droid::Database::DataSetObserver;
using Elastos::Droid::Database::IDataSetObserver;
using Elastos::Droid::Database::ContentObserver;
using Elastos::Droid::Os::IHandler;
using Elastos::Core::ICharSequence;

namespace Elastos {
namespace Droid {
namespace Widget {

/**
 * Adapter that exposes data from a {@link android.database.Cursor Cursor} to a
 * {@link android.widget.ListView ListView} widget. The Cursor must include
 * a column named "_id" or this class will not work.
 */
class ECO_PUBLIC CursorAdapter
    : public BaseAdapter
    , public ICursorAdapter
    , public IFilterable
    , public ICursorFilterClient
{
public:
    class ChangeObserver : public ContentObserver
    {
    public:
        ChangeObserver();

        CARAPI constructor(
            /* [in] */ ICursorAdapter* host);

        virtual CARAPI DeliverSelfNotifications(
            /* [out] */ Boolean* result);

        virtual CARAPI OnChange(
            /* [in] */ Boolean selfChange);

    protected:
        AutoPtr<IWeakReference> mWeakHost;
    };

    class MyDataSetObserver
        : public DataSetObserver
    {
    public:
        MyDataSetObserver();

        CARAPI constructor(
            /* [in] */ ICursorAdapter* host);

        virtual CARAPI OnChanged();

        virtual CARAPI OnInvalidated();

    protected:
        AutoPtr<IWeakReference> mWeakHost;
    };

public:
    CAR_INTERFACE_DECL()

    CursorAdapter();

    /**
     * Constructor. The adapter will call requery() on the cursor whenever
     * it changes so that the most recent data is always displayed.
     *
     * @param c The cursor from which to get the data.
     * @param context The context
     */
    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ ICursor* c);

    /**
     * Constructor
     * @param c The cursor from which to get the data.
     * @param context The context
     * @param autoRequery If true the adapter will call requery() on the
     *                    cursor whenever it changes so the most recent
     *                    data is always displayed.
     */
    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ ICursor* c,
        /* [in] */ Boolean autoRequery);

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ ICursor* c,
        /* [in] */ Int32 flags);

    virtual CARAPI GetCursor(
        /* [out] */ ICursor** cursor);

    /**
     * @see android.widget.ListAdapter#getCount()
     */
    virtual CARAPI GetCount(
        /* [out] */ Int32* count);

    /**
     * @see android.widget.ListAdapter#getItem(Int32)
     */
    virtual CARAPI GetItem(
        /* [in] */ Int32 position,
        /* [out] */ IInterface** item);

    /**
     * @see android.widget.ListAdapter#getItemId(Int32)
     */
    virtual CARAPI GetItemId(
        /* [in] */ Int32 position,
        /* [out] */ Int64* id);

    virtual CARAPI HasStableIds(
        /* [out] */ Boolean* hasStableIds);

    /**
     * @see android.widget.ListAdapter#getView(Int32, View, ViewGroup)
     */
    virtual CARAPI GetView(
        /* [in] */ Int32 position,
        /* [in] */ IView* convertView,
        /* [in] */ IViewGroup* parent,
        /* [out] */ IView** view);

    virtual CARAPI GetDropDownView(
        /* [in] */ Int32 position,
        /* [in] */ IView* convertView,
        /* [in] */ IViewGroup* parent,
        /* [out] */ IView** view);

    /**
     * Makes a new view to hold the data pointed to by cursor.
     * @param context Interface to application's global information
     * @param cursor The cursor from which to get the data. The cursor is already
     * moved to the correct position.
     * @param parent The parent to which the new view is attached to
     * @return the newly created view.
     */
    virtual CARAPI NewView(
        /* [in] */ IContext* context,
        /* [in] */ ICursor* cursor,
        /* [in] */ IViewGroup* parent,
        /* [out] */ IView** view) = 0;

    /**
     * Makes a new drop down view to hold the data pointed to by cursor.
     * @param context Interface to application's global information
     * @param cursor The cursor from which to get the data. The cursor is already
     * moved to the correct position.
     * @param parent The parent to which the new view is attached to
     * @return the newly created view.
     */
    virtual CARAPI NewDropDownView(
        /* [in] */ IContext* context,
        /* [in] */ ICursor* cursor,
        /* [in] */ IViewGroup* parent,
        /* [out] */ IView** view);

    /**
     * Bind an existing view to the data pointed to by cursor
     * @param view Existing view, returned earlier by newView
     * @param context Interface to application's global information
     * @param cursor The cursor from which to get the data. The cursor is already
     * moved to the correct position.
     */
    virtual CARAPI BindView(
        /* [in] */ IView* view,
        /* [in] */ IContext* context,
        /* [in] */ ICursor* cursor) = 0;

    /**
     * Change the underlying cursor to a new cursor. If there is an existing cursor it will be
     * closed.
     *
     * @param cursor the new cursor to be used
     */
    virtual CARAPI ChangeCursor(
        /* [in] */ ICursor* cursor);

    /**
     * <p>Converts the cursor into a CharSequence. Subclasses should override this
     * method to convert their results. The default implementation returns an
     * empty String for NULL values or the default String representation of
     * the value.</p>
     *
     * @param cursor the cursor to convert to a CharSequence
     * @return a CharSequence representing the value
     */
    virtual CARAPI ConvertToString(
        /* [in] */ ICursor* cursor,
        /* [out] */ ICharSequence** charSequence);

    /**
     * Runs a query with the specified constraint. This query is requested
     * by the filter attached to this adapter.
     *
     * The query is provided by a
     * {@link android.widget.FilterQueryProvider}.
     * If no provider is specified, the current cursor is not filtered and returned.
     *
     * After this method returns the resulting cursor is passed to {@link #changeCursor(Cursor)}
     * and the previous cursor is closed.
     *
     * This method is always executed on a background thread, not on the
     * application's main thread (or UI thread.)
     *
     * Contract: when constraint is NULL or empty, the original results,
     * prior to any filtering, must be returned.
     *
     * @param constraint the constraint with which the query must be filtered
     *
     * @return a Cursor representing the results of the new query
     *
     * @see #getFilter()
     * @see #getFilterQueryProvider()
     * @see #setFilterQueryProvider(android.widget.FilterQueryProvider)
     */
    virtual CARAPI RunQueryOnBackgroundThread(
        /* [in] */ ICharSequence* constraint,
        /* [out] */ ICursor** cursor);

    virtual CARAPI GetFilter(
        /* [out] */ IFilter** filter);

    /**
     * Returns the query filter provider used for filtering. When the
     * provider is NULL, no filtering occurs.
     *
     * @return the current filter query provider or NULL if it does not exist
     *
     * @see #setFilterQueryProvider(android.widget.FilterQueryProvider)
     * @see #runQueryOnBackgroundThread(CharSequence)
     */
    virtual CARAPI GetFilterQueryProvider(
        /* [out] */ IFilterQueryProvider** filterQueryProvider);

    /**
     * Sets the query filter provider used to filter the current Cursor.
     * The provider's
     * {@link android.widget.FilterQueryProvider#runQuery(CharSequence)}
     * method is invoked when filtering is requested by a client of
     * this adapter.
     *
     * @param filterQueryProvider the filter query provider or NULL to remove it
     *
     * @see #getFilterQueryProvider()
     * @see #runQueryOnBackgroundThread(CharSequence)
     */
    virtual CARAPI SetFilterQueryProvider(
        /* [in] */ IFilterQueryProvider* filterQueryProvider);

    /**
     * Swap in a new Cursor, returning the old Cursor.  Unlike
     * {@link #changeCursor(Cursor)}, the returned old Cursor is <em>not</em>
     * closed.
     *
     * @param newCursor The new cursor to be used.
     * @return Returns the previously set Cursor, or null if there wasa not one.
     * If the given new Cursor is the same instance is the previously set
     * Cursor, null is also returned.
     */
    virtual CARAPI SwapCursor(
        /* [in] */ ICursor* newCursor,
        /* [out] */ ICursor** cursor);

protected:
    virtual CARAPI Init(
        /* [in] */ IContext* context,
        /* [in] */ ICursor* c,
        /* [in] */ Boolean autoRequery);

    virtual CARAPI Init(
        /* [in] */ IContext* context,
        /* [in] */ ICursor* c,
        /* [in] */ Int32 flags = ICursorAdapter::FLAG_AUTO_REQUERY);
    /**
     * Called when the {@link ContentObserver} on the cursor receives a change notification.
     * The default implementation provides the auto-requery logic, but may be overridden by
     * sub classes.
     *
     * @see ContentObserver#onChange(Boolean)
     */
    virtual CARAPI OnContentChanged();

protected:
    /**
     * This field should be made private, so it is hidden from the SDK.
     * {@hide}
     */
    Boolean mDataValid;
    /**
     * This field should be made private, so it is hidden from the SDK.
     * {@hide}
     */
    Boolean mAutoRequery;
    /**
     * This field should be made private, so it is hidden from the SDK.
     * {@hide}
     */
    AutoPtr<ICursor> mCursor;
    /**
     * This field should be made private, so it is hidden from the SDK.
     * {@hide}
     */
    // context usually holds adapter, we use weak-reference here.
    AutoPtr<IWeakReference> mWeakContext;
    /**
     * This field should be made private, so it is hidden from the SDK.
     * {@hide}
     */
    Int32 mRowIDColumn;
    /**
     * This field should be made private, so it is hidden from the SDK.
     * {@hide}
     */
    AutoPtr<IContentObserver> mChangeObserver;
    /**
     * This field should be made private, so it is hidden from the SDK.
     * {@hide}
     */
    AutoPtr<IDataSetObserver> mDataSetObserver;// = new DataSetObserver();
    /**
     * This field should be made private, so it is hidden from the SDK.
     * {@hide}
     */
    AutoPtr<ICursorFilter> mCursorFilter;
    /**
     * This field should be made private, so it is hidden from the SDK.
     * {@hide}
     */
    AutoPtr<IFilterQueryProvider> mFilterQueryProvider;
};

}// namespace Elastos
}// namespace Droid
}// namespace Widget

#endif // __ELASTOS_DROID_WIDGET_CURSORADAPTER_H__
