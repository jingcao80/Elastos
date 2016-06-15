#ifndef __ELASTOS_DROID_PROVIDER_CTELEPHONYSMSCONVERSATIONS_H__
#define __ELASTOS_DROID_PROVIDER_CTELEPHONYSMSCONVERSATIONS_H__

#include "_Elastos_Droid_Provider_CTelephonySmsConversations.h"
#include "elastos/core/Singleton.h"

using Elastos::Droid::Net::IUri;

namespace Elastos {
namespace Droid {
namespace Provider {

CarClass(CTelephonySmsConversations)
    , public Singleton
    , public ITelephonySmsConversations
    , public IBaseColumns
    , public ITelephonyTextBasedSmsColumns
{
public:
    CAR_INTERFACE_DECL();
    CAR_SINGLETON_DECL();

    CARAPI GetCONTENT_URI(
        /* [out] */ IUri** uri);

};

} // namespace Provider
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_PROVIDER_CTELEPHONYSMSCONVERSATIONS_H__
