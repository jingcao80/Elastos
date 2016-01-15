
#ifndef __ELASTOS_DROID_NET_HTTP_CCERTIFICATECHAINVALIDATOR_H__
#define __ELASTOS_DROID_NET_HTTP_CCERTIFICATECHAINVALIDATOR_H__

#include "_Elastos_Droid_Net_Http_CCertificateChainValidator.h"
#include "elastos/droid/net/http/CertificateChainValidator.h"

namespace Elastos {
namespace Droid {
namespace Net {
namespace Http {

/**
 * Class responsible for all server certificate validation functionality
 *
 * {@hide}
 */
CarClass(CCertificateChainValidator)
    , public CertificateChainValidator
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Http
} // namespace Net
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_NET_HTTP_CCERTIFICATECHAINVALIDATOR_H__
