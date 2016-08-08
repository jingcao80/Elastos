#ifndef __ELASTOS_DROID_CONTACTS_COMMON_LIST_CONTACTENTRYLISTADAPTER_H__
#define __ELASTOS_DROID_CONTACTS_COMMON_LIST_CONTACTENTRYLISTADAPTER_H__

#include "elastos/droid/contacts/common/list/IndexerListAdapter.h"

namespace Elastos{
namespace Droid{
namespace Contacts {
namespace Common {
namespace List {

/**
 * Common base class for various contact-related lists, e.g. contact list, phone number list
 * etc.
 */
class ContactEntryListAdapter
    : public IndexerListAdapter
    , public IContactEntryListAdapter
{
public:
    CAR_INTERFACE_DECL()

    ContactEntryListAdapter();

    virtual ~ContactEntryListAdapter() {}
};

} // List
} // Common
} // Contacts
} // Droid
} // Elastos

#endif //__ELASTOS_DROID_CONTACTS_COMMON_LIST_CONTACTENTRYLISTADAPTER_H__
