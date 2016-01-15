#include "elastos/droid/systemui/recent/CRecentTasksLoaderHelper.h"
#include "elastos/droid/systemui/recent/CRecentTasksLoader.h"

using Elastos::Droid::SystemUI::Recent::CRecentTasksLoader;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace Recent {

ECode CRecentTasksLoaderHelper::GetInstance(
    /* [in] */ IContext* context,
    /* [out] */ IRecentTasksLoader** loader)
{
    VALIDATE_NOT_NULL(loader);
    AutoPtr<IRecentTasksLoader> l = CRecentTasksLoader::GetInstance(context);
    *loader = l;
    REFCOUNT_ADD(*loader);
    return NOERROR;
}

}// namespace Recent
}// namespace SystemUI
}// namespace Droid
}// namespace Elastos
