
#ifndef __ELASTOS_DROID_NET_DHCP_DHCPOFFERPACKET_H__
#define __ELASTOS_DROID_NET_DHCP_DHCPOFFERPACKET_H__

#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/net/dhcp/DhcpPacket.h"

namespace Elastos {
namespace Droid {
namespace Net {
namespace Dhcp {

/**
 * This class implements the DHCP-OFFER packet.
 */
class DhcpOfferPacket
    : public DhcpPacket
{
public:
    DhcpOfferPacket();

    /**
     * Generates a OFFER packet with the specified parameters.
     */
    CARAPI constructor(
        /* [in] */ Int32 transId,
        /* [in] */ Boolean broadcast,
        /* [in] */ IInetAddress* serverAddress,
        /* [in] */ IInetAddress* clientIp,
        /* [in] */ ArrayOf<Byte>* clientMac);

    CARAPI ToString(
        /* [out] */ String* result);

    /**
     * Fills in a packet with the specified OFFER attributes.
     */
    CARAPI BuildPacket(
        /* [in] */ Int32 encap,
        /* [in] */ Int16 destUdp,
        /* [in] */ Int16 srcUdp,
        /* [out] */ IByteBuffer** result);

    /**
     * Adds the optional parameters to the server-generated OFFER packet.
     */
    CARAPI FinishPacket(
        /* [in] */ IByteBuffer* buffer);

    /**
     * Notifies the state machine of the OFFER packet parameters.
     */
    CARAPI DoNextOp(
        /* [in] */ DhcpStateMachine* machine);

private:
    /**
     * The IP address of the server which sent this packet.
     */
    AutoPtr<IInetAddress> mSrcIp;
};

} // namespace Dhcp
} // namespace Net
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_NET_DHCP_DHCPOFFERPACKET_H__
