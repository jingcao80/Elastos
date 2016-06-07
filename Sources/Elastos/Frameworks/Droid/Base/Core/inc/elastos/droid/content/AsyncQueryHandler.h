
#ifndef __ELASTOS_DROID_CONTENT_ASYNCQUERYHANDLER_H__
#define __ELASTOS_DROID_CONTENT_ASYNCQUERYHANDLER_H__

#include "Elastos.Droid.Content.h"
#include "elastos/droid/os/Handler.h"

using Elastos::Droid::Database::ICursor;
using Elastos::Droid::Net::IUri;
using Elastos::Droid::Os::Handler;
using Elastos::Core::IRunnable;

namespace Elastos {
namespace Droid {
namespace Content {

/**
 * A helper class to help make handling asynchronous {@link ContentResolver}
 * queries easier.
 */
class ECO_PUBLIC AsyncQueryHandler
    : public Handler
    , public IAsyncQueryHandler
{
protected:
    class WorkerArgs
        : public Object
    {
    public:
        WorkerArgs();

        virtual ~WorkerArgs();

    public:
        AutoPtr<IUri> mUri;
        AutoPtr<IHandler> mHandler;
        AutoPtr<ArrayOf<String> > mProjection;
        String mSelection;
        AutoPtr<ArrayOf<String> > mSelectionArgs;
        String mOrderBy;
        AutoPtr<IInterface> mResult;
        AutoPtr<IInterface> mCookie;
        AutoPtr<IContentValues> mValues;
    };

    class WorkerHandler
        : public Handler
    {
    public:
        TO_STRING_IMPL("AsyncQueryHandler::WorkerHandler")

        WorkerHandler(
            /* [in] */ IWeakReference* context);

        virtual ~WorkerHandler();

        CARAPI HandleMessage(
            /* [in] */ IMessage* msg);

    private:
        AutoPtr<IWeakReference> mWeakContext;
        //AutoPtr<AsyncQueryHandler> mContext;
    };

friend class WorkerHandler;

public:
    CAR_INTERFACE_DECL()

    TO_STRING_IMPL("AsyncQueryHandler")

    // must call constructor right after create a AsyncQueryHandler.
    AsyncQueryHandler();

    virtual ~AsyncQueryHandler();

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ IContentResolver* cr);

    /**
     * This method begins an asynchronous query. When the query is done
     * {@link #onQueryComplete} is called.
     *
     * @param token A token passed into {@link #onQueryComplete} to identify
     *  the query.
     * @param cookie An object that gets passed into {@link #onQueryComplete}
     * @param uri The URI, using the content:// scheme, for the content to
     *         retrieve.
     * @param projection A list of which columns to return. Passing null will
     *         return all columns, which is discouraged to prevent reading data
     *         from storage that isn't going to be used.
     * @param selection A filter declaring which rows to return, formatted as an
     *         SQL WHERE clause (excluding the WHERE itself). Passing null will
     *         return all rows for the given URI.
     * @param selectionArgs You may include ?s in selection, which will be
     *         replaced by the values from selectionArgs, in the order that they
     *         appear in the selection. The values will be bound as Strings.
     * @param orderBy How to order the rows, formatted as an SQL ORDER BY
     *         clause (excluding the ORDER BY itself). Passing null will use the
     *         default sort order, which may be unordered.
     */
    CARAPI StartQuery(
        /* [in] */ Int32 token,
        /* [in] */ IInterface* cookie,
        /* [in] */ IUri* uri,
        /* [in] */ ArrayOf<String>* projection,
        /* [in] */ const String& selection,
        /* [in] */ ArrayOf<String>* selectionArgs,
        /* [in] */ const String& orderBy);

    /**
     * Attempts to cancel operation that has not already started. Note that
     * there is no guarantee that the operation will be canceled. They still may
     * result in a call to on[Query/Insert/Update/Delete]Complete after this
     * call has completed.
     *
     * @param token The token representing the operation to be canceled.
     *  If multiple operations have the same token they will all be canceled.
     */
    CARAPI CancelOperation(
        /* [in] */ Int32 token);

    /**
     * This method begins an asynchronous insert. When the insert operation is
     * done {@link #onInsertComplete} is called.
     *
     * @param token A token passed into {@link #onInsertComplete} to identify
     *  the insert operation.
     * @param cookie An object that gets passed into {@link #onInsertComplete}
     * @param uri the Uri passed to the insert operation.
     * @param initialValues the ContentValues parameter passed to the insert operation.
     */
    CARAPI StartInsert(
        /* [in] */ Int32 token,
        /* [in] */ IInterface* cookie,
        /* [in] */ IUri* uri,
        /* [in] */ IContentValues* initialValues);

