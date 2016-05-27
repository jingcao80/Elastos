
#ifndef __ORG_APACHE_HARMONY_XNET_PROVIDER_JSSE_CCIPHERSUITE_H__
#define __ORG_APACHE_HARMONY_XNET_PROVIDER_JSSE_CCIPHERSUITE_H__

#include "_Org_Apache_Harmony_Xnet_Provider_Jsse_CCipherSuite.h"
#include <elastos/core/Object.h>

using Elastos::Core::Object;

namespace Org {
namespace Apache {
namespace Harmony {
namespace Xnet {
namespace Provider {
namespace Jsse {

CarClass(CCipherSuite)
    , public Object
    , public ICipherSuite
{
public:
    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    CARAPI IsAnonymous(
        /* [out] */ Boolean * pAnnoymous);

    CARAPI GetName(
        /* [out] */ String * pName);

    CARAPI ToBytes(
        /* [out, callee] */ ArrayOf<Byte> ** ppBytes);

    CARAPI ToString(
        /* [out] */ String * pStr);

    CARAPI GetBulkEncryptionAlgorithm(
        /* [out] */ String * pAlgorithm);

    CARAPI GetBlockSize(
        /* [out] */ Int32 * pSize);

    CARAPI GetHmacName(
        /* [out] */ String * pName);

    CARAPI GetHashName(
        /* [out] */ String * pName);

    CARAPI GetMACLength(
        /* [out] */ Int32 * pLength);

    CARAPI IsExportable(
        /* [out] */ Boolean * pIsExportable);

    CARAPI GetServerKeyType(
        /* [out] */ String * pType);

    CARAPI GetAuthType(
        /* [in] */ Boolean emphemeral,
        /* [out] */ String * pType);

private:
    // TODO: Add your private member variables here.
};

}
}
}
}
}
}

#endif // __ORG_APACHE_HARMONY_XNET_PROVIDER_JSSE_CCIPHERSUITE_H__
