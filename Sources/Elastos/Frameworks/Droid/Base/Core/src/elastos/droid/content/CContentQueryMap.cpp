
#include <Elastos.CoreLibrary.IO.h>
#include "Elastos.Droid.Database.h"
#include "elastos/droid/content/CContentQueryMap.h"
#include "elastos/droid/content/CContentValues.h"
#include "elastos/droid/os/CHandler.h"
#include <elastos/core/CoreUtils.h>
#include <elastos/core/AutoLock.h>

using Elastos::Droid::Os::CHandler;

using Elastos::Utility::IArrayList;
using Elastos::Utility::CArrayList;
using Elastos::Utility::CHashMap;
using Elastos::Utility::IObservable;
using Elastos::Utility::EIID_IObservable;
using Elastos::Core::CString;
using Elastos::Core::ICharSequence;
using Elastos::Core::CoreUtils;
using Elastos::IO::ICloseable;

namespace Elastos {
namespace Droid {
namespace Content {

//=========================================================================
// CContentQueryMap::KeepUpdatedContentObserver
//=========================================================================
CContentQueryMap::KeepUpdatedContentObserver::KeepUpdatedContentObserver(
    /* [in] */ IWeakReference* host)
    : mWeakHost(host)
{
}

// @Override
ECode CContentQueryMap::KeepUpdatedContentObserver::OnChange(
    /* [in] */ Boolean selfChange)
{
    // If anyone is listening, we need to do this now to broadcast
    // to the observers.  Otherwise, we'll just set mDirty and
    // let it query lazily when they ask for the values.
    AutoPtr<IContentQueryMap> map;
    mWeakHost->Resolve(EIID_IContentQueryMap, (IInterface**)&map);
    if (map != NULL) {
        Int32 count = 0;
        IObservable* observable = IObservable::Probe(map.Get());
        assert(observable != NULL);
        observable->CountObservers(&count);

        if (count != 0) {
            map->Requery();
        }
        else {
            ((CContentQueryMap*)map.Get())->mDirty = TRUE;
        }
    }

    return NOERROR;
}

//=========================================================================
// CContentQueryMap
//=========================================================================
CAR_INTERFACE_IMPL(CContentQueryMap, Observable, IContentQueryMap)

CAR_OBJECT_IMPL(CContentQueryMap)

CContentQueryMap::CContentQueryMap()
    : mKeyColumn(0)
    , mKeepUpdated(FALSE)
    , mDirty(FALSE)
{}

CContentQueryMap::~CContentQueryMap()
{
    Finalize();
}

ECode CContentQueryMap::constructor(
    /* [in] */ ICursor* cursor,
    /* [in] */ const String& columnNameOfKey,
    /* [in] */ Boolean keepUpdated,
    /* [in] */ IHandler* handlerForUpdateNotifications)
{
    mCursor = cursor;
    FAIL_RETURN(mCursor->GetColumnNames((ArrayOf<String>**)&mColumnNames))
    FAIL_RETURN(mCursor->GetColumnIndexOrThrow(columnNameOfKey, &mKeyColumn))
    mHandlerForUpdateNotifications = handlerForUpdateNotifications;
    FAIL_RETURN(SetKeepUpdated(keepUpdated))

    // If we aren't keeping the cache updated with the current state of the cursor's
    // ContentProvider then read it once into the cache. Otherwise the cache will be filled
    // automatically.
    if (!keepUpdated) {
        FAIL_RETURN(ReadCursorIntoCache(cursor))
    }

    return Observable::constructor();
}

ECode CContentQueryMap::SetKeepUpdated(
    /* [in] */ Boolean keepUpdated)
{
    if (keepUpdated == mKeepUpdated) return NOERROR;
    mKeepUpdated = keepUpdated;

    if (!mKeepUpdated) {
        FAIL_RETURN(mCursor->UnregisterContentObserver(mContentObserver))
        mContentObserver = NULL;
    }
    else {
        if (NULL == mHandlerForUpdateNotifications) {
            CHandler::New((IHandler**)&mHandlerForUpdateNotifications);
        }

        if (NULL == mContentObserver) {
            AutoPtr<IWeakReference> wr;
            GetWeakReference((IWeakReference**)&wr);
            mContentObserver = new KeepUpdatedContentObserver(wr);
            mContentObserver->constructor(mHandlerForUpdateNotifications);
        }
        FAIL_RETURN(mCursor->RegisterContentObserver(mContentObserver))
        // mark dirty, since it is possible the cursor's backing data had changed before we
        // registered for changes
        mDirty = TRUE;
    }

    return NOERROR;
}

ECode CContentQueryMap::GetValues(
    /* [in] */ const String& rowName,
    /* [out] */ IContentValues** contentValues)
{
    VALIDATE_NOT_NULL(contentValues)
    *contentValues = NULL;

    AutoLock lock(this);
    if (mDirty) {
        FAIL_RETURN(Requery())
    }

    AutoPtr<IInterface> value;
    AutoPtr<ICharSequence> keyObj = CoreUtils::Convert(rowName);
    mValues->Get(TO_IINTERFACE(keyObj), (IInterface**)&value);
    *contentValues = IContentValues::Probe(value);
    REFCOUNT_ADD(*contentValues)
    return NOERROR;
}

ECode CContentQueryMap::Requery()
{
    AutoPtr<ICursor> cursor = mCursor;
    if (NULL == cursor) {
        // If mCursor is null then it means there was a requery() in flight
        // while another thread called close(), which nulls out mCursor.
        // If this happens ignore the requery() since we are closed anyways.
        return NOERROR;
    }

    mDirty = FALSE;
    Boolean ret = FALSE;
    FAIL_RETURN(cursor->Requery(&ret))
    if (!ret) {
        // again, don't do anything if the cursor is already closed
        return NOERROR;
    }
    FAIL_RETURN(ReadCursorIntoCache(cursor))

    IObservable* observable = this;
    FAIL_RETURN(observable->SetChanged())
    FAIL_RETURN(observable->NotifyObservers())
    return NOERROR;
}

ECode CContentQueryMap::ReadCursorIntoCache(
    /* [in] */ ICursor* cursor)
{
    AutoLock lock(this);
    // Make a new map so old values returned by getRows() are undisturbed.
    Int32 capacity = 0;
    if (mValues != NULL) {
        mValues->GetSize(&capacity);
    }
    mValues = NULL;
    CHashMap::New(capacity, (IMap**)&mValues);

    Boolean hasNext = FALSE;
    String columnValue, keyColumn;
    AutoPtr<ICharSequence> key;

    while ((cursor->MoveToNext(&hasNext), hasNext)) {
        AutoPtr<IContentValues> values;
        FAIL_RETURN(CContentValues::New((IContentValues**)&values))
        for (Int32 i = 0; i < mColumnNames->GetLength(); i++) {
            if (i != mKeyColumn) {
                FAIL_RETURN(cursor->GetString(i, &columnValue))
                FAIL_RETURN(values->Put((*mColumnNames)[i], columnValue))
            }
        }
        FAIL_RETURN(cursor->GetString(mKeyColumn, &keyColumn))
        key = CoreUtils::Convert(keyColumn);
        mValues->Put(TO_IINTERFACE(key), TO_IINTERFACE(values));
    }

    return NOERROR;
}

ECode CContentQueryMap::GetRows(
    /* [out] */ IMap** rows)
{
    VALIDATE_NOT_NULL(rows);
    *rows = NULL;

    AutoLock lock(this);
    if (mDirty) {
        FAIL_RETURN(Requery())
    }

    *rows = mValues;
    REFCOUNT_ADD(*rows)

    return NOERROR;
}

ECode CContentQueryMap::Close()
{
    AutoLock lock(this);
    if (NULL != mContentObserver) {
        FAIL_RETURN(mCursor->UnregisterContentObserver(mContentObserver))
        mContentObserver = NULL;
    }
    FAIL_RETURN(ICloseable::Probe(mCursor)->Close())
    mCursor = NULL;
    return NOERROR;
}

ECode CContentQueryMap::Finalize()
{
    if (NULL != mCursor) {
        FAIL_RETURN(Close())
    }
//    super.finalize();
    return NOERROR;
}

}
}
}

