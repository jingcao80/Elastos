
#ifndef __ELASTOS_DROID_NET_DHCP_DHCPREQUESTPACKET_H__
#define __ELASTOS_DROID_NET_DHCP_DHCPREQUESTPACKET_H__

#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/net/dhcp/DhcpPacket.h"

namespace Elastos {
namespace Droid {
namespace Net {
namespace Dhcp {

/**
 * This class implements the DHCP-REQUEST packet.
 */
class DhcpRequestPacket
    : public DhcpPacket
{
public:
    /**
     * Generates a REQUEST packet with the specified parameters.
     */
    CARAPI constructor(
        /* [in] */ Int32 transId,
        /* [in] */ IInetAddress* clientIp,
        /* [in] */ ArrayOf<Byte>* clientMac,
        /* [in] */ Boolean broadcast);

    CARAPI ToString(
        /* [out] */ String* result);

    /**
     * Fills in a packet with the requested REQUEST attributes.
     */
    CARAPI BuildPacket(
        /* [in] */ Int32 encap,
        /* [in] */ Int16 destUdp,
        /* [in] */ Int16 srcUdp,
        /* [out] */ IByteBuffer** result);

    /**
     * Adds the optional parameters to the client-generated REQUEST packet.
     */
    CARAPI FinishPacket(
        /* [in] */ IByteBuffer* buffer);

    /**
     * Notifies the specified state machine of the REQUEST packet parameters.
     */
    CARAPI DoNextOp(
        /* [in] */ DhcpStateMachine* machine);
};

} // namespace Dhcp
} // namespace Net
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_NET_DHCP_DHCPREQUESTPACKET_H__
