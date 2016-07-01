
#include "CCollapser.h"
#include "Collapser.h"

namespace Elastos{
namespace Apps{
namespace Contacts {
namespace Common {

CAR_INTERFACE_IMPL(CCollapser, Singleton, ICollapser)

CAR_SINGLETON_IMPL(CCollapser)

ECode CCollapser::CollapseList(
    /* [in] */ IList* list,
    /* [in] */ IContext* context)
{
    Collapser::CollapseList(list, context);
    return NOERROR;
}

} // Common
} // Contacts
} // Apps
} // Elastos
