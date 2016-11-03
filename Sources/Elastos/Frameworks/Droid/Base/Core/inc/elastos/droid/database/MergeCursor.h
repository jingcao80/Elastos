#ifndef __ELASTOS_DROID_DATABASE_MERGECURSOR_H__
#define __ELASTOS_DROID_DATABASE_MERGECURSOR_H__

#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/database/AbstractCursor.h"
#include "elastos/droid/database/DataSetObserver.h"
#include <elastos/core/Object.h>


namespace Elastos {
namespace Droid {
namespace Database {
/**
 * A convience class that lets you present an array of Cursors as a single linear Cursor.
 * The schema of the cursors presented is entirely up to the creator of the MergeCursor, and
 * may be different if that is desired. Calls to getColumns, getColumnIndex, etc will return the
 * value for the row that the MergeCursor is currently pointing at.
 */
class ECO_PUBLIC MergeCursor
    : public AbstractCursor
    , public IMergeCursor
{
private:
    class ECO_LOCAL MyDataSetObserver
        : public DataSetObserver
    {
    public:
        MyDataSetObserver(
            /* [in] */ MergeCursor* host);

        CARAPI OnChanged();

        CARAPI OnInvalidated();

    private:
        MergeCursor* mHost;
    };

public:
    CAR_INTERFACE_DECL()

    MergeCursor();

    CARAPI constructor(
        /* [in] */ ArrayOf<ICursor*>* cursors);

    //@Override
    virtual CARAPI GetCount(
        /* [out] */ Int32* count);

    //@Override
    virtual CARAPI OnMove(
        /* [in] */ Int32 oldPosition,
        /* [in] */ Int32 newPosition,
        /* [out] */ Boolean* succeeded);

    //@Override
    virtual CARAPI GetString(
        /* [in] */ Int32 column,
        /* [out] */ String* str);

    //@Override
    virtual CARAPI GetInt16(
        /* [in] */ Int32 column,
        /* [out] */ Int16* value);

    //@Override
    virtual CARAPI GetInt32(
        /* [in] */ Int32 column,
        /* [out] */ Int32* value);

    //@Override
    virtual CARAPI GetInt64(
        /* [in] */ Int32 column,
        /* [out] */ Int64* value);

    //@Override
    virtual CARAPI GetFloat(
        /* [in] */ Int32 column,
        /* [out] */ Float* value);

    //@Override
    virtual CARAPI GetDouble(
        /* [in] */ Int32 column,
        /* [out] */ Double* value);

    //@Override
    virtual CARAPI GetType(
        /* [in] */ Int32 column,
        /* [out] */ Int32* type);

    //@Override
    virtual CARAPI IsNull(
        /* [in] */ Int32 column,
        /* [out] */ Boolean* isNull);

    //@Override
    virtual CARAPI GetBlob(
        /* [in] */ Int32 column,
        /* [out, callee] */ ArrayOf<Byte>** blob);

    //@Override
    virtual CARAPI GetColumnNames(
        /* [out, callee] */ ArrayOf<String>** names);

    //@Override
    virtual CARAPI Deactivate();

    //@Override
    virtual CARAPI Close();

    //@Override
    virtual CARAPI RegisterContentObserver(
        /* [in] */ IContentObserver* observer);

    //@Override
    virtual CARAPI UnregisterContentObserver(
        /* [in] */ IContentObserver* observer);

    //@Override
    virtual CARAPI RegisterDataSetObserver(
        /* [in] */ IDataSetObserver* observer);

    //@Override
    virtual CARAPI UnregisterDataSetObserver(
        /* [in] */ IDataSetObserver* observer);

    //@Override
    virtual CARAPI Requery(
        /* [out] */ Boolean* result);

private:
    AutoPtr<MyDataSetObserver> mObserver;

    AutoPtr<ICursor> mCursor; //updated in onMove
    AutoPtr< ArrayOf<ICursor*> > mCursors;
};

} //Database
} //Droid
} //Elastos

#endif // __ELASTOS_DROID_DATABASE_MERGECURSOR_H__
