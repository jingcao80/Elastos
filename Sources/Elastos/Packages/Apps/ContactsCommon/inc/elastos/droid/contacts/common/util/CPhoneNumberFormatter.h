#ifndef __ELASTOS_APPS_CONTACTS_COMMON_UTIL_CPHONENUMBERFORMATTER_H__
#define __ELASTOS_APPS_CONTACTS_COMMON_UTIL_CPHONENUMBERFORMATTER_H__

#include "_Elastos_Apps_Contacts_Common_Util_CPhoneNumberFormatter.h"
#include <elastos/core/Singleton.h>

namespace Elastos{
namespace Apps{
namespace Contacts {
namespace Common {
namespace Util {

CarClass(CPhoneNumberFormatter)
    , public singleton
    , public IPhoneNumberFormatter
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    CARAPI SetPhoneNumberFormattingTextWatcher(
        /* [in] */ IContext* context,
        /* [in] */ ITextView* textView);
};

} // Util
} // Common
} // Contacts
} // Apps
} // Elastos

#endif //__ELASTOS_APPS_CONTACTS_COMMON_UTIL_CPHONENUMBERFORMATTER_H__
