
#include "ContactEntry.h"

namespace Elastos{
namespace Apps{
namespace Contacts {
namespace Common {
namespace List {

AutoPtr<IContactEntry> CreateBLANK_ENTRY()
{
    AutoPtr<IContactEntry> entry;
    CContactEntry::New((IContactEntry**)&entry);
    return entry;
}

const AutoPtr<IContactEntry> BLANK_ENTRY = CreateBLANK_ENTRY();

CAR_INTERFACE_IMPL(ContactEntry, Object, IContactEntry)

ContactEntry::ContactEntry()
    : mPinned(IPinnedPositions::UNPINNED)
    , mIsFavorite(FALSE)
    , mIsDefaultNumber(FALSE)
{}

} // List
} // Common
} // Contacts
} // Apps
} // Elastos
