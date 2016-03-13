
#ifndef __ELASTOS_DROID_TELEPHONY_CDISCONNECTCAUSE_H__
#define __ELASTOS_DROID_TELEPHONY_CDISCONNECTCAUSE_H__

#include "_Elastos_Droid_Telephony_CDisconnectCause.h"
#include <elastos/core/Singleton.h>

namespace Elastos {
namespace Droid {
namespace Telephony {

CarClass(CDisconnectCause)
    , public Singleton
    , public IDisconnectCause
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    /** Returns descriptive string for the specified disconnect cause. */
    CARAPI ToString(
        /* [in] */ Int32 cause,
        /* [out] */ String* str);
};

} //namespace Elastos
} //namespace Droid
} //namespace Telephony

#endif // __ELASTOS_DROID_TELEPHONY_CDISCONNECTCAUSE_H__
