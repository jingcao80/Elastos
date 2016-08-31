
#ifndef __ELASTOS_DROID_CONTACTS_COMMON_UTIL_CEMPTYSERVICE_H__
#define __ELASTOS_DROID_CONTACTS_COMMON_UTIL_CEMPTYSERVICE_H__

#include "_Elastos_Droid_Contacts_Common_Util_CEmptyService.h"
#include <elastos/droid/app/Service.h>

using Elastos::Droid::App::Service;

namespace Elastos {
namespace Droid {
namespace Contacts {
namespace Common {
namespace Util {

CarClass(CEmptyService)
    , public Elastos::Droid::App::Service
{
public:
    CAR_OBJECT_DECL()

    // @Override
    CARAPI OnBind(
        /* [in] */ IIntent* intent,
        /* [out] */ IBinder** binder);
};

} // Util
} // Common
} // Contacts
} // Droid
} // Elastos

#endif //__ELASTOS_DROID_CONTACTS_COMMON_UTIL_CEMPTYSERVICE_H__
