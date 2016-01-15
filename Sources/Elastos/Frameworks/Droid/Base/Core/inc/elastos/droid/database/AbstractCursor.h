#ifndef __ELASTOS_DROID_DATABASE_ABSTRACTCURSOR_H__
#define __ELASTOS_DROID_DATABASE_ABSTRACTCURSOR_H__

#include "elastos/droid/ext/frameworkext.h"
#include <Elastos.CoreLibrary.IO.h>
#include "elastos/droid/database/ContentObserver.h"
#include <elastos/core/Object.h>
#include <elastos/utility/etl/HashMap.h>

using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Content::IContentResolver;
using Elastos::IO::ICloseable;
using Elastos::Utility::Etl::HashMap;


namespace Elastos {
namespace Droid {
namespace Database {
/**
 * This is an abstract cursor class that handles a lot of the common code
 * that all cursors need to deal with and is provided for convenience reasons.
 */
class AbstractCursor
    : public Object
    , public IAbstractCursor
    , public ICrossProcessCursor
    , public ICursor
    , public ICloseable
{
protected:
    class SelfContentObserver : public ContentObserver
    {
    public:
        SelfContentObserver();

        CARAPI constructor(
            /* [in] */ IAbstractCursor* cursor);

        //@Override
        CARAPI DeliverSelfNotifications(
            /* [out] */ Boolean* rst);

        //@Override
        CARAPI OnChange(
            /* [in] */ Boolean selfChange);

    protected:
        AutoPtr<IWeakReference> mCursor;
    };

public:
    CAR_INTERFACE_DECL()

    AbstractCursor();

    virtual ~AbstractCursor();

    virtual CARAPI GetCount(
        /* [out] */ Int32* count) = 0;

    virtual CARAPI GetColumnNames(
        /* [out, callee] */ ArrayOf<String>** names) = 0;

    virtual CARAPI GetString(
        /* [in] */ Int32 column,
        /* [out] */ String* str) = 0;

    virtual CARAPI GetInt16(
        /* [in] */ Int32 column,
        /* [out] */ Int16* value) = 0;

    virtual CARAPI GetInt32(
        /* [in] */ Int32 column,
        /* [out] */ Int32* value) = 0;

    virtual CARAPI GetInt64(
        /* [in] */ Int32 column,
        /* [out] */ Int64* value) = 0;

    virtual CARAPI GetFloat(
        /* [in] */ Int32 column,
        /* [out] */ Float* value) = 0;

    virtual CARAPI GetDouble(
        /* [in] */ Int32 column,
        /* [out] */ Double* value) = 0;

    virtual CARAPI IsNull(
        /* [in] */ Int32 column,
        /* [out] */ Boolean* isNull) = 0;

    virtual CARAPI GetType(
        /* [in] */ Int32 column,
        /* [out] */ Int32* type);

    // TODO implement getBlob in all cursor types
    virtual CARAPI GetBlob(
        /* [in] */ Int32 column,
        /* [out] */ ArrayOf<Byte>** blob);

    /* Methods that may optionally be implemented by subclasses */

    /**
     * If the cursor is backed by a {@link CursorWindow}, returns a pre-filled
     * window with the contents of the cursor, otherwise null.
     *
     * @return The pre-filled window that backs this cursor, or null if none.
     */
    virtual CARAPI GetWindow(
        /* [out] */ ICursorWindow** window);

    virtual CARAPI GetColumnCount(
        /* [out] */ Int32* columnCount);

    virtual CARAPI Deactivate();

    virtual CARAPI Requery(
        /* [out] */ Boolean* result);

    virtual CARAPI IsClosed(
        /* [out] */ Boolean* closed);

    virtual CARAPI Close();

    /**
     * This function is called every time the cursor is successfully scrolled
     * to a new position, giving the subclass a chance to update any state it
     * may have. If it returns false the move function will also do so and the
     * cursor will scroll to the beforeFirst position.
     *
     * @param oldPosition the position that we're moving from
     * @param newPosition the position that we're moving to
     * @return true if the move is successful, false otherwise
     */
    virtual CARAPI OnMove(
        /* [in] */ Int32 oldPosition,
        /* [in] */ Int32 newPosition,
        /* [out] */ Boolean* succeeded);

    virtual CARAPI CopyStringToBuffer(
        /* [in] */ Int32 columnIndex,
        /* [in] */ ICharArrayBuffer* buffer);

    virtual CARAPI GetPosition(
        /* [out] */ Int32* position);

    virtual CARAPI MoveToPosition(
        /* [in] */ Int32 position,
        /* [out] */ Boolean* succeeded);

    //@Override
    virtual CARAPI FillWindow(
        /* [in] */ Int32 position,
        /* [in] */ ICursorWindow* window);

    virtual CARAPI Move(
        /* [in] */ Int32 offset,
        /* [out] */ Boolean* succeeded);

    virtual CARAPI MoveToFirst(
        /* [out] */ Boolean* succeeded);

    virtual CARAPI MoveToLast(
        /* [out] */ Boolean* succeeded);

    virtual CARAPI MoveToNext(
        /* [out] */ Boolean* succeeded);

    virtual CARAPI MoveToPrevious(
        /* [out] */ Boolean* succeeded);

    virtual CARAPI IsFirst(
        /* [out] */ Boolean* isFirst);

    virtual CARAPI IsLast(
        /* [out] */ Boolean* isLast);

    virtual CARAPI IsBeforeFirst(
        /* [out] */ Boolean* isBeforeFirst);

    virtual CARAPI IsAfterLast(
        /* [out] */ Boolean* isAfterLast);

    virtual CARAPI GetColumnIndex(
        /* [in] */ const String& columnName,
        /* [out] */ Int32* columnIndex);

    virtual CARAPI GetColumnIndexOrThrow(
        /* [in] */ const String& columnName,
        /* [out] */ Int32* index);

    virtual CARAPI GetColumnName(
        /* [in] */ Int32 columnIndex,
        /* [out] */ String* columnName);

    virtual CARAPI RegisterContentObserver(
        /* [in] */ IContentObserver* observer);

    virtual CARAPI UnregisterContentObserver(
        /* [in] */ IContentObserver* observer);

    virtual CARAPI RegisterDataSetObserver(
        /* [in] */ IDataSetObserver* observer);

    virtual CARAPI UnregisterDataSetObserver(
        /* [in] */ IDataSetObserver* observer);

    /**
     * Specifies a content URI to watch for changes.
     *
     * @param cr The content resolver from the caller's context.
     * @param notifyUri The URI to watch for changes. This can be a
     * specific row URI, or a base URI for a whole class of content.
     */
    virtual CARAPI SetNotificationUri(
         /* [in] */ IContentResolver* cr,
         /* [in] */ IUri* notifyUri);

    /** @hide - set the notification uri but with an observer for a particular user's view */
    virtual CARAPI SetNotificationUri(
        /* [in] */ IContentResolver* cr,
        /* [in] */ IUri* notifyUri,
        /* [in] */ Int32 userHandle);

    virtual CARAPI GetNotificationUri(
        /* [out] */ IUri** uri);

    virtual CARAPI GetWantsAllOnMoveCalls(
        /* [out] */ Boolean* calls);

    /**
     * Sets a {@link Bundle} that will be returned by {@link #getExtras()}.  <code>null</code> will
     * be converted into {@link Bundle#EMPTY}.
     *
     * @param extras {@link Bundle} to set.
     * @hide
     */
    virtual CARAPI SetExtras(
        /* [in] */ IBundle* extras);

    virtual CARAPI GetExtras(
        /* [out] */ IBundle** extras);

    virtual CARAPI Respond(
        /* [in] */ IBundle* extras,
        /* [out] */ IBundle** v);

protected:
    /** @hide */
    CARAPI OnDeactivateOrClose();

    /**
     * Subclasses must call this method when they finish committing updates to notify all
     * observers.
     *
     * @param selfChange
     */
    CARAPI OnChange(
        /* [in] */ Boolean selfChange);

    /**
     * @deprecated Always returns false since Cursors do not support updating rows
     */
    //@Deprecated
    CARAPI_(Boolean) IsFieldUpdated(
        /* [in] */ Int32 columnIndex);

    /**
     * @deprecated Always returns null since Cursors do not support updating rows
     */
    //@Deprecated
    CARAPI_(AutoPtr<IInterface>) GetUpdatedField(
        /* [in] */ Int32 columnIndex);

    /**
     * This function throws CursorIndexOutOfBoundsException if
     * the cursor position is out of bounds. Subclass implementations of
     * the get functions should call this before attempting
     * to retrieve data.
     *
     * @throws CursorIndexOutOfBoundsException
     */
    CARAPI CheckPosition();

protected:
    /**
     * @deprecated This is never updated by this class and should not be used
     */
    //@Deprecated
    typedef HashMap<String, AutoPtr<IInterface> > StringObjectMap;
    HashMap<Int64, AutoPtr<StringObjectMap> > mUpdatedRows;

    Int32 mPos;

    /**
     * This must be set to the index of the row ID column by any
     * subclass that wishes to support updates.
     *
     * @deprecated This field should not be used.
     */
    //@Deprecated
    Int32 mRowIdColumnIndex;

    /**
     * If {@link #mRowIdColumnIndex} is not -1 this contains contains the value of
     * the column at {@link #mRowIdColumnIndex} for the current row this cursor is
     * pointing at.
     *
     * @deprecated This field should not be used.
     */
    //@Deprecated
    Int64 mCurrentRowID;
    Boolean mClosed;
    AutoPtr<IContentResolver> mContentResolver;

private:
    const static String TAG;

    AutoPtr<IUri> mNotifyUri;

    Object mSelfObserverLock;
    AutoPtr<IContentObserver> mSelfObserver;
    Boolean mSelfObserverRegistered;

    AutoPtr<IDataSetObservable> mDataSetObservable;
    AutoPtr<IContentObservable> mContentObservable;

    AutoPtr<IBundle> mExtras;

    friend class SelfContentObserver;
};

} //Database
} //Droid
} //Elastos

#endif //__ELASTOS_DROID_DATABASE_ABSTRACTCURSOR_H__
