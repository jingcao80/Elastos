
#ifndef __ELASTOS_DROID_CONTENT_CCONTENTQUERYMAP_H__
#define __ELASTOS_DROID_CONTENT_CCONTENTQUERYMAP_H__

#include "_Elastos_Droid_Content_CContentQueryMap.h"
#include <elastos/utility/Observable.h>
#include "elastos/droid/database/ContentObserver.h"

using Elastos::Droid::Database::ICursor;
using Elastos::Droid::Database::IContentObserver;
using Elastos::Droid::Database::ContentObserver;
using Elastos::Droid::Os::IHandler;
using Elastos::Utility::IObserver;
using Elastos::Utility::IMap;
using Elastos::Utility::Observable;

namespace Elastos {
namespace Droid {
namespace Content {

/**
 * Caches the contents of a cursor into a Map of String->ContentValues and optionally
 * keeps the cache fresh by registering for updates on the content backing the cursor. The column of
 * the database that is to be used as the key of the map is user-configurable, and the
 * ContentValues contains all columns other than the one that is designated the key.
 * <p>
 * The cursor data is accessed by row key and column name via getValue().
 */
CarClass(CContentQueryMap)
    , public Observable
    , public IContentQueryMap
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CContentQueryMap();

    virtual ~CContentQueryMap();

    CARAPI constructor(
        /* [in] */ ICursor* cursor,
        /* [in] */ const String& columnNameOfKey,
        /* [in] */ Boolean keepUpdated,
        /* [in] */ IHandler* handlerForUpdateNotifications);

    /**
     * Change whether or not the ContentQueryMap will register with the cursor's ContentProvider
     * for change notifications. If you use a ContentQueryMap in an activity you should call this
     * with false in onPause(), which means you need to call it with true in onResume()
     * if want it to be kept updated.
     * @param keepUpdated if true the ContentQueryMap should be registered with the cursor's
     * ContentProvider, false otherwise
     */
    CARAPI SetKeepUpdated(
        /* [in] */ Boolean keepUpdated);

    /**
     * Access the ContentValues for the row specified by rowName
     * @param rowName which row to read
     * @return the ContentValues for the row, or null if the row wasn't present in the cursor
     */
    CARAPI GetValues(
        /* [in] */ const String& rowName,
        /* [out] */ IContentValues** contentValues);

    /** Requeries the cursor and reads the contents into the cache */
    CARAPI Requery();

    CARAPI GetRows(
        /* [out] */ IMap** rows);

    CARAPI Close();

public:
    class KeepUpdatedContentObserver
        : public ContentObserver
    {
    public:
        TO_STRING_IMPL("KeepUpdatedContentObserver::KeepUpdatedContentObserver")

        KeepUpdatedContentObserver(
            /* [in] */ IWeakReference* host);

        // @Override
        CARAPI OnChange(
            /* [in] */ Boolean selfChange);

    private:
        AutoPtr<IWeakReference> mWeakHost;
    };

protected:
    CARAPI Finalize();

private:
    CARAPI ReadCursorIntoCache(
        /* [in] */ ICursor* cursor);

private:
    friend class KeepUpdatedContentObserver;

    /* volatile */ AutoPtr<ICursor> mCursor;
    AutoPtr<ArrayOf<String> > mColumnNames;
    Int32 mKeyColumn;

    AutoPtr<IHandler> mHandlerForUpdateNotifications;
    Boolean mKeepUpdated;

    AutoPtr<IMap> mValues;
    AutoPtr<KeepUpdatedContentObserver> mContentObserver;

    /** Set when a cursor change notification is received and is cleared on a call to requery(). */
    Boolean mDirty;
};

}
}
}

#endif // __ELASTOS_DROID_CONTENT_CCONTENTQUERYMAP_H__
