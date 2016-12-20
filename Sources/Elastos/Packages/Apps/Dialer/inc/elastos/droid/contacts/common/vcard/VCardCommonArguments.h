
#ifndef __ELASTOS_DROID_CONTACTS_COMMON_VCARD_VCARDCOMMONARGUMENTS_H__
#define __ELASTOS_DROID_CONTACTS_COMMON_VCARD_VCARDCOMMONARGUMENTS_H__

#include <elastos.h>

namespace Elastos {
namespace Droid {
namespace Contacts {
namespace Common {
namespace VCard {

class VCardCommonArguments
{
public:
    // Argument used to pass calling activities to the target activity or service.
    // The value should be a string class name (e.g. com.android.contacts.vcard.VCardCommonArgs)
    static const String ARG_CALLING_ACTIVITY;
};

} // VCard
} // Common
} // Contacts
} // Droid
} // Elastos

#endif //__ELASTOS_DROID_CONTACTS_COMMON_VCARD_VCARDCOMMONARGUMENTS_H__
