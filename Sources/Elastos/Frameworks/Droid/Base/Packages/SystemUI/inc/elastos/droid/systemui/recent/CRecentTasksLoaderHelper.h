#ifndef __ELASTOS_DROID_SYSTEMUI_RECENT_CRECENTTASKSLOADERHELPER_H__
#define __ELASTOS_DROID_SYSTEMUI_RECENT_CRECENTTASKSLOADERHELPER_H__

#include "_Elastos_Droid_SystemUI_Recent_CRecentTasksLoaderHelper.h"

using Elastos::Droid::SystemUI::Recent::IRecentTasksLoader;
using Elastos::Droid::Content::IContext;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace Recent {

CarClass(CRecentTasksLoaderHelper)
{
public:
    GetInstance(
        /* [in] */ IContext* context,
        /* [out] */ IRecentTasksLoader** loader);
};

}// namespace Recent
}// namespace SystemUI
}// namespace Droid
}// namespace Elastos

#endif //__ELASTOS_DROID_SYSTEMUI_RECENT_CRECENTTASKSLOADERHELPER_H__
