
#include "elastos/droid/dialerbind/DatabaseHelperManager.h"

namespace Elastos {
namespace Droid {
namespace DialerBind {

AutoPtr<DialerDatabaseHelper> DatabaseHelperManager::GetDatabaseHelper(
    /* [in] */ IContext* context)
{
    return DialerDatabaseHelper::GetInstance(context);
}

} // DialerBind
} // Droid
} // Elastos
