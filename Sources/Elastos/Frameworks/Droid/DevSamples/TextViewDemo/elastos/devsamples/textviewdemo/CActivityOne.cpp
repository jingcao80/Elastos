
#include "CActivityOne.h"
#include "R.h"
#include <Elastos.Droid.App.h>
#include <Elastos.Droid.Content.h>
#include <Elastos.Droid.Os.h>
#include <Elastos.Droid.View.h>
#include <Elastos.Droid.Widget.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::App::IIActivityManager;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Os::IServiceManager;
using Elastos::Droid::Os::CServiceManager;
using Elastos::Droid::View::EIID_IViewOnClickListener;
using Elastos::Droid::Widget::ITextView;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace DevSamples {
namespace TextViewDemo {

static const String DBG_TAG("CActivityOne");

//=======================================================================
// CActivityOne
//=======================================================================

CAR_OBJECT_IMPL(CActivityOne)

ECode CActivityOne::constructor()
{
    Logger::I(DBG_TAG, " >> constructor()");
    return Activity::constructor();
}

ECode CActivityOne::OnCreate(
    /* [in] */ IBundle* savedInstanceState)
{
    Logger::I(DBG_TAG, " >> OnCreate()");
    Activity::OnCreate(savedInstanceState);
    SetContentView(R::layout::main);

    AutoPtr<IView> view = FindViewById(R::id::TextView);
    assert(view != NULL);
    ITextView* textView = ITextView::Probe(view);

    return NOERROR;
}

ECode CActivityOne::OnStart()
{
    Logger::I(DBG_TAG, " >> OnStart()");
    return Activity::OnStart();
}

ECode CActivityOne::OnResume()
{
    Logger::I(DBG_TAG, " >> OnResume()");
    return Activity::OnResume();
}

ECode CActivityOne::OnPause()
{
    Logger::I(DBG_TAG, " >> OnPause()");
    return Activity::OnPause();
}

ECode CActivityOne::OnStop()
{
    Logger::I(DBG_TAG, " >> OnStop()");
    return Activity::OnStop();
}

ECode CActivityOne::OnDestroy()
{
    Logger::I(DBG_TAG, " >> OnDestroy()");
    return Activity::OnDestroy();
}

ECode CActivityOne::OnActivityResult(
    /* [in] */ Int32 requestCode,
    /* [in] */ Int32 resultCode,
    /* [in] */ IIntent *data)
{
    Logger::I(DBG_TAG, " >> OnActivityResult()");
    return Activity::OnActivityResult(requestCode, resultCode, data);
}

} // namespace TextViewDemo
} // namespace DevSamples
} // namespace Elastos
