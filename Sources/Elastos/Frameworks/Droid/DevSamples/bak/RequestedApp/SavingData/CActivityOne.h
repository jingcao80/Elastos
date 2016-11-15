
#ifndef __CACTIVITYONE_H__
#define __CACTIVITYONE_H__

#include "elastos/droid/app/Activity.h"
#include "_CActivityOne.h"
#include <database/sqlite/SQLiteOpenHelper.h>

using Elastos::Droid::App::Activity;
using Elastos::Droid::Content::IContentValues;
using Elastos::Droid::Database::Sqlite::SQLiteOpenHelper;
using Elastos::Droid::View::IViewOnClickListener;
using Elastos::Droid::Widget::ITextView;
using Elastos::Droid::Widget::ISimpleAdapter;
using Elastos::Droid::Widget::IListView;

namespace Elastos {
namespace Droid {
namespace DevSamples {
namespace SavingData {

class FeedEntry {
public:
    static String TABLE_NAME;
    static String _ID;
    static String COLUMN_NAME_ENTRY_ID;
    static String COLUMN_NAME_TITLE;
    static String COLUMN_NAME_SUBTITLE;
};

class CActivityOne;

class FeedReaderDbHelper
    : public ElRefBase
    , public SQLiteOpenHelper
{
public:
    FeedReaderDbHelper(
        /* [in] */ IContext* context);

    CARAPI OnCreate(
        /* [in] */ ISQLiteDatabase* db);

    CARAPI OnUpgrade(
        /* [in] */ ISQLiteDatabase* db,
        /* [in] */ Int32 oldVersion,
        /* [in] */ Int32 newVersion);

    CARAPI OnDowngrade(
        /* [in] */ ISQLiteDatabase* db,
        /* [in] */ Int32 oldVersion,
        /* [in] */ Int32 newVersion);

public:
    // If you change the database schema, you must increment the database version.
    static const Int32 DATABASE_VERSION;
    static String DATABASE_NAME;
    friend class CActivityOne;
};

class CActivityOne : public Activity
{
public:
    class MyListener
        : public ElRefBase
        , public IViewOnClickListener
    {
    public:
        MyListener(
            /* [in] */ CActivityOne* host);

        CARAPI_(PInterface) Probe(
            /* [in]  */ REIID riid);

        CARAPI_(UInt32) AddRef();

        CARAPI_(UInt32) Release();

        CARAPI GetInterfaceID(
            /* [in] */ IInterface *pObject,
            /* [out] */ InterfaceID *pIID);

        CARAPI OnClick(
            /* [in] */ IView* v);

    private:
        CActivityOne* mHost;
        Int32 mRef;
    };

protected:
    CARAPI OnCreate(
        /* [in] */ IBundle* savedInstanceState);

    CARAPI OnStart();

    CARAPI OnResume();

    CARAPI OnPause();

    CARAPI OnStop();

    CARAPI OnDestroy();

private:
    CARAPI OnActivityResult(
        /* [in] */ Int32 requestCode,
        /* [in] */ Int32 resultCode,
        /* [in] */ IIntent *data);

    CARAPI_(AutoPtr<ISimpleAdapter>) GetSimpleAdapter(
        /* [in] */ ICursor* cursor);

public:
    static String TEXT_TYPE;
    static String COMMA_SEP;
    static String SQL_CREATE_ENTRIES;
    static String SQL_DELETE_ENTRIES;

private:
    AutoPtr<ITextView>  mValueText;
    AutoPtr<ITextView>  mInternalText;
    AutoPtr<ITextView>  mExternalText;
    AutoPtr<IView> mAddData;
    AutoPtr<IView> mDelData;
    AutoPtr<IListView> mListView;
    AutoPtr<FeedReaderDbHelper> mDbHelper;
    AutoPtr<ISQLiteDatabase> mDb;
};

} // namespace SavingData
} // namespace DevSamples
} // namespace Droid
} // namespace Elastos

#endif // __CACTIVITYONE_H__
