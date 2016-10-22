
#include "elastos/droid/contacts/common/list/ContactEntry.h"
#include "Elastos.Droid.Provider.h"

using Elastos::Droid::Provider::IContactsContractPinnedPositions;

namespace Elastos {
namespace Droid {
namespace Contacts {
namespace Common {
namespace List {

const AutoPtr<ContactEntry> ContactEntry::BLANK_ENTRY = new ContactEntry();

ContactEntry::ContactEntry()
    : mId(0)
    , mPinned(IContactsContractPinnedPositions::UNPINNED)
    , mIsFavorite(FALSE)
    , mIsDefaultNumber(FALSE)
{}

} // List
} // Common
} // Contacts
} // Droid
} // Elastos
