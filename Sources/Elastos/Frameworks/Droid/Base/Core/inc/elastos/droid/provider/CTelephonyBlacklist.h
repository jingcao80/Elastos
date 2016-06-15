#ifndef __ELASTOS_DROID_PROVIDER_CTELEPHONYBLACKLIST_H__
#define __ELASTOS_DROID_PROVIDER_CTELEPHONYBLACKLIST_H__

#include "_Elastos_Droid_Provider_CTelephonyBlacklist.h"
#include "elastos/core/Singleton.h"

using Elastos::Droid::Net::IUri;

namespace Elastos {
namespace Droid {
namespace Provider {

CarClass(CTelephonyBlacklist)
    , public Singleton
    , public ITelephonyBlacklist
    , public IBaseColumns
{
public:
    CAR_INTERFACE_DECL();
    CAR_SINGLETON_DECL();

    CARAPI GetCONTENT_URI(
        /* [out] */ IUri** uri);

    CARAPI GetCONTENT_FILTER_BYNUMBER_URI(
        /* [out] */ IUri** result);

    CARAPI GetCONTENT_PHONE_URI(
        /* [out] */ IUri** result);

    CARAPI GetCONTENT_MESSAGE_URI(
        /* [out] */ IUri** result);
};

} // namespace Provider
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_PROVIDER_CTELEPHONYBLACKLIST_H__
