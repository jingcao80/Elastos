
#ifndef __ELASTOS_DROID_NET_CWEBADDRESS_H__
#define __ELASTOS_DROID_NET_CWEBADDRESS_H__

#include "_Elastos_Droid_Net_CWebAddress.h"
#include "elastos/droid/net/WebAddress.h"

namespace Elastos {
namespace Droid {
namespace Net {

/**
 * {@hide}
 *
 * Web Address Parser
 *
 * This is called WebAddress, rather than URL or URI, because it
 * attempts to parse the stuff that a user will actually type into a
 * browser address widget.
 *
 * Unlike java.net.uri, this parser will not choke on URIs missing
 * schemes.  It will only throw a ParseException if the input is
 * really hosed.
 *
 * If given an https scheme but no port, fills in port
 *
 */
CarClass(CWebAddress)
    , public WebAddress
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Net
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_NET_CWEBADDRESS_H__
