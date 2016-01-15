#include "BooksDB.h"
#include <stdio.h>
#include <elastos/core/StringUtils.h>

using Elastos::Core::StringUtils;

namespace Elastos {
namespace Droid {
namespace DevSamples {
namespace DatabaseDemo {

const String BooksDB::BOOK_ID = String("book_id");
const String BooksDB::BOOK_NAME = String("book_name");
const String BooksDB::BOOK_AUTHOR = String("book_author");
const String BooksDB::DATABASE_NAME = String("BOOKS.db");
const Int32  BooksDB::DATABASE_VERSION = 1;
const String BooksDB::TABLE_NAME = String("books_table");

BooksDB::BooksDB(
    /* [in] */ IContext* context)
{
    SQLiteOpenHelper::Init(context, DATABASE_NAME, NULL, DATABASE_VERSION);
}

ECode BooksDB::OnCreate(
    /* [in] */ ISQLiteDatabase* db)
{
    StringBuilder sb;
    sb += "CREATE TABLE ";
    sb += TABLE_NAME;
    sb += " (";
    sb += BOOK_ID;
    sb += " INTEGER primary key autoincrement, ";
    sb += BOOK_NAME;
    sb += " text, ";
    sb += BOOK_AUTHOR;
    sb += " text);";
    return db->ExecSQL(sb.ToString());
}

ECode BooksDB::OnUpgrade(
    /* [in] */ ISQLiteDatabase* db,
    /* [in] */ Int32 oldVersion,
    /* [in] */ Int32 newVersion)
{
    StringBuilder sb;
    sb += "DROP TABLE IF EXISTS ";
    sb += TABLE_NAME;
    FAIL_RETURN(db->ExecSQL(sb.ToString()))
    return OnCreate(db);
}

AutoPtr<ICursor> BooksDB::Select()
{
    AutoPtr<ISQLiteDatabase> db;
    GetReadableDatabase((ISQLiteDatabase**)&db);
    AutoPtr<ICursor> cursor;
    db->Query(TABLE_NAME, NULL, String(NULL), NULL, String(NULL), String(NULL), String(NULL), (ICursor**)&cursor);
    return cursor;
}

Int64 BooksDB::Insert(
    /* [in] */ const String& bookname,
    /* [in] */ const String& author)
{
    AutoPtr<ISQLiteDatabase> db;
    GetWritableDatabase((ISQLiteDatabase**)&db);
    AutoPtr<IContentValues> cv;
    CContentValues::New((IContentValues**)&cv);
    AutoPtr<ICharSequence> _bookname, _author;
    CStringWrapper::New(bookname, (ICharSequence**)&_bookname);
    CStringWrapper::New(author, (ICharSequence**)&_author);
    cv->PutString(BOOK_NAME, _bookname);
    cv->PutString(BOOK_AUTHOR, _author);
    Int64 row;
    FAIL_RETURN(db->Insert(TABLE_NAME, String(NULL), cv, &row))
    return row;
}

void BooksDB::Delete(
    /* [in] */ Int32 id)
{
    AutoPtr<ISQLiteDatabase> db;
    GetWritableDatabase((ISQLiteDatabase**)&db);
    StringBuilder sb;
    sb += BOOK_ID;
    sb += " = ?";
    AutoPtr<ArrayOf<String> > whereValue = ArrayOf<String>::Alloc(1);
    (*whereValue)[0] = StringUtils::ToString(id);
    Int32 tmp;
    db->Delete(TABLE_NAME, sb.ToString(), whereValue, &tmp);
}

void BooksDB::Update(
    /* [in] */ Int32 id,
    /* [in] */ const String& bookname,
    /* [in] */ const String& author)
{
    AutoPtr<ISQLiteDatabase> db;
    GetWritableDatabase((ISQLiteDatabase**)&db);
    StringBuilder sb;
    sb += BOOK_ID;
    sb += " = ?";
    AutoPtr<ArrayOf<String> > whereValue = ArrayOf<String>::Alloc(1);
    (*whereValue)[0] = StringUtils::ToString(id);

    AutoPtr<IContentValues> cv;
    CContentValues::New((IContentValues**)&cv);
    AutoPtr<ICharSequence> _bookname, _author;
    CStringWrapper::New(bookname, (ICharSequence**)&_bookname);
    CStringWrapper::New(author, (ICharSequence**)&_author);
    cv->PutString(BOOK_NAME, _bookname);
    cv->PutString(BOOK_AUTHOR, _author);
    Int32 tmp;
    db->Update(TABLE_NAME, cv, sb.ToString(), whereValue, &tmp);
}

}  //DatabaseDemo
}  //DevSamples
}  //Droid
}  //Elastos
