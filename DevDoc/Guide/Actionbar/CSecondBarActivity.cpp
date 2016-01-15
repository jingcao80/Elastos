
#include "CSecondBarActivity.h"
#include "R.h"
#include <elastos/Logger.h>
#include <elastos/StringUtils.h>

using Elastos::Core::StringUtils;
using Elastos::Utility::Logging::Logger;
using Elastos::Droid::App::IActionBar;

namespace Elastos {
namespace Droid {
namespace DevSamples {
namespace Actionbar {

const String CSecondBarActivity::TAG("CSecondBarActivity");

ECode CSecondBarActivity::OnCreate(
    /* [in] */ IBundle* savedInstanceState)
{
    Activity::OnCreate(savedInstanceState);
    SetContentView(R::layout::activity_second_bar);

    Logger::D(TAG,"CSecondBarActivity::OnCreate\n");

    return NOERROR;
}

ECode CSecondBarActivity::OnCreateOptionsMenu(
    /* [in] */ IMenu* menu,
    /* [out] */ Boolean* allowToShow)
{
	VALIDATE_NOT_NULL(allowToShow);
    AutoPtr<IMenuInflater> menuFlater;
    GetMenuInflater((IMenuInflater**)&menuFlater);
    menuFlater->Inflate(R::menu::second_bar, menu);
    Activity::OnCreateOptionsMenu(menu, allowToShow);

    Logger::D(TAG, "menu return:%s", StringUtils::BooleanToString(*allowToShow).string());

    return NOERROR;
}

ECode CSecondBarActivity::OnOptionsItemSelected(
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
        case R::id::second_action_settings:
            Logger::D(TAG, "select menu:%s", title.string());
            break;
        case Elastos::Droid::R::id::homeAsUp:
            Logger::D(TAG, "home");
            break;
        default:
            Activity::OnOptionsItemSelected(item, res);
    }

    Logger::D(TAG, "item id:%s", StringUtils::Int32ToOctalString(itemId).string());
    return NOERROR;
}

}//end namespace Actionbar
}//end namespace DevSamples
}//end namespace Droid
}//end namespace Elastos
