
#include "CDatabaseHelperManager.h"

namespace Elastos{
namespace Apps{
namespace DialerBinder {

CAR_INTERFACE_IMPL(CDatabaseHelperManager, Singleton, IDatabaseHelperManager)

CAR_SINGLETON_IMPL(CDatabaseHelperManager)

ECode CDatabaseHelperManager::GetDatabaseHelper(
    /* [in] */ IContext* context,
    /* [out] */ IDialerDatabaseHelper** helper)
{
    VALIDATE_NOT_NULL(helper);
    AutoPtr<IDialerDatabaseHelper> result = GetDatabaseHelper(context);
    *helper = result;
    REFCOUNT_ADD(*helper)
    return NOERROR;
}

AutoPtr<IDialerDatabaseHelper> CDatabaseHelperManager::GetDatabaseHelper(
    /* [in] */ IContext* context)
{
    return DialerDatabaseHelper::GetInstance(context);
}

} // DialerBinder
} // Apps
} // Elastos
