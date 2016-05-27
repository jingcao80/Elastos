#ifndef __ELASTOS_APPS_CONTACTS_COMMON_LIST_CONTACTENTRY_H__
#define __ELASTOS_APPS_CONTACTS_COMMON_LIST_CONTACTENTRY_H__

namespace Elastos{
namespace Apps{
namespace Contacts {
namespace Common {
namespace List {

class ContactEntry
    : public Object
    , public IContactEntry
{
public:
    CAR_INTERFACE_DECL()

    ContactEntry();

public:
    String mName;
    String mStatus;
    String mPhoneLabel;
    String mPhoneNumber;
    AutoPtr<IUri> mPhotoUri;
    AutoPtr<IUri> mLookupUri;
    String mLookupKey;
    AutoPtr<IDrawable> mPresenceIcon;
    Int64 mId;
    Int32 mPinned; // = PinnedPositions.UNPINNED;
    Boolean mIsFavorite; // = false;
    Boolean mIsDefaultNumber; // = false;

    static const AutoPtr<IContactEntry> BLANK_ENTRY; // = new ContactEntry();
};

} // List
} // Common
} // Contacts
} // Apps
} // Elastos

#endif //__ELASTOS_APPS_CONTACTS_COMMON_LIST_CONTACTENTRY_H__