#ifndef __ELASTOS_APPS_CONTACTS_COMMON_LIST_CPHONENUMBERLISTADAPTERPHONEQUERYHELPER_H__
#define __ELASTOS_APPS_CONTACTS_COMMON_LIST_CPHONENUMBERLISTADAPTERPHONEQUERYHELPER_H__

#include "_Elastos_Apps_Contacts_Common_List_CPhoneNumberListAdapterPhoneQueryHelper.h"
#include "ContactEntryListAdapter.h"

namespace Elastos{
namespace Apps{
namespace Contacts {
namespace Common {
namespace List {

CarClass(CPhoneNumberListAdapterPhoneQueryHelper)
    , public Singleton
    , public IPhoneNumberListAdapterPhoneQueryHelper
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    CARAPI GetPROJECTION_PRIMARY(
        /* [out, callee] */ ArrayOf<String>** result);

    CARAPI GetPROJECTION_ALTERNATIVE(
        /* [out, callee] */ ArrayOf<String>** result);
};

} // List
} // Common
} // Contacts
} // Apps
} // Elastos

#endif //__ELASTOS_APPS_CONTACTS_COMMON_LIST_CPHONENUMBERLISTADAPTERPHONEQUERYHELPER_H__
