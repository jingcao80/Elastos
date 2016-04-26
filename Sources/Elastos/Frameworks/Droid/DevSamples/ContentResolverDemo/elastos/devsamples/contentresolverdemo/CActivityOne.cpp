
#include "CActivityOne.h"
#include "Utils.h"
#include "R.h"
#include <Elastos.Droid.App.h>
#include <Elastos.Droid.Content.h>
#include <Elastos.Droid.Os.h>
#include <Elastos.Droid.View.h>
#include <Elastos.Droid.Widget.h>
#include <Elastos.Droid.Net.h>
#include <elastos/core/StringBuilder.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Net::IUri;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Content::IContentUris;
using Elastos::Droid::Content::CContentUris;
using Elastos::Droid::Content::IContentValues;
using Elastos::Droid::Content::CContentValues;
using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::View::EIID_IViewOnClickListener;
using Elastos::Core::StringBuilder;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace DevSamples {
namespace ContentResolverDemo {

static const String TAG("ContentResolverDemo::CActivityOne");

//=======================================================================
// MyListener
//=======================================================================

CAR_INTERFACE_IMPL(CActivityOne::MyListener, Object, IViewOnClickListener)

CActivityOne::MyListener::MyListener(
    /* [in] */ CActivityOne* host)
    : mHost(host)
{
}

ECode CActivityOne::MyListener::OnClick(
    /* [in] */ IView* v)
{
    Int32 id;
    v->GetId(&id);

    switch(id) {

        case R::id::Add:
            mHost->Add();
            break;

        case R::id::Query:
            mHost->Query();
            break;

        case R::id::Update:
            mHost->Update();
            break;

        case R::id::Delete:
            mHost->Delete();
            break;
    }
    return NOERROR;
}

//=======================================================================
// CActivityOne
//=======================================================================

CAR_OBJECT_IMPL(CActivityOne)

ECode CActivityOne::constructor()
{
    Logger::I(TAG, " >> constructor()");
    return Activity::constructor();
}

ECode CActivityOne::OnCreate(
    /* [in] */ IBundle* savedInstanceState)
{
    Logger::I(TAG, " >> OnCreate()");
    Activity::OnCreate(savedInstanceState);
    SetContentView(R::layout::main);

    AutoPtr<MyListener> l = new MyListener(this);
    AutoPtr<IView> view;

    view = FindViewById(R::id::Add);
    view->SetOnClickListener(l.Get());

    view = FindViewById(R::id::Query);
    view->SetOnClickListener(l.Get());

    view = FindViewById(R::id::Update);
    view->SetOnClickListener(l.Get());

    view = FindViewById(R::id::Delete);
    view->SetOnClickListener(l.Get());

    return NOERROR;
}

ECode CActivityOne::OnStart()
{
    Logger::I(TAG, " >> OnStart()");
    return Activity::OnStart();
}

ECode CActivityOne::OnResume()
{
    Logger::I(TAG, " >> OnResume()");
    return Activity::OnResume();
}

ECode CActivityOne::OnPause()
{
    Logger::I(TAG, " >> OnPause()");
    return Activity::OnPause();
}

ECode CActivityOne::OnStop()
{
    Logger::I(TAG, " >> OnStop()");
    return Activity::OnStop();
}

ECode CActivityOne::OnDestroy()
{
    Logger::I(TAG, " >> OnDestroy()");
    return Activity::OnDestroy();
}

ECode CActivityOne::OnActivityResult(
    /* [in] */ Int32 requestCode,
    /* [in] */ Int32 resultCode,
    /* [in] */ IIntent *data)
{
    Logger::I(TAG, " >> OnActivityResult()");
    return Activity::OnActivityResult(requestCode, resultCode, data);
}

//====================================================================================


ECode CActivityOne::Add()
{
    AutoPtr<IContentValues> values;
    CContentValues::New((IContentValues**)&values);
    values->Put(Utils::USERNAME, String("kesalin"));
    values->Put(Utils::SEX, String("男"));
    values->Put(Utils::EMAIL, String("kesalin@gmail.com"));

    AutoPtr<IContentResolver> contentResolver;
    GetContentResolver((IContentResolver**)&contentResolver);
    AutoPtr<IUri> uri;
    ECode ec = contentResolver->Insert(Utils::CONTENT_URI, values, (IUri**)&uri);
    Logger::I(TAG, " insert: %s, uri:%s, ec=%08x", TO_CSTR(values), TO_CSTR(uri), ec);

    values = NULL; uri = NULL;
    CContentValues::New((IContentValues**)&values);
    values->Put(Utils::USERNAME, String("Elastos"));
    values->Put(Utils::SEX, String("男"));
    values->Put(Utils::EMAIL, String("elastos@kortide.com"));
    contentResolver->Insert(Utils::CONTENT_URI, values, (IUri**)&uri);
    Logger::I(TAG, " insert: %s, result uri: %s, ec=%08x", TO_CSTR(values), TO_CSTR(uri), ec);

    values = NULL; uri = NULL;
    CContentValues::New((IContentValues**)&values);
    values->Put(Utils::USERNAME, String("Google"));
    values->Put(Utils::SEX, String("男"));
    values->Put(Utils::EMAIL, String("elastos@gmail.com"));
    contentResolver->Insert(Utils::CONTENT_URI, values, (IUri**)&uri);
    Logger::I(TAG, " insert: %s, result uri: %s, ec=%08x", TO_CSTR(values), TO_CSTR(uri), ec);

    return NOERROR;
}

Int64 CActivityOne::QueryRowId()
{
    Int64 id = -1;

    AutoPtr<IContentResolver> contentResolver;
    GetContentResolver((IContentResolver**)&contentResolver);

    String nullStr;
    AutoPtr<ArrayOf<String> > projections = ArrayOf<String>::Alloc(3);
    (*projections)[0] = Utils::TAG_ID;

    AutoPtr<ICursor> cursor;
    ECode ec = contentResolver->Query(
        Utils::CONTENT_URI, projections, nullStr, NULL, nullStr, (ICursor**)&cursor);
    Logger::I(TAG, " == QueryRowId: cursor: %s, ec=%08x", TO_CSTR(cursor), ec);
    if (cursor != NULL) {
        Int32 count = 0, i = 0;
        cursor->GetCount(&count);
        Boolean bval;
        while (cursor->MoveToNext(&bval), bval) {
            Int32 index;
            cursor->GetColumnIndex(Utils::TAG_ID, &index);
            cursor->GetInt64(index, &id);
            break;
        }

        StartManagingCursor(cursor);  //查找后关闭游标
    }

    Logger::I(TAG, " >>>> QueryRowId: %lld", id);
    return id;
}

ECode CActivityOne::Query()
{
    AutoPtr<IContentResolver> contentResolver;
    GetContentResolver((IContentResolver**)&contentResolver);

    String nullStr;
    AutoPtr<ArrayOf<String> > projections = ArrayOf<String>::Alloc(3);
    (*projections)[0] = Utils::USERNAME;
    (*projections)[1] = Utils::SEX;
    (*projections)[2] = Utils::EMAIL;

    AutoPtr<ICursor> cursor;
    ECode ec = contentResolver->Query(
        Utils::CONTENT_URI, projections, nullStr, NULL, nullStr, (ICursor**)&cursor);
    Logger::I(TAG, " == Query: cursor: %s, ec=%08x", TO_CSTR(cursor), ec);
    if (cursor != NULL) {
        Int32 count = 0, i = 0;
        cursor->GetCount(&count);
        Logger::I(TAG, " == Query result: %d items:", count);
        Boolean bval;
        while (cursor->MoveToNext(&bval), bval) {
            Int32 index;
            String name, sex, email;

            cursor->GetColumnIndex(Utils::USERNAME, &index);
            cursor->GetString(index, &name);

            cursor->GetColumnIndex(Utils::SEX, &index);
            cursor->GetString(index, &sex);

            cursor->GetColumnIndex(Utils::EMAIL, &index);
            cursor->GetString(index, &email);

            Logger::I(TAG, "     >> item %d: [%s, %s, %s]", ++i, name.string(), sex.string(), email.string());
        }

        StartManagingCursor(cursor);  //查找后关闭游标
    }

    return NOERROR;
}

ECode CActivityOne::UpdateByRowId(
    /* [in] */ Int64 rowId)
{
    AutoPtr<IContentUris> contentUris;
    CContentUris::AcquireSingleton((IContentUris**)&contentUris);
    AutoPtr<IUri> uri;
    contentUris->WithAppendedId(Utils::CONTENT_URI, rowId, (IUri**)&uri);

    String where;
    AutoPtr<IContentValues> values;
    CContentValues::New((IContentValues**)&values);
    values->Put(Utils::USERNAME, String("update_by_rowId"));
    values->Put(Utils::EMAIL, String("kesalin-rowId@gmail.com"));

    AutoPtr<IContentResolver> contentResolver;
    GetContentResolver((IContentResolver**)&contentResolver);
    Int32 rowsAffected;
    ECode ec = contentResolver->Update(uri, values, where, NULL, &rowsAffected);
    Logger::I(TAG, " upadte %lld : %s, values: %s, rowsAffected:%d, ec=%08x",
        rowId, TO_CSTR(uri), TO_CSTR(values), rowsAffected, ec);
    return NOERROR;
}

static String GenerateWhereByName(
    /* [in] */ const String& userName)
{
    StringBuilder sb(Utils::USERNAME);
    sb += "='";
    sb += userName;
    sb += "'";
    return sb.ToString();
}

ECode CActivityOne::UpdateByName(
    /* [in] */ const String& userName)
{
    String where = GenerateWhereByName(userName);
    AutoPtr<IContentValues> values;
    CContentValues::New((IContentValues**)&values);
    values->Put(Utils::USERNAME, String("update_by_name"));
    values->Put(Utils::EMAIL, String("kesalin@gmail.com"));

    AutoPtr<IContentResolver> contentResolver;
    GetContentResolver((IContentResolver**)&contentResolver);
    Int32 rowsAffected;
    ECode ec = contentResolver->Update(Utils::CONTENT_URI, values, where, NULL, &rowsAffected);
    Logger::I(TAG, " upadte %s, values: %s, rowsAffected:%d, ec=%08x",
        userName.string(), TO_CSTR(values), rowsAffected, ec);
    return ec;
}

ECode CActivityOne::Update()
{
    Int64 id = QueryRowId();
    if (id > 0) {
        UpdateByRowId(id);
    }

    UpdateByName(String("kesalin"));

    Query();
    return NOERROR;
}

ECode CActivityOne::DeleteByName(
    /* [in] */ const String& userName)
{
    AutoPtr<IContentResolver> contentResolver;
    GetContentResolver((IContentResolver**)&contentResolver);
    StringBuilder sb(Utils::USERNAME);
    sb += "='";
    sb += userName;
    sb += "'";
    String where = sb.ToString();

    Int32 rowsAffected;
    ECode ec = contentResolver->Delete(Utils::CONTENT_URI, where, NULL, &rowsAffected);
    Logger::I(TAG, " delete: %s, rowsAffected:%d, ec=%08x", where.string(), rowsAffected, ec);
    return ec;
}

ECode CActivityOne::Delete()
{
    DeleteByName(String("kesalin"));
    DeleteByName(String("Elastos"));
    Query();

    DeleteByName(String("update_by_rowId"));
    DeleteByName(String("update_by_name"));
    DeleteByName(String("Google"));
    Query();
    return NOERROR;
}


} // namespace ContentResolverDemo
} // namespace DevSamples
} // namespace Elastos
