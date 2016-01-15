
#ifndef __ELASTOS_DROID_NET_NSD_CDNSSDTXTRECORD_H__
#define __ELASTOS_DROID_NET_NSD_CDNSSDTXTRECORD_H__

#include "_Elastos_Droid_Net_Nsd_CDnsSdTxtRecord.h"
#include "elastos/droid/net/nsd/DnsSdTxtRecord.h"

namespace Elastos {
namespace Droid {
namespace Net {
namespace Nsd {

/**
 * This class handles TXT record data for DNS based service discovery as specified at
 * http://tools.ietf.org/html/draft-cheshire-dnsext-dns-sd-11
 *
 * DNS-SD specifies that a TXT record corresponding to an SRV record consist of
 * a packed array of bytes, each preceded by a length byte. Each string
 * is an attribute-value pair.
 *
 * The DnsSdTxtRecord object stores the entire TXT data as a single byte array, traversing it
 * as need be to implement its various methods.
 * @hide
 *
 */
CarClass(CDnsSdTxtRecord)
    , public DnsSdTxtRecord
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Nsd
} // namespace Net
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_NET_NSD_CDNSSDTXTRECORD_H__
