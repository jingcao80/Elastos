
#include "database/CDialerDatabaseHelperHelper.h"
#include "database/DialerDatabaseHelper.h"

namespace Elastos {
namespace Apps {
namespace Dialer {
namespace Database {

CAR_INTERFACE_IMPL(CDialerDatabaseHelperHelper, Singleton, IDialerDatabaseHelperHelper);

CAR_SINGLETON_IMPL(CDialerDatabaseHelperHelper);

ECode CDialerDatabaseHelperHelper::GetInstance(
    /* [in] */ IContext* context,
    /* [out] */ IDialerDatabaseHelper** helper)
{
    VALIDATE_NOT_NULL(helper);

    AutoPtr<IDialerDatabaseHelper> result = DialerDatabaseHelper::GetInstance(context);
    *helper = result;
    REFCOUNT_ADD(*helper);
    return NOERROR;
}

} // Database
} // Dialer
} // Apps
} // Elastos
