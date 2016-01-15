
#ifndef __ELASTOS_DROID_NET_DHCP_DHCPDISCOVERPACKET_H__
#define __ELASTOS_DROID_NET_DHCP_DHCPDISCOVERPACKET_H__

#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/net/dhcp/DhcpPacket.h"

namespace Elastos {
namespace Droid {
namespace Net {
namespace Dhcp {

/**
 * This class implements the DHCP-DISCOVER packet.
 */
class DhcpDiscoverPacket
    : public DhcpPacket
{
public:
    DhcpDiscoverPacket();

    /**
     * Generates a DISCOVER packet with the specified parameters.
     */
    CARAPI constructor(
        /* [in] */ Int32 transId,
        /* [in] */ ArrayOf<Byte>* clientMac,
        /* [in] */ Boolean broadcast);

    CARAPI ToString(
        /* [out] */ String* result);

    /**
     * Fills in a packet with the requested DISCOVER parameters.
     */
    CARAPI BuildPacket(
        /* [in] */ Int32 encap,
        /* [in] */ Int16 destUdp,
        /* [in] */ Int16 srcUdp,
        /* [out] */ IByteBuffer** result);

    /**
     * Adds optional parameters to a DISCOVER packet.
     */
    CARAPI FinishPacket(
        /* [in] */ IByteBuffer* buffer);

    /**
     * Informs the state machine of the arrival of a DISCOVER packet.
     */
    CARAPI DoNextOp(
        /* [in] */ DhcpStateMachine* machine);
};

} // namespace Dhcp
} // namespace Net
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_NET_DHCP_DHCPDISCOVERPACKET_H__
