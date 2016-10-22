
#ifndef __ELASTOS_DROID_CONTACTS_COMMON_LIST_CONTACTENTRY_H__
#define __ELASTOS_DROID_CONTACTS_COMMON_LIST_CONTACTENTRY_H__

#include "_Elastos.Droid.Dialer.h"
#include "Elastos.Droid.Net.h"
#include "Elastos.Droid.Graphics.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Graphics::Drawable::IDrawable;
using Elastos::Droid::Net::IUri;
using Elastos::Core::Object;

namespace Elastos {
namespace Droid {
namespace Contacts {
namespace Common {
namespace List {

/**
 * Class to hold contact information
 */
class ContactEntry : public Object
{
public:
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
    Int32 mPinned;
    Boolean mIsFavorite;
    Boolean mIsDefaultNumber;

    static const AutoPtr<ContactEntry> BLANK_ENTRY;
};

} // List
} // Common
} // Contacts
} // Droid
} // Elastos

#endif //__ELASTOS_DROID_CONTACTS_COMMON_LIST_CONTACTENTRY_H__
