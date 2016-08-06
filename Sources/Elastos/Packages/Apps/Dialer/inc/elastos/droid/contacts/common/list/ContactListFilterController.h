
#ifndef __ELASTOS_DROID_CONTACTS_COMMON_LIST_CONTACTLISTFILTERCONTROLLER_H__
#define __ELASTOS_DROID_CONTACTS_COMMON_LIST_CONTACTLISTFILTERCONTROLLER_H__

#include "_Elastos.Droid.Dialer.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Content::IContext;
using Elastos::Core::Object;

namespace Elastos {
namespace Droid {
namespace Contacts {
namespace Common {
namespace List {

/**
 * Constructs shortcut intents.
 */
class ContactListFilterController
    : public Object
    , public IContactListFilterController
{
public:
    CAR_INTERFACE_DECL()

    static CARAPI_(AutoPtr<IContactListFilterController>) GetInstance(
        /* [in] */ IContext* context);
};

} // List
} // Common
} // Contacts
} // Apps
} // Elastos

#endif //__ELASTOS_DROID_CONTACTS_COMMON_LIST_CONTACTLISTFILTERCONTROLLER_H__
