
#include "CActivityOne.h"
#include "R.h"
#include <Elastos.Droid.App.h>
#include <Elastos.Droid.Content.h>
#include <Elastos.Droid.Os.h>
#include <Elastos.Droid.View.h>
#include <elastos/core/CoreUtils.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::App::IIActivityManager;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Os::IServiceManager;
using Elastos::Droid::Os::CServiceManager;
using Elastos::Droid::Widget::EIID_ITabHostOnTabChangeListener;
using Elastos::Droid::Widget::IImageView;
using Elastos::Droid::Widget::ITabHost;
using Elastos::Droid::Widget::ITabSpec;
using Elastos::Core::CoreUtils;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace DevSamples {
namespace TabHostDemo {

static const String TAG("TabHostDemo::CActivityOne");

//=======================================================================
// MyListener
//=======================================================================

CAR_INTERFACE_IMPL(CActivityOne::MyListener, Object, ITabHostOnTabChangeListener)

CActivityOne::MyListener::MyListener(
    /* [in] */ CActivityOne* host)
    : mHost(host)
{
}

ECode CActivityOne::MyListener::OnTabChanged(
    /* [in] */ const String& tabId)
{
    Logger::I(TAG, "MyListener::OnTabChanged %s", tabId.string());
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
    SetContentView(R::layout::tabhost);

    AutoPtr<ITabHost> tabHost;
    GetTabHost((ITabHost**)&tabHost);

    AutoPtr<ITabSpec> page1;
    tabHost->NewTabSpec(String("tab1"), (ITabSpec**)&page1);
    page1->SetIndicator(CoreUtils::Convert("叶子"));
    page1->SetContent(R::id::leaf);
    tabHost->AddTab(page1);

    AutoPtr<ITabSpec> page2;
    tabHost->NewTabSpec(String("tab2"), (ITabSpec**)&page2);
    page2->SetIndicator(CoreUtils::Convert("彩叶"));
    page2->SetContent(R::id::colorleaf);
    tabHost->AddTab(page2);

    AutoPtr<ITabSpec> page3;
    tabHost->NewTabSpec(String("tab3"), (ITabSpec**)&page3);
    page3->SetIndicator(CoreUtils::Convert("碟子"));
    page3->SetContent(R::id::cupple);
    tabHost->AddTab(page3);

    AutoPtr<ITabHostOnTabChangeListener> listener = new MyListener(this);
    tabHost->SetOnTabChangedListener(listener);
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

} // namespace TabHostDemo
} // namespace DevSamples
} // namespace Elastos
