
#include "CActivityOne.h"
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
using Elastos::Droid::Widget::IImageView;
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
    Logger::I(TAG, "MyListener::OnClick");
    mHost->Query();
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

    AutoPtr<IView> view = FindViewById(R::id::ImageView);
    IImageView* imageView = IImageView::Probe(view);
    imageView->SetImageResource(R::drawable::transformer);
    imageView->SetScaleType(Elastos::Droid::Widget::ImageViewScaleType_FIT_CENTER);

    AutoPtr<MyListener> l = new MyListener(this);
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

ECode CActivityOne::Add()
{
    AutoPtr<IContentValues> values;
    CContentValues::New((IContentValues**)&values);
    values->Put(Utils::USERNAME, username);
    values->Put(Utils::SEX, sex);
    values->Put(Utils::EMAIL, email);

    AutoPtr<IContentResolver> contentResolver;
    GetContentResolver((IContentResolver**)&contentResolver);
    contentResolver->Insert(Utils::CONTENT_URI, values);
    return NOERROR;
}

} // namespace ContentResolverDemo
} // namespace DevSamples
} // namespace Elastos
