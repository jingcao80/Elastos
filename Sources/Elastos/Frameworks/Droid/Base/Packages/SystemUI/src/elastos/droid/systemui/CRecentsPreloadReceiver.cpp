
#include "CRecentsPreloadReceiver.h"

using Elastos::Droid::SystemUI::Recent::IRecentTasksLoader;
using Elastos::Droid::SystemUI::Recent::IRecentTasksLoaderHelper;
using Elastos::Droid::SystemUI::Recent::CRecentTasksLoaderHelper;
using Elastos::Droid::SystemUI::Recent::IRecentsActivity;

namespace Elastos {
namespace Droid {
namespace DevSamples {
namespace SystemUI {

ECode CRecentsPreloadReceiver::OnReceive(
    /* [in] */ IContext* context,
    /* [in] */ IIntent* intent)
{
    AutoPtr<IRecentTasksLoaderHelper> helper;
    CRecentTasksLoaderHelper::AcquireSingleton((IRecentTasksLoaderHelper**)&helper);
    AutoPtr<IRecentTasksLoader> loader;
    helper->GetInstance(context, (IRecentTasksLoader**)&loader);
    String action;
    intent->GetAction(&action);
    if (action.Equals(IRecentsActivity::PRELOAD_INTENT)) {
        return loader->PreloadRecentTasksList();
    }
    else if (action.Equals(IRecentsActivity::CANCEL_PRELOAD_INTENT)) {
        return loader->CancelPreloadingRecentTasksList();
    }
    return NOERROR;
}

}// namespace SystemUI
}// namespace DevSamples
}// namespace Droid
}// namespace Elastos
