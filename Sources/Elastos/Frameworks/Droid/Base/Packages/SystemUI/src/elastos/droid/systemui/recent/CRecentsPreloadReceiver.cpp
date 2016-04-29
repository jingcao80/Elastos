
#include "elastos/droid/systemui/recent/CRecentsPreloadReceiver.h"
#include "elastos/droid/systemui/recent/RecentTasksLoader.h"

using Elastos::Droid::SystemUI::Recent::IRecentsActivity;
using Elastos::Droid::SystemUI::Recent::IRecentTasksLoader;
using Elastos::Droid::SystemUI::Recent::RecentTasksLoader;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace Recent {

CAR_OBJECT_IMPL(CRecentsPreloadReceiver);
ECode CRecentsPreloadReceiver::OnReceive(
    /* [in] */ IContext* context,
    /* [in] */ IIntent* intent)
{
    String action;
    intent->GetAction(&action);
    AutoPtr<IRecentTasksLoader> rtl = RecentTasksLoader::GetInstance(context);
    if (IRecentsActivity::PRELOAD_INTENT.Equals(action)) {
        rtl->PreloadRecentTasksList();
    }
    else if (IRecentsActivity::CANCEL_PRELOAD_INTENT.Equals(action)){
        rtl->CancelPreloadingRecentTasksList();
    }
    return NOERROR;
}

} // namespace Recent
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos