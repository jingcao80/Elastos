
#include "CActivityOne.h"
#include "R.h"
#include <elautoptr.h>
#include <elastos/core/StringUtils.h>
#include <elastos/utility/logging/Slogger.h>


using Elastos::Utility::Logging::Slogger;
using Elastos::Core::StringUtils;
using Elastos::Core::ICharSequence;
using Elastos::Core::CStringWrapper;
using Elastos::Core::IInteger32;
using Elastos::Core::CInteger32;
using Elastos::Droid::View::EIID_IViewOnClickListener;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::EIID_IContext;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Content::CContentValues;
using Elastos::Droid::Content::ISharedPreferencesEditor;
using Elastos::Droid::Os::CEnvironment;
using Elastos::Droid::Os::IEnvironment;
using Elastos::Droid::Widget::IAdapter;
using Elastos::Droid::Widget::CSimpleAdapter;
using Elastos::Droid::Utility::CParcelableObjectContainer;
using Elastos::Utility::CObjectStringMap;
using Elastos::IO::CFile;

namespace Elastos {
namespace Droid {
namespace DevSamples {
namespace SavingData {

String FeedEntry::TABLE_NAME("entry");
String FeedEntry::_ID("entry");
String FeedEntry::COLUMN_NAME_ENTRY_ID("entryid");
String FeedEntry::COLUMN_NAME_TITLE("title");
String FeedEntry::COLUMN_NAME_SUBTITLE("subtitle");
String CActivityOne::TEXT_TYPE(" TEXT");
String CActivityOne::COMMA_SEP(",");
String CActivityOne::SQL_CREATE_ENTRIES =
        String("CREATE TABLE ") + FeedEntry::TABLE_NAME + String(" (") +
        FeedEntry::_ID + String(" INTEGER PRIMARY KEY,") +
        FeedEntry::COLUMN_NAME_ENTRY_ID + TEXT_TYPE + COMMA_SEP +
        FeedEntry::COLUMN_NAME_TITLE + TEXT_TYPE + String(" )");

String CActivityOne::SQL_DELETE_ENTRIES =
        String("DROP TABLE IF EXISTS ") + FeedEntry::TABLE_NAME;

/* Checks if external storage is available for read and write */
Boolean IsExternalStorageWritable()
{
    AutoPtr<IEnvironment> env;
    CEnvironment::AcquireSingleton((IEnvironment**)&env);
    String state;
    env->GetExternalStorageState(&state);
    String MEDIA_MOUNTED("mounted");
    if (MEDIA_MOUNTED.Equals(state)) {
        return TRUE;
    }
    return FALSE;
}

CActivityOne::MyListener::MyListener(
    /* [in] */ CActivityOne* host) :
    mHost(host),
    mRef(0)
{
}

PInterface CActivityOne::MyListener::Probe(
    /* [in]  */ REIID riid)
{
    if (riid == EIID_IInterface) {
        return (PInterface)(IViewOnClickListener*)this;
    }
    else if (riid == EIID_IViewOnClickListener) {
        return (IViewOnClickListener*)this;
    }

    return NULL;
}

UInt32 CActivityOne::MyListener::AddRef()
{
    return ElRefBase::AddRef();
}

UInt32 CActivityOne::MyListener::Release()
{
    return ElRefBase::Release();
}

ECode CActivityOne::MyListener::GetInterfaceID(
    /* [in] */ IInterface *pObject,
    /* [out] */ InterfaceID *pIID)
{
    if (pIID == NULL) {
        return E_INVALID_ARGUMENT;
    }

    if (pObject == (IInterface*)(IViewOnClickListener*)this) {
        *pIID = EIID_IViewOnClickListener;
    }
    else {
        return E_INVALID_ARGUMENT;
    }
    return NOERROR;
}

ECode CActivityOne::MyListener::OnClick(
    /* [in] */ IView* v)
{
    Int32 id;
    v->GetId(&id);
    if (id == R::id::adddata) {
        AutoPtr<ISharedPreferences> shared;
        //1. SharedPreferences IContext::MODE_PRIVATE
        String sharedfile("com.elastos.SavingData.key");
        mHost->GetSharedPreferences(sharedfile, IContext::MODE_PRIVATE, (ISharedPreferences**)&shared);
        assert(shared != NULL);

        Int32 value = 0;
        String key("com.elastos.SavingData.key");
        shared->GetInt32(key, -1, &value);

        // Create a new map of values, where column names are the keys
        AutoPtr<IContentValues> values;
        CContentValues::New((IContentValues**)&values);

        AutoPtr<ICharSequence> csObj0, csObj;
        CStringWrapper::New(StringUtils::Int32ToString(value), (ICharSequence**)&csObj0);
        values->PutString(FeedEntry::COLUMN_NAME_ENTRY_ID, csObj0);
        CStringWrapper::New(String("Title") + StringUtils::Int32ToString(value), (ICharSequence**)&csObj);
        values->PutString(FeedEntry::COLUMN_NAME_TITLE, csObj);

        assert(mHost->mDbHelper != NULL);
        AutoPtr<ISQLiteDatabase> db;
        mHost->mDbHelper->GetWritableDatabase((ISQLiteDatabase**)&db);
        // Insert the new row, returning the primary key value of the new row
        Int64 newRowId = 0;
        db->Insert(FeedEntry::TABLE_NAME, String(NULL), values, &newRowId);

        AutoPtr<ICursor> cursor;
        assert(db != NULL);
        db->Query(FeedEntry::TABLE_NAME, NULL, String(NULL),
                NULL, String(NULL), String(NULL), String(NULL), (ICursor**)&cursor);

        AutoPtr<ISimpleAdapter> adapter = mHost->GetSimpleAdapter(cursor);
        mHost->mListView->SetAdapter(IAdapter::Probe(adapter));

        //Write new data.
        AutoPtr<ISharedPreferencesEditor> editor;
        shared->Edit((ISharedPreferencesEditor**)&editor);
        assert(editor != NULL);
        editor->PutInt32(key, value + 1);
        Boolean succeded = FALSE;
        editor->Commit(&succeded);
    }
    else if (id == R::id::deldata) {
        AutoPtr<ICursor> c;

        assert(mHost->mDbHelper != NULL);
        AutoPtr<ISQLiteDatabase> db;
        mHost->mDbHelper->GetWritableDatabase((ISQLiteDatabase**)&db);
        db->Query(FeedEntry::TABLE_NAME, NULL, String(NULL),
                NULL, String(NULL), String(NULL), String(NULL), (ICursor**)&c);

        Int32 count = 0;
        Boolean toFirst = FALSE;
        if (c != NULL && (c->GetCount(&count), count) > 0 && (c->MoveToFirst(&toFirst), toFirst)) {
            Int32 id;
            String value;
            c->GetColumnIndex(FeedEntry::COLUMN_NAME_ENTRY_ID, &id);
            c->GetString(id, &value);
            AutoPtr<ICharSequence> integerId;
            CStringWrapper::New(value, (ICharSequence**)&integerId);
            String idString;
            integerId->ToString(&idString);
            AutoPtr< ArrayOf<String> > dArray = ArrayOf<String>::Alloc(1);
            (*dArray)[0] = idString;
            Int32 dNum;
            db->Delete(FeedEntry::TABLE_NAME, String("entryid=?"),
                    dArray, &dNum);

            c = NULL;
            db->Query(FeedEntry::TABLE_NAME, NULL, String(NULL),
                    NULL, String(NULL), String(NULL), String(NULL), (ICursor**)&c);

            AutoPtr<ISimpleAdapter> adapter = mHost->GetSimpleAdapter(c);
            mHost->mListView->SetAdapter(IAdapter::Probe(adapter));

            c->Close();
        }
    }

    return NOERROR;
}

ECode CActivityOne::OnCreate(
    /* [in] */ IBundle* savedInstanceState)
{
    Activity::OnCreate(savedInstanceState);
    SetContentView(R::layout::main);
    AutoPtr<MyListener> l = new MyListener(this);
    IViewOnClickListener* clickListener = (IViewOnClickListener*)(l->Probe(EIID_IViewOnClickListener));

    mListView = IListView::Probe(FindViewById(R::id::sqlListView));
    assert(mListView != NULL);

    mAddData = FindViewById(R::id::adddata);
    assert(mAddData != NULL);
    mAddData->SetOnClickListener(clickListener);

    mDelData = FindViewById(R::id::deldata);
    assert(mDelData != NULL);
    mDelData->SetOnClickListener(clickListener);

    mValueText = ITextView::Probe(FindViewById(R::id::sharedtext));
    assert(mValueText != NULL);

    mInternalText = ITextView::Probe(FindViewById(R::id::internaltext));
    assert(mInternalText != NULL);

    mExternalText = ITextView::Probe(FindViewById(R::id::externaltext));
    assert(mExternalText != NULL);

    //database
    mDbHelper = new FeedReaderDbHelper(THIS_PROBE(IContext));

    return NOERROR;
}

ECode CActivityOne::OnStart()
{
    // TODO: Add your code here
    return NOERROR;
}

ECode CActivityOne::OnResume()
{
    AutoPtr<ISharedPreferences> shared;
    //1. SharedPreferences IContext::MODE_PRIVATE
    String sharedfile("com.elastos.SavingData.key");
    GetSharedPreferences(sharedfile, IContext::MODE_PRIVATE, (ISharedPreferences**)&shared);
    assert(shared != NULL);

    Int32 value = 0;
    String key("com.elastos.SavingData.key");
    shared->GetInt32(key, -1, &value);
    assert(mValueText != NULL);
    AutoPtr<ICharSequence> txt;
    CStringWrapper::New(String("        Current Shared Value is: ") + StringUtils::Int32ToString(value), (ICharSequence**)&txt);
    mValueText->SetText(txt);

    //Write new data.
    // AutoPtr<ISharedPreferencesEditor> editor;
    // shared->Edit((ISharedPreferencesEditor**)&editor);
    // assert(editor != NULL);
    // editor->PutInt32(key, value + 1);
    // Boolean succeded = FALSE;
    // editor->Commit(&succeded);

    //2. Create internal file.
    AutoPtr<IFile> dir;
    GetFilesDir((IFile**)&dir);
    assert(dir != NULL);

    AutoPtr<IFile> file;
    String filename("savingfile.xml");
    CFile::New(dir, filename, (IFile**)&file);
    assert(file != NULL);
    String strValue;
    file->GetAbsolutePath(&strValue);
    txt = NULL;
    CStringWrapper::New(String("[2]. Path: ") + strValue, (ICharSequence**)&txt);
    mInternalText->SetText(txt);

    //3. Create external file.
    txt = NULL;
    if (IsExternalStorageWritable()) {
        strValue = "[3]. The external storage is available";
    }
    else {
        strValue = "[3]. The external storage is not available";
    }
    CStringWrapper::New(strValue, (ICharSequence**)&txt);
    mExternalText->SetText(txt);

    //4. SQL
    assert(mDbHelper != NULL);
    AutoPtr<ISQLiteDatabase> db;
    mDbHelper->GetWritableDatabase((ISQLiteDatabase**)&db);
    AutoPtr<ICursor> cursor;
    assert(db != NULL);
    db->Query(FeedEntry::TABLE_NAME, NULL, String(NULL),
            NULL, String(NULL), String(NULL), String(NULL), (ICursor**)&cursor);

    AutoPtr<ISimpleAdapter> adapter = GetSimpleAdapter(cursor);
    mListView->SetAdapter(IAdapter::Probe(adapter));

    return NOERROR;
}

ECode CActivityOne::OnPause()
{
    // TODO: Add your code here
    return NOERROR;
}

ECode CActivityOne::OnStop()
{
    // TODO: Add your code here
    return NOERROR;
}

ECode CActivityOne::OnDestroy()
{
    // TODO: Add your code here
    return NOERROR;
}

ECode CActivityOne::OnActivityResult(
    /* [in] */ Int32 requestCode,
    /* [in] */ Int32 resultCode,
    /* [in] */ IIntent *data)
{
    return NOERROR;
}

AutoPtr<ISimpleAdapter> CActivityOne::GetSimpleAdapter(
    /* [in] */ ICursor* cursor)
{
    AutoPtr<IObjectContainer> dataList;
    if (cursor != NULL) {
        CParcelableObjectContainer::New((IObjectContainer**)&dataList);
        Boolean success = FALSE;
        String value;
        Int32 id = 0;
        AutoPtr<IObjectStringMap> map;
        AutoPtr<ICharSequence> cs;

        while(cursor->MoveToNext(&success),success) {
            map = NULL;
            CObjectStringMap::New((IObjectStringMap**)&map);

            cursor->GetColumnIndex(FeedEntry::COLUMN_NAME_ENTRY_ID, &id);
            cursor->GetString(id, &value);
Slogger::D("CActivityOne", "GetSimpleAdapter, id=[%d], value=[%s]=========1", id, value.string());
            cs = NULL;
            CStringWrapper::New(value, (ICharSequence**)&cs);
            map->Put(FeedEntry::COLUMN_NAME_ENTRY_ID, cs);
            cursor->GetColumnIndex(FeedEntry::COLUMN_NAME_TITLE, &id);
            cursor->GetString(id, &value);
Slogger::D("CActivityOne", "GetSimpleAdapter, value=[%s]=========2", value.string());
            cs = NULL;
            CStringWrapper::New(value, (ICharSequence**)&cs);
            map->Put(FeedEntry::COLUMN_NAME_TITLE, cs);

            dataList->Add(map);
        }
Slogger::D("CActivityOne", "GetSimpleAdapter, success=[%d]=========4", success);
        cursor->Close();
    }

    AutoPtr<ISimpleAdapter> simpleAdapter;
    if (dataList != NULL) {
        AutoPtr<ArrayOf<String> > from = ArrayOf<String>::Alloc(2);
        (*from)[0] = FeedEntry::COLUMN_NAME_ENTRY_ID;
        (*from)[1] = FeedEntry::COLUMN_NAME_TITLE;
        AutoPtr<ArrayOf<Int32> > to = ArrayOf<Int32>::Alloc(2);
        (*to)[0] = R::id::dataid;
        (*to)[1] = R::id::datatitle;

        CSimpleAdapter::New(this, dataList, R::layout::list_item, from, to, (ISimpleAdapter**)&simpleAdapter);
    }

    return simpleAdapter;
}

const Int32 FeedReaderDbHelper::DATABASE_VERSION = 1;
String FeedReaderDbHelper::DATABASE_NAME = String("FeedReader.db");
FeedReaderDbHelper::FeedReaderDbHelper(
    /* [in] */ IContext* context)
{
    SQLiteOpenHelper::Init(context, DATABASE_NAME, NULL, DATABASE_VERSION);
}

ECode FeedReaderDbHelper::OnCreate(
    /* [in] */ ISQLiteDatabase* db)
{
Slogger::D("CActivityOne", "FeedReaderDbHelper=====OnCreate, value=[%s]=========2", CActivityOne::SQL_CREATE_ENTRIES.string());
    return db->ExecSQL(CActivityOne::SQL_CREATE_ENTRIES);
}

ECode FeedReaderDbHelper::OnUpgrade(
    /* [in] */ ISQLiteDatabase* db,
    /* [in] */ Int32 oldVersion,
    /* [in] */ Int32 newVersion)
{
    // This database is only a cache for online data, so its upgrade policy is
    // to simply to discard the data and start over
    db->ExecSQL(CActivityOne::SQL_DELETE_ENTRIES);
    return OnCreate(db);
}

ECode FeedReaderDbHelper::OnDowngrade(
    /* [in] */ ISQLiteDatabase* db,
    /* [in] */ Int32 oldVersion,
    /* [in] */ Int32 newVersion)
{
    return OnUpgrade(db, oldVersion, newVersion);
}

} // namespace SavingData
} // namespace DevSamples
} // namespace Droid
} // namespace Elastos
