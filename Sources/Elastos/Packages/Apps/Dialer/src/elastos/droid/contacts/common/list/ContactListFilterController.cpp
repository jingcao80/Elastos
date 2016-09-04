
#include "elastos/droid/contacts/common/list/ContactListFilterController.h"

namespace Elastos {
namespace Droid {
namespace Contacts {
namespace Common {
namespace List {

CAR_INTERFACE_IMPL(ContactListFilterController, Object, IContactListFilterController)

AutoPtr<IContactListFilterController> ContactListFilterController::GetInstance(
    /* [in] */ IContext* context)
{
    assert(0);
    return NULL;
}

} // List
} // Common
} // Contacts
} // Droid
} // Elastos
