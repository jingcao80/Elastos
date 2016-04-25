
#include "CActivityOne.h"
#include "CDatabaseOpenHelper.h"
#include "Utils.h"
#include "R.h"
#include <Elastos.Droid.App.h>
#include <Elastos.Droid.Content.h>
#include <Elastos.Droid.Os.h>
#include <Elastos.Droid.View.h>
#include <Elastos.Droid.Widget.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::View::EIID_IViewOnClickListener;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace DevSamples {
namespace ContentProviderDemo {

static const String TAG("ContentProviderDemo::CActivityOne");

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
        case R::id::Create:
            mHost->Create();
            break;

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

    view = FindViewById(R::id::Create);
    view->SetOnClickListener(l.Get());

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

ECode CActivityOne::Create()
{
    if (mDBOpenHelper == NULL ) {
        ECode ec = CDatabaseOpenHelper::New(this, (ISQLiteOpenHelper**)&mDBOpenHelper);
        Logger::I(TAG, "create CDatabaseOpenHelper, ec=%08x", ec);
    }

    return NOERROR;
}

ECode CActivityOne::Add()
{
    // AutoPtr<IContentValues> values;
    // CContentValues::New((IContentValues**)&values);
    // values->Put(Utils::USERNAME, String("kesalin");
    // values->Put(Utils::SEX, String("男"));
    // values->Put(Utils::EMAIL, String("kesalin@gmail.com"));

    // AutoPtr<IContentResolver> contentResolver;
    // GetContentResolver((IContentResolver**)&contentResolver);
    // AutoPtr<IUri> uri;
    // contentResolver->Insert(Utils::CONTENT_URI, values, (IUri**)&uri);

    if (mDBOpenHelper != NULL) {
        CDatabaseOpenHelper* doh = (CDatabaseOpenHelper*)mDBOpenHelper.Get();
        doh->Add(String("kesalin"), String("男"), String("kesalin@gmail.com"));
        doh->Add(String("Elastos"), String("男"), String("elastos@kortide.com"));
    }
    return NOERROR;
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
    Logger::I(TAG, " == Query: %s, ec=%08x", TO_CSTR(cursor), ec);
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

            Logger::I(TAG, " >> item %d: [%s, %s, %s]", ++i, name.string(), sex.string(), email.string());
        }

        StartManagingCursor(cursor);  //查找后关闭游标
    }

    return NOERROR;
}

ECode CActivityOne::Update()
{
    return NOERROR;
}

ECode CActivityOne::Delete()
{
    return NOERROR;
}


} // namespace ContentProviderDemo
} // namespace DevSamples
} // namespace Elastos
