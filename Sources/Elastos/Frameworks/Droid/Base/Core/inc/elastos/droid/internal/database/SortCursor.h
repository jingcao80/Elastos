#ifndef __ELASTOS_DROID_INTERNAL_UTILITY_SORTCURSOR_H__
#define __ELASTOS_DROID_INTERNAL_UTILITY_SORTCURSOR_H__

#include "elastos/droid/ext/frameworkext.h"
#include "Elastos.Droid.Internal.h"
#include "elastos/droid/database/AbstractCursor.h"

using Elastos::Droid::Database::AbstractCursor;
using Elastos::Droid::Database::IAbstractCursor;
using Elastos::Droid::Database::ICrossProcessCursor;
using Elastos::Droid::Database::ICursor;
using Elastos::Droid::Database::IDataSetObserver;
using Elastos::IO::ICloseable;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Database {

class SortCursor
    : public AbstractCursor
    , public ISortCursor
{
private:
    class DataSetObserver
        : public Object
        , public IDataSetObserver
    {
    public:
        DataSetObserver(
            /* [in] */ SortCursor* owner);

        CAR_INTERFACE_DECL()

        // @Override
        CARAPI OnChanged();

        // @Override
        CARAPI OnInvalidated();

    private:
        SortCursor* mOwner;
    };

public:
    CAR_INTERFACE_DECL()

    SortCursor();

    CARAPI constructor(
        /* [in] */ ArrayOf<ICursor*>* cursors,
        /* [in] */ const String& sortcolumn);

    // @Override
    CARAPI GetCount(
        /* [out] */ Int32* result);

    // @Override
    CARAPI OnMove(
        /* [in] */ Int32 oldPosition,
        /* [in] */ Int32 newPosition,
        /* [out] */ Boolean* result);

    // @Override
    CARAPI GetString(
        /* [in] */ Int32 column,
        /* [out] */ String* result);

    // @Override
    CARAPI GetInt16(
        /* [in] */ Int32 column,
        /* [out] */ Int16* result);

    // @Override
    CARAPI GetInt32(
        /* [in] */ Int32 column,
        /* [out] */ Int32* result);

    // @Override
    CARAPI GetInt64(
        /* [in] */ Int32 column,
        /* [out] */ Int64* result);

    // @Override
    CARAPI GetFloat(
        /* [in] */ Int32 column,
        /* [out] */ Float* result);

    // @Override
    CARAPI GetDouble(
        /* [in] */ Int32 column,
        /* [out] */ Double* result);

    // @Override
    CARAPI GetType(
        /* [in] */ Int32 column,
        /* [out] */ Int32* result);

    // @Override
    CARAPI IsNull(
        /* [in] */ Int32 column,
        /* [out] */ Boolean* result);

    // @Override
    CARAPI GetBlob(
        /* [in] */ Int32 column,
        /* [out,callee]*/ ArrayOf<Byte>** blob);

    // @Override
    CARAPI GetColumnNames(
        /* [out,callee]*/ ArrayOf<String>** columnNames);

    // @Override
    CARAPI Deactivate();

    // @Override
    CARAPI Close();

    // @Override
    CARAPI RegisterDataSetObserver(
        /* [in] */ IDataSetObserver* observer);

    // @Override
    CARAPI UnregisterDataSetObserver(
        /* [in] */ IDataSetObserver* observer);

    // @Override
    CARAPI Requery(
        /* [out] */ Boolean* result);

private:
    static const String TAG;
    AutoPtr<ICursor> mCursor; // updated in onMove
    AutoPtr<ArrayOf<ICursor*> > mCursors;
    AutoPtr<ArrayOf<Int32> > mSortColumns;
    const Int32 ROWCACHESIZE;
    AutoPtr<ArrayOf<Int32> > mRowNumCache;
    AutoPtr<ArrayOf<Int32> > mCursorCache;
    AutoPtr<ArrayOf<ArrayOf<Int32>*> > mCurRowNumCache;
    Int32 mLastCacheHit;

    AutoPtr<IDataSetObserver> mObserver;
};

} // namespace Database
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_INTERNAL_UTILITY_SORTCURSOR_H__
