
#include "CActivityOne.h"
#include "R.h"
#include "Elastos.Droid.Widget.h"
#include <elastos/core/CoreUtils.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::View::EIID_IViewOnClickListener;
using Elastos::Droid::Widget::ITextView;
using Elastos::Core::CoreUtils;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace DevSamples {
namespace ActionBar {

// CAR_INTERFACE_IMPL(CActivityOne::MyListener, Object, IViewOnClickListener);
// CActivityOne::MyListener::MyListener(
//     /* [in] */ CActivityOne* host) :
//     mHost(host)
// {
// }

// ECode CActivityOne::MyListener::OnClick(
//     /* [in] */ IView* v)
// {
//     return NOERROR;
// }

const String CActivityOne::TAG("ActionBar.CActivityOne");
CAR_OBJECT_IMPL(CActivityOne);
ECode CActivityOne::constructor()
{
    return Activity::constructor();
}

ECode CActivityOne::OnCreate(
    /* [in] */ IBundle* savedInstanceState)
{
    Logger::D(TAG, " >> OnCreate");
    Activity::OnCreate(savedInstanceState);
    SetContentView(R::layout::main);

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

ECode CActivityOne::OnCreateOptionsMenu(
    /* [in] */ IMenu* menu,
    /* [out] */ Boolean* allowToShow)
{
    Logger::D(TAG, "OnCreateOptionsMenu=======================1");
    // Inflate the menu items for use in the action bar
    AutoPtr<IMenuInflater> inflater;
    GetMenuInflater((IMenuInflater**)&inflater);
    inflater->Inflate(R::menu::action_menu, menu);
    return Activity::OnCreateOptionsMenu(menu, allowToShow);
}

ECode CActivityOne::OnOptionsItemSelected(
    /* [in] */ IMenuItem* item,
    /* [out] */ Boolean* result)
{
    Logger::I(TAG, " >> OnOptionsItemSelected()");
    VALIDATE_NOT_NULL(result)
    AutoPtr<IView> view = FindViewById(R::id::showtext);
    ITextView* tv = ITextView::Probe(view);

    Int32 itemId;
    item->GetItemId(&itemId);
    switch(itemId){
    case R::id::action_share:
        tv->SetText(CoreUtils::Convert("[Share]"));
        break;
    case R::id::action_search:
        tv->SetText(CoreUtils::Convert("[Search]"));
        break;
    case R::id::action_settings:
        tv->SetText(CoreUtils::Convert("[Settings]"));
        break;
    case R::id::action_help:
        tv->SetText(CoreUtils::Convert("[Help]"));
        break;
    }

    return Activity::OnOptionsItemSelected(item, result);

}

ECode CActivityOne::OnActivityResult(
    /* [in] */ Int32 requestCode,
    /* [in] */ Int32 resultCode,
    /* [in] */ IIntent *data)
{
    return NOERROR;
}

} // namespace ActionBar
} // namespace DevSamples
} // namespace Elastos