    /**
     * This method begins an asynchronous update. When the update operation is
     * done {@link #onUpdateComplete} is called.
     *
     * @param token A token passed into {@link #onUpdateComplete} to identify
     *  the update operation.
     * @param cookie An object that gets passed into {@link #onUpdateComplete}
     * @param uri the Uri passed to the update operation.
     * @param values the ContentValues parameter passed to the update operation.
     */
    CARAPI StartUpdate(
        /* [in] */ Int32 token,
        /* [in] */ IInterface* cookie,
        /* [in] */ IUri* uri,
        /* [in] */ IContentValues* values,
        /* [in] */ const String& selection,
        /* [in] */ ArrayOf<String>* selectionArgs);

    /**
     * This method begins an asynchronous delete. When the delete operation is
     * done {@link #onDeleteComplete} is called.
     *
     * @param token A token passed into {@link #onDeleteComplete} to identify
     *  the delete operation.
     * @param cookie An object that gets passed into {@link #onDeleteComplete}
     * @param uri the Uri passed to the delete operation.
     * @param selection the where clause.
     */
    CARAPI StartDelete(
        /* [in] */ Int32 token,
        /* [in] */ IInterface* cookie,
        /* [in] */ IUri* uri,
        /* [in] */ const String& selection,
        /* [in] */ ArrayOf<String>* selectionArgs);

    CARAPI HandleMessage(
        /* [in] */ IMessage* msg);

protected:
    CARAPI CreateHandler(
        /* [in] */ ILooper* looper,
        /* [out] */ IHandler** handler);

    /**
     * Called when an asynchronous query is completed.
     *
     * @param token the token to identify the query, passed in from
     *            {@link #startQuery}.
     * @param cookie the cookie object passed in from {@link #startQuery}.
     * @param cursor The cursor holding the results from the query.
     */
    virtual CARAPI OnQueryComplete(
        /* [in] */ Int32 token,
        /* [in] */ IInterface* cookie,
        /* [in] */ ICursor* cursor);

    /**
     * Called when an asynchronous insert is completed.
     *
     * @param token the token to identify the query, passed in from
     *        {@link #startInsert}.
     * @param cookie the cookie object that's passed in from
     *        {@link #startInsert}.
     * @param uri the uri returned from the insert operation.
     */
    virtual CARAPI OnInsertComplete(
        /* [in] */ Int32 token,
        /* [in] */ IInterface* cookie,
        /* [in] */ IUri* uri);

    /**
     * Called when an asynchronous update is completed.
     *
     * @param token the token to identify the query, passed in from
     *        {@link #startUpdate}.
     * @param cookie the cookie object that's passed in from
     *        {@link #startUpdate}.
     * @param result the result returned from the update operation
     */
    virtual CARAPI OnUpdateComplete(
        /* [in] */ Int32 token,
        /* [in] */ IInterface* cookie,
        /* [in] */ Int32 result);

    /**
     * Called when an asynchronous delete is completed.
     *
     * @param token the token to identify the query, passed in from
     *        {@link #startDelete}.
     * @param cookie the cookie object that's passed in from
     *        {@link #startDelete}.
     * @param result the result returned from the delete operation
     */
    virtual CARAPI OnDeleteComplete(
        /* [in] */ Int32 token,
        /* [in] */ IInterface* cookie,
        /* [in] */ Int32 result);

private:
    ECO_LOCAL static const String TAG;
    ECO_LOCAL static const Boolean localLOGV;

    ECO_LOCAL static const Int32 EVENT_ARG_QUERY = 1;
    ECO_LOCAL static const Int32 EVENT_ARG_INSERT = 2;
    ECO_LOCAL static const Int32 EVENT_ARG_UPDATE = 3;
    ECO_LOCAL static const Int32 EVENT_ARG_DELETE = 4;

    ECO_LOCAL static AutoPtr<ILooper> sLooper;

private:
    /* package */
//    final WeakReference<ContentResolver> mResolver;
    AutoPtr<IWeakReference> mResolver;
    AutoPtr<IHandler> mWorkerThreadHandler;
    Object mAsyncQueryHandlerLock;
};

}
}
}

#endif // __ELASTOS_DROID_CONTENT_ASYNCQUERYHANDLER_H__
