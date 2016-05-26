
#ifndef __ORG_APACHE_HARMONY_SECURITY_PROVIDER_CRYPTO_CRANDOMBITSSUPPLIERHELPER_H__
#define __ORG_APACHE_HARMONY_SECURITY_PROVIDER_CRYPTO_CRANDOMBITSSUPPLIERHELPER_H__

#include "_Org_Apache_Harmony_Security_Provider_Crypto_CRandomBitsSupplierHelper.h"
#include <elastos/core/Singleton.h>

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace Provider {
namespace Crypto {

CarClass(CRandomBitsSupplierHelper)
    , public Singleton
    , public IRandomBitsSupplierHelper
{
public:
    CAR_SINGLETON_DECL()

    CAR_INTERFACE_DECL()

    CARAPI IsServiceAvailable(
        /* [out] */ Boolean * pAvailable);

    CARAPI GetRandomBits(
        /* [in] */ Int32 numBytes,
        /* [out, callee] */ ArrayOf<Byte> ** ppRandomBits);

private:
    // TODO: Add your private member variables here.
};

}
}
}
}
}
}

#endif // __ORG_APACHE_HARMONY_SECURITY_PROVIDER_CRYPTO_CRANDOMBITSSUPPLIERHELPER_H__
