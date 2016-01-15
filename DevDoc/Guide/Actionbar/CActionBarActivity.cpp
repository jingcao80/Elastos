#include "CActionBarActivity.h"
#include "CSecondBarActivity.h"
#include "R.h"
#include <elastos/Logger.h>
#include <elastos/StringUtils.h>

using Elastos::Core::StringUtils;
using Elastos::Utility::Logging::Logger;
using Elastos::Droid::App::IActionBar;
using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Content::Pm::IPackageInfo;
using Elastos::Droid::Content::Pm::IPackageManager;
using Elastos::Droid::View::IViewOnClickListener;
using Elastos::Droid::View::EIID_IViewOnClickListener;

namespace Elastos {
namespace Droid {
namespace DevSamples {
namespace Actionbar {

const String CActionBarActivity::TAG("CActionBarActivity");

CAR_INTERFACE_IMPL(CActionBarActivity::MenuItemClickListener, IViewOnClickListener)

CActionBarActivity::MenuItemClickListener::MenuItemClickListener(
    /* [in] */ CActionBarActivity* host)
    : mHost(host)
{
}

ECode CActionBarActivity::MenuItemClickListener::OnClick(
    /*[in]*/ IView* view)
{
    Int32 viewId;
    view->GetId(&viewId);
    switch(viewId){
        case R::id::secondBar:
            mHost->GoSecondActivity(view);
            break;
        case R::id::send_text: {
            AutoPtr<IIntent> intent;
            CIntent::New((IIntent**)&intent);
            intent->SetAction(IIntent::ACTION_SEND);
            intent->SetType(String("text/plain"));
            mHost->StartActivity(intent);
            Logger::D(TAG, "CActionBarActivity::sent text");
            break;
        }
    }

    return NOERROR;
}

CActionBarActivity::CActionBarActivity()
{
    mItemClickListener = new MenuItemClickListener(this);
}

ECode CActionBarActivity::OnCreate(
    /* [in] */ IBundle* savedInstanceState)
{
    Activity::OnCreate(savedInstanceState);
    SetContentView(R::layout::activity_action_bar);

    AutoPtr<IView> button = FindViewById(R::id::secondBar);
    button->SetOnClickListener(mItemClickListener);

    button = FindViewById(R::id::send_text);
    button->SetOnClickListener(mItemClickListener);

    Logger::D(TAG, "CActionBarActivity::OnCreate");

    return NOERROR;
}

ECode CActionBarActivity::goSecondActivity(
    /* [in] */ IView* view)
{
    Logger::D(TAG, "start second activity:");
    AutoPtr<IIntent> secondIntent;
    CIntent::New((IIntent**)&secondIntent);
    secondIntent->SetClassNameEx(String("Actionbar"), String("Actionbar.CSecondBarActivity"));

    if (FAILED(StartActivity(secondIntent))) {
        Logger::E(TAG, "OnClick()---StartActivity CSecondBarActivity failed!");
    }
    else {
        Logger::D(TAG, "OnClick()---StartActivity CSecondBarActivity succeeded!");
    }

    return NOERROR;
}

ECode CActionBarActivity::OnCreateOptionsMenu(
    /* [in] */ IMenu* menu,
    /* [out] */ Boolean* allowToShow)
{
	VALIDATE_NOT_NULL(allowToShow);
    AutoPtr<IMenuInflater> menuFlater;
    GetMenuInflater((IMenuInflater**)&menuFlater);
    menuFlater->Inflate(R::menu::action_bar, menu);
    Activity::OnCreateOptionsMenu(menu, allowToShow);
    Logger::D(TAG, "menu return:%s", StringUtils::BooleanToString(*allowToShow).string());

    return NOERROR;
}

ECode CActionBarActivity::OnOptionsItemSelected(
    /* [in] */ IMenuItem* item,
    /* [out] */ Boolean* res)
{
	VALIDATE_NOT_NULL(res);
    Int32 itemId = 0;
    item->GetItemId(&itemId);

    AutoPtr<ICharSequence> csq;
    item->GetTitle((ICharSequence**)&csq);

    String title("");
    csq->ToString(&title);

    switch(itemId){
        case R::id::action_settings:
            Logger::D(TAG, "select menu:%s", title.string());
            break;
        case Elastos::Droid::R::id::homeAsUp:
            Logger::D(TAG, "home");
            break;
        case R::id::action_search:
            Logger::D(TAG, "Search item clicked");
            break;
        default:
            Activity::OnOptionsItemSelected(item,res);
            break;
    }
    Logger::D(TAG, "item id:%s", StringUtils::Int32ToOctalString(itemId).string());

    return NOERROR;
}

ECode CActionBarActivity::OnStart()
{
    Activity::OnStart();
    Logger::D(TAG, "Activity onStart");
    return NOERROR;
}

ECode CActionBarActivity::OnRestart()
{
    Activity::OnRestart();
    Logger::D(TAG, "Activity onReStart");
    return NOERROR;
}

ECode CActionBarActivity::OnPause()
{
    Activity::OnPause();
    Logger::D(TAG, "Activity onPause");
    return NOERROR;
}

ECode CActionBarActivity::OnResume()
{
    Activity::OnResume();
    Logger::D(TAG, "Activity onResume");
    return NOERROR;
}

ECode CActionBarActivity::OnStop()
{
    Activity::OnStop();
    Logger::D(TAG, "Activity onStop");
    return NOERROR;
}

ECode CActionBarActivity::OnDestroy()
{
    Activity::OnDestroy();
    Logger::D(TAG, "Activity onDestory");
    return NOERROR;
}

}//end namespace Actionbar
}//end namespace DevSamples
}//end namespace Droid
}//end namespace Elastos
