
#ifndef __BOOKSDB_H__
#define __BOOKSDB_H__

#include <elautoptr.h>
#include "Elastos.Droid.Core.h"
#include "Elastos.Core.h"
#include <elastos/core/StringBuilder.h>
#include <database/sqlite/SQLiteOpenHelper.h>
#include <stdio.h>

using Elastos::Droid::Content::IContentValues;
using Elastos::Droid::Content::CContentValues;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Database::ICursor;
using Elastos::Droid::Database::Sqlite::ISQLiteDatabase;
using Elastos::Droid::Database::Sqlite::SQLiteOpenHelper;
using Elastos::Core::StringBuilder;
using Elastos::Core::ICharSequence;
using Elastos::Core::CStringWrapper;

namespace Elastos {
namespace Droid {
namespace DevSamples {
namespace DatabaseDemo {

class BooksDB
    : public SQLiteOpenHelper
{
public:
    BooksDB(
        /* [in] */ IContext* context);

    CARAPI OnCreate(
        /* [in] */ ISQLiteDatabase* db);

    CARAPI OnUpgrade(
        /* [in] */ ISQLiteDatabase* db,
        /* [in] */ Int32 oldVersion,
        /* [in] */ Int32 newVersion);

    CARAPI_(AutoPtr<ICursor>) Select();

    CARAPI_(Int64) Insert(
        /* [in] */ const String& bookname,
        /* [in] */ const String& author);

    CARAPI_(void) Delete(
        /* [in] */ Int32 id);

    CARAPI_(void) Update(
        /* [in] */ Int32 id,
        /* [in] */ const String& bookname,
        /* [in] */ const String& author);

public:
    const static String BOOK_ID;
    const static String BOOK_NAME;
    const static String BOOK_AUTHOR;

private:
    const static String DATABASE_NAME;
    const static Int32  DATABASE_VERSION;
    const static String TABLE_NAME;
};

}  //DatabaseDemo
}  //DevSamples
}  //Droid
}  //Elastos

#endif //__CBOOKSDB_H__



